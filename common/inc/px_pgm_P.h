#ifndef __PX_PGM_P_H__
#define __PX_PGM_P_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_pgm_P.h : Support for constant data stored in program memory
    Author(s):      Pieter Conradie
    Creation Date:  2013-07-04

============================================================================= */

/** 
 *  @ingroup COMMON
 *  @defgroup PX_PGM_P px_pgm_P.h : Support for constant data stored in program memory
 *  
 *  RAM saving support for architectures that require methods to store and read
 *  constant data in program memory.
 *  
 *  File(s):
 *  - common/px_pgm_P.h
 *  
 *  This utility module attempts to provide a standardised method to store and
 *  read constant data in program memory for devices with separate program and
 *  data memory spaces, most notably the Microchip AVR architecture.
 *  
 *  For a great introduction, read:
 *  - [AVR-GCC and the PROGMEM Attribute](http://www.fourwalledcubicle.com/AVRArticles.php)
 *    by Dean Camera.
 *  - [avr-libc: Data in Program Space](http://www.nongnu.org/avr-libc/user-manual/pgmspace.html)
 *  
 *  Constant data that must be stored in program memory is tagged with 
 *  the PX_ATTR_PGM attribute.
 *  
 *  The suffix "_P" convention is used to indicate that the function reads data
 *  stored in program memory, e.g. printf_P() vs. printf(). It is also used to
 *  indicate that a source file has been optimised to use const data stored in
 *  program memory, e.g. "px_cli_P.c" vs. "px_cli.c".
 *  
 *  The "PX_PGM_STR()" macro is used for inline declaration of a string in
 *  program memory.
 *  See <avr/pgmspace.h> from [AVR-Libc](http://www.nongnu.org/avr-libc/)
 *  
 *  For extra convenience, all-capital macros have been provided to call a
 *  program memory function AND inline declare a string in program memory:
 *  
 *      @code{.c}
 *      // Call progmem version of 'printf' and declare string in program memory
 *      printf_P(PSTR("Hello world!\n"));
 *  
 *      // Use variable argument convenience macro
 *      PX_PRINTF_P("Hello world!\n");
 *      @endcode
 *  
 *  This module also has the ability to revert to normal RAM storage for
 *  architectures that do not require it, or if PX_PGM_P_USE_RAM is defined.
 *  
 *  Example:
 *  
 *      @code{.c}
 *      #include "px_pgm_P.h"
 *  
 *      // Program memory table for the number of days in each month (non leap year)
 *      const uint8_t px_rtc_month_day_table[] PX_ATTR_PGM = 
 *      {  
 *          0,  // Invalid month
 *         31,  // January
 *         28,  // February
 *         31,  // March
 *         30,  // April
 *         31,  // May
 *         30,  // June
 *         31,  // July
 *         31,  // August
 *         30,  // September
 *         31,  // October
 *         30,  // November
 *         31   // December
 *      };
 *  
 *      // String stored in program memory
 *      const char calender_str[] PX_ATTR_PGM = "Gregorian Calender\n";
 *  
 *      void report_days(uint8_t month)
 *      {
 *          uint8_t days_in_month = px_pgm_rd_u8(&px_rtc_month_day_table[month]);
 *  
 *          printf_P(calender_str);
 *          PX_PRINTF_P("Days in month = %d\n", days_in_month);
 *      }
 *      @endcode
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_compiler.h"

#if defined(PX_COMPILER_GCC_AVR) && !defined(PX_PGM_USE_RAM)
#include <avr/pgmspace.h>
#else
#include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
#if defined(PX_COMPILER_GCC_AVR) && !defined(PX_PGM_P_USE_RAM)

/// Mark a variable as stored in program memory
#define PX_ATTR_PGM __attribute__((__progmem__))

/**
 *  Inline declaration of a string in program memory.
 *  
 *  This definition is copied from <avr/pgmspace.h> in
 *  [AVR-Libc](http://www.nongnu.org/avr-libc/)
 */
#define PX_PGM_STR(s) \
    (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))

