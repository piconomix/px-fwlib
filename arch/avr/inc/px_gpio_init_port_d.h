#ifndef __PX_GPIO_INIT_PORT_D_H___
#define __PX_GPIO_INIT_PORT_D_H___
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_d.h : GPIO map to register init values
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

// PORTD init value
#define GPIO_PORTD_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_D0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_D7)
      
// DDRD init value
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

#endif
