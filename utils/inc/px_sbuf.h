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
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
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
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

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
 *  Reset safe buffer
 *
 *  @param sbuf Pointer to buffer object
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
 *  @retval false   safe buffer is empty
 */
bool px_sbuf_is_full(const px_sbuf_t * sbuf);

/**
 *  Append printf string to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param format   Printf format string
 */
void px_sbuf_printf(px_sbuf_t * sbuf, const char * format, ...);

/**
 *  Append character to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param c        Character to append
 */
void px_sbuf_putchar(px_sbuf_t * sbuf, char c);

/**
 *  Append string to safe buffer
 *
 *  @param sbuf     Pointer to safe buffer object
 *  @param str      String to append
 */
void px_sbuf_strcpy(px_sbuf_t * sbuf, const char * str);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
