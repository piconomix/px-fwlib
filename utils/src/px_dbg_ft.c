/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

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
/// Index of next empty position; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
PX_ATTR_SECTION(".noinit") uint8_t px_dbg_ft_buf_idx;

/// Buffer for log output; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
PX_ATTR_SECTION(".noinit") uint32_t px_dbg_ft_buf[PX_DBG_FT_CFG_BUF_SIZE];

/* _____LOCAL VARIABLES______________________________________________________ */
// Name strings defined?
#ifdef PX_LOG_CFG_NAMES
// Define array of name strings for each name value
PX_LOG_CFG_NAMES()
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef PX_LOG_CFG_NAMES
const char * px_dbg_ft_name_val_to_str(px_dbg_ft_name_t name)
{
    if(name >= PX_SIZEOF_ARRAY(px_dbg_ft_name_str))
    {
        return "";
    }
    return px_dbg_ft_name_str[name];
}
#endif

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_dbg_ft_init(void)
{
    px_dbg_ft_buf_idx = 0;

    for(int i = 0; i < PX_DBG_FT_CFG_BUF_SIZE; i++)
    {
        px_dbg_ft_buf[i] = 0;
    }
}

void _px_dbg_ft_log(const px_dbg_ft_name_t name, uint16_t line)
{
    uint32_t ft = (((uint32_t)name) << 16) | ((uint32_t)line);

    px_dbg_ft_buf[px_dbg_ft_buf_idx] = ft;

    if(++px_dbg_ft_buf_idx == PX_DBG_FT_CFG_BUF_SIZE)
    {
        px_dbg_ft_buf_idx = 0;
    }
}

void _px_dbg_ft_log_param(const px_dbg_ft_name_t name, uint16_t line, uint8_t param)
{
    uint32_t ft = (((uint32_t)param) << 24) | (((uint32_t)name) << 16) | ((uint32_t)line);

    px_dbg_ft_buf[px_dbg_ft_buf_idx] = ft;

    if(++px_dbg_ft_buf_idx == PX_DBG_FT_CFG_BUF_SIZE)
    {
        px_dbg_ft_buf_idx = 0;
    }
}

void px_dbg_ft_report(void)
{
    uint8_t  idx = px_dbg_ft_buf_idx;
    uint8_t  name, param;
    uint16_t line;

    do
    {
        if(idx != 0)
        {
            idx--;
        }
        else
        {
            idx = PX_DBG_FT_CFG_BUF_SIZE - 1;
        }
        param = (px_dbg_ft_buf[idx] >> 24) & 0xff;
        name  = (px_dbg_ft_buf[idx] >> 16) & 0xff;
        line  = (px_dbg_ft_buf[idx] >>  0) & 0xffff;
#ifdef PX_LOG_CFG_NAMES
        printf("%u %s # %u (%u)\n", name, px_dbg_ft_name_val_to_str(name), line, param);
#else
        printf("%u # %u (%u)\n", name, line, param);
#endif
    }
    while(idx != px_dbg_ft_buf_idx);
}

