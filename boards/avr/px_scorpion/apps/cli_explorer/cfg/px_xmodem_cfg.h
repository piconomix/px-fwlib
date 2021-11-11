#ifndef __PX_XMODEM_CFG_H__
#define __PX_XMODEM_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_xmodem_cfg.h : XMODEM Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_xmodem_glue.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Retry timeout in milliseconds
#define PX_XMODEM_CFG_TIMEOUT_MS            1000

/// Maximum number of retries to start a transfer
#define PX_XMODEM_CFG_MAX_RETRIES_START     4

/// Maximum nymber of retries during a transfer
#define PX_XMODEM_CFG_MAX_RETRIES           4

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
 *  @retval false   Timer has not expired
 */
#define PX_XMODEM_CFG_TMR_HAS_EXPIRED()    px_xmodem_tmr_has_expired()

#endif
