/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup STM32_TUTORIALS 
 *  @defgroup STM32_TUT01_GPIO_PART_D 01_gpio_part_d
 *  
 *  How to configure and use General Purpose (digital) I/O pins - part D.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/01_gpio_part_d/gpio.c
 *  
 *  The LED flashes at 1 Hz (500 ms on; 500 ms off). On the Hero Board, the LED
 *  is wired to Port H pin 0. PH0 must be configured as a digital output pin.
 *  Set PH0 high to enable the LED and set PH0 low to disable the LED.
 *  
 *  ST provides the STM32Cube library that defines a HAL (Hardware Abstraction
 *  Layer) and LL (Low Level) API to access the peripherals. See:
 *  
 *      libs/STM32Cube/L0/Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_ll_bus.h
 *      libs/STM32Cube/L0/Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_ll_gpio.h
 *  
 *  @tip_s
 *  Part A, B and C was created as an introduction to understanding how to use
 *  the definitions in these files.
 *  @tip_e
 *  
 *  It may seem counter intuitive, but I have found that it is easier to scan
 *  the relevant peripheral register section in the manual, figure out which
 *  register needs to be changed and then search for that register name in the
 *  STM32Cube library until I find the correct API function to call. For
 *  example, if I want to change the mode of a GPIO pin, I will search for
 *  "MODER" and find the following hits:
 *  
 *      libs/STM32Cube/L0/Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_ll_gpio.h:
 *  
 *      138 49:#define LL_GPIO_MODE_OUTPUT                GPIO_MODER_MODE0_0
 *      139 49:#define LL_GPIO_MODE_ALTERNATE             GPIO_MODER_MODE0_1
 *      140 49:#define LL_GPIO_MODE_ANALOG                GPIO_MODER_MODE0
 *      244 13:  * @rmtoll MODER        MODEy         LL_GPIO_SetPinMode
 *      272 21:  MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
 *      272 48:  MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
 *      279 13:  * @rmtoll MODER        MODEy         LL_GPIO_GetPinMode
 *      306 37:  return (uint32_t)(READ_BIT(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0)) / (Pin * Pin));
 *      306 64:  return (uint32_t)(READ_BIT(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0)) / (Pin * Pin));
 *  
 *  Line 272 looks promising and there I find the following function:
 *  
 *      @code{.c}
 *      __STATIC_INLINE void LL_GPIO_SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
 *      {
 *        MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
 *      }
 *      @endcode
 *  
 *  The function has a Doxygen comment header (not reproduced here) that shows
 *  what parameter names to use.
 *  
 *  @tip_s
 *  On the Hero Board, the 6/YES button is wired to PC9. Now that you have
 *  mastered the GPIO peripheral, create a project that configures PC9 as a
 *  digital input with pull-up resistor enabled. If the button is pressed, PC9
 *  goes low. Enable the LED while the button is being pressed.
 *  @tip_e
 *  
 *  @warn_s
 *  Remember to enable the clock to Port C!
 *  @warn_e
 *  
 *  
 */

#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"

void delay(void)
{
    // Wait ~ 500 ms; default system clock is 2.1 MHz after startup from power reset
    for(uint32_t i = 0x40000; i!=0; i--)
    {
        // Prevent compiler from optimizing and removing empty delay loop
        __asm__ __volatile__("\n\t");
    }
}

int main(void)
{
    // Set RCC_IOPENR register bit 7 to enable clock of Port H
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

    // Configure PH0 as a digital output
    LL_GPIO_SetPinMode(GPIOH, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();

        // Clear PH0 output to disable LED
        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
    }
}
