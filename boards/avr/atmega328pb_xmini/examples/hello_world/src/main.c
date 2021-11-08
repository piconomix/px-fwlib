/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          "hello world" example
    Author(s):      Pieter Conradie
    Creation Date:  2021-09-26
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_pgm_P.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_systmr_t      px_systmr;
static px_uart_handle_t px_uart_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_uart_init();
    /* Supposed to be 115200 but compensating for imprecise BAUD rate of mEDBG debugger */
    px_uart_open2(&px_uart_handle, PX_UART_NR_0, 111111,
                  PX_UART_DATA_BITS_8, PX_UART_PARITY_NONE, PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&px_uart_handle);
    // Enable interrupts
    px_interrupts_enable();
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    // Initialise board
    main_init();

    // Enable LED
    PX_LED_ON();
    // Start LED timer
    px_systmr_start(&px_systmr, PX_SYSTMR_MS_TO_TICKS(500));
    // Send message
    PX_PRINTF_P("Hello World!\n");

    // Loop forever
    while(true)
    {
        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Echo byte
            px_uart_wr_u8(&px_uart_handle, data);
        }
        // LED timer expired?
        if(px_systmr_has_expired(&px_systmr))
        {
            // Toggle LED
            PX_LED_TOGGLE();
            // Button pressed?
            if(PX_BTN_IS_PRESSED())
            {
                // Fast flash
                px_systmr_start(&px_systmr, PX_SYSTMR_MS_TO_TICKS(50));
            }
            else
            {
                // Slow flash
                px_systmr_start(&px_systmr, PX_SYSTMR_MS_TO_TICKS(500));
            }
        }
    }
}
