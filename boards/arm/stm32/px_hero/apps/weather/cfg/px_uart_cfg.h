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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
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
#define PX_UART_CFG_UART2_EN 1

/// Enable/disable support for UART4 peripheral
#define PX_UART_CFG_UART4_EN 1

/// Enable/disable support for UART5 peripheral
#define PX_UART_CFG_UART5_EN 0

/// Default BAUD rate
#define PX_UART_CFG_DEFAULT_BAUD 115200ul

/// General size of UART transmit buffer
#define PX_UART_CFG_UART_TX_BUF_SIZE    64
/// General size of UART receive buffer
#define PX_UART_CFG_UART_RX_BUF_SIZE    64

/// Size of UART1 transmit buffer
#define PX_UART_CFG_UART1_TX_BUF_SIZE   PX_UART_CFG_UART_TX_BUF_SIZE
/// Size of UART1 receive buffer
#define PX_UART_CFG_UART1_RX_BUF_SIZE   PX_UART_CFG_UART_RX_BUF_SIZE

/// Size of UART2 transmit buffer
#define PX_UART_CFG_UART2_TX_BUF_SIZE   PX_UART_CFG_UART_TX_BUF_SIZE
/// Size of UART2 receive buffer
#define PX_UART_CFG_UART2_RX_BUF_SIZE   PX_UART_CFG_UART_RX_BUF_SIZE

/// Size of UART4 transmit buffer
#define PX_UART_CFG_UART4_TX_BUF_SIZE   PX_UART_CFG_UART_TX_BUF_SIZE
/// Size of UART4 receive buffer
#define PX_UART_CFG_UART4_RX_BUF_SIZE   PX_UART_CFG_UART_RX_BUF_SIZE

/// Size of UART5 transmit buffer
#define PX_UART_CFG_UART5_TX_BUF_SIZE   PX_UART_CFG_UART_TX_BUF_SIZE
/// Size of UART5 receive buffer
#define PX_UART_CFG_UART5_RX_BUF_SIZE   PX_UART_CFG_UART_RX_BUF_SIZE

#endif
