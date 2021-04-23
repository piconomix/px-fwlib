#ifndef __PX_GPIO_INIT_PORT_C_H___
#define __PX_GPIO_INIT_PORT_C_H___
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_c.h : GPIO map to register init values
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

// PORTC init value
#define GPIO_PORTC_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_C0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_C7)
      
// DDRC init value
#define GPIO_DDRC_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_C0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_C7)

#ifdef __cplusplus
}
#endif

#endif
