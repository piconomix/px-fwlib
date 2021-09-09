#ifndef __PX_GPIO_INIT_PORT_A_H___
#define __PX_GPIO_INIT_PORT_A_H___
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_a.h : GPIO map to register init values
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
// Port A defaults
#ifndef PX_GPIO_A0
#define PX_GPIO_A0        PX_GPIO(A, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A1
#define PX_GPIO_A1        PX_GPIO(A, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A2
#define PX_GPIO_A2        PX_GPIO(A, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A3
#define PX_GPIO_A3        PX_GPIO(A, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A4
#define PX_GPIO_A4        PX_GPIO(A, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A5
#define PX_GPIO_A5        PX_GPIO(A, 5, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A6
#define PX_GPIO_A6        PX_GPIO(A, 6, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif
#ifndef PX_GPIO_A7
#define PX_GPIO_A7        PX_GPIO(A, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_LO)
#endif

// PORTA init value
#define GPIO_PORTA_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_A0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A5) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A6) \
            + PX_GPIO_INIT_PORT(PX_GPIO_A7)
      
// DDRA init value
#define GPIO_DDRA_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_A0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A5) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A6) \
            + PX_GPIO_INIT_DDR(PX_GPIO_A7)

#ifdef __cplusplus
}
#endif

#endif
