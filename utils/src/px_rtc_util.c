/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_rtc.h : Real Time Clock module
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-22

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_rtc_util.h"
#include "px_pgm_P.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("rtc_util");

/* _____LOCAL VARIABLES______________________________________________________ */
/// Structure to keep track of time & date
static px_rtc_date_time_t px_rtc_util_date_time;

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
/// Variable to track seconds since Y2K (2000-01-01 00:00:00)
static volatile px_rtc_sec_since_y2k_t px_rtc_util_sec_since_y2k;
#endif

/// Structure to keep track of alarm
static px_rtc_date_time_t px_rtc_util_alarm;

/// Alarm mask
static px_rtc_alarm_mask_t px_rtc_util_alarm_mask;

/// Flag to indicate that an alarm has occurred
static volatile bool px_rtc_util_alarm_flag;

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS

/// Flag to indicate that minutes have been incremented (every 60 seconds)
static volatile bool px_rtc_util_period_min_flag;

/// Flag to indicate that hours have been incremented (every 60 minutes)
static volatile bool px_rtc_util_period_hour_flag;

/// Flag to indicate that days have been incremented (every 24 hours)
static volatile bool px_rtc_util_period_day_flag;

#endif

/// Table for the number of days in each month (non leap year)
static const uint8_t px_rtc_util_month_day_table[] PX_ATTR_PGM =
{
    0,   // Invalid month
    31,  // January
    28,  // February
    31,  // March
    30,  // April
    31,  // May
    30,  // June
    31,  // July
    31,  // August
    30,  // September
    31,  // October
    30,  // November
    31   // December
};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static bool     px_rtc_util_is_leap_year         (uint8_t year) PX_ATTR_CONST;
static uint16_t px_rtc_util_days_in_year         (uint8_t year) PX_ATTR_CONST;
static int8_t   px_rtc_util_days_in_month        (uint8_t year, uint8_t month) PX_ATTR_CONST;
static void     px_rtc_util_inc_date_time_on_tick(void);
static bool     px_rtc_util_date_time_match      (const px_rtc_date_time_t * date_time1,
                                                  const px_rtc_date_time_t * date_time2);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_rtc_util_is_leap_year(uint8_t year)
{
    // Sanity checks
    PX_LOG_ASSERT(year <= 99);

    //  Apply simplified (2000 to 2099) Gregorian calender rules:
    //  Every 4 years is a leap year, e.g. [2000, 2004, 2008, ..., 2096]
    if ((year % 4) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static uint16_t px_rtc_util_days_in_year(uint8_t year)
{
    // Sanity checks
    PX_LOG_ASSERT(year <= 99);

    //  Apply simplified (2000 to 2099) Gregorian calender rules:
    //  Every 4 years is a leap year, e.g. [2000, 2004, 2008, ..., 2096]
    if ((year % 4) == 0)
    {
        return 366;
    }
    else
    {
        return 365;
    }
}

static int8_t px_rtc_util_days_in_month(uint8_t year, uint8_t month)
{
    uint8_t days;

    // Sanity checks
    PX_LOG_ASSERT(year <= 99);
    PX_LOG_ASSERT((month >= 1) && (month <= 12));

    // Fetch number of days in month from table
    days = px_pgm_rd_u8(&px_rtc_util_month_day_table[month]);
    // February and leap year?
    if((month == 2) && (px_rtc_util_is_leap_year(year)))
    {
        // Leap year, so there is an extra day in February
        days++;
    }

    return days;
}

static void px_rtc_util_inc_date_time_on_tick(void)
{
    uint8_t days_in_month;

#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    // Increment sub second ticks
    if(++px_rtc_util_date_time.ticks < PX_RTC_UTIL_CFG_TICKS_PER_SEC)
    {
        return;
    }
    px_rtc_util_date_time.ticks = 0;
#endif

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
    // Increment seconds since Y2K
    px_rtc_util_sec_since_y2k++;
#endif

    // Increment seconds
    if(++px_rtc_util_date_time.sec < 60)
    {
        return;
    }
    px_rtc_util_date_time.sec = 0;

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS
    // Set minute flag
    px_rtc_util_period_min_flag = true;
#endif

    // Increment minutes
    if(++px_rtc_util_date_time.min < 60)
    {
        return;
    }
    px_rtc_util_date_time.min = 0;

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS
    // Set hour flag
    px_rtc_util_period_hour_flag = true;
#endif

    // Increment hours
    if(++px_rtc_util_date_time.hour < 24)
    {
        return;
    }
    px_rtc_util_date_time.hour = 0;

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS
    // Set day flag
    px_rtc_util_period_day_flag = true;
#endif

#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    // Next day of week
    if(++px_rtc_util_date_time.day_of_week > 6)
    {
        px_rtc_util_date_time.day_of_week = 0;
    }
#endif

    // Determine number of days in month
    days_in_month = px_rtc_util_days_in_month(px_rtc_util_date_time.year, px_rtc_util_date_time.month);

    // Increment days
    if(++px_rtc_util_date_time.day <= days_in_month)
    {
        return;
    }
    px_rtc_util_date_time.day = 1;

    // Increment months
    if(++px_rtc_util_date_time.month <= 12)
    {
        return;
    }
    px_rtc_util_date_time.month = 1;

    // Increment years
    px_rtc_util_date_time.year++;
}

static bool px_rtc_util_date_time_match(const px_rtc_date_time_t * date_time1,
                                        const px_rtc_date_time_t * date_time2)
{
    if(date_time1->sec != date_time2->sec)
    {
        return false;
    }
    if(date_time1->min != date_time2->min)
    {
        return false;
    }
    if(date_time1->hour != date_time2->hour)
    {
        return false;
    }
    if(date_time1->day != date_time2->day)
    {
        return false;
    }
    if(date_time1->month != date_time2->month)
    {
        return false;
    }
    if(date_time1->year != date_time2->year)
    {
        return false;
    }
    // Match
    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_rtc_util_init(void)
{
    px_rtc_date_time_t date_time;

    // Disable alarm
    px_rtc_util_alarm_mask = PX_RTC_UTIL_ALARM_MASK_DIS;
    px_rtc_util_alarm_flag = false;

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS
    // Reset periodic flags Flag to indicate that minutes have been incremented (every 60 seconds)
    px_rtc_util_period_min_flag  = false;
    px_rtc_util_period_hour_flag = false;
    px_rtc_util_period_day_flag  = false;
#endif

    // Reset time
    px_rtc_util_date_time_reset(&date_time);
    px_rtc_util_date_time_wr(&date_time);

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
    // Reset seconds since Y2K
    px_rtc_util_sec_since_y2k = 0;
#endif
}

void px_rtc_util_on_tick(void)
{
    // Increment time
    px_rtc_util_inc_date_time_on_tick();

#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    // Second elapsed?
    if(px_rtc_util_date_time.ticks != 0)
    {
        // No
        return;
    }
#endif

    // Must alarm flag be set?
    if(px_rtc_util_alarm_mask == PX_RTC_UTIL_ALARM_MASK_DIS)
    {
        // No
        return;
    }
    // Compare seconds?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_SEC)
    {
        // Seconds match?
        if(px_rtc_util_date_time.sec != px_rtc_util_alarm.sec)
        {
            // No
            return;
        }
    }
    // Compare minutes?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_MIN)
    {
        // Seconds match?
        if(px_rtc_util_date_time.min != px_rtc_util_alarm.min)
        {
            // No
            return;
        }
    }
    // Compare hours?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_HOUR)
    {
        // Hours match?
        if(px_rtc_util_date_time.hour != px_rtc_util_alarm.hour)
        {
            // No
            return;
        }
    }
    // Compare days?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_DAY)
    {
        // Days match?
        if(px_rtc_util_date_time.day != px_rtc_util_alarm.day)
        {
            // No
            return;
        }
    }
    // Compare months?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_MONTH)
    {
        // Months match?
        if(px_rtc_util_date_time.month != px_rtc_util_alarm.month)
        {
            // No
            return;
        }
    }
    // Compare years?
    if(px_rtc_util_alarm_mask & PX_RTC_UTIL_ALARM_MASK_YEAR)
    {
        // Years match?
        if(px_rtc_util_date_time.year != px_rtc_util_alarm.year)
        {
            // No
            return;
        }
    }

    /// Set flag to indicate that an alarm has occurred
    px_rtc_util_alarm_flag = true;
}

void px_rtc_util_date_time_wr(px_rtc_date_time_t * date_time)
{
    px_rtc_date_time_t      date_time_cmp;
#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
    px_rtc_sec_since_y2k_t  sec_since_y2k;
#endif

    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time));

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
    // Convert date-time to seconds since Y2K
    sec_since_y2k = px_rtc_util_date_time_to_sec_since_y2k(date_time);
#endif
#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    // Set day of week
    date_time->day_of_week = px_rtc_util_date_to_day_of_week(date_time);
#endif
#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    // Set ticks to zero
    date_time->ticks = 0;
#endif

    // Repeat writing date-time and sec since Y2K together until they match
    while(true)
    {
        // Write date-time
        memcpy(&px_rtc_util_date_time, date_time, sizeof(px_rtc_date_time_t));
#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
        // Write seconds since Y2K
        px_rtc_util_sec_since_y2k = sec_since_y2k;
#endif
        // Read back date-time
        memcpy(&date_time_cmp, &px_rtc_util_date_time, sizeof(px_rtc_date_time_t));
        // date-time match?
        if(!px_rtc_util_date_time_match(date_time, &date_time_cmp))
        {
            // No
            continue;
        }
#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
        // sec since Y2K match?
        if(px_rtc_util_sec_since_y2k != sec_since_y2k)
        {
            // No
            continue;
        }
#endif
        // Match!
        return;
    }
}

void px_rtc_util_date_time_rd(px_rtc_date_time_t * date_time)
{
    px_rtc_date_time_t date_time_cmp;

    do
    {
        // Read date-time
        memcpy(date_time, &px_rtc_util_date_time, sizeof(px_rtc_date_time_t));
        // Read date-time again
        memcpy(&date_time_cmp, &px_rtc_util_date_time, sizeof(px_rtc_date_time_t));
    }
    // Match?
    while(!px_rtc_util_date_time_match(date_time, &date_time_cmp));
}

#if PX_RTC_UTIL_CFG_SEC_SINCE_Y2K
px_rtc_sec_since_y2k_t px_rtc_util_sec_since_y2k_rd(void)
{
    return px_rtc_util_sec_since_y2k;
}
#endif

void px_rtc_util_alarm_wr(const px_rtc_date_time_t * alarm,
                          px_rtc_alarm_mask_t        alarm_mask)
{
    // Sanity checks
    PX_LOG_ASSERT(alarm->year  <= 99);
    PX_LOG_ASSERT(alarm->month <= 12);
    PX_LOG_ASSERT(alarm->day   <= 31);
    PX_LOG_ASSERT(alarm->hour  <= 23);
    PX_LOG_ASSERT(alarm->min   <= 59);
    PX_LOG_ASSERT(alarm->sec   <= 59);

    // Disable alarm first
    px_rtc_util_alarm_mask = PX_RTC_UTIL_ALARM_MASK_DIS;
    // Write alarm
    memcpy(&px_rtc_util_alarm, alarm, sizeof(px_rtc_util_alarm));
    // Reset alarm flag
    px_rtc_util_alarm_flag = false;
    // Write alarm mask
    px_rtc_util_alarm_mask = alarm_mask;
}

void px_rtc_util_alarm_rd(px_rtc_date_time_t *  alarm,
                          px_rtc_alarm_mask_t * alarm_mask)
{
    // Read alarm
    memcpy(alarm, &px_rtc_util_alarm, sizeof(*alarm));
    // Read alarm mask
    *alarm_mask = px_rtc_util_alarm_mask;
}

bool px_rtc_util_alarm_flag_is_set(void)
{
    return px_rtc_util_alarm_flag;
}

void px_rtc_util_alarm_flag_clear(void)
{
    px_rtc_util_alarm_flag = false;
}

#if PX_RTC_UTIL_CFG_PERIODIC_FLAGS

bool px_rtc_util_period_min_flag_is_set(void)
{
    return px_rtc_util_period_min_flag;
}

void px_rtc_util_period_min_flag_clear(void)
{
    px_rtc_util_period_min_flag = false;
}

bool px_rtc_util_period_hour_flag_is_set(void)
{
    return px_rtc_util_period_hour_flag;
}

void px_rtc_util_period_hour_flag_clear(void)
{
    px_rtc_util_period_hour_flag = false;
}

bool px_rtc_util_period_day_flag_is_set(void)
{
    return px_rtc_util_period_day_flag;
}

void px_rtc_util_period_day_flag_clear(void)
{
    px_rtc_util_period_day_flag = false;
}

#endif

void px_rtc_util_date_time_reset(px_rtc_date_time_t * date_time)
{
    date_time->year         = 0;
    date_time->month        = 1;
    date_time->day          = 1;
    date_time->hour         = 0;
    date_time->min          = 0;
    date_time->sec          = 0;
#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    date_time->day_of_week  = PX_RTC_UTIL_DAY_SAT;
#endif
#if PX_RTC_UTIL_CFG_TICKS_PER_SEC
    date_time->ticks        = 0;
#endif
}

bool px_rtc_util_date_time_fields_valid(const px_rtc_date_time_t * date_time)
{
    // Year?
    if(date_time->year > 99)
    {
        PX_LOG_D("Year %u is invalid", date_time->year);
        return false;
    }
    // Month?
    if((date_time->month <  1) || (date_time->month > 12))
    {
        PX_LOG_D("Month %u is invalid", date_time->month);
        return false;
    }
    // Day?
    if((date_time->day < 1) || (date_time->day > px_rtc_util_days_in_month(date_time->year, date_time->month)))
    {
        PX_LOG_D("Day %u is invalid", date_time->day);
        return false;
    }
    // Hour?
    if(date_time->hour > 23)
    {
        PX_LOG_D("Hour %u is invalid", date_time->hour);
        return false;
    }
    // Minute?
    if(date_time->min > 59)
    {
        PX_LOG_D("Minute %u is invalid", date_time->min);
        return false;
    }
    // Second?
    if(date_time->sec > 59)
    {
        PX_LOG_D("Second %u is invalid", date_time->sec);
        return false;
    }

    return true;
}

px_rtc_time_compare_t px_rtc_util_cmp_date_time(const px_rtc_date_time_t * date_time1,
                                                const px_rtc_date_time_t * date_time2)
{
    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time1));
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time2));

    // Year?
    if(date_time1->year > date_time2->year)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->year < date_time2->year)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Month?
    if(date_time1->month > date_time2->month)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->month < date_time2->month)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Day?
    if(date_time1->day > date_time2->day)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->day < date_time2->day)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Hour?
    if(date_time1->hour > date_time2->hour)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->hour < date_time2->hour)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Minute?
    if(date_time1->min > date_time2->min)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->min < date_time2->min)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Second?
    if(date_time1->sec > date_time2->sec)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->sec < date_time2->sec)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }

    return PX_RTC_UTIL_TIME_EQUAL;
}

