/*
 * route_delay.c
 *
 * Created: 27-12-2024 12:41:37
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "route.h"
#include "route_delay.h"
#include "ticks.h"


typedef struct route_delay_t_
{
    struct route_delay_t_ *next;
    ticks_t         timeout;
    route_delay_cb *cb;
    routenum_t      routenum;
} route_delay_t;

static route_delay_t *head = NULL;


void route_delay_add(uint16_t timeout, route_delay_cb *cb, routenum_t num)
{
    route_delay_t  *p, *t;

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u add delay %u seconds\n"), num, timeout);
#endif

    t = malloc(sizeof(*t));
    if (!t)
    {
        printf_P(PSTR("ERROR: Out of memory\n"));
        return;
    }

    t->timeout = ticks_get() + TICKS_FROM_SEC(timeout);
    t->cb = cb;
    t->routenum = num;

    if (!head)                  // No timers in list
    {
        head = t;
        head->next = NULL;
        return;
    }

    if (!((head->timeout - timeout) & 0x80000000))
    {
        // New timer is shorter than timer in head. Put new timer first
        t->next = head;
        head = t;
        return;
    }

    p = head;
    while (p->next)
    {
        if (!((p->next->timeout - timeout) & 0x80000000))
            break;
        p = p->next;
    }

    t->next = p->next;
    p->next = t;
}

void route_delay_cancel(routenum_t num)
{
    route_delay_t **p = &head;

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u cancel delay\n"), num);
#endif

    while (*p)
    {
        if ((*p)->routenum == num)
        {
            route_delay_t  *q = *p;

            *p = q->next;
            free(q);
        }
        else
        {
            p = &((*p)->next);
        }
    }
}

void route_delay_update(void)
{
    route_delay_t  *p;

    if (!head)
        return;

    if ((ticks_get() - head->timeout) & 0x80000000)
        return;

    // Remove timer from list
    p = head;
    head = head->next;

#ifdef ROUTE_DEBUG
    printf_P(PSTR("Route %u delay timeout\n"), p->routenum);
#endif

    // Timeout callback
    p->cb(p->routenum);

    free(p);
}
