#ifndef __PX_BMP280_H__
#define __PX_BMP280_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          Bosch BMP280 Barometric pressure sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-07-12

============================================================================= */

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_BMP280 px_bmp280.h : Bosch BMP280 Barometric pressure sensor driver
 *  
 *  File(s):
 *  - devices/sensor/inc/px_bmp280.h
 *  - devices/sensor/src/px_bmp280.c 
 *   
 *  Reference: 
 *  - [Bosch BMP280 datasheet](https://www.bosch-sensortec.com/bst/products/all_products/bmp280)
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
/// BMP280 I2C Slave address
#define PX_BMP280_I2C_SLA_ADR   0x76

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
extern bool px_bmp280_init(px_i2c_handle_t * handle);
extern bool px_bmp280_read(int32_t * temperature, int32_t * pressure);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
