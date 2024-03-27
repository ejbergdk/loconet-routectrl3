/*
 * ctrl_util.c
 *
 * Created: 27-03-2024 15:32:52
 *  Author: Mikael Ejberg Pedersen
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "ctrl_util.h"
#include "fb_handler.h"
#include "mmi.h"
#include "route.h"
#include "sw_handler.h"


#define SWITCH_LOCK_OFFSET  300
#define DKW_OFFSET          900


void util_lock_sw(uint16_t adr, bool opt)
{
    route_send_fb(adr + SWITCH_LOCK_OFFSET, FB_OCCUPIED);
    route_send_sw(adr, opt);
}


void util_unlock_sw(uint16_t adr)
{
    route_send_fb(adr + SWITCH_LOCK_OFFSET, FB_FREE);
}


void util_lock_dkw(uint16_t adr, bool opt, bool opt2)
{
    route_send_fb(adr + SWITCH_LOCK_OFFSET, FB_OCCUPIED);
    route_send_sw(adr, opt);
    route_send_sw(adr + DKW_OFFSET, opt2);
}


/*
 * Track-Control route specific.
 * Feedback 400-599 must be freed as soon as an occupy is received,
 * or Track-Control will lock up its routes.
 */
static void tc_route_release(uint16_t adr)
{
    route_send_fb_prio(adr, FB_FREE);
}

FEEDBACK_RANGE_OCC(400, 599, tc_route_release);


/*
 * Remote control of operating level.
 */
static void operating_level_cmd(uint16_t adr, bool opt)
{
    if (opt == SW_R)
        mmi_operating_level_decrease();
    else
        mmi_operating_level_increase();
}

SWITCH_REQ(200, operating_level_cmd);


uint16_t util_rand(uint16_t min, uint16_t max)
{
    max = max - min + 1;
    return (rand() % max) + min;
}