px_rtc_time_compare_t px_rtc_util_cmp_date(const px_rtc_date_time_t * date_time1,
                                           const px_rtc_date_time_t * date_time2)
{
    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time1));
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time2));

    // Year?
    if(date_time1->year > date_time2->year)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->year < date_time2->year)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Month?
    if(date_time1->month > date_time2->month)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->month < date_time2->month)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }
    // Day?
    if(date_time1->day > date_time2->day)
    {
        return PX_RTC_UTIL_TIME_NEWER;
    }
    else if(date_time1->day < date_time2->day)
    {
        return PX_RTC_UTIL_TIME_OLDER;
    }

    return PX_RTC_UTIL_TIME_EQUAL;
}

bool px_rtc_util_date_is_equal(const px_rtc_date_time_t * date_time1,
                               const px_rtc_date_time_t * date_time2)
{
    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time1));
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time2));

    // Year?
    if(date_time1->year != date_time2->year)
    {
        return false;
    }
    // Month?
    if(date_time1->month != date_time2->month)
    {
        return false;
    }
    // Day?
    if(date_time1->day != date_time2->day)
    {
        return false;
    }

    return true;
}

px_rtc_sec_since_y2k_t px_rtc_util_date_time_to_sec_since_y2k(const px_rtc_date_time_t * date_time)
{
    int8_t                 i;
    uint16_t               days;
    px_rtc_sec_since_y2k_t sec_since_y2k;

    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time));

    // Years (ignoring extra day for each leap year)
    sec_since_y2k = date_time->year * PX_RTC_SEC_PER_DAY * 365ul;

    // Calculate completed days in specified year
    days = date_time->day - 1;
    for(i = 1; i < date_time->month; i++)
    {
        days += px_rtc_util_days_in_month(date_time->year, i);
    }

    // Add an extra day for each leap year using simplified (2000 to 2099)
    // Gregorian calender rules. Every 4th year is a leap year, including 2000,
    // e.g. 2000, 2004, 2008, 2012, ...
    for(i = 0; i < date_time->year; i++)
    {
        if(px_rtc_util_is_leap_year(i))
        {
            days++;
        }
    }

    // Days
    sec_since_y2k += days * PX_RTC_SEC_PER_DAY;
    // Hours
    sec_since_y2k += date_time->hour * PX_RTC_SEC_PER_HOUR;
    // Minutes
    sec_since_y2k += date_time->min * PX_RTC_SEC_PER_MIN;
    // Seconds
    sec_since_y2k += date_time->sec;

    return sec_since_y2k;
}

