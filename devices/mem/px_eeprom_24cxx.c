/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          24Cxx I2C EEPROM Driver
    Author(s):      pconradi
    Creation Date:  2012-06-05

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_eeprom_24cxx.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle_t * px_eeprom_24cxx_i2c_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_eeprom_24cxx_init(px_i2c_handle_t * px_i2c_handle)
{
    px_eeprom_24cxx_i2c_handle = px_i2c_handle;
}

bool px_eeprom_24cxx_present_or_not_busy(void)
{
    if(!px_i2c_wr(px_eeprom_24cxx_i2c_handle, NULL, 0, PX_I2C_FLAG_START_AND_STOP))
    {
        return false;
    }
    return true;
}

bool px_eeprom_24cxx_rd_u8(uint16_t adr, uint8_t * data)
{
    uint8_t i2c_data[1];


    // Change I2C Slave Address to include A10,A9 and A8
    px_i2c_ioctl_change_slave_adr(px_eeprom_24cxx_i2c_handle, 
                                  PX_EEPROM_24CXX_I2C_SLA_ADR | (PX_U16_HI8(adr) & 0x7));

    i2c_data[0] = PX_U16_LO8(adr);
    if(!px_i2c_wr(px_eeprom_24cxx_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_STOP))
    {
        return false;
    }
    if(!px_i2c_rd(px_eeprom_24cxx_i2c_handle, data, 1, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        return false;
    }
    return true;
}

bool px_eeprom_24cxx_rd_data(uint16_t adr, void * data, size_t nr_of_bytes)
{
    uint8_t i2c_data[1];

    // Change I2C Slave Address to include A10,A9 and A8
    px_i2c_ioctl_change_slave_adr(px_eeprom_24cxx_i2c_handle, 
                                  PX_EEPROM_24CXX_I2C_SLA_ADR | (PX_U16_HI8(adr) & 0x7));

    i2c_data[0] = PX_U16_LO8(adr);
    if(!px_i2c_wr(px_eeprom_24cxx_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START_AND_STOP))
    {
        return false;
    }
    if(!px_i2c_rd(px_eeprom_24cxx_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        return false;
    }
    return true;
}

bool px_eeprom_24cxx_wr_u8(uint16_t adr, uint8_t data)
{
    uint8_t i2c_data[1];

    // Change I2C Slave Address to include A10,A9 and A8
    px_i2c_ioctl_change_slave_adr(px_eeprom_24cxx_i2c_handle, 
                                  PX_EEPROM_24CXX_I2C_SLA_ADR | (PX_U16_HI8(adr) & 0x7));

    i2c_data[0] = PX_U16_LO8(adr);
    if(!px_i2c_wr(px_eeprom_24cxx_i2c_handle, i2c_data, 1, PX_I2C_FLAG_START))
    {
        return false;
    }
    if(!px_i2c_wr(px_eeprom_24cxx_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_STOP))
    {
        return false;
    }
    return true;
}
