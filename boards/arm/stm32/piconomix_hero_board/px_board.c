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
   
    Title:          board.h : Piconomix STM32 Hero Board
    Author(s):      Pieter Conradie
    Creation Date:  2017-11-13

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_lib_stm32cube.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Check that correct 'board.h' has been included
#ifndef PX_BOARD_PICONOMIX_HERO_BOARD
#warning "Wrong 'board.h' has been included. Check that include path is correct."
#endif

// Report board support package
#warning "This BSP is for Piconomix STM32 Hero Board"

// Test for correct microcontroller
#ifndef STM32L072xx
#warning "This Board contains a different microcontroller"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_board_fatal_error(void)
{
    while(true)
    {
        ;
    }
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
    // Wait until ready
    while(!LL_RCC_HSI_IsReady())
    {
        ;
    }
    // Trim HSI RC Oscillator to 16 MHz +- 1%
    LL_RCC_HSI_SetCalibTrimming(16);
    
    // Enable High Speed Internal 48 MHz RC Oscillator (HSI48) for USB
    LL_RCC_HSI48_Enable();
    // Enable buffer used to generate VREFINT reference for HSI48 oscillator
    LL_SYSCFG_VREFINT_EnableHSI48();
    // Wait until ready
    while(!LL_RCC_HSI48_IsReady())
    {
        ;
    }

    // Enable CRS (Clock Recovery System) peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CRS);
    // Enable synchronisation to trim HSI48 oscillator with 1 kHz USB SOF (Start Of Frame)
    LL_CRS_ConfigSynchronization(LL_CRS_HSI48CALIBRATION_DEFAULT,
                                 LL_CRS_ERRORLIMIT_DEFAULT,
                                 LL_CRS_RELOADVALUE_DEFAULT,
                                 LL_CRS_SYNC_DIV_1 | LL_CRS_SYNC_SOURCE_USB | LL_CRS_SYNC_POLARITY_RISING);
    // Enable automatic trimming
    LL_CRS_EnableAutoTrimming();
    // Enable frequency error counter
    LL_CRS_EnableFreqErrorCounter();

    // Enable write access to RTC, RTC Backup registers and
    // Reset and Clock Control Control Status Register (RCC_CSR)
    LL_PWR_EnableBkUpAccess();
#if 0
    // Reset the RTC peripheral, it's clock source selection and backup registers
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
#endif
    // Set Low Speed External 32768 Hz Oscillator (LSE) drive to low
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    // Enable Low Speed External 32768 Hz crystal oscillator (LSE)
    LL_RCC_LSE_Enable();
    // Wait until ready
    while(!LL_RCC_LSE_IsReady())
    {
        ;
    }

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
    // Enable PLL; wait until ready
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady())
    {
        ;
    }

    // Set Arm High Performance Bus (AHB) clock prescaler to 1
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    // Set Arm Peripheral Bus 1 (APB1) clock prescaler to 1
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    // Set Arm Peripheral Bus 2 (APB2) clock prescaler to 1
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // Select 32 MHz output of PLL as clock source for system; wait until ready
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
        ;
    }

    // Store system frequency in global SystemCoreClock CMSIS variable
    LL_SetSystemCoreClock(PX_BOARD_SYS_CLK_HZ);

    // Select clock sources for USART1, USART2, I2C1 and USB peripherals
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
}

static void px_board_gpio_port_init(GPIO_TypeDef * gpio_base_reg,
                                    uint32_t       moder,
                                    uint32_t       otyper,
                                    uint32_t       ospeedr,
                                    uint32_t       pupdr,
                                    uint32_t       odr,
                                    uint32_t       afrl,
                                    uint32_t       afrh)
{
    // Select Pull-Up / Pull-Down or None
    gpio_base_reg->PUPDR   = pupdr;
    // Select output type: Push-Pull or Open-Drain
    gpio_base_reg->OTYPER  = otyper;
    // Select output speed: Slow, Medium, Fast or Very Fast
    gpio_base_reg->OSPEEDR = ospeedr;
    // Set output data: Low or High
    gpio_base_reg->ODR     = odr;
    // Select Alternative Function AF0..AF7 for pins 0..7
    gpio_base_reg->AFR[0]  = afrl;
    // Select Alternative Function AF0..AF7 for pins 8..15
    gpio_base_reg->AFR[1]  = afrh;
    // Select mode: Input, Output, Alternative Function or Analog
    gpio_base_reg->MODER   = moder;
}