/// Read a uint8_t from program memory
#define px_pgm_rd_u8(adr)           ((uint8_t)pgm_read_byte(adr))
/// Read a uint16_t from program memory
#define px_pgm_rd_u16(adr)          ((uint16_t)pgm_read_word(adr))
/// Read a uint32_t from program memory
#define px_pgm_rd_u32(adr)          ((uint32_t)pgm_read_dword(adr))
/// Read a int8_t from program memory
#define px_pgm_rd_s8(adr)           ((int8_t)pgm_read_byte(adr))
/// Read a int16_t from program memory
#define px_pgm_rd_s16(adr)          ((int16_t)pgm_read_word(adr))
/// Read a int32_t from program memory
#define px_pgm_rd_s32(adr)          ((int32_t)pgm_read_dword(adr))
/// Read a void pointer from program memory
#define px_pgm_rd_void_ptr(adr)     ((void*)pgm_read_word((adr)))
/// Read a char from program memory
#define px_pgm_rd_char(adr)         ((char)pgm_read_byte((adr)))

/// Variable argument macro that calls @b printf_P with format string declared in program memory
#define PX_PRINTF_P(format, ...)    printf_P(PX_PGM_STR(format), ## __VA_ARGS__)

#else
// Use normal RAM declarations and functions to read data
#define PX_ATTR_PGM

#define PX_PGM_STR(s)                       (s)

#define px_pgm_rd_u8(adr)                   (*((const uint8_t *)adr))
#define px_pgm_rd_u16(adr)                  (*((const uint16_t *)adr))
#define px_pgm_rd_u32(adr)                  (*((const uint32_t *)adr))
#define px_pgm_rd_s8(adr)                   (*((const int8_t *)adr))
#define px_pgm_rd_s16(adr)                  (*((const int16_t *)adr))
#define px_pgm_rd_s32(adr)                  (*((const int32_t *)adr))
#define px_pgm_rd_void_ptr(adr)             (*(const void *)(adr))
#define px_pgm_rd_char(adr)                 (*((const char *)adr))

#define memchr_P(src, val, len)             memchr((src), (val), (len))
#define memcmp_P(s1, s2, len)               memcmp((s1), (s2), (len))
#define memccpy_P(dest, src, val, len)      memccpy((dest), (src), (val), (len))
#define memcpy_P(dest, src, len)            memcpy((dest), (src), (len))
#define memmem_P(s1, len1, s2, len2)        memmem((s1), (len1), (s2), (len2))
#define memrchr_P(src, val, len)            memrchr((src), (val), (len))
#define strcat_P(dest, src)                 strcat((dest), (src))
#define strchr_P(src, val)                  strchr((src), (val))
#define strchrnul_P(s, c)                   strchrnul((s), (c))
#define strcmp_P(s1, s2)                    strcmp((s1), (s2))
#define strcpy_P(dest, src)                 strcpy((dest), (src))
#define strcasecmp_P(s1, s2)                strcasecmp((s1), (s2))
#define strcasestr_P(s1, s2)                strcasestr((s1), (s2))
#define strcspn_P(s, reject)                strcspn((s), (reject))
#define strlcat_P(dst, src, siz)            strlcat((dst), (src), (siz))
#define strlcpy_P(dst, src, siz)            strlcpy((dst), (src), (siz))
#define strnlen_P(src, len)                 strnlen((src), (len))
#define strncmp_P(s1, s2, len)              strncmp((s1), (s2), (len))
#define strncasecmp_P(s1, s2, len)          strncasecmp((s1), (s2), (len))
#define strncat_P(dest, src, len)           strncat((dest), (src), (len))
#define strncpy_P(dest, src, len)           strncpy((dest), (src), (len))
#define strpbrk_P(s, accept)                strpbrk((s), (accept))
#define strrchr_P(src, val)                 strrchr((src), (val))
#define strsep_P(sp, delim)                 strsep((sp), (delim))
#define strspn_P(s, accept)                 strspn((s), (accept))
#define strstr_P(s1, s2)                    strstr((s1), (s2))
#define strtok_P(s, delim)                  strtok((s), (delim))
#define strtok_rP(string, delim, last)      strtok_r((string), (delim), (last))
#define strlen_P(src)                       strlen((src))

#define vsnprintf_P(__s, __n, __fmt, ...)   vsnprintf((__s), (__n), (__fmt), ## __VA_ARGS__)

#define PX_PRINTF_P(fmt, ...)               printf(fmt, ## __VA_ARGS__)

#endif

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_PGM_P_H__
