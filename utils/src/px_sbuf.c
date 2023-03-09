/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_sbuf.h : Safe buffer string appending functions that avoid overflow
    Author(s):      Pieter Conradie
    Creation Date:  2021-04-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_sbuf.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_sbuf_init(px_sbuf_t * sbuf, char * buf, size_t buf_size)
{
    sbuf->buf      = buf;
    sbuf->buf_size = buf_size;
    sbuf->index    = 0;
    sbuf->buf[0]   = '\0';
}

void px_sbuf_malloc(px_sbuf_t * sbuf, size_t buf_size)
{
    char * buf = malloc(buf_size);
    px_sbuf_init(sbuf, buf, buf_size);
}

void px_sbuf_free(px_sbuf_t * sbuf)
{
    free(sbuf->buf);
    memset(sbuf, 0, sizeof(*sbuf));
}

void px_sbuf_reset(px_sbuf_t * sbuf)
{
    sbuf->index    = 0;
    sbuf->buf[0]   = '\0';
}

bool px_sbuf_is_empty(const px_sbuf_t * sbuf)
{
    if(sbuf->index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_sbuf_is_full(const px_sbuf_t * sbuf)
{
    if(sbuf->index >= (sbuf->buf_size - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t px_sbuf_get_size_remaining(const px_sbuf_t * sbuf)
{
    if(sbuf->buf_size <= sbuf->index)
    {
        return 0;
    }
    else
    {
        return (sbuf->buf_size - sbuf->index);
    }
}

void px_sbuf_putchar(px_sbuf_t * sbuf, char c)
{
    // Buffer full?
    if(px_sbuf_is_full(sbuf))
    {
        return;
    }
    // Append char
    sbuf->buf[sbuf->index] = c;
    // Adjust to new position
    sbuf->index++;
    // Zero terminate
    sbuf->buf[sbuf->index] = '\0';
}

void px_sbuf_puts(px_sbuf_t * sbuf, const char * str)
{
    px_sbuf_print(sbuf, str);
    px_sbuf_putchar(sbuf, '\n');
}

void px_sbuf_print(px_sbuf_t * sbuf, const char * str)
{
    char c;

    // Buffer full?
    if(px_sbuf_is_full(sbuf))
    {
        return;
    }
    // Append string
    while(true)
    {
        // Read char
        c = *str++;
        // End of string?
        if(c == '\0')
        {
            break;
        }
        // Write char
        sbuf->buf[sbuf->index] = c;
        // Next index
        sbuf->index++;
        // End of buffer?
        if(sbuf->index == (sbuf->buf_size - 1))
        {
            break;
        }
    }
    // Zero terminate
    sbuf->buf[sbuf->index] = '\0';
}

void px_sbuf_printf(px_sbuf_t * sbuf, const char * format, ...)
{
    va_list args;
    size_t  rem_buf_size;
    int     i;

    // Buffer full?
    if(px_sbuf_is_full(sbuf))
    {
        return;
    }
    // Calculate remaining size
    rem_buf_size = sbuf->buf_size - sbuf->index;
    // Append formatted string
    va_start(args, format);
    i = vsnprintf(&sbuf->buf[sbuf->index], rem_buf_size, format, args);
    va_end(args);
    // Error?
    if(i <= 0)
    {
        // Zero terminate to remove error
        sbuf->buf[sbuf->index] = '\0';
        return;
    }
    // Overflow?
    if(i > rem_buf_size)
    {
        i = rem_buf_size;
    }
    // Adjust to new position
    sbuf->index += i;
}

void px_sbuf_append(px_sbuf_t * sbuf, const char * data, size_t nr_of_bytes)
{
    while(nr_of_bytes != 0)
    {
        // Buffer full?
        if(px_sbuf_is_full(sbuf))
        {
            return;
        }
        // Append byte
        sbuf->buf[sbuf->index] = *data++;
        // Next byte
        sbuf->index++;
        nr_of_bytes--;
    }
}
