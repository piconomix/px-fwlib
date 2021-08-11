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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_lis3dh.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_lis3dh");

/// MSBit of Sub Address must be set so that it is auto incremented when multiple bytes are read
#define PX_LIS3DH_SUB_ADR_AUTO_INC                  (1 << 7)

/// @name Registers (table 21; p. 31)
//@{
#define PX_LIS3DH_REG_STATUS_REG_AUX                0x07

#define PX_LIS3DH_REG_OUT_ADC1_L                    0x08
#define PX_LIS3DH_REG_OUT_ADC1_H                    0x09
#define PX_LIS3DH_REG_OUT_ADC2_L                    0x0a
#define PX_LIS3DH_REG_OUT_ADC2_H                    0x0b
#define PX_LIS3DH_REG_OUT_ADC3_L                    0x0c
#define PX_LIS3DH_REG_OUT_ADC3_H                    0x0d

#define PX_LIS3DH_REG_WHO_AM_I                      0x0f

#define PX_LIS3DH_REG_CTRL_REG0                     0x1e
#define PX_LIS3DH_REG_TEMP_CFG_REG                  0x1f
#define PX_LIS3DH_REG_CTRL_REG1                     0x20
#define PX_LIS3DH_REG_CTRL_REG2                     0x21
#define PX_LIS3DH_REG_CTRL_REG3                     0x22
#define PX_LIS3DH_REG_CTRL_REG4                     0x23
#define PX_LIS3DH_REG_CTRL_REG5                     0x24
#define PX_LIS3DH_REG_CTRL_REG6                     0x25
#define PX_LIS3DH_REG_REFERENCE                     0x26
#define PX_LIS3DH_REG_STATUS_REG                    0x27

#define PX_LIS3DH_REG_OUT_X_L                       0x28
#define PX_LIS3DH_REG_OUT_X_H                       0x29
#define PX_LIS3DH_REG_OUT_Y_L                       0x2a
#define PX_LIS3DH_REG_OUT_Y_H                       0x2b
#define PX_LIS3DH_REG_OUT_Z_L                       0x2c
#define PX_LIS3DH_REG_OUT_Z_H                       0x2d
#define PX_LIS3DH_REG_FIFO_CTRL_REG                 0x2e
#define PX_LIS3DH_REG_FIFO_SRC_REG                  0x2f

#define PX_LIS3DH_REG_INT1_CFG                      0x30
#define PX_LIS3DH_REG_INT1_SRC                      0x31
#define PX_LIS3DH_REG_INT1_THS                      0x32
#define PX_LIS3DH_REG_INT1_DURATION                 0x33
#define PX_LIS3DH_REG_INT2_CFG                      0x34
#define PX_LIS3DH_REG_INT2_SRC                      0x35
#define PX_LIS3DH_REG_INT2_THS                      0x36
#define PX_LIS3DH_REG_INT2_DURATION                 0x37
#define PX_LIS3DH_REG_CLICK_CFG                     0x38
#define PX_LIS3DH_REG_CLICK_SRC                     0x39
#define PX_LIS3DH_REG_CLICK_THS                     0x3a
#define PX_LIS3DH_REG_TIME_LIMIT                    0x3b
#define PX_LIS3DH_REG_TIME_LATENCY                  0x3c
#define PX_LIS3DH_REG_TIME_WINDOW                   0x3d
#define PX_LIS3DH_REG_ACT_THS                       0x3e
#define PX_LIS3DH_REG_ACT_DUR                       0x3f
//@}

/// @name STATUS_REG_AUX register bits
//@{
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_321OR     7
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_3OR       6
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_2OR       5
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_1OR       4
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_321DA     3
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_3DA       2
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_2DA       1
#define PX_LIS3DH_REG_CTRL_STATUS_REG_AUX_1DA       0
//@}

/// Device identification register value
#define PX_LIS3DH_REG_WHO_AM_I_VAL                  0x33

/// @name CTRL_REG0 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG0_SDO_PU_DISC         7
#define PX_LIS3DH_REG_CTRL_REG0_SDO_PU_DIS          (1 << PX_LIS3DH_REG_CTRL_REG0_SDO_PU_DISC)
#define PX_LIS3DH_REG_CTRL_REG0_SDO_PU_EN           (0 << PX_LIS3DH_REG_CTRL_REG0_SDO_PU_DISC)
//@}

