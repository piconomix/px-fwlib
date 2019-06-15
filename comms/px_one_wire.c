/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_one_wire.h : Dallas/Maxim 1-Wire bit-bang driver
    Author(s):      Pieter Conradie
    Creation Date:  2017-01-27

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_one_wire.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_one_wire");;

#define PX_ONE_WIRE_CMD_SEARCH_ROM     0xf0
#define PX_ONE_WIRE_CMD_READ_ROM       0x33
#define PX_ONE_WIRE_CMD_MATCH_ROM      0x55
#define PX_ONE_WIRE_CMD_SKIP_ROM       0xcc

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_one_wire_wr_bit(uint8_t bit)
{
    if (bit)
    {
        // Write '1' bit
        PX_ONE_WIRE_CFG_INTS_DISABLE();
        PX_ONE_WIRE_CFG_PIN_SET_LO();
        PX_ONE_WIRE_CFG_DELAY_US(6);
        PX_ONE_WIRE_CFG_PIN_DIR_SET_IN();
        PX_ONE_WIRE_CFG_INTS_ENABLE();
        PX_ONE_WIRE_CFG_DELAY_US(64);        
    }
    else
    {
        // Write '0' bit
        PX_ONE_WIRE_CFG_INTS_DISABLE();
        PX_ONE_WIRE_CFG_PIN_SET_LO();
        PX_ONE_WIRE_CFG_DELAY_US(60);
        PX_ONE_WIRE_CFG_PIN_DIR_SET_IN();
        PX_ONE_WIRE_CFG_INTS_ENABLE();
        PX_ONE_WIRE_CFG_DELAY_US(10); 
    }
}

static uint8_t px_one_wire_rd_bit(void)
{
    uint8_t bit;

    PX_ONE_WIRE_CFG_INTS_DISABLE();
    PX_ONE_WIRE_CFG_PIN_SET_LO();
    PX_ONE_WIRE_CFG_DELAY_US(6);
    PX_ONE_WIRE_CFG_PIN_DIR_SET_IN();
    PX_ONE_WIRE_CFG_DELAY_US(9);

    // Input is high?
    if(PX_ONE_WIRE_CFG_PIN_IS_HI())
    {
        // Bit is 1
        bit = 1;
    }
    else
    {
        // Bit is 0
        bit = 0;
    }

    PX_ONE_WIRE_CFG_INTS_ENABLE();
    PX_ONE_WIRE_CFG_DELAY_US(55);
    
    return bit;
}

