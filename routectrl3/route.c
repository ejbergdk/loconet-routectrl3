/*
 * route.c
 *
 * Created: 13-01-2024 16:22:08
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "fb_handler.h"
#include "route.h"
#include "route_queue.h"
#include "switch_queue.h"
#include "lib/loconet-avrda/ln_tx.h"


extern const __flash route_table_t __loconet_routetable_start;
extern const __flash route_table_t __loconet_routetable_end;

typedef struct
{
    route_state_t   state;
} routeparm_t;

static routeparm_t parm[MAXROUTES];


static const __flash route_table_t *getrouteentry(routenum_t num)
{
    const __flash route_table_t *p = &__loconet_routetable_start;

    while (p < &__loconet_routetable_end)
    {
        if (p->routenum == num)
            return p;
        p++;
    }

    return NULL;
}

static bool checkdependencies(const __flash route_table_t * rc)
{
    size_t          i = rc->dependency_cnt;
    const __flash routenum_t *dep = rc->dependency;

    while (i--)
    {
        routenum_t      num = *dep++;

        if (num >= MAXROUTES)
            continue;

        route_state_t   state = parm[num].state;

        if (state == ROUTE_AWAITEXE || state == ROUTE_ACTIVE)
            return false;
    }
    return true;
}

void route_init(void)
{
}

void route_update(void)
{
    static routenum_t num = 0;
    const __flash route_table_t *p;

    // Update sub-includes
    route_queue_update();

    // Check one route per update
    if (++num >= MAXROUTES)
        num = 0;

    switch (parm[num].state)
    {
    case ROUTE_FREE:
    case ROUTE_ACTIVE:
        return;

    case ROUTE_AWAITDEP:
        p = getrouteentry(num);
        if (!p)
            return;
        // Check dependencies
        if (!checkdependencies(p))
            return;
        break;

    case ROUTE_AWAITEXE:
        p = getrouteentry(num);
        break;

    default:
        printf_P(PSTR("ERROR: Unknown route state %u %u\n"), num, parm[num].state);
        return;
    }

    // Activate route
    parm[num].state = ROUTE_ACTIVE;
    if (p)
    {
#ifdef ROUTE_DEBUG
        printf_P(PSTR("Route %u activate\n"), num);
#endif
        if (p->activateroute)
            p->activateroute();
    }
    else
    {
        printf_P(PSTR("ERROR: Activating undefined route: %u\n"), num);
    }
}

void route_request(routenum_t num)
{
    const __flash route_table_t *p;

    if (num >= MAXROUTES)
    {
        printf_P(PSTR("ERROR: Requesting invalid route num: %u\n"), num);
        return;
    }

    if (parm[num].state != ROUTE_FREE)
        return;

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u request\n"), num);
#endif

    // Check dependencies
    p = getrouteentry(num);
    if (p)
    {
        if (checkdependencies(p))
            parm[num].state = ROUTE_AWAITEXE;
        else
            parm[num].state = ROUTE_AWAITDEP;
    }
    else
    {
        printf_P(PSTR("ERROR: Requesting undefined route: %u\n"), num);
    }
}

void route_free(routenum_t num)
{
    const __flash route_table_t *p;

    if (num >= MAXROUTES)
    {
        printf_P(PSTR("ERROR: Freeing invalid route num: %u\n"), num);
        return;
    }

    if (parm[num].state != ROUTE_ACTIVE)
        return;

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u free\n"), num);
#endif

    parm[num].state = ROUTE_FREE;
    p = getrouteentry(num);
    if (p)
    {
        if (p->freeroute)
            p->freeroute();
    }
    else
    {
        printf_P(PSTR("ERROR: Freeing undefined route: %u\n"), num);
    }
}

void route_cancel(routenum_t num)
{
    const __flash route_table_t *p;

    if (num >= MAXROUTES)
    {
        printf_P(PSTR("ERROR: Canceling invalid route num: %u\n"), num);
        return;
    }

#ifdef ROUTE_DEBUG
    if (parm[num].state != ROUTE_FREE)
    {
        printf_P(PSTR("Route %u cancel\n"), num);
    }
#endif

    if (parm[num].state == ROUTE_ACTIVE)
    {
        p = getrouteentry(num);
        if (p)
        {
            if (p->cancelroute)
                p->cancelroute();
        }
        else
        {
            printf_P(PSTR("ERROR: Canceling undefined route: %u\n"), num);
        }
    }

    parm[num].state = ROUTE_FREE;
}

void route_forceactive(routenum_t num)
{
    if (num >= MAXROUTES)
    {
        printf_P(PSTR("ERROR: Forcing invalid route num: %u\n"), num);
        return;
    }

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u force active\n"), num);
#endif

    parm[num].state = ROUTE_ACTIVE;
}

void route_kill(routenum_t num)
{
    if (num >= MAXROUTES)
    {
        printf_P(PSTR("ERROR: Killing invalid route num: %u\n"), num);
        return;
    }

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u kill\n"), num);
#endif

    parm[num].state = ROUTE_FREE;
}

route_state_t route_state(routenum_t num)
{
    return parm[num].state;
}

bool route_exists(routenum_t num)
{
    return (getrouteentry(num) != NULL);
}

void route_send_sw(uint16_t adr, bool opt)
{
    route_queue_add(adr, opt, RQ_CMD_SW);
}

void route_send_sw_prio(uint16_t adr, bool opt)
{
#ifdef ROUTE_DEBUG
    printf_P(PSTR("Send SW %u %c\n"), adr, opt ? 'G' : 'R');
#endif

    switch_queue_add(adr, opt);
}

void route_send_fb(uint16_t adr, bool opt)
{
    route_queue_add(adr, opt, RQ_CMD_FB);
}

void route_send_fb_prio(uint16_t adr, bool opt)
{
    ln_tx_opc_input_rep(adr, opt, NULL, NULL);

#ifndef LNECHO
    // Update fb state (only needed if not receiving own LN echo).
    fb_handler_set_state(adr, opt);
#endif

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Send FB %u %S\n"), adr, opt ? PSTR("OCCUPIED") : PSTR("FREE"));
#endif
}
