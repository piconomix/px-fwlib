/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : PX-ZER0 STM32C0 Board
    Author(s):      Pieter Conradie
    Creation Date:  2025-05-24

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
#ifndef PX_BOARD_PX_ZERO_STM32C0
#warning "Wrong 'board.h' has been included. Check that include path is correct."
#endif

// Report board support package
#warning "This BSP is for PX-ZER0 STM32C0 Board"

// Test for correct microcontroller
#ifndef STM32C071xx
#warning "This Board contains a different microcontroller"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_board_clocks_init(void)
{
    // Enable SYSCFG peripheral clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    // Enable PWR peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    // Select 1 wait state to read from Flash (NVM), because core frequency will
    // be set to 48 MHz (0 wait state if core freq <= 16 MHz)
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    // Enable High Speed Internal 48 MHz RC Oscillator (HSI)
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady()) {;}
    // Trim HSI RC Oscillator to +- 1%
    LL_RCC_HSI_SetCalibTrimming(64);
    LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
    // Enable HSI48
    LL_RCC_HSI48_Enable();
    while(!LL_RCC_HSI48_IsReady()) {;}
    // Enable LSE
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    LL_RCC_LSE_Enable();
    while(!LL_RCC_LSE_IsReady()) {;}
#if 0
    // Enable LSI
    LL_RCC_LSI_Enable();
    while(!LL_RCC_LSI_IsReady()) {;}
    // Enable CSS (Clock Security System)
    LL_RCC_LSE_EnableCSS();
#endif
    // Select LSE for RTC clock source
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTC);
    LL_RCC_EnableRTC();
    // Set AHB prescaler
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    // Set APB1 prescaler
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    // SysClk activation on the HSI
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_HSI) {;}
#if 0
    LL_Init1msTick(48000000);
#endif
    // Store system frequency in global SystemCoreClock CMSIS variable
    LL_SetSystemCoreClock(48000000);
    // Enable CRS (Clock Recovery System)
    LL_CRS_SetSyncDivider(LL_CRS_SYNC_DIV_1);
    LL_CRS_SetSyncPolarity(LL_CRS_SYNC_POLARITY_RISING);
    LL_CRS_SetSyncSignalSource(LL_CRS_SYNC_SOURCE_USB);
    LL_CRS_SetReloadCounter(__LL_CRS_CALC_CALCULATE_RELOADVALUE(48000000, 1000));
    LL_CRS_SetFreqErrorLimit(34);
    LL_CRS_SetHSI48SmoothTrimming(32);
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

void px_board_spi_cs_lo(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI1_CS:     PX_GPIO_OUT_SET_LO(PX_GPIO_SPI1_CS);     break;
    case PX_BOARD_SPI1_CS_SF:  PX_GPIO_OUT_SET_LO(PX_GPIO_SPI1_CS_SF);  break;
    default:                   break;
    }
}

void px_board_spi_cs_hi(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI1_CS:     PX_GPIO_OUT_SET_HI(PX_GPIO_SPI1_CS);     break;
    case PX_BOARD_SPI1_CS_SF:  PX_GPIO_OUT_SET_HI(PX_GPIO_SPI1_CS_SF);  break;
    default:                   break;
    }
}

void px_board_delay_init(void)
{
    // Enable TIM14 peripheral clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
    // Set prescaler so that eack clock tick is 1 us
    TIM14->PSC = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, 1000000) - 1;
    // Set auto-reload value to maximum
    TIM14->ARR = 0xffff;
    // Enable one-pulse mode
    TIM14->CR1 |= TIM_CR1_OPM;
}

void px_board_delay_deinit(void)
{
    // Disable TIM14 peripheral clock
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM14);
}

void px_board_delay_us(uint16_t delay_us)
{
    // Set auto-reload register to delay (in us)
    TIM14->ARR = delay_us;
    // Re-initialize the timer counter (prescaler counter is cleared too)
    TIM14->EGR = TIM_EGR_UG;
    // Enable counter
    TIM14->CR1 |= TIM_CR1_CEN;
    // Wait until CEN is cleared (in one-pulse mode when an update event occurs)
    while(TIM14->CR1 & TIM_CR1_CEN) {;}
}

void px_board_delay_ms(uint16_t delay_ms)
{
    while(delay_ms != 0)
    {
        px_board_delay_us(1000);
        delay_ms--;
    }
}

void px_board_buzzer_on(uint16_t freq_hz)
{
    uint32_t psc;
    uint32_t arr;

    // Enable TIM3 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    // Set prescaler
    psc = __LL_TIM_CALC_PSC(PX_BOARD_PER_CLK_HZ, 100000);
    LL_TIM_SetPrescaler(TIM2, psc);
    // Set auto reload value according to desired frequency (in Hz)
    LL_TIM_EnableARRPreload(TIM2);
    arr = __LL_TIM_CALC_ARR(PX_BOARD_PER_CLK_HZ, psc, ((uint32_t)freq_hz) * 2);
    LL_TIM_SetAutoReload(TIM2, arr);
    // Select output toggle mode (when counter reaches maximum value)
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_TOGGLE);
    LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCPOLARITY_HIGH);
    // Change PWM pin GPIO mode from Analog to Alternative Function
    px_gpio_mode_set(&px_gpio_pwm_buzzer, PX_GPIO_MODE_AF);
    // Start PWM
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH3);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

void px_board_buzzer_off(void)
{
    // Change PWM pin GPIO mode from Alternative Function to Analog
    px_gpio_mode_set(&px_gpio_pwm_buzzer, PX_GPIO_MODE_ANA);
    // Disable timer
    LL_TIM_DisableCounter(TIM2);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
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
