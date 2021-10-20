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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_bmp280.h"
#include "px_i2c.h"
#include "px_board.h"
#include "px_log.h"

#ifdef PX_BMP280_DBG
#warning "Debugging of BMP280 enabled. Using constants to verify formula"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("bmp280");

/// @name Memory Map (table 18, p. 24)
/// @{
#define PX_BMP280_REG_CALIB00                   0x88
#define PX_BMP280_REG_CALIB25                   0xa1
#define PX_BMP280_REG_ID                        0xd0
#define PX_BMP280_REG_RESET                     0xe0
#define PX_BMP280_REG_STATUS                    0xf3
#define PX_BMP280_REG_CTRL_MEAS                 0xf4
#define PX_BMP280_REG_CONFIG                    0xf5
#define PX_BMP280_REG_PRESS_MSB                 0xf7
#define PX_BMP280_REG_PRESS_LSB                 0xf8
#define PX_BMP280_REG_PRESS_XLSB                0xf9
#define PX_BMP280_REG_TEMP_MSB                  0xfa
#define PX_BMP280_REG_TEMP_LSB                  0xfb
#define PX_BMP280_REG_TEMP_XLSB                 0xfc
/// @}

// 4.3.1 Register 0xD0 "id"
#define PX_BMP280_REG_ID_VAL                    0x58

// 4.3.2 Register 0xE0 "reset"
#define PX_BMP280_REG_RESET_VAL                 0xb6

// 4.3.3 Register 0xF3 "status"
#define PX_BMP280_REG_STATUS_MEAS_POS           3
#define PX_BMP280_REG_STATUS_MEAS_MSK           (1<<3)
#define PX_BMP280_REG_STATUS_IM_UPDATE_POS      0
#define PX_BMP280_REG_STATUS_IM_UPDATE_MSK      (1<<0)

// 4.3.4 Register 0xF4 "ctrl_meas"
#define PX_BMP280_REG_CTRL_MEAS_OSRS_T_POS      5
#define PX_BMP280_REG_CTRL_MEAS_OSRS_T_MSK      (7<<5)
#define PX_BMP280_REG_CTRL_MEAS_OSRS_P_POS      2
#define PX_BMP280_REG_CTRL_MEAS_OSRS_P_MSK      (7<<2)
#define PX_BMP280_REG_CTRL_MEAS_MODE_POS        0
#define PX_BMP280_REG_CTRL_MEAS_MODE_MSK        (3<<0)
#define PX_BMP280_REG_CTRL_MEAS_OVERS_SKIPPED   0
#define PX_BMP280_REG_CTRL_MEAS_OVERS_1         1
#define PX_BMP280_REG_CTRL_MEAS_OVERS_2         2
#define PX_BMP280_REG_CTRL_MEAS_OVERS_4         3
#define PX_BMP280_REG_CTRL_MEAS_OVERS_8         4
#define PX_BMP280_REG_CTRL_MEAS_OVERS_16        5
#define PX_BMP280_REG_CTRL_MEAS_MODE_SLEEP      0
#define PX_BMP280_REG_CTRL_MEAS_MODE_FORCED     1
#define PX_BMP280_REG_CTRL_MEAS_MODE_NORMAL     3

// 4.3.5 Register 0xF5 "config"
#define PX_BMP280_REG_CONFIG_T_SB_POS           5
#define PX_BMP280_REG_CONFIG_T_SB_MSK           (7<<5)
#define PX_BMP280_REG_CONFIG_FILTER_POS         2
#define PX_BMP280_REG_CONFIG_FILTER_MSK         (7<<2)
#define PX_BMP280_REG_CONFIG_SPI3W_EN_POS       0
#define PX_BMP280_REG_CONFIG_SPI3W_EN_MSK       (1<<0)

// 4.3.6 Register 0xF7..0xF9 "press" (_msb, _lsb, _xlsb)
#define PX_BMP280_PRESS_XLSB_POS                4
#define PX_BMP280_PRESS_XLSB_MSK                (15<<4)

// 4.3.7 Register 0xFA..0xFC "temp" (_msb, _lsb, _xlsb)
#define PX_BMP280_TEMP_XLSB_POS                 4
#define PX_BMP280_TEMP_XLSB_MSK                 (15<<4)

