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
 *  @defgroup STM32_TUT01_GPIO_PART_C 01_gpio_part_c
 *  
 *  How to configure and use General Purpose (digital) I/O pins - part C.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/01_gpio_part_c/gpio.c
 *  
 *  The LED flashes at 1 Hz (500 ms on; 500 ms off). On the Hero Board, the LED
 *  is wired to Port H pin 0. PH0 must be configured as a digital output pin.
 *  Set PH0 high to enable the LED and set PH0 low to disable the LED.
 *  
 *  ST provides the STM32Cube library that defines direct access to the
 *  peripheral registers. See:
 *  
 *      libs/STM32Cube/L0/Drivers/CMSIS/Device/ST/STM32L0xx/Include/stm32l0xx.h
 *      libs/STM32Cube/L0/Drivers/CMSIS/Device/ST/STM32L0xx/Include/stm32l072xx.h
 *  
 *  @tip_s
 *  Part A and B was created as an introduction to understanding how to use the
 *  definitions in these files.
 *  @tip_e
 *  
 *  Use the family include file "stm32l0xx.h" and specify the specific
 *  microcontroller in the Makefile:
 *  
 *      CDEFS += STM32L072xx
 *  
 *  Inspect "stm32l072xx.h" and you will find the relevant definitions:
 *  
 *      @code{.c}
 *      typedef struct
 *      {
 *        __IO uint32_t MODER;
 *        __IO uint32_t OTYPER;
 *        __IO uint32_t OSPEEDR;
 *        __IO uint32_t PUPDR;
 *        __IO uint32_t IDR;
 *        __IO uint32_t ODR;
 *        __IO uint32_t BSRR;
 *        __IO uint32_t LCKR;
 *        __IO uint32_t AFR[2];
 *        __IO uint32_t BRR;
 *      } GPIO_TypeDef;
 *  
 *      ...
 *  
 *      #define PERIPH_BASE             (0x40000000UL)
        #define IOPPERIPH_BASE          (PERIPH_BASE + 0x10000000UL)
 *      #define GPIOH_BASE              (IOPPERIPH_BASE + 0x00001C00UL)
 *      #define GPIOH                   ((GPIO_TypeDef *) GPIOH_BASE)
 *  
 *      ...
 *  
 *      #define GPIO_MODER_MODE0_Pos    (0U)  
 *      #define GPIO_MODER_MODE0_Msk    (0x3UL << GPIO_MODER_MODE0_Pos)
 *      #define GPIO_MODER_MODE0        GPIO_MODER_MODE0_Msk                   
 *      #define GPIO_MODER_MODE0_0      (0x1UL << GPIO_MODER_MODE0_Pos)
 *      #define GPIO_MODER_MODE0_1      (0x2UL << GPIO_MODER_MODE0_Pos)
 *      #define GPIO_MODER_MODE1_Pos    (2U)
 *      @endcode
 *  
 *  In "libs/STM32Cube/L0/Drivers/CMSIS/Include/core_cm0plus.h" you will find
 *  the definition of '__IO':
 *  
 *      @code{.c}
 *      #define __IO            volatile
 *      @endcode
 *  
 */

#include "stm32l0xx.h"

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
    uint32_t val;

    // Set RCC_IOPENR register bit 7 to enable clock of Port H
    RCC->IOPENR |= RCC_IOPENR_IOPHEN;

    // Read GPIOH_MODER register value
    val          = GPIOH->MODER;
    // Set MODE0[1:0] = 01 (General purpose output mode)
    val         &= ~GPIO_MODER_MODE0_Msk;        // Clear bit 1 and bit 0
    val         |=  (1 << GPIO_MODER_MODE0_Pos); // Set bit 0
    // Write updated GPIOH_MODER register value
    GPIOH->MODER = val;

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        GPIOH->BSRR = GPIO_BSRR_BS_0;
        delay();

        // Clear PH0 output to disable LED
        GPIOH->BRR  = GPIO_BRR_BR_0;
        delay();
    }
}
