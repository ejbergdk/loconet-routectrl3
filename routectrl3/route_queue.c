/*
 * route_queue.c
 *
 * Created: 15-01-2024 16:46:24
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "fb_handler.h"
#include "route_queue.h"
#include "switch_queue.h"
#include "ticks.h"
#include "lib/loconet-avrda/ln_tx.h"

#define CMD_DELAY_TIME      TICKS_FROM_MS(50)
#define QUEUE_SIZE          128

typedef struct
{
    uint16_t        adr:12;
    uint16_t        opt:1;
    uint16_t        cmd:3;
} rq_cmd_t;

static rq_cmd_t queue[QUEUE_SIZE];
static uint8_t  queue_ridx = 0;
static uint8_t  queue_widx = 0;

static ticks_t  last_activity = 0;


void route_queue_add(uint16_t adr, bool opt, route_queue_cmd_t cmd)
{
    // NOTE: There is no buffer full check
    queue[queue_widx].adr = adr;
    queue[queue_widx].opt = opt ? 1 : 0;
    queue[queue_widx].cmd = cmd;
    queue_widx++;
    if (queue_widx >= QUEUE_SIZE)
        queue_widx = 0;
}

void route_queue_update(void)
{
    if (ticks_elapsed(last_activity) < CMD_DELAY_TIME || queue_ridx == queue_widx)
        return;

    if (!switch_queue_empty())
        return;

    switch (queue[queue_ridx].cmd)
    {
    case RQ_CMD_SW:
#ifdef ROUTE_DEBUG
        printf_P(PSTR("Send SW %u %c\n"), queue[queue_ridx].adr, queue[queue_ridx].opt ? 'G' : 'R');
#endif

        switch_queue_add(queue[queue_ridx].adr, queue[queue_ridx].opt != 0);
        break;

    case RQ_CMD_FB:
        if (ln_tx_opc_input_rep(queue[queue_ridx].adr, queue[queue_ridx].opt, NULL, NULL) != 0)
            return;             // input_rep failed. Do not advance queue_ridx

#ifndef LNECHO
        // Update fb state (only needed if not receiving own LN echo).
        fb_handler_set_state(queue[queue_ridx].adr, queue[queue_ridx].opt != 0);
#endif

#ifdef ROUTE_DEBUG
        printf_P(PSTR("Send FB %u %S\n"), queue[queue_ridx].adr,
                 queue[queue_ridx].opt ? PSTR("OCCUPIED") : PSTR("FREE"));
#endif
        break;

    default:
        break;
    }

    last_activity = ticks_get();
    queue_ridx++;
    if (queue_ridx >= QUEUE_SIZE)
        queue_ridx = 0;
}
