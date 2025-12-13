/*
 * eeram.c
 *
 * Helper functions for interfacing to EERAM.
 * Compatible with: 47C04, 47C16, 47L04, 47L16.
 *
 * Created: 13-12-2025 13:49:51
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "eeram.h"
#include "ticks.h"
#include "twim.h"


#define EERAM_CTRL_ADR 0x30
#define EERAM_SRAM_ADR 0xa0

#define CTRL_STATUS    0x00
#define CTRL_COMMAND   0x55

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
    EERAM_STATE_INIT,
    EERAM_STATE_SET_ASE,
    EERAM_STATE_IDLE,
    EERAM_STATE_BUSY,
    EERAM_STATE_ERROR
} eeram_state_t;

static eeram_state_t state = EERAM_STATE_INIT;
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
        printf_P(PSTR("EERAM device not found\n"));
        break;

    case TWIM_STATUS_ERROR:
        printf_P(PSTR("EERAM device error\n"));
        break;

    default:
        printf_P(PSTR("TWI unknown status: %d\n"), ts);
        break;
    }
}

static void status_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        eeram_buf_status_t status;

        status.data = buffer[0];
        if (status.bit.ase)
        {
            // Auto-store is already enabled. Nothing to do
            printf_P(PSTR("EERAM ready\n"));
            state = EERAM_STATE_IDLE;
        }
        else
        {
            // Need to enable auto-store for EERAM to work properly
            state = EERAM_STATE_SET_ASE;
        }
    }
    else
    {
        print_twi_error(ts);
        state = EERAM_STATE_ERROR;
    }
}


static void set_ase_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        printf_P(PSTR("EERAM configured and ready\n"));
        state = EERAM_STATE_IDLE;
    }
    else
    {
        print_twi_error(ts);
        state = EERAM_STATE_ERROR;
    }
}


static void twi_done_cb(twim_status_t ts)
{
    if (ts == TWIM_STATUS_DONE)
    {
        state = EERAM_STATE_IDLE;
    }
    else
    {
        print_twi_error(ts);
        state = EERAM_STATE_ERROR;
    }
}


void eeram_init(void)
{
}


void eeram_update(void)
{
    switch (state)
    {
    case EERAM_STATE_INIT:
        if (twim_read(EERAM_CTRL_ADR, buffer, sizeof(eeram_buf_status_t), status_cb))
        {
            tstart = ticks_get();
            state = EERAM_STATE_BUSY;
        }
        break;

    case EERAM_STATE_SET_ASE:
        {
            eeram_buf_status_t status;

            status.data = buffer[0];
            status.bit.ase = 1;
            buffer[0] = CTRL_STATUS;
            buffer[1] = status.data;
            if (twim_write(EERAM_CTRL_ADR, buffer, 2, set_ase_cb))
            {
                tstart = ticks_get();
                state = EERAM_STATE_BUSY;
            }
            break;
        }

    case EERAM_STATE_IDLE:
        if (writestack_cnt > 0 && twim_ready())
        {
            // Write new byte to EERAM
            buffer[0] = writestack[writestack_cnt].adr >> 8;    // High byte first
            buffer[1] = writestack[writestack_cnt].adr & 0xff;
            buffer[2] = writestack[writestack_cnt].data;
            if (twim_write(EERAM_SRAM_ADR, buffer, 3, twi_done_cb))
            {
                writestack_cnt--;
                tstart = ticks_get();
                state = EERAM_STATE_BUSY;
            }
        }
        break;

    case EERAM_STATE_BUSY:
        if (ticks_elapsed(tstart) >= TWI_TIMEOUT)
        {
            printf_P(PSTR("TWI timeout\n"));
            state = EERAM_STATE_ERROR;
        }
        break;

    case EERAM_STATE_ERROR:
    default:
        break;
    }
}


bool eeram_ready(void)
{
    return state == EERAM_STATE_IDLE;
}


bool eeram_read(uint16_t adr, uint8_t *buf, uint16_t len)
{
    if (!eeram_ready() || !twim_ready())
        return false;
    buffer[0] = adr >> 8;       // High byte first
    buffer[1] = adr & 0xff;     // Low byte
    if (twim_write_read(EERAM_SRAM_ADR, buffer, 2, buf, len, twi_done_cb))
    {
        tstart = ticks_get();
        state = EERAM_STATE_BUSY;
        return true;
    }

    return false;
}


void eeram_write(uint16_t adr, uint8_t data)
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
