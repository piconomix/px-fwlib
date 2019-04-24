#ifndef __PX_ONE_WIRE_CFG_H__
#define __PX_ONE_WIRE_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
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
 
    Title:          px_one_wire.h : Dallas/Maxim 1-Wire bit-bang driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2017-01-27

============================================================================= */

/** 
 *  @addtogroup PX_ONE_WIRE
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_gpio.h"
#include "px_compiler.h"

/* _____DEFINITIONS__________________________________________________________ */
/**
 *  Initialise 1-Wire GPIO pin as an input with internal pull-up disabled. 
 *  There must be an external 4k7 pull-up resistor between +3V3 and 1-Wire GPIO 
 *  pin. 
 *   
 *  When the 1-Wire GPIO pin's direction is changed from input to output, it 
 *  will output a LOW. 
 */
#define PX_ONE_WIRE_CFG_PIN_INIT() \
    { \
        PX_GPIO_DIR_SET_IN(PX_GPIO_0); \
        PX_GPIO_PULLUP_DISABLE(PX_GPIO_0); \
        PX_GPIO_PIN_SET_LO(PX_GPIO_0); \
    }

/// Set 1-Wire GPIO pin as an input
#define PX_ONE_WIRE_CFG_PIN_DIR_SET_IN()    PX_GPIO_DIR_SET_IN(PX_GPIO_0)

/// Set 1-Wire GPIO pin as an output LOW
#define PX_ONE_WIRE_CFG_PIN_SET_LO()        PX_GPIO_DIR_SET_OUT(PX_GPIO_0)

/// Is 1-Wire GPIO pin input high?
#define PX_ONE_WIRE_CFG_PIN_IS_HI()         PX_GPIO_PIN_IS_HI(PX_GPIO_0)

/// Is 1-Wire GPIO pin input low?
#define PX_ONE_WIRE_CFG_PIN_IS_LO()         PX_GPIO_PIN_IS_LO(PX_GPIO_0)

/// Delay trim factor in microseconds
#define PX_ONE_WIRE_CFG_DELAY_TRIM_US       0

#ifdef PX_COMPILER_GCC_AVR
#include <util/delay.h>
/// Delay function in microseconds
#define PX_ONE_WIRE_CFG_DELAY_US(delay_us)  _delay_us(delay_us - (PX_ONE_WIRE_CFG_DELAY_TRIM_US))
#else
/// Delay function in microseconds
#define PX_ONE_WIRE_CFG_DELAY_US(delay_us)  px_board_delay_us(delay_us - (PX_ONE_WIRE_CFG_DELAY_TRIM_US))
#endif

/// Disable interrupts during critical delay timing
#define PX_ONE_WIRE_CFG_INTS_DISABLE()      px_interrupts_disable()

/// Enable interrupts after critical delay timing
#define PX_ONE_WIRE_CFG_INTS_ENABLE()       px_interrupts_enable()

/// @}
#endif // #ifndef __PX_ONE_WIRE_CFG_H__
