/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Bosch LTR329ALS Barometric pressure sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-12

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ltr329als.h"
#include "px_i2c.h"
#include "px_board.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("ltr329als");

/// @name Register Set (p. 12)
//@{
#define PX_LTR329ALS_REG_ALS_CONTR                 0x80
#define PX_LTR329ALS_REG_ALS_MEAS_RATE             0x85
#define PX_LTR329ALS_REG_PART_ID                   0x86
#define PX_LTR329ALS_REG_MANUFAC_ID                0x87
#define PX_LTR329ALS_REG_ALS_DATA_CH1_0            0x88
#define PX_LTR329ALS_REG_ALS_DATA_CH1_1            0x89
#define PX_LTR329ALS_REG_ALS_DATA_CH0_0            0x8a
#define PX_LTR329ALS_REG_ALS_DATA_CH0_1            0x8b
#define PX_LTR329ALS_REG_ALS_STATUS                0x8c

//@}

// ALS_CONTR Register (0x80)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_1X     (0 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_2X     (1 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_4X     (2 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_8X     (3 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_48X    (6 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_96X    (7 << 2)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_POS    2
#define PX_LTR329ALS_REG_ALS_CONTR_SW_RESET        (1 << 1)
#define PX_LTR329ALS_REG_ALS_CONTR_ALS_MODE        (1 << 0)

// ALS_MEAS_RATE Register (0x81)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_100MS   (0 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_50MS    (1 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_200MS   (2 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_400MS   (3 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_150MS   (4 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_250MS   (5 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_300MS   (6 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_350MS   (7 << 3)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_POS     3
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_50MS        (0 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_100MS       (1 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_200MS       (2 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_500MS       (3 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_1000MS      (4 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_2000MS      (5 << 0)
#define PX_LTR329ALS_REG_ALS_MEAS_RATE_POS         0

// PART_ID Register (0x86)
#define PX_LTR329ALS_REG_PART_ID_PART_NUMBER_POS   4
#define PX_LTR329ALS_REG_PART_ID_PART_NUMBER_MSK   (15 << 4)
#define PX_LTR329ALS_REG_PART_ID_REV_POS           0
#define PX_LTR329ALS_REG_PART_ID_REV_MSK           (15 << 0)

// ALS_STATUS Register (0x8c)
#define PX_LTR329ALS_REG_ALS_STATUS_DATA_VALID     (1 << 7)
#define PX_LTR329ALS_REG_ALS_STATUS_GAIN_POS       4
#define PX_LTR329ALS_REG_ALS_STATUS_GAIN_MSK       (7 << 4)
#define PX_LTR329ALS_REG_ALS_STATUS_INT            (1 << 3)
#define PX_LTR329ALS_REG_ALS_STATUS_DATA           (1 << 2)

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_ltr329als_i2c_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_ltr329als_reg_wr(uint8_t adr, uint8_t data)
{
    uint8_t i2c_data[2];

    i2c_data[0] = adr;
    i2c_data[1] = data;

    if(!px_i2c_wr(px_ltr329als_i2c_handle, i2c_data, 2, PX_I2C_FLAG_START_AND_STOP))
    {
        // Error
        PX_DBG_E("Unable to write register value");
        return false;
    }
    // Success
    return true;
}

static bool px_ltr329als_reg_rd(uint8_t adr, uint8_t * data)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_ltr329als_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_DBG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_ltr329als_i2c_handle,
                  data,
                  1,
                  PX_I2C_FLAG_REP_START_AND_STOP))
    {
        // Error
        PX_DBG_E("Unable to read register value");
        return false;
    }
    // Success
    return true;
}

static bool px_ltr329als_reg_rd_data(uint8_t adr, void * data, uint8_t nr_of_bytes)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_ltr329als_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_DBG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_ltr329als_i2c_handle,
                  data,
                  nr_of_bytes,
                  PX_I2C_FLAG_REP_START_AND_STOP))
    {
        // Error
        PX_DBG_E("Unable to read register data");
        return false;
    }
    // Success
    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_ltr329als_init(px_i2c_handle_t * handle)
{
    // Save I2C slave handle
    px_ltr329als_i2c_handle = handle;
    // Reset sensor
    if(!px_ltr329als_reg_wr(PX_LTR329ALS_REG_ALS_CONTR,
                            PX_LTR329ALS_REG_ALS_CONTR_SW_RESET))
    {
        return false;
    }
    // Success
    return true;
}

