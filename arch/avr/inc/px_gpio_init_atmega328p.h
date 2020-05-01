#ifndef __PX_GPIO_INIT_ATMEGA328P_H___
#define __PX_GPIO_INIT_ATMEGA328P_H___
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_atmega328p.h : GPIO map to register init values
    Author(s):      Pieter Conradie
    Creation Date:  2019-06-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
// Port B defaults
#ifndef PX_GPIO_B0
#define PX_GPIO_B0        PX_GPIO(B, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B1
#define PX_GPIO_B1        PX_GPIO(B, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B2
#define PX_GPIO_B2        PX_GPIO(B, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B3
#define PX_GPIO_B3        PX_GPIO(B, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B4
#define PX_GPIO_B4        PX_GPIO(B, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B5
#define PX_GPIO_B5        PX_GPIO(B, 5, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B6
#define PX_GPIO_B6        PX_GPIO(B, 6, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_B7
#define PX_GPIO_B7        PX_GPIO(B, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
// Port C defaults
#ifndef PX_GPIO_C0
#define PX_GPIO_C0        PX_GPIO(C, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C1
#define PX_GPIO_C1        PX_GPIO(C, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C2
#define PX_GPIO_C2        PX_GPIO(C, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C3
#define PX_GPIO_C3        PX_GPIO(C, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C4
#define PX_GPIO_C4        PX_GPIO(C, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C5
#define PX_GPIO_C5        PX_GPIO(C, 5, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C6
#define PX_GPIO_C6        PX_GPIO(C, 6, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_C7
#define PX_GPIO_C7        PX_GPIO(C, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
// Port D defaults
#ifndef PX_GPIO_D0
#define PX_GPIO_D0        PX_GPIO(D, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D1
#define PX_GPIO_D1        PX_GPIO(D, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D2
#define PX_GPIO_D2        PX_GPIO(D, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D3
#define PX_GPIO_D3        PX_GPIO(D, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D4
#define PX_GPIO_D4        PX_GPIO(D, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D5
#define PX_GPIO_D5        PX_GPIO(D, 5, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D6
#define PX_GPIO_D6        PX_GPIO(D, 6, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_D7
#define PX_GPIO_D7        PX_GPIO(D, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif

// Mode init values
#define GPIO_PORTB_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_B0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B7)
      
#define GPIO_DDRB_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_B0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B7)

#define GPIO_PORTC_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_C0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C7)
      
#define GPIO_DDRC_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_C0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C7)

#define GPIO_PORTD_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_D0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D7)
      
#define GPIO_DDRD_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_D0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_D7)

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GPIO_INIT_ATMEGA328P_H___
