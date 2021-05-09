/*
 * ticks.h
 *
 * Created: 16-05-2020 16:23:39
 *  Author: Mikael Ejberg Pedersen
 */ 


#ifndef TICKS_H_
#define TICKS_H_

#include <stdint.h>

#define TICKS_PER_SEC   1024

typedef uint32_t ticks_t;

extern void ticks_init(void);
extern ticks_t ticks_get(void);
extern ticks_t ticks_elapsed(ticks_t t0);

#endif /* TICKS_H_ */
