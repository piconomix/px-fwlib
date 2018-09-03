#ifndef __PX_BUZZER_H__
#define __PX_BUZZER_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (C) 2013 Piconomix Design <https://piconomix.com>
    All rights reserved.
 
    The source code contained in this file is confidential, proprietary 
    and a trade secret. Without prior written approval from Piconomix Design
    no part of this source code may be reproduced or transmitted in any 
    form or by any means, included but not limited to electronic, 
    mechanical, photocopying or recording or stored in any retrieval 
    system of whatever nature. Use of any copyright notice does not 
    imply unrestricted public access to any part of this source code.
 
    All content is owned by Piconomix Design. Other trademarks and copyright 
    notices are acknowledged as the property of their rightful owners.
 
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
    
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
/* _____DEFINITIONS _________________________________________________________ */
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

#endif // #ifndef __PX_BUZZER_H__
