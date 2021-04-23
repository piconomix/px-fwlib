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
 *  - arch/arm/stm32/inc/px_rtc.h
 *  - arch/arm/stm32/src/px_rtc.c
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
typedef enum
{
    PX_RTC_WAKEUP_PRESC_CLK_DIV_16   = 0,   ///< RTC clk / 16
    PX_RTC_WAKEUP_PRESC_CLK_DIV_8    = 1,   ///< RTC clk / 8
    PX_RTC_WAKEUP_PRESC_CLK_DIV_4    = 2,   ///< RTC clk / 4
    PX_RTC_WAKEUP_PRESC_CLK_DIV_2    = 3,   ///< RTC clk / 2
    PX_RTC_WAKEUP_PRESC_CLK_DIV_1    = 4,   ///< RTC clk
    PX_RTC_WAKEUP_PRESC_CLK_DIV_1_HI = 5,   ///< RTC clk + 2^16
} px_rtc_wakeup_presc_clk_t;

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
 *  Enable Alarm A.
 *  
 *  @param[in] alarm        Pointer to a structure that contains alarm fields
 *                          (day, hour, min, sec).
 *  @param[in] alarm_mask   Bit mask indicating on which fields (day, hour, min,
 *                          sec) alarm must match.
 */
void px_rtc_alarm_a_enable(const px_rtc_date_time_t * alarm, 
                           px_rtc_alarm_mask_t        alarm_mask);

/**
 *  Disable Alarm A.
 */
void px_rtc_alarm_a_disable(void);

/**
 *  Check if Alarm A has expired.
 *  
 *  @retval true    Alarm has expired
 *  @retval false   Alarm has not expired
 */
bool px_rtc_alarm_a_flag_is_set(void);

/**
 *  Clear Alarm A flag.
 *  
 */
void px_rtc_alarm_a_flag_clear(void);

/**
 *  Enable Alarm B.
 *  
 *  @param[in] alarm        Pointer to a structure that contains alarm fields
 *                          (day, hour, min, sec).
 *  @param[in] alarm_mask   Bit mask indicating on which fields (day, hour, min,
 *                          sec) alarm must match.
 */
void px_rtc_alarm_b_enable(const px_rtc_date_time_t * alarm, 
                           px_rtc_alarm_mask_t        alarm_mask);

/**
 *  Disable Alarm B.
 */
void px_rtc_alarm_b_disable(void);

/**
 *  Check if Alarm B has expired.
 *  
 *  @retval true    Alarm has expired
 *  @retval false   Alarm has not expired
 */
bool px_rtc_alarm_b_flag_is_set(void);

/**
 *  Clear Alarm B flag.
 */
void px_rtc_alarm_b_flag_clear(void);

/**
 *  Enable Wakeup Timer
 *  
 *  @param wakeup_presc_clk Prescaler for wakeup timer clock source
 *  @param wakeup_reload    Reload value for wakeup timer
 */
void px_rtc_wakeup_tmr_enable(px_rtc_wakeup_presc_clk_t wakeup_presc_clk,
                              uint16_t                  wakeup_reload);

/**
 *  Disable Wakeup Timer.
 */
void px_rtc_wakeup_tmr_disable(void);

/**
 *  Check if Wakeup Timer has expired.
 *  
 *  @retval true    Timer has expired
 *  @retval false   Timer has not expired
 */
bool px_rtc_wakeup_tmr_flag_is_set(void);

/**
 *  Clear Wakeup Timer flag.
 */
void px_rtc_wakeup_tmr_flag_clear(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
