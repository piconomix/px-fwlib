/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_dbg.h : Debug module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_dbg.h"
#if PX_DBG_CFG_COLOR
#include "px_vt100.h"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Number of bytes per row for hex dump
#define PX_DBG_TRACE_DATA_BYTES_PER_ROW    16

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate internal buffer for debug output
static char px_dbg_buf[PX_DBG_CFG_BUF_SIZE];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void inline px_dbg_put_char(char data)
{
#ifdef PX_DBG_CFG_PUT_CHAR
    // Output character using configured function
    PX_DBG_CFG_PUT_CHAR(data);
#else
    // Output character to stdout
    putchar(data);
#endif
}

static void px_dbg_put_str(const char  * data)
{
    while(*data != '\0')
    {
        px_dbg_put_char(*data++);
    }
}

static void inline px_dbg_put_hex04(uint8_t data)
{
    if(data < 10)
    {
        data += (uint8_t)'0';
    }
    else
    {
        data += (uint8_t)'A' - 10;
    }
    px_dbg_put_char((char)data);
}

static void inline px_dbg_put_hex08(uint8_t data)
{
    px_dbg_put_hex04(PX_U8_HI4(data));
    px_dbg_put_hex04(PX_U8_LO4(data));    
}

#ifdef PX_COMPILER_GCC_AVR
static void px_dbg_put_str_P(const char * data)
{
    char c;

    while(true)
    {
        c = px_pgm_rd_char(data++);
        if(c == '\0')
        {
            return;
        }
        px_dbg_put_char(c);
    }
}

