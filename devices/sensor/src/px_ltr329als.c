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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ltr329als.h"
#include "px_i2c.h"

#include "px_log.h"
PX_LOG_NAME("px_ltr329als")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name registers
/// @{
#define PX_LTR329ALS_REG_ALS_CONTR           0x80
#define PX_LTR329ALS_REG_ALS_MEAS_RATE       0x85
#define PX_LTR329ALS_REG_PART_ID             0x86
#define PX_LTR329ALS_REG_MANUFAC_ID          0x87
#define PX_LTR329ALS_REG_ALS_DATA_CH1_0      0x88
#define PX_LTR329ALS_REG_ALS_DATA_CH1_1      0x89
#define PX_LTR329ALS_REG_ALS_DATA_CH0_0      0x8a
#define PX_LTR329ALS_REG_ALS_DATA_CH0_1      0x8b
#define PX_LTR329ALS_REG_STATUS              0x8c
/// @}

/// Part Number ID value
#define PX_LTR329ALS_PART_ID_VAL             0xa0
/// Manufacturer ID value
#define PX_LTR329ALS_MANUFAC_ID_VAL          0x05

typedef union
{
    uint8_t u8;

    /// ALS_CONTR definition
    struct
    {
        uint8_t als_mode : 1;       ///< Active mode: 0 = Stand-by mode (default); 1 = active mode
        uint8_t sw_reset : 1;       ///< 0 = Initial start-up procedure is NOT started; 1 = Initial start-up procedure is started
        uint8_t als_gain : 3;       ///< Gain selection: 0 = 1X; 1 = 2X; 2 = 4X; 3 = 8X; 6 = 48X; 7 = 96X
        uint8_t reserved : 3;       ///< Set to 0
    } als_contr;

    /// ALS_MEAS_RATE definition
    struct
    {
        uint8_t als_meas_rate : 3;  ///< 0 = 50 ms; 1 = 100 ms; 2 = 200 ms; 3 = 500 ms (default); 4 = 1000 ms; 5,6,7 = 2000 ms
        uint8_t als_int_time  : 3;  ///< 0 = 100 ms (default); 1 = 50 ms; 2 = 200 ms; 3 = 400 ms; 4 = 150 ms; 5 = 250 ms; 6 = 300 ms; 7 = 350 ms
        uint8_t reserved      : 2;  ///< Set to 0
    } als_meas_rate;

    /// ALS_STATUS definition
    struct
    {
        uint8_t reserved1           : 2;    ///< Don't care
        uint8_t als_data_status     : 1;    ///< 0 = OLD data; 1 = NEW data
        uint8_t reserved2           : 1;    ///< Don't care
        uint8_t als_data_gain_range : 3;    ///< 0 = 1X (default); 1 = 2X; 2 = 4X; 3 = 8X; 4 = Invalid; 5 = Invalid; 6 = 48X; 7 = 96X
        uint8_t als_data_valid      : 1;    ///< 0 = ALS Data is Valid (default); 1 = ALS Data is Invalid
    } als_status;

} px_ltr329als_reg_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_ltr329als_i2c_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_ltr329als_wr(uint8_t reg, uint8_t val)
{
    uint8_t buf[2];

    buf[0] = reg;
    buf[1] = val;

    if(!px_i2c_wr(px_ltr329als_i2c_handle, buf, 2, PX_I2C_FLAG_START_AND_STOP))
    {
        PX_LOG_E("Wr");
        return false;
    }

    return true;
}