void px_rtc_util_sec_since_y2k_to_date_time(px_rtc_sec_since_y2k_t sec_since_y2k,
                                            px_rtc_date_time_t *   date_time)
{
    uint32_t seconds;
    uint16_t days;

    // Calculate number of completed days since Y2K
    days    = (uint16_t)(sec_since_y2k / PX_RTC_SEC_PER_DAY);
#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    // Calculate day of week (2000:01:01 was a Saturday)
    date_time->day_of_week = (days + PX_RTC_UTIL_DAY_SAT) % 7;
#endif
    // Calculate number of seconds since midnight for specified day
    seconds = sec_since_y2k % PX_RTC_SEC_PER_DAY;

    // Calculate hour
    date_time->hour = (uint8_t)(seconds / PX_RTC_SEC_PER_HOUR);
    seconds         = seconds % PX_RTC_SEC_PER_HOUR;
    // Calculate minute
    date_time->min  = (uint8_t)(seconds / PX_RTC_SEC_PER_MIN);
    // Calculate seconds
    date_time->sec  = (uint8_t)(seconds % PX_RTC_SEC_PER_MIN);

    // Calculate year (every four years has one leap day)
    date_time->year = (uint8_t)(days / (365*4 + 1)) * 4;
    days            = days % (365*4 + 1);
    while(days >= px_rtc_util_days_in_year(date_time->year))
    {
        days -= px_rtc_util_days_in_year(date_time->year);
        date_time->year++;
    }

    // Calculate month
    date_time->month = 1;
    while(days >= px_rtc_util_days_in_month(date_time->year, date_time->month))
    {
        days -= px_rtc_util_days_in_month(date_time->year, date_time->month);
        date_time->month++;
    }

    // Calculate day
    date_time->day = days + 1;
}