bool px_ltr329als_rd_manufacturer_id(uint8_t * id)
{
    // Read manufacturer ID
    if(!px_ltr329als_reg_rd(PX_LTR329ALS_REG_MANUFAC_ID, id))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void px_ltr329als_cfg(px_ltr329als_int_time_t int_time, px_ltr329als_meas_rate_t meas_rate)
{
    // Set integration time to 50 ms and measurement rate to 2 s
    px_ltr329als_reg_wr(PX_LTR329ALS_REG_ALS_MEAS_RATE,
                           (int_time  << PX_LTR329ALS_REG_ALS_MEAS_RATE_INT_POS)
                         | (meas_rate << PX_LTR329ALS_REG_ALS_MEAS_RATE_POS));
}

void px_ltr329als_standby_mode(void)
{
    // Set sensor in standby mode
    px_ltr329als_reg_wr(PX_LTR329ALS_REG_ALS_CONTR, 0);
}

void px_ltr329als_active_mode(px_ltr329als_gain_t gain)
{
    // Set sensor in active mode
    px_ltr329als_reg_wr(PX_LTR329ALS_REG_ALS_CONTR,
                          (gain << PX_LTR329ALS_REG_ALS_CONTR_ALS_GAIN_POS)
                        | PX_LTR329ALS_REG_ALS_CONTR_ALS_MODE);
}

bool px_ltr329als_meas_ready(void)
{
    uint8_t data;

    // Read status register
    if(!px_ltr329als_reg_rd(PX_LTR329ALS_REG_ALS_STATUS, &data))
    {
        return false;
    }
    // New data available?
    if(data & PX_LTR329ALS_REG_ALS_STATUS_DATA)
    {
        // Yes
        return true;
    }
    else
    {
        // No
        return false;
    }
}

void px_ltr329als_meas_rd(uint16_t * ch0, uint16_t * ch1)
{
    uint8_t data[4];

    // Read Channel 1 and Channel 0 data
    if(!px_ltr329als_reg_rd_data(PX_LTR329ALS_REG_ALS_DATA_CH1_0, data, 4))
    {
        return;
    }
    // Assemble data
    *ch1 = PX_U16_CONCAT_U8(data[1], data[0]);
    *ch0 = PX_U16_CONCAT_U8(data[3], data[2]);
}

uint16_t px_ltr329als_meas_convert_to_lux(px_ltr329als_gain_t     gain,
                                          px_ltr329als_int_time_t int_time,
                                          uint16_t                ch0,
                                          uint16_t                ch1)
{
    uint32_t ratio;
    uint32_t calc;
    uint16_t lux;
    uint8_t  gain_factor;
    uint16_t int_factor;

    // Get gain factor
    switch(gain)
    {
    case PX_LTR329ALS_GAIN_1X:  gain_factor = 1;  break;
    case PX_LTR329ALS_GAIN_2X:  gain_factor = 2;  break;
    case PX_LTR329ALS_GAIN_4X:  gain_factor = 4;  break;
    case PX_LTR329ALS_GAIN_8X:  gain_factor = 8;  break;
    case PX_LTR329ALS_GAIN_48X: gain_factor = 48; break;
    case PX_LTR329ALS_GAIN_96X: gain_factor = 96; break;
    default:
        PX_DBG_E("Invalid gain");
        return 0;
    }

    // Get integration time factor
    switch(int_time)
    {
    case PX_LTR329ALS_INT_TIME_100MS: int_factor = 10; break;
    case PX_LTR329ALS_INT_TIME_50MS:  int_factor = 5;  break;
    case PX_LTR329ALS_INT_TIME_200MS: int_factor = 20; break;
    case PX_LTR329ALS_INT_TIME_400MS: int_factor = 40; break;
    case PX_LTR329ALS_INT_TIME_150MS: int_factor = 15; break;
    case PX_LTR329ALS_INT_TIME_250MS: int_factor = 25; break;
    case PX_LTR329ALS_INT_TIME_300MS: int_factor = 30; break;
    case PX_LTR329ALS_INT_TIME_350MS: int_factor = 35; break;
    default:
        PX_DBG_E("Invalid integration time");
        return 0;
    }

    // Use fast integer math instead of slow floating point
    ratio = ((uint32_t)ch1 * 1024) / ((uint32_t)ch0 + (uint32_t)ch1);

    if (ratio < (0.45 * 1024))
    {
        calc =   (1.7743 * 1024) * (uint32_t)ch0
               + (1.1059 * 1024) * (uint32_t)ch1;
    }
    else if (ratio < (0.64 * 1024))
    {
        calc =   (4.2785 * 1024) * (uint32_t)ch0
               - (1.9548 * 1024) * (uint32_t)ch1;
    }
    else if (ratio < (0.85 * 1024))
    {
        calc =   (0.5926 * 1024) * (uint32_t)ch0
               + (0.1185 * 1024) * (uint32_t)ch1;
    }
    else
    {
        return 0;
    }

    // Adjust for gain and integration time
    calc = (calc * 10) / ((uint32_t)gain_factor * (uint32_t)int_factor);

    // Calculate integer part
    lux = calc / 1024;
    // Round up?
    if((calc % 1024) >= 512)
    {
        // Yes
        lux++;
    }

    return lux;
}

