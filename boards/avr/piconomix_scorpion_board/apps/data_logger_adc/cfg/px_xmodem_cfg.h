#ifndef __PX_XMODEM_CFG_H__
#define __PX_XMODEM_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          px_xmodem_cfg.h : XMODEM Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_xmodem_glue.h"

/* _____DEFINITIONS _________________________________________________________ */
#define PX_XMODEM_CFG_MAX_RETRIES       4
#define PX_XMODEM_CFG_MAX_RETRIES_START 4

/**
 *  See if a received byte is available and store it in the specified location.
 *  
 *  @param[out] data    Pointer to location where data byte must be stored
 *  
 *  @retval true        Received byte is stored in specified location
 *  @retval false       No received data available (receive buffer empty)
 */
#define PX_XMODEM_CFG_RD_U8(data)          px_xmodem_rd_u8(data)

/**
 *  Write one byte.
 *  
 *  This function blocks until space is available in the transmit buffer.
 *  
 *  @param[in] data   Byte to be written
 */
#define PX_XMODEM_CFG_WR_U8(data)          px_xmodem_wr_u8(data)

/**
 *  Start timeout timer.
 *  
 *  This function starts the XMODEM timeout timer.
 *  
 *  @param[in] time_ms   Time in milliseconds to wait before timer has expired
 */
#define PX_XMODEM_CFG_TMR_START(time_ms)   px_xmodem_tmr_start(time_ms)


/**
 *  See if timer has expired.
 *  
 *  @retval true    Timer has expired
 *  @retval true    Timer has not expired
 */
#define PX_XMODEM_CFG_TMR_HAS_EXPIRED()    px_xmodem_tmr_has_expired()


/// @}
#endif // #ifndef __PX_XMODEM_CFG_H__