static uint8_t px_one_wire_update_crc8(uint8_t crc, uint8_t data)
{
    uint8_t i;
    uint8_t x8;

    for (i = 8; i != 0; i--)
    {
        // Calc X^8
        x8 = (crc ^ data) & 0x01;
        // Shift register
        crc >>= 1;
        // X^8 = 1?
        if(x8 != 0)
        {
            // XOR with X^5+ X^4 + X^0 (reverse order)
            crc ^= 0x8c;
        }
        // Next data bit
        data >>= 1;
    }
    
    return crc;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_one_wire_init(void)
{
    // Initialise 1-Wire pin as input (pull-up disabled); requires external 4k7 pull-up
    PX_ONE_WIRE_CFG_PIN_INIT();
}

bool px_one_wire_reset(void)
{
    bool presence_detected;

    PX_ONE_WIRE_CFG_PIN_SET_LO();
    PX_ONE_WIRE_CFG_DELAY_US(480);
    PX_ONE_WIRE_CFG_PIN_DIR_SET_IN();
    PX_ONE_WIRE_CFG_DELAY_US(70);

    // Any slave(s) present?
    if(PX_ONE_WIRE_CFG_PIN_IS_LO())
    {
        // Yes
        presence_detected = true;
    }
    else
    {
        // No
        presence_detected = false;
    }
    PX_ONE_WIRE_CFG_DELAY_US(410);

    return presence_detected;
}

void px_one_wire_wr_u8(uint8_t data)
{
    int i;

    for(i = 8; i != 0; i--)
    {
        // Write bit
        px_one_wire_wr_bit(data & 0x01);
        // Next bit
        data >>= 1;
    }
}

uint8_t px_one_wire_rd_u8(void)
{
    int  i;
    uint8_t data = 0;
    
    for(i=8; i != 0; i--)
    {
        // Shift to make space for next bit
        data >>= 1;
        // Read bit
        if(px_one_wire_rd_bit())
        {
            // Set most significant bit
            data |= 0x80;
        }
    }

    return data;
}

uint8_t px_one_wire_calc_crc8(void * data, size_t nr_of_bytes)
{
    uint8_t *  data_u8 = (uint8_t *)data;
    uint8_t    crc = 0;

    while(nr_of_bytes != 0)
    {
        // Update CRC
        crc = px_one_wire_update_crc8(crc, *data_u8);
        // Next byte
        data_u8++;
        nr_of_bytes--;
    }
    
    return crc;
}

uint8_t px_one_wire_rd_time_slot(void)
{
    return px_one_wire_rd_bit();
}

px_one_wire_error_t px_one_wire_rd_rom(px_one_wire_rom_t * rom)
{
    uint8_t   i;
    uint8_t   crc;
    uint8_t * data_u8 = (uint8_t *)rom;

    // Reset device(s)
    if(!px_one_wire_reset())
    {
        PX_DBG_WARN("No devices found");
        return PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT;
    }

    // Issue READ ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_READ_ROM);

    // Read ROM content
    for(i=sizeof(px_one_wire_rom_t); i != 0; i--)
    {
        *data_u8++ = px_one_wire_rd_u8();
    }
    PX_DBG_INFO("Family code = 0x%02X", rom->content.family_code);
    PX_DBG_INFO("Serial      = %02X:%02X:%02X:%02X:%02X:%02X",
                rom->content.serial[0],
                rom->content.serial[1],
                rom->content.serial[2],
                rom->content.serial[3],
                rom->content.serial[4],
                rom->content.serial[5]);
    PX_DBG_INFO("CRC         = 0x%02X", rom->content.crc);

    // Calculate CRC
    crc = px_one_wire_calc_crc8(rom->data, offsetof(px_one_wire_rom_content_t, crc));

    // Check CRC
    if(rom->content.crc != crc)
    {
        PX_DBG_ERR("CRC check failed. Read 0x%02X, calculated 0x%02X",
                rom->content.crc, crc);
        return PX_ONE_WIRE_ERR_CRC_CHECK_FAILED;
    }

    // Success
    return PX_ONE_WIRE_ERR_NONE;
}

px_one_wire_error_t px_one_wire_skip_rom(void)
{
    // Reset device(s)
    if(!px_one_wire_reset())
    {
        PX_DBG_ERR("No devices found");
        return PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT;
    }

    // Issue SKIP ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_SKIP_ROM);

    // Success
    return PX_ONE_WIRE_ERR_NONE;
}

px_one_wire_error_t px_one_wire_match_rom(px_one_wire_rom_t * rom)
{
    uint8_t   i;
    uint8_t * data_u8 = (uint8_t *)rom;

    // Reset device(s)
    if(!px_one_wire_reset())
    {
        PX_DBG_ERR("No devices found");
        return PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT;
    }

    // Issue MATCH ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_MATCH_ROM);

    // Write ROM content
    for(i=sizeof(px_one_wire_rom_t); i != 0; i--)
    {
        px_one_wire_wr_u8(*data_u8++);
    }

    // Success
    return PX_ONE_WIRE_ERR_NONE;
}

px_one_wire_error_t px_one_wire_search_rom_first(px_one_wire_search_t * one_wire_search)
{
    one_wire_search->last_discrepancy        = 0;
    one_wire_search->last_device_flag        = false;

    return px_one_wire_search_rom_next(one_wire_search);
}

px_one_wire_error_t px_one_wire_search_rom_next(px_one_wire_search_t * one_wire_search)
{
    uint8_t rom_bit_number;     // 1 to 64
    uint8_t rom_byte_number;    // 0 to 7
    uint8_t rom_byte_mask;
    uint8_t crc;

    uint8_t rom_bit;
    uint8_t rom_compliment_bit;
    uint8_t rom_search_dir;
    uint8_t rom_last_zero;

    // Last device found during previous call?
    if(one_wire_search->last_device_flag)
    {
        return PX_ONE_WIRE_ERR_LAST_DEVICE;
    }

    // Perform 1-Wire reset
    if(!px_one_wire_reset())
    {
        PX_DBG_ERR("No devices present");
        return PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT;
    }

    // Issue SEARCH ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_SEARCH_ROM);

    // loop until through all ROM bits
    rom_bit_number  = 1;
    rom_last_zero   = 0;
    rom_byte_number = 0;
    rom_byte_mask   = (1 << 0);
    crc             = 0;
    while(rom_bit_number < 65)
    {
        // Read a bit and its complement at the current position
        rom_bit            = px_one_wire_rd_bit();
        rom_compliment_bit = px_one_wire_rd_bit();

        // No devices participating in search?
        if((rom_bit == 1) && (rom_compliment_bit == 1))
        {
            PX_DBG_ERR("No devices participating in search");
            return PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT;
        }

        // Does all devices have a 0 or 1 at the current bit position?
        if(rom_bit != rom_compliment_bit)
        {
            // Yes. Save bit write value for search
            rom_search_dir = rom_bit;
        }
        else
        {
            // No. Discrepancy detected.

            // Is this discrepancy before the last discrepancy on a previous run?
            if(rom_bit_number < one_wire_search->last_discrepancy)
            {
                //  Pick the same as last time
                rom_search_dir = (one_wire_search->rom.data[rom_byte_number] & rom_byte_mask) != 0 ? 1 : 0;
            }
            // Is this discrepancy equal to the last discrepancy on the last run?
            else if(rom_bit_number == one_wire_search->last_discrepancy)
            {
                // Pick 1
                rom_search_dir = 1;
            }
            else
            {
                // New discrepancy. Pick 0
                rom_search_dir = 0;
            }

            // Was 0 was picked?
            if(rom_search_dir == 0)
            {
                // Save position
                rom_last_zero = rom_bit_number;
            }
        }

        // Write search direction bit
        px_one_wire_wr_bit(rom_search_dir);

        // Save bit in ROM data
        if(rom_search_dir == 0)
        {
            one_wire_search->rom.data[rom_byte_number] &= ~rom_byte_mask;
        }
        else
        {
            one_wire_search->rom.data[rom_byte_number] |= rom_byte_mask;
        }
        // Next bit in ROM
        rom_bit_number++;
        rom_byte_mask <<= 1;
        // Next ROM byte?
        if(rom_byte_mask == 0)
        {
            // Update CRC
            crc = px_one_wire_update_crc8(crc, one_wire_search->rom.data[rom_byte_number]);
            // Next byte
            rom_byte_number++;
            // Reset bit mask
            rom_byte_mask = (1 << 0);
        }
    }

    // CRC OK?
    if(crc != 0)
    {
        PX_DBG_ERR("CRC check failed");
        return PX_ONE_WIRE_ERR_CRC_CHECK_FAILED;
    }
    // Family code OK?
    if(one_wire_search->rom.content.family_code == 0)
    {
        PX_DBG_ERR("Family code is zero");
        return PX_ONE_WIRE_ERR_FAMILY_CODE_IS_ZERO;
    }
    // Save last discrepancy
    one_wire_search->last_discrepancy = rom_last_zero;
    // Is this the last device?
    if(one_wire_search->last_discrepancy == 0)
    {
        PX_DBG_INFO("Last device found");
        one_wire_search->last_device_flag = true;
    }
    // Success
    return PX_ONE_WIRE_ERR_NONE;
}
