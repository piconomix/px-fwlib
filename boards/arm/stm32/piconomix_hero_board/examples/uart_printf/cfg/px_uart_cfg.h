#ifndef __PX_UART_CFG_H__
#define __PX_UART_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_uart_cfg.h : UART Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-26

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS _________________________________________________________ */
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
