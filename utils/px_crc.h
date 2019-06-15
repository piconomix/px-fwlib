#ifndef __PX_CRC_H__
#define __PX_CRC_H__
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

/** 
 *  @ingroup UTILS
 *  @defgroup PX_CRC px_crc.h : CRC calculator
 *  
 *  CRC (Cyclic Redundancy Check) calculator for checksums.
 *  
 *  File(s):
 *  - utils/px_crc.h
 *  - utils/px_crc.c
 *  
 *  A CRC is an error-detecting code that is used for data integrity checks.
 *  
 *  @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Size of CRC
typedef uint16_t px_crc_t;
/// The generator polynomial CRC16-CCITT: x^16 + x^12 + x^5 + x^0
#define PX_CRC_POLYNOMIAL  0x8408
/// Initial CRC value
#define PX_CRC_INIT_VAL    0xffff
/// Magic CRC value
#define PX_CRC_MAGIC_VAL   0xf0b8

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Calculate the CRC over one byte.
 *   
 *  @param crc       The initial CRC to start the calculation with
 *  @param data      The value to calculate the CRC over
 *   
 *  @return uint16_t    The resultant CRC
 */
px_crc_t px_crc_update_u8(px_crc_t crc, uint8_t data);

/** 
 *  Calculate the CRC over a number of bytes.
 *   
 *  @param crc           The initial CRC to start the calculation with
 *  @param data          Pointer to the data to calculate the CRC over
 *  @param nr_of_bytes   The amount of bytes to calculate the CRC over
 *   
 *  @return px_crc_t        The resultant CRC over the group of bytes
 */
px_crc_t px_crc_update_data(px_crc_t crc, const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_CRC_H__
