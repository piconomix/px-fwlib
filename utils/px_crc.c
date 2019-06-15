/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_crc.h : CRC calculator
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_crc.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_crc_t px_crc_update_u8(px_crc_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= data;

    for(i=8; i!=0; i--)
    {
        if(crc & 1)
        {
            crc = (crc>>1) ^ PX_CRC_POLYNOMIAL;
        }
        else
        {
            crc = (crc>>1);
        }
    }

    return crc;
}

px_crc_t px_crc_update_data(px_crc_t crc, const void * data, size_t nr_of_bytes)
{
    const uint8_t * data_u8 = (uint8_t *)data;

    while(nr_of_bytes)
    {
        crc = px_crc_update_u8(crc, *data_u8++);
        nr_of_bytes--;
    }

    return crc;
}

