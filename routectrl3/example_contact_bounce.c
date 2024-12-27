/*
 * example_contact_bounce.c
 *
 * Created: 26-12-2024 15:46:07
 *  Author: Mikael Ejberg Pedersen
 */

#include "fb_handler.h"
#include "route.h"
#include "route_delay.h"


/*
 * Time (in seconds) for debouncing feedback free reports.
 */
#define DEBOUNCE_TIME 2


/*
 * Offset for converting feedback addresses to route numbers.
 */
#define ROUTE_OFFSET 100


/*
 * Called when any feedback in the wanted range gets occupied.
 */
static void feedback_debounce_occupied(uint16_t adr)
{
    routenum_t routenum = adr + ROUTE_OFFSET;

    // Cancel previous free delay (if it exists).
    route_delay_cancel(routenum);

    // Request route immediately.
    // Occupied signals aren't debounced, but requesting a route multiple times doesn't do anything.
    route_request(routenum);
}

/*
 * Called DEBOUNCE_TIME seconds after the last feedback free message.
 */
static void debounce_free_done(routenum_t routenum)
{
    // Free route.
    route_free(routenum);
}

/*
 * Called when any feedback in the wanted range gets freed.
 */
static void feedback_debounce_free(uint16_t adr)
{
    routenum_t routenum = adr + ROUTE_OFFSET;

    // Cancel previous free delay (if it exists).
    route_delay_cancel(routenum);

    // Start delay.
    route_delay_add(DEBOUNCE_TIME, debounce_free_done, routenum);
}

/*
 * Set the feedback range that will be debounced (1 to 8).
 */
FEEDBACK_RANGE_OCC(1, 8, feedback_debounce_occupied);
FEEDBACK_RANGE_FREE(1, 8, feedback_debounce_free);


/*
 * Define routes that'll act as debounced feedback.
 * You can create functions for route activation (feedback occupied)
 * and route free (feedback free).
 * These will be called once only, no matter how much contact bounce
 * there is in the feedback signals.
 *
 * Or you can skip the functions (use NULL instead), and just use the
 * routes as constraints for other routes.
 *
 * In this example route 101 and 102 have functions, and 103-108 doesn't.
 */

// ROUTE 101: Debounced feedback 1
static void activate101(void)
{
    // Do what you want here when feedback 1 is occupied
}

static void free101(void)
{
    // Do what you want here when feedback 1 is free
}

ROUTE(101, activate101, free101, NULL);


// ROUTE 102: Debounced feedback 2
static void activate102(void)
{
    // Do what you want here when feedback 2 is occupied
}

static void free102(void)
{
    // Do what you want here when feedback 2 is free
}

ROUTE(102, activate102, free102, NULL);


ROUTE(103, NULL, NULL, NULL);
ROUTE(104, NULL, NULL, NULL);
ROUTE(105, NULL, NULL, NULL);
ROUTE(106, NULL, NULL, NULL);
ROUTE(107, NULL, NULL, NULL);
ROUTE(108, NULL, NULL, NULL);
