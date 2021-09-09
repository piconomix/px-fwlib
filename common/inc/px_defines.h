#ifndef __PX_DEFINES_H__
#define __PX_DEFINES_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_defines.h : Common definitions
    Author(s):      Pieter Conradie
    Creation Date:  2008-02-05

============================================================================= */
/** 
 *  @ingroup COMMON
 *  @defgroup PX_DEFINES px_defines.h : Common definitions
 *  
 *  Definition of standard types, boolean, scope and utility macros.
 *  
 *  File:
 *  - common/inc/px_defines.h
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include "px_compiler.h"

#ifndef __PX_STD_TYPES_ABSENT__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#endif

/* _____PROJECT INCLUDES_____________________________________________________ */

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#ifndef NULL
/// NULL pointer
#define NULL 0
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
#ifdef __PX_STD_TYPES_ABSENT__
// Provide standard types if absent from standard C library
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned long      uint32_t;
typedef signed long        int32_t;
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;

#ifndef _Bool
#define _Bool uint8_t
#endif
#ifndef bool
#define bool _Bool
#endif
#ifndef false
#define false  0
#endif
#ifndef true
#define true   (!false)
#endif

#endif // #ifdef __PX_STD_TYPES_ABSENT__

/* _____MACROS_______________________________________________________________ */
/// @name Minimum and maximum of standard types
//@{
#define PX_U8_MIN   0x00            ///< Minimum of unsigned 8-bit value
#define PX_U8_MAX   0xff            ///< Maximum of unsigned 8-bit value
#define PX_U16_MIN  0x0000          ///< Minimum of unsigned 16-bit value
#define PX_U16_MAX  0xffff          ///< Maximum of unsigned 16-bit value
#define PX_U32_MIN  0x00000000      ///< Minimum of unsigned 32-bit value
#define PX_U32_MAX  0xffffffff      ///< Maximum of unsigned 32-bit value
#define PX_S8_MIN   (-0x80)         ///< Minimum of signed 8-bit value
#define PX_S8_MAX   0x7f            ///< Maximum of signed 8-bit value
#define PX_S16_MIN  (-0x8000)       ///< Minimum of signed 16-bit value
#define PX_S16_MAX  0x7fff          ///< Maximum of signed 16-bit value
#define PX_S32_MIN  (-0x80000000)   ///< Minimum of signed 32-bit value
#define PX_S32_MAX  0x7fffffff      ///< Maximum of signed 32-bit value
//@}

/**
 * @name Type size and sign macros
 *  
 * Source: [Catching Integer Overflows in C](http://www.fefe.de/intof.html)
 */
//@{
///@cond INTERNAL
#define __PX_HALF_MAX_SIGNED(type)  ((type)1 << (sizeof(type)*8 - 2))
#define __PX_MAX_SIGNED(type)       (__PX_HALF_MAX_SIGNED(type) - 1 + __PX_HALF_MAX_SIGNED(type))
#define __PX_MIN_SIGNED(type)       (-1 - __PX_MAX_SIGNED(type))
///@endcond

/// Test if a type is signed or unsigned
#define PX_TYPE_IS_SIGNED(type)     ((type)-1 < 1)

/**
 *  Return the minimum of a type.
 *  
 *  For an unsigned type it is zero.
 *  For a signed type this is the largest negative value
 */
#define PX_MIN_OF_TYPE(type)    (PX_TYPE_IS_SIGNED(type) ? __PX_MIN_SIGNED(type) : (type)0)

///  Return the maximum of a type.
#define PX_MAX_OF_TYPE(type)    ((type) ~PX_MIN_OF_TYPE(type))
//@}

/// @name Concatenation macros
//@{
#define _PX_CONCAT(x, y) x ## y