void px_rtc_util_date_time_inc(px_rtc_date_time_t *       date_time,
                               const px_rtc_date_time_t * date_time_inc)
{
    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time));
    PX_LOG_ASSERT(date_time_inc->year  <= 99);
    PX_LOG_ASSERT(date_time_inc->month <= 12);
    PX_LOG_ASSERT(date_time_inc->day   <= 31);
    PX_LOG_ASSERT(date_time_inc->hour  <= 23);
    PX_LOG_ASSERT(date_time_inc->min   <= 59);
    PX_LOG_ASSERT(date_time_inc->sec   <= 59);

    // Seconds
    date_time->sec += date_time_inc->sec;
    while(date_time->sec >= 60)
    {
        date_time->sec -= 60;
        date_time->min++;
    }
    // Minutes
    date_time->min += date_time_inc->min;
    while(date_time->min >= 60)
    {
        date_time->min -= 60;
        date_time->hour++;
    }
    // Hours
    date_time->hour += date_time_inc->hour;
    while(date_time->hour >= 24)
    {
        date_time->hour -= 24;
        date_time->day++;
    }
    // Days
    date_time->day += date_time_inc->day;
    while(date_time->day > px_rtc_util_days_in_month(date_time->year, date_time->month))
    {
        date_time->day -= px_rtc_util_days_in_month(date_time->year, date_time->month);
        if(++date_time->month > 12)
        {
            date_time->month -= 12;
            date_time->year++;
            // Overflow?
            if(date_time->year > 99)
            {
                // Reset
                PX_LOG_E("Overflow occured");
                px_rtc_util_date_time_reset(date_time);
                return;
            }
        }
    }
    // Months
    date_time->month += date_time_inc->month;
    while(date_time->month > 12)
    {
        date_time->month -= 12;
        date_time->year++;
    }
    // Years
    date_time->year += date_time_inc->year;

    // Overflow?
    if(date_time->year > 99)
    {
        // Reset
        PX_LOG_E("Overflow occured");
        px_rtc_util_date_time_reset(date_time);
        return;
    }

