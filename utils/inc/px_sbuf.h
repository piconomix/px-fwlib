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
    char * buf;
    size_t buf_size;
    size_t index;
} px_sbuf_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_sbuf_init    (px_sbuf_t * sbuf, char * buf, size_t buf_size);
void px_sbuf_reset   (px_sbuf_t * sbuf);

void px_sbuf_printf  (px_sbuf_t * sbuf, const char * format, ...);
void px_sbuf_putchar (px_sbuf_t * sbuf, char c);
void px_sbuf_strcpy(px_sbuf_t * sbuf, const char * str);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
