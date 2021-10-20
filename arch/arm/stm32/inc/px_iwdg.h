#ifndef __PX_IWDG_H__
#define __PX_IWDG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_iwdg.h : Independent Watchdog module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-08

============================================================================= */

/**
 *  @ingroup STM32
 *  @defgroup STM32_IWDG px_iwdg.h : Independent Watchdog module
 *
 *  This module is used to initialise the IWDG peripheral.
 *
 *  File(s):
 *  - arch/arm/stm32/inc/px_iwdg.h
 *  - arch/arm/stm32/src/px_iwdg.c
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
/// Independent Watchdog clock prescaler (40 kHz LSI clock source)
typedef enum
{
    PX_IWDG_PRESCALER_4     = 0,    ///< Clock = 40 kHz / 4
    PX_IWDG_PRESCALER_8     = 1,    ///< Clock = 40 kHz / 8
    PX_IWDG_PRESCALER_16    = 2,    ///< Clock = 40 kHz / 16
    PX_IWDG_PRESCALER_32    = 3,    ///< Clock = 40 kHz / 32
    PX_IWDG_PRESCALER_64    = 4,    ///< Clock = 40 kHz / 64
    PX_IWDG_PRESCALER_128   = 5,    ///< Clock = 40 kHz / 128
    PX_IWDG_PRESCALER_256   = 6,    ///< Clock = 40 kHz / 256
} px_iwdg_prescaler_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise Independent Watchdog
 *
 *  @param prescaler    Clock prescaler. See #px_iwdg_prescaler_t
 *  @param reload       Counter reload value (1 to 0x0fff)
 *  @param window       Reload window (0 if not used; max 0x0fff)
 */
void px_iwdg_init(px_iwdg_prescaler_t prescaler, uint16_t reload, uint16_t window);

/**
 *  Reload watchdog counter, a.k.a. "patting the dog"
 */
void px_iwdg_reload_counter(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