/// @name TEMP_CFG_REG register bits
//@{
#define PX_LIS3DH_REG_CTRL_TEMP_CFG_REG_ADC_EN      7
#define PX_LIS3DH_REG_CTRL_TEMP_CFG_REG_TEMP_EN     6
//@}

/// @name CTRL_REG1 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG1_ODR3                7
#define PX_LIS3DH_REG_CTRL_REG1_ODR2                6
#define PX_LIS3DH_REG_CTRL_REG1_ODR1                5
#define PX_LIS3DH_REG_CTRL_REG1_ODR0                4
#define PX_LIS3DH_REG_CTRL_REG1_LPEN                3
#define PX_LIS3DH_REG_CTRL_REG1_ZEN                 2
#define PX_LIS3DH_REG_CTRL_REG1_YEN                 1
#define PX_LIS3DH_REG_CTRL_REG1_XEN                 0

#define PX_LIS3DH_REG_CTRL_REG1_ODR_PD              (0 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_1HZ             (1 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_10HZ            (2 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_25HZ            (3 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_50HZ            (4 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_100HZ           (5 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_200HZ           (6 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_400HZ           (7 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_1620HZ          (8 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_ODR_1344HZ          (9 << PX_LIS3DH_REG_CTRL_REG1_ODR0)
#define PX_LIS3DH_REG_CTRL_REG1_LPEN_DIS            (0 << PX_LIS3DH_REG_CTRL_REG1_LPEN)
#define PX_LIS3DH_REG_CTRL_REG1_LPEN_EN             (1 << PX_LIS3DH_REG_CTRL_REG1_LPEN)
#define PX_LIS3DH_REG_CTRL_REG1_ZEN_DIS             (0 << PX_LIS3DH_REG_CTRL_REG1_ZEN)
#define PX_LIS3DH_REG_CTRL_REG1_ZEN_EN              (1 << PX_LIS3DH_REG_CTRL_REG1_ZEN)
#define PX_LIS3DH_REG_CTRL_REG1_YEN_DIS             (0 << PX_LIS3DH_REG_CTRL_REG1_YEN)
#define PX_LIS3DH_REG_CTRL_REG1_YEN_EN              (1 << PX_LIS3DH_REG_CTRL_REG1_YEN)
#define PX_LIS3DH_REG_CTRL_REG1_XEN_DIS             (0 << PX_LIS3DH_REG_CTRL_REG1_XEN)
#define PX_LIS3DH_REG_CTRL_REG1_XEN_EN              (1 << PX_LIS3DH_REG_CTRL_REG1_XEN)
//@}

/// @name CTRL_REG2 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG2_HPM1                7
#define PX_LIS3DH_REG_CTRL_REG2_HPM0                6
#define PX_LIS3DH_REG_CTRL_REG2_HPCF2               5
#define PX_LIS3DH_REG_CTRL_REG2_HPCF1               4
#define PX_LIS3DH_REG_CTRL_REG2_FDS                 3
#define PX_LIS3DH_REG_CTRL_REG2_HPCLICK             2
#define PX_LIS3DH_REG_CTRL_REG2_HP_IA2              1
#define PX_LIS3DH_REG_CTRL_REG2_HP_IA1              0
//@}

/// @name CTRL_REG3 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG3_I1_CLICK            7
#define PX_LIS3DH_REG_CTRL_REG3_I1_IA1              6
#define PX_LIS3DH_REG_CTRL_REG3_I1_IA2              5
#define PX_LIS3DH_REG_CTRL_REG3_I1_ZYXDA            4
#define PX_LIS3DH_REG_CTRL_REG3_I1_321DA            3
#define PX_LIS3DH_REG_CTRL_REG3_I1_WTM              2
#define PX_LIS3DH_REG_CTRL_REG3_I1_OVERRUN          1
//@}

