/*
 * ctrl_routetables.c
 *
 * Created: 27-03-2024 16:01:52
 *  Author: Mikael Ejberg Pedersen
 */

#include <stdlib.h>
#include "ctrl_util.h"
#include "mmi.h"
#include "route.h"
#include "route_delay.h"


// ROUTE 1: A to E
static void rt1(void)
{
    route_request(105);
    util_lock_sw(1, SW_G);
    route_request(150);
}

static void fr1(void)
{
    util_unlock_sw(1);
    if (mmi_operating_level_get() >= 3)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(9) != ROUTE_FREE || route_state(10) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(9);   // Station entry, platform 1 (50%)
        route_request(10);      // Station entry, platform 2
        route_request(9);       // Station entry, platform 1
    }
}

static void ca1(void)
{
    route_cancel(150);
    route_cancel(105);
    util_unlock_sw(1);
}

ROUTE(1, rt1, fr1, ca1, 103, 105, 130, 133, 2, 4, 5, 11);


// ROUTE 2: B to E
static void rt2(void)
{
    route_request(105);
    util_lock_dkw(2, SW_R, SW_G);
    util_lock_sw(1, SW_R);
    route_request(151);
}

static void fr2(void)
{
    route_cancel(3);
    route_cancel(46);
    util_unlock_sw(2);
    util_unlock_sw(1);
    if (mmi_operating_level_get() >= 3)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(9) != ROUTE_FREE || route_state(10) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(9);   // Station entry, platform 1 (50%)
        route_request(10);      // Station entry, platform 2
        route_request(9);       // Station entry, platform 1
    }
}

static void ca2(void)
{
    route_cancel(151);
    route_cancel(105);
    util_unlock_sw(2);
    util_unlock_sw(1);
}

ROUTE(2, rt2, fr2, ca2, 103, 105, 130, 133, 1, 3, 4, 5, 11, 46, 47);


// ROUTE 3: B to N
static void rt3(void)
{
    route_request(114);
    route_request(136);         // ExB
    util_lock_dkw(2, SW_G, SW_R);
    util_lock_sw(33, SW_G);
    route_request(152);
}

static void fr3(void)
{
    route_cancel(2);
    route_cancel(46);
    util_unlock_sw(2);
    // Can't unlock sw 33 yet. Wait for train to pass ExB
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(27) != ROUTE_FREE || route_state(28) != ROUTE_FREE || route_state(29) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(27);  // Tunnel (50%)
        if (rand() & 1)
            route_request(29);  // Skyline (50%)
        route_request(28);      // Shadow station
        route_request(27);      // Tunnel
        route_request(29);      // Skyline
    }
}

static void ca3(void)
{
    route_cancel(152);
    route_cancel(114);
    route_cancel(136);          // ExB
    util_unlock_sw(2);
    util_unlock_sw(33);
}

ROUTE(3, rt3, fr3, ca3, 114, 136, 2, 4, 46, 47);


// ROUTE 4: C to D
static void rt4(void)
{
    route_request(104);
    util_lock_sw(1, SW_R);
    util_lock_dkw(2, SW_G, SW_G);
    util_lock_sw(3, SW_R);
    route_request(155);
    route_delay_cancel(13);
    route_delay_cancel(16);
}

static void fr4(void)
{
    route_cancel(5);
    util_unlock_sw(1);
    util_unlock_sw(2);
    util_unlock_sw(3);
    if (mmi_operating_level_get() >= 3)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(6) != ROUTE_FREE || route_state(7) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(7);   // Station entry, platform 3 (50%)
        route_request(6);       // Station entry, platform 2
        route_request(7);       // Station entry, platform 3
    }
}

static void ca4(void)
{
    route_cancel(155);
    route_cancel(104);
    util_unlock_sw(1);
    util_unlock_sw(2);
    util_unlock_sw(3);
}

ROUTE(4, rt4, fr4, ca4, 104, 1, 2, 3, 5, 8, 30, 46, 47);


// ROUTE 5: C to L
static void rt5(void)
{
    route_request(112);
    util_lock_sw(1, SW_G);
    route_request(156);
    route_delay_cancel(13);
    route_delay_cancel(16);
}

static void fr5(void)
{
    route_cancel(4);
    util_unlock_sw(1);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(22) != ROUTE_FREE || route_state(23) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(22);  // Front (50%)
        route_request(23);      // Skyline
        route_request(22);      // Front
    }
}

static void ca5(void)
{
    route_cancel(156);
    route_cancel(112);
    util_unlock_sw(1);
}

ROUTE(5, rt5, fr5, ca5, 101, 112, 129, 131, 1, 2, 4);


// ROUTE 6: D to G
static void rt6(void)
{
    route_request(107);
    util_lock_sw(22, SW_R);
    util_lock_sw(21, SW_G);
    route_request(158);
}

static void fr6delay(routenum_t num)
{
    if (rand() & 1)
        route_request(13);      // Lower west departure (50%)
    route_request(14);          // Upper west departure
    route_request(13);          // Lower west departure
}

static void fr6(void)
{
    route_cancel(7);
    util_unlock_sw(22);
    util_unlock_sw(21);
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(13) != ROUTE_FREE || route_state(14) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr6delay, 6);
    }
}

static void ca6(void)
{
    route_cancel(158);
    route_cancel(107);
    util_unlock_sw(22);
    util_unlock_sw(21);
}

ROUTE(6, rt6, fr6, ca6, 107, 110, 127, 128, 7, 8, 15, 21, 44);


// ROUTE 7: D to H
static void rt7(void)
{
    route_request(108);
    util_lock_sw(22, SW_G);
    util_lock_sw(19, SW_G);
    route_request(159);
}

static void fr7delay(routenum_t num)
{
    if (rand() & 1)
        route_request(17);      // Upper west departure (50%)
    route_request(16);          // Lower west departure
    route_request(17);          // Upper west departure
}

static void fr7(void)
{
    route_cancel(6);
    util_unlock_sw(22);
    // w19 can't be unlocked yet, as it is inside track H and may be occupied
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(16) != ROUTE_FREE || route_state(17) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr7delay, 7);
    }
}

static void ca7(void)
{
    route_cancel(159);
    route_cancel(108);
    util_unlock_sw(22);
    util_unlock_sw(19);
}

ROUTE(7, rt7, fr7, ca7, 108, 6, 8, 18);


// ROUTE 8: D shunting
static void rt8(void)
{
    route_send_sw(108, SW_G);
}

static void fr8(void)
{
    route_send_sw(108, SW_R);   // Only for making Track-Control board happy
    route_send_sw(107, SW_R);
}

ROUTE(8, rt8, fr8, NULL);


// ROUTE 9: E to I
static void rt9(void)
{
    route_request(109);
    util_lock_sw(9, SW_R);
    route_request(161);
}

static void fr9delay(routenum_t num)
{
    route_request(19);          // East departure
}

static void fr9(void)
{
    route_cancel(10);
    util_unlock_sw(9);
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(19) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr9delay, 9);
    }
}

