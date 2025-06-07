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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_rtc.h : RTC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-17

============================================================================= */
/** 
 *  @ingroup STM32
 *  @defgroup STM32_RTC px_rtc.h : RTC peripheral driver
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_rtc.h
 *  - arch/arm/stm32/src/px_rtc.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
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

/**
 *  Enable Alarm.
 *  
 *  @param[in] alarm        Pointer to a structure that contains alarm fields
 *                          (day, hour, min, sec).
 *  @param[in] alarm_mask   Bit mask indicating on which fields (day, hour, min,
 *                          sec) alarm must match.
 */
void px_rtc_alarm_enable(const px_rtc_date_time_t * alarm,
                         px_rtc_alarm_mask_t        alarm_mask);

/**
 *  Disable Alarm.
 */
void px_rtc_alarm_disable(void);

/**
 *  Check if Alarm has expired.
 *  
 *  @retval true    Alarm has expired
 *  @retval false   Alarm has not expired
 */
bool px_rtc_alarm_flag_is_set(void);

/**
 *  Clear Alarm flag.
 *  
 */
void px_rtc_alarm_flag_clear(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