/// BMP280 calibration coefficients
typedef struct
{
    uint16_t t1;
    int16_t  t2;
    int16_t  t3;
    uint16_t p1;
    int16_t  p2;
    int16_t  p3;
    int16_t  p4;
    int16_t  p5;
    int16_t  p6;
    int16_t  p7;
    int16_t  p8;
    int16_t  p9;
} px_bmp280_cal_t;

/// Number of calibration coefficients
#define PX_BMP280_NR_OF_CALS    12

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_bmp280_i2c_handle;

static union
{
    px_bmp280_cal_t dig;
    uint16_t        data[PX_BMP280_NR_OF_CALS];
} px_bmp280_cal;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_bmp280_reg_wr(uint8_t adr, uint8_t data)
{
    uint8_t i2c_data[2];

    i2c_data[0] = adr;
    i2c_data[1] = data;

    if(!px_i2c_wr(px_bmp280_i2c_handle, i2c_data, 2, PX_I2C_FLAG_START_AND_STOP))
    {
        // Error
        PX_LOG_E("Unable to write register value");
        return false;
    }
    // Success
    return true;
}

static bool px_bmp280_reg_rd(uint8_t adr, uint8_t * data)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_bmp280_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_bmp280_i2c_handle,
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

static bool px_bmp280_reg_rd_data(uint8_t adr, void * data, uint8_t nr_of_bytes)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_bmp280_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_bmp280_i2c_handle,
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

static bool px_bmp280_cal_rd(void)
{
    uint8_t  i;
    uint8_t  adr;
    uint8_t  i2c_data[2];
    uint16_t val;

    // Start at first calibration coefficient
    adr = PX_BMP280_REG_CALIB00;

    for(i = 0; i < PX_BMP280_NR_OF_CALS; i++)
    {
        // Read 16-bit value (Least Significant Byte first)
        if(!px_bmp280_reg_rd_data(adr, i2c_data, 2))
        {
            // Error
            PX_LOG_E("Unable to read calibration value");
            return false;
        }
        PX_LOG_D("Adr %02X: %02X %02X", adr, i2c_data[0], i2c_data[1]);
        // Assemble value
        val = PX_U16_CONCAT_U8(i2c_data[1], i2c_data[0]);
        // Store value
        px_bmp280_cal.data[i] = val;
        // Next calibration coefficient
        adr += 2;
    }

#ifdef PX_BMP280_DBG
    // Set calibration values to excel table on page 23 to verify formula
    px_bmp280_cal.dig.t1 = 27504;
    px_bmp280_cal.dig.t2 = 26435;
    px_bmp280_cal.dig.t3 = -1000;
    px_bmp280_cal.dig.p1 = 36477;
    px_bmp280_cal.dig.p2 = -10685;
    px_bmp280_cal.dig.p3 = 3024;
    px_bmp280_cal.dig.p4 = 2855;
    px_bmp280_cal.dig.p5 = 140;
    px_bmp280_cal.dig.p6 = -7;
    px_bmp280_cal.dig.p7 = 15500;
    px_bmp280_cal.dig.p8 = -14600;
    px_bmp280_cal.dig.p9 = 6000;
#endif

    if(PX_LOG_LEVEL_IS_D())
    {
        // Report calibration values
        PX_LOG_D("BMP280 Cal:");
        PX_LOG_TRACE("T1 = %d\n", px_bmp280_cal.dig.t1);
        PX_LOG_TRACE("T2 = %d\n", px_bmp280_cal.dig.t2);
        PX_LOG_TRACE("T3 = %u\n", px_bmp280_cal.dig.t3);
        PX_LOG_TRACE("P1 = %d\n", px_bmp280_cal.dig.p1);
        PX_LOG_TRACE("P2 = %d\n", px_bmp280_cal.dig.p2);
        PX_LOG_TRACE("P3 = %d\n", px_bmp280_cal.dig.p3);
        PX_LOG_TRACE("P4 = %d\n", px_bmp280_cal.dig.p4);
        PX_LOG_TRACE("P5 = %d\n", px_bmp280_cal.dig.p5);
        PX_LOG_TRACE("P6 = %d\n", px_bmp280_cal.dig.p6);
        PX_LOG_TRACE("P7 = %d\n", px_bmp280_cal.dig.p7);
        PX_LOG_TRACE("P8 = %d\n", px_bmp280_cal.dig.p8);
        PX_LOG_TRACE("P9 = %d\n", px_bmp280_cal.dig.p9);
    }

    return true;
}

