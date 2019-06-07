/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
    RTC->DR =   (((uint32_t)0x00) << RTC_DR_YU_Pos)
              | (((uint32_t)0x04) << RTC_DR_WDU_Pos)
              | (((uint32_t)0x01) << RTC_DR_MU_Pos)
              | (((uint32_t)0x01) << RTC_DR_DU_Pos);

    // Reset time to 00:00:00
    RTC->TR =   (((uint32_t)0x00) << RTC_TR_HU_Pos )
              | (((uint32_t)0x00) << RTC_TR_MNU_Pos)
              | (((uint32_t)0x00) << RTC_TR_SU_Pos );

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
    RTC->TR =   (((uint32_t)px_rtc_bin_to_bcd(date_time->hour)) << RTC_TR_HU_Pos)
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->min )) << RTC_TR_MNU_Pos)
              | (((uint32_t)px_rtc_bin_to_bcd(date_time->sec )) << RTC_TR_SU_Pos);

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
    uint32_t rtc_tr, rtc_tr_cmp;
    uint32_t rtc_dr;

    do
    {
        rtc_tr     = RTC->TR;
        rtc_dr     = RTC->DR;
        rtc_tr_cmp = RTC->TR;
        rtc_dr     = RTC->DR;
    }
    while(rtc_tr != rtc_tr_cmp);

    date_time->year  = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_YU_Pos)  & 0xff);
    date_time->month = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_MU_Pos)  & 0x1f);
    date_time->day   = px_rtc_bcd_to_bin((rtc_dr >> RTC_DR_DU_Pos)  & 0x3f);

    date_time->hour  = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_HU_Pos)  & 0x3f);
    date_time->min   = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_MNU_Pos) & 0x7f);
    date_time->sec   = px_rtc_bcd_to_bin((rtc_tr >> RTC_TR_SU_Pos)  & 0x7f);
}
