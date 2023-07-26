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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
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
 *  - utils/inc/px_xtea.h
 *  - utils/src/px_xtea.c
 *  
 *  @see http://en.wikipedia.org/wiki/XTEA
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Key size in bytes
#define XTEA_KEY_SIZE_BYTES     16
/// Data block size in bytes
#define XTEA_BLOCK_SIZE_BYTES   8

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
 *  Encrypt a data buffer using Electronic Codebook (ECB) block cipher operation.
 *  
 *  If the data_in is not an exact multiple of the block size (8 bytes), zero
 *  padding is used for the last block. The data_out buffer MUST be a multiple
 *  of 8 bytes.
 *  
 *  The data can be encrypted in place by setting both the data_in and data_out
 *  pointer to the start of the buffer to be encrypted.
 *  
 *  @param data_in          Pointer to data buffer to be encrypted
 *  @param data_out         Pointer to data buffer to contain encrypted data
 *  @param nr_of_bytes      Number of bytes to encrypt
 *  
 *  @returns size_t         Number of bytes encrypted (multiple of block size)
 */
size_t px_xtea_encrypt_data_ecb(const uint8_t * data_in,
                                uint8_t *       data_out, 
                                size_t          nr_of_bytes);

/**
 *  Decrypt 64-bit data with 128-bit key. 
 *   
 *  @param data     2 x 32-bit values 
 */
void px_xtea_decrypt(uint32_t data[2]);

/**
 *  Decrypt a data buffer using Electronic Codebook (ECB) block cipher operation.
 *  
 *  If the data_in is not an exact multiple of the block size (8 bytes), zero
 *  padding is used for the last block. The data_out buffer MUST be a multiple
 *  of 8 bytes.
 *  
 *  The data can be decrypted in place by setting both the data_in and data_out
 *  pointer to the start of the buffer to be decrypted.
 *  
 *  @param data_in          Pointer to data buffer to be decrypted
 *  @param data_out         Pointer to data buffer to contain decrypted data
 *  @param nr_of_bytes      Number of bytes to decrypt
 *  
 *  @returns size_t         Number of bytes decrypted (multiple of block size)
 */
size_t px_xtea_decrypt_data_ecb(const uint8_t * data_in, 
                                uint8_t *       data_out,
                                size_t          nr_of_bytes);

/**
 *  Set Initialization Vector (IV) for Cipher Block Chaining (CBC)
 *
 *  @param iv     Initialization Vector
 */
void px_xtea_init_cbc(const uint8_t iv[8]);

/**
 *  Encrypt a data buffer using Cipher Block Chaining (CBC) block cipher operation.
 *
 *  The data in and out buffer must be MUST be a multiple of 8 bytes.
 *
 *  The data can be encrypted in place by setting both the data_in and data_out
 *  pointer to the start of the buffer to be encrypted.
 *
 *  @param data_in          Pointer to data buffer to be encrypted
 *  @param data_out         Pointer to data buffer to contain encrypted data
 *  @param nr_of_bytes      Number of bytes to encrypt (multiple of block size)
 *
 */
/**
 *  Encrypt a data buffer using Cipher Block Chaining (CBC) block cipher operation.
 *
 *  The data in and out buffer MUST be a multiple of 8 bytes.
 *
 *  The data can be encrypted in place by setting both the data_in and data_out
 *  pointer to the start of the buffer to be encrypted.
 *
 *  @param data_in          Pointer to data buffer to be encrypted
 *  @param data_out         Pointer to data buffer to contain encrypted data
 *  @param nr_of_bytes      Number of bytes to encrypt (multiple of block size)
 *
 */
void px_xtea_encrypt_data_cbc(const uint8_t * data_in,
                              uint8_t *       data_out,
                              size_t          nr_of_bytes);

/**
 *  Decrypt a data buffer using Cipher Block Chaining (CBC) block cipher operation.
 *
 *  The data in and out buffer MUST be a multiple of 8 bytes.
 *
 *  The data can be encrypted in place by setting both the data_in and data_out
 *  pointer to the start of the buffer to be encrypted.
 *
 *  @param data_in          Pointer to data buffer to be encrypted
 *  @param data_out         Pointer to data buffer to contain encrypted data
 *  @param nr_of_bytes      Number of bytes to encrypt (multiple of block size)
 *
 */
void px_xtea_decrypt_data_cbc(const uint8_t * data_in,
                              uint8_t *       data_out,
                              size_t          nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
