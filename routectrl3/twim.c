/*
 * twim.c
 *
 * TWI master driver for AVR DA.
 * Interrupt driven.
 * Not tested in a multi-master setup.
 *
 * Created: 21-09-2025 12:58:20
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "twim.h"


#define TWI_BAUD 115            // 115 = 100 kHz. 27 = 400 kHz. Only valid if F_CPU is 24 MHz

typedef enum
{
    TWIM_STATE_IDLE,
    TWIM_STATE_WADR,
    TWIM_STATE_WDATA,
    TWIM_STATE_RDATA
} twim_state_t;


static twim_cb *callback = NULL;
static const uint8_t *pwbuf;
static uint8_t *prbuf;
static uint16_t wcount, rcount;
static uint8_t  radr;
static twim_status_t twi_status;
static twim_state_t twi_state = TWIM_STATE_IDLE;

void twim_init(void)
{
    // Init port pins (Errata 2.12.1: Clear port pins before enabling twi)
    PORTA.OUTCLR = PIN2_bm | PIN3_bm;
    PORTA.DIRSET = PIN2_bm | PIN3_bm;
    TWI0.CTRLA = 0;             // I2C mode, no fast mode plus
    TWI0.DUALCTRL = 0;
    TWI0.MBAUD = TWI_BAUD;
    TWI0.MCTRLA = TWI_RIEN_bm | TWI_WIEN_bm | TWI_ENABLE_bm;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}


void twim_update(void)
{
    if (callback && twi_state == TWIM_STATE_IDLE)
    {
        callback(twi_status);
        callback = NULL;
    }
}


bool twim_ready(void)
{
    return callback == NULL && (TWI0.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_IDLE_gc;
}


bool twim_write(uint8_t adr, const uint8_t *buf, uint16_t len, twim_cb *cb)
{
    if (!twim_ready())
        return false;
    callback = cb;
    pwbuf = buf;
    wcount = len;
    rcount = 0;
    twi_state = TWIM_STATE_WADR;
    TWI0.MADDR = adr & 0xfe;
    return true;
}


bool twim_read(uint8_t adr, uint8_t *buf, uint16_t len, twim_cb *cb)
{
    if (!twim_ready())
        return false;
    callback = cb;
    prbuf = buf;
    wcount = 0;
    rcount = len;
    twi_state = TWIM_STATE_WADR;
    TWI0.MADDR = adr | 0x01;
    return true;
}


bool twim_write_read(uint8_t adr, const uint8_t *wbuf, uint16_t wlen, uint8_t *rbuf, uint16_t rlen, twim_cb *cb)
{
    if (!twim_ready())
        return false;
    callback = cb;
    pwbuf = wbuf;
    prbuf = rbuf;
    wcount = wlen;
    rcount = rlen;
    radr = adr | 0x01;
    twi_state = TWIM_STATE_WADR;
    TWI0.MADDR = adr & 0xfe;
    return true;
}


ISR(TWI0_TWIM_vect)
{
    uint8_t         status = TWI0.MSTATUS;

    if (status & TWI_WIF_bm)
    {
        if ((status & TWI_RXACK_bm) && (twi_state == TWIM_STATE_WADR))
        {
            // No device responded on the given address
            twi_status = TWIM_STATUS_NODEVICE;
            twi_state = TWIM_STATE_IDLE;
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            return;
        }
        if (status & (TWI_ARBLOST_bm | TWI_BUSERR_bm))
        {
            // Arbitration lost or buserror
            twi_status = TWIM_STATUS_ERROR;
            twi_state = TWIM_STATE_IDLE;
            return;
        }
        // Write successful
        twi_state = TWIM_STATE_WDATA;
        if (wcount)
        {
            // More data to send
            if (!(status & TWI_RXACK_bm))
            {
                TWI0.MDATA = *pwbuf++;
                wcount--;
            }
            else
            {
                // Device sent NACK
                twi_status = TWIM_STATUS_ERROR;
                twi_state = TWIM_STATE_IDLE;
                TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            }
            return;
        }
        // No more data to send. Should we receive data?
        if (rcount)
        {
            // Send repeated start to initiate reception
            twi_state = TWIM_STATE_WADR;
            TWI0.MADDR = radr;
        }
        else
        {
            // Tx done
            twi_status = TWIM_STATUS_DONE;
            twi_state = TWIM_STATE_IDLE;
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        }
        return;
    }
    if (status & TWI_RIF_bm)
    {
        twi_state = TWIM_STATE_RDATA;
        *prbuf++ = TWI0.MDATA;
        if (--rcount)
        {
            // More data to receive
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
        }
        else
        {
            // Last byte received
            twi_status = TWIM_STATUS_DONE;
            twi_state = TWIM_STATE_IDLE;
            TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
        }
    }
}


#if __has_include("avr-shell-cmd/cmd.h")

#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include "avr-shell-cmd/cmd.h"

static uint8_t  debug_buf[20];
static uint8_t  debug_len = 0;

static void twi_debug_cb(twim_status_t status)
{
    switch (status)
    {
    case TWIM_STATUS_DONE:
        printf_P(PSTR("\nTWIM_STATUS_DONE\n"));
        if (debug_len)
        {
            for (uint8_t i = 0; i < debug_len; i++)
                printf_P(PSTR(" %02x"), debug_buf[i]);
            putchar('\n');
        }
        break;
    case TWIM_STATUS_NODEVICE:
        printf_P(PSTR("\nTWIM_STATUS_NODEVICE\n"));
        break;
    case TWIM_STATUS_ERROR:
        printf_P(PSTR("\nTWIM_STATUS_ERROR\n"));
        break;
    default:
        printf_P(PSTR("\nTWIM: Unknown status %u\n"), status);
        break;
    }
    debug_len = 0;
}


static void twiCmd(uint8_t argc, char *argv[])
{
    if (argc < 4)
    {
        printf_P(PSTR("Missing argument\nArguments:\n"));
        printf_P(PSTR(" w <adr> <data1> [<datan>]         - Write packet\n"));
        printf_P(PSTR(" r <adr> <len>                     - Read packet\n"));
        printf_P(PSTR(" x <adr> <rxlen> <data1> [<datan>] - Write and read packet\n"));
        return;
    }

    uint8_t         adr = strtoul(argv[2], NULL, 0);

    switch (argv[1][0])
    {
    case 'w':
        {
            uint8_t         len = argc - 3;

            for (uint8_t i = 0; i < len; i++)
                debug_buf[i] = strtoul(argv[i + 3], NULL, 0);
            debug_len = 0;
            if (!twim_write(adr, debug_buf, len, twi_debug_cb))
                printf_P(PSTR("ERROR\n"));
            break;
        }

    case 'r':
        {
            debug_len = strtoul(argv[3], NULL, 0);
            if (!twim_read(adr, debug_buf, debug_len, twi_debug_cb))
            {
                debug_len = 0;
                printf_P(PSTR("ERROR\n"));
            }
            break;
        }

    case 'x':
        {
            uint8_t         len = argc - 4;

            if (len == 0)
            {
                printf_P(PSTR("No write data\n"));
                break;
            }
            for (uint8_t i = 0; i < len; i++)
                debug_buf[i] = strtoul(argv[i + 4], NULL, 0);
            debug_len = strtoul(argv[3], NULL, 0);
            if (!twim_write_read(adr, debug_buf, len, debug_buf, debug_len, twi_debug_cb))
                printf_P(PSTR("ERROR\n"));
            break;
        }

    default:
        printf_P(PSTR("Unknown argument\n"));
        break;
    }
}

CMD(twi, "TWI test");

#endif
