/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_ft.h : Flow Trace logging module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log_ft.h"
#if PX_LOG_FT

#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Magic value to indicate that buffer has been initialized and has valid content
#define PX_LOG_FT_MARKER    0xdeadc0de

typedef struct
{
    uint32_t marker;                        /// Magic value to indicate that buffer has been initialized and has valid content
    uint8_t  idx;                           ///< Index of next empty position
    uint32_t buf[PX_LOG_FT_CFG_BUF_SIZE];   ///< Buffer for log output
} px_log_ft_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Buffer for log output; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
PX_ATTR_SECTION(".noinit") px_log_ft_t px_log_ft;

/* _____LOCAL VARIABLES______________________________________________________ */
// Name strings defined?
#ifdef PX_LOG_FT_CFG_NAMES
// Define array of name strings for each name value
PX_LOG_FT_CFG_NAMES()
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef PX_LOG_FT_CFG_NAMES
const char * px_log_ft_name_to_str(px_log_ft_name_t name)
{
    if(name >= PX_SIZEOF_ARRAY(px_log_ft_name_str))
    {
        return "";
    }
    return px_log_ft_name_str[name];
}
#endif

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_log_ft_init(void)
{
    px_log_ft.idx = 0;

    for(int i = 0; i < PX_LOG_FT_CFG_BUF_SIZE; i++)
    {
        px_log_ft.buf[i] = 0;
    }

    px_log_ft.marker = PX_LOG_FT_MARKER;
}

void _px_log_ft_tag(const px_log_ft_name_t name, uint16_t line)
{
    uint32_t ft = (((uint32_t)name) << 16) | ((uint32_t)line);

    px_log_ft.buf[px_log_ft.idx] = ft;

    if(++px_log_ft.idx == PX_LOG_FT_CFG_BUF_SIZE)
    {
        px_log_ft.idx = 0;
    }
}

void _px_log_ft_tag_param(const px_log_ft_name_t name, uint16_t line, uint8_t param)
{
    uint32_t ft = (((uint32_t)param) << 24) | (((uint32_t)name) << 16) | ((uint32_t)line);

    px_log_ft.buf[px_log_ft.idx] = ft;

    if(++px_log_ft.idx == PX_LOG_FT_CFG_BUF_SIZE)
    {
        px_log_ft.idx = 0;
    }
}

void px_log_ft_report(void)
{
    uint8_t  idx = px_log_ft.idx;
    uint8_t  name, param;
    uint16_t line;

    if(px_log_ft.marker != PX_LOG_FT_MARKER      ) return;
    if(px_log_ft.idx    >= PX_LOG_FT_CFG_BUF_SIZE) return;

    do
    {
        if(idx != 0)
        {
            idx--;
        }
        else
        {
            idx = PX_LOG_FT_CFG_BUF_SIZE - 1;
        }
        param = (px_log_ft.buf[idx] >> 24) & 0xff;
        name  = (px_log_ft.buf[idx] >> 16) & 0xff;
        line  = (px_log_ft.buf[idx] >>  0) & 0xffff;
#ifdef PX_LOG_FT_CFG_NAMES
        PX_LOG_TRACE("%u %s # %u (%u)\n", name, px_log_ft_name_to_str((px_log_ft_name_t)name), line, param);
#else
        PX_LOG_TRACE("%u # %u (%u)\n", name, line, param);
#endif
    }
    while(idx != px_log_ft.idx);
}

#endif