/// @name CTRL_REG4 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG4_BDU                 7
#define PX_LIS3DH_REG_CTRL_REG4_BLE                 6
#define PX_LIS3DH_REG_CTRL_REG4_FS1                 5
#define PX_LIS3DH_REG_CTRL_REG4_FS0                 4
#define PX_LIS3DH_REG_CTRL_REG4_HR                  3
#define PX_LIS3DH_REG_CTRL_REG4_ST1                 2
#define PX_LIS3DH_REG_CTRL_REG4_ST0                 1
#define PX_LIS3DH_REG_CTRL_REG4_SIM                 0
//@}

/// @name CTRL_REG5 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG5_BOOT                7
#define PX_LIS3DH_REG_CTRL_REG5_FIFO_EN             6
#define PX_LIS3DH_REG_CTRL_REG5_LIR_INT1            3
#define PX_LIS3DH_REG_CTRL_REG5_D4D_INT1            2
#define PX_LIS3DH_REG_CTRL_REG5_LIR_INT2            1
#define PX_LIS3DH_REG_CTRL_REG5_D4D_INT2            0
//@}

/// @name CTRL_REG6 register bits
//@{
#define PX_LIS3DH_REG_CTRL_REG6_I2_CLICK            7
#define PX_LIS3DH_REG_CTRL_REG6_I2_IA1              6
#define PX_LIS3DH_REG_CTRL_REG6_I2_IA2              5
#define PX_LIS3DH_REG_CTRL_REG6_I2_BOOT             4
#define PX_LIS3DH_REG_CTRL_REG6_I2_ACT              3
#define PX_LIS3DH_REG_CTRL_REG6_INT_POLARITY        1
//@}

/// @name STATUS_REG register bits
//@{
#define PX_LIS3DH_REG_CTRL_STATUS_REG_ZYXOR         7
#define PX_LIS3DH_REG_CTRL_STATUS_REG_ZOR           6
#define PX_LIS3DH_REG_CTRL_STATUS_REG_YOR           5
#define PX_LIS3DH_REG_CTRL_STATUS_REG_XOR           4
#define PX_LIS3DH_REG_CTRL_STATUS_REG_ZYXDA         3
#define PX_LIS3DH_REG_CTRL_STATUS_REG_ZDA           2
#define PX_LIS3DH_REG_CTRL_STATUS_REG_YDA           1
#define PX_LIS3DH_REG_CTRL_STATUS_REG_XDA           0
//@}

/// @name FIFO_CTRL_REG register bits
//@{
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FM1        7
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FM0        6
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_TR         5
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FTH4       4
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FTH3       3
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FTH2       2
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FTH1       1
#define PX_LIS3DH_REG_CTRL_FIFO_CTRL_REG_FTH0       0
//@}

/// @name FIFO_SRC_REG register bits
//@{
#define PX_LIS3DH_REG_FIFO_SRC_REG_WTM              7
#define PX_LIS3DH_REG_FIFO_SRC_REG_OVRN_FIFO        6
#define PX_LIS3DH_REG_FIFO_SRC_REG_EMPTY            5
#define PX_LIS3DH_REG_FIFO_SRC_REG_FSS4             4
#define PX_LIS3DH_REG_FIFO_SRC_REG_FSS3             3
#define PX_LIS3DH_REG_FIFO_SRC_REG_FSS2             2
#define PX_LIS3DH_REG_FIFO_SRC_REG_FSS1             1
#define PX_LIS3DH_REG_FIFO_SRC_REG_FSS0             0
//@}

/// @name INT1_CFG register bits
//@{
#define PX_LIS3DH_REG_INT1_CFG_AOI                  7
#define PX_LIS3DH_REG_INT1_CFG_6D                   6
#define PX_LIS3DH_REG_INT1_CFG_ZHIE                 5
#define PX_LIS3DH_REG_INT1_CFG_ZLIE                 4
#define PX_LIS3DH_REG_INT1_CFG_YHIE                 3
#define PX_LIS3DH_REG_INT1_CFG_YLIE                 2
#define PX_LIS3DH_REG_INT1_CFG_XHIE                 1
#define PX_LIS3DH_REG_INT1_CFG_XLIE                 0
//@}

