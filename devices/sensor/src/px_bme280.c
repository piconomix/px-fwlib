/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          Bosch BME280 Barometric press sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-12

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_bme280.h"
#include "px_i2c.h"
#include "px_board.h"
#include "px_log.h"

#ifdef PX_BME280_DBG
#warning "Debugging of BME280 enabled. Using constants to verify formula"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("bme280");

/// @name Memory Map (table 18, p. 25)
/// @{
#define PX_BME280_REG_CALIB00                   0x88
#define PX_BME280_REG_CALIB25                   0xa1
#define PX_BME280_REG_ID                        0xd0
#define PX_BME280_REG_RESET                     0xe0
#define PX_BME280_REG_CALIB26                   0xe1
#define PX_BME280_REG_CALIB32                   0xe7
#define PX_BME280_REG_CALIB41                   0xf0
#define PX_BME280_REG_CTRL_HUM                  0xf2
#define PX_BME280_REG_STATUS                    0xf3
#define PX_BME280_REG_CTRL_MEAS                 0xf4
#define PX_BME280_REG_CONFIG                    0xf5
#define PX_BME280_REG_PRESS_MSB                 0xf7
#define PX_BME280_REG_PRESS_LSB                 0xf8
#define PX_BME280_REG_PRESS_XLSB                0xf9
#define PX_BME280_REG_TEMP_MSB                  0xfa
#define PX_BME280_REG_TEMP_LSB                  0xfb
#define PX_BME280_REG_TEMP_XLSB                 0xfc
#define PX_BME280_REG_HUM_MSB                   0xfd
#define PX_BME280_REG_HUM_LSB                   0xfe
/// @}

// 7.4.1 Register 0xD0 "id"
#define PX_BME280_REG_ID_VAL                    0x60

// 7.4.2 Register 0xE0 "reset"
#define PX_BME280_REG_RESET_VAL                 0xb6

// 7.4.3 Register 0xF2 "ctrl_hum"
#define PX_BME280_REG_CTRL_HUM_OVERS_SKIPPED    0
#define PX_BME280_REG_CTRL_HUM_OVERS_1          1
#define PX_BME280_REG_CTRL_HUM_OVERS_2          2
#define PX_BME280_REG_CTRL_HUM_OVERS_4          3
#define PX_BME280_REG_CTRL_HUM_OVERS_8          4
#define PX_BME280_REG_CTRL_HUM_OVERS_16         5

// 7.4.4 Register 0xF3 "status"
#define PX_BME280_REG_STATUS_MEAS_POS           3
#define PX_BME280_REG_STATUS_MEAS_MSK           (1 << 3)
#define PX_BME280_REG_STATUS_IM_UPDATE_POS      0
#define PX_BME280_REG_STATUS_IM_UPDATE_MSK      (1 << 0)

// 7.4.5 Register 0xF4 "ctrl_meas"
#define PX_BME280_REG_CTRL_MEAS_OSRS_T_POS      5
#define PX_BME280_REG_CTRL_MEAS_OSRS_T_MSK      (7 << 5)
#define PX_BME280_REG_CTRL_MEAS_OSRS_P_POS      2
#define PX_BME280_REG_CTRL_MEAS_OSRS_P_MSK      (7 << 2)
#define PX_BME280_REG_CTRL_MEAS_MODE_POS        0
#define PX_BME280_REG_CTRL_MEAS_MODE_MSK        (3 << 0)
#define PX_BME280_REG_CTRL_MEAS_OVERS_SKIPPED   0
#define PX_BME280_REG_CTRL_MEAS_OVERS_1         1
#define PX_BME280_REG_CTRL_MEAS_OVERS_2         2
#define PX_BME280_REG_CTRL_MEAS_OVERS_4         3
#define PX_BME280_REG_CTRL_MEAS_OVERS_8         4
#define PX_BME280_REG_CTRL_MEAS_OVERS_16        5
#define PX_BME280_REG_CTRL_MEAS_MODE_SLEEP      0
#define PX_BME280_REG_CTRL_MEAS_MODE_FORCED     1
#define PX_BME280_REG_CTRL_MEAS_MODE_NORMAL     3

