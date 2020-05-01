#ifndef __PX_GPIO_INIT_PORT_A_H__
#define __PX_GPIO_INIT_PORT_A_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_a.h : Macros to auto calculate init values for GPIO port A
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
#ifndef PX_GPIO_A0
#define PX_GPIO_A0 PX_GPIO(A, 0,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A1
#define PX_GPIO_A1 PX_GPIO(A, 1,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A2
#define PX_GPIO_A2 PX_GPIO(A, 2,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A3
#define PX_GPIO_A3 PX_GPIO(A, 3,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A4
#define PX_GPIO_A4 PX_GPIO(A, 4,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A5
#define PX_GPIO_A5 PX_GPIO(A, 5,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A6
#define PX_GPIO_A6 PX_GPIO(A, 6,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A7
#define PX_GPIO_A7 PX_GPIO(A, 7,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A8
#define PX_GPIO_A8 PX_GPIO(A, 8,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A9
#define PX_GPIO_A9 PX_GPIO(A, 9,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A10
#define PX_GPIO_A10 PX_GPIO(A, 10, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A11
#define PX_GPIO_A11 PX_GPIO(A, 11, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A12
#define PX_GPIO_A12 PX_GPIO(A, 12, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A13
#define PX_GPIO_A13 PX_GPIO(A, 13, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A14
#define PX_GPIO_A14 PX_GPIO(A, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_A15
#define PX_GPIO_A15 PX_GPIO(A, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif

// Mode init values
#define GPIOA_MODER_VAL \
      PX_GPIO_REG_MODER_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A7)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_A15)

// Output type init values
#define GPIOA_OTYPER_VAL \
      PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A7)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_A15)

// Output speed init values
#define GPIOA_OSPEEDR_VAL \
      PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A7)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_A15)

// Pull up / pull-down register init values
#define GPIOA_PUPDR_VAL \
      PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A7)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_A15)

// Output data register init values
#define GPIOA_ODR_VAL \
      PX_GPIO_REG_ODR_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A7)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_A15)

// Alternative function register init values
#define GPIOA_AFRL_VAL \
      PX_GPIO_REG_AFRL_INIT(PX_GPIO_A0)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A1)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A2)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A3)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A4)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A5)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A6)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_A7)  \

#define GPIOA_AFRH_VAL \
      PX_GPIO_REG_AFRH_INIT(PX_GPIO_A8)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A9)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A10) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A11) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A12) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A13) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A14) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_A15)

static const px_gpio_port_init_t px_gpio_port_a_init =
{
    GPIOA,
    GPIOA_MODER_VAL, 
    GPIOA_OTYPER_VAL, 
    GPIOA_OSPEEDR_VAL, 
    GPIOA_PUPDR_VAL, 
    GPIOA_ODR_VAL, 
    GPIOA_AFRL_VAL, 
    GPIOA_AFRH_VAL
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GPIO_INIT_PORT_A_H__
