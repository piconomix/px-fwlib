#ifndef __PX_GPIO_INIT_PORT_B_H___
#define __PX_GPIO_INIT_PORT_B_H___
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_b.h : GPIO map to register init values
    Author(s):      Pieter Conradie
    Creation Date:  2020-05-04

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

// PORTB init value
#define GPIO_PORTB_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_B0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_B7)
      
// DDRB init value    
#define GPIO_DDRB_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_B0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_B7)

#ifdef __cplusplus
}
#endif

#endif
