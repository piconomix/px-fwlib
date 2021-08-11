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
 *  @defgroup AVR_TUT05_PRINTF 05_printf
 *  
 *  How to direct the AVR Libc stdout stream to send characters using a UART.
 *  
 *  File(s): 
 *  - arch/avr/tutorials/05_printf/printf.c 
 *  
 *  The UART is configured to 115200 BAUD, 8 Data Bits, No Parity and 1 Stop Bit. 
 *  The following strings are sent upon start up:
 *  
 *      printf - String in data memory (SRAM)
 *      printf_P - String in program memory (FLASH)
 *      PRINTF_P - String in program memory (FLASH)
 *  
 *  @tip_s
 *  Remember that the XMODEM-CRC bootloader will send a 'C' character upon
 *  reset and this will be shown in addition to the printf strings.
 *  @tip_e
 *  
 *  A custom handler "uart_put_char(...)" is linked to the stdout stream to 
 *  send a character and inject a "\r" (Carriage Return) before every
 *  "\n" (Line Feed).
 *  
 *  This example draws attention to the handling of the Harvard architecture by 
 *  the GCC compiler and the AVR Libc library. The 8-bit AVRs have a small
 *  amount of SRAM (data memory) and therefore it is desirable to store constant
 *  strings in FLASH (program memory) and access it from there and not use a
 *  copy in SRAM. 
 *  
 *  The recommended solution is to use the macros in
 *  [<avr/pgmspace.h>](https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html)
 *  and string handling functions with the suffix "_P", e.g. "printf_P(...)"
 *  instead of "printf(...)". This handling has been encapsulated with the
 *  following variable argument macro:
 *  
 *      @code{.c}
 *      #define PRINTF_P(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)
 *      @endcode
 *  
 *  All of this functionality has been encapsulated in the following modules:
 *  
 *  - @ref AVR_UART_STDIO
 *  - @ref PX_PGM_P
 *  
 *  See:
 *  - @ref AVR_EX_PRINTF
 *  
 *  More info:
 *  - [AVR-GCC and the PROGMEM Attribute](http://www.fourwalledcubicle.com/AVRArticles.php)
 *    by Dean Camera
 *  - [avr-libc: Data in Program Space](http://www.nongnu.org/avr-libc/user-manual/pgmspace.html)
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "px_board.h"

// Macro to send strings stored in program memory space
#define PRINTF_P(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

// Define baud rate
#define UART0_BAUD      115200ul
#define UART0_UBBR_VAL  ((F_CPU / (UART0_BAUD * 16)) - 1)

void uart_init(void)
{
    // Set baud rate
    UBRR0 = UART0_UBBR_VAL;

    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

int uart_put_char(char data, FILE * stream)
{
    // Recursive function to prepend a carriage return before a line feed
    if(data == '\n')
    {
        uart_put_char('\r', stream);
    }
    // Wait until data register is empty
    while((UCSR0A & (1<<UDRE0)) == 0)
    {
        ;
    }
    // Load transmit register with new data
    UDR0 = data;

    return 0;
}

// Declare stream object in data memory (not heap memory)
FILE uart_stream = FDEV_SETUP_STREAM(uart_put_char, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
    // Initialise board
    px_board_init();

    // Initialise UART
    uart_init();

    // Direct stdout stream to uart_stream
    stdout = &uart_stream;

    // String stored in program memory and copied to SRAM upon startup by
    // C Run Time set up (CRT).
    printf("printf - String in data memory (SRAM)\n");

    // String stored in and accessed from program memory
    printf_P(PSTR("printf_P - String in program memory (FLASH)\n"));

    // Convenience macro to store and access string in program memory
    PRINTF_P("PRINTF_P - String also in program memory (FLASH)\n");

    // Repeat forever
    for(;;)
    {
        ;
    }
}
