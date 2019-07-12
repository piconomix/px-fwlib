#ifndef __PX_UART_CFG_H__
#define __PX_UART_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_uart_cfg.h : UART Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-26

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for UART1 peripheral
#define PX_UART_CFG_UART1_EN 1

/// Enable/disable support for UART2 peripheral
#define PX_UART_CFG_UART2_EN 0

/// Enable/disable support for UART4 peripheral
#define PX_UART_CFG_UART4_EN 0

/// Enable/disable support for UART5 peripheral
#define PX_UART_CFG_UART5_EN 0

/// Default BAUD rate
#define PX_UART_CFG_DEFAULT_BAUD 115200ul

/// Size of transmit buffer
#define PX_UART_CFG_TX_BUF_SIZE 64

/// Size of receive buffer
#define PX_UART_CFG_RX_BUF_SIZE 64

#endif // #ifndef __PX_UART_CFG_H__
