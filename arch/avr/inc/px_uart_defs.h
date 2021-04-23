#ifndef __PX_UART_DEFS_H__
#define __PX_UART_DEFS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
   
    Title:          px_uart.h : UART peripheral compatibility definitions
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-10

============================================================================= */

#include <avr/io.h>

#ifdef USART_RX_vect
#define USART0_RX_vect USART_RX_vect
#endif

#ifdef USART_UDRE_vect
#define USART0_UDRE_vect USART_UDRE_vect
#endif

#ifdef USART_TX_vect
#define USART0_TX_vect USART_TX_vect
#endif


#endif
