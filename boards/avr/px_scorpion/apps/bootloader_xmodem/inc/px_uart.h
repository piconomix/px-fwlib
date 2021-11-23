#ifndef __PX_UART_H__
#define __PX_UART_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          px_uart.h : UART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-10

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise UART driver.
 */
void  px_uart_init(void);

/**
 *  Transmit one byte.
 *  
 *  @param[in] data   Byte to be transmitted
 */
void px_uart_putchar(char data);

/**
 *  Buffer one byte for transmission.
 *  
 *  @param[in] data   Byte to be transmitted
 *  
 *  @retval true    Byte has been buffered
 *  @retval false   Byte has not been buffered, because transmit buffer is full
 */
bool px_uart_wr_u8(uint8_t data);

/**
 *  Receive one byte.
 *  
 *  @return     Received byte
 */
char px_uart_getchar(void);

/**
 *  See if received byte is available and store it in specified location.
 *  
 *  @param[out] data    Pointer to location where data byte must be stored
 *  
 *  @retval true        Received byte is stored in specified location
 *  @retval false       No received data available (receive buffer empty)
 */
bool px_uart_rd_u8(uint8_t * data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
