#ifndef __PX_XTEA_H__
#define __PX_XTEA_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2010 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_xtea.h : XTEA (eXtended Tiny Encryption Algorithm)
    Author(s):      Pieter Conradie
    Creation Date:  2010-05-03

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_XTEA px_xtea.h : XTEA (eXtended Tiny Encryption Algorithm)
 *  
 *  Block cipher encryption/decryption algorithm.
 *  
 *  File(s):
 *  - utils/px_xtea.h
 *  - utils/px_xtea.c
 *  
 *  @see http://en.wikipedia.org/wiki/XTEA
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise 128-bit key for encryption/decryption.
 *   
 *  @param key      4 x 32-bit values
 */
void px_xtea_init(const uint32_t key[4]);

/**
 *  Encrypt 64-bit data with 128-bit key.
 *   
 *  @param data     2 x 32-bit values
 */
void px_xtea_encrypt(uint32_t data[2]);

/**
 *  Decrypt 64-bit data with 128-bit key. 
 *   
 *  @param data     2 x 32-bit values 
 */
void px_xtea_decrypt(uint32_t data[2]);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_XTEA_H__
