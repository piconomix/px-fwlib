#ifndef __PX_BUZZER_H__
#define __PX_BUZZER_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          Buzzer Driver
    Author(s):      pieter.conradie
    Creation Date:  2013-06-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_systmr.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_BUZZER_TUNE_END    0x00
#define PX_BUZZER_TUNE_REPEAT 0xFF

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Buzzer tune entry
typedef struct
{
    uint8_t           piano_key;       ///< Piano key (mapped to a frequency)
    px_systmr_ticks_t duration_ticks;  ///< Duration of note in system clock ticks
} buzzer_tune_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_buzzer_init     (void);
bool px_buzzer_is_active(void);

void px_buzzer_off      (void);
void px_buzzer_on       (const uint16_t freq_hz);

void px_buzzer_beep     (const uint16_t freq_hz, 
                         const px_systmr_ticks_t px_systmr_ticks);
void px_buzzer_play     (const buzzer_tune_t * tune);

bool px_buzzer_task     (void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
