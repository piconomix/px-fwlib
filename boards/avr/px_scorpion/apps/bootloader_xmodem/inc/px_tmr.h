#ifndef __PX_TMR_H__
#define __PX_TMR_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
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