#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    // Calculate day of week
    date_time->day_of_week = px_rtc_util_date_to_day_of_week(date_time);
#endif
}

void px_rtc_util_date_time_dec(px_rtc_date_time_t *       date_time,
                               const px_rtc_date_time_t * date_time_dec)
{
    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time));
    PX_LOG_ASSERT(date_time_dec->year  <= 99);
    PX_LOG_ASSERT(date_time_dec->month <= 12);
    PX_LOG_ASSERT(date_time_dec->day   <= 31);
    PX_LOG_ASSERT(date_time_dec->hour  <= 23);
    PX_LOG_ASSERT(date_time_dec->min   <= 59);
    PX_LOG_ASSERT(date_time_dec->sec   <= 59);

    // Seconds
    date_time->sec -= date_time_dec->sec;
    while(date_time->sec >= 60)
    {
        date_time->min--;
        date_time->sec += 60;
    }
    // Minutes
    date_time->min -= date_time_dec->min;
    while(date_time->min >= 60)
    {
        date_time->hour--;
        date_time->min += 60;
    }
    // Hours
    date_time->hour -= date_time_dec->hour;
    while(date_time->hour >= 24)
    {
        date_time->day--;
        date_time->hour += 24;
    }
    // Days
    date_time->day -= date_time_dec->day;
    while((date_time->day < 1) || (date_time->day > 31))
    {
        if(--date_time->month < 1)
        {
            date_time->month += 12;
            date_time->year--;
            // Underflow?
            if(date_time->year > 99)
            {
                // Reset
                PX_LOG_E("Underflow occured");
                px_rtc_util_date_time_reset(date_time);
                return;
            }
        }
        date_time->day += px_rtc_util_days_in_month(date_time->year, date_time->month);
    }
    // Months
    date_time->month -= date_time_dec->month;
    while((date_time->month < 1) || (date_time->month > 12))
    {
        date_time->year--;
        date_time->month += 12;
    }
    // Years
    date_time->year -= date_time_dec->year;

    // Underflow?
    if(date_time->year > 99)
    {
        // Reset
        PX_LOG_E("Underflow occured");
        px_rtc_util_date_time_reset(date_time);
        return;
    }

