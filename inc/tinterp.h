//==============================================================================
// Simple C interpreter - parses command and arguments from a string
//
//------------------------------------------------------------------------------
// [tiny-interp] 2019 Eden Synrez <esynr3z@gmail.com>
//==============================================================================

#ifndef __TINTERP_H
#define __TINTERP_H

//-- Includes ------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-- Exported macro ------------------------------------------------------------
#define TINTERP_BUF_SIZE 256
#define TINTERP_CMD_ARGS_MAX 8

//-- Exported types ------------------------------------------------------------
typedef enum {
    TINTERP_SUCCESS = 0,
    TINTERP_FAIL
} tinterp_status_t;

typedef enum {
    TINTERP_CMD_HELP = 0,
    TINTERP_CMD_GET_TIME,
    TINTERP_CMD_SET_TIME,
    TINTERP_CMD_SET_NAME,
    TINTERP_CMD_GET_NAME,
    TINTERP_CMDS_TOTAL
} tinterp_cmds_t;

typedef union {
    char* s;
    uint32_t ui;
} tinterp_arg_t;

//-- Exported functions --------------------------------------------------------
tinterp_status_t tinterp_parse(char* str);
void tinterp_set_callback(tinterp_cmds_t cmd, void (*func)(uint32_t, tinterp_arg_t*));

#endif // __TINTERP_H