static void ca9(void)
{
    route_cancel(161);
    route_cancel(109);
    util_unlock_sw(9);
}

ROUTE(9, rt9, fr9, ca9, 109, 10, 11, 20, 13, 16);


// ROUTE 10: E to J
static void rt10(void)
{
    route_request(110);
    util_lock_sw(9, SW_G);
    util_lock_dkw(10, SW_R, SW_R);
    route_request(162);
}

static void fr10delay(routenum_t num)
{
    route_request(21);          // East departure
}

static void fr10(void)
{
    route_cancel(9);
    util_unlock_sw(9);
    util_unlock_sw(10);
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(21) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr10delay, 10);
    }
}

static void ca10(void)
{
    route_cancel(162);
    route_cancel(110);
    util_unlock_sw(9);
    util_unlock_sw(10);
}

ROUTE(10, rt10, fr10, ca10, 107, 110, 127, 128, 9, 11, 12, 13, 14, 15, 16, 17, 48);


// ROUTE 11: E shunting
static void rt11(void)
{
    route_send_sw(116, SW_G);
}

static void fr11(void)
{
    route_send_sw(116, SW_R);   // Only for making Track-Control board happy
    route_send_sw(115, SW_R);
}

ROUTE(11, rt11, fr11, NULL);


// ROUTE 12: F to J
static void rt12(void)
{
    route_request(110);
    util_lock_dkw(10, SW_G, SW_G);
    route_request(164);
}

static void fr12delay(routenum_t num)
{
    route_request(21);          // East departure
}

static void fr12(void)
{
    util_unlock_sw(10);
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(21) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr12delay, 12);
    }
}

static void ca12(void)
{
    route_cancel(164);
    route_cancel(110);
    util_unlock_sw(10);
}

ROUTE(12, rt12, fr12, ca12, 107, 110, 127, 128, 10, 11, 13, 14, 15, 16, 17, 48);


// ROUTE 13: G to C
static void rt13delay(routenum_t num)
{
    if (rand() & 1)
        route_request(4);       // Behind town (50%)
    route_request(5);           // Tunnel
    route_request(4);           // Behind town
}

static void rt13(void)
{
    route_request(103);
    route_send_sw(119, SW_G);   // Set SU signal
    util_lock_dkw(10, SW_R, SW_R);
    util_lock_sw(9, SW_G);
    route_request(166);
    route_delay_cancel(6);
    route_delay_cancel(45);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(4) != ROUTE_FREE || route_state(5) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(5, 15), rt13delay, 13);
    }
}

static void fr13(void)
{
    route_cancel(14);
    util_unlock_sw(10);
    util_unlock_sw(9);
}

static void ca13(void)
{
    route_send_sw(119, SW_R);   // Set SU signal
    route_cancel(166);
    route_cancel(103);
    util_unlock_sw(10);
    util_unlock_sw(9);
    if (mmi_operating_level_get() >= 2)
    {
        route_delay_cancel(13);
        route_cancel(4);
        route_cancel(5);
    }
}

ROUTE(13, rt13, fr13, ca13, 103, 105, 130, 133, 9, 10, 11, 12, 14, 15, 16, 17, 48);


// ROUTE 14: G to W
static void rt14(void)
{
    route_request(123);
    route_send_sw(121, SW_G);   // Set U signal
    util_lock_dkw(10, SW_G, SW_G);
    route_request(167);
    route_delay_cancel(6);
    route_delay_cancel(45);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(41) != ROUTE_FREE)
            return;
        route_request(41);      // Next block on skyline
    }
}

static void fr14(void)
{
    route_cancel(13);
    util_unlock_sw(10);
}

static void ca14(void)
{
    route_send_sw(121, SW_R);   // Set U signal
    route_cancel(167);
    route_cancel(123);
    util_unlock_sw(10);
    if (mmi_operating_level_get() >= 1)
        route_cancel(41);
}

ROUTE(14, rt14, fr14, ca14, 106, 123, 124, 134, 10, 11, 12, 13, 15, 16, 17, 48);


// ROUTE 15: G+J shunting
static void rt15(void)
{
    route_send_sw(104, SW_G);
    route_send_sw(112, SW_G);
}

static void fr15(void)
{
    route_send_sw(104, SW_R);   // Only for making Track-Control board happy
    route_send_sw(103, SW_R);
    route_send_sw(112, SW_R);   // Only for making Track-Control board happy
    route_send_sw(111, SW_R);
}

ROUTE(15, rt15, fr15, NULL);


// ROUTE 16: H to C
static void rt16delay(routenum_t num)
{
    if (rand() & 1)
        route_request(5);       // Tunnel (50%)
    route_request(4);           // Behind town
    route_request(5);           // Tunnel
}

static void rt16(void)
{
    route_request(103);
    route_send_sw(119, SW_G);   // Set SU signal
    util_lock_sw(11, SW_R);
    util_lock_dkw(10, SW_G, SW_R);
    util_lock_sw(9, SW_G);
    route_request(169);
    route_delay_cancel(7);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(4) != ROUTE_FREE || route_state(5) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(5, 15), rt16delay, 16);
    }
}

static void fr16(void)
{
    route_cancel(17);
    util_unlock_sw(11);
    util_unlock_sw(10);
    util_unlock_sw(9);
}

static void ca16(void)
{
    route_send_sw(119, SW_R);   // Set SU signal
    route_cancel(169);
    route_cancel(103);
    util_unlock_sw(11);
    util_unlock_sw(10);
    util_unlock_sw(9);
    if (mmi_operating_level_get() >= 2)
    {
        route_delay_cancel(16);
        route_cancel(5);
        route_cancel(4);
    }
}

ROUTE(16, rt16, fr16, ca16, 103, 105, 130, 133, 9, 10, 11, 12, 13, 14, 17, 18, 48);


// ROUTE 17: H to W
static void rt17(void)
{
    route_request(123);
    route_send_sw(121, SW_G);   // Set U signal
    util_lock_sw(11, SW_R);
    util_lock_dkw(10, SW_R, SW_G);
    route_request(170);
    route_delay_cancel(7);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(41) != ROUTE_FREE)
            return;
        route_request(41);      // Next block on skyline
    }
}

static void fr17(void)
{
    route_cancel(16);
    util_unlock_sw(11);
    util_unlock_sw(10);
}

static void ca17(void)
{
    route_send_sw(121, SW_R);   // Set U signal
    route_cancel(170);
    route_cancel(123);
    util_unlock_sw(11);
    util_unlock_sw(10);
    if (mmi_operating_level_get() >= 1)
        route_cancel(41);
}

ROUTE(17, rt17, fr17, ca17, 106, 123, 124, 134, 10, 11, 12, 13, 14, 16, 18, 48);


// ROUTE 18: H shunting
static void rt18(void)
{
    route_send_sw(114, SW_G);
}

static void fr18(void)
{
    route_send_sw(114, SW_R);   // Only for making Track-Control board happy
    route_send_sw(113, SW_R);
}

ROUTE(18, rt18, fr18, NULL);