static bool px_bmp280_temp_raw_rd(int32_t * data)
{
    uint8_t i2c_data[3];

    if(!px_bmp280_reg_rd_data(PX_BMP280_REG_TEMP_MSB, i2c_data, 3))
    {
        // Error
        PX_LOG_E("Unable to read temp");
        return false;
    }
    PX_LOG_D("Temp data: %02X %02X %02X", i2c_data[0], i2c_data[1], i2c_data[2]);

    // Assemble value
    *data = (int32_t)(   (((uint32_t)i2c_data[0]) << 12)
                       | (((uint32_t)i2c_data[1]) <<  4)
                       | (((uint32_t)i2c_data[2]) >>  4)  );

#ifdef PX_BMP280_DBG
    *data = 519888;
#endif

    PX_LOG_D("Raw Temp = %ld", *data);

    return true;
}

static bool px_bmp280_press_raw_rd(int32_t * data)
{
    uint8_t i2c_data[3];

    if(!px_bmp280_reg_rd_data(PX_BMP280_REG_PRESS_MSB, i2c_data, 3))
    {
        // Error
        PX_LOG_E("Unable to read temp");
        return false;
    }
    PX_LOG_D("Press data: %02X %02X %02X", i2c_data[0], i2c_data[1], i2c_data[2]);

    // Assemble value
    *data = (int32_t)(   (((uint32_t)i2c_data[0]) << 12)
                       | (((uint32_t)i2c_data[1]) <<  4)
                       | (((uint32_t)i2c_data[2]) >>  4)  );

#ifdef PX_BMP280_DBG
    *data = 415148;
#endif

    PX_LOG_D("Raw Press = %ld", *data);

    return true;
}

/**
 *  Calculate compensated temp in 0.01 deg C steps, e.g.
 *  "5123" equals 51.23 deg C
 *  
 *  @param ut[in]       Uncompensated Temperature
 *  @param t_fine[out]  Fine temperature used to calculate compensated pressure
 *  
 *  @return int32_t     Compensated Temperature
 */
int32_t px_bmp280_comp_temp(int32_t ut, int32_t * t_fine)
{
    int32_t var1;
    int32_t var2;
    int32_t temp;

    var1 = (   (((ut >> 3) - ((int32_t)px_bmp280_cal.dig.t1 << 1)))
             * ((int32_t)px_bmp280_cal.dig.t2)) >> 11;
    var2 = (   ((   ((ut >> 4) - ((int32_t)px_bmp280_cal.dig.t1))
                  * ((ut >> 4) - ((int32_t)px_bmp280_cal.dig.t1))) >> 12)
             * ((int32_t)px_bmp280_cal.dig.t3)) >> 14;

    *t_fine = var1 + var2;
    temp    = (*t_fine * 5 + 128) >> 8;

    PX_LOG_D("Compensated Temp = %ld", temp);

    return temp;
}

/**
 *  Calculate compensated pressure in Pa as unsigned 32 bit integer, e.g.
 *  "96386" equals 96386 Pa = 963.86 hPa
 *  
 *  @param up[in]       Uncompensated Pressure
 *  @param t_fine[in]   Fine temperature returned from compensated temperature
 *                      calculation
 *  
 *  @return uint32_t    Comnpensated Pressure
 */
