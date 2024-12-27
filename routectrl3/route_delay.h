/*
 * route_delay.h
 *
 * Created: 27-12-2024 12:56:53
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef ROUTE_DELAY_H_
#define ROUTE_DELAY_H_

#include <stdint.h>
#include "route.h"

/*
 * Route delay callback function prototype.
 */
typedef void    (route_delay_cb) (routenum_t);

/*
 * Add a delayed execution.
 *
 * @param timeout   Delay time in seconds.
 * @param cb        Delay timeout function callback.
 * @param num       Route number. Used as reference for delay cancel.
 */
extern void     route_delay_add(uint16_t timeout, route_delay_cb *cb, routenum_t num);

/*
 * Cancel a delay.
 *
 * @param num Route number to cancel delay for.
 */
extern void     route_delay_cancel(routenum_t num);

/*
 * Update route delay module.
 *
 * Call regularly from mainloop.
 */
extern void     route_delay_update(void);

#endif /* ROUTE_DELAY_H_ */