// ROUTE 19: I to B
static void rt19(void)
{
    route_request(102);
    route_request(135);         // ExA
    route_send_sw(105, SW_G);
    util_lock_sw(23, SW_R);
    util_lock_sw(37, SW_R);
    route_request(172);
    route_delay_cancel(9);
}

static void fr19(void)
{
    route_cancel(43);
    util_unlock_sw(23);
    // w37 can't be unlocked yet. Wait for train to pass ExA
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(2) != ROUTE_FREE || route_state(3) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(3);   // Left ramp down (50%)
        route_request(2);       // Left ramp up
        route_request(3);       // Left ramp down
    }
}

static void ca19(void)
{
    route_send_sw(105, SW_R);
    route_cancel(135);          // ExA
    route_cancel(172);
    route_cancel(102);
    util_unlock_sw(23);
    util_unlock_sw(37);
}

ROUTE(19, rt19, fr19, ca19, 102, 135, 20, 21, 43, 44, 45);


// ROUTE 20: I shunting
static void rt20(void)
{
    route_send_sw(102, SW_G);
}

static void fr20(void)
{
    route_send_sw(102, SW_R);   // Only for making Track-Control board happy
    route_send_sw(101, SW_R);
}

ROUTE(20, rt20, fr20, NULL);


// ROUTE 21: J to B
static void rt21(void)
{
    route_request(102);
    route_request(135);         // ExA
    route_send_sw(105, SW_G);
    util_lock_sw(21, SW_R);
    util_lock_sw(23, SW_G);
    util_lock_sw(37, SW_R);
    route_request(174);
    route_delay_cancel(10);
    route_delay_cancel(12);
}

static void fr21(void)
{
    route_cancel(44);
    util_unlock_sw(21);
    util_unlock_sw(23);
    // w37 can't be unlocked yet. Wait for train to pass ExA
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(2) != ROUTE_FREE || route_state(3) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(3);   // Left ramp down (50%)
        route_request(2);       // Left ramp up
        route_request(3);       // Left ramp down
    }
}

static void ca21(void)
{
    route_send_sw(105, SW_R);
    route_cancel(135);          // ExA
    route_cancel(174);
    route_cancel(102);
    util_unlock_sw(21);
    util_unlock_sw(23);
    util_unlock_sw(37);
}

ROUTE(21, rt21, fr21, ca21, 102, 135, 15, 19, 20, 43, 44, 45);


// ROUTE 22: L to P
static void rt22(void)
{
    route_request(116);
    util_lock_sw(25, SW_G);
    util_lock_sw(26, SW_R);
    util_lock_dkw(17, SW_R, SW_G);
    util_lock_sw(18, SW_G);
    route_request(175);
}

static void fr22(void)
{
    route_cancel(23);
    util_unlock_sw(25);
    util_unlock_sw(26);
    util_unlock_sw(17);
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca22(void)
{
    route_cancel(175);
    route_cancel(116);
    util_unlock_sw(25);
    util_unlock_sw(26);
    util_unlock_sw(17);
    util_unlock_sw(18);
}

ROUTE(22, rt22, fr22, ca22, 116, 23, 24, 25, 26, 27, 28, 29, 36, 37, 38, 39, 40);


// ROUTE 23: L to X
static void rt23(void)
{
    route_request(124);
    util_lock_sw(25, SW_G);
    util_lock_sw(26, SW_G);
    util_lock_sw(24, SW_R);
    route_request(175);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(42) != ROUTE_FREE)
            return;
        route_request(42);      // Next block on skyline
    }
}

static void fr23(void)
{
    route_cancel(22);
    util_unlock_sw(25);
    util_unlock_sw(26);
    util_unlock_sw(24);
}

static void ca23(void)
{
    route_cancel(175);
    route_cancel(124);
    util_unlock_sw(25);
    util_unlock_sw(26);
    util_unlock_sw(24);
    if (mmi_operating_level_get() >= 1)
        route_cancel(42);
}

ROUTE(23, rt23, fr23, ca23, 113, 123, 124, 132, 22, 24, 25, 26, 27, 28, 29);


// ROUTE 24: M to A
static void rt24(void)
{
    route_request(101);
    util_lock_sw(24, SW_R);
    util_lock_sw(26, SW_G);
    util_lock_sw(25, SW_G);
    route_request(177);
}

static void fr24(void)
{
    route_cancel(25);
    route_cancel(26);
    util_unlock_sw(24);
    util_unlock_sw(26);
    util_unlock_sw(25);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(1) != ROUTE_FREE)
            return;
        route_request(1);       // Tunnel
    }
}

static void ca24(void)
{
    route_cancel(177);
    route_cancel(101);
    util_unlock_sw(24);
    util_unlock_sw(26);
    util_unlock_sw(25);
}

ROUTE(24, rt24, fr24, ca24, 101, 112, 129, 131, 22, 23, 25, 26, 27, 28, 29);


// ROUTE 25: M to P
static void rt25(void)
{
    route_request(116);
    util_lock_sw(24, SW_G);
    util_lock_dkw(17, SW_G, SW_G);
    util_lock_sw(18, SW_G);
    route_request(178);
}

static void fr25(void)
{
    route_cancel(24);
    route_cancel(26);
    util_unlock_sw(24);
    util_unlock_sw(17);
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca25(void)
{
    route_cancel(178);
    route_cancel(116);
    util_unlock_sw(24);
    util_unlock_sw(17);
    util_unlock_sw(18);
}

ROUTE(25, rt25, fr25, ca25, 116, 22, 23, 24, 26, 27, 28, 29, 36, 37, 38, 39, 40);


// ROUTE 26: M to Q
static void rt26(void)
{
    route_request(117);
    util_lock_sw(24, SW_R);
    util_lock_sw(26, SW_G);
    util_lock_sw(25, SW_R);
    route_request(177);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(31) != ROUTE_FREE || route_state(32) != ROUTE_FREE || route_state(33) != ROUTE_FREE ||
            route_state(34) != ROUTE_FREE || route_state(35) != ROUTE_FREE)
            return;
        route_request(31);      // Shadow station, platform 1
        route_request(32);      // Shadow station, platform 2
        route_request(33);      // Shadow station, platform 3
        route_request(34);      // Shadow station, platform 4
        route_request(35);      // Shadow station, platform 5
    }
}

static void fr26(void)
{
    route_cancel(24);
    route_cancel(25);
    util_unlock_sw(24);
    util_unlock_sw(26);
    util_unlock_sw(25);
}

static void ca26(void)
{
    route_cancel(177);
    route_cancel(117);
    util_unlock_sw(24);
    util_unlock_sw(26);
    util_unlock_sw(25);
    if (mmi_operating_level_get() >= 1)
    {
        route_cancel(31);
        route_cancel(32);
        route_cancel(33);
        route_cancel(34);
        route_cancel(35);
    }
}

ROUTE(26, rt26, fr26, ca26, 117, 22, 23, 24, 25, 27, 28, 29);


