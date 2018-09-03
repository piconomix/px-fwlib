/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2010 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_xtea.h : XTEA (eXtended Tiny Encryption Algorithm)
    Author(s):      Pieter Conradie
    Creation Date:  2010-05-03

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

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
void px_xtea_init(const uint32_t key[4])
{
    uint8_t i;

    for(i=0;i<4;i++)
    {
        px_xtea_key[i] = key[i];
    }
}

void px_xtea_encrypt(uint32_t data[2])
{
    uint8_t i;

    uint32_t d0  = data[0];
    uint32_t d1  = data[1];
    uint32_t sum = 0;

    for (i=PX_XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d0  += (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + px_xtea_key[sum & 3]);
        sum += PX_XTEA_DELTA;
        d1  += (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + px_xtea_key[(sum>>11) & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}

void px_xtea_decrypt(uint32_t data[2])
{
    uint8_t i;

    uint32_t d0  = data[0];
    uint32_t d1  = data[1];
    uint32_t sum = PX_XTEA_DELTA*PX_XTEA_NUMBER_OF_ROUNDS;

    for (i=PX_XTEA_NUMBER_OF_ROUNDS; i != 0; i--)
    {
        d1  -= (((d0 << 4) ^ (d0 >> 5)) + d0) ^ (sum + px_xtea_key[(sum>>11) & 3]);
        sum -= PX_XTEA_DELTA;
        d0  -= (((d1 << 4) ^ (d1 >> 5)) + d1) ^ (sum + px_xtea_key[sum & 3]);
    }
    data[0] = d0;
    data[1] = d1;
}

