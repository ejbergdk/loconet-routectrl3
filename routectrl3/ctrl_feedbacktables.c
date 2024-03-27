/*
 * ctrl_feedbacktables.c
 *
 * Created: 04-04-2024 15:55:40
 *  Author: Mikael Ejberg Pedersen
 */

#include <stdint.h>
#include "ctrl_init.h"
#include "ctrl_util.h"
#include "fb_handler.h"
#include "mmi.h"
#include "route.h"


/*
 * Routes can be activated externally from a Track-Control panel.
 * This is done with feedback addresses 401 to 449 (route 1 to 49).
 */
static void activate_route(uint16_t num)
{
    route_request(num - 400);
}

FEEDBACK_RANGE_OCC(401, 449, activate_route);


/************************************************************************/
/* Normal feedback handling                                             */
/************************************************************************/

static void k1o(uint16_t num)
{
    route_send_sw(701, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(1, k1o);


static void k1f(uint16_t num)
{
    route_free(136);            // Free ExB
}

FEEDBACK_FREE(1, k1f);


static void k2o(uint16_t num)
{
    route_send_sw(702, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(2, k2o);


static void k3o(uint16_t num)
{
    route_send_sw(703, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(3, k3o);


static void k4o(uint16_t num)
{
    route_send_sw(704, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(4, k4o);


static void k5o(uint16_t num)
{
    route_send_sw(705, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(5, k5o);


static void k6o(uint16_t num)
{
    route_send_sw(706, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(116);
    }
}

FEEDBACK_OCC(6, k6o);


static void k6f(uint16_t num)
{
    route_free(116);
    route_free(182);
}

FEEDBACK_FREE(6, k6f);


static void k7o(uint16_t num)
{
    route_free(166);
    route_free(169);
    route_send_sw(707, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(7, k7o);


static void k7f(uint16_t num)
{
    route_send_sw(119, SW_R);   // Set SU signal red
    route_free(13);
    route_free(16);
    route_free(105);
    route_free(160);
}

FEEDBACK_FREE(7, k7f);


static void k8o(uint16_t num)
{
    route_send_sw_prio(120, SW_R);      // Set SU signal red+on (priority for looks)
    route_send_sw(708, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(133);
    }
}

FEEDBACK_OCC(8, k8o);


static void k8f(uint16_t num)
{
    route_free(133);
}

FEEDBACK_FREE(8, k8f);


static void k9o(uint16_t num)
{
    route_free(161);
    route_send_sw(709, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(9, k9o);


static void k9f(uint16_t num)
{
    route_free(9);
}

FEEDBACK_FREE(9, k9f);


static void k10o(uint16_t num)
{
    route_free(162);
    route_free(164);
    route_send_sw(710, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(10, k10o);


static void k10f(uint16_t num)
{
    route_free(10);
    route_free(12);
    route_free(107);
    route_free(165);
}

FEEDBACK_FREE(10, k10f);


static void k11o(uint16_t num)
{
    route_send_sw(711, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(108);
    }
}

FEEDBACK_OCC(11, k11o);


static void k11f(uint16_t num)
{
    route_free(108);
    route_free(168);
    util_unlock_sw(19);         // Unlock turnout (special handling of w19 as it is inside track H holding area)
}

FEEDBACK_FREE(11, k11f);


static void k12o(uint16_t num)
{
    route_send_sw(712, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(127); // Does this work ???
    }
}

FEEDBACK_OCC(12, k12o);


static void k12f(uint16_t num)
{
    route_free(127);
}

FEEDBACK_FREE(12, k12f);


static void k13o(uint16_t num)
{
    route_free(167);
    route_free(170);
    route_send_sw(713, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(13, k13o);


static void k13f(uint16_t num)
{
    route_send_sw(121, SW_R);   // Set SU signal red
    route_free(14);
    route_free(17);
    route_free(106);
    route_free(163);
}

FEEDBACK_FREE(13, k13f);


static void k14o(uint16_t num)
{
    route_send_sw(714, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(14, k14o);


static void k15o(uint16_t num)
{
    route_send_sw_prio(122, SW_R);      // Set SU signal red+on (priority for looks)
    route_send_sw(715, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(134); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(15, k15o);


static void k15f(uint16_t num)
{
    route_free(134);
}

FEEDBACK_FREE(15, k15f);


static void k16o(uint16_t num)
{
    route_free(183);
    route_send_sw(716, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(16, k16o);


static void k17o(uint16_t num)
{
    route_free(31);
    route_send_sw(717, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(118); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(17, k17o);


static void k17f(uint16_t num)
{
    route_free(118);
}

FEEDBACK_FREE(17, k17f);


static void k18o(uint16_t num)
{
    route_free(32);
    route_send_sw(718, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(119); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(18, k18o);


static void k18f(uint16_t num)
{
    route_free(119);
}

FEEDBACK_FREE(18, k18f);


static void k19o(uint16_t num)
{
    route_free(33);
    route_send_sw(719, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(120); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(19, k19o);


static void k19f(uint16_t num)
{
    route_free(120);
}

FEEDBACK_FREE(19, k19f);


static void k20o(uint16_t num)
{
    route_free(34);
    route_send_sw(720, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(121); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(20, k20o);


static void k20f(uint16_t num)
{
    route_free(121);
}

FEEDBACK_FREE(20, k20f);


static void k21o(uint16_t num)
{
    route_free(35);
    route_send_sw(721, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(122); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(21, k21o);


static void k21f(uint16_t num)
{
    route_free(122);
}

FEEDBACK_FREE(21, k21f);


static void k22o(uint16_t num)
{
    route_send_sw(722, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(22, k22o);


static void k22f(uint16_t num)
{
    route_free(26);
    route_free(28);
}

FEEDBACK_FREE(22, k22f);


static void k23o(uint16_t num)
{
    route_send_sw(723, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(112); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(23, k23o);


static void k23f(uint16_t num)
{
    route_free(24);
    route_free(27);
    route_free(112);
    route_free(175);
}

FEEDBACK_FREE(23, k23f);


static void k24o(uint16_t num)
{
    route_send_sw(134, SW_R);
    route_send_sw(724, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(124); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(24, k24o);


static void k24f(uint16_t num)
{
    route_free(41);
    route_free(42);
    route_free(124);
}

FEEDBACK_FREE(24, k24f);


static void k25o(uint16_t num)
{
    route_send_sw(725, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(131); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(25, k25o);


static void k25f(uint16_t num)
{
    route_free(131);
}

FEEDBACK_FREE(25, k25f);


// Common function for feedback 26 to 32, as they are very similar
static void ko_track_reserved_off_only(uint16_t num)
{
    route_send_sw(num + 700, SW_R);     // Turn off track reserved yellow light
}

FEEDBACK_RANGE_OCC(26, 32, ko_track_reserved_off_only);


static void k33o(uint16_t num)
{
    route_send_sw(733, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(114); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(33, k33o);


static void k33f(uint16_t num)
{
    route_free(114);
    route_free(179);
}

FEEDBACK_FREE(33, k33f);


static void k34o(uint16_t num)
{
    route_send_sw(734, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(34, k34o);


static void k34f(uint16_t num)
{
    route_free(22);
    route_free(25);
    route_free(36);
    route_free(37);
    route_free(38);
    route_free(39);
    route_free(40);
}

FEEDBACK_FREE(34, k34f);


static void k35o(uint16_t num)
{
    route_send_sw(735, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(109); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(35, k35o);


static void k35f(uint16_t num)
{
    route_free(109);
    route_free(171);
}

FEEDBACK_FREE(35, k35f);


static void k36o(uint16_t num)
{
    route_free(158);
    route_send_sw(736, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(36, k36o);


static void k36f(uint16_t num)
{
    route_free(6);
    route_free(45);
    route_free(110);
    route_free(173);
}

FEEDBACK_FREE(36, k36f);


static void k37o(uint16_t num)
{
    route_send_sw(737, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(128); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(37, k37o);


static void k37f(uint16_t num)
{
    route_free(128);
}

FEEDBACK_FREE(37, k37f);


static void k38o(uint16_t num)
{
    route_free(159);
    route_send_sw(738, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(38, k38o);


static void k38f(uint16_t num)
{
    route_free(7);
}

FEEDBACK_FREE(38, k38f);


static void k39o(uint16_t num)
{
    route_free(172);
    route_free(174);
    route_send_sw(739, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(39, k39o);


static void k39f(uint16_t num)
{
    route_free(19);
    route_free(21);
    route_free(43);
    route_free(44);
}

FEEDBACK_FREE(39, k39f);


static void k40o(uint16_t num)
{
    route_send_sw(740, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(104); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(40, k40o);


static void k40f(uint16_t num)
{
    route_free(104);
    route_free(157);
}

FEEDBACK_FREE(40, k40f);


static void k41o(uint16_t num)
{
    route_send_sw(741, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(113); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(41, k41o);


static void k41f(uint16_t num)
{
    route_free(23);
    route_free(29);
    route_free(113);
    route_free(176);
}

FEEDBACK_FREE(41, k41f);


static void k42o(uint16_t num)
{
    route_send_sw_prio(106, SW_R);      // Set SU signal red+on (priority for looks)
    route_send_sw(742, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(42, k42o);


static void k42f(uint16_t num)
{
    route_send_sw_prio(105, SW_R);      // Set SU signal on red
    route_free(135);
}

FEEDBACK_FREE(42, k42f);


static void k43o(uint16_t num)
{
    route_free(178);
    route_free(181);
    route_send_sw(743, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(43, k43o);


static void k44o(uint16_t num)
{
    route_free(180);
    route_send_sw(744, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(44, k44o);


static void k45o(uint16_t num)
{
    route_free(177);
    route_send_sw(745, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(45, k45o);


static void k46o(uint16_t num)
{
    route_send_sw(746, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(132); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(46, k46o);


static void k46f(uint16_t num)
{
    route_free(132);
}

FEEDBACK_FREE(46, k46f);


static void k47o(uint16_t num)
{
    route_send_sw(747, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(47, k47o);


static void k48o(uint16_t num)
{
    route_send_sw(748, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(48, k48o);


static void k49o(uint16_t num)
{
    route_free(156);
    route_send_sw(749, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(101); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(49, k49o);


static void k49f(uint16_t num)
{
    route_free(5);
    route_free(101);
    route_free(150);
}

FEEDBACK_FREE(49, k49f);


static void k50o(uint16_t num)
{
    route_send_sw(750, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(102); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(50, k50o);


static void k50f(uint16_t num)
{
    route_free(102);
    route_free(153);
}

FEEDBACK_FREE(50, k50f);


static void k51o(uint16_t num)
{
    route_free(155);
    route_send_sw(751, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(51, k51o);


static void k51f(uint16_t num)
{
    route_free(4);
    route_free(30);
    route_free(47);
}

FEEDBACK_FREE(51, k51f);


static void k52o(uint16_t num)
{
    route_free(151);
    route_send_sw(752, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(103); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(52, k52o);


static void k52f(uint16_t num)
{
    route_free(1);
    route_free(2);
    route_free(103);
    route_free(154);
}

FEEDBACK_FREE(52, k52f);


static void k53o(uint16_t num)
{
    route_free(152);
    route_send_sw(753, SW_R);   // Turn off track reserved yellow light
}

FEEDBACK_OCC(53, k53o);


static void k53f(uint16_t num)
{
    route_free(3);
    route_free(46);
}

FEEDBACK_FREE(53, k53f);


static void k54o(uint16_t num)
{
    route_send_sw(754, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(117); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(54, k54o);


static void k54f(uint16_t num)
{
    route_free(4);
    route_free(117);
}

FEEDBACK_FREE(54, k54f);


static void k55o(uint16_t num)
{
    route_send_sw(755, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(129); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(55, k55o);


static void k55f(uint16_t num)
{
    route_free(129);
}

FEEDBACK_FREE(55, k55f);


static void k56o(uint16_t num)
{
    route_send_sw(756, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(130); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(56, k56o);


static void k56f(uint16_t num)
{
    route_free(130);
}

FEEDBACK_FREE(56, k56f);


static void k57o(uint16_t num)
{
    route_send_sw(136, SW_R);
    route_send_sw(757, SW_R);   // Turn off track reserved yellow light

#ifndef AUTO_OCCUPY
    if (!ctrl_init_done())
#endif
    {                           // if (not init_done) or AUTO_OCCUPY is defined
        route_forceactive(123); // Force route active, as we already have a train there
    }
}

FEEDBACK_OCC(57, k57o);


static void k57f(uint16_t num)
{
    route_free(123);
}

FEEDBACK_FREE(57, k57f);


// Common function for feedback 58 to 64, as they are very similar
FEEDBACK_RANGE_OCC(58, 64, ko_track_reserved_off_only);


// Strange and undocumented FB actions found in the old route controller 2.4
// Seems to be unreferenced

#if 0
static void kf_freeroutes_400(uint16_t num)
{
    route_free(num - 400);
}

FEEDBACK_FREE(8, kf_freeroutes_400);
FEEDBACK_FREE(11, kf_freeroutes_400);
FEEDBACK_FREE(15, kf_freeroutes_400);
FEEDBACK_FREE(18, kf_freeroutes_400);
FEEDBACK_FREE(20, kf_freeroutes_400);
#endif



static void cancel_routes_from_a(uint16_t num)
{
    route_cancel(1);
}

static void cancel_routes_from_b(uint16_t num)
{
    route_cancel(2);
    route_cancel(3);
    route_cancel(46);
}

static void cancel_routes_from_c(uint16_t num)
{
    route_cancel(4);
    route_cancel(5);
}

static void cancel_routes_from_d(uint16_t num)
{
    route_cancel(6);
    route_cancel(7);
    route_cancel(8);
}

static void cancel_routes_from_e(uint16_t num)
{
    route_cancel(9);
    route_cancel(10);
    route_cancel(11);
}

static void cancel_routes_from_f(uint16_t num)
{
    route_cancel(12);
    route_cancel(48);
}

static void cancel_routes_from_g(uint16_t num)
{
    route_cancel(13);
    route_cancel(14);
    route_cancel(15);
}

static void cancel_routes_from_h(uint16_t num)
{
    route_cancel(16);
    route_cancel(17);
    route_cancel(18);
}

static void cancel_routes_from_i(uint16_t num)
{
    route_cancel(19);
    route_cancel(20);
    route_cancel(43);
}

static void cancel_routes_from_j(uint16_t num)
{
    route_cancel(15);
    route_cancel(21);
    route_cancel(44);
}

// K not in use

static void cancel_routes_from_l(uint16_t num)
{
    route_cancel(22);
    route_cancel(23);
}

static void cancel_routes_from_m(uint16_t num)
{
    route_cancel(24);
    route_cancel(25);
    route_cancel(26);
}

static void cancel_routes_from_n(uint16_t num)
{
    route_cancel(27);
    route_cancel(28);
    route_cancel(29);
}

// O not in use

static void cancel_routes_from_p(uint16_t num)
{
    route_cancel(30);
}

static void cancel_routes_from_q(uint16_t num)
{
    route_cancel(31);
    route_cancel(32);
    route_cancel(33);
    route_cancel(34);
    route_cancel(35);
}

static void cancel_routes_from_r(uint16_t num)
{
    route_cancel(36);
}

static void cancel_routes_from_s(uint16_t num)
{
    route_cancel(37);
}

static void cancel_routes_from_t(uint16_t num)
{
    route_cancel(38);
}

static void cancel_routes_from_u(uint16_t num)
{
    route_cancel(39);
}

static void cancel_routes_from_v(uint16_t num)
{
    route_cancel(40);
}

static void cancel_routes_from_w(uint16_t num)
{
    route_cancel(41);
}

static void cancel_routes_from_x(uint16_t num)
{
    route_cancel(42);
}

static void cancel_routes_from_y(uint16_t num)
{
    route_cancel(45);
    route_free(135);            // Free ExA (manual free after train has arrived at Y without feedback)
}

static void cancel_routes_from_z(uint16_t num)
{
    route_cancel(47);
    route_free(136);            // Free ExB (manual free after train has arrived at Y without feedback)
}

FEEDBACK_OCC(501, cancel_routes_from_a);
FEEDBACK_OCC(502, cancel_routes_from_b);
FEEDBACK_OCC(503, cancel_routes_from_c);
FEEDBACK_OCC(504, cancel_routes_from_d);
FEEDBACK_OCC(505, cancel_routes_from_e);
FEEDBACK_OCC(506, cancel_routes_from_f);
FEEDBACK_OCC(507, cancel_routes_from_g);
FEEDBACK_OCC(508, cancel_routes_from_h);
FEEDBACK_OCC(509, cancel_routes_from_i);
FEEDBACK_OCC(510, cancel_routes_from_j);
//FEEDBACK_OCC(511, cancel_routes_from_k);
FEEDBACK_OCC(512, cancel_routes_from_l);
FEEDBACK_OCC(513, cancel_routes_from_m);
FEEDBACK_OCC(514, cancel_routes_from_n);
//FEEDBACK_OCC(515, cancel_routes_from_o);
FEEDBACK_OCC(516, cancel_routes_from_p);
FEEDBACK_OCC(517, cancel_routes_from_q);
FEEDBACK_OCC(518, cancel_routes_from_r);
FEEDBACK_OCC(519, cancel_routes_from_s);
FEEDBACK_OCC(520, cancel_routes_from_t);
FEEDBACK_OCC(521, cancel_routes_from_u);
FEEDBACK_OCC(522, cancel_routes_from_v);
FEEDBACK_OCC(523, cancel_routes_from_w);
FEEDBACK_OCC(524, cancel_routes_from_x);
FEEDBACK_OCC(525, cancel_routes_from_y);
FEEDBACK_OCC(526, cancel_routes_from_z);
