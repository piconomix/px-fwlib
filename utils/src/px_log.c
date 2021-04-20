/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_log.h : Debug logging module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Number of bytes per row for hex dump
#define PX_LOG_TRACE_DATA_BYTES_PER_ROW    16

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate internal buffer for log output
static char px_log_buf[PX_LOG_CFG_BUF_SIZE];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void inline px_log_putchar(char data)
{
#ifdef PX_LOG_CFG_PUTCHAR
    // Output character using configured function
    PX_LOG_CFG_PUTCHAR(data);
#else
    // Output character to stdout
    putchar(data);
#endif
}

static void px_log_print_str(const char  * data)
{
    while(*data != '\0')
    {
        px_log_putchar(*data++);
    }
}

static void inline px_log_print_hex04(uint8_t data)
{
    if(data < 10)
    {
        data += (uint8_t)'0';
    }
    else
    {
        data += (uint8_t)'A' - 10;
    }
    px_log_putchar((char)data);
}

static void inline px_log_print_hex08(uint8_t data)
{
    px_log_print_hex04(PX_U8_HI4(data));
    px_log_print_hex04(PX_U8_LO4(data));
}

#ifdef PX_COMPILER_GCC_AVR
static void px_log_print_str_P(const char * data)
{
    char c;

    while(true)
    {
        c = px_pgm_rd_char(data++);
        if(c == '\0')
        {
            return;
        }
        px_log_putchar(c);
    }
}

static void px_log_printf_vargs_P(const char * format, va_list args)
{
    size_t len;

    // Populate buffer with log string
    vsnprintf_P(px_log_buf, PX_LOG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_log_buf[PX_LOG_CFG_BUF_SIZE - 1] = '\0';
    // Remove tab character ('\t') at the end of user formatted string
    len = strlen(px_log_buf);
    if((len != 0) && (px_log_buf[len - 1] == '\t'))
    {
        px_log_buf[len - 1] = '\0';
    }
    // Output user formatted string
    px_log_print_str(px_log_buf);
}

static void px_log_printf_P(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_log_printf_vargs_P(format, args);
    va_end(args);
}

#else

static void px_log_printf_vargs(const char * format, va_list args)
{
    size_t len;

    // Populate buffer with log string
    vsnprintf(px_log_buf, PX_LOG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_log_buf[PX_LOG_CFG_BUF_SIZE - 1] = '\0';
    // Remove tab character ('\t') at the end of user formatted string
    len = strlen(px_log_buf);
    if((len != 0) && (px_log_buf[len - 1] == '\t'))
    {
        px_log_buf[len - 1] = '\0';
    }
    // Output user formatted string
    px_log_print_str(px_log_buf);
}

static void px_log_printf(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_log_printf_vargs(format, args);
    va_end(args);
}
#endif

static void px_log_report_log_prefix(px_log_level_t level,
                                     const char *   name,
                                     uint16_t       line)
{
    switch(level)
    {
    case PX_LOG_LEVEL_ERROR:
#if PX_LOG_CFG_COLOR
        px_log_print_str(PX_LOG_CF_RED "E");
#else
        px_log_putchar('E');
#endif
        break;

    case PX_LOG_LEVEL_WARNING:
#if PX_LOG_CFG_COLOR
        px_log_print_str(PX_LOG_CF_YELLOW "W");
#else
        px_log_putchar('W');
#endif
        break;

    case PX_LOG_LEVEL_INFO:
#if PX_LOG_CFG_COLOR
        px_log_print_str(PX_LOG_CF_GREEN "I");
#else
        px_log_putchar('I');
#endif
        break;

    case PX_LOG_LEVEL_DEBUG:
#if PX_LOG_CFG_COLOR
        px_log_print_str(PX_LOG_CF_BLUE "D");
#else
        px_log_putchar('D');
#endif
        break;

    case PX_LOG_LEVEL_VERBOSE:
#if PX_LOG_CFG_COLOR
        px_log_print_str(PX_LOG_CF_CYAN "V");
#else
        px_log_putchar('V');
#endif
        break;

    default:
        px_log_putchar('?');
        break;
    }
    px_log_putchar(' ');

    // Timestamp function provided in 'px_log_cfg.h'?
#ifdef PX_LOG_CFG_TIMESTAMP
    // Get user supplied timestamp string
    PX_LOG_CFG_TIMESTAMP(px_log_buf);
    // Append terminating zero in case of buffer overrun
    px_log_buf[PX_LOG_CFG_BUF_SIZE - 1] = '\0';
    // Output timestamp
    px_log_print_str(px_log_buf);
    px_log_putchar(' ');
#endif

#if PX_LOG_CFG_COLOR
    // Send VT100 sequence to reset all attributes
    px_log_print_str(PX_LOG_CR);
#endif

    // Output file and line
#ifdef PX_COMPILER_GCC_AVR
    px_log_print_str_P(name);
    px_log_printf_P(PX_PGM_STR(" %04u : "), line);
#else
    px_log_printf("%s %04u : ", name, line);
#endif
}

static void _px_log_log_terminate(const char * format)
{
#ifdef PX_COMPILER_GCC_AVR
    size_t len = strlen_P(format);
    if((len == 0) || px_pgm_rd_char(format + len - 1) != '\t')
    {
        // Append end-of-line
        px_log_putchar('\n');
    }
#else
    size_t len = strlen(format);
    if((len == 0) || format[len - 1] != '\t')
    {
        // Append end-of-line
        px_log_putchar('\n');
    }
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_log_log_error(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_ERROR, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);

    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_log_log_terminate(format);
}

void _px_log_log_warning(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_WARNING, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);

    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_log_log_terminate(format);
}

void _px_log_log_info(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_INFO, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);

    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_log_log_terminate(format);
}

