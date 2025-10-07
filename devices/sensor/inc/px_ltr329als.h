#ifndef __PX_LTR329ALS_H__
#define __PX_LTR329ALS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_ltr329als.h : Liteon LTR-329ALS ambient light sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2025-07-07

============================================================================= */

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_LTR329ALS px_ltr329als.h : LITEON LTR-329ALS ambient light sensor driver
 *  
 *  File(s):
 *  - devices/sensor/inc/px_ltr329als.h
 *  - devices/sensor/src/px_ltr329als.c
 *  
 *  Reference: 
 *  - [LITEON LTR-329ALS datasheet](https://optoelectronics.liteon.com/upload/download/DS86-2014-0006/LTR-329ALS-01_DS_V1.8.PDF)
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// LTR329ALS I2C Slave address
#define PX_LTR329ALS_I2C_SLA_ADR        0x29

/// Gain
typedef enum
{
    PX_LTR329ALS_GAIN_1X            = 0,
    PX_LTR329ALS_GAIN_2X            = 1,
    PX_LTR329ALS_GAIN_4X            = 2,
    PX_LTR329ALS_GAIN_8X            = 3,
    PX_LTR329ALS_GAIN_48X           = 6,
    PX_LTR329ALS_GAIN_96X           = 7,
} px_ltr329als_gain_t;

/// Integration time
typedef enum
{
    PX_LTR329ALS_INT_TIME_100MS     = 0,
    PX_LTR329ALS_INT_TIME_50MS      = 1,
    PX_LTR329ALS_INT_TIME_200MS     = 2,
    PX_LTR329ALS_INT_TIME_400MS     = 3,
    PX_LTR329ALS_INT_TIME_150MS     = 4,
    PX_LTR329ALS_INT_TIME_250MS     = 5,
    PX_LTR329ALS_INT_TIME_300MS     = 6,
    PX_LTR329ALS_INT_TIME_350MS     = 7,
} px_ltr329als_int_time_t;

/// Measurement rate
typedef enum
{
    PX_LTR329ALS_MEAS_RATE_50MS     = 0,
    PX_LTR329ALS_MEAS_RATE_100MS    = 1,
    PX_LTR329ALS_MEAS_RATE_200MS    = 2,
    PX_LTR329ALS_MEAS_RATE_500MS    = 3,
    PX_LTR329ALS_MEAS_RATE_1000MS   = 4,
    PX_LTR329ALS_MEAS_RATE_2000MS   = 5,
} px_ltr329als_meas_rate_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef struct
{
    uint16_t ch1;           ///< IR light
    uint16_t ch0;           ///< Ambient light
} px_ltr329als_meas_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver.
 *
 *  @param handle    I2C handle of device
 *  @param gain      Gain
 *  @param int_time  Integration time
 *  @param meas_rate Measurement rate
 *
 *  @retval true    Device detected on I2C and Manufacturer and ID value is correct
 *  @retval false   Device not detected on I2C bus
 */
bool px_ltr329als_init(px_i2c_handle_t *        handle,
                       px_ltr329als_gain_t      gain,
                       px_ltr329als_int_time_t  int_time,
                       px_ltr329als_meas_rate_t meas_rate);

/**
 *  Put sensor into standby mode.
 */
bool px_ltr329als_standby_mode(void);

/**
 *  Put sensor into active mode.
 *
 *  @param gain
 */
bool px_ltr329als_active_mode(px_ltr329als_gain_t gain);

/**
 *  Check if new data is available to be read.
 *
 *  @return true    New data is available
 *  @return false   OLD data (data already read)
 */
bool px_ltr329als_meas_ready(void);

/**
 *  Read light data.
 *
 *  @return true  Success
 *  @return false Error
 */
bool px_ltr329als_meas_rd(px_ltr329als_meas_t * meas);

/**
 *  Convert measurement to lux
 *
 *  @param gain
 *  @param int_time
 *  @param meas
 *
 *  @return uint16_t
 */
uint16_t px_ltr329als_meas_convert_to_lux(px_ltr329als_gain_t         gain,
                                          px_ltr329als_int_time_t     int_time,
                                          const px_ltr329als_meas_t * meas);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
