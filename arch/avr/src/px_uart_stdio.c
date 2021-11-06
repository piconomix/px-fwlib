/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_uart_stdio.h : Module to redirect stdio output (e.g. printf) to a UART
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart_stdio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// UART handle
static px_uart_handle_t * px_uart_stdio_handle;

/// Declare stream object in data memory (not heap memory)
FILE px_uart_stdio_stream = FDEV_SETUP_STREAM(px_uart_stdio_putchar,
                                              px_uart_stdio_getchar,
                                              _FDEV_SETUP_RW);

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_stdio_init(px_uart_handle_t * handle)
{
    // Save handle
    px_uart_stdio_handle = handle;

    // Redirect stdout and stdin stream to px_uart_stdio_stream
    stdout = &px_uart_stdio_stream;
    stdin  = &px_uart_stdio_stream;
}

int px_uart_stdio_putchar(char data, FILE *stream)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        px_uart_stdio_putchar('\r', stream);
    }

    // Send character over UART
    px_uart_putchar(px_uart_stdio_handle, data);

    return 0;
}

int px_uart_stdio_getchar(FILE *stream)
{
    // Receive character over UART
    return (int)px_uart_getchar(px_uart_stdio_handle);
}