uint32_t px_bmp280_comp_press(int32_t up, int32_t t_fine)
{
    int32_t  var1;
    int32_t  var2;
    uint32_t press;

    var1  = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2  = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)px_bmp280_cal.dig.p6);
    var2  = var2 + ((var1 * ((int32_t)px_bmp280_cal.dig.p5)) << 1);
    var2  = (var2 >> 2) + (((int32_t)px_bmp280_cal.dig.p4) << 16);
    var1  = (   ((px_bmp280_cal.dig.p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
              + ((((int32_t)px_bmp280_cal.dig.p2) * var1) >> 1)) >> 18;
    var1  = (((32768 + var1)) * ((int32_t)px_bmp280_cal.dig.p1)) >> 15;
    press = (((uint32_t)(((int32_t)1048576) - up) - (var2 >> 12))) * 3125;
    
    // Avoid divide by zero
    if(var1 == 0)
    {
        return 0;
    }

    // Avoid overflow
    if (press < 0x80000000)
    {
        press = (press << 1) / ((uint32_t)var1);
    }
    else
    {
        press = (press / (uint32_t)var1) * 2;
    }

    var1  = (   ((int32_t)px_bmp280_cal.dig.p9)
              * ((int32_t)(((press >> 3) * (press >> 3)) >> 13))  ) >> 12;
    var2  = (((int32_t)(press >> 2)) * ((int32_t)px_bmp280_cal.dig.p8)) >> 13;
    press = (uint32_t)((int32_t)press + ((var1 + var2 + px_bmp280_cal.dig.p7) >> 4));

    PX_LOG_D("Compensated Press = %ld", press);

    return press;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_bmp280_init(px_i2c_handle_t * handle)
{
    uint8_t data;

    // Save I2C slave handle
    px_bmp280_i2c_handle = handle;

    // Verify Chip ID
    if(!px_bmp280_reg_rd(PX_BMP280_REG_ID, &data))
    {
        // Error
        PX_LOG_E("Unable to read Chip ID");
        return false;
    }
    if(data != PX_BMP280_REG_ID_VAL)
    {
        // Error
        PX_LOG_E("Chip ID does not match. Received %02X, but expected %02X",
                 data, PX_BMP280_REG_ID_VAL);
        return false;
    }

    // Wait until NVM calibration data is copied to registers
    do
    {
        if(!px_bmp280_reg_rd(PX_BMP280_REG_STATUS, &data))
        {
            // Error
            PX_LOG_E("Unable to read status");
            return false;
        }
    }
    while(data & PX_BMP280_REG_STATUS_IM_UPDATE_MSK);

    // Read calibration
    if(!px_bmp280_cal_rd())
    {
        // Error
        return false;
    }

    // Success
    return true;
}

bool px_bmp280_read(int32_t * temperature, int32_t * pressure)
{
    uint8_t data;
    int32_t t_fine;

    // Read calibration
    if(!px_bmp280_cal_rd())
    {
        // Error
        return false;
    }

    // Start new "forced" measurement (pressure x 8, temperature x 1 oversampled)
    data =   (PX_BMP280_REG_CTRL_MEAS_OVERS_1     << PX_BMP280_REG_CTRL_MEAS_OSRS_T_POS)
           + (PX_BMP280_REG_CTRL_MEAS_OVERS_8     << PX_BMP280_REG_CTRL_MEAS_OSRS_P_POS)
           + (PX_BMP280_REG_CTRL_MEAS_MODE_FORCED << PX_BMP280_REG_CTRL_MEAS_MODE_POS);
    if(!px_bmp280_reg_wr(PX_BMP280_REG_CTRL_MEAS, data))
    {
        // Error
        PX_LOG_E("Unable to start measurement");
        return false;
    }

    // Wait until conversion is finished
    do
    {
        if(!px_bmp280_reg_rd(PX_BMP280_REG_STATUS, &data))
        {
            // Error
            PX_LOG_E("Unable to read status");
            return false;
        }
    }
    while(data & PX_BMP280_REG_STATUS_MEAS_MSK);

    // Read raw temperature value
    if(!px_bmp280_temp_raw_rd(temperature))
    {
        return false;
    }

    // Read raw pressure value
    if(!px_bmp280_press_raw_rd(pressure))
    {
        return false;
    }

    // Calculated compensated temperature value
    *temperature = px_bmp280_comp_temp(*temperature, &t_fine);

    // Calculated compensated pressure value
    *pressure = px_bmp280_comp_press(*pressure, t_fine);
    
    // Success
    return true;
}