// 7.4.6 Register 0xF5 "config"
#define PX_BME280_REG_CONFIG_T_SB_POS           5
#define PX_BME280_REG_CONFIG_T_SB_MSK           (7 << 5)
#define PX_BME280_REG_CONFIG_FILTER_POS         2
#define PX_BME280_REG_CONFIG_FILTER_MSK         (7 << 2)
#define PX_BME280_REG_CONFIG_SPI3W_EN_POS       0
#define PX_BME280_REG_CONFIG_SPI3W_EN_MSK       (1 << 0)

// 7.4.7 Register 0xF7..0xF9 "press" (_msb, _lsb, _xlsb)
#define PX_BME280_PRESS_XLSB_POS                4
#define PX_BME280_PRESS_XLSB_MSK                (15<<4)

// 7.4.8 Register 0xFA..0xFC "temp" (_msb, _lsb, _xlsb)
#define PX_BME280_TEMP_XLSB_POS                 4
#define PX_BME280_TEMP_XLSB_MSK                 (15 << 4)

// 7.4.9 Register 0xFD..0xFE "hum" (_msb, _lsb)

/// Table 16 BME280 calibration coefficients
typedef struct
{
    uint16_t t1;    // 0x88 / 0x89
    int16_t  t2;    // 0x8A / 0x8B
    int16_t  t3;    // 0x8C / 0x8D

    uint16_t p1;    // 0x8E / 0x8F
    int16_t  p2;    // 0x90 / 0x91
    int16_t  p3;    // 0x92 / 0x93
    int16_t  p4;    // 0x94 / 0x95
    int16_t  p5;    // 0x96 / 0x97
    int16_t  p6;    // 0x98 / 0x99
    int16_t  p7;    // 0x9A / 0x9B
    int16_t  p8;    // 0x9C / 0x9D
    int16_t  p9;    // 0x9E / 0x9F

    uint8_t  h1;    // 0xA1
    int16_t  h2;    // 0xE1 / 0xE2
    uint8_t  h3;    // 0xE3
    int16_t  h4;    // 0xE4 / 0xE5[3:0]
    int16_t  h5;    // 0xE5[7:4] / 0xE6
    int8_t   h6;    // 0xE7
} px_bme280_cal_t;

/// Number of calibration coefficients
#define PX_BME280_NR_OF_CALS    12

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_bme280_i2c_handle;
static px_bme280_cal_t   px_bme280_cal;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_bme280_reg_wr(uint8_t adr, uint8_t data)
{
    uint8_t i2c_data[2];

    i2c_data[0] = adr;
    i2c_data[1] = data;

    if(!px_i2c_wr(px_bme280_i2c_handle, i2c_data, 2, PX_I2C_FLAG_START_AND_STOP))
    {
        // Error
        PX_LOG_E("Unable to write register value");
        return false;
    }
    // Success
    return true;
}

static bool px_bme280_reg_rd(uint8_t adr, uint8_t * data)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_bme280_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_bme280_i2c_handle,
                  data,
                  1,
                  PX_I2C_FLAG_REP_START_AND_STOP))
    {
        // Error
        PX_LOG_E("Unable to read register value");
        return false;
    }
    // Success
    return true;
}

static bool px_bme280_reg_rd_data(uint8_t adr, void * data, uint8_t nr_of_bytes)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_bme280_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_bme280_i2c_handle,
                  data,
                  nr_of_bytes,
                  PX_I2C_FLAG_REP_START_AND_STOP))
    {
        // Error
        PX_LOG_E("Unable to read register data");
        return false;
    }
    // Success
    return true;
}

