/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2019 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V3.12                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Config_FreeRTOS.c
Purpose : Sample setup configuration of SystemView with FreeRTOS.
Revision: $Rev: 7745 $
*/
#include "FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"

#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_tim.h"

extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
// The application name to be displayed in SystemViewer
#define SYSVIEW_APP_NAME        "FreeRTOS app"

// The target device name
#define SYSVIEW_DEVICE_NAME     "Cortex-M0+"

// Frequency of the timestamp. Must match SEGGER_SYSVIEW_GET_TIMESTAMP in SEGGER_SYSVIEW_Conf.h
#define SYSVIEW_TIMESTAMP_FREQ  1000000

// System Frequency. SystemcoreClock is used in most CMSIS compatible projects.
#define SYSVIEW_CPU_FREQ        configCPU_CLOCK_HZ

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (0x20000000)

/*********************************************************************
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
  SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",D="SYSVIEW_DEVICE_NAME",O=FreeRTOS");
  SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick");
  SEGGER_SYSVIEW_SendSysDesc("I#23=EXTI[15:4]");
}

/// [SYSVIEW_TIMESTAMP]
static U16 SEGGER_SYSVIEW_TickCnt;

void TIM7_IRQHandler(void)
{
    // Interrupt flag set?
    if(LL_TIM_IsActiveFlag_UPDATE(TIM7))
    {
        // Clear flag
        LL_TIM_ClearFlag_UPDATE(TIM7);
        // Increment counter
        SEGGER_SYSVIEW_TickCnt++;
    }
}

static void SEGGER_SYSVIEW_TimeStampInit(void)
{
    #warning "TIM7 peripheral is used for Segger SysView timestamping"
    // Enable TIM7 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
    // Set prescaler to match timestamp frequency
    LL_TIM_SetPrescaler(TIM7, __LL_TIM_CALC_PSC(SYSVIEW_CPU_FREQ, SYSVIEW_TIMESTAMP_FREQ));
    // Enable timer interrupt
    NVIC_EnableIRQ(TIM7_IRQn);
    LL_TIM_EnableIT_UPDATE(TIM7);
    // Enable counter
    LL_TIM_EnableCounter(TIM7);
}

U32 SEGGER_SYSVIEW_X_GetTimestamp(void)
{
    U16 Counter;
    U16 TickCount;
    U32 TimeStamp;

    // If a timer interrupt is pending, adjust overflow counter
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7))
    {
      LL_TIM_ClearFlag_UPDATE(TIM7);
      SEGGER_SYSVIEW_TickCnt++;
    }
    // Read overflow counter
    TickCount = SEGGER_SYSVIEW_TickCnt;
    // Read timer counter
    Counter   = LL_TIM_GetCounter(TIM7);
    // Create combined 32-bit timestamp
    TimeStamp = (((U32)TickCount) << 16) | Counter;

    return TimeStamp;
}
/// [SYSVIEW_TIMESTAMP]

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void SEGGER_SYSVIEW_Conf(void) {
  SEGGER_SYSVIEW_TimeStampInit();
  SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ,
                      &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
  SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
}

/*************************** End of file ****************************/