void _px_log_log_debug(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_DEBUG, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);

    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_log_log_terminate(format);
}

void _px_log_log_verbose(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_VERBOSE, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);

    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_log_log_terminate(format);
}

void _px_log_assert(const char * name,
                    uint16_t     line, 
                    const char * expression)
{
    // Output log prefix (level, timestamp, name and line)
    px_log_report_log_prefix(PX_LOG_LEVEL_ERROR, name, line);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_P(PX_PGM_STR("ASSERT: "));
    px_log_printf_P(expression);
#else
    px_log_printf("ASSERT: ");
    px_log_printf(expression);
#endif

    // Block forever
    for(;;)
    {
        ;
    }
}

void _px_log_trace(const char * format, ...)
{
    va_list args;

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_log_printf_vargs_P(format, args);
#else
    px_log_printf_vargs(format, args);
#endif
    va_end(args);
}

void _px_log_trace_data(const void * data, size_t nr_of_bytes)
{
    size_t          i;
    const uint8_t * data_u8 = (const uint8_t *)data;

    for(i = 0; i < nr_of_bytes; i++)
    {
        px_log_print_hex08(*data_u8++);
        px_log_putchar(' ');
    }
}

void _px_log_trace_hexdump(const void * data, size_t nr_of_bytes)
{
    size_t          i, j;
    const uint8_t * row_data;

    // Split data up into rows
    for(i = 0; i < nr_of_bytes; i+= PX_LOG_TRACE_DATA_BYTES_PER_ROW)
    {
        // Insert extra empty row?
        if(  (i != 0) && ((i % (PX_LOG_TRACE_DATA_BYTES_PER_ROW * 4)) == 0)  )
        {
            // Yes
            px_log_putchar('\n');
        }

        // Select row data
        row_data = &((const uint8_t *)data)[i];

        // Display Hexidecimal data
        for(j=0; j<PX_LOG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // Insert extra space?
            if((j != 0) && ((j % 4) == 0))
            {
                // Yes
                px_log_putchar(' ');
            }
            // End of data?
            if((i + j) < nr_of_bytes)
            {
                // No
                px_log_print_hex08(row_data[j]);
                px_log_putchar(' ');
            }
            else
            {
                // No more data
#ifdef PX_COMPILER_GCC_AVR
                px_log_printf_P(PX_PGM_STR("   "));
#else
                px_log_print_str("   ");
#endif
            }
        }

        // Display ASCII data
        for(j = 0; j < PX_LOG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // End of data?
            if((i + j) < nr_of_bytes)
            {
                // No. Displayable?
                if( (row_data[j] >= 32) && (row_data[j] <= 127) )
                {
                    // Yes
                    px_log_putchar(row_data[j]);
                }
                else
                {
                    // No
                    px_log_putchar('.');
                }
            }
            else
            {
                // No more data
                px_log_putchar(' ');
            }
        }
        px_log_putchar('\n');
    }
}
