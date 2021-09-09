#ifndef __PX_GPIO_INIT_PORT_H_H__
#define __PX_GPIO_INIT_PORT_H_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_h.h : Macros to auto calculate init values for GPIO port H
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
// Port A defaults
#ifndef PX_GPIO_H0
#define PX_GPIO_H0 PX_GPIO(H, 0,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H1
#define PX_GPIO_H1 PX_GPIO(H, 1,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H2
#define PX_GPIO_H2 PX_GPIO(H, 2,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H3
#define PX_GPIO_H3 PX_GPIO(H, 3,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H4
#define PX_GPIO_H4 PX_GPIO(H, 4,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H5
#define PX_GPIO_H5 PX_GPIO(H, 5,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H6
#define PX_GPIO_H6 PX_GPIO(H, 6,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H7
#define PX_GPIO_H7 PX_GPIO(H, 7,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H8
#define PX_GPIO_H8 PX_GPIO(H, 8,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H9
#define PX_GPIO_H9 PX_GPIO(H, 9,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H10
#define PX_GPIO_H10 PX_GPIO(H, 10, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H11
#define PX_GPIO_H11 PX_GPIO(H, 11, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H12
#define PX_GPIO_H12 PX_GPIO(H, 12, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H13
#define PX_GPIO_H13 PX_GPIO(H, 13, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H14
#define PX_GPIO_H14 PX_GPIO(H, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_H15
#define PX_GPIO_H15 PX_GPIO(H, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif

// Mode init values
#define GPIOH_MODER_VAL \
      PX_GPIO_REG_MODER_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H7)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_H15)

// Output type init values
#define GPIOH_OTYPER_VAL \
      PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H7)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_H15)

// Output speed init values
#define GPIOH_OSPEEDR_VAL \
      PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H7)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_H15)

// Pull up / pull-down register init values
#define GPIOH_PUPDR_VAL \
      PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H7)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_H15)

// Output data register init values
#define GPIOH_ODR_VAL \
      PX_GPIO_REG_ODR_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H7)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_H15)

// Alternative function register init values
#define GPIOH_AFRL_VAL \
      PX_GPIO_REG_AFRL_INIT(PX_GPIO_H0)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H1)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H2)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H3)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H4)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H5)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H6)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_H7)  \

#define GPIOH_AFRH_VAL \
      PX_GPIO_REG_AFRH_INIT(PX_GPIO_H8)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H9)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H10) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H11) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H12) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H13) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H14) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_H15)

static const px_gpio_port_init_t px_gpio_port_h_init =
{
    GPIOH,
    GPIOH_MODER_VAL, 
    GPIOH_OTYPER_VAL, 
    GPIOH_OSPEEDR_VAL, 
    GPIOH_PUPDR_VAL, 
    GPIOH_ODR_VAL, 
    GPIOH_AFRL_VAL, 
    GPIOH_AFRH_VAL
};

#ifdef __cplusplus
}
#endif

#endif
