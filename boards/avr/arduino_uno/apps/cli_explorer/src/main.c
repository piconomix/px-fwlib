/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Arduino Uno Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2015-01-31
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/sleep.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_pgm_P.h"
#include "px_cli.h"
#include "px_cli_cmds.h"
#include "px_systmr.h"
#include "px_rtc_util.h"

#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart_handle;
px_i2c_handle_t  px_i2c_handle;

/* _____LOCAL VARIABLES______________________________________________________ */
/// CLI splash text on start up. Back slashes ('\') have to be escaped ('\\')
PX_ATTR_PGM static const char main_cli_init_str[] =
    " ____   __  __ TM\n"
    "|  _ \\  \\ \\/ /\n"
    "| |_) |  \\  /\n"
    "|  __/   /  \\ PICONOMIX - Embedded Elegance\n"
    "|_|     /_/\\_\\    https://piconomix.com\n\n";

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_rtc_util_init();
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_0,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&px_uart_handle);

    px_spi_init();
    px_i2c_init();
    px_i2c_open(&px_i2c_handle, PX_I2C_NR_0, 0x00);
    
    // Enable interrupts
    px_interrupts_enable();

    // Success
    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    main_init();

    // Initialise CLI (after STDIO is ready)
    px_cli_init(px_cli_cmd_list, main_cli_init_str);

    for(;;)
    {
        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }

        // Sleep in Idle mode until an interrupt wakes up the CPU
        set_sleep_mode(SLEEP_MODE_IDLE); 
        sleep_mode();
    }
}
