#ifndef __PX_MAX31855_H__
#define __PX_MAX31855_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_max31855.h : Maxim MAX31855 thermocouple driver
    Author(s):      Pieter Conradie
    Creation Date:  2020-11-07

============================================================================= */

/**
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_MAX31855 px_max31855.h : Maxim MAX31855 thermocouple driver
 *
 *  File(s):
 *  - devices/sensor/inc/px_max31855.h
 *  - devices/sensor/src/px_max31855.c
 *
 *  Reference:
 *  - [Maxim MAX31855 datasheet](https://datasheets.maximintegrated.com/en/ds/MAX31855.pdf)
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_spi.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef struct
{
    int16_t temp_tc;    ///< 14-bit thermocouple temperature data (0.25 deg C resolution)
    int16_t temp_int;   ///< 12-bit internal temperature data (0.0625 deg C resolution)
    bool    fault;      ///< Fault flag
    bool    fault_scv;  ///< Thermocouple short-circuited to VCC
    bool    fault_scg;  ///< Thermocouple short-circuited to GND
    bool    fault_oc;   ///< Thermocouple is open (no connections)
} px_max31855_data_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_max31855_init(px_spi_handle_t    * handle);
void px_max31855_rd  (px_max31855_data_t * data);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_MAX31855_H__
