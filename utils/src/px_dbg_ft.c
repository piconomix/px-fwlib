/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_dbg_ft.h : Debug Flow Trace module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_dbg_ft.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Index of next empty position; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
PX_ATTR_SECTION(".noinit") static uint8_t px_dbg_ft_buf_index;

/// Buffer for log output; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
PX_ATTR_SECTION(".noinit") static uint32_t px_dbg_ft_buf[PX_DBG_FT_CFG_BUF_SIZE];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_dbg_ft_init(void)
{
    px_dbg_ft_buf_index = 0;

    for(int i = 0; i < PX_DBG_FT_CFG_BUF_SIZE; i++)
    {
        px_dbg_ft_buf[i] = 0;
    }
}

void _px_dbg_ft_log(const px_dbg_ft_name_t name, uint16_t line)
{
    uint32_t ft = (((uint32_t)name) << 16) | ((uint32_t)line);

    px_dbg_ft_buf[px_dbg_ft_buf_index] = ft;

    if(++px_dbg_ft_buf_index == PX_DBG_FT_CFG_BUF_SIZE)
    {
        px_dbg_ft_buf_index = 0;
    }
}

void _px_dbg_ft_log_param(const px_dbg_ft_name_t name, uint16_t line, uint8_t param)
{
    uint32_t ft = (((uint32_t)param) << 24) | (((uint32_t)name) << 16) | ((uint32_t)line);

    px_dbg_ft_buf[px_dbg_ft_buf_index] = ft;

    if(++px_dbg_ft_buf_index == PX_DBG_FT_CFG_BUF_SIZE)
    {
        px_dbg_ft_buf_index = 0;
    }
}

void px_dbg_ft_report(void)
{
    uint8_t  index = px_dbg_ft_buf_index;
    uint8_t  name, param;
    uint16_t line;

    do
    {
        if(index != 0)
        {
            index--;
        }
        else
        {
            index = PX_DBG_FT_CFG_BUF_SIZE - 1;
        }
        param = (px_dbg_ft_buf[index] >> 24) & 0xff;
        name  = (px_dbg_ft_buf[index] >> 16) & 0xff;
        line  = (px_dbg_ft_buf[index] >>  0) & 0xffff;
        printf("%u line %u : param %u\n", name, line, param);
    }
    while(index != px_dbg_ft_buf_index);
}

