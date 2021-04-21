#ifndef __PX_CRC32_H__
#define __PX_CRC32_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_crc32.h : 32-bit CRC calculator
    Author(s):      Pieter Conradie
    Creation Date:  2019-08-06

============================================================================= */

/**
 *  @ingroup UTILS
 *  @defgroup PX_CRC32 px_crc32.h : 32-bit CRC calculator
 *
 *  32-bit CRC (Cyclic Redundancy Check) calculator for checksums.
 *
 *  File(s):
 *  - utils/inc/px_crc32.h
 *  - utils/inc/px_crc32_cfg_template.h
 *  - utils/src/px_crc32.c
 *
 *  A CRC is an error-detecting code that is used for data integrity checks.
 *
 *  @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  @see https://crccalc.com
 *
 *  Note that the polynomial value is bit reversed and a right shift operator is
 *  used during the calculation.
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_crc32_cfg_template.h"
#include "px_crc32_cfg.h"

// Check that all project specific options have been specified in "px_rtc_util_cfg.h"
#if (   !defined(PX_CRC32_RAM_TABLE) \
     || !defined(PX_CRC32_ROM_TABLE)  )
      )
#error "One or more options not defined in 'px_crc32_cfg.h'"
#endif

#if (PX_CRC32_RAM_TABLE + PX_CRC32_ROM_TABLE > 1)
#error "Use either RAM or ROM table option, not both"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// The generator polynomial CRC32 (reversed representation):
///   x^32 + x^26 + x^23 + x^22 + x^16
/// + x^12 + x^11 + x^10 + x^8  + x^7 + x^5 + x^4 + x^2 + x + 1
#define PX_CRC32_POLYNOMIAL  0xedb88320
/// Initial CRC value
#define PX_CRC32_INIT_VAL    0xffffffff

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise 32-bit CRC calculator.
 *
 *  This function will initialise the table in RAM with constants to speed up
 *  CRC calculation. For other options it does nothing.
 */
void px_crc32_init(void);

/**
 *  Calculate the 32-bit CRC over one byte.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         The value to calculate the CRC over
 *
 *  @return uint32_t    The resultant CRC
 */
uint32_t px_crc32_update_u8(uint32_t crc, uint8_t data);

/**
 *  Calculate the 32-bit CRC over a number of bytes.
 *
 *  @param crc          The initial CRC to start the calculation with
 *  @param data         Pointer to the data to calculate the CRC over
 *  @param nr_of_bytes  The amount of bytes to calculate the CRC over
 *
 *  @return uint32_t    The resultant CRC over the group of bytes
 */
uint32_t px_crc32_update_data(uint32_t crc, const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_CRC32_H__