/** 
 *  Recursive token concatenation macro.
 *  
 *  Example:
 *      @code{.c}
 *      #define XY       123456
 *      #define TOKEN1   X
 *      #define TOKEN2   Y
 *      #define TOKEN1_2 CONCAT(TOKEN1,TOKEN2)
 *  
 *      int i = TOKEN1_2;
 *      @endcode
 *  
 *  Preprocessor steps:
 *  
 *  1. TOKEN1_2 is replaced with CONCAT(TOKEN1,TOKEN2)
 *  2. CONCAT(TOKEN1,TOKEN2) is replaced with _CONCAT(X,Y)
 *  3. _CONCAT(X,Y) is replaced with XY
 *  4. XY is replaced with 123456
 *  
 *  Compiler step:
 *  
 *  1. An integer variable called "i" is created and initialised with 123456
 */
#define PX_CONCAT(x, y) _PX_CONCAT(x, y)
//@}

/// @name Stringify macros
//@{
#define _PX_STRINGIFY(x) #x

/** 
 *  Recursive stringify macro.
 *  
 *  Example:
 *      @code{.c}
 *      #define VERSION_MAJOR 2
 *      #define VERSION_MINOR 12
 *  
 *      printf("Version " PX_STRINGIFY(VERSION_MAJOR) "." PX_STRINGIFY(VERSION_MINOR)); 
 *      @endcode
 */
#define PX_STRINGIFY(x) _PX_STRINGIFY(x)
//@}

/// @name Bit manipulation macros useful for example to manipulate GPIO pins
//@{
/// Set a bit (1)
#define PX_BIT_SET_HI(var, bit)             do { (var) |=  (1ul << (bit)); } while(0)

/// Clear a bit (0)
#define PX_BIT_SET_LO(var, bit)             do { (var) &= ~(1ul << (bit)); } while(0)

/// Toggle a bit
#define PX_BIT_TOGGLE(var, bit)             do { (var) ^=  (1ul << (bit)); } while(0)

/// Test if a bit is set (1?)
#define PX_BIT_IS_HI(var, bit)              (((var) & (1ul << (bit))) != 0)

/// Test if a bit is cleared (0?)
#define PX_BIT_IS_LO(var, bit)              (((var) & (1ul << (bit))) == 0)

/// Wait until a bit is set
#define PX_WAIT_UNTIL_BIT_IS_HI(var, bit)   do { ; } while(PX_BIT_IS_LO((var), (bit)))

/// Wait until a bit is cleared
#define PX_WAIT_UNTIL_BIT_IS_LO(var, bit)   do { ; } while(PX_BIT_IS_HI((var), (bit)))
//@}

/// @name Bit mask macros
//@{
/// Clear bits (set bits to 0)
#define PX_BIT_MASK_SET_LO(var, bit_mask, shift) \
    do { (var) &= ~((bit_mask) << (shift)); } while(0)

/// Set bits (set bits to 1)
#define PX_BIT_MASK_SET_HI(var, bit_mask, shift) \
    do { (var) |= ((bit_mask) << (shift)); } while(0)

/// Mask bits (set to 0) and then set to value
#define PX_BIT_MASK_SET_VAL(var, bit_mask, val, shift) \
    do { (var) = ((var) & (~((bit_mask) << (shift)))) | ((val) << (shift)); } while(0)
//@}

/// @name Byte extraction macros
//@{
/// Extract the high 4 bits of a 8-bit value
#define PX_U8_HI4(data)     ((uint8_t)(((data) >> 4) & 0x0f))

/// Extract the low 4 bits of a 8-bit value
#define PX_U8_LO4(data)     ((uint8_t)((data) & 0x0f))

/// Extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define PX_U16_HI8(data)    ((uint8_t)(((data) >> 8) & 0xff))

/// Extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define PX_U16_LO8(data)    ((uint8_t)((data) & 0xff))

/// Extract the high 8 bits (bits 31..24) of a 32-bit value
#define PX_U32_HI8(data)    ((uint8_t)(((data) >> 24) & 0xff))

