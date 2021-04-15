/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_veml6075.h : Vishay VEML6075 UV light sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-05

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_veml6075.h"
#include "px_i2c.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_veml6075")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name Command code and register map (table 1, p. 6)
//@{
#define PX_VEML6075_REG_UV_CFG              0x00
#define PX_VEML6075_REG_UVA_DATA            0x07
#define PX_VEML6075_REG_UVB_DATA            0x09
#define PX_VEML6075_REG_UVCOMP1_DATA        0x0A
#define PX_VEML6075_REG_UVCOMP2_DATA        0x0B
#define PX_VEML6075_REG_ID                  0x0C
//@}

/// @name Config register parameters (table 2, p. 7)
//@{
#define PX_VEML6075_REG_CFG_UV_IT_50MS      0x00
#define PX_VEML6075_REG_CFG_UV_IT_100MS     0x10
#define PX_VEML6075_REG_CFG_UV_IT_200MS     0x20
#define PX_VEML6075_REG_CFG_UV_IT_400MS     0x30
#define PX_VEML6075_REG_CFG_UV_IT_800MS     0x40
#define PX_VEML6075_REG_CFG_HD              0x08
#define PX_VEML6075_REG_CFG_UV_TRIG_NO      0x00
#define PX_VEML6075_REG_CFG_UV_TRIG_ONCE    0x04
#define PX_VEML6075_REG_CFG_UV_AF_NORMAL    0x00
#define PX_VEML6075_REG_CFG_UV_AF_FORCE     0x02
#define PX_VEML6075_REG_CFG_SD              0x01
//@}

/// Register 0x0C "ID" value
#define PX_VEML6075_REG_ID_VAL              0x26

/// Register data definition
typedef struct
{
    uint8_t lsb;    ///< Least Significant Byte
    uint8_t msb;    ///< Most Significant Byte
} px_veml6075_reg_data;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_veml6075_i2c_handle;

static const float px_veml6075_uva_a_coef = 2.22;
static const float px_veml6075_uva_b_coef = 1.33;
static const float px_veml6075_uva_c_coef = 2.95;
static const float px_veml6075_uva_d_coef = 1.74;
static const float px_veml6075_uva_resp   = 0.001461;
static const float px_veml6075_uvb_resp   = 0.002591;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_veml6075_reg_wr(uint8_t cmd, const px_veml6075_reg_data * data)
{
    uint8_t buf[3];

    buf[0] = cmd;
    buf[1] = data->lsb;
    buf[2] = data->msb;

    if(!px_i2c_wr(px_veml6075_i2c_handle, buf, 3, PX_I2C_FLAG_START_AND_STOP))
    {
        PX_DBG_E("Write failed");
        return false;
    }

    return true;
}

static bool px_veml6075_reg_rd(uint8_t cmd, px_veml6075_reg_data * data)
{
    uint8_t buf[2];

    buf[0] = cmd;

    if(!px_i2c_wr(px_veml6075_i2c_handle, buf, 1, PX_I2C_FLAG_START_AND_END))
    {
        PX_DBG_E("Write failed");
        return false;
    }

    if(!px_i2c_rd(px_veml6075_i2c_handle, buf, 2, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        PX_DBG_E("Read failed");
        return false;
    }

    data->lsb = buf[0];
    data->msb = buf[1];

    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_veml6075_init(px_i2c_handle_t * handle)
{
    px_veml6075_reg_data reg_data;

    // Save I2C slave handle
    px_veml6075_i2c_handle = handle;

    // Verify Chip ID
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_ID, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read Chip ID");
        return false;
    }
    if(reg_data.lsb != PX_VEML6075_REG_ID_VAL)
    {
        // Error
        PX_DBG_E("Chip ID does not match. Received %02X, but expected %02X",
                   reg_data.lsb, PX_VEML6075_REG_ID_VAL);
        return false;
    }
    PX_DBG_I("UV sensor detected");
    // Configure sensor
    reg_data.lsb =   PX_VEML6075_REG_CFG_UV_IT_100MS 
                   | PX_VEML6075_REG_CFG_UV_TRIG_NO
                   | PX_VEML6075_REG_CFG_UV_AF_NORMAL;
    reg_data.msb = 0;
    if(!px_veml6075_reg_wr(PX_VEML6075_REG_UV_CFG, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to configure sensor");
        return false;
    }
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_UV_CFG, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read config");
        return false;
    }
    PX_DBG_I("Cfg = 0x%02X, 0x%02X", reg_data.lsb, reg_data.msb);

    // Success
    return true;
}

bool px_veml6075_meas(px_veml6075_data_t * data)
{
    px_veml6075_reg_data reg_data;

    // Read UVA register
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_UVA_DATA, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read UVA");
        return false;
    }
    data->uva = PX_U16_CONCAT_U8(reg_data.msb, reg_data.lsb);
    // Read UVB register
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_UVB_DATA, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read UVB");
        return false;
    }
    data->uvb = PX_U16_CONCAT_U8(reg_data.msb, reg_data.lsb);
    // Read UVCOMP1 register
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_UVCOMP1_DATA, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read UVCOMP1");
        return false;
    }
    data->uvcomp1 = PX_U16_CONCAT_U8(reg_data.msb, reg_data.lsb);
    // Read UVCOMP2 register
    if(!px_veml6075_reg_rd(PX_VEML6075_REG_UVCOMP2_DATA, &reg_data))
    {
        // Error
        PX_DBG_E("Unable to read UVCOMP2");
        return false;
    }
    data->uvcomp2 = PX_U16_CONCAT_U8(reg_data.msb, reg_data.lsb);
    // Success
    PX_DBG_I("UVA=0x%04X, UVB=%04X, UVCOMP1=0x%04X, UVCOMP2=0x%04X",
                data->uva, data->uvb, data->uvcomp1, data->uvcomp2);
    return true;
}

float px_veml6075_uva(const px_veml6075_data_t * data)
{
    float uva;

    uva =   data->uva 
          - (px_veml6075_uva_a_coef * data->uvcomp1)
          - (px_veml6075_uva_b_coef * data->uvcomp2);

    return uva;
}

float px_veml6075_uvb(const px_veml6075_data_t * data)
{
    float uvb;

    uvb =   data->uvb 
          - (px_veml6075_uva_c_coef * data->uvcomp1)
          - (px_veml6075_uva_d_coef * data->uvcomp2);

    return uvb;
}

float px_veml6075_uvi(const px_veml6075_data_t * data)
{
    float uva;
    float uvb;
    float uvi;

    uva = px_veml6075_uva(data);
    uvb = px_veml6075_uvb(data);

    uva = uva * px_veml6075_uva_resp;
    uvb = uvb * px_veml6075_uvb_resp;
    uvi = (uva + uvb) / 2.0;

    return uvi;
}
