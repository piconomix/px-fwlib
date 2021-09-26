/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_board.h : ATmega328PB-XMINI Board
    Author(s):      Pieter Conradie
    Creation Date:  2021-09-26

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include <util/delay.h>
#include "px_gpio_init_port_b.h"
#include "px_gpio_init_port_c.h"
#include "px_gpio_init_port_d.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("board");

// Check that correct 'px_board.h' has been included
#ifndef BOARD_AVR_ATMEGA328PB_XMINI
#warning "Wrong 'px_board.h' has been included. Check that include path is correct."
#endif

// Report board support package
#warning "This BSP is for ATmega328PB-XMINI Board"

// Test for correct microcontroller
#ifndef __AVR_ATmega328PB__
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