// ROUTE 27: N to A
static void rt27(void)
{
    route_request(101);
    util_lock_dkw(17, SW_G, SW_R);
    util_lock_sw(26, SW_R);
    util_lock_sw(25, SW_G);
    route_request(180);
}

static void fr27(void)
{
    route_cancel(28);
    route_cancel(29);
    util_unlock_sw(17);
    util_unlock_sw(26);
    util_unlock_sw(25);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(1) != ROUTE_FREE)
            return;
        route_request(1);       // Tunnel
    }
}

static void ca27(void)
{
    route_cancel(180);
    route_cancel(101);
    util_unlock_sw(17);
    util_unlock_sw(26);
    util_unlock_sw(25);
}

ROUTE(27, rt27, fr27, ca27, 101, 112, 129, 131, 22, 23, 24, 25, 26, 28, 29);


// ROUTE 28: N to Q
static void rt28(void)
{
    route_request(117);
    util_lock_dkw(17, SW_G, SW_R);
    util_lock_sw(26, SW_R);
    util_lock_sw(25, SW_R);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(31) != ROUTE_FREE || route_state(32) != ROUTE_FREE || route_state(33) != ROUTE_FREE ||
            route_state(34) != ROUTE_FREE || route_state(35) != ROUTE_FREE)
            return;
        route_request(31);      // Shadow station, platform 1
        route_request(32);      // Shadow station, platform 2
        route_request(33);      // Shadow station, platform 3
        route_request(34);      // Shadow station, platform 4
        route_request(35);      // Shadow station, platform 5
    }
}

static void fr28(void)
{
    route_cancel(27);
    route_cancel(29);
    util_unlock_sw(17);
    util_unlock_sw(26);
    util_unlock_sw(25);
}

static void ca28(void)
{
    route_cancel(180);
    route_cancel(117);
    util_unlock_sw(17);
    util_unlock_sw(26);
    util_unlock_sw(25);
    if (mmi_operating_level_get() >= 1)
    {
        route_cancel(31);
        route_cancel(32);
        route_cancel(33);
        route_cancel(34);
        route_cancel(35);
    }
}

ROUTE(28, rt28, fr28, ca28, 117, 22, 23, 24, 25, 26, 27, 29);


// ROUTE 29: N to X
static void rt29(void)
{
    route_request(124);
    util_lock_dkw(17, SW_R, SW_R);
    util_lock_sw(24, SW_G);
    route_request(181);
    if (mmi_operating_level_get() >= 1)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(42) != ROUTE_FREE)
            return;
        route_request(42);      // Next block on skyline
    }
}

static void fr29(void)
{
    route_cancel(27);
    route_cancel(28);
    util_unlock_sw(17);
    util_unlock_sw(24);
}

static void ca29(void)
{
    route_cancel(124);
    route_cancel(181);
    util_unlock_sw(17);
    util_unlock_sw(24);
    if (mmi_operating_level_get() >= 1)
        route_cancel(42);
}

ROUTE(29, rt29, fr29, ca29, 113, 123, 124, 132, 22, 23, 24, 25, 26, 27, 28);


// ROUTE 30: P to D
static void rt30(void)
{
    route_request(104);
    util_lock_sw(3, SW_G);
    route_request(183);
}

static void fr30(void)
{
    util_unlock_sw(3);
    if (mmi_operating_level_get() >= 3)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(6) != ROUTE_FREE || route_state(7) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(7);   // Station entry, platform 3 (50%)
        route_request(6);       // Station entry, platform 2
        route_request(7);       // Station entry, platform 3
    }
}

static void ca30(void)
{
    route_cancel(183);
    route_cancel(104);
    util_unlock_sw(3);
}

ROUTE(30, rt30, fr30, ca30, 104, 4, 8, 47);


// ROUTE 31: Q to R
static void rt31(void)
{
    route_request(118);
    route_send_sw(5, SW_G);
    route_send_sw(6, SW_G);
    route_send_sw(7, SW_G);
    route_send_sw(8, SW_G);
    route_send_sw(29, SW_G);
    route_send_sw(4, SW_G);
}

static void fr31(void)
{
    route_cancel(32);
    route_cancel(33);
    route_cancel(34);
    route_cancel(35);
    route_send_sw_prio(29, SW_R);
}

static void ca31(void)
{
    route_send_sw(4, SW_R);
    route_cancel(118);
}

ROUTE(31, rt31, fr31, ca31, 118, 32, 33, 34, 35);


// ROUTE 32: Q to S
static void rt32(void)
{
    route_request(119);
    route_send_sw(5, SW_G);
    route_send_sw(6, SW_G);
    route_send_sw(7, SW_G);
    route_send_sw(8, SW_R);
    route_send_sw(30, SW_G);
    route_send_sw(4, SW_G);
}

static void fr32(void)
{
    route_cancel(31);
    route_cancel(33);
    route_cancel(34);
    route_cancel(35);
    route_send_sw_prio(30, SW_R);
}

static void ca32(void)
{
    route_send_sw(4, SW_R);
    route_cancel(119);
}

ROUTE(32, rt32, fr32, ca32, 119, 31, 33, 34, 35);


// ROUTE 33: Q to T
static void rt33(void)
{
    route_request(120);
    route_send_sw(5, SW_G);
    route_send_sw(6, SW_G);
    route_send_sw(7, SW_R);
    route_send_sw(31, SW_G);
    route_send_sw(4, SW_G);
}

static void fr33(void)
{
    route_cancel(31);
    route_cancel(32);
    route_cancel(34);
    route_cancel(35);
    route_send_sw_prio(31, SW_R);
}

static void ca33(void)
{
    route_send_sw(4, SW_R);
    route_cancel(120);
}

ROUTE(33, rt33, fr33, ca33, 120, 31, 32, 34, 35);


// ROUTE 34: Q to U
static void rt34(void)
{
    route_request(121);
    route_send_sw(5, SW_G);
    route_send_sw(6, SW_R);
    route_send_sw(32, SW_G);
    route_send_sw(4, SW_G);
}

static void fr34(void)
{
    route_cancel(31);
    route_cancel(32);
    route_cancel(33);
    route_cancel(35);
    route_send_sw_prio(32, SW_R);
}

static void ca34(void)
{
    route_send_sw(4, SW_R);
    route_cancel(121);
}

ROUTE(34, rt34, fr34, ca34, 121, 31, 32, 33, 35);


// ROUTE 35: Q to V
static void rt35(void)
{
    route_request(122);
    route_send_sw(5, SW_R);
    route_send_sw(16, SW_G);
    route_send_sw(4, SW_G);
}

static void fr35(void)
{
    route_cancel(31);
    route_cancel(32);
    route_cancel(33);
    route_cancel(34);
    route_send_sw_prio(16, SW_R);
}

static void ca35(void)
{
    route_send_sw(4, SW_R);
    route_cancel(122);
}

ROUTE(35, rt35, fr35, ca35, 122, 31, 32, 33, 34);


// ROUTE 36: R to P
static void rt36(void)
{
    route_request(116);
    util_lock_sw(18, SW_R);
    route_send_sw(29, SW_G);
}

