/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2023 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : STM32C0116-DK board
    Author(s):      Pieter Conradie
    Creation Date:  2023-05-20

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_gpio_init_port_a.h"
#include "px_gpio_init_port_b.h"
#include "px_gpio_init_port_c.h"
#include "px_stm32cube.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Check that correct 'board.h' has been included
#ifndef PX_BOARD_STM32C0116_DK
#warning "Wrong 'board.h' has been included. Check that include path is correct."
#endif
// Report board support package
#warning "This BSP is for STM32C0116-DK board"
// Test for correct microcontroller
#ifndef STM32C011xx
#warning "This Board contains a different microcontroller"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#if 0
static void px_board_fatal_error(void)
{
    while(true) {;}
}
#endif

static void px_board_clocks_init(void)
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    LL_SYSCFG_EnablePinRemap(LL_SYSCFG_PIN_RMP_PA11);
    LL_SYSCFG_EnablePinRemap(LL_SYSCFG_PIN_RMP_PA12);
    // Select 1 wait state to read from Flash (NVM), because core frequency will
    // be set to 48 MHz (0 wait state if core freq <= 16 MHz)
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    // Enable High Speed Internal 48 MHz RC Oscillator (HSI)
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady()) {;}
    // Trim HSI RC Oscillator to +- 1%
    LL_RCC_HSI_SetCalibTrimming(64);
    LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
    // Set AHB prescaler
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    // SysClk activation on the HSI
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {;}
    // Set APB1 prescaler
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
#if 0
    LL_Init1msTick(48000000);
#endif
    // Store system frequency in global SystemCoreClock CMSIS variable
    LL_SetSystemCoreClock(48000000);
}

static void px_board_gpio_init(void)
{
    // Enable GPIO peripheral clocks
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    // Initialise GPIO ports
    px_gpio_port_init(&px_gpio_port_a_init);
    px_gpio_port_init(&px_gpio_port_b_init);
    px_gpio_port_init(&px_gpio_port_c_init);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_board_init(void)
{
    // Initialise clock system
    px_board_clocks_init();
    // Initialise GPIOs
    px_board_gpio_init();
    // Initialise delay
    px_board_delay_init();
}

void px_board_delay_init(void)
{
    // Enable TIM3 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    // Set prescaler so that eack clock tick is 1 us
    TIM3->PSC = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, 1000000) - 1;
    // Set auto-reload value to maximum
    TIM3->ARR = 0xffff;
    // Enable one-pulse mode
    TIM3->CR1 |= TIM_CR1_OPM;
}

void px_board_delay_deinit(void)
{
    // Disable TIM3 peripheral clock
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
}

void px_board_delay_us(uint16_t delay_us)
{
    // Set auto-reload register to delay (in us)
    TIM3->ARR = delay_us;
    // Re-initialize the timer counter (prescaler counter is cleared too)
    TIM3->EGR = TIM_EGR_UG;
    // Enable counter
    TIM3->CR1 |= TIM_CR1_CEN;
    // Wait until CEN is cleared (in one-pulse mode when an update event occurs)
    while(TIM3->CR1 & TIM_CR1_CEN) {;}
}

void px_board_delay_ms(uint16_t delay_ms)
{
    while(delay_ms != 0)
    {
        px_board_delay_us(1000);
        delay_ms--;
    }
}

void px_board_dbg_enable(void)
{
    // Enable SWD pins with pull resistors
    px_gpio_mode_set(&px_gpio_swdck, PX_GPIO_MODE_AF);
    px_gpio_pull_dn_enable(&px_gpio_swdck);
    px_gpio_mode_set(&px_gpio_swdio, PX_GPIO_MODE_AF);
    px_gpio_pull_up_enable(&px_gpio_swdio);

    // Enable Debug Module during SLEEP and STOP mode
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DBGMCU);
    LL_DBGMCU_EnableDBGStopMode();
}

void px_board_dbg_disable(void)
{
    // Set SWD pins to analog with no pull resistors
    px_gpio_mode_set(&px_gpio_swdck, PX_GPIO_MODE_ANA);
    px_gpio_pull_disable(&px_gpio_swdck);
    px_gpio_mode_set(&px_gpio_swdio, PX_GPIO_MODE_ANA);
    px_gpio_pull_disable(&px_gpio_swdio);

    // Disable Debug Module during STOP mode
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DBGMCU);
    LL_DBGMCU_DisableDBGStopMode();
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_DBGMCU);
}
