#ifndef __PX_DS18B20_H__
#define __PX_DS18B20_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_ds18b20.h : Maxim 1-Wire digital thermometer driver
    Author(s):      Pieter Conradie
    Creation Date:  2017-02-01

============================================================================= */

/**
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_DS18B20 px_ds18b20.h : Maxim 1-Wire digital thermometer driver
 *
 *  File(s):
 *  - devices/sensor/inc/px_ds18b20.h
 *  - devices/sensor/src/px_ds18b20.c
 *
 *  Reference:
 *  - [Maxim DS18B20 datasheet](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf)
 *
 *  Example:
 *
 *  @include devices/sensor/test/px_ds18b20_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_one_wire.h"

#ifdef __cplusplus
extern "C" {
#endif

/* _____DEFINITIONS__________________________________________________________ */
/// PX_DS18B20 configuration register values
#define PX_DS18B20_CFG_REG_RES_9BIT    (0<<5)
#define PX_DS18B20_CFG_REG_RES_10BIT   (1<<5)
#define PX_DS18B20_CFG_REG_RES_11BIT   (2<<5)
#define PX_DS18B20_CFG_REG_RES_12BIT   (3<<5)
#define PX_DS18B20_CFG_REG_RES_MASK    ((1<<6)|(1<<5))
#define PX_DS18B20_CFG_REG_RESERVED    0x1f

#define PX_DS18B20_FAMILY_CODE         0x28

/// PX_DS18B20 temperature conversion time depending on resolution
#define PX_DS18B20_CONV_TIME_MS_9BIT   94
#define PX_DS18B20_CONV_TIME_MS_10BIT  188
#define PX_DS18B20_CONV_TIME_MS_11BIT  375
#define PX_DS18B20_CONV_TIME_MS_12BIT  750

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef enum
{
    PX_DS18B20_ERR_NONE = 0,
    PX_DS18B20_ERR_NO_DEVICES_PRESENT,
    PX_DS18B20_ERR_CRC_CHECK_FAILED,
} px_ds18b20_error_t;

typedef struct
{
    uint8_t temp_lsb;
    uint8_t temp_msb;
    uint8_t th;
    uint8_t tl;
    uint8_t cfg_reg;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t crc;
} px_ds18b20_scratchpad_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_ds18b20_error_t px_ds18b20_start_temp_conversion     (px_one_wire_rom_t * rom);

bool               px_ds18b20_temp_conversion_finished  (void);

px_ds18b20_error_t px_ds18b20_rd_scratchpad             (px_one_wire_rom_t *       rom,
                                                         px_ds18b20_scratchpad_t * scratchpad);

px_ds18b20_error_t px_ds18b20_wr_scratchpad             (px_one_wire_rom_t *             rom,
                                                         const px_ds18b20_scratchpad_t * scratchpad);

px_ds18b20_error_t px_ds18b20_copy_scratchpad           (px_one_wire_rom_t * rom);


px_ds18b20_error_t px_ds18b20_rd_pwr_supply             (px_one_wire_rom_t *       rom,
                                                         bool *                    bus_pwr_flag);

px_ds18b20_error_t px_ds18b20_rd_temp                   (px_one_wire_rom_t *       rom,
                                                         uint8_t *                 temp_msb,
                                                         uint8_t *                 temp_lsb);

int16_t           px_ds18b20_util_convert_t_to_temp     (uint8_t temp_msb,
                                                         uint8_t temp_lsb,
                                                         uint8_t cfg_reg);

uint16_t          px_ds18b20_util_cfg_to_temp_conv_time_ms(uint8_t cfg_reg);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
