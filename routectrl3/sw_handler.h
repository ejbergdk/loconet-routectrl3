/*
 * sw_handler.h
 *
 * Switch handler.
 * Receives OPC_SW_REQ from Loconet and calls switch subscribers.
 *
 * Created: 29-01-2024 16:31:11
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef SW_HANDLER_H_
#define SW_HANDLER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Switch subscriber callback function prototype.
 *
 * @param adr Switch address.
 * @param dir True if G, false if R.
 */
typedef void    (*switchreq_cb_t)(uint16_t, bool);

typedef struct
{
    const uint16_t  adr;
    const switchreq_cb_t cb;
} switchreq_table_t;

/**
 * Switch callback subscription macro.
 *
 * Subscribe to a single switch address.
 * When the given switch address is received from Loconet, the function is called.
 * Note: Only the activation of the switch (on=1) is reported.
 *       Deactivations (on=0) are silently discarded.
 *
 * @param num  Switch address.
 * @param func Callback function.
 */
#define SWITCH_REQ(num, func) static const switchreq_table_t swreqentry##num \
    __attribute__((used, section("loconet.swreqtable." #num))) = \
    {.adr = num, .cb = func};

typedef struct
{
    const uint16_t  adr_start;
    const uint16_t  adr_end;
    const switchreq_cb_t cb;
} swreqrange_table_t;

/**
 * Switch range callback subscription macro.
 *
 * Subscribe to a range of switch addresses.
 * When a switch address in the given range is received from Loconet, the function is called.
 * Note: Only the activation of the switch (on=1) is reported.
 *       Deactivations (on=0) are silently discarded.
 *
 * @param start Switch start address.
 * @param end   Switch end address.
 * @param func  Callback function.
 */
#define SWITCH_REQ_RANGE(start, end, func) static const swreqrange_table_t swreqrangeentry##start##end \
    __attribute__((used, section("loconet.swreqrangetable"))) = \
    {.adr_start = start, .adr_end = end, .cb = func};


/**
 * Update switch handler module.
 *
 * Call regularly from mainloop.
 */
extern void     sw_handler_update(void);

/**
 * Set state of switch address.
 *
 * Calling this function does NOT send a Loconet packet.
 * It only sets the switch state locally.
 *
 * @param adr Switch address.
 * @param dir True if G, false if R.
 */
extern void     sw_handler_set_state(uint16_t adr, bool dir);

/**
 * Get state of switch address.
 *
 * @param adr Switch address.
 * @return    True if G, false if R.
 */
extern bool     sw_handler_get_state(uint16_t adr);

#endif /* SW_HANDLER_H_ */
