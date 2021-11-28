/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 - 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup AVR_TUTORIALS 
 *  @defgroup AVR_TUT07_ADC 07_adc
 *  
 *  How to configure and use one channel of the ADC converter in a polled
 *  fashion.
 *  
 *  File(s): 
 *  - arch/avr/tutorials/07_adc/adc.c
 *  
 *  The analog voltage of ADC0 is measured once a second and reported via UART
 *  at 115200 BAUD, 8 Data Bits, No Parity and 1 Stop Bit. A voltage from +0V
 *  to +3.3V may be applied.
 */

#include <stdint.h>
#include <stdbool.h>
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
    UCSR0C = (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

int uart_putchar(char data, FILE * stream)
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

void tmr_init(void)
{
    /* 
     *  Start 16-bit TMR1 with clock prescaler CLK/1024. For F_CPU = 7372800 Hz,
     *  resolution is 139 us, maximum time is 9.1 s
     *
     *  Select Clear Timer on Compare match (CTC) mode of operation. This means
     *  that when TCNT1 reaches the OCR1A value, OCF1A flag will be set and TCNT1
     *  will be reset to 0.
     */ 
    TCCR1A = (0 << WGM11) | (0 << WGM10);
    TCCR1B = (0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10);
}

void tmr_delay(uint16_t delay_ms)
{
    // Calculate and set delay
    OCR1A = ((F_CPU / 1024) * delay_ms) / 1000;
    // Reset counter
    TCNT1 = 0;
    // Clear OCF1A flag by writing a logical 1; other flags are unchanged
    // This is more efficient than using "TIFR |= (1 << OCF1A);"
    TIFR1 = (1 << OCF1A);
    // Wait until OCF1A flag is set
    while((TIFR1 & (1 << OCF1A)) == 0) {;}
}

void adc_init(void)
{
    /* 
     *  Select AVCC as reference with external capacitor at AREF pin
     *  Select ADC0 (MUX3...0 = 0000b)
     */
    ADMUX =   (0 << REFS1) | (1 << REFS0)
            | (0 << MUX3)  | (0 << MUX2)  | (0 << MUX1) | (0 << MUX0);
    /*
     *  Enable ADC; Select ADC Prescaler of 128 resulting in an ADC clock
     *  frequency of 57.6 kHz with F_CPU = 7.3728 MHz
     */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_get_sample(void)
{
    // Start conversion
    ADCSRA |= (1 << ADSC);
    /*
     *  Wait until conversion is finished, which normally takes 13 ADC clock 
     *  cycles (226 us).
     */
    while(ADCSRA & (1 << ADSC)) {;}

    // Return sampled value
    return ADC;
}

int main(void)
{
    uint16_t adc;

    // Initialise board
    px_board_init();
    // Initialise UART
    uart_init();
    // Direct stdout stream to uart_stream
    stdout = &uart_stream;
    // Initialise timer
    tmr_init();
    // Initialise ADC
    adc_init();

    // Loop forever
    while(true)
    {
        // Retrieve a sample
        adc = adc_get_sample();
        // Report value
        PRINTF_P("ADC value is %d\n", adc);
        // Wait 1 s
        tmr_delay(1000);
    }
}