static void fr36(void)
{
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca36(void)
{
    route_send_sw(29, SW_R);
    route_cancel(116);
    util_unlock_sw(18);
}

ROUTE(36, rt36, fr36, ca36, 116, 22, 25, 37, 38, 39, 40);


// ROUTE 37: S to P
static void rt37(void)
{
    route_request(116);
    util_lock_sw(18, SW_R);
    route_send_sw(30, SW_G);
}

static void fr37(void)
{
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca37(void)
{
    route_send_sw(30, SW_R);
    route_cancel(116);
    util_unlock_sw(18);
}

ROUTE(37, rt37, fr37, ca37, 116, 22, 25, 36, 38, 39, 40);


// ROUTE 38: T to P
static void rt38(void)
{
    route_request(116);
    util_lock_sw(18, SW_R);
    route_send_sw(31, SW_G);
}

static void fr38(void)
{
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca38(void)
{
    route_send_sw(31, SW_R);
    route_cancel(116);
    util_unlock_sw(18);
}

ROUTE(38, rt38, fr38, ca38, 116, 22, 25, 36, 37, 39, 40);


// ROUTE 39: U to P
static void rt39(void)
{
    route_request(116);
    util_lock_sw(18, SW_R);
    route_send_sw(32, SW_G);
}

static void fr39(void)
{
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca39(void)
{
    route_send_sw(32, SW_R);
    route_cancel(116);
    util_unlock_sw(18);
}

ROUTE(39, rt39, fr39, ca39, 116, 22, 25, 36, 37, 38, 40);


// ROUTE 40: V to P
static void rt40(void)
{
    route_request(116);
    util_lock_sw(18, SW_R);
    route_send_sw(16, SW_G);
}

static void fr40(void)
{
    util_unlock_sw(18);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(30) != ROUTE_FREE)
            return;
        route_request(30);      // Behind town
    }
}

static void ca40(void)
{
    route_send_sw(16, SW_R);
    route_cancel(116);
    util_unlock_sw(18);
}

ROUTE(40, rt40, fr40, ca40, 116, 22, 25, 36, 37, 38, 39);


// ROUTE 41: W to M
static void rt41(void)
{
    route_request(113);
    route_send_sw(133, SW_G);
}

static void fr41(void)
{
    route_send_sw(133, SW_R);
    if (mmi_operating_level_get() >= 2)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(24) != ROUTE_FREE || route_state(25) != ROUTE_FREE || route_state(26) != ROUTE_FREE)
            return;
        if (rand() & 1)
            route_request(25);  // Front (50%)
        if (rand() & 1)
            route_request(24);  // Tunnel (50%)
        route_request(26);      // Shadow station
        route_request(25);      // Front
        route_request(24);      // Tunnel
    }
}

static void ca41(void)
{
    route_send_sw(133, SW_R);
    route_cancel(113);
}

ROUTE(41, rt41, fr41, ca41, 113, 124, 132);


// ROUTE 42: X to F
static void rt42(void)
{
    route_request(106);
    route_send_sw(135, SW_G);
}

static void fr42(void)
{
    route_send_sw(135, SW_R);
    if (mmi_operating_level_get() >= 3)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(12) != ROUTE_FREE)
            return;
        route_request(12);      // Station entry, platform 2
    }
}

static void ca42(void)
{
    route_send_sw(135, SW_R);
    route_cancel(106);
}

ROUTE(42, rt42, fr42, ca42, 106, 123, 134, 48);


// ROUTE 43: I to Y
static void rt43(void)
{
    route_request(125);
    route_request(135);         // ExA
    util_lock_sw(23, SW_R);
    util_lock_sw(37, SW_G);
    route_request(172);
}

static void fr43(void)
{
    route_cancel(19);
    util_unlock_sw(23);
    // w37 can't be unlocked yet. Wait for train to pass ExA
}

static void ca43(void)
{
    route_cancel(172);
    route_cancel(125);
    route_cancel(135);          // ExA
    util_unlock_sw(23);
    util_unlock_sw(37);
}

ROUTE(43, rt43, fr43, ca43, 125, 135, 19, 20, 21, 44, 45);


// ROUTE 44: J to Y
static void rt44(void)
{
    route_request(125);
    route_request(135);         // ExA
    util_lock_sw(21, SW_R);
    util_lock_sw(23, SW_G);
    util_lock_sw(37, SW_G);
    route_request(174);
}

static void fr44(void)
{
    route_cancel(21);
    util_unlock_sw(21);
    util_unlock_sw(23);
    // w37 can't be unlocked yet. Wait for train to pass ExA
}

static void ca44(void)
{
    route_cancel(174);
    route_cancel(125);
    route_cancel(135);          // ExA
    util_unlock_sw(21);
    util_unlock_sw(23);
    util_unlock_sw(37);
}

ROUTE(44, rt44, fr44, ca44, 125, 135, 15, 19, 20, 21, 43, 45);


// ROUTE 45: Y to G
static void rt45(void)
{
    route_request(107);
    util_lock_sw(37, SW_G);
    util_lock_sw(23, SW_G);
    util_lock_sw(21, SW_R);
    // route_request(xxx); No signal defined at Y yet
}

static void fr45delay(routenum_t num)
{
    if (rand() & 1)
        route_request(14);      // Upper west departure (50%)
    route_request(13);          // Lower west departure
    route_request(14);          // Upper west departure
}

static void fr45(void)
{
    util_unlock_sw(37);
    util_unlock_sw(23);
    util_unlock_sw(21);
    if (mmi_operating_level_get() >= 4)
    {
        // Auto request route out of here
        // If such a route is already requested, don't bother
        if (route_state(13) != ROUTE_FREE || route_state(14) != ROUTE_FREE)
            return;
        route_delay_add(util_rand(10, 30), fr45delay, 45);
    }
}

static void ca45(void)
{
    // route_cancel(xxx); No signal defined at Y yet
    route_cancel(107);
    util_unlock_sw(37);
    util_unlock_sw(23);
    util_unlock_sw(21);
}

ROUTE(45, rt45, fr45, ca45, 107, 110, 135, 6, 15, 19, 20, 21, 43, 44, 127, 128);


// ROUTE 46: B to Z
static void rt46(void)
{
    route_request(126);
    route_request(136);         // ExB
    util_lock_dkw(2, SW_G, SW_R);
    util_lock_sw(33, SW_R);
    route_request(152);
}

static void fr46(void)
{
    route_cancel(2);
    route_cancel(3);
    util_unlock_sw(2);
    // w33 can't be unlocked yet. Wait for train to pass ExB
}

static void ca46(void)
{
    route_cancel(152);
    route_cancel(126);
    route_cancel(136);
    util_unlock_sw(2);
    util_unlock_sw(33);
}

ROUTE(46, rt46, fr46, ca46, 126, 136, 2, 3, 4, 47);


// ROUTE 47: Z to D
static void rt47(void)
{
    route_request(104);
    util_lock_sw(33, SW_R);
    util_lock_dkw(2, SW_R, SW_R);
    util_lock_sw(3, SW_R);
    // route_request(xxx); No signal defined at Z yet
}

