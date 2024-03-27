/*
 * mmi.h
 *
 * Created: 25-03-2024 11:16:35
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef MMI_H_
#define MMI_H_

#include <stdbool.h>
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
 * Get operating level.
 *
 * @return Current operating level.
 */
extern uint8_t  mmi_operating_level_get(void);

/*
 * Decrease operating level by one.
 */
extern void     mmi_operating_level_decrease(void);

/*
 * Increase operating level by one.
 */
extern void     mmi_operating_level_increase(void);

/**
 * Set constant flashing to signal init.
 *
 * @param init True to flash non-stop. False for normal operation.
 */
extern void     mmi_in_init(bool init);

#endif /* MMI_H_ */
