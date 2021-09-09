/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_xmodem_glue.h : Glue functions for XMODEM module
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_xmodem_glue.h"
#include "px_systmr.h"
#include "main.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_xmodem_glue");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
px_systmr_t px_xmodem_tmr;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_xmodem_rd_u8(uint8_t * data)
{
    return px_uart_rd_u8(&px_uart_handle, data);
}

void px_xmodem_wr_u8(uint8_t data)
{
    while(!px_uart_wr_u8(&px_uart_handle, data))
    {
        ;
    }
}

void px_xmodem_tmr_start(uint16_t time_ms)
{
    px_systmr_ticks_t delay_in_ticks;

    delay_in_ticks = PX_SYSTMR_MS_TO_TICKS(time_ms);
    px_systmr_start(&px_xmodem_tmr, delay_in_ticks);
}

bool px_xmodem_tmr_has_expired(void)
{
    return px_systmr_has_expired(&px_xmodem_tmr);
}

