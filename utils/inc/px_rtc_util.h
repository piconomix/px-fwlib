#ifndef __PX_RTC_UTIL_H__
#define __PX_RTC_UTIL_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_rtc_util.h : Software RTC and utility functions
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-22

============================================================================= */
/**
 *  @ingroup UTILS
 *  @defgroup PX_RTC_UTIL px_rtc_util.h : Software RTC and utility functions
 *
 *  This module provides basic time, calender and alarm functionality.
 *
 *  File(s):
 *  - utils/inc/px_rtc_util.h
 *  - utils/inc/px_rtc_util_cfg_template.h
 *  - utils/src/px_rtc_util.c
 *
 */
/// @{

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_rtc_util_cfg_template.h"
#include "px_rtc_util_cfg.h"

// Check that all project specific options have been specified in "px_rtc_util_cfg.h"
#if (   !defined(PX_RTC_UTIL_CFG_SEC_SINCE_Y2K ) \
     || !defined(PX_RTC_UTIL_CFG_PERIODIC_FLAGS) \
     || !defined(PX_RTC_UTIL_CFG_TICKS_PER_SEC ) \
     || !defined(PX_RTC_UTIL_CFG_DAY_OF_WEEK   )   )
#error "One or more options not defined in 'px_rtc_util_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
// Utility definitions
#define PX_RTC_SEC_PER_MIN  (60ul)
#define PX_RTC_SEC_PER_HOUR (60*PX_RTC_SEC_PER_MIN)
#define PX_RTC_SEC_PER_DAY  (24*PX_RTC_SEC_PER_HOUR)

/// @name Alarm bit mask
//@{
#define PX_RTC_UTIL_ALARM_MASK_DIS      0       ///< Disable alarm
#define PX_RTC_UTIL_ALARM_MASK_SEC      (1<<0)  ///< Alarm match on second
#define PX_RTC_UTIL_ALARM_MASK_MIN      (1<<1)  ///< Alarm match on minute
#define PX_RTC_UTIL_ALARM_MASK_HOUR     (1<<2)  ///< Alarm match on hour
#define PX_RTC_UTIL_ALARM_MASK_DAY      (1<<3)  ///< Alarm match on day
#define PX_RTC_UTIL_ALARM_MASK_MONTH    (1<<4)  ///< Alarm match on month
#define PX_RTC_UTIL_ALARM_MASK_YEAR     (1<<5)  ///< Alarm match on year
//@}

/// Day of week
typedef enum
{
    PX_RTC_UTIL_DAY_MON = 0,                    ///< Monday
    PX_RTC_UTIL_DAY_TUE = 1,                    ///< Tuesday
    PX_RTC_UTIL_DAY_WED = 2,                    ///< Wednesday
    PX_RTC_UTIL_DAY_THU = 3,                    ///< Thursday
    PX_RTC_UTIL_DAY_FRI = 4,                    ///< Friday
    PX_RTC_UTIL_DAY_SAT = 5,                    ///< Saturday
    PX_RTC_UTIL_DAY_SUN = 6,                    ///< Sunday
} px_rtc_util_day_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition to track seconds since Y2K (2000-01-01 00:00:00 Saturday)
typedef uint32_t px_rtc_sec_since_y2k_t;

#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    #if (PX_RTC_UTIL_CFG_TICKS_PER_SEC <= PX_U8_MAX)
        /// Size definition ticks per second as 8-bit value
        typedef uint8_t px_rtc_ticks_per_sec_t;
    #else
        /// Size definition ticks per second as 16-bit value
        typedef uint16_t px_rtc_ticks_per_sec_t;
    #endif
#endif

/// Structure to store date and time
typedef struct
{
    uint8_t                 year;           ///< Years:   0 to 99 (2000 - 2099)
    uint8_t                 month;          ///< Months:  1 to 12
    uint8_t                 day;            ///< Days:    1 to 31 (depending on month)
    uint8_t                 hour;           ///< Hours:   0 to 23
    uint8_t                 min;            ///< Minutes: 0 to 59
    uint8_t                 sec;            ///< Seconds: 0 to 59
#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    uint8_t                 day_of_week;    ///< Day of week: Mon = 0; Sun = 6
#endif
#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    px_rtc_ticks_per_sec_t  ticks;          ///< Sub second ticks
#endif
} px_rtc_date_time_t;

