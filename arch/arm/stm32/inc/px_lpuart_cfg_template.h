#ifndef __PX_LPUART_CFG_H__
#define __PX_LPUART_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2022 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_lpuart_cfg.h : LPUART Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2022-04-13

============================================================================= */

/** 
 *  @addtogroup STM32_UART
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Default BAUD rate
#define PX_LPUART_CFG_DEFAULT_BAUD 115200ul

/// General size of LPUART transmit buffer
#define PX_LPUART_CFG_LPUART_TX_BUF_SIZE    64
/// General size of LPUART receive buffer
#define PX_LPUART_CFG_LPUART_RX_BUF_SIZE    64

/// LPUART peripheral clock source (0 = no change, 1 = PCLK, 2 = SYSCLK, 3 = HSI, 4 = LSE)
#define PX_LPUART_CFG_CLK_SRC   3
/// LPUART peripheral clock in Hz
#define PX_LPUART_CFG_CLK_HZ    16000000

/// @}
#endif
