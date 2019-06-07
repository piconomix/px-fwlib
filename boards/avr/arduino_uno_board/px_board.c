/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_board.h : Arduino UNO R3 Board
    Author(s):      Pieter Conradie
    Creation Date:  2015-01-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include <util/delay.h>
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("board");

// Check that correct 'px_board.h' has been included
#ifndef BOARD_AVR_ARDUINO_UNO
#warning "Wrong 'px_board.h' has been included. Check that include path is correct."
#endif

// Report board support package
#warning "This BSP is for Arduino UNO R3 Board"

// Test for correct microcontroller
#ifndef __AVR_ATmega328P__
#warning "This Board contains a different microcontroller"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_board_init(void)
{
    // Initialise General Purpose Input/Output pins
    PORTB = GPIO_PORTB_INIT_VAL;      
    DDRB  = GPIO_DDRB_INIT_VAL;

    PORTC = GPIO_PORTC_INIT_VAL;
    DDRC  = GPIO_DDRC_INIT_VAL;

    PORTD = GPIO_PORTD_INIT_VAL;
    DDRD  = GPIO_DDRD_INIT_VAL;
}

void px_board_spi_cs_lo(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI_CS_NONE:               break;
    case PX_BOARD_SPI_SS:
        PX_GPIO_PIN_SET_LO(PX_GPIO_SPI_SS);  break;
    case PX_BOARD_SPI_D2:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D2);      break;
    case PX_BOARD_SPI_D3:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D3);      break;
    case PX_BOARD_SPI_D4:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D4);      break;
    case PX_BOARD_SPI_D5:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D5);      break;
    case PX_BOARD_SPI_D6:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D6);      break;
    case PX_BOARD_SPI_D7:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D7);      break;
    case PX_BOARD_SPI_D8:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D8);      break;
    case PX_BOARD_SPI_D9:
        PX_GPIO_PIN_SET_LO(PX_GPIO_D9);      break;
    default:
        PX_DBG_ASSERT(false);
        break;
    }
}

void px_board_spi_cs_hi(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI_CS_NONE:               break;
    case PX_BOARD_SPI_SS:
        PX_GPIO_PIN_SET_HI(PX_GPIO_SPI_SS);  break;
    case PX_BOARD_SPI_D2:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D2);      break;
    case PX_BOARD_SPI_D3:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D3);      break;
    case PX_BOARD_SPI_D4:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D4);      break;
    case PX_BOARD_SPI_D5:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D5);      break;
    case PX_BOARD_SPI_D6:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D6);      break;
    case PX_BOARD_SPI_D7:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D7);      break;
    case PX_BOARD_SPI_D8:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D8);      break;
    case PX_BOARD_SPI_D9:
        PX_GPIO_PIN_SET_HI(PX_GPIO_D9);      break;
    default:
        PX_DBG_ASSERT(false);
        break;
    }
}

void px_board_delay_us(uint16_t delay_us)
{
    while(delay_us != 0)
    {
        _delay_us(1);
        delay_us--;
    }
}

void px_board_delay_ms(uint16_t delay_ms)
{
    while(delay_ms != 0)
    {
        px_board_delay_us(1000);
        delay_ms--;
    }
}
