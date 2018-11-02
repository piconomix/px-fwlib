/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_dac.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LCD_BTN_3_UP_Pin LL_GPIO_PIN_13
#define LCD_BTN_3_UP_GPIO_Port GPIOC
#define OSC32_IN_Pin LL_GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin LL_GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define USR_LED_Pin LL_GPIO_PIN_0
#define USR_LED_GPIO_Port GPIOH
#define LCD_BTN_2_RT_Pin LL_GPIO_PIN_1
#define LCD_BTN_2_RT_GPIO_Port GPIOH
#define LCD_BTN_1_LT_Pin LL_GPIO_PIN_0
#define LCD_BTN_1_LT_GPIO_Port GPIOC
#define GPIO6_Pin LL_GPIO_PIN_1
#define GPIO6_GPIO_Port GPIOC
#define GPIO5_Pin LL_GPIO_PIN_2
#define GPIO5_GPIO_Port GPIOC
#define GPIO4_Pin LL_GPIO_PIN_3
#define GPIO4_GPIO_Port GPIOC
#define GPIO3_Pin LL_GPIO_PIN_0
#define GPIO3_GPIO_Port GPIOA
#define GPIO2_Pin LL_GPIO_PIN_1
#define GPIO2_GPIO_Port GPIOA
#define UART2_TX_Pin LL_GPIO_PIN_2
#define UART2_TX_GPIO_Port GPIOA
#define UART2_RX_Pin LL_GPIO_PIN_3
#define UART2_RX_GPIO_Port GPIOA
#define DAC_Pin LL_GPIO_PIN_4
#define DAC_GPIO_Port GPIOA
#define GPIO1_Pin LL_GPIO_PIN_5
#define GPIO1_GPIO_Port GPIOA
#define GPIO0_Pin LL_GPIO_PIN_6
#define GPIO0_GPIO_Port GPIOA
#define ADC3_Pin LL_GPIO_PIN_7
#define ADC3_GPIO_Port GPIOA
#define ADC2_Pin LL_GPIO_PIN_4
#define ADC2_GPIO_Port GPIOC
#define ADC1_Pin LL_GPIO_PIN_5
#define ADC1_GPIO_Port GPIOC
#define ADC0_Pin LL_GPIO_PIN_0
#define ADC0_GPIO_Port GPIOB
#define ADC_VBAT_Pin LL_GPIO_PIN_1
#define ADC_VBAT_GPIO_Port GPIOB
#define SPI2_CS_DF_Pin LL_GPIO_PIN_2
#define SPI2_CS_DF_GPIO_Port GPIOB
#define SPI2_CS_LCD_Pin LL_GPIO_PIN_10
#define SPI2_CS_LCD_GPIO_Port GPIOB
#define LCD_RS_Pin LL_GPIO_PIN_11
#define LCD_RS_GPIO_Port GPIOB
#define LCD_BACKLIGHT_Pin LL_GPIO_PIN_12
#define LCD_BACKLIGHT_GPIO_Port GPIOB
#define SPI2_SCK_Pin LL_GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin LL_GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin LL_GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define PWM_BUZZER_Pin LL_GPIO_PIN_6
#define PWM_BUZZER_GPIO_Port GPIOC
#define VBAT_MEAS_EN_Pin LL_GPIO_PIN_7
#define VBAT_MEAS_EN_GPIO_Port GPIOC
#define PWR_HOLD_Pin LL_GPIO_PIN_8
#define PWR_HOLD_GPIO_Port GPIOC
#define LCD_BTN_6_YES_Pin LL_GPIO_PIN_9
#define LCD_BTN_6_YES_GPIO_Port GPIOC
#define USB_DET_Pin LL_GPIO_PIN_8
#define USB_DET_GPIO_Port GPIOA
#define UART1_TX_Pin LL_GPIO_PIN_9
#define UART1_TX_GPIO_Port GPIOA
#define UART1_RX_Pin LL_GPIO_PIN_10
#define UART1_RX_GPIO_Port GPIOA
#define USB1D_N_Pin LL_GPIO_PIN_11
#define USB1D_N_GPIO_Port GPIOA
#define USB1D_P_Pin LL_GPIO_PIN_12
#define USB1D_P_GPIO_Port GPIOA
#define SWDIO_Pin LL_GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin LL_GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define LCD_BTN_5_NO_Pin LL_GPIO_PIN_15
#define LCD_BTN_5_NO_GPIO_Port GPIOA
#define UART3_TX_Pin LL_GPIO_PIN_10
#define UART3_TX_GPIO_Port GPIOC
#define UART3_RX_Pin LL_GPIO_PIN_11
#define UART3_RX_GPIO_Port GPIOC
#define LCD_BTN_4_DN_Pin LL_GPIO_PIN_12
#define LCD_BTN_4_DN_GPIO_Port GPIOC
#define SPI1_CS_SD_Pin LL_GPIO_PIN_2
#define SPI1_CS_SD_GPIO_Port GPIOD
#define SPI1_SCK_Pin LL_GPIO_PIN_3
#define SPI1_SCK_GPIO_Port GPIOB
#define SPI1_MISO_Pin LL_GPIO_PIN_4
#define SPI1_MISO_GPIO_Port GPIOB
#define SPI1_MOSI_Pin LL_GPIO_PIN_5
#define SPI1_MOSI_GPIO_Port GPIOB
#define SPI1_CS_Pin LL_GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOB
#define GPIO7_SD_CD_Pin LL_GPIO_PIN_7
#define GPIO7_SD_CD_GPIO_Port GPIOB
#define I2C1_SCL_Pin LL_GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I21C1_SDA_Pin LL_GPIO_PIN_9
#define I21C1_SDA_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
