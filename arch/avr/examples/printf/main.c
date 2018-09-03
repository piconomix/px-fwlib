/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012-2014 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          printf.h : Microchip AVR printf example
    Author(s):      Pieter Conradie
    Creation Date:  2007-04-27
 
============================================================================= */

/** 
 *  @ingroup AVR_EXAMPLES
 *  @defgroup AVR_EX_PRINTF printf : Microchip AVR printf example
 *   
 *  This example initialised a UART, redirects stdout to the UART and 
 *  demonstrates how to print strings stored in program memory and RAM.
 *   
 *  File(s): 
 *  - arch/avr/examples/printf/main.c 
 *   
 *  The following modules are exercised: 
 *  - @ref AVR_UART 
 *  - @ref AVR_UART_STDIO 
 *  - @ref PX_PGM_P 
 *   
 *  See also: 
 *  - [Tutorial 5 - printf](@ref AVR_TUT05_PRINTF)
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_pgm_P.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
// Declare UART handle structure
static px_uart_handle_t px_uart_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    // Initialise modules
    px_board_init();
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_PER_0,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&px_uart_handle);

    // Enable interrupts
    px_interrupts_enable();

    // String stored in program memory and copied to SRAM upon startup by CRT
    printf("printf - String in data memory (RAM)\n");

    // String stored in and accessed from program memory
    printf_P(PX_PGM_STR("printf_P - String in program memory (ROM)\n"));

    // Convenience macro to store and access string in program memory
    PX_PRINTF_P("PRINTF_P - String also in program memory (ROM)\n");

    // Repeat forever
    for(;;)
    {
        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Echo byte
            px_uart_wr_u8(&px_uart_handle, data);
        }
    }
}