static bool px_bme280_cal_rd(void)
{
    uint8_t  data[26];

    // Read CALIB00 to CALIB25
    if(!px_bme280_reg_rd_data(PX_BME280_REG_CALIB00, &data[0], PX_BME280_REG_CALIB25 - PX_BME280_REG_CALIB00 + 1))
    {
        // Error
        PX_LOG_E("Unable to read CALIB00 to CALIB25");
        return false;
    }
    // Assemble calibration values
    px_bme280_cal.t1 =          PX_U16_CONCAT_U8(data[1],  data[0]);
    px_bme280_cal.t2 = (int16_t)PX_U16_CONCAT_U8(data[3],  data[2]);
    px_bme280_cal.t3 = (int16_t)PX_U16_CONCAT_U8(data[5],  data[4]);
    px_bme280_cal.p1 =          PX_U16_CONCAT_U8(data[7],  data[6]);
    px_bme280_cal.p2 = (int16_t)PX_U16_CONCAT_U8(data[9],  data[8]);
    px_bme280_cal.p3 = (int16_t)PX_U16_CONCAT_U8(data[11], data[10]);
    px_bme280_cal.p4 = (int16_t)PX_U16_CONCAT_U8(data[13], data[12]);
    px_bme280_cal.p5 = (int16_t)PX_U16_CONCAT_U8(data[15], data[14]);
    px_bme280_cal.p6 = (int16_t)PX_U16_CONCAT_U8(data[17], data[16]);
    px_bme280_cal.p7 = (int16_t)PX_U16_CONCAT_U8(data[19], data[18]);
    px_bme280_cal.p8 = (int16_t)PX_U16_CONCAT_U8(data[21], data[20]);
    px_bme280_cal.p9 = (int16_t)PX_U16_CONCAT_U8(data[23], data[22]);
    px_bme280_cal.h1 = data[25];
    // Read CALIB26 to CALIB32
    if(!px_bme280_reg_rd_data(PX_BME280_REG_CALIB26, &data[0], PX_BME280_REG_CALIB32 - PX_BME280_REG_CALIB26 + 1))
    {
        // Error
        PX_LOG_E("Unable to read CALIB26 to CALIB32");
        return false;
    }
    // Assemble calibration values
    px_bme280_cal.h2 = (int16_t)PX_U16_CONCAT_U8(data[1], data[0]);
    px_bme280_cal.h3 = data[2];
    px_bme280_cal.h4 = ((int16_t)(int8_t)data[3] * 16) | ((int16_t)(data[4] & 0x0f));
    px_bme280_cal.h5 = ((int16_t)(int8_t)data[5] * 16) | ((int16_t)(data[4] >> 4));
    px_bme280_cal.h6 = (int8_t)data[6];

    if(PX_LOG_LEVEL_IS_D())
    {
        // Report calibration values
        PX_LOG_D("BME280 Cal:");
        PX_LOG_TRACE("T1 = %d\n",   px_bme280_cal.t1);
        PX_LOG_TRACE("T2 = %d\n",   px_bme280_cal.t2);
        PX_LOG_TRACE("T3 = %u\n",   px_bme280_cal.t3);
        PX_LOG_TRACE("P1 = %d\n",   px_bme280_cal.p1);
        PX_LOG_TRACE("P2 = %d\n",   px_bme280_cal.p2);
        PX_LOG_TRACE("P3 = %d\n",   px_bme280_cal.p3);
        PX_LOG_TRACE("P4 = %d\n",   px_bme280_cal.p4);
        PX_LOG_TRACE("P5 = %d\n",   px_bme280_cal.p5);
        PX_LOG_TRACE("P6 = %d\n",   px_bme280_cal.p6);
        PX_LOG_TRACE("P7 = %d\n",   px_bme280_cal.p7);
        PX_LOG_TRACE("P8 = %d\n",   px_bme280_cal.p8);
        PX_LOG_TRACE("P9 = %d\n",   px_bme280_cal.p9);
        PX_LOG_TRACE("H1 = %hhu\n", px_bme280_cal.h1);
        PX_LOG_TRACE("H2 = %hd\n",  px_bme280_cal.h2);
        PX_LOG_TRACE("H3 = %hhu\n", px_bme280_cal.h3);
        PX_LOG_TRACE("H4 = %hd\n",  px_bme280_cal.h4);
        PX_LOG_TRACE("H5 = %hd\n",  px_bme280_cal.h5);
        PX_LOG_TRACE("H6 = %hhd\n", px_bme280_cal.h6);
    }

    return true;
}

