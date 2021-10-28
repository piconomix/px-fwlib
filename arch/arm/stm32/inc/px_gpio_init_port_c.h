#ifndef __PX_GPIO_INIT_PORT_C_H__
#define __PX_GPIO_INIT_PORT_C_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_c.h : Macros to auto calculate init values for GPIO port C
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
// Port C defaults
#ifndef PX_GPIO_C0
#define PX_GPIO_C0 PX_GPIO(C, 0,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C1
#define PX_GPIO_C1 PX_GPIO(C, 1,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C2
#define PX_GPIO_C2 PX_GPIO(C, 2,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C3
#define PX_GPIO_C3 PX_GPIO(C, 3,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C4
#define PX_GPIO_C4 PX_GPIO(C, 4,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C5
#define PX_GPIO_C5 PX_GPIO(C, 5,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C6
#define PX_GPIO_C6 PX_GPIO(C, 6,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C7
#define PX_GPIO_C7 PX_GPIO(C, 7,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C8
#define PX_GPIO_C8 PX_GPIO(C, 8,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C9
#define PX_GPIO_C9 PX_GPIO(C, 9,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C10
#define PX_GPIO_C10 PX_GPIO(C, 10, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C11
#define PX_GPIO_C11 PX_GPIO(C, 11, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C12
#define PX_GPIO_C12 PX_GPIO(C, 12, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C13
#define PX_GPIO_C13 PX_GPIO(C, 13, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C14
#define PX_GPIO_C14 PX_GPIO(C, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_C15
#define PX_GPIO_C15 PX_GPIO(C, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif

// Mode init values
#define GPIOC_MODER_VAL \
      PX_GPIO_REG_MODER_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C7)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_C15)

// Output type init values
#define GPIOC_OTYPER_VAL \
      PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C7)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_C15)

// Output speed init values
#define GPIOC_OSPEEDR_VAL \
      PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C7)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_C15)

// Pull up / pull-down register init values
#define GPIOC_PUPDR_VAL \
      PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C7)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_C15)

// Output data register init values
#define GPIOC_ODR_VAL \
      PX_GPIO_REG_ODR_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C7)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_C15)

// Alternative function register init values
#define GPIOC_AFRL_VAL \
      PX_GPIO_REG_AFRL_INIT(PX_GPIO_C0)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C1)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C2)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C3)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C4)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C5)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C6)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_C7)  \

#define GPIOC_AFRH_VAL \
      PX_GPIO_REG_AFRH_INIT(PX_GPIO_C8)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C9)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C10) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C11) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C12) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C13) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C14) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_C15)

static const px_gpio_port_init_t px_gpio_port_c_init =
{
    GPIOC,
    GPIOC_MODER_VAL, 
    GPIOC_OTYPER_VAL, 
    GPIOC_OSPEEDR_VAL, 
    GPIOC_PUPDR_VAL, 
    GPIOC_ODR_VAL, 
    GPIOC_AFRL_VAL, 
    GPIOC_AFRH_VAL
};

#ifdef __cplusplus
}
#endif

#endif
