#ifndef __PX_WWDG_H__
#define __PX_WWDG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_wwdg.h : System Window Watchdog module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-08

============================================================================= */

/**
 *  @ingroup STM32
 *  @defgroup STM32_WWDG px_wwdg.h : System Window Watchdog module
 *
 *  This module is used to initialise the WWDG peripheral.
 *
 *  File(s):
 *  - arch/arm/stm32/inc/px_wwdg.h
 *  - arch/arm/stm32/src/px_wwdg.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Window Watchdog clock prescaler (PCLK / 4096 clock source)
typedef enum
{
    PX_WWDG_PRESCALER_1     = 0,    ///< Clock = (PCLK / 4096) / 1
    PX_WWDG_PRESCALER_2     = 1,    ///< Clock = (PCLK / 4096) / 2
    PX_WWDG_PRESCALER_4     = 2,    ///< Clock = (PCLK / 4096) / 4
    PX_WWDG_PRESCALER_8     = 3,    ///< Clock = (PCLK / 4096) / 8
} px_wwdg_prescaler_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise System Window Watchdog
 *
 *  @param prescaler    Clock prescaler. See #px_wwdg_prescaler_t
 *  @param counter      Reload counter value (max 0x7f)
 *  @param window       Reload window (0 if not used; max 0x7f)
 */
void px_wwdg_init(px_wwdg_prescaler_t prescaler, uint8_t counter, uint8_t window);

/**
 *  Reload watchdog counter, a.k.a. "patting the dog"
 */
void px_wwdg_reload_counter(void);

/**
 *  Halt watchdog by disabling peripheral clock.
 */
void px_wwdg_halt(void);

/**
 *  Resume watchdog by enabling peripheral clock.
 */
void px_wwdg_resume(void);

/**
 *  Report state saved by early warning interrupt in .noinit variables.
 */
void px_wwdg_dbg_report(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