/// Result of RTC time comparision
typedef enum
{
    PX_RTC_UTIL_TIME_OLDER,             ///< Time is older
    PX_RTC_UTIL_TIME_EQUAL,             ///< Time is equal
    PX_RTC_UTIL_TIME_NEWER              ///< Time is newer
} px_rtc_time_compare_t;

/// Alarm mask
typedef uint8_t px_rtc_alarm_mask_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise module.
 */
void px_rtc_util_init(void);

/**
 *  Call this function once a second to update date, time and alarm flag.
 */
void px_rtc_util_on_tick(void);

/**
 *  Set the time and date.
 *
 *  @param[in] date_time    Pointer to a structure that contains the new date
 *                          and time.
 */
void px_rtc_util_date_time_wr(px_rtc_date_time_t * date_time);

/**
 *  Get a copy of the time and date.
 *
 *  @param[out] date_time   Pointer to a structure that will hold the new date
 *                          and time.
 */
void px_rtc_util_date_time_rd(px_rtc_date_time_t * date_time);

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
/**
 *  Return the number of seconds elapsed since Y2K (2000-01-01 00:00:00)
 *
 *  @return px_rtc_util_sec_since_y2k_t     number of seconds elapsed since Y2K
 */
px_rtc_sec_since_y2k_t px_rtc_util_sec_since_y2k_rd(void);
#endif

/**
 *  Set the alarm time and date.
 *
 *  The alarm flag will be cleared.
 *
 *  @param alarm         Pointer to a structure that contains the new alarm
 *                       time.
 *  @param alarm_mask    Alarm mask (disabled, sec, min, hour, day, month, year)
 */
void px_rtc_util_alarm_wr(const px_rtc_date_time_t * alarm,
                          px_rtc_alarm_mask_t        alarm_mask);

/**
 *  Get a copy of the alarm time and date.
 *
 *  @param alarm         Pointer to a structure that will hold the current alarm
 *                       time.
 *  @param alarm_mask    Pointer to hold the current alarm mask to match on
 *                       (disabled, sec, min, hour, day, month, year)
 */
void px_rtc_util_alarm_rd(px_rtc_date_time_t *  alarm,
                          px_rtc_alarm_mask_t * alarm_mask);

/**
 *  See if alarm flag is set.
 *
 *  @retval true         Alarm flag set (alarm has occurred)
 *  @retval false        Alarm flag not set (alarm has not occurred)
 */
bool px_rtc_util_alarm_flag_is_set(void);

/**
 *  Clear alarm flag.
 */
void px_rtc_util_alarm_flag_clear(void);

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS

/**
 *  See if minute period flag is set.
 *
 *  @retval true         Minute period flag set
 *  @retval false        Minute period flag set
 */
bool px_rtc_util_period_min_flag_is_set(void);

/**
 *  Clear minute period flag.
 */
void px_rtc_util_period_min_flag_clear(void);

/**
 *  See if hour period flag is set.
 *
 *  @retval true         Hour period flag set
 *  @retval false        Hour period flag set
 */
bool px_rtc_util_period_hour_flag_is_set(void);

/**
 *  Clear hour period flag.
 */
void px_rtc_util_period_hour_flag_clear(void);

/**
 *  See if day period flag is set.
 *
 *  @retval true        Day period flag set
 *  @retval false       Day period flag set
 */
bool px_rtc_util_period_day_flag_is_set(void);

/**
 *  Clear day period flag.
 */
void px_rtc_util_period_day_flag_clear(void);

#endif

/**
 *  Reset date-time structure to 2000-01-01 00:00
 *
 *  @param date_time        Pointer to a structure that will hold the
 *                          new date and time.
 */
void px_rtc_util_date_time_reset(px_rtc_date_time_t * date_time);

/**
 *  Test if all of the fields in the date-time structure are correct.
 *
 *  @param date_time        Pointer to a structure containing the date and time.
 *
 *  @retval true            All of the fields are valid.
 *  @retval false           One or more of the fields are invalid.
 */
bool px_rtc_util_date_time_fields_valid(const px_rtc_date_time_t * date_time);

/**
 *  Compare two date-time structures to see if the first is older, equal or
 *  newer than the second.
 *
 *  @param date_time1               First date-time structure
 *  @param date_time2               Second date-time structure
 *
 *  @retval PX_RTC_UTIL_TIME_OLDER  First date-time is older than second.
 *  @retval PX_RTC_UTIL_TIME_EQUAL  First date-time is equal to second.
 *  @retval PX_RTC_UTIL_TIME_NEWER  First date-time is newer than second.
 */
