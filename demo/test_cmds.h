/*
 * test_cmds.h
 *
 * Created: 09-05-2021 14:46:26
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef TEST_CMDS_H_
#define TEST_CMDS_H_

#include <stdint.h>


extern const __flash char cmdin_name[];
extern const __flash char cmdin_help[];
extern void  in_cmd(uint8_t argc, char *argv[]);

extern const __flash char cmdsw_name[];
extern const __flash char cmdsw_help[];
extern void  sw_cmd(uint8_t argc, char *argv[]);


#endif /* TEST_CMDS_H_ */