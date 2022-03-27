/*
 * collision_check.c
 *
 * Created: 04-07-2021 15:17:45
 *  Author: Mikael Ejberg Pedersen
 *
 * If CCLDEBUG is defined, a very short pulse is sent on port D3 in case
 * of a collision (this is part of the loconet-avrda library). This is fine
 * for a logic analyzer, but not for visual indication on a LED.
 *
 * This file adds a longer pulse (250 ms) on port D3 whenever a collision
 * happens, but only if CCLDEBUG is NOT defined.
 */

#include <avr/io.h>
#include "ticks.h"
#include "lib/loconet-avrda/hal_ln.h"

#define COLLISION_TICKS (TICKS_PER_SEC / 4)

static ticks_t  coll_time = 0;


void collision_check_init(void)
{
#ifndef CCLDEBUG
    PORTD.OUTCLR = PIN3_bm;
    PORTD.DIRSET = PIN3_bm;
#endif
}

void collision_check_update(void)
{
#ifndef CCLDEBUG
    if (hal_ln_tx_collision())
    {
        PORTD.OUTSET = PIN3_bm;
        coll_time = ticks_get();
    }
    else if (ticks_elapsed(coll_time) >= COLLISION_TICKS)
    {
        PORTD.OUTCLR = PIN3_bm;
    }
#endif
}
