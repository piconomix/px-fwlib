#ifndef __PX_LIS3DH_H__
#define __PX_LIS3DH_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_lis3dh.h : ST LIS3DH accelerometer driver
    Author(s):      Pieter Conradie
    Creation Date:  2020-11-24

============================================================================= */

/**
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_LIS3DH px_lis3dh.h : ST LIS3DH accelerometer driver
 *
 *  File(s):
 *  - devices/sensor/inc/px_lis3dh.h
 *  - devices/sensor/src/px_lis3dh.c
 *
 *  Reference:
 *  - [ST LIS3DH accelerometer datasheet](https://www.st.com/resource/en/datasheet/lis3dh.pdf)
 *
 *  This driver initialises the accelerometer with a High Pass Filter enabled
 *  to detect movement. It tries to filter out the static G-force applied by our
 *  little blue green planet and be orientation agnostic.
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
/// I2C Slave address
#define PX_LIS3DH_I2C_SLA_ADR   0x18

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Acelerometer XYZ data
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} px_lis3dh_xyz_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
bool px_lis3dh_init              (px_i2c_handle_t * handle);
bool px_lis3dh_enable            (void);
bool px_lis3dh_disable           (void);

bool px_lis3dh_xyz_data_ready    (void);
bool px_lis3dh_rd_xyz_data       (px_lis3dh_xyz_t * xyz);
bool px_lis3dh_set_int1_threshold(uint8_t threshold);
bool px_lis3dh_rd_int1_src       (uint8_t * data);
bool px_lis3dh_rd_int2_src       (uint8_t * data);

bool px_lis3dh_rd_who_am_i       (uint8_t * data);
bool px_lis3dh_connected         (void);

bool px_lis3dh_test_start        (void);
bool px_lis3dh_test_stop         (void);
bool px_lis3dh_test_active       (void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