static bool px_ltr329als_rd(uint8_t reg, uint8_t * val)
{
    uint8_t buf[1];

    buf[0] = reg;
    if(!px_i2c_wr(px_ltr329als_i2c_handle, buf, 1, PX_I2C_FLAG_START_AND_END))
    {
        PX_LOG_E("Wr");
        return false;
    }
    if(!px_i2c_rd(px_ltr329als_i2c_handle, val, 1, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        PX_LOG_E("Rd");
        return false;
    }

    return true;
}

static bool px_ltr329als_rd_data(uint8_t reg, void * data, uint8_t nr_of_bytes)
{
    uint8_t buf[1];

    buf[0] = reg;
    if(!px_i2c_wr(px_ltr329als_i2c_handle, buf, 1, PX_I2C_FLAG_START_AND_END))
    {
        PX_LOG_E("Wr");
        return false;
    }
    if(!px_i2c_rd(px_ltr329als_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        PX_LOG_E("Rd");
        return false;
    }

    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_ltr329als_init(px_i2c_handle_t *       handle,
                       px_ltr329als_gain_t     gain,
                       px_ltr329als_int_time_t int_time,
                       px_ltr329als_meas_rate_t meas_rate)
{
    px_ltr329als_reg_t val;

    px_board_delay_ms(100);

    // Save I2C slave handle
    px_ltr329als_i2c_handle = handle;

    // Verify Part ID
    if(!px_ltr329als_rd(PX_LTR329ALS_REG_PART_ID, &val.u8))
    {
        // Error
        PX_LOG_E("Unable to read Part ID");
        return false;
    }
    if(val.u8 != PX_LTR329ALS_PART_ID_VAL)
    {
        // Error
        PX_LOG_E("Part ID fail (%02X)", val.u8);
        return false;
    }
    // Verify Manufacturer ID
    if(!px_ltr329als_rd(PX_LTR329ALS_REG_MANUFAC_ID, &val.u8))
    {
        // Error
        PX_LOG_E("Unable to read Man ID");
        return false;
    }
    if(val.u8 != PX_LTR329ALS_MANUFAC_ID_VAL)
    {
        // Error
        PX_LOG_E("Man ID fail (%02X)", val.u8);
        return false;
    }
    PX_LOG_I("LTR-329ALS found");
    // Configure light sensor
    val.u8                          = 0;
    val.als_meas_rate.als_int_time  = int_time;
    val.als_meas_rate.als_meas_rate = meas_rate;
    if(!px_ltr329als_wr(PX_LTR329ALS_REG_ALS_MEAS_RATE, val.u8))
    {
        // Error
        PX_LOG_E("Unable to set ALS_MEAS_RATE");
        return false;
    }
    // Enable light sensor
    val.u8                 = 0;
    val.als_contr.als_mode = 1;
    val.als_contr.als_gain = gain;
    if(!px_ltr329als_wr(PX_LTR329ALS_REG_ALS_CONTR, val.u8))
    {
        // Error
        PX_LOG_E("Unable to enable");
        return false;
    }

    px_board_delay_ms(10);

    // Success
    return true;
}

bool px_ltr329als_standby_mode(void)
{
    return px_ltr329als_wr(PX_LTR329ALS_REG_ALS_CONTR, 0);
}

bool px_ltr329als_active_mode(px_ltr329als_gain_t gain)
{
    px_ltr329als_reg_t val;

    val.u8                 = 0;
    val.als_contr.als_mode = 1;
    val.als_contr.als_gain = gain;
    if(!px_ltr329als_wr(PX_LTR329ALS_REG_ALS_CONTR, val.u8))
    {
        // Error
        PX_LOG_E("Unable to enable");
        return false;
    }
    return true;
}

bool px_ltr329als_meas_ready(void)
{
    px_ltr329als_reg_t val;

    if(!px_ltr329als_rd(PX_LTR329ALS_REG_STATUS, &val.u8))
    {
        PX_LOG_E("Unable to rd status");
        return false;
    }

    return (val.als_status.als_data_status != 0);
}

bool px_ltr329als_meas_rd(px_ltr329als_meas_t * meas)
{
    // Read Channel 1 and Channel 0 data
    if(!px_ltr329als_rd_data(PX_LTR329ALS_REG_ALS_DATA_CH1_0, (uint8_t *)meas, sizeof(*meas)))
    {
        PX_LOG_E("Unable to rd meas");
        return false;
    }
    return true;
}

uint16_t px_ltr329als_meas_convert_to_lux(px_ltr329als_gain_t         gain,
                                          px_ltr329als_int_time_t     int_time,
                                          const px_ltr329als_meas_t * meas)
{
    uint32_t ratio;
    uint32_t calc;
    uint32_t lux;
    uint8_t  gain_factor;
    uint8_t  int_factor;

    uint32_t ch0 = meas->ch0;
    uint32_t ch1 = meas->ch1;

    if((ch0 == 0) && (ch1 == 0))
    {
        PX_LOG_E("Meas invalid");
        return 0;
    }

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
        PX_LOG_E("Invalid gain");
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
        PX_LOG_E("Invalid integration time");
        return 0;
    }

    // Use fast integer math instead of slow floating point
    ratio = (ch1 * 1024) / (ch0 + ch1);

    if (ratio < (uint32_t)(0.45 * 1024))
    {
        calc =   (uint32_t)(1.7743 * 1024) * ch0
               + (uint32_t)(1.1059 * 1024) * ch1;
    }
    else if (ratio < (uint32_t)(0.64 * 1024))
    {
        calc =   (uint32_t)(4.2785 * 1024) * ch0
               - (uint32_t)(1.9548 * 1024) * ch1;
    }
    else if (ratio < (uint32_t)(0.85 * 1024))
    {
        calc =   (uint32_t)(0.5926 * 1024) * ch0
               + (uint32_t)(0.1185 * 1024) * ch1;
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

    return (uint16_t)lux;
}
