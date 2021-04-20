/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
   
    Title:          px_uart.h : UART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-10

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_uart_polled");

#ifndef PX_UART_DEFAULT_BAUD
#define PX_UART_DEFAULT_BAUD   115200ul
#endif
#ifndef PX_UART_DEFAULT_UCSRC
#define PX_UART_DEFAULT_UCSRC  ((1<<UCSZ01) | (1<<UCSZ00))
#endif

/* _____MACROS_______________________________________________________________ */
/// Macro to convert the specified BAUD rate to a 16-bit UBRR register value
#define PX_UART_CALC_UBRR(baud) (PX_UDIV_ROUND(F_CPU, (16ul * baud)) - 1)

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_init(void)
{
    // Set BAUD rate by initalising 16-bit UBRR register
    UBRR0H = PX_U16_HI8(PX_UART_CALC_UBRR(PX_UART_DEFAULT_BAUD));
    UBRR0L = PX_U16_LO8(PX_UART_CALC_UBRR(PX_UART_DEFAULT_BAUD));

    // Configure Data bits, parity and stop bits
    UCSR0C = PX_UART_DEFAULT_UCSRC;

    // Enable RxD/TxD
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void px_uart_put_char(char data)
{
    // Wait until transmit register is empty
    PX_WAIT_UNTIL_BIT_IS_HI(UCSR0A, UDRE0);

    // Buffer byte to transmit
    UDR0 = data;
}

bool px_uart_wr_u8(uint8_t data)
{
    // Is transmit register to be empty?
    if(PX_BIT_IS_LO(UCSR0A, UDRE0))
    {
        // No
        return false;
    }

    // Buffer byte to transmit
    UDR0 = data;
    return true;
}

char px_uart_get_char(void)
{
    uint8_t ucsra;
    uint8_t udr;

    while(true)
    {
        // Wait until data received flag is set
        PX_WAIT_UNTIL_BIT_IS_HI(UCSR0A, RXC0);
    
        // Accept data only if there were no Framing, Data Overrun or Parity Error(s)
        ucsra = UCSR0A;
        udr   = UDR0;
        if(ucsra & ((1<<FE0) | (1<<DOR0) | (1<<UPE0)))
        {
            // Received data had an error, discard received data
            continue;
        }
        // Return received byte
        return udr;
    }
}

bool px_uart_rd_u8(uint8_t * data)
{
    uint8_t ucsra;
    uint8_t udr;

    // Data received flag is set?
    if(PX_BIT_IS_LO(UCSR0A,RXC0))
    {
        // No
        return false;
    }

    // Accept data only if there were no Framing, Data Overrun or Parity Error(s)
    ucsra = UCSR0A;
    udr   = UDR0;
    if(ucsra & ((1<<FE0)|(1<<DOR0)|(1<<UPE0)))
    {
        // Received data had an error, discard received data
        return false;
    }
    // Return received byte
    *data = udr;

    // Success
    return true;
}