static void fr47(void)
{
    util_unlock_sw(33);
    util_unlock_sw(2);
    util_unlock_sw(3);
}

static void ca47(void)
{
    // route_cancel(xxx); No signal defined at Z yet
    route_cancel(104);
    util_unlock_sw(33);
    util_unlock_sw(2);
    util_unlock_sw(3);
}

ROUTE(47, rt47, fr47, ca47, 104, 136, 2, 3, 4, 8, 30, 46);


// ROUTE 48: F shunting
static void rt48(void)
{
    route_send_sw(118, SW_G);
}

static void fr48(void)
{
    route_send_sw(118, SW_R);   // Only for making Track-Control board happy
    route_send_sw(117, SW_R);
}

ROUTE(48, rt48, fr48, NULL);


/************************************************************************/
/* Routes 101 to 136 reserved for track occupied indicators.            */
/* Track occupied has no constraints and only illuminates track         */
/* occupancy indicators (address 7xx) at activation.                    */
/************************************************************************/

// ROUTE 101: A in use
static void rt101(void)
{
    route_send_sw(723, SW_G);
    route_send_sw(725, SW_G);
    route_send_sw(755, SW_G);
    route_send_sw(749, SW_G);
}

static void ca101(void)
{
    route_send_sw(723, SW_R);
    route_send_sw(725, SW_R);
    route_send_sw(755, SW_R);
    route_send_sw(749, SW_R);
}

ROUTE(101, rt101, NULL, ca101);


// ROUTE 102: B in use
static void rt102(void)
{
    route_send_sw(739, SW_G);
    route_send_sw(742, SW_G);
    route_send_sw(750, SW_G);
}

static void ca102(void)
{
    route_send_sw(739, SW_R);
    route_send_sw(742, SW_R);
    route_send_sw(750, SW_R);
}

ROUTE(102, rt102, NULL, ca102);


// ROUTE 103: C in use
static void rt103(void)
{
    route_send_sw(707, SW_G);
    route_send_sw(708, SW_G);
    route_send_sw(756, SW_G);
    route_send_sw(752, SW_G);
}

static void ca103(void)
{
    route_send_sw(707, SW_R);
    route_send_sw(708, SW_R);
    route_send_sw(756, SW_R);
    route_send_sw(752, SW_R);
}

ROUTE(103, rt103, NULL, ca103);


// ROUTE 104: D in use
static void rt104(void)
{
    route_send_sw(751, SW_G);
    route_send_sw(740, SW_G);
}

static void ca104(void)
{
    route_send_sw(751, SW_R);
    route_send_sw(740, SW_R);
}

ROUTE(104, rt104, NULL, ca104);


// ROUTE 105: E in use
static void rt105(void)
{
    route_send_sw(752, SW_G);
    route_send_sw(756, SW_G);
    route_send_sw(708, SW_G);
    route_send_sw(707, SW_G);
}

static void ca105(void)
{
    route_send_sw(752, SW_R);
    route_send_sw(756, SW_R);
    route_send_sw(708, SW_R);
    route_send_sw(707, SW_R);
}

ROUTE(105, rt105, NULL, ca105);


// ROUTE 106: F in use
static void rt106(void)
{
    route_send_sw(726, SW_G);
    route_send_sw(715, SW_G);
    route_send_sw(713, SW_G);
}

static void ca106(void)
{
    route_send_sw(726, SW_R);
    route_send_sw(715, SW_R);
    route_send_sw(713, SW_R);
}

ROUTE(106, rt106, NULL, ca106);


// ROUTE 107: G in use
static void rt107(void)
{
    route_send_sw(736, SW_G);
    route_send_sw(737, SW_G);
    route_send_sw(712, SW_G);
    route_send_sw(710, SW_G);
}

static void ca107(void)
{
    route_send_sw(736, SW_R);
    route_send_sw(737, SW_R);
    route_send_sw(712, SW_R);
    route_send_sw(710, SW_R);
}

ROUTE(107, rt107, NULL, ca107);


// ROUTE 108: H in use
static void rt108(void)
{
    route_send_sw(738, SW_G);
    route_send_sw(711, SW_G);
}

static void ca108(void)
{
    route_send_sw(738, SW_R);
    route_send_sw(711, SW_R);
}

ROUTE(108, rt108, NULL, ca108);


// ROUTE 109: I in use
static void rt109(void)
{
    route_send_sw(709, SW_G);
    route_send_sw(735, SW_G);
}

static void ca109(void)
{
    route_send_sw(709, SW_R);
    route_send_sw(735, SW_R);
}

ROUTE(109, rt109, NULL, ca109);


// ROUTE 110: J in use
static void rt110(void)
{
    route_send_sw(710, SW_G);
    route_send_sw(712, SW_G);
    route_send_sw(737, SW_G);
    route_send_sw(736, SW_G);
}

static void ca110(void)
{
    route_send_sw(710, SW_R);
    route_send_sw(712, SW_R);
    route_send_sw(737, SW_R);
    route_send_sw(736, SW_R);
}

ROUTE(110, rt110, NULL, ca110);


/*
// ROUTE 111: K in use (unallocated)
static void rt111(void)
{

}

static void ca111(void)
{

}

ROUTE(111, rt111, NULL, ca111);
*/


// ROUTE 112: L in use
static void rt112(void)
{
    route_send_sw(749, SW_G);
    route_send_sw(755, SW_G);
    route_send_sw(725, SW_G);
    route_send_sw(723, SW_G);
}

static void ca112(void)
{
    route_send_sw(749, SW_R);
    route_send_sw(755, SW_R);
    route_send_sw(725, SW_R);
    route_send_sw(723, SW_R);
}

ROUTE(112, rt112, NULL, ca112);


// ROUTE 113: M in use
static void rt113(void)
{
    route_send_sw(724, SW_G);
    route_send_sw(746, SW_G);
    route_send_sw(741, SW_G);
}

static void ca113(void)
{
    route_send_sw(724, SW_R);
    route_send_sw(746, SW_R);
    route_send_sw(741, SW_R);
}

ROUTE(113, rt113, NULL, ca113);


// ROUTE 114: N in use
static void rt114(void)
{
    route_send_sw(701, SW_G);
    route_send_sw(733, SW_G);
}

static void ca114(void)
{
    route_send_sw(701, SW_R);
    route_send_sw(733, SW_R);
}

ROUTE(114, rt114, NULL, ca114);


/*
// ROUTE 115: O in use
static void rt115(void)
{

}

static void ca115(void)
{

}

ROUTE(115, rt115, NULL, ca115);
*/


// ROUTE 116: P in use
static void rt116(void)
{
    route_send_sw(734, SW_G);
    route_send_sw(706, SW_G);
}

static void ca116(void)
{
    route_send_sw(734, SW_R);
    route_send_sw(706, SW_R);
}

ROUTE(116, rt116, NULL, ca116);


// ROUTE 117: Q in use
static void rt117(void)
{
    route_send_sw(722, SW_G);
    route_send_sw(754, SW_G);
}

