/*
 * fb_handler.h
 *
 * Created: 02-01-2023 17:28:01
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef FB_HANDLER_H_
#define FB_HANDLER_H_

#include <stdbool.h>
#include <stdint.h>

typedef void    (*feedback_cb_t)(uint16_t);

typedef struct
{
    const uint16_t  adr;
    const feedback_cb_t cb;
} feedback_table_t;

#define FEEDBACK_OCC(num, func) static const feedback_table_t fboccentry##num \
    __attribute__((used, section("loconet.fbocctable." #num))) = \
    {.adr = num, .cb = func};

#define FEEDBACK_FREE(num, func) static const feedback_table_t fbfreeentry##num \
    __attribute__((used, section("loconet.fbfreetable." #num))) = \
    {.adr = num, .cb = func};

typedef struct
{
    const uint16_t  adr_start;
    const uint16_t  adr_end;
    const feedback_cb_t cb;
} feedbackrange_table_t;

#define FEEDBACK_RANGE_OCC(start, end, func) static const feedbackrange_table_t fbrangeoccentry##start##end \
    __attribute__((used, section("loconet.fbrangeocctable"))) = \
    {.adr_start = start, .adr_end = end, .cb = func};

#define FEEDBACK_RANGE_FREE(start, end, func) static const feedbackrange_table_t fbrangefreeentry##start##end \
    __attribute__((used, section("loconet.fbrangefreetable"))) = \
    {.adr_start = start, .adr_end = end, .cb = func};


/**
 * Get state of feedback address.
 *
 * @param adr Feedback address.
 * @return    True if occupied.
 */
extern bool     fb_get_state(uint16_t adr);

#endif /* FB_HANDLER_H_ */
