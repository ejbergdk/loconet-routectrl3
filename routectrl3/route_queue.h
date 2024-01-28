/*
 * route_queue.h
 *
 * Created: 15-01-2024 16:46:58
 *  Author: Mikael Ejberg Pedersen
 *
 * route_queue is a subpart of route.
 * Do not include from main program.
 */

#ifndef ROUTE_QUEUE_H_
#define ROUTE_QUEUE_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    RQ_CMD_SW,
    RQ_CMD_FB
} route_queue_cmd_t;


/**
 * Update route queue.
 *
 * Call regularly from mainloop.
 */
extern void     route_queue_update(void);

/**
 * Add a command to route queue.
 *
 * @param cmd Command.
 * @param adr Address.
 * @param dir Option (true = closed/green/occupied, false = thrown/red/free).
 */
extern void     route_queue_add(uint16_t adr, bool opt, route_queue_cmd_t cmd);

#endif /* ROUTE_QUEUE_H_ */