static void ca117(void)
{
    route_send_sw(722, SW_R);
    route_send_sw(754, SW_R);
}

ROUTE(117, rt117, NULL, ca117);


// ROUTE 118: R in use
static void rt118(void)
{
    route_send_sw(717, SW_G);
}

static void ca118(void)
{
    route_send_sw(717, SW_R);
}

ROUTE(118, rt118, NULL, ca118);


// ROUTE 119: S in use
static void rt119(void)
{
    route_send_sw(718, SW_G);
}

static void ca119(void)
{
    route_send_sw(718, SW_R);
}

ROUTE(119, rt119, NULL, ca119);


// ROUTE 120: T in use
static void rt120(void)
{
    route_send_sw(719, SW_G);
}

static void ca120(void)
{
    route_send_sw(719, SW_R);
}

ROUTE(120, rt120, NULL, ca120);


// ROUTE 121: U in use
static void rt121(void)
{
    route_send_sw(720, SW_G);
}

static void ca121(void)
{
    route_send_sw(720, SW_R);
}

ROUTE(121, rt121, NULL, ca121);


// ROUTE 122: V in use
static void rt122(void)
{
    route_send_sw(721, SW_G);
}

static void ca122(void)
{
    route_send_sw(721, SW_R);
}

ROUTE(122, rt122, NULL, ca122);


// ROUTE 123: W in use
static void rt123(void)
{
    route_send_sw(713, SW_G);
    route_send_sw(715, SW_G);
    route_send_sw(726, SW_G);
}

static void ca123(void)
{
    route_send_sw(713, SW_R);
    route_send_sw(715, SW_R);
    route_send_sw(726, SW_R);
}

ROUTE(123, rt123, NULL, ca123);


// ROUTE 124: X in use
static void rt124(void)
{
    route_send_sw(741, SW_G);
    route_send_sw(746, SW_G);
    route_send_sw(724, SW_G);
}

static void ca124(void)
{
    route_send_sw(741, SW_R);
    route_send_sw(746, SW_R);
    route_send_sw(724, SW_R);
}

ROUTE(124, rt124, NULL, ca124);


/* Y NOT DEFINED YET
// ROUTE 125: Y in use
static void rt125(void)
{

}

static void ca125(void)
{

}

ROUTE(125, rt125, NULL, ca125);
*/


// ROUTE 126: Z in use
static void rt126(void)
{
    route_send_sw(702, SW_G);
}

static void ca126(void)
{
    route_send_sw(702, SW_R);
}

ROUTE(126, rt126, NULL, ca126);


// ROUTE 127: Special G-inner occupied. Only used at init for train auto detect
ROUTE(127, NULL, NULL, NULL);

// ROUTE 128: Special J-inner occupied. Only used at init for train auto detect
ROUTE(128, NULL, NULL, NULL);

// ROUTE 129: Special A-inner occupied. Only used at init for train auto detect
ROUTE(129, NULL, NULL, NULL);

// ROUTE 130: Special C-inner occupied. Only used at init for train auto detect
ROUTE(130, NULL, NULL, NULL);

// ROUTE 131: Special L-inner occupied. Only used at init for train auto detect
ROUTE(131, NULL, NULL, NULL);

// ROUTE 132: Special M-inner occupied. Only used at init for train auto detect
ROUTE(132, NULL, NULL, NULL);

// ROUTE 133: Special E-inner occupied. Only used at init for train auto detect
ROUTE(133, NULL, NULL, NULL);

// ROUTE 134: Special F-inner occupied. Only used at init for train auto detect
ROUTE(134, NULL, NULL, NULL);


// ROUTE 135: ExA occupied. East going train between w23 and w37
static void fr135(void)
{
    route_send_fb(337, FB_FREE);
}

ROUTE(135, NULL, fr135, NULL);


// ROUTE 136: ExB occupied. South going train between w2 and w33
static void fr136(void)
{
    route_send_fb(333, FB_FREE);
}

ROUTE(136, NULL, fr136, NULL);


/************************************************************************/
/* Routes 150 to 183 controls correct switching of complex signals      */
/************************************************************************/

// ROUTE 150: Signal 123
static void rt150(void)
{
    route_send_sw(123, SW_G);
}

static void frca150(void)
{
    route_send_sw_prio(123, SW_R);      // Priority for looks
}

ROUTE(150, rt150, frca150, frca150);


// ROUTE 151: Signal 125 E
// ROUTE 152: Signal 125 N
static void rt151152(void)
{
    route_request(153);
}

static void fr151152(void)
{
    route_send_sw_prio(126, SW_R);      // Priority for looks
}

static void ca151152(void)
{
    route_cancel(153);
}

ROUTE(151, rt151152, fr151152, ca151152);
ROUTE(152, rt151152, fr151152, ca151152);


// ROUTE 153: Signal 125 common
static void rt153(void)
{
    route_send_sw(125, SW_G);
}

static void fr153(void)
{
    route_kill(151);
    route_kill(152);
    route_send_sw_prio(125, SW_R);      // Priority for looks
}

static void ca153(void)
{
    route_send_sw_prio(125, SW_R);      // Priority for looks
}

ROUTE(153, rt153, fr153, ca153);


// ROUTE 154: Signal 127 common
static void rt154(void)
{
    route_send_sw(127, SW_G);
}

static void fr154(void)
{
    route_kill(155);
    route_kill(156);
    route_send_sw_prio(127, SW_R);      // Priority for looks
}

static void ca154(void)
{
    route_send_sw_prio(127, SW_R);      // Priority for looks
}

ROUTE(154, rt154, fr154, ca154);


// ROUTE 155: Signal 127 D
// ROUTE 156: Signal 127 L
static void rt155156(void)
{
    route_request(154);
}

static void fr155156(void)
{
    route_send_sw_prio(128, SW_R);      // Priority for looks
}

static void ca155156(void)
{
    route_cancel(154);
}

ROUTE(155, rt155156, fr155156, ca155156);
ROUTE(156, rt155156, fr155156, ca155156);


// ROUTE 157: Signal 107 common
static void rt157(void)
{
    route_send_sw(107, SW_G);
}

static void fr157(void)
{
    route_kill(158);
    route_kill(159);
    route_send_sw_prio(107, SW_R);      // Priority for looks
}

static void ca157(void)
{
    route_send_sw_prio(107, SW_R);      // Priority for looks
}

ROUTE(157, rt157, fr157, ca157);


// ROUTE 158: Signal 107 G
// ROUTE 159: Signal 107 H
static void rt158159(void)
{
    route_request(157);
}

static void fr158159(void)
{
    route_send_sw_prio(108, SW_R);      // Priority for looks
}

static void ca158159(void)
{
    route_cancel(157);
}

ROUTE(158, rt158159, fr158159, ca158159);
ROUTE(159, rt158159, fr158159, ca158159);


// ROUTE 160: Signal 115 common
static void rt160(void)
{
    route_send_sw(115, SW_G);
}

