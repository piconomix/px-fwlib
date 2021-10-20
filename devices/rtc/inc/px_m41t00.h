#ifndef __PX_RTC_M41T00_H__
#define __PX_RTC_M41T00_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2007 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_m41t00.h : Driver for an ST M41T00 Real Time Clock
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */
/** 
 *  @ingroup DEVICES_RTC
 *  @defgroup PX_M41T00 px_m41t00.h : Driver for an ST M41T00 Real Time Clock
 *  
 *  This driver interface with an ST M41T00 RTC using I2C
 *  
 *  File(s):
 *  - devices/rtc/inc/px_m41t00.h
 *  - devices/rtc/src/px_m41t00.c
 *  
 *  Reference:
 *  - [ST M41T00M6E](http://us1.st.com/stonline/products/literature/ds/6100.pdf) datasheet
 *  
 *  Example:
 *  
 *  @include devices/rtc/test/px_m41t00_test.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// I2C Slave Address
#define PX_M41T00_SLA_ADR 0x68

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Structure to store time and date
typedef struct
{
   uint8_t sec;            ///< Seconds:       0  to 59
   uint8_t min;            ///< Minutes:       0  to 59
   uint8_t hour;           ///< Hours:         0  to 23
   uint8_t day_of_week;    ///< Day of Week:   1  to 7
   uint8_t day_of_month;   ///< Days of Month: 1  to 31 (depending on month)
   uint8_t month;          ///< Months:        1  to 12
   uint8_t year;           ///< Years:         00 to 99
} px_m41t00_time_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver.
 *   
 *  @param px_i2c_handle    Pointer to I2C driver handle 
 */
extern void px_m41t00_init(px_i2c_handle_t * px_i2c_handle);

/**
 *  Get RTC time using I2C driver.
 *  
 *  @param[out] px_rtc_time Pointer to a structure that will contain the new time.
 *  
 *  @retval true            Time succesfully retrieved and copied into structure.
 *  @retval false           Unable to get the time (I2C communication error) 
 *                          or timer has been stopped (new battery inserted)
 */
bool px_m41t00_get_time(px_m41t00_time_t * px_rtc_time);

/**
 *  Set RTC time using I2C driver.
 *  
 *  @param[in] px_rtc_time  Pointer to a structure containing the new time.
 *  
 *  @retval true            Time succesfully set
 *  @retval false           Unable to set the time (I2C communication error)
 */
bool px_m41t00_set_time(const px_m41t00_time_t * px_rtc_time);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
