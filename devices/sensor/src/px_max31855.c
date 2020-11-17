/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Bosch MAX31855 Barometric pressure sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2019-09-12

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_max31855.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("max31855");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t * px_max31855_spi_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static int16_t px_max31855_data_to_tc_temp(uint8_t * data)
{
    int16_t t;

    // Asemble bits D[30:18]
    t = (((uint16_t)data[0] & 0x7f) << 6) | (((uint16_t)data[1] & 0xfc) >> 2);
    // Negative value (D31 set)?
    if(data[0] & 0x80)
    {
        // Negate
        t *= -1;
    }

    return t;
}

static int16_t px_max31855_data_to_int_temp(uint8_t * data)
{
    int16_t t;

    // Asemble bits D[14:4]
    t = (((uint16_t)data[2] & 0x7f) << 4) | (((uint16_t)data[3] & 0xf0) >> 4);
    // Negative value (D15 set)?
    if(data[2] & 0x80)
    {
        // Negate
        t *= -1;
    }

    return t;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_max31855_init(px_spi_handle_t * handle)
{
    // Save SPI handle
    px_max31855_spi_handle = handle;
}

void px_max31855_rd(px_max31855_data_t * data)
{
    uint8_t data_u8[4];

    px_spi_rd(&px_spi_tc_handle,
              data_u8,
              4,
              PX_SPI_FLAG_START_AND_STOP);

    // Thermocouple temperature
    data->temp_tc = px_max31855_data_to_tc_temp(data_u8);
    // Internal temperature
    data->temp_int = px_max31855_data_to_int_temp(data_u8);
    // Fault flag
    if(PX_BIT_IS_HI(data_u8[1], 0))
    {
        data->fault = true;
    }
    else
    {
        data->fault = false;
    }
    // SCV flag
    if(PX_BIT_IS_HI(data_u8[3], 2))
    {
        data->fault_scv = true;
    }
    else
    {
        data->fault_scv = false;
    }
    // SCG flag
    if(PX_BIT_IS_HI(data_u8[3], 1))
    {
        data->fault_scg = true;
    }
    else
    {
        data->fault_scg = false;
    }
    // OC flag
    if(PX_BIT_IS_HI(data_u8[3], 0))
    {
        data->fault_oc = true;
    }
    else
    {
        data->fault_oc = false;
    }
}
