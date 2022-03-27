/*
 * switch_queue.h
 *
 * Created: 24-07-2021 14:54:25
 *  Author: Mikael Ejberg Pedersen
 *
 * switch_queue allow the application to queue up several switch requests
 * in quick succession, and have the switch requests sent with proper timing.
 */

#ifndef SWITCH_QUEUE_H_
#define SWITCH_QUEUE_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Update switch queue.
 *
 * Call regularly from mainloop.
 */
extern void     switch_queue_update(void);

/**
 * Add a switch request to switch queue.
 *
 * @param adr Address of switch.
 * @param dir Direction of switch (true = closed/green, false = thrown/red)
 */
extern void     switch_queue_add(uint16_t adr, bool dir);

#endif /* SWITCH_QUEUE_H_ */
