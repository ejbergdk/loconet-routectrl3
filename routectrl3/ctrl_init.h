/*
 * ctrl_init.h
 *
 * Created: 27-03-2024 13:26:37
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef CTRL_INIT_H_
#define CTRL_INIT_H_

#include <stdbool.h>


/*
 * Init control-init module.
 *
 * Call once at startup.
 */
extern void     ctrl_init_init(void);

/*
 * Init sequence done.
 *
 * @return True if init sequence is done.
 */
extern bool     ctrl_init_done(void);

#endif /* CTRL_INIT_H_ */