/// @name INT1_SRC register bits
//@{
#define PX_LIS3DH_REG_INT1_SRC_IA                   6
#define PX_LIS3DH_REG_INT1_SRC_ZH                   5
#define PX_LIS3DH_REG_INT1_SRC_ZL                   4
#define PX_LIS3DH_REG_INT1_SRC_YH                   3
#define PX_LIS3DH_REG_INT1_SRC_YL                   2
#define PX_LIS3DH_REG_INT1_SRC_XH                   1
#define PX_LIS3DH_REG_INT1_SRC_XL                   0
//@}

/// @name INT2_CFG register bits
//@{
#define PX_LIS3DH_REG_INT2_CFG_AOI                  7
#define PX_LIS3DH_REG_INT2_CFG_6D                   6
#define PX_LIS3DH_REG_INT2_CFG_ZHIE                 5
#define PX_LIS3DH_REG_INT2_CFG_ZLIE                 4
#define PX_LIS3DH_REG_INT2_CFG_YHIE                 3
#define PX_LIS3DH_REG_INT2_CFG_YLIE                 2
#define PX_LIS3DH_REG_INT2_CFG_XHIE                 1
#define PX_LIS3DH_REG_INT2_CFG_XLIE                 0
//@}

/// @name INT2_SRC register bits
//@{
#define PX_LIS3DH_REG_INT2_SRC_IA                   6
#define PX_LIS3DH_REG_INT2_SRC_ZH                   5
#define PX_LIS3DH_REG_INT2_SRC_ZL                   4
#define PX_LIS3DH_REG_INT2_SRC_YH                   3
#define PX_LIS3DH_REG_INT2_SRC_YL                   2
#define PX_LIS3DH_REG_INT2_SRC_XH                   1
#define PX_LIS3DH_REG_INT2_SRC_XL                   0
//@}

/// @name CLICK_CFG register bits
//@{
#define PX_LIS3DH_REG_CLICK_CFG_ZD                  5
#define PX_LIS3DH_REG_CLICK_CFG_ZS                  4
#define PX_LIS3DH_REG_CLICK_CFG_YD                  3
#define PX_LIS3DH_REG_CLICK_CFG_YS                  2
#define PX_LIS3DH_REG_CLICK_CFG_XD                  1
#define PX_LIS3DH_REG_CLICK_CFG_XS                  0
//@}

/// @name CLICK_SRC register bits
//@{
#define PX_LIS3DH_REG_CLICK_SRC_IA                  6
#define PX_LIS3DH_REG_CLICK_SRC_DCLICK              5
#define PX_LIS3DH_REG_CLICK_SRC_SCLICK              4
#define PX_LIS3DH_REG_CLICK_SRC_SIGN                3
#define PX_LIS3DH_REG_CLICK_SRC_Z                   2
#define PX_LIS3DH_REG_CLICK_SRC_Y                   1
#define PX_LIS3DH_REG_CLICK_SRC_X                   0
//@}

/// @name CLICK_THS register bits
//@{
#define PX_LIS3DH_REG_CLICK_THS_LIR_CLICK           7
#define PX_LIS3DH_REG_CLICK_THS_THS6                6
#define PX_LIS3DH_REG_CLICK_THS_THS5                5
#define PX_LIS3DH_REG_CLICK_THS_THS4                4
#define PX_LIS3DH_REG_CLICK_THS_THS3                3
#define PX_LIS3DH_REG_CLICK_THS_THS2                2
#define PX_LIS3DH_REG_CLICK_THS_THS1                1
#define PX_LIS3DH_REG_CLICK_THS_THS0                0
//@}

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_lis3dh_i2c_handle;
static bool              px_lis3dh_test_active_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_lis3dh_reg_wr_u8(uint8_t adr, uint8_t data)
{
    uint8_t i2c_data[2];

    i2c_data[0] = adr;
    i2c_data[1] = data;

    if(!px_i2c_wr(px_lis3dh_i2c_handle, i2c_data, 2, PX_I2C_FLAG_START_AND_STOP))
    {
        // Error
        PX_LOG_E("Unable to write register value");
        return false;
    }
    // Success
    return true;
}

