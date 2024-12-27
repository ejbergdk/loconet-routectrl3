/*
 * route.h
 *
 * Created: 13-01-2024 16:22:19
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef ROUTE_H_
#define ROUTE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "flashmem.h"
#include "timer.h"


#ifndef MAXROUTES
#define MAXROUTES 200
#endif

#if MAXROUTES <= 256
typedef uint8_t routenum_t;
#else
typedef uint16_t routenum_t;
#endif

typedef void    (*route_cb_t)(void);

typedef struct
{
    const routenum_t routenum;
    const size_t    constraint_cnt;
    const FLASHMEM uint16_t *constraint;
    const route_cb_t activateroute;
    const route_cb_t freeroute;
    const route_cb_t cancelroute;
} route_table_t;

/**
 * Route creation macro.
 *
 * Creates a new route.
 * When route is requested, it is queued up and will automatically activate
 * when all constraints are satisfied (all are free).
 *
 * @param num Route number. Must be unique and from 0 to MAXROUTES-1.
 * @param act Route activation function callback. NULL if not used.
 * @param fre Route free function callback. NULL if not used.
 * @param can Route cancel function callback. NULL if not used.
 * @param ... List of route constraints. Can be omitted.
 */
#define ROUTE(num, act, fre, can, ...) \
static const FLASHMEM uint16_t routecstrs##num[] = { __VA_ARGS__ }; \
static const route_table_t routeentry##num \
__attribute__((used, section("loconet.routetable"))) = { \
    .routenum = num, \
    .constraint_cnt = sizeof(routecstrs##num) / sizeof(routecstrs##num[0]), \
    .constraint = routecstrs##num, \
    .activateroute = act, \
    .freeroute = fre, \
    .cancelroute = can \
};

#define ROUTE_CSTR_DATA_MASK 0x0fff
#define ROUTE_CSTR_TYPE_MASK 0xf000
#define ROUTE_CSTR_TYPE_RT   0x0000
#define ROUTE_CSTR_TYPE_FB   0x1000

/**
 * Route constraint (not really needed)
 */
#define CSTR_RT(num) num

/**
 * Feedback constraint
 */
#define CSTR_FB(num) (num | ROUTE_CSTR_TYPE_FB)

typedef enum
{
    ROUTE_FREE = 0,
    ROUTE_AWAITCSTR,
    ROUTE_AWAITEXE,
    ROUTE_ACTIVE
} route_state_t;


/*
 * Init route module.
 *
 * Call once at startup.
 */
extern void     route_init(void);

/*
 * Update route module.
 *
 * Call regularly from mainloop.
 */
extern void     route_update(void);

/*
 * Request route.
 *
 * Route request is queued, and it will activate as soon as
 * all route constraints are free.
 *
 * @param num Route number to request.
 */
extern void     route_request(routenum_t num);

/*
 * Free route.
 *
 * Frees an active route.
 *
 * @param num Route number to free.
 */
extern void     route_free(routenum_t num);

/*
 * Cancel route.
 *
 * Cancels a route waiting to be activated.
 *
 * @param num Route number to cancel.
 */
extern void     route_cancel(routenum_t num);

/*
 * Force activate route.
 *
 * Mark route as active, without running activation callback or checking constraints.
 * Usually used during init while populating track occupancy feedback.
 *
 * @param num Route number to force activate.
 */
extern void     route_forceactive(routenum_t num);

/*
 * Kill route.
 *
 * Kills route (force free) without running free callback.
 *
 * @param num Route number to kill.
 */
extern void     route_kill(routenum_t num);

/*
 * Get route state.
 *
 * @param num Route number to get state for.
 * @return Route state.
 */
extern route_state_t route_state(routenum_t num);

/*
 * Get route existential info.
 *
 * @param num Route number to check.
 * @return true if route number is defined. False if not.
 */
extern bool     route_exists(routenum_t num);

#define SW_R false
#define SW_G true
#define FB_FREE false
#define FB_OCCUPIED true

/*
 * Send switch command.
 *
 * Queue up a switch command for sending.
 * Queue is shared between switch cmds and feedback reports.
 *
 * @param adr Switch address.
 * @param opt Switch direction (SW_R / SW_G).
 */
extern void     route_send_sw(uint16_t adr, bool opt);

/*
 * Send prioritized switch command.
 *
 * Send a switch command before other commands already in queue.
 *
 * @param adr Switch address.
 * @param opt Switch direction (SW_R / SW_G).
 */
extern void     route_send_sw_prio(uint16_t adr, bool opt);

/*
 * Send feedback report.
 *
 * Queue up a feedback report for sending.
 * Queue is shared between switch cmds and feedback reports.
 *
 * @param adr Feedback address.
 * @param opt Feedback info (FB_FREE / FB_OCCUPIED).
 */
extern void     route_send_fb(uint16_t adr, bool opt);

/*
 * Send prioritized feedback report.
 *
 * Send a feedback report before other commands already in queue.
 *
 * @param adr Feedback address.
 * @param opt Feedback info (FB_FREE / FB_OCCUPIED).
 */
extern void     route_send_fb_prio(uint16_t adr, bool opt);

#endif /* ROUTE_H_ */