static void fr160(void)
{
    route_kill(161);
    route_kill(162);
    route_send_sw_prio(115, SW_R);      // Priority for looks
}

static void ca160(void)
{
    route_send_sw_prio(115, SW_R);      // Priority for looks
}

ROUTE(160, rt160, fr160, ca160);


// ROUTE 161: Signal 115 I
// ROUTE 162: Signal 115 J
static void rt161162(void)
{
    route_request(160);
}

static void fr161162(void)
{
    route_send_sw_prio(116, SW_R);      // Priority for looks
}

static void ca161162(void)
{
    route_cancel(160);
}

ROUTE(161, rt161162, fr161162, ca161162);
ROUTE(162, rt161162, fr161162, ca161162);


// ROUTE 163: Signal 117 common
static void rt163(void)
{
    route_send_sw(117, SW_G);
}

static void fr163(void)
{
    route_kill(164);
    route_send_sw_prio(117, SW_R);      // Priority for looks
}

static void ca163(void)
{
    route_send_sw_prio(117, SW_R);      // Priority for looks
}

ROUTE(163, rt163, fr163, ca163);


// ROUTE 164: Signal 117 J
static void rt164(void)
{
    route_request(163);
}

static void fr164(void)
{
    route_send_sw_prio(118, SW_R);      // Priority for looks
}

static void ca164(void)
{
    route_cancel(163);
}

ROUTE(164, rt164, fr164, ca164);


// ROUTE 165: Signal 111 common
static void rt165(void)
{
    route_send_sw(111, SW_G);
}

static void fr165(void)
{
    route_kill(166);
    route_kill(167);
    route_send_sw_prio(111, SW_R);      // Priority for looks
}

static void ca165(void)
{
    route_send_sw_prio(111, SW_R);      // Priority for looks
}

ROUTE(165, rt165, fr165, ca165);


// ROUTE 166: Signal 111 C
// ROUTE 167: Signal 111 W
static void rt166167(void)
{
    route_request(165);
}

static void fr166167(void)
{
    route_send_sw_prio(112, SW_R);      // Priority for looks
}

static void ca166167(void)
{
    route_cancel(165);
}

ROUTE(166, rt166167, fr166167, ca166167);
ROUTE(167, rt166167, fr166167, ca166167);


// ROUTE 168: Signal 113 common
static void rt168(void)
{
    route_send_sw(113, SW_G);
}

static void fr168(void)
{
    route_kill(169);
    route_kill(170);
    route_send_sw_prio(113, SW_R);      // Priority for looks
}

static void ca168(void)
{
    route_send_sw_prio(113, SW_R);      // Priority for looks
}

ROUTE(168, rt168, fr168, ca168);


// ROUTE 169: Signal 113 C
// ROUTE 170: Signal 113 W
static void rt169170(void)
{
    route_request(168);
}

static void fr169170(void)
{
    route_send_sw_prio(114, SW_R);      // Priority for looks
}

static void ca169170(void)
{
    route_cancel(168);
}

ROUTE(169, rt169170, fr169170, ca169170);
ROUTE(170, rt169170, fr169170, ca169170);


// ROUTE 171: Signal 101 common
static void rt171(void)
{
    route_send_sw(101, SW_G);
}

static void fr171(void)
{
    route_kill(172);
    route_send_sw_prio(101, SW_R);      // Priority for looks
}

static void ca171(void)
{
    route_send_sw_prio(101, SW_R);      // Priority for looks
}

ROUTE(171, rt171, fr171, ca171);


// ROUTE 172: Signal 101 B
static void rt172(void)
{
    route_request(171);
}

static void fr172(void)
{
    route_send_sw_prio(102, SW_R);      // Priority for looks
}

static void ca172(void)
{
    route_cancel(171);
}

ROUTE(172, rt172, fr172, ca172);


// ROUTE 173: Signal 103 common
static void rt173(void)
{
    route_send_sw(103, SW_G);
}

static void fr173(void)
{
    route_kill(174);
    route_send_sw_prio(103, SW_R);      // Priority for looks
}

static void ca173(void)
{
    route_send_sw_prio(103, SW_R);      // Priority for looks
}

ROUTE(173, rt173, fr173, ca173);


// ROUTE 174: Signal 103 B
static void rt174(void)
{
    route_request(173);
}

static void fr174(void)
{
    route_send_sw_prio(104, SW_R);      // Priority for looks
}

static void ca174(void)
{
    route_cancel(173);
}

ROUTE(174, rt174, fr174, ca174);


// ROUTE 175: Signal 124
static void rt175(void)
{
    route_send_sw(124, SW_G);
}

static void frca175(void)
{
    route_send_sw_prio(124, SW_R);      // Priority for looks
}

ROUTE(175, rt175, frca175, frca175);


// ROUTE 176: Signal 109 common
static void rt176(void)
{
    route_send_sw(109, SW_G);
}

static void fr176(void)
{
    route_kill(177);
    route_kill(178);
    route_send_sw_prio(109, SW_R);      // Priority for looks
}

static void ca176(void)
{
    route_send_sw_prio(109, SW_R);      // Priority for looks
}

ROUTE(176, rt176, fr176, ca176);


// ROUTE 177: Signal 109 A+Q
// ROUTE 178: Signal 109 P
static void rt177178(void)
{
    route_request(176);
}

static void fr177178(void)
{
    route_send_sw_prio(110, SW_R);      // Priority for looks
}

static void ca177178(void)
{
    route_cancel(176);
}

ROUTE(177, rt177178, fr177178, ca177178);
ROUTE(178, rt177178, fr177178, ca177178);


// ROUTE 179: Signal 131 common
static void rt179(void)
{
    route_send_sw(131, SW_G);
}

static void fr179(void)
{
    route_kill(180);
    route_kill(181);
    route_send_sw_prio(131, SW_R);      // Priority for looks
}

static void ca179(void)
{
    route_send_sw_prio(131, SW_R);      // Priority for looks
}

ROUTE(179, rt179, fr179, ca179);


// ROUTE 180: Signal 131 A+Q
// ROUTE 181: Signal 131 X
static void rt180181(void)
{
    route_request(179);
}

static void fr180181(void)
{
    route_send_sw_prio(132, SW_R);      // Priority for looks
}

static void ca180181(void)
{
    route_cancel(179);
}

ROUTE(180, rt180181, fr180181, ca180181);
ROUTE(181, rt180181, fr180181, ca180181);


// ROUTE 182: Signal 129 common
static void rt182(void)
{
    route_send_sw(129, SW_G);
}

static void fr182(void)
{
    route_kill(183);
    route_send_sw_prio(129, SW_R);      // Priority for looks
}

static void ca182(void)
{
    route_send_sw_prio(129, SW_R);      // Priority for looks
}

ROUTE(182, rt182, fr182, ca182);


// ROUTE 183: Signal 129 D
static void rt183(void)
{
    route_request(182);
}

static void fr183(void)
{
    route_send_sw_prio(130, SW_R);      // Priority for looks
}

static void ca183(void)
{
    route_cancel(182);
}

ROUTE(183, rt183, fr183, ca183);
