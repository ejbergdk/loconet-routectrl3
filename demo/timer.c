/*
 * timer.c
 *
 * Created: 10-05-2021 18:06:55
 *  Author: Mikael Ejberg Pedersen
 */

#include <stddef.h>
#include <stdlib.h>
#include "ticks.h"
#include "timer.h"


typedef struct timer_t_
{
    struct timer_t_ *next;
    ticks_t         timeout;
    timer_cb       *cb;
    void           *ctx;
} timer_t;

static timer_t *head = NULL;


void timer_add(ticks_t timeout, timer_cb *cb, void *ctx)
{
    timer_t *p, *t;

    t = malloc(sizeof(*t));
    if (!t)
        return;

    timeout += ticks_get();
    t->timeout = timeout;
    t->cb = cb;
    t->ctx = ctx;

    if (!head)  // No timers in list
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

void timer_update(void)
{
    timer_t *p;

    if (!head)
        return;

    if ((ticks_get() - head->timeout) & 0x80000000)
        return;

    // Remove timer from list
    p = head;
    head = head->next;

    // Timeout callback
    p->cb(p->ctx);

    free(p);
}