static bool px_bme280_temp_raw_rd(int32_t * data)
{
    uint8_t i2c_data[3];

    if(!px_bme280_reg_rd_data(PX_BME280_REG_TEMP_MSB, i2c_data, 3))
    {
        // Error
        PX_LOG_E("Unable to read temp");
        return false;
    }
    PX_LOG_D("Temp data: %02X %02X %02X", i2c_data[0], i2c_data[1], i2c_data[2]);
    // Assemble value
    *data = (int32_t)(   (((uint32_t)i2c_data[0]) << 12)
                       | (((uint32_t)i2c_data[1]) <<  4)
                       | (((uint32_t)i2c_data[2]) >>  4) );

    return true;
}

static bool px_bme280_press_raw_rd(int32_t * data)
{
    uint8_t i2c_data[3];

    if(!px_bme280_reg_rd_data(PX_BME280_REG_PRESS_MSB, i2c_data, 3))
    {
        // Error
        PX_LOG_E("Unable to read press");
        return false;
    }
    PX_LOG_D("Press data: %02X %02X %02X", i2c_data[0], i2c_data[1], i2c_data[2]);
    // Assemble value
    *data = (int32_t)(   (((uint32_t)i2c_data[0]) << 12)
                       | (((uint32_t)i2c_data[1]) <<  4)
                       | (((uint32_t)i2c_data[2]) >>  4) );

    return true;
}

static bool px_bme280_hum_raw_rd(int32_t * data)
{
    uint8_t i2c_data[2];

    if(!px_bme280_reg_rd_data(PX_BME280_REG_HUM_MSB, i2c_data, 2))
    {
        // Error
        PX_LOG_E("Unable to read humidity");
        return false;
    }
    PX_LOG_D("Hum data: %02X %02X", i2c_data[0], i2c_data[1]);
    // Assemble value
    *data = (int32_t)(   (((uint32_t)i2c_data[0]) << 8)
                       | (((uint32_t)i2c_data[1]) << 0) );

    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_bme280_init(px_i2c_handle_t * handle)
{
    uint8_t data;

    // Save I2C slave handle
    px_bme280_i2c_handle = handle;
    // Verify Chip ID
    if(!px_bme280_reg_rd(PX_BME280_REG_ID, &data))
    {
        // Error
        PX_LOG_E("Unable to read Chip ID");
        return false;
    }
    if(data != PX_BME280_REG_ID_VAL)
    {
        // Error
        PX_LOG_E("Chip ID does not match. Received %02X, but expected %02X",
                   data, PX_BME280_REG_ID_VAL);
        return false;
    }
    // Wait until NVM calibration data is copied to registers
    do
    {
        if(!px_bme280_reg_rd(PX_BME280_REG_STATUS, &data))
        {
            // Error
            PX_LOG_E("Unable to read status");
            return false;
        }
    }
    while(data & PX_BME280_REG_STATUS_IM_UPDATE_MSK);
    // Read calibration
    if(!px_bme280_cal_rd())
    {
        // Error
        return false;
    }
    // Success
    return true;
}

bool px_bme280_rd(px_bme280_data_t * data)
{
    uint8_t ctrl;
    uint8_t status;

    // Set humidity oversampling to 1
    if(!px_bme280_reg_wr(PX_BME280_REG_CTRL_HUM, PX_BME280_REG_CTRL_HUM_OVERS_1))
    {
        // Error
        PX_LOG_E("Unable to set humidity oversampling");
        return false;
    }

    // Start new "forced" measurement (press x 1, temp x 1 oversampled)
    ctrl =   (PX_BME280_REG_CTRL_MEAS_OVERS_1     << PX_BME280_REG_CTRL_MEAS_OSRS_T_POS)
           + (PX_BME280_REG_CTRL_MEAS_OVERS_1     << PX_BME280_REG_CTRL_MEAS_OSRS_P_POS)
           + (PX_BME280_REG_CTRL_MEAS_MODE_FORCED << PX_BME280_REG_CTRL_MEAS_MODE_POS);
    if(!px_bme280_reg_wr(PX_BME280_REG_CTRL_MEAS, ctrl))
    {
        // Error
        PX_LOG_E("Unable to start measurement");
        return false;
    }
    // Wait until conversion is finished
    do
    {
        if(!px_bme280_reg_rd(PX_BME280_REG_STATUS, &status))
        {
            // Error
            PX_LOG_E("Unable to read status");
            return false;
        }
    }
    while(status & PX_BME280_REG_STATUS_MEAS_MSK);
    // Read raw temp value
    if(!px_bme280_temp_raw_rd(&data->temp))
    {
        return false;
    }
    // Read raw press value
    if(!px_bme280_press_raw_rd(&data->press))
    {
        return false;
    }
    // Read raw humidity value
    if(!px_bme280_hum_raw_rd(&data->hum))
    {
        return false;
    }
    // Success
    return true;
}

int32_t px_bme280_temp(px_bme280_data_t * data)
{
    int32_t var1;
    int32_t var2;
    int32_t temp;
    int32_t temp_min = -4000;
    int32_t temp_max = 8500;

    var1 = (int32_t)((data->temp / 8) - ((int32_t)px_bme280_cal.t1 * 2));
    var1 = (var1 * ((int32_t)px_bme280_cal.t2)) / 2048;
    var2 = (int32_t)((data->temp / 16) - ((int32_t)px_bme280_cal.t1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)px_bme280_cal.t3)) / 16384;
    data->t_fine = var1 + var2;
    temp = (data->t_fine * 5 + 128) / 256;
    if(temp < temp_min)
    {
        temp = temp_min;
    }
    else if(temp > temp_max)
    {
        temp = temp_max;
    }

    return temp;
}

