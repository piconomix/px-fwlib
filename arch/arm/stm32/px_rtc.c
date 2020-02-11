/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_rtc.h : RTC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_rtc.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("rtc");

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile bool px_rtc_alarm_a_flag;
static volatile bool px_rtc_wakeup_tmr_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint8_t px_rtc_bcd_to_bin(uint8_t bcd)
{
    uint8_t bin  = PX_U8_HI4(bcd) * 10 + PX_U8_LO4(bcd);

    return bin;
}

static uint8_t px_rtc_bin_to_bcd(uint8_t bin)
{
    uint8_t bcd  = ((uint8_t)(bin / 10) << 4) + bin % 10;

    return bcd;
}

void RTC_IRQHandler(void)
{
    // Alarm A interrupt enabled?
    if(LL_RTC_IsEnabledIT_ALRA(RTC))
    {
        // Alarm A interrupt flag set?
        if(LL_RTC_IsActiveFlag_ALRA(RTC))
        {
            // Clear interrupt flag
            LL_RTC_ClearFlag_ALRA(RTC);
            // Set flag
            px_rtc_alarm_a_flag = true;
        }
        // Clear EXTI line flag associated with RTC Alarm
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
    }
    // Wakeup Timer interrupt enabled?
    if(LL_RTC_IsEnabledIT_WUT(RTC))
    {
        // Wakeup Timer interrupt flag set?
        if(LL_RTC_IsActiveFlag_WUT(RTC))
        {
            // Clear interrupt flag
            LL_RTC_ClearFlag_WUT(RTC);
            // Set flag
            px_rtc_wakeup_tmr_flag = true;
        }
        // Clear EXTI line flag associated with RTC Wakeup Timer
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_rtc_init(void)
{
    // Is magic value stored in RTC backup register?
    if(LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) == 0xcafe)
    {
        // RTC has already been initialised
        return;
    }
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Enter initialization mode
    LL_RTC_EnableInitMode(RTC);
    while(!LL_RTC_IsActiveFlag_INIT(RTC))
    {
        ;
    }
    // Set asynchronous prescaler
    LL_RTC_SetAsynchPrescaler(RTC, 127);
    // Set synchronous prescaler
    LL_RTC_SetSynchPrescaler(RTC, 255);
    // Reset date to 2000-01-01 (Thursday)
    RTC->DR =   (((uint32_t)0x00) << RTC_DR_YU_Pos )
              | (((uint32_t)0x04) << RTC_DR_WDU_Pos)
              | (((uint32_t)0x01) << RTC_DR_MU_Pos )
              | (((uint32_t)0x01) << RTC_DR_DU_Pos );
    // Reset time to 00:00:00
    RTC->TR =   (((uint32_t)0x00) << RTC_TR_HU_Pos )
              | (((uint32_t)0x00) << RTC_TR_MNU_Pos)
              | (((uint32_t)0x00) << RTC_TR_SU_Pos );
    // Bypass shadow registers
    LL_RTC_EnableShadowRegBypass(RTC);
    // Exit Initialization mode
    LL_RTC_DisableInitMode(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);
    // Store magic value in RTC backup register
    LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, 0xcafe);
}

void px_rtc_date_time_wr(const px_rtc_date_time_t * date_time)
{
    // Verify that date-time fields are valid
    if(!px_rtc_util_date_time_fields_valid(date_time))
    {
        return;
    }
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Enter initialization mode
    LL_RTC_EnableInitMode(RTC);
    while(!LL_RTC_IsActiveFlag_INIT(RTC))
    {
        ;
    }
    // Set new time
    RTC->TR =   (((uint32_t)px_rtc_bin_to_bcd(date_time->hour)) << RTC_TR_HU_Pos )
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->min )) << RTC_TR_MNU_Pos)
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->sec )) << RTC_TR_SU_Pos );
    // Set new date
    RTC->DR =   (((uint32_t)px_rtc_bin_to_bcd(date_time->year )) << RTC_DR_YU_Pos)
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->month)) << RTC_DR_MU_Pos)
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->day  )) << RTC_DR_DU_Pos);    
    // Exit Initialization mode
    LL_RTC_DisableInitMode(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);
}

void px_rtc_date_time_rd(px_rtc_date_time_t * date_time)
{
    uint32_t rtc_dr;
    uint32_t rtc_tr;
    uint32_t rtc_tr_old = 0xffffffff;    

    // Read time & date twice until both readings of time match
    while(true)
    {
        // Read time & date
        rtc_tr     = RTC->TR;
        rtc_dr     = RTC->DR;
        // Match?
        if(rtc_tr == rtc_tr_old)
        {
            break;
        }
        // Save previous time value
        rtc_tr_old = rtc_tr;
    };
    // Return date
    date_time->year  = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_YU_Pos)  & 0xff);
    date_time->month = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_MU_Pos)  & 0x1f);
    date_time->day   = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_DU_Pos)  & 0x3f);
    // Return time
    date_time->hour  = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_HU_Pos)  & 0x3f);
    date_time->min   = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_MNU_Pos) & 0x7f);
    date_time->sec   = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_SU_Pos)  & 0x7f);
}

