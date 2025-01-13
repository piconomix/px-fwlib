#ifndef __PX_VEML7700_H__
#define __PX_VEML7700_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2024 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_veml7700.h : Vishay VEML7700 ambient light sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2024-09-30

============================================================================= */

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_VEML7700 px_veml7700.h : Vishay VEML7700 ambient light sensor driver
 *  
 *  File(s):
 *  - devices/sensor/inc/px_veml7700.h
 *  - devices/sensor/src/px_veml7700.c
 *  
 *  Reference: 
 *  - [Vishay VEML7700 datasheet](https://www.vishay.com/docs/84286/veml7700.pdf)
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
/// VEML7700 I2C Slave address
#define PX_VEML7700_I2C_SLA_ADR   0x10

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Raw sensor data
typedef struct
{
    uint16_t uva;
    uint16_t uvb;
    uint16_t uvcomp1;
    uint16_t uvcomp2;
} px_veml7700_data_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver.
 *  
 *  @param handle   I2C handle of device
 *  
 *  @retval true    Device detected on I2C and ID value is correct
 *  @retval false   Device not detected on I2C bus 
 */
bool px_veml7700_init(px_i2c_handle_t * handle);

/**
 *  Take UVA & UVB measurement.
 *  
 *  @param data Pointer to structure to contain sensor data
 *  
 *  @retval true    Measurement made
 *  @retval false   Failure
 */
bool px_veml7700_meas(px_veml7700_data_t * data);

/**
 *  Calculate UVA from raw sensor data.
 *  
 *  @param data     Pointer to raw sensor data
 *  
 *  @return float   UVA
 */
float px_veml7700_uva(const px_veml7700_data_t * data);


/**
 *  Calculate UVB from raw sensor data.
 *  
 *  @param data     Pointer to raw sensor data
 *  
 *  @return float   UVB
 */
float px_veml7700_uvb(const px_veml7700_data_t * data);

/**
 *  Calculate UVI from raw sensor data.
 *  
 *  @param data     Pointer to raw sensor data
 *  
 *  @return float   UVI
 */
float px_veml7700_uvi(const px_veml7700_data_t * data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
