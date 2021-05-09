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
#include "lib/loconet-avrda/hal_ln.h"


/*
 * Test command IN
 *
 * Sends an OPC_INPUT_REP with track occupied, then (after some seconds)
 * send another one with track free.
 */

typedef struct
{
    uint16_t    adr;
    ticks_t     delay;
} cmd_in_t;

static void in_timer_cb(void *ctx)
{
    // OPC_INPUT_REP timeout. Send track free.
    cmd_in_t   *p = (cmd_in_t *)ctx;
    lnpacket_t *txdata;
    uint16_t    adr;

    txdata = hal_ln_packet_get();
    if (!txdata)
    {
        printf_P(PSTR("Out of lnpackets\n"));
        return;
    }

    printf_P(PSTR("Sending track free on %u\n"), p->adr);

    adr = p->adr - 1;
    free(p);

    txdata->input_rep.op = OPC_INPUT_REP;
    txdata->input_rep.i = adr;
    txdata->input_rep.adrl = adr >> 1;
    txdata->input_rep.adrh = adr >> 8;
    txdata->input_rep.l = 0;
    txdata->input_rep.x = 1;
    txdata->input_rep.zero1 = 0;
    txdata->input_rep.zero2 = 0;
    hal_ln_send(txdata, NULL, NULL);
}

static void in_cb(void *ctx)
{
    // OPC_INPUT_REP track occupied sent. Wait before sending track free.
    cmd_in_t   *p = (cmd_in_t *)ctx;

    timer_add(p->delay, in_timer_cb, p);
}

const __flash char cmdin_name[] = "in";
const __flash char cmdin_help[] = "Send OPC_INPUT_REP";

void in_cmd(uint8_t argc, char *argv[])
{
    cmd_in_t   *p;
    lnpacket_t *txdata;
    uint16_t    adr;

    if (argc < 2)
    {
        printf_P(PSTR("Usage: in <adr> [<delay>]\n"));
        printf_P(PSTR(" <adr>   : Input address\n"));
        printf_P(PSTR(" <delay> : Optional delay for track free (in seconds). Default 2\n"));
        return;
    }

    txdata = hal_ln_packet_get();
    if (!txdata)
    {
        printf_P(PSTR("Out of lnpackets\n"));
        return;
    }

    p = malloc(sizeof(*p));
    if (!p)
    {
        printf_P(PSTR("Out of memory\n"));
        hal_ln_packet_free(txdata);
        return;
    }

    p->adr = strtoul(argv[1], NULL, 0);
    if (argc >= 3)
        p->delay = strtoul(argv[2], NULL, 0) * TICKS_PER_SEC;
    else
        p->delay = 2 * TICKS_PER_SEC;   // Default 2 seconds

    adr = p->adr - 1;
    txdata->input_rep.op = OPC_INPUT_REP;
    txdata->input_rep.i = adr;
    txdata->input_rep.adrl = adr >> 1;
    txdata->input_rep.adrh = adr >> 8;
    txdata->input_rep.l = 1;
    txdata->input_rep.x = 1;
    txdata->input_rep.zero1 = 0;
    txdata->input_rep.zero2 = 0;
    hal_ln_send(txdata, in_cb, p);
}


/********************************************************************/


/*
 * Test command SW
 *
 * Sends an OPC_SW_REQ .... TODO
 */

typedef struct
{
    uint16_t    adr;
    uint8_t     dir;
} cmd_sw_t;

static void sw_timer_cb(void *ctx)
{
    // OPC_SW_REQ timeout. Send off.
    cmd_sw_t   *p = (cmd_sw_t *)ctx;
    lnpacket_t *txdata;
    uint16_t    adr;
    uint8_t     dir;

    txdata = hal_ln_packet_get();
    if (!txdata)
    {
        printf_P(PSTR("Out of lnpackets\n"));
        return;
    }

    printf_P(PSTR("Sending sw off on %u\n"), p->adr);

    adr = p->adr - 1;
    dir = p->dir;
    free(p);

    txdata->sw.op = OPC_SW_REQ;
    txdata->sw.adrl = adr;
    txdata->sw.adrh = adr >> 7;
    txdata->sw.on = 0;
    txdata->sw.dir = dir;
    txdata->sw.zero1 = 0;
    txdata->sw.zero2 = 0;
    hal_ln_send(txdata, NULL, NULL);
}

static void sw_cb(void *ctx)
{
    // OPC_SW_REQ on sent. Wait 250 ms before sending off.
    timer_add(TICKS_PER_SEC / 4, sw_timer_cb, ctx);
}

const __flash char cmdsw_name[] = "sw";
const __flash char cmdsw_help[] = "Send OPC_SW_REQ";

void sw_cmd(uint8_t argc, char *argv[])
{
    cmd_sw_t   *p;
    lnpacket_t *txdata;
    uint16_t    adr;

    if (argc < 3)
    {
        printf_P(PSTR("Usage: sw <adr> <dir>\n"));
        printf_P(PSTR(" <adr> : Input address\n"));
        printf_P(PSTR(" <dir> : Direction R/G or 0/1\n"));
        return;
    }

    txdata = hal_ln_packet_get();
    if (!txdata)
    {
        printf_P(PSTR("Out of lnpackets\n"));
        return;
    }

    p = malloc(sizeof(*p));
    if (!p)
    {
        printf_P(PSTR("Out of memory\n"));
        hal_ln_packet_free(txdata);
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

    adr = p->adr - 1;
    txdata->sw.op = OPC_SW_REQ;
    txdata->sw.adrl = adr;
    txdata->sw.adrh = adr >> 7;
    txdata->sw.on = 1;
    txdata->sw.dir = p->dir;
    txdata->sw.zero1 = 0;
    txdata->sw.zero2 = 0;
    hal_ln_send(txdata, sw_cb, p);
}
