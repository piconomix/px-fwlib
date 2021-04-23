#ifndef __PX_CRC8_H__
#define __PX_CRC8_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_crc8.h : 8-bit CRC calculator
    Author(s):      Pieter Conradie
    Creation Date:  2020-10-23

============================================================================= */

/**
 *  @ingroup UTILS
 *  @defgroup PX_CRC8 px_crc8.h : 8-bit CRC calculator
 *
 *  8-bit CRC (Cyclic Redundancy Check) calculator for checksums.
 *
 *  File(s):
 *  - utils/inc/px_crc8.h
 *  - utils/inc/px_crc8_cfg_template.h
 *  - utils/src/px_crc8.c
 *
 *  A CRC is an error-detecting code that is used for data integrity checks.
 *
 *  @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  @see https://crccalc.com
 *
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_crc8_cfg_template.h"
#include "px_crc8_cfg.h"

// Check that all project specific options have been specified in "px_crc8_cfg.h"
#if (   !defined(PX_CRC8_RAM_TABLE) \
     || !defined(PX_CRC8_ROM_TABLE)  )
      )
#error "One or more options not defined in 'px_crc8_cfg.h'"
#endif

#if (PX_CRC8_RAM_TABLE + PX_CRC8_ROM_TABLE > 1)
#error "Use either RAM or ROM table option, not both"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// The generator polynomial CRC8 (normal representation):
/// x^8 + x^2 + x + 1
#define PX_CRC8_POLYNOMIAL  0x07
/// Initial CRC value
#define PX_CRC8_INIT_VAL    0x00

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise 8-bit CRC calculator.
 *
 *  This function will initialise the table in RAM with constants to speed up
 *  CRC calculation. For other options it does nothing.
 */
void px_crc8_init(void);

/**
 *  Calculate the 8-bit CRC over one byte.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         The value to calculate the CRC over
 *
 *  @return uint8_t     The resultant CRC
 */
uint8_t px_crc8_update_u8(uint8_t crc, uint8_t data);

/**
 *  Calculate the 8-bit CRC over a number of bytes.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         Pointer to the data to calculate the CRC over
 *  @param nr_of_bytes  The amount of bytes to calculate the CRC over
 *
 *  @return uint8_t     The resultant CRC over the group of bytes
 */
uint8_t px_crc8_update_data(uint8_t crc, const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
