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
 
    Title:          px_adc.h : ADC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-04

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_adc.h"
#include "px_board.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_adc");

/// Internal data for each ADC handle
typedef struct px_adc_data_s
{
    /// ADC peripheral base register address
    ADC_TypeDef * adc_base_adr;
    /// Peripheral
    px_adc_per_t peripheral;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
} px_adc_data_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled ADC peripheral
#if PX_ADC_CFG_ADC1_EN
static px_adc_data_t px_adc1_data;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_adc_init_peripheral(ADC_TypeDef * adc_base_adr,
                                   px_adc_per_t  peripheral)
{
    // Enable peripheral clock
    switch(peripheral)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_PER_1:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }

    // Set ADC clock source
    switch(PX_ADC_CFG_CLK)
    {
    case PX_ADC_CFG_CLK_PCLK:
        LL_ADC_SetClock(adc_base_adr, LL_ADC_CLOCK_SYNC_PCLK_DIV1);
        break;
    case PX_ADC_CFG_CLK_PCLK_DIV2:
        LL_ADC_SetClock(adc_base_adr, LL_ADC_CLOCK_SYNC_PCLK_DIV2);
        break;
    case PX_ADC_CFG_CLK_PCLK_DIV4:
        LL_ADC_SetClock(adc_base_adr, LL_ADC_CLOCK_SYNC_PCLK_DIV4);
        break;
    default:
        PX_DBG_ERR("Unsupported ADC clock");
        break;
    }

    // Enable regulator
    LL_ADC_EnableInternalRegulator(adc_base_adr);

    // Set ADC sampling time
    LL_ADC_SetSamplingTimeCommonChannels(adc_base_adr, PX_ADC_CFG_SAMPLE_TIME);

    // Set ADC resolution
    LL_ADC_SetResolution(adc_base_adr, PX_ADC_CFG_RES_12_BITS << ADC_CFGR1_RES_Pos);

    // Set ADC oversampling and shift
    if(PX_ADC_CFG_OVERSAMPLING != PX_ADC_CFG_OVERSAMPLING_NONE)
    {
        LL_ADC_SetOverSamplingScope(adc_base_adr, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
        LL_ADC_ConfigOverSamplingRatioShift(adc_base_adr, 
                                            ((uint32_t)PX_ADC_CFG_OVERSAMPLING - 1) << ADC_CFGR2_OVSR_Pos,
                                            ((uint32_t)PX_ADC_CFG_OVERSAMPLING_SHIFT) << ADC_CFGR2_OVSS_Pos);
    }
    

    // Calibrate ADC
    LL_ADC_StartCalibration(adc_base_adr);
    while(LL_ADC_IsCalibrationOnGoing(adc_base_adr))
    {
        ;
    }

    // Enable peripheral
    LL_ADC_ClearFlag_ADRDY(adc_base_adr);
    LL_ADC_Enable(adc_base_adr);
    while(LL_ADC_IsActiveFlag_ADRDY(adc_base_adr) == 0)
    {
        ;
    }
}


static void px_adc_init_peripheral_data(px_adc_per_t    peripheral,
                                        px_adc_data_t * adc_data)
{
    // Set peripheral
    adc_data->peripheral = peripheral;
    // Set peripheral base address
    switch(peripheral)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_PER_1:
        adc_data->adc_base_adr = ADC1;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Clear reference counter
    adc_data->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_adc_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_ADC_CFG_ADC1_EN
    px_adc_init_peripheral_data(PX_ADC_PER_1, &px_adc1_data);
#endif
}

bool px_adc_open(px_adc_handle_t * handle,
                 px_adc_per_t      peripheral)
{
    px_adc_data_t * adc_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->adc_data = NULL;

    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_PER_1:
        adc_data = &px_adc1_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

    // Initialise peripheral
    px_adc_init_peripheral(adc_data->adc_base_adr,
                           peripheral);

    // Point handle to peripheral
    handle->adc_data = adc_data;

    // Success
    adc_data->open_counter++;
    return true;
}

bool px_adc_close(px_adc_handle_t * handle)
{
    px_adc_data_t * adc_data;
    ADC_TypeDef *   adc_base_adr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    adc_data = handle->adc_data;
    // Check that handle is open
    PX_DBG_ASSERT(adc_data != NULL);
    PX_DBG_ASSERT(adc_data->open_counter != 0);

    // Get ADC peripheral base register address
    adc_base_adr = adc_data->adc_base_adr;

    // Decrement open count
    adc_data->open_counter--;

    // More open handles referencing peripheral?
    if(adc_data->open_counter != 0)
    {        
        // Close handle
        handle->adc_data = NULL;
        // Success
        return true;
    }

    // Disable peripheral
    LL_ADC_Disable(adc_base_adr);
    while(LL_ADC_IsDisableOngoing(adc_base_adr))
    {
        ;
    }

    // Disable regulator
    LL_ADC_DisableInternalRegulator(adc_base_adr);
    while(LL_ADC_IsInternalRegulatorEnabled(adc_base_adr))
    {
        ;
    }

    // Disable peripheral clock
    switch(handle->adc_data->peripheral)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_PER_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
        break;
#endif
    default:
        break;
    }

    // Close handle
    handle->adc_data = NULL;

    // Success
    return true;
}

uint16_t px_adc_sample(px_adc_handle_t * handle, px_adc_ch_t ch)
{
    px_adc_data_t * adc_data;
    ADC_TypeDef *   adc_base_adr;
    uint16_t        data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    adc_data = handle->adc_data;
    // Check that handle is open
    PX_DBG_ASSERT(adc_data != NULL);
    PX_DBG_ASSERT(adc_data->open_counter != 0);

    // Get ADC peripheral base register address
    adc_base_adr = adc_data->adc_base_adr;

    // Select channel
    adc_base_adr->CHSELR = PX_ADC_CH_TO_BITMASK(ch);
    // Start conversion
    LL_ADC_REG_StartConversion(adc_base_adr);
    // Wait for conversion to finish
    while(LL_ADC_REG_IsConversionOngoing(adc_base_adr))
    {
        ;
    }

    // Read result
    data = LL_ADC_REG_ReadConversionData12(adc_base_adr);

    return data;
}
