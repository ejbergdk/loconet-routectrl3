/*
 * test_cmds.c
 *
 * Created: 09-05-2021 14:46:09
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "switch_queue.h"
#include "ticks.h"
#include "timer.h"
#include "lib/avr-shell-cmd/cmd.h"
#include "lib/loconet-avrda/ln_tx.h"


/*
 * Test command IN
 *
 * Sends an OPC_INPUT_REP with track occupied, then (after some seconds)
 * send another one with track free.
 */

typedef struct
{
    uint16_t        adr;
    ticks_t         delay;
} cmd_in_t;

static void in_timer_cb(void *ctx)
{
    // OPC_INPUT_REP timeout. Send track free.
    cmd_in_t       *p = (cmd_in_t *) ctx;
    uint16_t        adr;

    adr = p->adr;
    free(p);

    printf_P(PSTR("Sending track free: %u\n"), adr);

    if (ln_tx_opc_input_rep(adr, false, NULL, NULL))
        printf_P(PSTR("Out of lnpackets\n"));
}

static void in_cb(void *ctx, hal_ln_result_t res)
{
    cmd_in_t       *p = (cmd_in_t *) ctx;

    if (res == HAL_LN_SUCCESS)
    {
        // OPC_INPUT_REP track occupied sent. Wait before sending track free.
        if (timer_add(p->delay, in_timer_cb, p) == 0)
            return;
    }

    // An error happened. Cleanup
    free(p);
    printf_P(PSTR("Tx fail\n"));
}

static void inCmd(uint8_t argc, char *argv[])
{
    cmd_in_t       *p;

    if (argc < 2)
    {
        printf_P(PSTR("Usage: in <adr> [<delay>]\n"));
        printf_P(PSTR(" <adr>   : Input address\n"));
        printf_P(PSTR(" <delay> : Optional delay for track free (in seconds). Default 2\n"));
        return;
    }

    p = malloc(sizeof(*p));
    if (!p)
    {
        printf_P(PSTR("Out of memory\n"));
        return;
    }

    p->adr = strtoul(argv[1], NULL, 0);
    if (argc >= 3)
        p->delay = strtoul(argv[2], NULL, 0) * TICKS_PER_SEC;
    else
        p->delay = 2 * TICKS_PER_SEC;   // Default 2 seconds

    printf_P(PSTR("Sending track occupied: %u\n"), p->adr);

    if (ln_tx_opc_input_rep(p->adr, true, in_cb, p))
    {
        free(p);
        printf_P(PSTR("Out of lnpackets\n"));
    }
}

CMD(in, "Send OPC_INPUT_REP");


/********************************************************************/


/*
 * Test command SW
 *
 * Sends an OPC_SW_REQ .... TODO
 */
static void swCmd(uint8_t argc, char *argv[])
{
    uint16_t        adr;
    bool            dir;

    if (argc < 3)
    {
        printf_P(PSTR("Usage: sw <adr> <dir>\n"));
        printf_P(PSTR(" <adr> : Input address\n"));
        printf_P(PSTR(" <dir> : Direction R/G or 0/1\n"));
        return;
    }

    adr = strtoul(argv[1], NULL, 0);
    switch (argv[2][0])
    {
    case '0':
    case 'r':
    case 'R':
    default:
        dir = false;
        break;
    case '1':
    case 'g':
    case 'G':
        dir = true;
        break;
    }

    printf_P(PSTR("Sending sw_req: %u %c\n"), adr, dir ? 'G' : 'R');

    switch_queue_add(adr, dir);
}

CMD(sw, "Send OPC_SW_REQ");


/********************************************************************/


/*
 * Test command TIME
 *
 * Time a command
 */
static void timeCmd(uint8_t argc, char *argv[])
{
    ticks_t         t0 = ticks_get();

    if (argc >= 2)
        cmd_exec(argc - 1, argv + 1);
    t0 = ticks_elapsed(t0);
    printf_P(PSTR("Time: %u ticks\n"), t0);
}

CMD(time, "Time a command");