static bool px_lis3dh_reg_rd_u8(uint8_t adr, uint8_t * data)
{
    uint8_t i2c_data[1];

    i2c_data[0] = adr;

    // Write address
    if(!px_i2c_wr(px_lis3dh_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_lis3dh_i2c_handle,
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

static bool px_lis3dh_reg_rd_data(uint8_t adr, uint8_t * data, size_t nr_of_bytes)
{
    uint8_t i2c_data[1];

    // Set MSB of address so that it is auto incremented after each byte is read
    i2c_data[0] = adr | PX_LIS3DH_SUB_ADR_AUTO_INC;
    // Write address
    if(!px_i2c_wr(px_lis3dh_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_END))
    {
        // Error
        PX_LOG_E("Unable to write register address");
        return false;
    }
    // Read data
    if(!px_i2c_rd(px_lis3dh_i2c_handle,
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

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_lis3dh_init(px_i2c_handle_t * handle)
{
    // Save I2C handle
    px_lis3dh_i2c_handle = handle;
    // Disable pull-up on SDO/SA0 to save power
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG0,
                          PX_LIS3DH_REG_CTRL_REG0_SDO_PU_DIS | 0x10) == false)
    {
        return false;
    }
    // Put accelerometer in power down mode
    if(px_lis3dh_reg_wr_u8(  PX_LIS3DH_REG_CTRL_REG1,
                             PX_LIS3DH_REG_CTRL_REG1_ODR_PD
                           | PX_LIS3DH_REG_CTRL_REG1_LPEN_EN
                           | PX_LIS3DH_REG_CTRL_REG1_ZEN_DIS
                           | PX_LIS3DH_REG_CTRL_REG1_YEN_DIS
                           | PX_LIS3DH_REG_CTRL_REG1_XEN_DIS) == false)
    {
        return false;
    }
    // Success
    return true;
}

bool px_lis3dh_enable(void)
{
    uint8_t data;

    // Enable Accelerometer X,Y & Z measurement @ 1 Hz (normal mode for 10-bit data output)
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG1,
                             PX_LIS3DH_REG_CTRL_REG1_ODR_1HZ
                           | PX_LIS3DH_REG_CTRL_REG1_LPEN_DIS
                           | PX_LIS3DH_REG_CTRL_REG1_ZEN_EN
                           | PX_LIS3DH_REG_CTRL_REG1_YEN_EN
                           | PX_LIS3DH_REG_CTRL_REG1_XEN_EN) == false)
    {
        return false;
    }
#if 0
    // Enable high-pass filter
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG2,
                             (0 << PX_LIS3DH_REG_CTRL_REG2_HPM1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG2_HPM0)
                           | (1 << PX_LIS3DH_REG_CTRL_REG2_HPCF2)
                           | (1 << PX_LIS3DH_REG_CTRL_REG2_HPCF1)
                           | (1 << PX_LIS3DH_REG_CTRL_REG2_FDS)
                           | (0 << PX_LIS3DH_REG_CTRL_REG2_HPCLICK)
                           | (0 << PX_LIS3DH_REG_CTRL_REG2_HP_IA2)
                           | (1 << PX_LIS3DH_REG_CTRL_REG2_HP_IA1)) == false)
    {
        return false;
    }
#endif
    // Enable IA1 interrupt on INT1 pin
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG3,
                             (0 << PX_LIS3DH_REG_CTRL_REG3_I1_CLICK)
                           | (1 << PX_LIS3DH_REG_CTRL_REG3_I1_IA1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_IA2)
                           | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_ZYXDA)
                           | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_WTM)
                           | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_OVERRUN)) == false)
    {
        return false;
    }
    // Set scale to +-4g (high-resolution mode disabled) and enable Block Data Update
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG4,
                             (1 << PX_LIS3DH_REG_CTRL_REG4_BDU)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_BLE)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_FS1)
                           | (1 << PX_LIS3DH_REG_CTRL_REG4_FS0)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_HR)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_ST1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_ST0)
                           | (0 << PX_LIS3DH_REG_CTRL_REG4_SIM)) == false)
    {
        return false;
    }
    // Disable latching of INT1 until status is read
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG5,
                             (0 << PX_LIS3DH_REG_CTRL_REG5_BOOT)
                           | (0 << PX_LIS3DH_REG_CTRL_REG5_FIFO_EN)
                           | (0 << PX_LIS3DH_REG_CTRL_REG5_LIR_INT1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG5_D4D_INT1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG5_LIR_INT2)
                           | (0 << PX_LIS3DH_REG_CTRL_REG5_D4D_INT2)) == false)
    {
        return false;
    }
    // Set interrupt polarity to active-high
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG6,
                             (0 << PX_LIS3DH_REG_CTRL_REG6_I2_CLICK)
                           | (0 << PX_LIS3DH_REG_CTRL_REG6_I2_IA1)
                           | (0 << PX_LIS3DH_REG_CTRL_REG6_I2_IA2)
                           | (0 << PX_LIS3DH_REG_CTRL_REG6_I2_BOOT)
                           | (0 << PX_LIS3DH_REG_CTRL_REG6_I2_ACT)
                           | (0 << PX_LIS3DH_REG_CTRL_REG6_INT_POLARITY) ) == false)
    {
        return false;
    }
    // Set threshold to 0.256 g (1 LSB = 32 mg @ FS = 4 g)
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_INT1_THS, 8) == false)
    {
        return false;
    }
    // Set duration to 0.1 s (1/ODR)
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_INT1_DURATION, 0) == false)
    {
        return false;
    }
    // Perform dummy read of REFERENCE to force HP filter to current acceleration value
    if(px_lis3dh_reg_rd_u8(PX_LIS3DH_REG_REFERENCE, &data) == false)
    {
        return false;
    }
    // Enable interrupt on X, Y or Z exceeding threshold
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_INT1_CFG,
                             (0 << PX_LIS3DH_REG_INT1_CFG_AOI)
                           | (0 << PX_LIS3DH_REG_INT1_CFG_6D)
                           | (1 << PX_LIS3DH_REG_INT1_CFG_ZHIE)
                           | (0 << PX_LIS3DH_REG_INT1_CFG_ZLIE)
                           | (1 << PX_LIS3DH_REG_INT1_CFG_YHIE)
                           | (0 << PX_LIS3DH_REG_INT1_CFG_YLIE)
                           | (1 << PX_LIS3DH_REG_INT1_CFG_XHIE)
                           | (0 << PX_LIS3DH_REG_INT1_CFG_XLIE)) == false)
    {
        return false;
    }
    // Success
    return true;
}

