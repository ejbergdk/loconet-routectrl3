/*
 * ctrl_init.c
 *
 * Initializes controller.
 * Reads status from feedback modules (Uhl 63330).
 *
 * Created: 27-03-2024 13:09:22
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctrl_init.h"
#include "fb_handler.h"
#include "mmi.h"
#include "route.h"
#include "ticks.h"
#include "timer.h"

#define MODULE_QUERY_ADR_START  1017
#define MODULE_QUERY_ADR_END    1020

#define INIT_DELAY_TICKS    TICKS_FROM_MS(10000)
#define RETRY_TICKS         TICKS_FROM_MS(2000)

static bool     init_done = false;

typedef struct
{
    uint16_t        adr;
    uint8_t         retry;
    uint16_t        fb_cnt;
} init_vars_t;


static void check_answer(void *ctx)
{
    init_vars_t    *p = (init_vars_t *) ctx;
    uint16_t        cnt = fb_handler_get_packets_received() - p->fb_cnt;

    if (cnt >= 16)
    {
        // All expected feedbacks received. Query next module
        printf_P(PSTR("Feedback module %u ok. cnt=%u\n"), p->adr, cnt);
        p->adr++;
        p->retry = 0;
    }
    else
    {
        // Not enough feedback packets received. Retry
        printf_P(PSTR("Feedback module %u cnt=%u\n"), p->adr, cnt);
        p->retry++;
    }
    if (p->retry >= 3)
    {
        printf_P(PSTR("Giving up\n"));
        p->adr++;
        p->retry = 0;
    }

    if (p->adr > MODULE_QUERY_ADR_END)
    {
        printf_P(PSTR("Init done\n"));
        init_done = true;
        mmi_in_init(false);
        free(p);
        return;
    }

    p->fb_cnt = fb_handler_get_packets_received();
    route_send_sw_prio(p->adr, SW_R);
    timer_add(RETRY_TICKS, check_answer, p);
}


static void start_query(void *ctx)
{
    init_vars_t    *p = (init_vars_t *) ctx;

    p->adr = MODULE_QUERY_ADR_START;
    p->retry = 0;
    p->fb_cnt = fb_handler_get_packets_received();
    route_send_sw_prio(p->adr, SW_R);
    timer_add(RETRY_TICKS, check_answer, p);
}


void ctrl_init_init(void)
{
    init_vars_t    *p = malloc(sizeof(*p));

    if (!p)
    {
        printf_P(PSTR("Init failed. Out of memory.\n"));
        return;
    }

    timer_add(INIT_DELAY_TICKS, start_query, p);
    mmi_in_init(true);
}

bool ctrl_init_done(void)
{
    return init_done;
}
