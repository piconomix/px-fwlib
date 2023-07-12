#ifndef __PX_SBUF_H__
#define __PX_SBUF_H__
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

/** 
 *  @ingroup UTILS
 *  @defgroup PX_SBUF px_sbuf.h : Safe buffer string appending functions that avoid overflow
 *  
 *  Safe buffer string appending functions that avoid overflow.
 *  
 *  File(s):
 *  - utils/inc/px_sbuf.h
 *  - utils/src/px_sbuf.c
 *
 *  This module provides a mechanism to provide a fixed size character buffer
 *  and append characters and strings safely and avoiding buffer overflows.
 *
 *  Example:
 *
 *  @include utils/test/px_sbuf_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef struct
{
    char * buf;         ///< Pointer to character buffer
    size_t buf_size;    ///< Size of character buffer
    size_t index;       ///< Index of next character to be copied into buffer
} px_sbuf_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param buf      Pointer to character buffer
 *  @param buf_size Size of character buffer
 */
void px_sbuf_init(px_sbuf_t * sbuf, char * buf, size_t buf_size);

/**
 *  Allocate buffer on heap and initialise safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param buf_size Size of character buffer
 *
 *  @retval true    Buffer could be allocated on heap
 *  @retval false   Buffer could not be allocated on heap
 */
bool px_sbuf_malloc(px_sbuf_t * sbuf, size_t buf_size);

/**
 *  Free buffer on heap and clear safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 */
void px_sbuf_free(px_sbuf_t * sbuf);

/**
 *  Reset safe buffer
 *
 *  @param sbuf     Pointer to buffer object
 */
void px_sbuf_reset(px_sbuf_t * sbuf);

/**
 *  Is safe buffer empty?
 *
 *  @param sbuf     Pointer to safe buffer object
 *
 *  @retval true    safe buffer is empty
 *  @retval false   safe buffer is not empty
 */
bool px_sbuf_is_empty(const px_sbuf_t * sbuf);

/**
 *  Is safe buffer full
 *
 *  @param sbuf     Pointer to safe buffer object
 *
 *  @retval true    safe buffer is full
 *  @retval false   safe buffer is not full
 */
bool px_sbuf_is_full(const px_sbuf_t * sbuf);

/**
 *  Return remaining size of safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *
 *  @return size_t  Remaining size
 */
size_t px_sbuf_get_size_remaining(const px_sbuf_t * sbuf);

/**
 *  Append a character to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param c        Character to append
 */
void px_sbuf_putchar(px_sbuf_t * sbuf, char c);

/**
 *  Append a string and newline character to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param str      String to append
 */
void px_sbuf_puts(px_sbuf_t * sbuf, const char * str);

/**
 *  Append a string to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param str      String to append
 */
void px_sbuf_print(px_sbuf_t * sbuf, const char * str);

/**
 *  Append a formatted string to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param format   Printf format string
 */
void px_sbuf_printf(px_sbuf_t * sbuf, const char * format, ...);

/**
 *  Append a variable argument string to a safe buffer
 *
 *  Example:
 *  @code
 *  #include <std_arg.h>
 *
 *  va_list args;
 *  va_start(args, format);
 *  px_sbuf_vprintf(&sbuf, format, &args);
 *  va_end(args);
 *  @endcode
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param format   Printf format string
 *  @param args     Pointer to variable argument list object
 */
void px_sbuf_vprintf(px_sbuf_t * sbuf, const char * format, const va_list * args);

/**
 *  Append data to a safe buffer
 *
 *  @param sbuf         Pointer to safe buffer object
 *  @param data         Pointer to data buffer
 *  @param nr_of_bytes  Number of bytes to append
 */
void px_sbuf_append(px_sbuf_t * sbuf, const char * data, size_t nr_of_bytes);

/**
 *  Append an array of HEX data to a safe buffer
 *
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 *
 */
void px_sbuf_print_data_hex(px_sbuf_t * sbuf, const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
