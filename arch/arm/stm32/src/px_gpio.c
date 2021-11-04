/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio.h : ST STM32 GPIO driver
    Author(s):      Pieter Conradie
    Creation Date:  2017-11-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gpio.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_gpio");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gpio_open2(px_gpio_handle_t * gpio, 
                   GPIO_TypeDef *     gpio_base_reg,
                   uint8_t            pin)
{
    uint32_t pin_bit_mask  = ((uint32_t)1) << pin;

    gpio->gpio_base_reg = gpio_base_reg;
    gpio->pin           = pin;
    gpio->mode          = (px_gpio_mode_t)LL_GPIO_GetPinMode(gpio_base_reg, pin_bit_mask);
    gpio->otype         = (px_gpio_otype_t)LL_GPIO_GetPinOutputType(gpio_base_reg, pin_bit_mask);
    gpio->ospeed        = (px_gpio_ospeed_t)LL_GPIO_GetPinSpeed(gpio_base_reg, pin_bit_mask);
    gpio->pull          = (px_gpio_pull_t)LL_GPIO_GetPinPull(gpio_base_reg, pin_bit_mask);
    if(LL_GPIO_IsOutputPinSet(gpio_base_reg, pin_bit_mask))
    {
        gpio->out_init = PX_GPIO_OUT_INIT_HI;
    }
    else
    {
        gpio->out_init = PX_GPIO_OUT_INIT_LO;
    }
    if(gpio->pin < 8)
    {
        gpio->af = (px_gpio_af_t)LL_GPIO_GetAFPin_0_7(gpio_base_reg, pin_bit_mask);
    }
    else
    {
        gpio->af = (px_gpio_af_t)LL_GPIO_GetAFPin_8_15(gpio_base_reg, pin_bit_mask);
    }
}

void px_gpio_init(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    // Set pull-up / pull-down (GPIOx_PUPDR: PUPDyy[1:0])
    LL_GPIO_SetPinPull(gpio->gpio_base_reg, pin_bit_mask, gpio->pull);
    // Set pin speed
    LL_GPIO_SetPinSpeed(gpio->gpio_base_reg, pin_bit_mask, gpio->ospeed);
    // Set pin output type (push-pull or open drain)
    LL_GPIO_SetPinOutputType(gpio->gpio_base_reg, pin_bit_mask, gpio->otype);
    // Output?
    if(gpio->out_init)
    {
        // Set output high (1)
        LL_GPIO_SetOutputPin(gpio->gpio_base_reg, pin_bit_mask);
    }
    else
    {
        // Set output low (0)
        LL_GPIO_ResetOutputPin(gpio->gpio_base_reg, pin_bit_mask);
    }
    // Set Alternative Function
    if(gpio->pin < 8)
    {
        LL_GPIO_SetAFPin_0_7(gpio->gpio_base_reg, pin_bit_mask, gpio->af);
    }
    else
    {
        LL_GPIO_SetAFPin_8_15(gpio->gpio_base_reg, pin_bit_mask, gpio->af);
    }
    // Set mode: in, out, alternative function or analog
    LL_GPIO_SetPinMode(gpio->gpio_base_reg, pin_bit_mask, gpio->mode);
}

void px_gpio_port_init(const px_gpio_port_init_t * init)
{
    GPIO_TypeDef * gpio_base_reg = init->gpio_base_reg;

    // Select Pull-Up / Pull-Down or None
    gpio_base_reg->PUPDR   = init->pupdr;
    // Select output type: Push-Pull or Open-Drain
    gpio_base_reg->OTYPER  = init->otyper;
    // Select output speed: Slow, Medium, Fast or Very Fast
    gpio_base_reg->OSPEEDR = init->ospeedr;
    // Set output data: Low or High
    gpio_base_reg->ODR     = init->odr;
    // Select Alternative Function AF0..AF7 for pins 0..7
    gpio_base_reg->AFR[0]  = init->afrl;
    // Select Alternative Function AF0..AF7 for pins 8..15
    gpio_base_reg->AFR[1]  = init->afrh;
    // Select mode: Input, Output, Alternative Function or Analog
    gpio_base_reg->MODER   = init->moder;
}
