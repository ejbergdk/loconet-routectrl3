/*
 * test_cmds.c
 *
 * Created: 09-05-2021 14:46:09
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_cmds.h"
#include "ticks.h"
#include "timer.h"
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
        // OPC_INPUT_REP track occupied sent. Wait before sending track free.
        timer_add(p->delay, in_timer_cb, p);
    else
        printf_P(PSTR("Tx fail\n"));
}

const __flash char cmdin_name[] = "in";
const __flash char cmdin_help[] = "Send OPC_INPUT_REP";

void in_cmd(uint8_t argc, char *argv[])
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
        printf_P(PSTR("Out of lnpackets\n"));
}


/********************************************************************/


/*
 * Test command SW
 *
 * Sends an OPC_SW_REQ .... TODO
 */

typedef struct
{
    uint16_t        adr;
    uint8_t         dir;
} cmd_sw_t;

static void sw_timer_cb(void *ctx)
{
    // OPC_SW_REQ timeout. Send off.
    cmd_sw_t       *p = (cmd_sw_t *) ctx;
    uint16_t        adr;
    uint8_t         dir;

    adr = p->adr;
    dir = p->dir;
    free(p);

    printf_P(PSTR("Sending sw off: %u %c\n"), adr, dir ? 'G' : 'R');

    if (ln_tx_opc_sw_req(adr, dir, false, NULL, NULL))
        printf_P(PSTR("Out of lnpackets\n"));
}

static void sw_cb(void *ctx, hal_ln_result_t res)
{
    if (res == HAL_LN_SUCCESS)
        // OPC_SW_REQ on sent. Wait 250 ms before sending off.
        timer_add(TICKS_PER_SEC / 4, sw_timer_cb, ctx);
    else
        printf_P(PSTR("Tx fail\n"));
}

const __flash char cmdsw_name[] = "sw";
const __flash char cmdsw_help[] = "Send OPC_SW_REQ";

void sw_cmd(uint8_t argc, char *argv[])
{
    cmd_sw_t       *p;

    if (argc < 3)
    {
        printf_P(PSTR("Usage: sw <adr> <dir>\n"));
        printf_P(PSTR(" <adr> : Input address\n"));
        printf_P(PSTR(" <dir> : Direction R/G or 0/1\n"));
        return;
    }

    p = malloc(sizeof(*p));
    if (!p)
    {
        printf_P(PSTR("Out of memory\n"));
        return;
    }

    p->adr = strtoul(argv[1], NULL, 0);
    switch (argv[2][0])
    {
    case '0':
    case 'r':
    case 'R':
    default:
        p->dir = 0;
        break;
    case '1':
    case 'g':
    case 'G':
        p->dir = 1;
        break;
    }

    printf_P(PSTR("Sending sw on: %u %c\n"), p->adr, p->dir ? 'G' : 'R');

    if (ln_tx_opc_sw_req(p->adr, p->dir, true, sw_cb, p))
        printf_P(PSTR("Out of lnpackets\n"));
}
