/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2011 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_compiler.h : Compiler detection and compiler specific definitions
    Author(s):      Pieter Conradie
    Creation Date:  2022-02-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_compiler.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#ifdef PX_COMPILER_GCC
#warning "GCC Compiler"
#endif
#ifdef PX_COMPILER_MSVC
#warning "Microsoft Visual C++ Compiler"
#endif
#ifdef PX_COMPILER_GCC_AVR
#warning "GCC AVR Compiler"
#endif
#ifdef PX_COMPILER_MICROCHIP_XC8
#warning "MPLAB XC8 Compiler"
#endif
#ifdef PX_COMPILER_MICROCHIP_XC16
#warning "MPLAB XC16 Compiler"
#endif
#ifdef PX_COMPILER_GCC_ARM
#warning "GCC ARM Compiler"
#endif
#ifdef PX_COMPILER_ARM_CC
#warning "ARM Real View C Compiler"
#endif
#ifdef PX_COMPILER_ICC_ARM
#warning "IAR ARM Compiler"
#endif
#ifdef PX_COMPILER_MSVC_ARM
#warning "Microsoft Visual C++ Compiler for ARM"
#endif
#ifdef PX_COMPILER_TI_ARM
#warning "Texas Instruments ARM Compiler"
#endif
#ifdef PX_COMPILER_UNKNOWN
#warning "Compiler unknown"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */

