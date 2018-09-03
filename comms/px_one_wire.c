/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
 
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
PX_DBG_DECL_NAME("px_one_wire");

/* _____LOCAL DEFINITIONS____________________________________________________ */
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
    uint8_t    i;
    uint8_t    crc, byte;
    uint8_t *  data_u8 = (uint8_t *)data;
    uint8_t    x8;

    crc = 0;
    while(nr_of_bytes != 0)
    {
        // Fetch byte
        byte = *data_u8;

        for (i = 8; i != 0; i--)
        {
            // Calc X^8
            x8 = (crc ^ byte) & 0x01;

            // Shift register
            crc >>= 1;

            // X^8 = 1?
			if(x8 != 0)
            {
                // XOR with X^5+ X^4 + X^0 (reverse order)
                crc ^= 0x8c;
            }

            // Next data bit
			byte >>= 1;
		}
        
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

    // Execute READ ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_READ_ROM);

    // Read ROM content
    for(i=sizeof(px_one_wire_rom_t); i != 0; i--)
    {
        *data_u8++ = px_one_wire_rd_u8();
    }
    PX_DBG_INFO("Family code = 0x%02X", rom->family_code);
    PX_DBG_INFO("Serial      = %02X:%02X:%02X:%02X:%02X:%02X",
                rom->serial[0],
                rom->serial[1],
                rom->serial[2],
                rom->serial[3],
                rom->serial[4],
                rom->serial[5]);
    PX_DBG_INFO("CRC         = 0x%02X", rom->crc);

    // Calculate CRC
    crc = px_one_wire_calc_crc8(rom, offsetof(px_one_wire_rom_t, crc));

    // Check CRC
    if(rom->crc != crc)
    {
        PX_DBG_ERR("CRC check failed. Read 0x%02X, calculated 0x%02X",
                rom->crc, crc);
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

    // Execute SKIP ROM command
    px_one_wire_wr_u8(PX_ONE_WIRE_CMD_SKIP_ROM);

    // Success
    return PX_ONE_WIRE_ERR_NONE;
}
