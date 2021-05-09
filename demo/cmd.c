/*
 * cmd.c
 *
 * Created: 16-05-2020 17:53:32
 *  Author: Mikael Ejberg Pedersen
 */ 

#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "cmd.h"
#include "test_cmds.h"
#include "lib/loconet-avrda/hal_ln.h"

typedef void (*cmdfunc_t)(uint8_t argc, char *argv[]);
static const __flash char cmdhelp_name[] = "help";
static const __flash char cmdhelp_help[] = "Help";

typedef struct  
{
    const __flash char *name;
    const __flash char *help;
    const cmdfunc_t func;
} cmdlist_t;

static void print_help(uint8_t argc, char *argv[]);

static const __flash cmdlist_t cmdlist[] = {
    {cmdhelp_name,  cmdhelp_help,   print_help},
    {cmdin_name,    cmdin_help,     in_cmd},
    {cmdln_name,    cmdln_help,     ln_cmd},
    {cmdsw_name,    cmdsw_help,     sw_cmd}
};

static void print_help(uint8_t argc, char *argv[])
{
    int i;

    for (i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++)
    {
        printf_P(PSTR("%-12S : %S\n"), cmdlist[i].name, cmdlist[i].help);
    }
}

void cmd_exec(uint8_t argc, char *argv[])
{
    int i;

    for (i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++)
    {
        if (!strcmp_P(argv[0], cmdlist[i].name))
        {
            cmdlist[i].func(argc, argv);
            return;
        }
    }

    printf_P(PSTR("Command not found\n"));
}
