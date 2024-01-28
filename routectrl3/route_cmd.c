/*
 * route_cmd.c
 *
 * Created: 21-01-2024 13:55:37
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/avr-shell-cmd/cmd.h"
#include "route.h"

static void routeCmd(uint8_t argc, char *argv[])
{
    routenum_t      num = 0, numto = 0;

    if (argc < 2)
    {
        printf_P(PSTR("Route params:\n"));
        printf_P(PSTR("c <num>   : Cancel route\n"));
        printf_P(PSTR("f <num>   : Free route\n"));
        printf_P(PSTR("k <num>   : Kill route\n"));
        printf_P(PSTR("o <num>   : Force route\n"));
        printf_P(PSTR("r <num>   : Request route\n"));
        printf_P(PSTR("s [<num>] : Route status\n"));
        return;
    }

    if (argc >= 3)
    {
        num = strtoul(argv[2], NULL, 0);
        if (argc >= 4)
            numto = strtoul(argv[3], NULL, 0);
        else
            numto = num;

        if (num >= MAXROUTES || numto >= MAXROUTES)
        {
            printf_P(PSTR("Invalid route number\n"));
            return;
        }
    }
    else if (argv[1][0] != 's')
    {
        printf_P(PSTR("Route number missing\n"));
        return;
    }

    switch (argv[1][0])
    {
    case 'c':
        route_cancel(num);
        break;

    case 'f':
        route_free(num);
        break;

    case 'k':
        route_kill(num);
        break;

    case 'o':
        route_forceactive(num);
        break;

    case 'r':
        route_request(num);
        break;

    case 's':
        if (argc < 3)
        {
            routenum_t      dep = 0, exe = 0, act = 0;

            for (num = 0; num < MAXROUTES; num++)
            {
                switch (route_state(num))
                {
                case ROUTE_AWAITDEP:
                    dep++;
                    break;
                case ROUTE_AWAITEXE:
                    exe++;
                    break;
                case ROUTE_ACTIVE:
                    act++;
                    break;
                default:
                    break;
                }
            }
            printf_P(PSTR("Routes active:      %u\n"), act);
            printf_P(PSTR("Await dependencies: %u\n"), dep);
            printf_P(PSTR("Await execution:    %u\n"), exe);
        }
        else
        {
            for (; num <= numto; num++)
            {
                printf_P(PSTR("Route %u: "), num);
                if (!route_exists(num))
                {
                    printf_P(PSTR("Undefined\n"));
                    continue;
                }

                switch (route_state(num))
                {
                case ROUTE_FREE:
                    printf_P(PSTR("FREE\n"));
                    break;
                case ROUTE_AWAITDEP:
                    printf_P(PSTR("AWAITDEP\n"));
                    break;
                case ROUTE_AWAITEXE:
                    printf_P(PSTR("AWAITEXE\n"));
                    break;
                case ROUTE_ACTIVE:
                    printf_P(PSTR("ACTIVE\n"));
                    break;
                default:
                    printf_P(PSTR("INVALID\n"));
                    break;
                }
            }
        }
        break;

    default:
        printf_P(PSTR("Unknown parameter\n"));
        break;
    }
}

CMD(route, "Route test and debug");
