/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
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
typedef struct px_adc_per_s
{
    ADC_TypeDef * adc_base_adr; ///< ADC peripheral base register address    
    px_adc_nr_t adc_nr;         ///< ADC peripheral number
    uint8_t open_counter;       ///< Number of open handles referencing peripheral    
} px_adc_per_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled ADC peripheral
#if PX_ADC_CFG_ADC1_EN
static px_adc_per_t px_adc_per_1;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_adc_init_peripheral(ADC_TypeDef * adc_base_adr,
                                   px_adc_nr_t   adc_nr)
{
    // Enable peripheral clock
    switch(adc_nr)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_NR_1:
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


static void px_adc_init_peripheral_data(px_adc_nr_t    adc_nr,
                                        px_adc_per_t * adc_per)
{
    // Set peripheral number
    adc_per->adc_nr = adc_nr;
    // Set peripheral base address
    switch(adc_nr)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_NR_1:
        adc_per->adc_base_adr = ADC1;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Clear reference counter
    adc_per->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_adc_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_ADC_CFG_ADC1_EN
    px_adc_init_peripheral_data(PX_ADC_NR_1, &px_adc_per_1);
#endif
}

bool px_adc_open(px_adc_handle_t * handle,
                 px_adc_nr_t       adc_nr)
{
    px_adc_per_t * adc_per;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->adc_per = NULL;
    // Set pointer to peripheral data
    switch(adc_nr)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_NR_1:
        adc_per = &px_adc_per_1;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }
    // Initialise peripheral
    px_adc_init_peripheral(adc_per->adc_base_adr,
                           adc_nr);
    // Point handle to peripheral
    handle->adc_per = adc_per;
    // Success
    adc_per->open_counter++;
    return true;
}

bool px_adc_close(px_adc_handle_t * handle)
{
    px_adc_per_t * adc_per;
    ADC_TypeDef *  adc_base_adr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    adc_per = handle->adc_per;
    // Check that handle is open
    PX_DBG_ASSERT(adc_per != NULL);
    PX_DBG_ASSERT(adc_per->open_counter != 0);

    // Get ADC peripheral base register address
    adc_base_adr = adc_per->adc_base_adr;
    // Decrement open count
    adc_per->open_counter--;
    // More open handles referencing peripheral?
    if(adc_per->open_counter != 0)
    {        
        // Close handle
        handle->adc_per = NULL;
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
    switch(handle->adc_per->adc_nr)
    {
#if PX_ADC_CFG_ADC1_EN
    case PX_ADC_NR_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_ADC1);
        break;
#endif
    default:
        break;
    }

    // Close handle
    handle->adc_per = NULL;
    // Success
    return true;
}

uint16_t px_adc_sample(px_adc_handle_t * handle, px_adc_ch_t ch)
{
    px_adc_per_t * adc_per;
    ADC_TypeDef *  adc_base_adr;
    uint16_t       data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    adc_per = handle->adc_per;
    // Check that handle is open
    PX_DBG_ASSERT(adc_per != NULL);
    PX_DBG_ASSERT(adc_per->open_counter != 0);

    // Get ADC peripheral base register address
    adc_base_adr = adc_per->adc_base_adr;

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