static void px_board_gpio_init(void)
{
    // Enable GPIO peripheral clocks
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

    // Initialise GPIOA[0..15]
    px_board_gpio_port_init(GPIOA,
                            GPIOA_MODER_VAL,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0);

    // Initialise GPIOA[0..15]
    px_board_gpio_port_init(GPIOA,
                            GPIOA_MODER_VAL,
                            GPIOA_OTYPER_VAL,
                            GPIOA_OSPEEDR_VAL,
                            GPIOA_PUPDR_VAL,
                            GPIOA_ODR_VAL,
                            GPIOA_AFRL_VAL,
                            GPIOA_AFRH_VAL);
    // Initialise GPIOB[0..15]
    px_board_gpio_port_init(GPIOB,
                            GPIOB_MODER_VAL,
                            GPIOB_OTYPER_VAL,
                            GPIOB_OSPEEDR_VAL,
                            GPIOB_PUPDR_VAL,
                            GPIOB_ODR_VAL,
                            GPIOB_AFRL_VAL,
                            GPIOB_AFRH_VAL);
    // Initialise GPIOC[0..15]
    px_board_gpio_port_init(GPIOC,
                            GPIOC_MODER_VAL,
                            GPIOC_OTYPER_VAL,
                            GPIOC_OSPEEDR_VAL,
                            GPIOC_PUPDR_VAL,
                            GPIOC_ODR_VAL,
                            GPIOC_AFRL_VAL,
                            GPIOC_AFRH_VAL);
    // Initialise GPIOD[2]
    px_board_gpio_port_init(GPIOD,
                            GPIOD_MODER_VAL,
                            GPIOD_OTYPER_VAL,
                            GPIOD_OSPEEDR_VAL,
                            GPIOD_PUPDR_VAL,
                            GPIOD_ODR_VAL,
                            GPIOD_AFRL_VAL,
                            0);
    // Initialise GPIOH[0..1]
    px_board_gpio_port_init(GPIOH,
                            GPIOH_MODER_VAL,
                            GPIOH_OTYPER_VAL,
                            GPIOH_OSPEEDR_VAL,
                            GPIOH_PUPDR_VAL,
                            GPIOH_ODR_VAL,
                            GPIOH_AFRL_VAL,
                            0);
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

void px_board_spi_cs_lo(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI1_CS:     PX_GPIO_PIN_SET_LO(PX_GPIO_SPI1_CS);     break;
    case PX_BOARD_SPI1_CS_SD:  PX_GPIO_PIN_SET_LO(PX_GPIO_SPI1_CS_SD);  break;
    case PX_BOARD_SPI2_CS_LCD: PX_GPIO_PIN_SET_LO(PX_GPIO_SPI2_CS_LCD); break;
    case PX_BOARD_SPI2_CS_SF:  PX_GPIO_PIN_SET_LO(PX_GPIO_SPI2_CS_SF);  break;
    default:                   break;
    }
}

void px_board_spi_cs_hi(uint8_t cs_id)
{
    switch(cs_id)
    {
    case PX_BOARD_SPI1_CS:     PX_GPIO_PIN_SET_HI(PX_GPIO_SPI1_CS);     break;
    case PX_BOARD_SPI1_CS_SD:  PX_GPIO_PIN_SET_HI(PX_GPIO_SPI1_CS_SD);  break;
    case PX_BOARD_SPI2_CS_LCD: PX_GPIO_PIN_SET_HI(PX_GPIO_SPI2_CS_LCD); break;
    case PX_BOARD_SPI2_CS_SF:  PX_GPIO_PIN_SET_HI(PX_GPIO_SPI2_CS_SF);  break;
    default:                   break;
    }
}

void px_board_delay_init(void)
{
    // Enable TIM6 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    // Set prescaler so that eack clock tick is 1 us
    TIM6->PSC = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, 1000000) - 1;
    // Set auto-reload value to maximum
    TIM6->ARR = 0xffff;
    // Enable one-pulse mode
    TIM6->CR1 |= TIM_CR1_OPM;
}

void px_board_delay_deinit(void)
{
    // Disable TIM6 peripheral clock
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM6);    
}

void px_board_delay_us(uint16_t delay_us)
{
    // Set auto-reload register to delay (in us)
    TIM6->ARR = delay_us;
    // Re-initialize the timer counter (prescaler counter is cleared too)
    TIM6->EGR = TIM_EGR_UG;
    // Enable counter
    TIM6->CR1 |= TIM_CR1_CEN;
    // Wait until CEN is cleared (in one-pulse mode when an update event occurs)
    while(TIM6->CR1 & TIM_CR1_CEN)
    {
        ;
    }
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
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    // Set prescaler
    psc = __LL_TIM_CALC_PSC(PX_BOARD_PER_CLK_HZ, 100000);
    LL_TIM_SetPrescaler(TIM3, psc);

    // Set auto reload value according to desired frequency (in Hz)
    LL_TIM_EnableARRPreload(TIM3);
    arr = __LL_TIM_CALC_ARR(PX_BOARD_PER_CLK_HZ, psc, ((uint32_t)freq_hz) * 2);
    LL_TIM_SetAutoReload(TIM3, arr);

    // Select output toggle mode (when counter reaches maximum value)
    LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_TOGGLE);
    LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);

    // Start PWM
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM3);
    LL_TIM_GenerateEvent_UPDATE(TIM3);
}

void px_board_buzzer_off(void)
{
    // Disable timer
    LL_TIM_DisableCounter(TIM3);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
}
