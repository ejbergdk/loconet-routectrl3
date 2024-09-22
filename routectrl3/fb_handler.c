/*
 * fb_handler.c
 *
 * Feedback handler.
 * Receives OPC_INPUT_REP from Loconet and calls feedback subscribers.
 *
 * Created: 02-01-2023 17:22:47
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fb_handler.h"
#include "flashmem.h"
#include "lib/avr-shell-cmd/cmd.h"
#include "lib/loconet-avrda/hal_ln.h"
#include "lib/loconet-avrda/ln_rx.h"

#ifndef FEEDBACK_ADR_MAX
#define FEEDBACK_ADR_MAX 4096
#endif

#define FB_ARRAY_SIZE ((FEEDBACK_ADR_MAX + 7) / 8)

static uint8_t  feedback_state[FB_ARRAY_SIZE];
static const __flash uint8_t adr_mask[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static uint16_t feedback_cnt = 0;

extern const FLASHMEM feedback_table_t __loconet_fbocctable_start;
extern const FLASHMEM feedback_table_t __loconet_fbocctable_end;
extern const FLASHMEM feedback_table_t __loconet_fbfreetable_start;
extern const FLASHMEM feedback_table_t __loconet_fbfreetable_end;
extern const FLASHMEM feedbackrange_table_t __loconet_fbrangeocctable_start;
extern const FLASHMEM feedbackrange_table_t __loconet_fbrangeocctable_end;
extern const FLASHMEM feedbackrange_table_t __loconet_fbrangefreetable_start;
extern const FLASHMEM feedbackrange_table_t __loconet_fbrangefreetable_end;


void fb_handler_set_state(uint16_t adr, bool l)
{
    uint16_t        idx;
    uint8_t         mask;

    if (adr == 0 || adr > FEEDBACK_ADR_MAX)
        return;

    adr--;
    idx = adr / 8;
    mask = adr_mask[adr & 7];
    if (l)                      // Occupied
        feedback_state[idx] |= mask;
    else                        // Free
        feedback_state[idx] &= ~mask;
}


bool fb_handler_get_state(uint16_t adr)
{
    uint16_t        idx;
    uint8_t         mask;

    if (adr == 0 || adr > FEEDBACK_ADR_MAX)
        return false;

    adr--;
    idx = adr / 8;
    mask = adr_mask[adr & 7];
    return (feedback_state[idx] & mask) != 0;
}


static void fbCmd(uint8_t argc, char *argv[])
{
    uint16_t        adr;

    if (argc < 2)
    {
        printf_P(PSTR("Usage: fb <adr>\n"));
        printf_P(PSTR(" <adr>   : Feedback address\n"));
        return;
    }

    adr = strtoul(argv[1], NULL, 0);
    if (fb_handler_get_state(adr))
        printf_P(PSTR("Occupied\n"));
    else
        printf_P(PSTR("Free\n"));
}

CMD(fb, "Feedback");


static void feedback_callback(uint16_t adr, uint8_t l)
{
    const FLASHMEM feedback_table_t *p, *pend;
    bool            found = false;

    if (l != 0)                 // Occupied
    {
        p = &__loconet_fbocctable_start;
        pend = &__loconet_fbocctable_end;
    }
    else                        // Free
    {
        p = &__loconet_fbfreetable_start;
        pend = &__loconet_fbfreetable_end;
    }

    while (p < pend)
    {
        if (p->adr == adr)
        {
            p->cb(adr);
            found = true;
        }
        else if (found)
        {
            break;
        }
        p++;
    }
}

static void feedback_range_callback(uint16_t adr, uint8_t l)
{
    const FLASHMEM feedbackrange_table_t *p, *pend;

    if (l != 0)                 // Occupied
    {
        p = &__loconet_fbrangeocctable_start;
        pend = &__loconet_fbrangeocctable_end;
    }
    else                        // Free
    {
        p = &__loconet_fbrangefreetable_start;
        pend = &__loconet_fbrangefreetable_end;
    }

    while (p < pend)
    {
        if (adr >= p->adr_start && adr <= p->adr_end)
            p->cb(adr);
        p++;
    }
}

void ln_rx_opc_input_rep(uint16_t adr, uint8_t l, uint8_t x)
{

    if (!x)
        return;

    feedback_cnt++;

    fb_handler_set_state(adr, l != 0);

    feedback_callback(adr, l);
    feedback_range_callback(adr, l);
}

uint16_t fb_handler_get_packets_received(void)
{
    return feedback_cnt;
}