bool px_lis3dh_disable(void)
{
    // Put accelerometer in power down mode
    if(px_lis3dh_reg_wr_u8(  PX_LIS3DH_REG_CTRL_REG1,
                             PX_LIS3DH_REG_CTRL_REG1_ODR_PD
                           | PX_LIS3DH_REG_CTRL_REG1_LPEN_EN
                           | PX_LIS3DH_REG_CTRL_REG1_ZEN_DIS
                           | PX_LIS3DH_REG_CTRL_REG1_YEN_DIS
                           | PX_LIS3DH_REG_CTRL_REG1_XEN_DIS) == false)
    {
        return false;
    }
    // Success
    return true;
}

bool px_lis3dh_xyz_data_ready(void)
{
    uint8_t data;

    // Read status register
    if(!px_lis3dh_reg_rd_u8(PX_LIS3DH_REG_STATUS_REG, &data))
    {
        return false;
    }
    if(data & (1 << PX_LIS3DH_REG_CTRL_STATUS_REG_ZYXDA))
    {
        // X,Y & Z data is ready
        PX_LOG_D("Data ready. StatusReg = 0x%02X", (unsigned int)data);
        return true;
    }
    else
    {
        return false;
    }
}

bool px_lis3dh_rd_xyz_data(px_lis3dh_xyz_t * xyz)
{
    uint8_t data[6];

    // Read X,Y & Z 16-bit data registers
    if(!px_lis3dh_reg_rd_data(PX_LIS3DH_REG_OUT_X_L, data, 6))
    {
        return false;
    }
    // Concatenated and return results
    xyz->x = (int16_t)(PX_U16_CONCAT_U8(data[1], data[0]));
    xyz->y = (int16_t)(PX_U16_CONCAT_U8(data[3], data[2]));
    xyz->z = (int16_t)(PX_U16_CONCAT_U8(data[5], data[4]));

    return true;
}

bool px_lis3dh_set_int1_threshold(uint8_t threshold)
{
    return px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_INT1_THS, threshold);
}

