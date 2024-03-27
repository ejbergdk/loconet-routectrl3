/*
 * ctrl_util.h
 *
 * Created: 27-03-2024 15:33:02
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef CTRL_UTIL_H_
#define CTRL_UTIL_H_

#include <stdbool.h>
#include <stdint.h>


/*
 * Lock switch and set direction.
 *
 * @param adr Switch address.
 * @param opt Switch direction (SW_R / SW_G).
 */
extern void     util_lock_sw(uint16_t adr, bool opt);

/*
 * Unlock switch.
 *
 * @param adr Switch address.
 */
extern void     util_unlock_sw(uint16_t adr);

/*
 * Lock DKW switch and set direction.
 *
 * @param adr  Switch address.
 * @param opt  Switch direction (SW_R / SW_G).
 * @param opt2 DKW direction (SW_R / SW_G).
 */
extern void     util_lock_dkw(uint16_t adr, bool opt, bool opt2);

/*
 * Return a random number between min and max (both incl).
 *
 * @param min Minimum number to return.
 * @param max Maximum number to return.
 * @return    Random number.
 */
extern uint16_t util_rand(uint16_t min, uint16_t max);

#endif /* CTRL_UTIL_H_ */
