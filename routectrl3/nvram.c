/*
 * nvram.c
 *
 * Helper functions for interfacing to non-volatile RAM.
 * Compatible with
 *   EERAM: 47C04, 47C16, 47L04, 47L16.
 *   FRAM:  MB85RC04, MB85RC16, MB85RC64, MB85RC128, MB85RC256.
 *
 * Created: 13-12-2025 13:49:51
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nvram.h"
#include "ticks.h"
#include "twim.h"


// Address used by both EERAM and FRAM
#define NVRAM_SRAM_ADR 0xa0

// EERAM specific address
#define EERAM_CTRL_ADR 0x30

#define EERAM_CTRL_STATUS  0x00
#define EERAM_CTRL_COMMAND 0x55

// FRAM specific address
#define FRAM_DEVID_ADR 0xf8

#define TWI_TIMEOUT    TICKS_FROM_MS(500)

typedef struct
{
    uint8_t         event:1;    // Event detect
    uint8_t         ase:1;      // Auto-store enable
    uint8_t         bp:3;       // Block protect
    uint8_t         reserved:2;
    uint8_t         am:1;       // Array modified
} eeram_reg_status_t;

typedef union
{
    uint8_t         data;
    eeram_reg_status_t bit;
} eeram_buf_status_t;

#define WRITESTACK_SIZE   8

typedef struct
{
    uint16_t        adr;
    uint8_t         data;
} writestack_t;

typedef enum
{
    NVRAM_STATE_INIT,
    NVRAM_STATE_EERAM_SET_ASE,
    NVRAM_STATE_IDLE,
    NVRAM_STATE_BUSY,
    NVRAM_STATE_ERROR
} nvram_state_t;

static nvram_state_t state = NVRAM_STATE_INIT;
static uint8_t  buffer[3];
static ticks_t  tstart;
static writestack_t writestack[WRITESTACK_SIZE];
static uint8_t  writestack_cnt = 0;



static void print_twi_error(twim_status_t ts)
{
    switch (ts)
    {
    case TWIM_STATUS_DONE:
        break;

    case TWIM_STATUS_NODEVICE:
        printf_P(PSTR("NVRAM device not found\n"));
        break;

    case TWIM_STATUS_ERROR:
        printf_P(PSTR("NVRAM device error\n"));
        break;

    default:
        printf_P(PSTR("TWI unknown status: %d\n"), ts);
        break;
    }
}

static void eeram_status_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        eeram_buf_status_t status;

        status.data = buffer[0];
        if (status.bit.ase)
        {
            // Auto-store is already enabled. Nothing to do
            printf_P(PSTR("EERAM ready\n"));
            state = NVRAM_STATE_IDLE;
        }
        else
        {
            // Need to enable auto-store for EERAM to work properly
            state = NVRAM_STATE_EERAM_SET_ASE;
        }
    }
    else if (ts == TWIM_STATUS_NODEVICE)
    {
        printf_P(PSTR("EERAM not detected. Assuming FRAM\n"));
        state = NVRAM_STATE_IDLE;
    }
    else
    {
        print_twi_error(ts);
        state = NVRAM_STATE_ERROR;
    }
}


static void eeram_set_ase_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        printf_P(PSTR("EERAM configured and ready\n"));
        state = NVRAM_STATE_IDLE;
    }
    else
    {
        print_twi_error(ts);
        state = NVRAM_STATE_ERROR;
    }
}


static void twi_done_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        state = NVRAM_STATE_IDLE;
    }
    else
    {
        print_twi_error(ts);
        state = NVRAM_STATE_ERROR;
    }
}


void nvram_init(void)
{
}


void nvram_update(void)
{
    switch (state)
    {
    case NVRAM_STATE_INIT:
        // Try EERAM CTRL address
        if (twim_read(EERAM_CTRL_ADR, buffer, sizeof(eeram_buf_status_t), eeram_status_cb))
        {
            tstart = ticks_get();
            state = NVRAM_STATE_BUSY;
        }
        break;

    case NVRAM_STATE_EERAM_SET_ASE:
        {
            eeram_buf_status_t status;

            status.data = buffer[0];
            status.bit.ase = 1;
            buffer[0] = EERAM_CTRL_STATUS;
            buffer[1] = status.data;
            if (twim_write(EERAM_CTRL_ADR, buffer, 2, eeram_set_ase_cb))
            {
                tstart = ticks_get();
                state = NVRAM_STATE_BUSY;
            }
            break;
        }

    case NVRAM_STATE_IDLE:
        if (writestack_cnt > 0 && twim_ready())
        {
            // Write new byte to NVRAM
            uint8_t         i = writestack_cnt - 1;
            bool            success;

#if NVRAM_BYTEADR
            // High byte of address is embedded in chip address
            buffer[0] = writestack[i].adr & 0xff;
            buffer[1] = writestack[i].data;
            success = twim_write(NVRAM_SRAM_ADR + ((writestack[i].adr >> 7) & 0x0e), buffer, 2, twi_done_cb);
#else
            buffer[0] = writestack[i].adr >> 8; // High byte first
            buffer[1] = writestack[i].adr & 0xff;
            buffer[2] = writestack[i].data;
            success = twim_write(NVRAM_SRAM_ADR, buffer, 3, twi_done_cb);
#endif
            if (success)
            {
                writestack_cnt = i;
                tstart = ticks_get();
                state = NVRAM_STATE_BUSY;
            }
        }
        break;

    case NVRAM_STATE_BUSY:
        if (ticks_elapsed(tstart) >= TWI_TIMEOUT)
        {
            printf_P(PSTR("TWI timeout\n"));
            state = NVRAM_STATE_ERROR;
        }
        break;

    case NVRAM_STATE_ERROR:
    default:
        break;
    }
}


bool nvram_ready(void)
{
    return state == NVRAM_STATE_IDLE;
}


bool nvram_read(uint16_t adr, uint8_t *buf, uint16_t len)
{
    bool            success;

    if (!nvram_ready() || !twim_ready())
        return false;

#if NVRAM_BYTEADR
    // High byte of address is embedded in chip address
    buffer[0] = adr & 0xff;
    success = twim_write_read(NVRAM_SRAM_ADR + ((adr >> 7) & 0x0e), buffer, 1, buf, len, twi_done_cb);
#else
    buffer[0] = adr >> 8;       // High byte first
    buffer[1] = adr & 0xff;     // Low byte
    success = twim_write_read(NVRAM_SRAM_ADR, buffer, 2, buf, len, twi_done_cb);
#endif
    if (success)
    {
        tstart = ticks_get();
        state = NVRAM_STATE_BUSY;
        return true;
    }

    return false;
}


void nvram_write(uint16_t adr, uint8_t data)
{
    if (writestack_cnt > 0)
    {
        // See if address is already queued up in the stack, and reuse it
        for (uint8_t i = 0; i < writestack_cnt; i++)
        {
            if (writestack[i].adr == adr)
            {
                writestack[i].data = data;
                return;
            }
        }
    }
    // Create new entry
    if (writestack_cnt < WRITESTACK_SIZE)
    {
        writestack[writestack_cnt].adr = adr;
        writestack[writestack_cnt].data = data;
        writestack_cnt++;
    }
}
