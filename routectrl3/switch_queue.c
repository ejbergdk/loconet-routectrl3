/*
 * switch_queue.c
 *
 * Created: 24-07-2021 14:54:12
 *  Author: Mikael Ejberg Pedersen
 */

#include <stdbool.h>
#include <stdint.h>
#include "switch_queue.h"
#include "ticks.h"
#include "lib/loconet-avrda/ln_tx.h"

#define SWITCH_ACTIVE_TIME  TICKS_FROM_MS(327)
#define SWITCH_DELAY_TIME   TICKS_FROM_MS(50)
#define QUEUE_SIZE          16

typedef enum
{
    SWQ_STATE_IDLE,
    SWQ_STATE_ACTIVE,
    SWQ_STATE_DELAY,
    SWQ_STATE_WAIT_CB
} swq_state_t;

typedef struct
{
    uint16_t        adr:15;
    uint16_t        dir:1;
} swq_switch_t;

static swq_switch_t queue[QUEUE_SIZE];
static uint8_t  queue_ridx = 0;
static uint8_t  queue_widx = 0;

static ticks_t  last_activity = 0;
static swq_state_t state = SWQ_STATE_IDLE;


void switch_queue_add(uint16_t adr, bool dir)
{
    // NOTE: There is no buffer full check
    queue[queue_widx].adr = adr;
    queue[queue_widx].dir = dir;
    queue_widx++;
    if (queue_widx >= QUEUE_SIZE)
        queue_widx = 0;
}

static void sw_cb(void *ctx, hal_ln_result_t res)
{
    if (res == HAL_LN_SUCCESS)
    {
        // OPC_SW_REQ sent. Activate next state.
        last_activity = ticks_get();
        state = *(swq_state_t *)ctx;
    }
    else
    {
        // Unable to transmit. Give up (or make better error handling here)
        state = SWQ_STATE_IDLE;
    }
}

void switch_queue_update(void)
{
    static swq_state_t next_state;

    switch (state)
    {
    case SWQ_STATE_IDLE:
    default:
        if (queue_ridx != queue_widx)
        {
            if (ln_tx_opc_sw_req(queue[queue_ridx].adr, queue[queue_ridx].dir, true, sw_cb, &next_state) == 0)
            {
                state = SWQ_STATE_WAIT_CB;
                next_state = SWQ_STATE_ACTIVE;
            }
        }
        break;

    case SWQ_STATE_ACTIVE:
        if (ticks_elapsed(last_activity) >= SWITCH_ACTIVE_TIME)
        {
            if (ln_tx_opc_sw_req(queue[queue_ridx].adr, queue[queue_ridx].dir, false, sw_cb, &next_state) == 0)
            {
                state = SWQ_STATE_WAIT_CB;
                next_state = SWQ_STATE_DELAY;
            }
        }
        break;

    case SWQ_STATE_DELAY:
        if (ticks_elapsed(last_activity) >= SWITCH_DELAY_TIME)
        {
            queue_ridx++;
            if (queue_ridx >= QUEUE_SIZE)
                queue_ridx = 0;
            state = SWQ_STATE_IDLE;
        }
        break;

    case SWQ_STATE_WAIT_CB:
        // Do nothing. Waiting for callback
        break;
    }
}

bool switch_queue_empty(void)
{
    return (state == SWQ_STATE_IDLE && queue_ridx == queue_widx);
}
