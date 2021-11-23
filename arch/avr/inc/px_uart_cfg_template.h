#ifndef __PX_UART_CFG_H__
#define __PX_UART_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_uart_cfg.h : UART Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-16

============================================================================= */

/** 
 *  @addtogroup AVR_UART
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for UART0 peripheral
#define PX_UART_CFG_UART0_EN 1

/// Enable/disable support for UART1 peripheral
#define PX_UART_CFG_UART1_EN 0

/// Default BAUD rate
#define PX_UART_CFG_DEFAULT_BAUD 115200ul

/// Default UART configuration (8 data bits, no parity, 1 stop bit)
#define PX_UART_CFG_DEFAULT_UCSRC  ((1 << UCSZ01) | (1 << UCSZ00))

/// Size of transmit buffer (must be a power of two, e.g. 2,4,8,..., 256)
#define PX_UART_CFG_TX_BUF_SIZE 32

/// Size of receive buffer (must be a power of two, e.g. 2,4,8,..., 256)
#define PX_UART_CFG_RX_BUF_SIZE 64

/**
 *  Use double rate UART Transmission speed (0 = no; 1 = yes)
 *  
 *  If enabled, it results in a smaller BAUD rate error when using imprecise 
 *  clock inputs, e.g. for F_CPU = 16 MHz and desired Baud Rate = 115200:
 *  * U2Xn = 0 : UBBRn = 8  (-3.5% error)
 *  * U2Xn = 1 : UBBRn = 16 (+2.1% error)
 */
#define PX_UART_CFG_USE_2X_BAUD_RATE 1

/// @}
#endif