px_rtc_time_compare_t px_rtc_util_cmp_date_time(const px_rtc_date_time_t * date_time1,
                                                const px_rtc_date_time_t * date_time2);

/**
 *  Compare two dates to see if the first is older, equal or newer than the
 *  second.
 *
 *  @param date_time1               First date-time structure
 *  @param date_time2               Second date-time structure
 *
 *  @retval PX_RTC_UTIL_TIME_OLDER  First date-time is older than second.
 *  @retval PX_RTC_UTIL_TIME_EQUAL  First date-time is equal to second.
 *  @retval PX_RTC_UTIL_TIME_NEWER  First date-time is newer than second.
 */
px_rtc_time_compare_t px_rtc_util_cmp_date(const px_rtc_date_time_t * date_time1,
                                           const px_rtc_date_time_t * date_time2);

/**
 *  Compare the dates to see if they fall on the same day.
 *
 *  @param date_time1       First date-time structure
 *  @param date_time2       Second data-time structure
 *
 *  @retval true            Dates fall on the same day
 *  @retval false           Dates do not fall on the same day
 */
bool px_rtc_util_date_is_equal(const px_rtc_date_time_t * date_time1,
                               const px_rtc_date_time_t * date_time2);

/**
 *  Convert a date and time to seconds elapsed since 2000-01-01 00:00 (Y2K)
 *
 *  @param date_time            date-time structure
 *
 *  @return px_rtc_util_sec_since_y2k_t Seconds since Y2K (2000-01-01 00:00:00)
 */
px_rtc_sec_since_y2k_t px_rtc_util_date_time_to_sec_since_y2k(const px_rtc_date_time_t * date_time);

/**
 *  Convert seconds elapsed since 2000-01-01 00:00 (Y2K) to a date and time
 *
 *  @param sec_since_y2k        Seconds since Y2K (2000-01-01 00:00:00)
 *  @param date_time            date-time structure to receive calculated date and time
 *
*/
void px_rtc_util_sec_since_y2k_to_date_time(px_rtc_sec_since_y2k_t sec_since_y2k,
                                            px_rtc_date_time_t *   date_time);

/**
 *  Convert seconds since Y2K to Unix epoch
 *
 *  @param sec_since_y2k Seconds since Y2K (2000-01-01 00:00:00)
 *
 *  @return uint32_t     Seconds Unix epoch (1970-01-01 00:00:00)
 */
static inline uint32_t px_rtc_util_sec_since_y2k_to_unix_epoch(px_rtc_sec_since_y2k_t sec_since_y2k)
{
    return sec_since_y2k + 946684800;
}

/**
 *  Increment a date-time structure with a specified number of years, months,
 *  days, hours, minutes and seconds.
 *
 *  @param date_time        date-time structure to increment
 *  @param date_time_inc    date-time structure containing amount to increment by
 */
void px_rtc_util_date_time_inc(px_rtc_date_time_t *       date_time,
                               const px_rtc_date_time_t * date_time_inc);

/**
 *  Decrement a date-time structure with a specified number of years, months,
 *  days, hours, minutes and seconds.
 *
 *  @param date_time        date-time structure to decrement
 *  @param date_time_dec    date-time structure containing amount to decrement by
 */
void px_rtc_util_date_time_dec(px_rtc_date_time_t *       date_time,
                               const px_rtc_date_time_t * date_time_dec);

/**
 *  Calculate day of week
 *
 *  @param date_time            date
 *
 *  @return px_rtc_util_day_t   day of week
 */
px_rtc_util_day_t px_rtc_util_date_to_day_of_week(const px_rtc_date_time_t * date_time);

/**
 *  Report date and time using printf.
 *
 *  @param date_time        date-time structure to report
 */
void px_rtc_util_report_date_time(const px_rtc_date_time_t * date_time);

/**
 *  Report date using printf.
 *
 *  @param date_time        date-time structure to report
 */
void px_rtc_util_report_date(const px_rtc_date_time_t * date_time);

/**
 *  Report time using printf.
 *
 *  @param date_time        date-time structure to report
 */
void px_rtc_util_report_time(const px_rtc_date_time_t * date_time);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
