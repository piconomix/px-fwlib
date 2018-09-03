/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          px_gpio.h : ST STM32 GPIO driver
    Author(s):      Pieter Conradie
    Creation Date:  2017-11-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gpio.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_gpio")

/* _____LOCAL DEFINITIONS____________________________________________________ */

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

void px_gpio_pin_init(const px_gpio_handle_t * gpio)
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
    // Alternative function?
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
