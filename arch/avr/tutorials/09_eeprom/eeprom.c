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
 *  @defgroup AVR_TUT09_EEPROM 09_eeprom
 *  
 *  How to write to and read from EEPROM.
 *  
 *  File(s):
 *  - arch/avr/tutorials/09_eeprom/eeprom.c
 *  
 *  0xAB is written to EEPROM address 0x000C, read back and reported 
 *  over UART at 115200 BAUD, 8 Data Bits, No Parity and 1 Stop Bit.
 *  
 *  @tip_s
 *  [<avr/eeprom.h>](https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html)
 *  can also be used. It is not neccessary to use an explicit EEPROM address.
 *  The compiler will allocate a unique address if the EEMEM prefix is used.
 *  For example:
 *  @tip_e
 *  
 *  @code{.c}
 *      #include <stdint.h>
 *      #include <avr/eeprom.h>
 *
 *      EEMEM uint16_t ee_setting;
 *
 *      int main(void)
 *      {
 *          uint16_t value;
 *
 *          // Load value from EEPROM
 *          value = eeprom_read_word(&ee_setting);
 *          // Modify value
 *          value += 1;
 *          // Commit new value to EEPROM
 *          eeprom_write_word(&ee_setting, value);
 *          eeprom_busy_wait();
 *      }
 *  @endcode
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
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
    UCSR0C = (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

int uart_putchar(char data, FILE *stream)
{
    // Recursive function to prepend a carriage return before a line feed
    if(data == '\n')
    {
        uart_putchar('\r', stream);
    }
    // Wait until data register is empty
    while((UCSR0A & (1 << UDRE0)) == 0) {;}
    // Load transmit register with new data
    UDR0 = data;

    return 0;
}

// Declare stream object in data memory (not heap memory)
FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

uint8_t ee_read_byte(uint16_t adr)
{
    // Set address
    EEAR = adr;
    // Start read operation
    EECR |= (1 << EERE);
    // Return data
    return EEDR;
}

void ee_write_byte(uint16_t adr, uint8_t data)
{
    uint8_t sreg;

    // Select EEPROM Programming mode: Erase and Write in one operation (Atomic Operation)
    EECR &= ~((1 << EEPM1) | (1 << EEPM0));
    // Set address
    EEAR = adr;
    // Set data to write
    EEDR = data;
    // Save status of global interrupts
    sreg = SREG;
    // Disable interrupts
    cli();
    // Start write operation
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
    // Restore status of global interrupts
    SREG = sreg;
    // Wait until write operation has finished
    while(EECR & (1 << EEPE)) {;}
}
int main(void)
{
    uint8_t data;

    // Initialise board
    px_board_init();
    // Initialise UART
    uart_init();
    // Direct stdout stream to uart_stream
    stdout = &uart_stream;

    PRINTF_P("\nEEPROM Tutorial\n");

    // Write a byte to EEPROM
    ee_write_byte(0x000C, 0xAB);
    // Read a byte from EEPROM
    data = ee_read_byte(0x000C);

    PRINTF_P("EEPROM value at adr 0x000C is 0x%02X\n", data);

    // Loop forever
    while(true) {;}
}
