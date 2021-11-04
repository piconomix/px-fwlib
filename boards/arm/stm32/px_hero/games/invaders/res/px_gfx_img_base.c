
/*******************************************************************************
*      ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM   *
*     |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /      *
*     | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /       *
*     |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \       *
*     |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\      *
*                                                                              *
*******************************************************************************/
#include "px_gfx.h"


static const uint8_t px_gfx_img_base_data[5] =
{
    // .OOOOOO.
    // OOOOOOOO
    // OOOOOOOO
    // OOO..OOO
    // OO....OO
    0x7e, 
    0xff, 
    0xff, 
    0xe7, 
    0xc3
};

const px_gfx_img_t px_gfx_img_base = {8, 5, px_gfx_img_base_data};