/// Extract the middle high 8 bits (bits 23..16) of a 32-bit value
#define PX_U32_MH8(data)    ((uint8_t)(((data) >> 16) & 0xff))

/// Extract the middle low 8 bits (bits 15..8) of a 32-bit value
#define PX_U32_ML8(data)    ((uint8_t)(((data) >> 8) & 0xff))

/// Extract the low 8 bits (bits 7..0) of a 32-bit value
#define PX_U32_LO8(data)    ((uint8_t)((data) & 0xff))

/// Extract the high 16 bits (bits 31..16) of a 32-bit value
#define PX_U32_HI16(data)   ((uint16_t)(((data) >> 16) & 0xffff))

/// Extract the low 16 bits (bits 15..0) of a 32-bit value
#define PX_U32_LO16(data)   ((uint16_t)((data) & 0xffff))
//@}

/// @name Bit concatenation macros
//@{
/// Concatenate 8 bits to form 8-bit value
#define PX_U8_CONCAT_U1(b7, b6, b5, b4, b3, b2, b1, b0) \
                            (   (((b7) & 0x01) << 7) | (((b6) & 0x01) << 6) | (((b5) & 0x01) << 5) | (((b4) & 0x01) << 4) \
                              | (((b3) & 0x01) << 3) | (((b2) & 0x01) << 2) | (((b1) & 0x01) << 1) | (((b0) & 0x01) << 0) )

//@}

/// @name Byte concatenation macros
//@{
/// Concatenate 2 x 4 bits to form 8-bit value
#define PX_U8_CONCAT_U4(hi4, lo4) \
                            (   (((uint8_t)((hi4) & 0x0f)) << 4) \
                              |  ((uint8_t)((lo4) & 0x0f))     )

/// Concatenate 2 x 8 bits to form 16-bit value
#define PX_U16_CONCAT_U8(hi8, lo8) \
                            (   (((uint16_t)((hi8) & 0xff)) << 8) \
                              |  ((uint16_t)((lo8) & 0xff))     )

/// Concatenate 4 x 8 bits to form 32-bit value
#define PX_U32_CONCAT_U8(hi8, mh8, ml8, lo8) \
                            (   (((uint32_t)((hi8) & 0xff)) << 24) \
                              | (((uint32_t)((mh8) & 0xff)) << 16) \
                              | (((uint32_t)((ml8) & 0xff)) << 8 ) \
                              |  ((uint32_t)((lo8) & 0xff))        )
//@}

/// @name General utility macros
//@{

/// Calculate unsigned division with rounding to nearest integer value
#define PX_UDIV_ROUND(dividend, divisor)    (((dividend) + ((divisor) / 2)) / (divisor))

/// Calculate unsigned division with rounding to nearest integer value
#define PX_UDIV_ROUNDUP(dividend, divisor)  (((dividend) + ((divisor) - 1)) / (divisor))

/// Calculate signed division with rounding to nearest integer value
#define PX_SDIV_ROUND(dividend, divisor)    (((dividend < 0) ^ (divisor < 0)) ? (((dividend) - ((divisor) / 2)) / (divisor)) : (((dividend) + ((divisor) / 2)) / (divisor)))

/// Calculate the number of items in an array
#define PX_SIZEOF_ARRAY(array)              (sizeof(array) / sizeof((array)[0]))

/// Is value a power of two (1, 2, 4, 8, 16, ...)?
#define PX_VAL_IS_PWR_OF_TWO(value)         (((value) & ((value) - 1)) == 0)

/// Swap the value of two variables
#define PX_SWAP(type_t, i , j)              do { type_t k = i; i = j; j = k; } while (0)

/// Maximum of two values
#define PX_MAX(i, j)                        (i > j ? i : j)

/// Minimum of two values
#define PX_MIN(i, j)                        (i < j ? i : j)

//@}

/// @}
#ifdef __cplusplus
}
#endif

#endif
