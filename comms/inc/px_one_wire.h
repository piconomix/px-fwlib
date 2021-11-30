#ifndef __PX_ONE_WIRE_H__
#define __PX_ONE_WIRE_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
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
 *  - comms/inc/px_one_wire.h
 *  - comms/inc/px_one_wire_cfg_template.h
 *  - comms/src/px_one_wire.c
 *  
 *  References:
 *  - [Maxim AN126 - 1-Wire Communication Through Software] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/126)
 *  - [Maxim AN937 - Book of iButton Standards, Chapter 4] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/937)
 *  - [Maxim AN187 - 1-Wire Search Algorithm] (https://www.maximintegrated.com/en/app-notes/index.mvp/id/187)
 *  - [Wikipedia - 1-Wire] (https://en.wikipedia.org/wiki/1-Wire)
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_one_wire_cfg_template.h"
#include "px_one_wire_cfg.h"

// Check that all project specific options have been correctly specified
#if (   !defined(PX_ONE_WIRE_CFG_PIN_INIT      ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_DIR_SET_IN) \
     || !defined(PX_ONE_WIRE_CFG_PIN_SET_LO    ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_IS_HI     ) \
     || !defined(PX_ONE_WIRE_CFG_PIN_IS_LO     ) \
     || !defined(PX_ONE_WIRE_CFG_DELAY_US      ) \
     || !defined(PX_ONE_WIRE_CFG_INTS_DISABLE  ) \
     || !defined(PX_ONE_WIRE_CFG_INTS_ENABLE   )  )
#error "One or more options not defined in 'px_xmodem_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* _____DEFINITIONS__________________________________________________________ */
/// Size of 1-Wire device serial number
#define PX_ONE_WIRE_SERIAL_SIZE_BYTES  6

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Error code
typedef enum
{
    PX_ONE_WIRE_ERR_NONE = 0,
    PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT,
    PX_ONE_WIRE_ERR_CRC_CHECK_FAILED,
    PX_ONE_WIRE_ERR_FAMILY_CODE_IS_ZERO,
    PX_ONE_WIRE_ERR_LAST_DEVICE,
} px_one_wire_error_t;

/// Device ROM content
typedef struct
{
    uint8_t family_code;
    uint8_t serial[PX_ONE_WIRE_SERIAL_SIZE_BYTES];
    uint8_t crc;
} px_one_wire_rom_content_t;

/// Device ROM
typedef struct
{
    union
    {
        uint8_t                   data[sizeof(px_one_wire_rom_content_t)];
        px_one_wire_rom_content_t content;
    };
} px_one_wire_rom_t;

/// 1-Wire device search
typedef struct
{
    uint8_t           last_discrepancy;
    bool              last_device_flag;
    px_one_wire_rom_t rom;
} px_one_wire_search_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise 1-Wire driver.
 */
void px_one_wire_init(void);

/**
 *  Perform 1-Wire bus reset.
 *
 *  @retval true        One or more devices detected on bus
 *  @retval false       No devices detected on bus
 */
bool px_one_wire_reset(void);
                                                
/**
 *  Write a byte on 1-Wire bus.
 *
 *  @param data         Value to write
 */
void px_one_wire_wr_u8(uint8_t data);

/**
 *  Read one byte on 1-Wire bus.
 *
 *  @return uint8_t     Value read
 */
uint8_t px_one_wire_rd_u8(void);

/**
 *  Calculate 1-Wire data checksum.
 *
 *  @param data         Pointer to buffer containing data to calculate CRC
 *  @param nr_of_bytes  Number of bytes to calculate CRC over
 *
 *  @return uint8_t     Calculated CRC
 */
uint8_t px_one_wire_calc_crc8(void * data, size_t nr_of_bytes);


/**
 *  Read time slot bit.
 *
 *
 *  @return uint8_t     Value
 */
uint8_t px_one_wire_rd_time_slot(void);
                                                
/**
 *  Read ROM of 1-Wire device on bus.
 *
 *  @param rom          Buffer to store ROM data read from 1-Wire device
 *
 *  @return px_one_wire_error_t error (NONE = 0)
 */
px_one_wire_error_t px_one_wire_rd_rom(px_one_wire_rom_t * rom);

/**
 *  Perform SKIP ROM command.
 *
 *  @return px_one_wire_error_t error (NONE = 0)
 */
px_one_wire_error_t px_one_wire_skip_rom(void);

/**
 *  Perform a MATCH ROM command to address a specific 1-Wire device.
 *
 *  @param rom
 *
 *  @return px_one_wire_error_t error (NONE = 0)
 */
px_one_wire_error_t px_one_wire_match_rom(px_one_wire_rom_t * rom);

/**
 *  Start 1-Wire search ROM procedure.
 *
 *  @param one_wire_search      Search state
 *
 *  @return px_one_wire_error_t error (NONE = 0)
 */
px_one_wire_error_t px_one_wire_search_rom_first(px_one_wire_search_t * one_wire_search);

/**
 *  Continue 1-Wire search ROM procedure to find next device.
 *
 *  @param one_wire_search      Search state
 *
 *  @return px_one_wire_error_t error (NONE = 0)
 */
px_one_wire_error_t px_one_wire_search_rom_next (px_one_wire_search_t * one_wire_search);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
