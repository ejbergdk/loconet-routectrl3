/*
 * fb_handler.h
 *
 * Feedback handler.
 * Receives OPC_INPUT_REP from Loconet and calls feedback subscribers.
 *
 * Created: 02-01-2023 17:28:01
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef FB_HANDLER_H_
#define FB_HANDLER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Feedback subscriber callback function prototype.
 *
 * @param adr Feedback address.
 */
typedef void    (*feedback_cb_t)(uint16_t);

typedef struct
{
    const uint16_t  adr;
    const feedback_cb_t cb;
} feedback_table_t;

/**
 * Feedback occupied callback subscription macro.
 *
 * Subscribe to a single feedback address.
 * When the given feedback (occupied) address is received from Loconet, the function is called.
 *
 * @param num  Feedback address.
 * @param func Callback function.
 */
#define FEEDBACK_OCC(num, func) static const feedback_table_t fboccentry##num \
    __attribute__((used, section("loconet.fbocctable." #num))) = \
    {.adr = num, .cb = func};

/**
 * Feedback free callback subscription macro.
 *
 * Subscribe to a single feedback address.
 * When the given feedback (free) address is received from Loconet, the function is called.
 *
 * @param num  Feedback address.
 * @param func Callback function.
 */
#define FEEDBACK_FREE(num, func) static const feedback_table_t fbfreeentry##num \
    __attribute__((used, section("loconet.fbfreetable." #num))) = \
    {.adr = num, .cb = func};

typedef struct
{
    const uint16_t  adr_start;
    const uint16_t  adr_end;
    const feedback_cb_t cb;
} feedbackrange_table_t;

/**
 * Feedback occupied range callback subscription macro.
 *
 * Subscribe to a range of feedback addresses.
 * When a feedback address in the given range is received from Loconet, the function is called.
 *
 * @param start Feedback start address.
 * @param end   Feedback end address.
 * @param func  Callback function.
 */
#define FEEDBACK_RANGE_OCC(start, end, func) static const feedbackrange_table_t fbrangeoccentry##start##end \
    __attribute__((used, section("loconet.fbrangeocctable"))) = \
    {.adr_start = start, .adr_end = end, .cb = func};

/**
 * Feedback free range callback subscription macro.
 *
 * Subscribe to a range of feedback addresses.
 * When a feedback address in the given range is received from Loconet, the function is called.
 *
 * @param start Feedback start address.
 * @param end   Feedback end address.
 * @param func  Callback function.
 */
#define FEEDBACK_RANGE_FREE(start, end, func) static const feedbackrange_table_t fbrangefreeentry##start##end \
    __attribute__((used, section("loconet.fbrangefreetable"))) = \
    {.adr_start = start, .adr_end = end, .cb = func};


/**
 * Set state of feedback address.
 *
 * Calling this function does NOT send a Loconet packet.
 * It only sets the feedback state locally.
 *
 * @param adr Feedback address.
 * @param l   True if occupied.
 */
extern void     fb_handler_set_state(uint16_t adr, bool l);

/**
 * Get state of feedback address.
 *
 * @param adr Feedback address.
 * @return    True if occupied.
 */
extern bool     fb_handler_get_state(uint16_t adr);

/**
 * Get number of received feedback packets received.
 * Wraps at overflow.
 *
 * @return    Feedback packets received.
 */
extern uint16_t fb_handler_get_packets_received(void);

#endif /* FB_HANDLER_H_ */
