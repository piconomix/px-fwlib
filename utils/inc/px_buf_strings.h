#ifndef __PX_BUF_STRINGS_H__
#define __PX_BUF_STRINGS_H__
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

/** 
 *  @ingroup UTILS
 *  @defgroup PX_BUF_STRINGS px_buf_strings.h : Buffer containing multiple strings
 *
 *  File(s):
 *  - utils/inc/px_buf_strings.h
 *  - utils/src/px_buf_strings.c
 *
 *  This module provides a mechanism to efficiently contain multiple zero
 *  terminated strings. The end is marked with an empty string. Character buffer
 *  overflow is avoided.
 *
 *  Example:
 *
 *  @include utils/test/px_buf_strings_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef struct
{
    char *       buf;       ///< Pointer to character buffer
    uint16_t     buf_size;  ///< Size of character buffer
    uint16_t     index;     ///< Index of next character to be copied into buffer
    const char * str;       ///< Pointer to current string
} px_buf_strings_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise object
 *
 *  @param buf_strings     Pointer to object
 *  @param buf             Pointer to character buffer
 *  @param buf_size        Size of character buffer
 */
void px_buf_strings_init(px_buf_strings_t * buf_strings, char * buf, uint16_t buf_size);

/**
 *  Allocate character buffer on heap and initialise object
 *
 *  @param buf_strings     Pointer to object
 *  @param buf_size        Size of character buffer
 *
 *  @retval true           Character buffer could be allocated on heap
 *  @retval false          Character buffer could not be allocated on heap
 */
bool px_buf_strings_malloc(px_buf_strings_t * buf_strings, uint16_t buf_size);

/**
 *  Free character buffer on heap and clear object
 *
 *  @param buf_strings     Pointer to object
 */
void px_buf_strings_free(px_buf_strings_t * buf_strings);

/**
 *  Reset object
 *
 *  @param buf_strings     Pointer to object
 */
void px_buf_strings_reset(px_buf_strings_t * buf_strings);

/**
 *  Is character buffer empty?
 *
 *  @param buf_strings     Pointer to object
 *
 *  @retval true           Buffer is empty
 *  @retval false          Buffer is not empty
 */
inline bool px_buf_strings_is_empty(const px_buf_strings_t * buf_strings)
{
    if(buf_strings->index == 0) return true;
    else                        return false;
}

/**
 *  Is character buffer full
 *
 *  @param buf_strings     Pointer to object
 *
 *  @retval true           Buffer is full
 *  @retval false          Buffer is not full
 */
inline bool px_buf_strings_is_full(const px_buf_strings_t * buf_strings)
{
    if(buf_strings->index >= (buf_strings->buf_size - 1)) return true;
    else                                                  return false;
}

/**
 *  Return remaining size available in character buffer (excluding zero terminator)
 *
 *  @param buf_strings     Pointer to object
 *
 *  @return uint16_t       Remaining size
 */
uint16_t px_buf_strings_get_size_remaining(const px_buf_strings_t * buf_strings);

/**
 *  Append char to current string
 *
 *  If a zero terminator ('0') is appended then this will signal the start of a
 *  new string.
 *
 *  @param buf_strings     Pointer to object
 *  @param c               Character to append
 *
 *  retval true            Success
 *  retval false           Failure: buffer was full
 */
bool px_buf_strings_append_char(px_buf_strings_t * buf_strings, char c);

/**
 *  Append a new string
 *
 *  Characters that exceed the character buffer will be discarded.
 *
 *  @param buf_strings     Pointer to object
 *  @param str             String to append
 */
void px_buf_strings_append_str(px_buf_strings_t * buf_strings, const char * str);

/**
 *  Get current string
 *
 *  @param buf_strings      Pointer to object
 *
 *  @return const char *    Pointer to start of current string
 */
inline const char * px_buf_strings_get_current_str(const px_buf_strings_t * buf_strings)
{
    return buf_strings->str;
}

/**
 *  Is current string empty?
 *
 *  @param buf_strings     Pointer to object
 *
 *  @retval true           Current string is empty
 *  @retval false          Current string is not empty
 */
inline bool px_buf_strings_is_current_str_empty(const px_buf_strings_t * buf_strings)
{
    if(*buf_strings->str == '\0') return true;
    else                          return false;
}

/**
 *  Get number of strings stored in buffer
 *
 *  @param buf_strings     Pointer to object
 *
 *  @return uint16_t       Number of strings
 */
uint16_t px_buf_strings_get_count(px_buf_strings_t * buf_strings);

/**
 *  Get first string in buffer
 *
 *  @param buf_strings     Pointer to object
 *
 *  @return const char *   Pointer to first string. NULL if buffer is empty
 */
const char * px_buf_strings_get_first(px_buf_strings_t * buf_strings);

/**
 *  Get next string in buffer
 *
 *  @param buf_strings Pointer to object
 *
 *  @return const char *    Pointer to next string. NULL if there are no more strings.
 */
const char * px_buf_strings_get_next(px_buf_strings_t * buf_strings);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
