#ifndef __PX_CRC16_H__
#define __PX_CRC16_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_crc16.h : 16-bit CRC calculator
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-06

============================================================================= */

/**
 *  @ingroup UTILS
 *  @defgroup PX_CRC16 px_crc16.h : 16-bit CRC calculator
 *
 *  16-bit CRC (Cyclic Redundancy Check) calculator for checksums.
 *
 *  File(s):
 *  - utils/inc/px_crc16.h
 *  - utils/inc/px_crc16_cfg_template.h
 *  - utils/src/px_crc16.c
 *
 *  A CRC is an error-detecting code that is used for data integrity checks.
 *
 *  @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  @see https://crccalc.com
 *
 *  Note that the polynomial value is bit reversed and a right shift operator is
 *  used during the calculation.
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_crc16_cfg_template.h"
#include "px_crc16_cfg.h"

// Check that all project specific options have been specified in "px_crc16_cfg.h"
#if (   !defined(PX_CRC16_RAM_TABLE) \
     || !defined(PX_CRC16_ROM_TABLE)  )
      )
#error "One or more options not defined in 'px_crc16_cfg.h'"
#endif

#if (PX_CRC16_RAM_TABLE + PX_CRC16_ROM_TABLE > 1)
#error "Use either RAM or ROM table option, not both"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// The generator polynomial CRC16-CCITT (reversed representation):
/// x^16 + x^12 + x^5 + 1
#define PX_CRC16_POLYNOMIAL  0x8408
/// Initial CRC value
#define PX_CRC16_INIT_VAL    0xffff
// Magic CRC value
#define PX_CRC16_MAGIC_VAL   0xf0b8

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise 16-bit CRC calculator.
 *
 *  This function will initialise the table in RAM with constants to speed up
 *  CRC calculation. For other options it does nothing.
 */
void px_crc16_init(void);

/**
 *  Calculate the 16-bit CRC over one byte.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         The value to calculate the CRC over
 *
 *  @return uint16_t    The resultant CRC
 */
uint16_t px_crc16_update_u8(uint16_t crc, uint8_t data);

/**
 *  Calculate the 16-bit CRC over a number of bytes.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         Pointer to the data to calculate the CRC over
 *  @param nr_of_bytes  The amount of bytes to calculate the CRC over
 *
 *  @return uint16_t    The resultant CRC over the group of bytes
 */
uint16_t px_crc16_update_data(uint16_t crc, const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
