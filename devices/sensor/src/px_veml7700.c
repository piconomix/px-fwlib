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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_veml7700.h"
#include "px_i2c.h"

#include "px_log.h"
PX_LOG_NAME("px_veml7700")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name Command registers
/// @{
#define PX_VEML7700_CMD_ALS_CONF_0          0x00
#define PX_VEML7700_CMD_ALS_WH              0x01
#define PX_VEML7700_CMD_ALS_WL              0x02
#define PX_VEML7700_CMD_PWR_SAVING          0x03
#define PX_VEML7700_CMD_ALS                 0x04
#define PX_VEML7700_CMD_WHITE               0x05
#define PX_VEML7700_CMD_ALS_INT             0x06
#define PX_VEML7700_CMD_ID                  0x07
/// @}

/// Device ID value
#define PX_VEML7700_DEV_ID                  0x81
/// Slave address 1 value
#define PX_VEML7700_SLAVE_ADR1              0xc4
/// Slave address 2 value
#define PX_VEML7700_SLAVE_ADR2              0xd4

/// ALS_CONF_0 data definition (table 1, page 7)
typedef union
{
    uint16_t val;
    struct
    {
        uint16_t als_sd     : 1;  ///< ALS shut down: 0 = ALS power on; 1 = ALS shut down
        uint16_t als_int_en : 1;  ///< ALS interupt enable: 0 = disable; 1 = enable
        uint16_t reserved1  : 2;  ///< Set to 0
        uint16_t als_pers   : 2;  ///< ALS persistence protect number: 0 = 1; 1 = 2; 2 = 4; 3 = 8
        uint16_t als_it     : 4;  ///< ALS integration time: 12 = 25 ms; 8 = 50 ms; 0 = 100 ms; 1 = 200 ms; 2 = 400 ms; 3 = 800 ms
        uint16_t reserved2  : 1;  ///< Set to 0
        uint16_t als_gain   : 2;  ///< Gain selection: 0 = ALS gain x 1; 1 = ALS gain x 2; 2 = ALS gain x (1/8); 3 = ALS gain x (1/4)
        uint16_t reserved3  : 3;  ///< Set to 0
    }
} px_veml7700_data_als_conf_0_t;

/// PWR_SAVING data definition (table 4, page 8)
typedef union
{
    uint16_t val;
    struct
    {
        uint16_t pwm_en     : 1;  ///< Power saving mode enable: 0 = disable; 1 = enable
        uint16_t psm        : 2;  ///< Power saving mode; 0 = mode 1; 1 = mode 2; 2 = mode 3; 3 = mode 4
        uint16_t reserved1  : 13; ///< Set to 0
    }
} px_veml7700_data_pwr_saving_t;

/// ALS_INT data definition (table 7, page 9)
typedef union
{
    uint16_t val;
    struct
    {
        uint16_t reserved1  : 13; ///< Set to 0
        uint16_t int_th_hi  : 1;  ///< 1 = high threshold exceeded
        uint16_t int_th_lo  : 1;  ///< 1 = low threshold exceeded
    }
} px_veml7700_data_pwr_saving_t;

/// ID data definition (table 8, page 9)
typedef union
{
    uint16_t val;
    struct
    {
        uint16_t dev_id     : 8;  ///< 0x81
        uint16_t slave_adr  : 8;  ///< 0xc4 or 0xd4
    }
} px_veml7700_data_id_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_veml7700_i2c_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_veml7700_wr(uint8_t cmd, uint16_t data)
{
    uint8_t buf[3];

    buf[0] = cmd;
    buf[1] = PX_U16_LO8(data);
    buf[2] = PX_U16_HI8(data);

    if(!px_i2c_wr(px_veml7700_i2c_handle, buf, 3, PX_I2C_FLAG_START_AND_STOP))
    {
        PX_LOG_E("Wr");
        return false;
    }

    return true;
}

static bool px_veml7700_rd(uint8_t cmd, uint16_t * data)
{
    uint8_t buf[2];

    buf[0] = cmd;
    if(!px_i2c_wr(px_veml7700_i2c_handle, buf, 1, PX_I2C_FLAG_START_AND_END))
    {
        PX_LOG_E("Wr");
        return false;
    }
    if(!px_i2c_rd(px_veml7700_i2c_handle, buf, 2, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        PX_LOG_E("Rd");
        return false;
    }
    *data = PX_U16_CONCAT_U8(buf[1], buf[0]);

    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_veml7700_init(px_i2c_handle_t * handle)
{
    px_veml7700_reg_data reg_data;

    // Save I2C slave handle
    px_veml7700_i2c_handle = handle;

    // Verify Chip ID
    if(!px_veml7700_rd(PX_VEML7700_REG_ID, &reg_data))
    {
        // Error
        PX_LOG_E("Unable to read Chip ID");
        return false;
    }
    if(reg_data.lsb != PX_VEML7700_REG_ID_VAL)
    {
        // Error
        PX_LOG_E("Chip ID does not match. Received %02X, but expected %02X",
                   reg_data.lsb, PX_VEML7700_REG_ID_VAL);
        return false;
    }
    PX_LOG_D("UV sensor detected");
    // Configure sensor
    reg_data.lsb =   PX_VEML7700_REG_CFG_UV_IT_100MS
                   | PX_VEML7700_REG_CFG_UV_TRIG_NO
                   | PX_VEML7700_REG_CFG_UV_AF_NORMAL;
    reg_data.msb = 0;
    if(!px_veml7700_wr(PX_VEML7700_REG_UV_CFG, &reg_data))
    {
        // Error
        PX_LOG_E("Unable to configure sensor");
        return false;
    }
    if(!px_veml7700_rd(PX_VEML7700_REG_UV_CFG, &reg_data))
    {
        // Error
        PX_LOG_E("Unable to read config");
        return false;
    }
    PX_LOG_D("Cfg = 0x%02X, 0x%02X", reg_data.lsb, reg_data.msb);

    // Success
    return true;
}
