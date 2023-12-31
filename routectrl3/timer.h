/*
 * timer.h
 *
 * Created: 10-05-2021 18:07:05
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "ticks.h"


typedef void    (timer_cb) (void *);

extern int8_t   timer_add(ticks_t timeout, timer_cb *cb, void *ctx);
extern void     timer_update(void);

#endif /* TIMER_H_ */
