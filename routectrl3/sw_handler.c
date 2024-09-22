/*
 * sw_handler.c
 *
 * Switch handler.
 * Receives OPC_SW_REQ from Loconet and calls switch subscribers.
 *
 * Created: 29-01-2024 16:31:54
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "flashmem.h"
#include "sw_handler.h"
#include "lib/avr-shell-cmd/cmd.h"
#include "lib/loconet-avrda/hal_ln.h"
#include "lib/loconet-avrda/ln_rx.h"

#ifndef SW_ADR_MAX
#define SW_ADR_MAX 2048
#endif

#define SW_ARRAY_SIZE ((SW_ADR_MAX + 7) / 8)

static uint8_t  sw_state[SW_ARRAY_SIZE];
static const __flash uint8_t adr_mask[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

extern const FLASHMEM switchreq_table_t __loconet_swreqtable_start;
extern const FLASHMEM switchreq_table_t __loconet_swreqtable_end;
extern const FLASHMEM swreqrange_table_t __loconet_swreqrangetable_start;
extern const FLASHMEM swreqrange_table_t __loconet_swreqrangetable_end;


void sw_handler_set_state(uint16_t adr, bool dir)
{
    uint16_t        idx;
    uint8_t         mask;

    if (adr == 0 || adr > SW_ADR_MAX)
        return;

    adr--;
    idx = adr / 8;
    mask = adr_mask[adr & 7];
    if (dir)                    // G
        sw_state[idx] |= mask;
    else                        // R
        sw_state[idx] &= ~mask;
}


bool sw_handler_get_state(uint16_t adr)
{
    uint16_t        idx;
    uint8_t         mask;

    if (adr == 0 || adr > SW_ADR_MAX)
        return false;

    adr--;
    idx = adr / 8;
    mask = adr_mask[adr & 7];
    return (sw_state[idx] & mask) != 0;
}


static void swsCmd(uint8_t argc, char *argv[])
{
    uint16_t        adr;

    if (argc < 2)
    {
        printf_P(PSTR("Usage: sws <adr>\n"));
        printf_P(PSTR(" <adr>   : Switch address\n"));
        return;
    }

    adr = strtoul(argv[1], NULL, 0);
    if (sw_handler_get_state(adr))
        printf_P(PSTR("G\n"));
    else
        printf_P(PSTR("R\n"));
}

CMD(sws, "Switch state");


static void swreq_callback(uint16_t adr, bool dir)
{
    const FLASHMEM switchreq_table_t *p, *pend;
    bool            found = false;

    p = &__loconet_swreqtable_start;
    pend = &__loconet_swreqtable_end;

    while (p < pend)
    {
        if (p->adr == adr)
        {
            p->cb(adr, dir);
            found = true;
        }
        else if (found)
        {
            break;
        }
        p++;
    }
}

static void swreq_range_callback(uint16_t adr, bool dir)
{
    const FLASHMEM swreqrange_table_t *p, *pend;

    p = &__loconet_swreqrangetable_start;
    pend = &__loconet_swreqrangetable_end;

    while (p < pend)
    {
        if (adr >= p->adr_start && adr <= p->adr_end)
            p->cb(adr, dir);
        p++;
    }
}

void ln_rx_opc_sw_req(uint16_t adr, uint8_t dir, uint8_t on)
{
    if (on != 0)
    {
        sw_handler_set_state(adr, dir != 0);
        swreq_callback(adr, dir != 0);
        swreq_range_callback(adr, dir != 0);
    }
}
