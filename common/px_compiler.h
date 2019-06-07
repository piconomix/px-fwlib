#ifndef __PX_COMPILER_H__
#define __PX_COMPILER_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2011 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_compiler.h : Compiler detection and compiler specific definitions
    Author(s):      Pieter Conradie
    Creation Date:  2011-03-01

============================================================================= */

/** 
 *  @ingroup COMMON
 *  @defgroup PX_COMPILER px_compiler.h : Compiler detection and compiler specific definitions
 *  
 *  This module detects and caters for differences in compilers. 
 *   
 *  Reference: 
 *  - [Pre-defined C/C++ Compiler Macros](https://sourceforge.net/p/predef/wiki/Home/)
 *  
 *  File(s):
 *  - common/px_compiler.h
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____DEFINITIONS__________________________________________________________ */
// Compiler detection:
#if defined(__GNUC__) && !defined(__CC_ARM)
    /// GCC Compiler
   #define PX_COMPILER_GCC
#elif defined(_MSC_VER)
    /// Microsoft Visual C++ Compiler
   #define PX_COMPILER_MSVC   
#endif

// Compiler target detection:
#if defined(__GNUC__) && defined(__AVR__)
    /// GCC AVR Compiler
   #define PX_COMPILER_GCC_AVR
#elif defined(__18CXX)
    /// MPLAB XC8 Compiler
   #define PX_COMPILER_MICROCHIP_XC8
#elif defined(__PIC24F__)
    /// MPLAB XC16 Compiler
   #define PX_COMPILER_MICROCHIP_XC16
#elif defined(__GNUC__) && defined(__arm__) && !defined(__CC_ARM)
    /// GCC ARM Compiler
   #define PX_COMPILER_GCC_ARM
#elif defined(__CC_ARM)
    /// ARM Real View C Compiler
    #define PX_COMPILER_ARM_CC
#elif defined(__ICCARM__)
    /// IAR ARM Compiler
    #define PX_COMPILER_ICC_ARM
#elif defined(_MSC_VER) && defined(_M_ARM)
    /// Microsoft Visual C++ Compiler for ARM
    #define PX_COMPILER_MSVC_ARM
#else
    /// Unknown
    #define PX_COMPILER_UNKNOWN
#endif
    
/*
 *  If the standard types (as defined in <stdint.h> and <stdbool.h>) are missing
 *  from standard C library, then __PX_STD_TYPES_ABSENT__ can be #defined to
 *  provide them.
 */
#if 0
#define __PX_STD_TYPES_ABSENT__
#endif

#ifdef PX_COMPILER_GCC_AVR
#include <avr/interrupt.h>
#define px_interrupts_enable()         sei()
#define px_interrupts_disable()        cli()
#ifndef px_nop
#define px_nop()                       __asm__ __volatile__("nop\n\t"::)
#endif
#endif

#ifdef PX_COMPILER_MICROCHIP_XC16
#include <xc.h>
#define px_interrupts_enable()         do{SRbits.IPL = 0;} while(0)
#define px_interrupts_disable()        do{SRbits.IPL = 7;} while(0)
#ifndef px_nop
#define px_nop()                       __asm__ __volatile__("nop\n\t"::)
#endif
#endif

#if defined(PX_COMPILER_GCC_ARM)
#define PX_ATTR_RAMFUNC                __attribute__((section(".RamFunc")))
#endif

#if defined(PX_COMPILER_GCC_ARM) || defined (PX_COMPILER_ARM_CC)
#define px_interrupts_enable()      __enable_irq()
#define px_interrupts_disable()     __disable_irq()
#endif

#if defined(PX_COMPILER_GCC) || defined (PX_COMPILER_ARM_CC)
// See https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
#define PX_ATTR_CONST                       __attribute__ ((const))
#define PX_ATTR_NORETURN                    __attribute__ ((noreturn))
#define PX_ATTR_NAKED                       __attribute__ ((naked))
#define PX_ATTR_ALWAYS_INLINE               __attribute__ ((always_inline))
#define PX_ATTR_UNUSED                      __attribute__ ((unused))
#define PX_ATTR_SECTION(section_str)        __attribute__ ((section (section_str)))
#define PX_ATTR_FORMAT_ARG(str_index)       __attribute__ ((format_arg(str_index)))
#define PX_ATTR_PACKED                      __attribute__ ((packed))
#define PX_ATTR_ALIGNED(a)                  __attribute__((__aligned__(a)))

#else

#define PX_ATTR_CONST
#define PX_ATTR_NORETURN
#define PX_ATTR_NAKED
#define PX_ATTR_ALWAYS_INLINE
#define PX_ATTR_UNUSED
#define PX_ATTR_SECTION(section_str)
#define PX_ATTR_FORMAT_ARG(str_index)
#define PX_ATTR_PACKED
#define PX_ATTR_ALIGNED(a)

#endif


/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

/// @}

#endif // #ifndef __PX_COMPILER_H__
