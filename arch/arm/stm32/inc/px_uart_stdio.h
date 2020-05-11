#ifndef __PX_UART_STDIO_H__
#define __PX_UART_STDIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_uart_stdio.h : Module to redirect stdio output (e.g. printf) to a UART
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-06

============================================================================= */
/** 
 *  @ingroup STM32
 *  @defgroup STM32_UART_STDIO px_uart_stdio.h : UART stdio redirect module
 *  
 *  Initialises the stdout stream to output to UART.
 *  
 *  File(s):
 *  - arch/avr/inc/px_uart_stdio.h
 *  - arch/avr/src/px_uart_stdio.c
 *  
 *  This component initialises the @b stdout stream to output over UART.
 *  
 *  px_uart_stdio_put_char() will intercept all line feed characters (@b \\n) 
 *  and replace it with a carriage return, line feed sequence (@b \\r\\n). 
 *   
 *  References: 
 *  1. newlib-2.5.0/newlib/libc/sys/arm/syscalls.c
 *  
 *  Example:
 *  
 *  @include arch/arm/stm32/test/px_uart_stdio_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_uart.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise @b stdio stream to use a UART driver.
 *  
 *  px_uart_stdio_put_char() is provided as the handler for all outgoing data.
 *  
 *  @param handle   opened handle to UART peripheral
 */
void px_uart_stdio_init(px_uart_handle_t * handle);

/**
 *  Function to send a byte.
 *  
 *  Every carriage return ("\n") will be intercepted and replaced with a
 *  carriage return, new line sequence ("\r\n").
 *  
 *  @param data     byte to send
 *  
 *  @return int     always returns 0
 */
int px_uart_stdio_put_char(char data);

/**
 *  Function to receive a byte.
 *  
 *  This function will block until a character has been received.
 *  
 *  
 *  @return int     received byte
 */
int px_uart_stdio_get_char(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_UART_STDIO_H__
