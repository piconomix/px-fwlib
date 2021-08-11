/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          ST Nucleo64 L053 CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2020-06-10
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_compiler.h"
#include "px_cli.h"
#include "px_cli_cmds.h"
#include "px_vt100.h"
#include "px_systmr.h"
#include "px_rtc.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_sysclk.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart2_handle;
uint8_t          main_buffer[MAIN_BUFFER_SIZE];


/* _____LOCAL VARIABLES______________________________________________________ */             
/// CLI splash text on start up. Back slashes ('\') have to be escaped ('\\')
static const char main_cli_init_str[] =
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
    px_rtc_init();
    px_uart_init();    

    // Open UART2
    px_uart_open2(&px_uart2_handle,
                  PX_UART_NR_2,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    // Connect stdin and stdout to UART
    px_uart_stdio_init(&px_uart2_handle);

    // Success
    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    // Initialize board and peripheral drivers
    main_init();
    // Enable LED
    PX_USR_LED_ON();
    // Report debug output
    PX_LOG_TRACE("Debug enabled\n");
    // Initialize CLI
    px_cli_init(px_cli_cmd_list, main_cli_init_str);

    for(;;)
    {
        // Received byte over UART?
        if(px_uart_rd_u8(&px_uart2_handle, &data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}

void main_log_putchar(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_log_putchar('\r');
    }

    px_uart_put_char(&px_uart2_handle, data);
}

void main_log_timestamp(char * str)
{
    px_rtc_date_time_t date_time;

    px_rtc_date_time_rd(&date_time);

    sprintf(str, "%02hu:%02hu:%02hu",
                 (unsigned short)date_time.hour,
                 (unsigned short)date_time.min,
                 (unsigned short)date_time.sec);
}
