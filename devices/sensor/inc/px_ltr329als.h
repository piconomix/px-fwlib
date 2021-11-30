#ifndef __PX_LTR329ALS_H__
#define __PX_LTR329ALS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_ltr329als.h : Lite-On LTR329ALS light sensor sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2020-10-01

============================================================================= */

/**
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_LTR329ALS px_ltr329als.h : Lite-On LTR329ALS light sensor sensor driver
 *
 *  File(s):
 *  - devices/sensor/inc/px_ltr329als.h
 *  - devices/sensor/src/px_ltr329als.c
 *
 *  Reference:
 *  - [Lite-On LTR329ALS datasheet](https://optoelectronics.liteon.com/upload/download/DS86-2014-0006/LTR-329ALS-01_DS_V1.5.PDF)
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// LTR329ALS I2C Slave address
#define PX_LTR329ALS_I2C_SLA_ADR        0x29

/// LTR329ALS Manufacturer ID value
#define PX_LTR329ALS_MANUFACTURER_ID    0x05

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

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
bool px_ltr329als_init                   (px_i2c_handle_t *        handle);
bool px_ltr329als_rd_id                  (uint8_t *                id);
void px_ltr329als_cfg                    (px_ltr329als_int_time_t  int_time,
                                          px_ltr329als_meas_rate_t meas_rate);
void px_ltr329als_standby_mode           (void);
void px_ltr329als_active_mode            (px_ltr329als_gain_t      gain);
bool px_ltr329als_meas_ready             (void);
void px_ltr329als_meas_rd                (uint16_t *               ch0,
                                          uint16_t *               ch1);
uint16_t px_ltr329als_meas_convert_to_lux(px_ltr329als_gain_t      gain,
                                          px_ltr329als_int_time_t  int_time,
                                          uint16_t                 ch0,
                                          uint16_t                 ch1);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
