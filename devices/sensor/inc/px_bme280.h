#ifndef __PX_BME280_H__
#define __PX_BME280_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          Bosch BME280 Barometric pressure sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-12

============================================================================= */

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_BME280 px_bme280.h : Bosch BME280 Barometric pressure sensor driver
 *  
 *  File(s):
 *  - devices/sensor/inc/px_bme280.h
 *  - devices/sensor/src/px_bme280.c 
 *   
 *  Reference: 
 *  - [Bosch BME280 datasheet](https://www.bosch-sensortec.com/bst/products/all_products/bme280)
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// BME280 I2C Slave address
#define PX_BME280_I2C_SLA_ADR   0x77

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Raw sensor data
typedef struct
{
    int32_t temp;       ///< Raw temperature reading
    int32_t press;      ///< Raw pressure reading
    int32_t hum;        ///< Raw humidity reading
    int32_t t_fine;     ///< Calculated temperature compensation
} px_bme280_data_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise BME280 and read calibration values
 *  
 *  @param handle   I2C handle of device
 *  
 *  @retval true    BME280 detected and calibration values read 
 *  @retval false   Communication failure with BME280 
 */
extern bool px_bme280_init(px_i2c_handle_t * handle);

/**
 *  Perform reading of raw temperature, pressure and humidty.
 *  
 *  @param data     Pointer to structure to contain raw sensor data
 *  
 *  @retval true    Sensor data succesfully read
 *  @retval false   Communication failure
 */
extern bool px_bme280_read(px_bme280_data_t * data);

/**
 *  Calculate compensated temperature from raw data.
 *  
 *  Returns temperature in deg C; Resolution is 0.01 deg C. For example "5123"
 *  equals 51.23 deg C.
 *  
 *  The data->t_fine field is updated during this step and used subsequently
 *  for compensated pressure and humidity calculations.
 *  
 *  @param data     Pointer to structure to contain raw sensor data
 *  
 *  @return int32_t Compensated temperature.
 */
extern int32_t  px_bme280_temperature(px_bme280_data_t * data);

/**
 *  Calculate compensated pressure from raw data.
 *  
 *  Returns pressure in Pa as unsigned 32-bit integer. For example "96386"
 *  represents 96386 Pa = 963.862 hPa
 *  
 *  px_bme280_temperature() must be called first so that the data->t_fine
 *  is updated.
 *  
 *  @param data     Pointer to structure to contain raw sensor data
 *  
 *  @return int32_t Compensated pressure.
 */
extern uint32_t px_bme280_pressure(px_bme280_data_t * data);

/**
 *  Calculate compensated humidity from raw data.
 *  
 *  Returns humidity in %RH as unsigned 32-bit integer in Q22.10 format (22
 *  integer bits and 10 fractional bits). For example "47445" represents
 *  47445/1024 = 46.333 %RH
 *  
 *  px_bme280_temperature() must be called first so that the data->t_fine
 *  is updated.
 *  
 *  @param data     Pointer to structure to contain raw sensor data
 *  
 *  @return int32_t Compensated humidity.
 */
extern uint32_t px_bme280_humidity(px_bme280_data_t * data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
