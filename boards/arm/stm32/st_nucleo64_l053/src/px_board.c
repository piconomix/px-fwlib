/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : ST Nucleo64-L053 board
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-30

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
#ifndef PX_BOARD_NUCLEO_L053
#warning "Wrong 'board.h' has been included. Check that include path is correct."
#endif
// Report board support package
#warning "This BSP is for Nucleo L053 board"
// Test for correct microcontroller
#ifndef STM32L053xx
#warning "This Board contains a different microcontroller"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_board_fatal_error(void)
{
    while(true) {;}
}

static void px_board_clocks_init(void)
{
    // Enable SYSCFG peripheral clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    // Enable PWR peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // Select 1 wait state to read from Flash (NVM), because core frequency will
    // be set to 32 MHz (0 wait state if core freq <= 16 MHz)
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
    {
        px_board_fatal_error();
    }
    // Set internal core voltage regulator to 1.8V so that core can operate at 
    // maximum frequency (32 MHz)
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    // Enable High Speed Internal 16 MHz RC Oscillator (HSI)
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady()) {;}
    // Trim HSI RC Oscillator to 16 MHz +- 1%
    LL_RCC_HSI_SetCalibTrimming(16);

    // Enable write access to RTC, RTC Backup registers and
    // Reset and Clock Control Control Status Register (RCC_CSR)
    LL_PWR_EnableBkUpAccess();

    // Set Low Speed External 32768 Hz Oscillator (LSE) drive to low
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    // Enable Low Speed External 32768 Hz crystal oscillator (LSE)
    LL_RCC_LSE_Enable();
    while(!LL_RCC_LSE_IsReady()) {;}

    // Select LSE as clock source for RTC
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    // Enable RTC
    LL_RCC_EnableRTC();
    
    // Configure Phase Locked Loop (PLL) to use 
    // High Speed Internal 16 MHz RC Oscillator (HSI) as source and 
    // output 32 MHz
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI,
                                LL_RCC_PLL_MUL_4, 
                                LL_RCC_PLL_DIV_2);
    // Enable PLL
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady()) {;}

    // Set Arm High Performance Bus (AHB) clock prescaler to 1
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    // Set Arm Peripheral Bus 1 (APB1) clock prescaler to 1
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    // Set Arm Peripheral Bus 2 (APB2) clock prescaler to 1
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // Select 32 MHz output of PLL as clock source for system
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {;}

    // Store system frequency in global SystemCoreClock CMSIS variable
    LL_SetSystemCoreClock(PX_BOARD_SYS_CLK_HZ);

    // Select clock source for USART2 peripheral
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
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

    // Enable pre-read flash buffer
    LL_FLASH_EnablePreRead();

    // Initialise GPIOs
    px_board_gpio_init();

    // Initialise delay
    px_board_delay_init();
}

void px_board_delay_init(void)
{
    // Enable TIM2 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    // Set prescaler so that eack clock tick is 1 us
    TIM2->PSC = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, 1000000) - 1;
    // Set auto-reload value to maximum
    TIM2->ARR = 0xffff;
    // Enable one-pulse mode
    TIM2->CR1 |= TIM_CR1_OPM;
}

void px_board_delay_deinit(void)
{
    // Disable TIM2 peripheral clock
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);    
}

void px_board_delay_us(uint16_t delay_us)
{
    // Set auto-reload register to delay (in us)
    TIM2->ARR = delay_us;
    // Re-initialize the timer counter (prescaler counter is cleared too)
    TIM2->EGR = TIM_EGR_UG;
    // Enable counter
    TIM2->CR1 |= TIM_CR1_CEN;
    // Wait until CEN is cleared (in one-pulse mode when an update event occurs)
    while(TIM2->CR1 & TIM_CR1_CEN) {;}
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
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
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
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
    LL_DBGMCU_DisableDBGStopMode();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
}

void px_board_stop_mode(void)
{
    uint32_t primask;
    bool     systick_int_enabled;

    // Save interrupt status
    primask = __get_PRIMASK();    
    // Disable interrupts
    __disable_irq();
    // Is SysTick interrupt enabled?
    if(LL_SYSTICK_IsEnabledIT())
    {
        // Set flag
        systick_int_enabled = true;
        // Disable SysTick interrupt.
        // This is only required if Debug during STOP mode is enabled.
        LL_SYSTICK_DisableIT();
    }
    else
    {
        // Clear flag
        systick_int_enabled = false;
    }

    // Enable ultra low power mode by switching off VREFINT during STOP mode
    LL_PWR_EnableUltraLowPower();
    // Enable fast wake up from ultra low power mode
    LL_PWR_EnableFastWakeUp();
    // Select HSI16 clock after wake up from STOP mode (default is MSI)
    LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_HSI);
    // Clear PDDS bit to enter STOP mode when the CPU enters Deepsleep
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    // Set LPSDSR bit to switch regulator to low-power mode when the CPU enters Deepsleep
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
    // Clear the WUF flag (after 2 clock cycles)
    LL_PWR_ClearFlag_WU();
    // Set SLEEPDEEP bit of Cortex System Control Register
    LL_LPM_EnableDeepSleep();

    // Put core into STOP mode until an interrupt occurs.
    // Core will wake up even though interrupts are disabled.
    __WFI();
    // After wakeup clock is 16 MHz (HSI16)

    // Enable PLL
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady()) {;}
    // Select 32 MHz output of PLL as clock source for system
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {;}

    // Clear SLEEPDEEP bit of Cortex System Control Register
    LL_LPM_EnableSleep();
    // Clear LPSDSR bit to switch regulator back to main mode
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_MAIN);
    // Must SysTick interrupt be restored?
    if(systick_int_enabled)
    {
        // Enable SysTick interrupt
        LL_SYSTICK_EnableIT();
    }
    // Restore interrupt (after clock has been restored to 32 MHz)
    __set_PRIMASK(primask);
}
