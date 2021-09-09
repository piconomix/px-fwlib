#ifndef __PX_GPIO_INIT_PORT_D_H__
#define __PX_GPIO_INIT_PORT_D_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio_init_port_d.h : Macros to auto calculate init values for GPIO port D
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
#ifndef PX_GPIO_D0
#define PX_GPIO_D0 PX_GPIO(D, 0,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D1
#define PX_GPIO_D1 PX_GPIO(D, 1,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D2
#define PX_GPIO_D2 PX_GPIO(D, 2,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D3
#define PX_GPIO_D3 PX_GPIO(D, 3,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D4
#define PX_GPIO_D4 PX_GPIO(D, 4,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D5
#define PX_GPIO_D5 PX_GPIO(D, 5,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D6
#define PX_GPIO_D6 PX_GPIO(D, 6,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D7
#define PX_GPIO_D7 PX_GPIO(D, 7,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D8
#define PX_GPIO_D8 PX_GPIO(D, 8,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D9
#define PX_GPIO_D9 PX_GPIO(D, 9,   PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D10
#define PX_GPIO_D10 PX_GPIO(D, 10, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D11
#define PX_GPIO_D11 PX_GPIO(D, 11, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D12
#define PX_GPIO_D12 PX_GPIO(D, 12, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D13
#define PX_GPIO_D13 PX_GPIO(D, 13, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D14
#define PX_GPIO_D14 PX_GPIO(D, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif
#ifndef PX_GPIO_D15
#define PX_GPIO_D15 PX_GPIO(D, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#endif

// Mode init values
#define GPIOD_MODER_VAL \
      PX_GPIO_REG_MODER_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D7)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_MODER_INIT(PX_GPIO_D15)

// Output type init values
#define GPIOD_OTYPER_VAL \
      PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D7)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_OTYPER_INIT(PX_GPIO_D15)

// Output speed init values
#define GPIOD_OSPEEDR_VAL \
      PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D7)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_OSPEEDR_INIT(PX_GPIO_D15)

// Pull up / pull-down register init values
#define GPIOD_PUPDR_VAL \
      PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D7)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_PUPDR_INIT(PX_GPIO_D15)

// Output data register init values
#define GPIOD_ODR_VAL \
      PX_GPIO_REG_ODR_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D7)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_ODR_INIT(PX_GPIO_D15)

// Alternative function register init values
#define GPIOD_AFRL_VAL \
      PX_GPIO_REG_AFRL_INIT(PX_GPIO_D0)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D1)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D2)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D3)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D4)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D5)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D6)  \
    + PX_GPIO_REG_AFRL_INIT(PX_GPIO_D7)  \

#define GPIOD_AFRH_VAL \
      PX_GPIO_REG_AFRH_INIT(PX_GPIO_D8)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D9)  \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D10) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D11) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D12) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D13) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D14) \
    + PX_GPIO_REG_AFRH_INIT(PX_GPIO_D15)

static const px_gpio_port_init_t px_gpio_port_d_init =
{
    GPIOD,
    GPIOD_MODER_VAL, 
    GPIOD_OTYPER_VAL, 
    GPIOD_OSPEEDR_VAL, 
    GPIOD_PUPDR_VAL, 
    GPIOD_ODR_VAL, 
    GPIOD_AFRL_VAL, 
    GPIOD_AFRH_VAL
};

#ifdef __cplusplus
}
#endif

#endif
