/*
 * mmi.h
 *
 * Created: 25-03-2024 11:16:35
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef MMI_H_
#define MMI_H_

#include <stdint.h>


/*
 * Init mmi module.
 *
 * Call once at startup.
 */
extern void     mmi_init(void);

/*
 * Update mmi module.
 *
 * Call regularly from mainloop.
 */
extern void     mmi_update(void);

/*
 * Set operating level.
 *
 * @param level New operating level.
 */
extern void     mmi_operating_level_set(uint8_t level);

/*
 * Get operating level.
 *
 * @return Current operating level.
 */
extern uint8_t  mmi_operating_level_get(void);


#endif /* MMI_H_ */
