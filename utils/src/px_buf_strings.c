/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2023 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_buf_strings.h : Buffer containing multiple strings
    Author(s):      Pieter Conradie
    Creation Date:  2023-04-25

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_buf_strings.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_buf_strings_init(px_buf_strings_t * buf_strings, char * buf, uint16_t buf_size)
{
    buf_strings->buf      = buf;
    buf_strings->buf_size = buf_size;
    buf_strings->index    = 0;
    buf_strings->buf[0]   = '\0';
    buf_strings->str      = buf;
}

bool px_buf_strings_malloc(px_buf_strings_t * buf_strings, uint16_t buf_size)
{
    char * buf = malloc(buf_size);
    if(buf != NULL)
    {
        px_buf_strings_init(buf_strings, buf, buf_size);
        return true;
    }
    else
    {
        memset(buf_strings, 0, sizeof(*buf_strings));
        return false;
    }
}

void px_buf_strings_free(px_buf_strings_t * buf_strings)
{
    free(buf_strings->buf);
    memset(buf_strings, 0, sizeof(*buf_strings));
}

void px_buf_strings_reset(px_buf_strings_t * buf_strings)
{
    buf_strings->index    = 0;
    buf_strings->buf[0]   = '\0';
    buf_strings->str      = buf_strings->buf;
}

bool px_buf_strings_is_empty(const px_buf_strings_t * buf_strings);

bool px_buf_strings_is_full(const px_buf_strings_t * buf_strings);

uint16_t px_buf_strings_get_size_remaining(const px_buf_strings_t * buf_strings)
{
    if(buf_strings->buf_size <= (buf_strings->index + 1))
    {
        return 0;
    }
    else
    {
        return (buf_strings->buf_size - buf_strings->index - 1);
    }
}

bool px_buf_strings_append_char(px_buf_strings_t * buf_strings, char c)
{
    // Buffer full?
    if(px_buf_strings_is_full(buf_strings))
    {
        // Failure
        return false;
    }
    // Append char
    buf_strings->buf[buf_strings->index] = c;
    // Adjust to new position
    buf_strings->index++;
    // Zero terminate
    buf_strings->buf[buf_strings->index] = '\0';
    // New string?
    if(c == '\0')
    {
        // Move to new string
        buf_strings->str = &buf_strings->buf[buf_strings->index];
    }
    // Success
    return true;
}

void px_buf_strings_append_str(px_buf_strings_t * buf_strings, const char * str)
{
    // String empty?
    if(*str == '\0')
    {
        // Ignore
        return;
    }
    // Append characters in string until buffer is full
    do
    {
        if(!px_buf_strings_append_char(buf_strings, *str++)) break;
    }
    while(*str != '\0');
}

const char * px_buf_strings_get_current_str(const px_buf_strings_t * buf_strings);

bool px_buf_strings_is_current_str_empty(const px_buf_strings_t * buf_strings);

uint16_t px_buf_strings_get_count(px_buf_strings_t * buf_strings)
{
    uint16_t     count;
    const char * c;

    // Start at beginning of buffer
    count = 0;
    c = buf_strings->buf;
    // Current string empty?
    while(*c != '\0')
    {
        // No. Increment count
        count++;
        // Find start of next string
        while(*c++ != '\0') {;}
    }

    // Return count
    return count;
}

const char * px_buf_strings_get_first(px_buf_strings_t * buf_strings)
{
    // Start at beginning of buffer
    buf_strings->str = buf_strings->buf;
    // String empty? Stop
    if(*buf_strings->str == '\0') return NULL;

    // Return first string
    return buf_strings->str;
}

const char * px_buf_strings_get_next(px_buf_strings_t * buf_strings)
{
    const char * c = buf_strings->str;

    // Current string empty? Stop
    if(*c == '\0') return NULL;
    // Find start of next string
    while(*c++ != '\0') {;}
    // Next string empty? Stop
    if(*c == '\0') return NULL;

    // Return next string
    buf_strings->str = c;
    return c;
}