bool px_lis3dh_rd_int1_src(uint8_t * data)
{
    return px_lis3dh_reg_rd_u8(PX_LIS3DH_REG_INT1_SRC, data);
}

bool px_lis3dh_rd_int2_src(uint8_t * data)
{
    return px_lis3dh_reg_rd_u8(PX_LIS3DH_REG_INT2_SRC, data);
}

bool px_lis3dh_rd_who_am_i(uint8_t * data)
{
    if(px_lis3dh_reg_rd_u8(PX_LIS3DH_REG_WHO_AM_I, data))
    {
        PX_LOG_D("WHO_AM_I = 0x%02X", (unsigned int)*data);
        return true;
    }
    else
    {
        PX_LOG_E("PX_LIS3DH_REG_WHO_AM_I rd failed");
        return false;
    }
}

bool px_lis3dh_connected(void)
{
    uint8_t data;

    // Check that "Who Am I" register returns expected value
    if(!px_lis3dh_rd_who_am_i(&data))
    {
        return false;
    }
    if(data != PX_LIS3DH_REG_WHO_AM_I_VAL)
    {
        return false;
    }
    return true;
}

bool px_lis3dh_test_start(void)
{
    px_lis3dh_test_active_flag = false;

    // Enable Accelerometer X,Y & Z measurement @ 10Hz (low power disabled)
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG1,
                            PX_LIS3DH_REG_CTRL_REG1_ODR_1HZ
                          | PX_LIS3DH_REG_CTRL_REG1_LPEN_DIS
                          | PX_LIS3DH_REG_CTRL_REG1_ZEN_EN
                          | PX_LIS3DH_REG_CTRL_REG1_YEN_EN
                          | PX_LIS3DH_REG_CTRL_REG1_XEN_EN) == false)
    {
        return false;
    }
    // Disable high-pass filter
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG2,
                            (0 << PX_LIS3DH_REG_CTRL_REG2_HPM1)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HPM0)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HPCF2)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HPCF1)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_FDS)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HPCLICK)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HP_IA2)
                          | (0 << PX_LIS3DH_REG_CTRL_REG2_HP_IA1)) == false)
    {
        return false;
    }
    // Disable IA1 interrupt on INT1 pin
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG3,
                            (0 << PX_LIS3DH_REG_CTRL_REG3_I1_CLICK)
                          | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_IA1)
                          | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_IA2)
                          | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_ZYXDA)
                          | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_WTM)
                          | (0 << PX_LIS3DH_REG_CTRL_REG3_I1_OVERRUN)) == false)
    {
        return false;
    }
    // Set scale to +-2g (high-resolution mode disabled) and enable Block Data Update
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG4,
                            (1 << PX_LIS3DH_REG_CTRL_REG4_BDU)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_BLE)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_FS1)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_FS0)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_HR)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_ST1)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_ST0)
                          | (0 << PX_LIS3DH_REG_CTRL_REG4_SIM)) == false)
    {
        return false;
    }
    // Disable interrupt on INT1
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_INT1_CFG,
                            (0 << PX_LIS3DH_REG_INT1_CFG_AOI)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_6D)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_ZHIE)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_ZLIE)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_YHIE)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_YLIE)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_XHIE)
                          | (0 << PX_LIS3DH_REG_INT1_CFG_XLIE)) == false)
    {
        return false;
    }
    // Success
    return true;
}

bool px_lis3dh_test_stop(void)
{
    px_lis3dh_test_active_flag = false;

    // Put accelerometer in power down mode
    if(px_lis3dh_reg_wr_u8(PX_LIS3DH_REG_CTRL_REG1,
                            PX_LIS3DH_REG_CTRL_REG1_ODR_PD
                          | PX_LIS3DH_REG_CTRL_REG1_LPEN_EN
                          | PX_LIS3DH_REG_CTRL_REG1_ZEN_DIS
                          | PX_LIS3DH_REG_CTRL_REG1_YEN_DIS
                          | PX_LIS3DH_REG_CTRL_REG1_XEN_DIS) == false)
    {
        return false;
    }
    // Success
    return true;
}

bool px_lis3dh_test_active(void)
{
    return px_lis3dh_test_active_flag;
}