#if PX_RTC_UTIL_CFG_DAY_OF_WEEK
    // Calculate day of week
    date_time->day_of_week = px_rtc_util_date_to_day_of_week(date_time);
#endif
}

px_rtc_util_day_t px_rtc_util_date_to_day_of_week(const px_rtc_date_time_t * date_time)
{
    int8_t      i;
    uint16_t    days;
    uint8_t     day;

    // Sanity checks
    PX_LOG_ASSERT(px_rtc_util_date_time_fields_valid(date_time));

    // Years (ignoring extra day for each leap year)
    days = date_time->year * 365ul;
    // Add an extra day for each leap year using simplified (2000 to 2099)
    // Gregorian calender rules. Every 4th year is a leap year, including 2000,
    // e.g. 2000, 2004, 2008, 2012, ...
    for(i = 0; i < date_time->year; i++)
    {
        if(px_rtc_util_is_leap_year(i))
        {
            days++;
        }
    }
    // Calculate completed days in specified year
    days += date_time->day - 1;
    for(i = 1; i < date_time->month; i++)
    {
        days += px_rtc_util_days_in_month(date_time->year, i);
    }

    // Calculate day of week (2000:01:01 was a Saturday)
    day = (days + PX_RTC_UTIL_DAY_SAT) % 7;

    return day;
}

void px_rtc_util_report_date_time(const px_rtc_date_time_t * date_time)
{
    PX_PRINTF_P("%02hu-%02hu-%02hu %02hu:%02hu:%02hu",
                (unsigned short)date_time->year,
                (unsigned short)date_time->month,
                (unsigned short)date_time->day,
                (unsigned short)date_time->hour,
                (unsigned short)date_time->min,
                (unsigned short)date_time->sec);
}

void px_rtc_util_report_date(const px_rtc_date_time_t * date_time)
{
    PX_PRINTF_P("%02hu-%02hu-%02hu",
                (unsigned short)date_time->year,
                (unsigned short)date_time->month,
                (unsigned short)date_time->day);
}

void px_rtc_util_report_time(const px_rtc_date_time_t * date_time)
{
    PX_PRINTF_P("%02hu:%02hu:%02hu",
                (unsigned short)date_time->hour,
                (unsigned short)date_time->min,
                (unsigned short)date_time->sec);
}