uint32_t px_bme280_press(px_bme280_data_t * data)
{
    int32_t  var1;
    int32_t  var2;
    int32_t  var3;
    int32_t  var4;
    uint32_t var5;
    uint32_t press;
    uint32_t press_min = 30000;
    uint32_t press_max = 110000;

    var1 = (((int32_t)data->t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)px_bme280_cal.p6);
    var2 = var2 + ((var1 * ((int32_t)px_bme280_cal.p5)) * 2);
    var2 = (var2 / 4) + (((int32_t)px_bme280_cal.p4) * 65536);
    var3 = (px_bme280_cal.p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)px_bme280_cal.p2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)px_bme280_cal.p1)) / 32768;

    /* avoid exception caused by division by zero */
    if(var1)
    {
        var5 = (uint32_t)((uint32_t)1048576) - data->press;
        press = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
        if(press < 0x80000000)
        {
            press = (press << 1) / ((uint32_t)var1);
        }
        else
        {
            press = (press / (uint32_t)var1) * 2;
        }
        var1 = (((int32_t)px_bme280_cal.p9) * ((int32_t)(((press / 8) * (press / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(press / 4)) * ((int32_t)px_bme280_cal.p8)) / 8192;
        press = (uint32_t)((int32_t)press + ((var1 + var2 + px_bme280_cal.p7) / 16));
        if(press < press_min)
        {
            press = press_min;
        }
        else if(press > press_max)
        {
            press = press_max;
        }
    }
    else
    {
        press = press_min;
    }

    return press;
}

uint32_t px_bme280_hum(px_bme280_data_t * data)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t hum;
    uint32_t hum_max = 102400;

    var1 = data->t_fine - ((int32_t)76800);
    var2 = (int32_t)(data->hum * 16384);
    var3 = (int32_t)(((int32_t)px_bme280_cal.h4) * 1048576);
    var4 = ((int32_t)px_bme280_cal.h5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)px_bme280_cal.h6)) / 1024;
    var3 = (var1 * ((int32_t)px_bme280_cal.h3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)px_bme280_cal.h2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)px_bme280_cal.h1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    hum = (uint32_t)(var5 / 4096);
    if(hum > hum_max)
    {
        hum = hum_max;
    }

    return hum;
}
