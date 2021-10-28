#ifndef __PX_GPIO_INIT_PORT_B_H__
#define __PX_GPIO_INIT_PORT_B_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_b.h : Macros to auto calculate init values for GPIO port B
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
// Port B defaults
#ifndef PX_GPIO_B0
#define PX_GPIO_B0 PX_GPIO(B, 0,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B1
#define PX_GPIO_B1 PX_GPIO(B, 1,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B2
#define PX_GPIO_B2 PX_GPIO(B, 2,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B3
#define PX_GPIO_B3 PX_GPIO(B, 3,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B4
#define PX_GPIO_B4 PX_GPIO(B, 4,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B5
#define PX_GPIO_B5 PX_GPIO(B, 5,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B6
#define PX_GPIO_B6 PX_GPIO(B, 6,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B7
#define PX_GPIO_B7 PX_GPIO(B, 7,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B8
#define PX_GPIO_B8 PX_GPIO(B, 8,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B9
#define PX_GPIO_B9 PX_GPIO(B, 9,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B10
#define PX_GPIO_B10 PX_GPIO(B, 10, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B11
#define PX_GPIO_B11 PX_GPIO(B, 11, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B12
#define PX_GPIO_B12 PX_GPIO(B, 12, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B13
#define PX_GPIO_B13 PX_GPIO(B, 13, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B14
#define PX_GPIO_B14 PX_GPIO(B, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_B15
#define PX_GPIO_B15 PX_GPIO(B, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif

// Mode init values
#define GPIOB_MODER_VAL \
      PX_GPIO_REG_MODER_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B7)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_B15)

// Output type init values
#define GPIOB_OTYPER_VAL \
      PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B7)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_B15)

// Output speed init values
#define GPIOB_OSPEEDR_VAL \
      PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B7)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_B15)

// Pull up / pull-down register init values
#define GPIOB_PUPDR_VAL \
      PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B7)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_B15)

// Output data register init values
#define GPIOB_ODR_VAL \
      PX_GPIO_REG_ODR_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B7)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_B15)

// Alternative function register init values
#define GPIOB_AFRL_VAL \
      PX_GPIO_REG_AFRL_INIT(PX_GPIO_B0)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B1)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B2)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B3)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B4)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B5)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B6)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_B7)  \

#define GPIOB_AFRH_VAL \
      PX_GPIO_REG_AFRH_INIT(PX_GPIO_B8)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B9)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B10) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B11) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B12) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B13) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B14) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_B15)

static const px_gpio_port_init_t px_gpio_port_b_init =
{
    GPIOB,
    GPIOB_MODER_VAL, 
    GPIOB_OTYPER_VAL, 
    GPIOB_OSPEEDR_VAL, 
    GPIOB_PUPDR_VAL, 
    GPIOB_ODR_VAL, 
    GPIOB_AFRL_VAL, 
    GPIOB_AFRH_VAL
};

#ifdef __cplusplus
}
#endif

#endif
