/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2010 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_xtea.h : XTEA (eXtended Tiny Encryption Algorithm)
    Author(s):      Pieter Conradie
    Creation Date:  2010-05-03

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_xtea.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#ifndef PX_XTEA_NUMBER_OF_ROUNDS
#define PX_XTEA_NUMBER_OF_ROUNDS 16
#endif

#define PX_XTEA_DELTA            0x9E3779B9

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint32_t px_xtea_key[4];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_xtea_init(const uint32_t key[4] )
{
    uint8_t i;

    for(i = 0; i < 4; i++)
    {
        px_xtea_key[i] = key[i];
    }
}

void px_xtea_encrypt(uint32_t data[2] )
{
    uint8_t i;

    uint32_t d0  = data[0];
    uint32_t d1  = data[1];
    uint32_t sum = 0;

    for(i = PX_XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d0  += (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + px_xtea_key[sum & 3]);
        sum += PX_XTEA_DELTA;
        d1  += (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + px_xtea_key[(sum >> 11) & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}

size_t px_xtea_encrypt_data_ecb(const uint8_t * data_in, uint8_t * data_out, size_t nr_of_bytes_in)
{
    uint32_t buf[2];
    size_t   nr_of_bytes_out = 0;

    while(nr_of_bytes_in != 0)
    {
        // Less than 8 bytes (64 bits)?
        if(nr_of_bytes_in < 8)
        {
            // Set bytes to zero
            buf[0] = 0;
            buf[1] = 0;
            // Copy remaining bytes
            memcpy(buf, data_in, nr_of_bytes_in);
            // Encrypt block
            px_xtea_encrypt(buf);
            memcpy(data_out, buf, 8);
            // Next block
            nr_of_bytes_out += 8;
            // Done
            break;
        }
        else
        {
            // Copy 8 bytes
            memcpy(buf, data_in, 8);
            // Encrypt block
            px_xtea_encrypt(buf);
            memcpy(data_out, buf, 8);
            // Next block
            nr_of_bytes_in  -= 8;
            nr_of_bytes_out += 8;
            data_in         += 8;
            data_out        += 8;
        }
    }
    return nr_of_bytes_out;
}

void px_xtea_decrypt(uint32_t data[2] )
{
    uint8_t i;

    uint32_t d0  = data[0];
    uint32_t d1  = data[1];
    uint32_t sum = (uint32_t)(PX_XTEA_DELTA * PX_XTEA_NUMBER_OF_ROUNDS);

    for(i = PX_XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d1  -= (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + px_xtea_key[(sum >> 11) & 3]);
        sum -= PX_XTEA_DELTA;
        d0  -= (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + px_xtea_key[sum & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}

size_t px_xtea_decrypt_data_ecb(const uint8_t * data_in, uint8_t * data_out, size_t nr_of_bytes_in)
{
    uint32_t buf[2];
    size_t   nr_of_bytes_out = 0;

    while(nr_of_bytes_in != 0)
    {
        // Less than 8 bytes (64 bits)?
        if(nr_of_bytes_in < 8)
        {
            // Set bytes to zero
            buf[0] = 0;
            buf[1] = 0;
            // Copy remaining bytes
            memcpy(buf, data_in, nr_of_bytes_in);
            // Decrypt block
            px_xtea_decrypt(buf);
            memcpy(data_out, buf, 8);
            // Next block
            nr_of_bytes_out += 8;
            // Done
            break;
        }
        else
        {
            // Copy 8 bytes
            memcpy(buf, data_in, 8);
            // Decrypt block
            px_xtea_decrypt(buf);
            memcpy(data_out, buf, 8);
            // Next block
            nr_of_bytes_in  -= 8;
            nr_of_bytes_out += 8;
            data_in         += 8;
            data_out        += 8;
        }
    }
    return nr_of_bytes_out;
}
