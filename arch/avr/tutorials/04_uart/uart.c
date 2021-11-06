/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006-2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup AVR_TUTORIALS 
 *  @defgroup AVR_TUT04_UART 04_uart
 *  
 *  How to configure and use a UART in a polled fashion.
 *  
 *  File(s):
 *  - arch/avr/tutorials/04_uart/uart.c 
 *  
 *  The UART is configured to 115200 BAUD, 8 Data Bits, No Parity and 1 Stop Bit. 
 *  The string "UART\r\n" is sent upon start up. Thereafter all received 
 *  characters are echoed back to the sender.
 *  
 *  The clock rate of the AVR is defined in "board.h" with the macro @b F_CPU.
 *  It is a good programming practice to calculate values at compile 
 *  time, instead of using "magic values". For example,  the value to use for 
 *  the UBBR register is calculated with the following macro: 
 *  
 *  @code{.c}
 *      #define UART0_UBBR_VAL  ((F_CPU / (UART0_BAUD * 16)) - 1)
 *  @endcode
 *  
 *  instead of using a hard coded value:
 *  
 *  @code{.c}
 *      #define UART0_UBBR_VAL   3 // Magic Value for 115200 Baud at a clock rate of 7.3728 MHz
 *  @endcode
 */

#include <stdint.h>
#include <avr/io.h>

#include "px_board.h"

// Define baud rate
#define UART0_BAUD      115200ul
#define UART0_UBBR_VAL  ((F_CPU / (UART0_BAUD * 16)) - 1)

void uart_init(void)
{
    // Set baud rate
    UBRR0 = UART0_UBBR_VAL;
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void uart_send_byte(uint8_t data)
{
    // Wait until data register is empty
    while((UCSR0A & (1 << UDRE0)) == 0) {;}
    // Load transmit register with new data
    UDR0 = data; 
}

uint8_t uart_receive_byte(void)
{
    // Wait until a byte has been received
    while((UCSR0A & (1<<RXC0)) == 0) {;}
    // Return received data
    return UDR0;
}

int main(void)
{
    uint8_t data;

    // Initialise board
    px_board_init();
    // Initialise UART
    uart_init();

    // Send string
    uart_send_byte('U');
    uart_send_byte('A');
    uart_send_byte('R');
    uart_send_byte('T');
    uart_send_byte('\r');
    uart_send_byte('\n');

    // Repeat forever
    for(;;)
    {
        // Echo received characters
        data = uart_receive_byte();
        uart_send_byte(data);
    }
}
