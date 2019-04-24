#ifndef __PX_TMR_H__
#define __PX_TMR_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
 
    Title:          Polled timeout functionality using a TMR peripheral
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise TMR peripheral.
 */
void px_tmr_init(void);

/**
 *  Set counter value so that overflow flag is set after the specified delay.
 *  
 *  @param ticks    Number of TMR ticks for required timeout
 */
void px_tmr_start(uint16_t ticks);

/**
 *  See if the timer counter has rolled over to zero by testing overflow flag.
 *  
 *  @retval true    timer has expired
 *  @retval false   timer has not expired
 */
bool px_tmr_has_expired(void);

/**
 *  Block for a specified delay.
 *  
 *  @param ticks    Number of TMR ticks for required timeout
 *  
 */
void px_tmr_wait(uint16_t ticks);

/* _____MACROS_______________________________________________________________ */
/** 
 *  Macro used to convert a timeout in milliseconds to a TMR ticks.
 *  
 *  @param[in] time_ms  Delay in milliseconds
 *  @return    uint16_t    TMR ticks
 */
#define TMR_MS_TO_TICKS(ms) (uint16_t)((uint32_t)ms * (F_CPU / 1024ul) / 1000ul)
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_TMR_H__