void px_rtc_alarm_a_enable(const px_rtc_date_time_t * alarm, 
                           px_rtc_alarm_mask_t        alarm_mask)
{
    uint32_t rtc_alrmar;

    // Sanity check
    if(alarm_mask == PX_RTC_UTIL_ALARM_MASK_DIS)
    {
        return;
    }
    
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Disable Alarm
    LL_RTC_ALMA_Disable(RTC);
    while(!LL_RTC_IsActiveFlag_ALRAW(RTC))
    {
        ;
    }
    // Set alarm and mask
    rtc_alrmar = RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1;
    if(alarm_mask & PX_RTC_UTIL_ALARM_MASK_SEC)
    {
        rtc_alrmar |= (uint32_t)px_rtc_bin_to_bcd(alarm->sec) << RTC_ALRMAR_SU_Pos;
        rtc_alrmar &= ~RTC_ALRMAR_MSK1;
    }
    if(alarm_mask & PX_RTC_UTIL_ALARM_MASK_MIN)
    {
        rtc_alrmar |= (uint32_t)px_rtc_bin_to_bcd(alarm->min) << RTC_ALRMAR_MNU_Pos;
        rtc_alrmar &= ~RTC_ALRMAR_MSK2;
    }
    if(alarm_mask & PX_RTC_UTIL_ALARM_MASK_HOUR)
    {
        rtc_alrmar |= (uint32_t)px_rtc_bin_to_bcd(alarm->hour) << RTC_ALRMAR_HU_Pos;
        rtc_alrmar &= ~RTC_ALRMAR_MSK3;
    }
    if(alarm_mask & PX_RTC_UTIL_ALARM_MASK_DAY)
    {
        rtc_alrmar |= (uint32_t)px_rtc_bin_to_bcd(alarm->day) << RTC_ALRMAR_DU_Pos;
        rtc_alrmar &= ~RTC_ALRMAR_MSK3;
    }
    RTC->ALRMAR = rtc_alrmar;
    // Clear flag
    px_rtc_alarm_a_flag = false;
    // Clear interrupt flag
    LL_RTC_ClearFlag_ALRA(RTC);
    // Enable rising edge external interrupt on line 17
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    // Enable RTC interrupt
    NVIC_EnableIRQ(RTC_IRQn);
    // Enable Alarm A interrupt
    LL_RTC_EnableIT_ALRA(RTC);
    // Enable Alarm
    LL_RTC_ALMA_Enable(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);
}

void px_rtc_alarm_a_disable(void)
{
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Disable Alarm
    LL_RTC_ALMA_Disable(RTC);
    while(!LL_RTC_IsActiveFlag_ALRAW(RTC))
    {
        ;
    }
    // Disable Alarm A interrupt
    LL_RTC_DisableIT_ALRA(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);
}

bool px_rtc_alarm_a_has_expired(void)
{
    bool flag = px_rtc_alarm_a_flag;

    if(flag)
    {
        // Clear flag
        px_rtc_alarm_a_flag = false;
    }

    return flag;
}

void px_rtc_wakeup_tmr_enable(px_rtc_wakeup_presc_clk_t wakeup_presc_clk,
                              uint16_t                  wakeup_reload)
{
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Disable Wakeup Timer
    LL_RTC_WAKEUP_Disable(RTC);
    while(!LL_RTC_IsActiveFlag_WUTW(RTC))
    {
        ;
    }
    // Set Wakeup Timer clock prescaler
    LL_RTC_WAKEUP_SetClock(RTC, wakeup_presc_clk);
    // Set auto reload value
    LL_RTC_WAKEUP_SetAutoReload(RTC, wakeup_reload);
    // Clear flag
    px_rtc_wakeup_tmr_flag = false;
    // Clear WUT interrupt flag
    LL_RTC_ClearFlag_WUT(RTC);
    // Enable rising edge external interrupt on line 20
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);
    // Enable RTC interrupt
    NVIC_EnableIRQ(RTC_IRQn);
    // Enable Wakeup Timer interrupt
    LL_RTC_EnableIT_WUT(RTC);
    // Enable Wakeup Timer
    LL_RTC_WAKEUP_Enable(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);    
}

void px_rtc_wakeup_tmr_disable(void)
{
    // Disable RTC write protection
    LL_RTC_DisableWriteProtection(RTC);
    // Disable Wakeup Timer
    LL_RTC_WAKEUP_Disable(RTC);
    while(!LL_RTC_IsActiveFlag_WUTW(RTC))
    {
        ;
    }
    // Disable Wakeup Timer interrupt
    LL_RTC_DisableIT_WUT(RTC);
    // Enable write protection for RTC registers
    LL_RTC_EnableWriteProtection(RTC);
}

bool px_rtc_wakeup_tmr_has_expired(void)
{
    bool flag = px_rtc_wakeup_tmr_flag;

    if(flag)
    {
        // Clear flag
        px_rtc_wakeup_tmr_flag = false;
    }

    return flag;
}