static void px_dbg_trace_vargs_P(const char * format, va_list args) 
{
    size_t len;

    // Populate buffer with debug string
    vsnprintf_P(px_dbg_buf, PX_DBG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Remove tab character ('\t') at the end of user formatted string
    len = strlen(px_dbg_buf);
    if((len != 0) && (px_dbg_buf[len - 1] == '\t'))
    {
        px_dbg_buf[len - 1] = '\0';
    }
    // Output user formatted string
    px_dbg_put_str(px_dbg_buf);
}

static void px_dbg_printf_P(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_dbg_trace_vargs_P(format, args);
    va_end(args);
}

#else

static void px_dbg_trace_vargs(const char * format, va_list args) 
{
    size_t len;

    // Populate buffer with debug string
    vsnprintf(px_dbg_buf, PX_DBG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Remove tab character ('\t') at the end of user formatted string
    len = strlen(px_dbg_buf);
    if((len != 0) && (px_dbg_buf[len - 1] == '\t'))
    {
        px_dbg_buf[len - 1] = '\0';
    }
    // Output user formatted string
    px_dbg_put_str(px_dbg_buf);
}

static void px_dbg_printf(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_dbg_trace_vargs(format, args);
    va_end(args);
}
#endif

static void px_dbg_report_log_prefix(uint8_t      level, 
                                     const char * name, 
                                     uint16_t     line)
{
    // Output level
    if(level == PX_DBG_CFG_MSG_LEVEL_ERR)
    {
#if PX_DBG_CFG_COLOR
        // Send VT100 sequence to set font color to RED
        px_dbg_put_str(PX_VT100_SET_FG_RED "E");
#else
        // Error
        px_dbg_put_char('E');
#endif
    }
    else if(level == PX_DBG_CFG_MSG_LEVEL_WARN)
    {
#if PX_DBG_CFG_COLOR
        // Send VT100 sequence to set font color to YELLOW
        px_dbg_put_str(PX_VT100_SET_FG_YELLOW "W");
#else
        // Warning
        px_dbg_put_char('W');
#endif
    }
    else
    {
#if PX_DBG_CFG_COLOR
        // Send VT100 sequence to set font color to GREEN
        px_dbg_put_str(PX_VT100_SET_FG_GREEN "I");
#else
        // Info
        px_dbg_put_char('I');
#endif
    }
    px_dbg_put_char(' ');

    // Timestamp function provided in 'px_dbg_cfg.h'?
#ifdef PX_DBG_CFG_TIMESTAMP
    // Get user supplied timestamp string
    PX_DBG_CFG_TIMESTAMP(px_dbg_buf);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Output timestamp
    px_dbg_put_str(px_dbg_buf);
    px_dbg_put_char(' ');
#endif

#if PX_DBG_CFG_COLOR
        // Send VT100 sequence to reset all attributes
        px_dbg_put_str(PX_VT100_RST_ALL_ATTRS);
#endif

    // Output file and line
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_put_str_P(name);
    px_dbg_printf_P(PX_PGM_STR(" %u : "), line);
#else
    px_dbg_printf("%s %u : ", name, line);
#endif
}

static void _px_dbg_log_terminate(const char * format)
{
#ifdef PX_COMPILER_GCC_AVR
    size_t len = strlen_P(format);
    if((len == 0) || px_pgm_rd_char(format + len - 1) != '\t')
    {
        // Append end-of-line
        px_dbg_put_char('\n');
    }
#else
    size_t len = strlen(format);
    if((len == 0) || format[len - 1] != '\t')
    {
        // Append end-of-line
        px_dbg_put_char('\n');
    }
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_dbg_log_info(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_INFO, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_dbg_log_terminate(format);
}

void _px_dbg_log_warn(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_WARN, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_dbg_log_terminate(format);
}

void _px_dbg_log_err(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_ERR, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append End Of Line ('\n') if format string does not end with a TAB character ('\t')
    _px_dbg_log_terminate(format);
}

void _px_dbg_assert(const char * name, 
                    uint16_t     line, 
                    const char * expression)
{
    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_ERR, name, line);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_printf_P(PX_PGM_STR("ASSERT: "));
    px_dbg_printf_P(expression);
#else
    px_dbg_printf("ASSERT: ");
    px_dbg_printf(expression);
#endif

     
#if 0
    // Generate debug breakpoint
    __asm__ __volatile__("bkpt #0\n\t"::);
#endif

    // Block forever
    for(;;)
    {
        ;
    }
}

void _px_dbg_trace(const char * format, ...)
{
    va_list args;

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
}

void _px_dbg_trace_data(const void * data, size_t nr_of_bytes)
{
    size_t          i;
    const uint8_t * data_u8 = (const uint8_t *)data;

    for(i=0; i<nr_of_bytes; i++)
    {
        px_dbg_put_hex08(*data_u8++);
        px_dbg_put_char(' ');
    }
}

void _px_dbg_trace_hexdump(const void * data, size_t nr_of_bytes)
{
    size_t          i, j;
    const uint8_t * row_data;

    // Split data up into rows
    for(i=0; i<nr_of_bytes; i+= PX_DBG_TRACE_DATA_BYTES_PER_ROW)
    {
        // Insert extra empty row?
        if(  (i != 0) && ((i%(PX_DBG_TRACE_DATA_BYTES_PER_ROW*4)) == 0)  )
        {
            // Yes
            px_dbg_put_char('\n');
        }

        // Select row data
        row_data = &((const uint8_t *)data)[i];

        // Display Hexidecimal data
        for(j=0; j<PX_DBG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // Insert extra space?
            if(  (j != 0) && ((j%4) == 0)  )
            {
                // Yes
                px_dbg_put_char(' ');
            }
            // End of data?
            if((i+j) < nr_of_bytes)
            {
                // No
                px_dbg_put_hex08(row_data[j]);
                px_dbg_put_char(' ');
            }
            else
            {
                // No more data
#ifdef PX_COMPILER_GCC_AVR
                px_dbg_printf_P(PX_PGM_STR("   "));
#else
                px_dbg_put_str("   ");
#endif
            }            
        }

        // Display ASCII data
        for(j=0; j<PX_DBG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // End of data?
            if((i+j) < nr_of_bytes)
            {
                // No. Displayable?
                if( (row_data[j] >= 32) && (row_data[j] <= 127) )
                {
                    // Yes
                    px_dbg_put_char(row_data[j]);
                }
                else
                {
                    // No
                    px_dbg_put_char('.');
                }
            }
            else
            {
                // No more data
                px_dbg_put_char(' ');
            }
        }
        px_dbg_put_char('\n');
    }
}
