#ifndef __PX_ONE_WIRE_H__
#define __PX_ONE_WIRE_H__
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

/** 
 *  @ingroup COMMS
 *  @defgroup PX_ONE_WIRE px_one_wire.h : Dallas/Maxim 1-Wire bit-bang driver
 *  
 *  Implements a Dallas/Maxim 1-Wire bit-bang driver.
 *  
 *  File(s):
 *  - comms/px_one_wire.h
 *  - comms/px_one_wire_cfg_template.h
 *  - comms/px_one_wire.c
 *  
 *  References:
 *  - [Maxim AN126 - 1-Wire Communication Through Software] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/126)
 *  - [Maxim AN937 - Book of iButton Standards, Chapter 4] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/937)
 *  - [Maxim AN187 - 1-Wire Search Algorithm] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/187)
 *  - [Wikipedia - 1-Wire] (https://en.wikipedia.org/wiki/1-Wire)
 *
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific config. See "px_one_wire_cfg_template.h"
#include "px_one_wire_cfg.h"

// Check that all project specific options have been correctly specified
#if (   !defined(PX_ONE_WIRE_CFG_PIN_INIT              ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_DIR_SET_IN        ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_SET_LO            ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_IS_HI             ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_IS_LO             ) \
     || !defined(PX_ONE_WIRE_CFG_DELAY_US              ) \
     || !defined(PX_ONE_WIRE_CFG_INTS_DISABLE) \
     || !defined(PX_ONE_WIRE_CFG_INTS_ENABLE )  )
#error "One or more options not defined in 'px_xmodem_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* _____DEFINITIONS__________________________________________________________ */
#define PX_ONE_WIRE_SERIAL_SIZE_BYTES  6

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef enum
{
    PX_ONE_WIRE_ERR_NONE = 0,
    PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT,
    PX_ONE_WIRE_ERR_CRC_CHECK_FAILED,
    PX_ONE_WIRE_ERR_FAMILY_CODE_IS_ZERO,
    PX_ONE_WIRE_ERR_LAST_DEVICE,
} px_one_wire_error_t;

typedef struct
{
    uint8_t family_code;
    uint8_t serial[PX_ONE_WIRE_SERIAL_SIZE_BYTES];
    uint8_t crc;
} px_one_wire_rom_content_t;

typedef struct
{
    union
    {
        uint8_t                   data[sizeof(px_one_wire_rom_content_t)];
        px_one_wire_rom_content_t content;
    };
} px_one_wire_rom_t;

typedef struct
{
    uint8_t           last_discrepancy;
    bool              last_device_flag;
    px_one_wire_rom_t rom;
} px_one_wire_search_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void                px_one_wire_init            (void);
bool                px_one_wire_reset           (void);
                                                
void                px_one_wire_wr_u8           (uint8_t data);
uint8_t             px_one_wire_rd_u8           (void);
uint8_t             px_one_wire_calc_crc8       (void * data, size_t nr_of_bytes);
uint8_t             px_one_wire_rd_time_slot    (void);
                                                
px_one_wire_error_t px_one_wire_rd_rom          (px_one_wire_rom_t * rom);
px_one_wire_error_t px_one_wire_skip_rom        (void);
px_one_wire_error_t px_one_wire_match_rom       (px_one_wire_rom_t * rom);
px_one_wire_error_t px_one_wire_search_rom_first(px_one_wire_search_t * one_wire_search);
px_one_wire_error_t px_one_wire_search_rom_next(px_one_wire_search_t * one_wire_search);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_ONE_WIRE_H__
