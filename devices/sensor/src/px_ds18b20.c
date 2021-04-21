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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ds18b20.h"
#include "px_one_wire.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_ds18b20");;

#define PX_DS18B20_CMD_CONVERT_T       0x44
#define PX_DS18B20_CMD_WR_SCRATCHPAD   0x4e
#define PX_DS18B20_CMD_RD_SCRATCHPAD   0xbe
#define PX_DS18B20_CMD_COPY_SCRATCHPAD 0x48
#define PX_DS18B20_CMD_RECALL_EE       0xb8
#define PX_DS18B20_CMD_RD_PWR_SUPPLY   0xb4

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static px_ds18b20_error_t px_ds18b20_start(px_one_wire_rom_t * rom)
{
    if(rom != NULL)
    {
        if(px_one_wire_match_rom(rom) != PX_ONE_WIRE_ERR_NONE)
        {
            PX_LOG_E("PX_DS18B20 device not present");
            return PX_DS18B20_ERR_NO_DEVICES_PRESENT;
        }
    }
    else
    {
        if(px_one_wire_skip_rom() != PX_ONE_WIRE_ERR_NONE)
        {
            PX_LOG_E("PX_DS18B20 device not present");
            return PX_DS18B20_ERR_NO_DEVICES_PRESENT;
        }
    }

    return PX_DS18B20_ERR_NONE;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_ds18b20_error_t px_ds18b20_start_temp_conversion(px_one_wire_rom_t * rom)
{
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Start temperature conversion
    px_one_wire_wr_u8(PX_DS18B20_CMD_CONVERT_T);

    return PX_DS18B20_ERR_NONE;
}

bool px_ds18b20_temp_conversion_finished(void)
{
    if(px_one_wire_rd_time_slot() == 0)
    {
        // Temperature conversion still busy
        return false;
    }
    else
    {
        // Temperature conversion finished
        return true;
    }
}

px_ds18b20_error_t px_ds18b20_rd_scratchpad(px_one_wire_rom_t *       rom,
                                            px_ds18b20_scratchpad_t * scratchpad)
{
    uint8_t            i;
    uint8_t *          data_u8 = (uint8_t *)scratchpad;
    uint8_t            crc;
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Send READ SCRATCHPAD command
    px_one_wire_wr_u8(PX_DS18B20_CMD_RD_SCRATCHPAD);

    // Read content
    for(i=sizeof(px_ds18b20_scratchpad_t); i != 0; i--)
    {
        *data_u8++ = px_one_wire_rd_u8();
    }
    PX_LOG_D("Temp = %02X%02X ", scratchpad->temp_msb, scratchpad->temp_lsb);
    PX_LOG_D("TH = %02X, TL = %02X", scratchpad->th, scratchpad->tl);
    PX_LOG_D("CFG = %02X", scratchpad->cfg_reg);
    PX_LOG_D("RES = %02X %02X %02X",
             scratchpad->reserved1,
             scratchpad->reserved2,
             scratchpad->reserved3);
    PX_LOG_D("CRC = %02X", scratchpad->crc);

    // Calculate CRC
    crc = px_one_wire_calc_crc8(scratchpad, offsetof(px_ds18b20_scratchpad_t, crc));

    // Check CRC
    if(scratchpad->crc != crc)
    {
        PX_LOG_E("CRC check failed. Read 0x%02X, calculated 0x%02X",
                scratchpad->crc, crc);
        return PX_DS18B20_ERR_CRC_CHECK_FAILED;
    }

    // Success
    return PX_DS18B20_ERR_NONE;
}

px_ds18b20_error_t px_ds18b20_wr_scratchpad(px_one_wire_rom_t *             rom,
                                            const px_ds18b20_scratchpad_t * scratchpad)
{
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Send WRITE SCRATCHPAD command
    px_one_wire_wr_u8(PX_DS18B20_CMD_WR_SCRATCHPAD);

    // Write content
    px_one_wire_wr_u8(scratchpad->th);
    px_one_wire_wr_u8(scratchpad->tl);
    px_one_wire_wr_u8(scratchpad->cfg_reg);

    // Success
    return PX_DS18B20_ERR_NONE;
}

px_ds18b20_error_t px_ds18b20_copy_scratchpad(px_one_wire_rom_t * rom)
{
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Send COPY SCRATCHPAD command
    px_one_wire_wr_u8(PX_DS18B20_CMD_COPY_SCRATCHPAD);

    // Success
    return PX_DS18B20_ERR_NONE;
}

px_ds18b20_error_t px_ds18b20_rd_pwr_supply(px_one_wire_rom_t * rom,
                                            bool *              bus_pwr_flag)
{
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Send Read Power Supply command
    px_one_wire_wr_u8(PX_DS18B20_CMD_RD_PWR_SUPPLY);

    if(px_one_wire_rd_time_slot() == 0)
    {
        // One or more devices are parasitic powered
        PX_LOG_D("Parasitic powered");
        *bus_pwr_flag = false;
    }
    else
    {
        // All of the devices are bus powered
        PX_LOG_D("Bus powered");
        *bus_pwr_flag = true;
    }

    // Success
    return PX_DS18B20_ERR_NONE;
}

px_ds18b20_error_t px_ds18b20_rd_temp(px_one_wire_rom_t * rom,
                                      uint8_t *           temp_msb,
                                      uint8_t *           temp_lsb)
{
    px_ds18b20_error_t error;

    // Address DS18B20
    error = px_ds18b20_start(rom);
    if(error != PX_DS18B20_ERR_NONE)
    {
        return error;
    }

    // Send Read Scratchpad command
    px_one_wire_wr_u8(PX_DS18B20_CMD_RD_SCRATCHPAD);

    // Read content
    *temp_lsb = px_one_wire_rd_u8();
    *temp_msb = px_one_wire_rd_u8();
    PX_LOG_D("Temp = %02X%02X ", *temp_msb, *temp_lsb);

    // Success
    return PX_DS18B20_ERR_NONE;
}

int16_t px_ds18b20_util_convert_t_to_temp(uint8_t temp_msb, uint8_t temp_lsb, uint8_t cfg_reg)
{
    int32_t i;

    // At what resolution was temperature measurement made?
    switch(cfg_reg & PX_DS18B20_CFG_REG_RES_MASK)
    {
    case PX_DS18B20_CFG_REG_RES_9BIT:
        // 9-bit resolution. Clear 3 least significant bits
        temp_lsb &= 0xf8;
        break;

    case PX_DS18B20_CFG_REG_RES_10BIT:
        // 10-bit resolution. Clear 2 least significant bits
        temp_lsb &= 0xfc;
        break;

    case PX_DS18B20_CFG_REG_RES_11BIT:
        // 11-bit resolution. Clear least significant bit
        temp_lsb &= 0xfe;
        break;

    case PX_DS18B20_CFG_REG_RES_12BIT:
        // Fall through...

    default:
        break;
    }

    // Assemble MSB and LSB
    i = (int32_t)((int16_t)PX_U16_CONCAT_U8(temp_msb, temp_lsb));

    /*
     *  Convert to 0.01 deg C resolution by multiplying with 100
     *  and dividing by 16 (with rounding), for example:
     *  0x0191 ->  2506 (+25.06 deg Celsius)
     *  0xff5e -> -1013 (-10.13 deg Celsius)
     */
    i = i * 100;
    if(i >= 0)
    {
        i = (i+8) / 16;
    }
    else
    {
        i = (i-8) / 16;
    }
    PX_LOG_D("Temp = %d", i);
    return (int16_t)i;
}

uint16_t px_ds18b20_util_cfg_to_temp_conv_time_ms(uint8_t cfg_reg)
{
    switch(cfg_reg & PX_DS18B20_CFG_REG_RES_MASK)
    {
    case PX_DS18B20_CFG_REG_RES_9BIT:   return PX_DS18B20_CONV_TIME_MS_9BIT;
    case PX_DS18B20_CFG_REG_RES_10BIT:  return PX_DS18B20_CONV_TIME_MS_10BIT;
    case PX_DS18B20_CFG_REG_RES_11BIT:  return PX_DS18B20_CONV_TIME_MS_11BIT;
    case PX_DS18B20_CFG_REG_RES_12BIT:  return PX_DS18B20_CONV_TIME_MS_12BIT;
    default:                            return PX_DS18B20_CONV_TIME_MS_12BIT;
    }
}

