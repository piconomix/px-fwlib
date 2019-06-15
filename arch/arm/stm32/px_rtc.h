#ifndef __PX_RTC_H__
#define __PX_RTC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_rtc.h : RTC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-17

============================================================================= */
/** 
 *  @ingroup STM32
 *  @defgroup STM32_RTC px_rtc.h : RTC peripheral driver
 *  
 *  File(s):
 *  - arch/arm/stm32/px_rtc.h
 *  - arch/arm/stm32/px_rtc.c
 *  
 */
/// @{

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_rtc_util.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise module.
 */
void px_rtc_init(void);

/**
 *  Set the time and date.
 *  
 *  @param[in] date_time    Pointer to a structure that contains the new date
 *                          and time.
 */
void px_rtc_date_time_wr(const px_rtc_date_time_t * date_time);

/**
 *  Get a copy of the time and date.
 *  
 *  @param[out] date_time   Pointer to a structure that will hold the new date
 *                          and time.
 */
void px_rtc_date_time_rd(px_rtc_date_time_t * date_time);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_RTC_H__
