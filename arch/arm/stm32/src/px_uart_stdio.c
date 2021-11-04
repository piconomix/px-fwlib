/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_uart_stdio.h : Module to redirect stdio output (e.g. printf) to a UART
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include "px_compiler.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart_stdio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// UART handle
px_uart_handle_t * px_uart_stdio_handle;

#ifdef PX_COMPILER_ARM_CC
#pragma import(__use_no_semihosting_swi)
struct __FILE
{ 
    int handle;
};

FILE __stdout;
FILE __stdin;
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef PX_COMPILER_GCC_ARM
int _write(int file, char * ptr, int len)
{
    int i;

    for(i = 0; i < len; i++)
    {
        px_uart_stdio_put_char(*ptr++);
    }
    return len;
}

int _read(int file, char * ptr, int len)
{
    int i;

    for(i = 0; i < len; i++)
    {
        *ptr++ = px_uart_stdio_get_char();
    }
    return len;  
}
#endif

#ifdef PX_COMPILER_ARM_CC
int fputc(int ch, FILE * f)
{
    px_uart_stdio_put_char(ch);

    return 0;
}

int fgetc(FILE * f)
{
    return px_uart_stdio_get_char();
}

void _sys_exit(int return_code)
{
    while(1);
}
#endif

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_stdio_init(px_uart_handle_t * handle)
{
    // Save handle
    px_uart_stdio_handle = handle;

#ifdef PX_COMPILER_GCC_ARM
    // Disable IO buffering in libc to avoid putchar() bug. See:
    // https://bugs.launchpad.net/gcc-arm-embedded/+bug/1380268
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

int px_uart_stdio_put_char(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        px_uart_putchar(px_uart_stdio_handle, '\r');
    }
    // Send character over UART
    px_uart_putchar(px_uart_stdio_handle, (uint8_t)data);

    return 0;
}

int px_uart_stdio_get_char(void)
{
    // Receive character over UART
    return (int)px_uart_getchar(px_uart_stdio_handle);
}
