/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          Piconomix STM32 Hero Board Low Power example
    Author(s):      Pieter Conradie
    Creation Date:  2019-03-08
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include "px_defines.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_spi.h"
#include "px_uart.h"
#include "px_at25s.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_spi_handle_t  px_spi_sf_handle;

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile bool main_button_press_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
void EXTI4_15_IRQHandler(void)
{
    // Pending flag set for line 9?
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9))
    {
        // Clear pending flag
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
        // Set software flag to indicate that button has been pressed
        main_button_press_flag = true;
    }    
}

static void main_init(void)
{
    // Initialize board
    px_board_init();
    px_spi_init();
    px_uart_init();

    // Open UART1
    px_uart_open2(&px_uart1_handle,
                  PX_UART_PER_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_PER_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);

    // Initialise AT25S Serial Flash driver
    px_spi_open2(&px_spi_sf_handle,
                 PX_SPI_PER_2,
                 PX_BOARD_SPI2_CS_SF,
                 px_spi_util_baud_hz_to_clk_div(PX_AT25S_MAX_SPI_CLOCK_HZ), 
                 PX_AT25S_SPI_MODE, 
                 PX_AT25S_SPI_DATA_ORDER,
                 0x00);
    px_at25s_init(&px_spi_sf_handle);

    // Power down Serial Flash to minimise power consumption
    px_at25s_power_down();
}

static void main_dbg_disable(void)
{
    // Set SWD pins to analog with no pull resistors
    px_gpio_mode_set(&px_gpio_swdck, PX_GPIO_MODE_ANA);
    px_gpio_pulldn_disable(&px_gpio_swdck);
    px_gpio_mode_set(&px_gpio_swdio, PX_GPIO_MODE_ANA);
    px_gpio_pullup_disable(&px_gpio_swdio);

    // Disable Debug Module during STOP mode
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
    LL_DBGMCU_DisableDBGStopMode();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
}

static void main_hsi48_disable(void)
{
    #warning "Does not work properly yet"

    // Disable Clock Recovery System
    LL_CRS_DisableFreqErrorCounter();
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_CRS);

    // Disable HSI48 oscillator
    LL_SYSCFG_VREFINT_DisableHSI48();
    LL_RCC_HSI48_Disable();
}

static void main_gpio_disable(void)
{
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5,  LL_GPIO_MODE_ANALOG); // PX_GPIO_SPI1_MOSI
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8,  LL_GPIO_MODE_ANALOG); // PX_GPIO_3V3_HOLD
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_11, LL_GPIO_MODE_ANALOG); // PX_GPIO_UART4_RX
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board
    main_init();

    // Initialise graphics
    px_gfx_init();
    // Draw text
    px_gfx_draw_str(&px_gfx_font_5x7, 0, 0, PX_GFX_COLOR_ON, "LOW POWER");
    // Update display
    px_gfx_update();
    
    #warning "low power debugging steps added here:"    
    //main_gpio_disable();    

    // Disable SWD debug module
    main_dbg_disable();

    // Disable 48 MHZ oscillator
    //main_hsi48_disable();

    // Select Port C pin 9 (6/YES button) for extended interrupt on EXTI9
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE9);
    // Enable falling edge external interrupt on line 9
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_9);
    // Enable EXTI4_15 interrupt
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    
    for(;;)
    {
        // Turn LCD power save off
        px_lcd_power_save_off();
        // Enable LCD backlight
        PX_LCD_BACKLIGHT_ON();
        // Clear flag
        main_button_press_flag = false;
        // LED on
        PX_USR_LED_ON();
        // Wait until button is released?
        while(PX_USR_PB_IS_PRESSED())
        {
            ;
        }
        // LED off
        PX_USR_LED_OFF();
        // Disable LCD backlight
        PX_LCD_BACKLIGHT_OFF();
        // Turn LCD power save on
        px_lcd_power_save_on();

        // Clear the WUF flag (after 2 clock cycles)
        LL_PWR_ClearFlag_WU();
        // Set LPSDSR to switch regulator to low-power mode when the CPU enters Deepsleep
        LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
        // Clear PDDS to enter STOP mode when the CPU enters Deepsleep
        LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
        // Use HSI16 oscillator after wake-up from STOP mode
        LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_HSI);
        // Enable ultra low-power mode by switching off VREFINT during STOP mode
        LL_PWR_EnableUltraLowPower();
        // Set SLEEPDEEP bit of Cortex System Control Register
        LL_LPM_EnableDeepSleep();

        // Put core into STOP mode until an interrupt occurs
        __WFI();
    }	
}
