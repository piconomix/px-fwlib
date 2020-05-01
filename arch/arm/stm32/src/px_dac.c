/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_dac.h : DAC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_dac.h"
#include "px_board.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_dac");

/// Internal data for each DAC handle
typedef struct px_dac_data_s
{
    /// DAC peripheral base register address
    DAC_TypeDef * dac_base_adr;
    /// Peripheral
    px_dac_per_t peripheral;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
} px_dac_data_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled DAC peripheral
#if PX_DAC_CFG_DAC1_EN
static px_dac_data_t px_dac1_data;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_dac_init_peripheral(DAC_TypeDef * dac_base_adr,
                                   px_dac_per_t  peripheral)
{
    uint32_t dac_channel = 0;

    // Enable peripheral clock
    switch(peripheral)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_PER_1:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }

    // Enable peripheral
#if PX_DAC_CFG_DAC1_CH1_EN
    dac_channel |= LL_DAC_CHANNEL_1;
#endif
#if PX_DAC_CFG_DAC1_CH2_EN
    dac_channel |= LL_DAC_CHANNEL_2;
#endif
    LL_DAC_Enable(dac_base_adr, dac_channel);
}


static void px_dac_init_peripheral_data(px_dac_per_t    peripheral,
                                        px_dac_data_t * dac_data)
{
    // Set peripheral
    dac_data->peripheral = peripheral;
    // Set peripheral base address
    switch(peripheral)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_PER_1:
        dac_data->dac_base_adr = DAC1;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Clear reference counter
    dac_data->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_dac_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_DAC_CFG_DAC1_EN
    px_dac_init_peripheral_data(PX_DAC_PER_1, &px_dac1_data);
#endif
}

bool px_dac_open(px_dac_handle_t * handle,
                 px_dac_per_t      peripheral)
{
    px_dac_data_t * dac_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->dac_data = NULL;

    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_PER_1:
        dac_data = &px_dac1_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

    // Initialise peripheral
    px_dac_init_peripheral(dac_data->dac_base_adr,
                           peripheral);

    // Point handle to peripheral
    handle->dac_data = dac_data;

    // Success
    dac_data->open_counter++;
    return true;
}

bool px_dac_close(px_dac_handle_t * handle)
{
    px_dac_data_t * dac_data;
    DAC_TypeDef *   dac_base_adr;
    uint32_t        dac_channel = 0;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    dac_data = handle->dac_data;
    // Check that handle is open
    PX_DBG_ASSERT(dac_data != NULL);
    PX_DBG_ASSERT(dac_data->open_counter != 0);

    // Get DAC peripheral base register address
    dac_base_adr = dac_data->dac_base_adr;

    // Decrement open count
    dac_data->open_counter--;

    // More open handles referencing peripheral?
    if(dac_data->open_counter != 0)
    {        
        // Close handle
        handle->dac_data = NULL;
        // Success
        return true;
    }

    // Disable peripheral
#if PX_DAC_CFG_DAC1_CH1_EN
    dac_channel |= LL_DAC_CHANNEL_1;
#endif
#if PX_DAC_CFG_DAC1_CH2_EN
    dac_channel |= LL_DAC_CHANNEL_2;
#endif
    LL_DAC_Disable(dac_base_adr, dac_channel);

    // Disable peripheral clock
    switch(handle->dac_data->peripheral)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_PER_1:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_DAC1);
        break;
#endif
    default:
        break;
    }

    // Close handle
    handle->dac_data = NULL;

    // Success
    return true;
}

void px_dac_wr(px_dac_handle_t * handle, 
               px_dac_channel_t  channel,
               uint16_t          data)
{
    px_dac_data_t * dac_data;
    DAC_TypeDef *   dac_base_adr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    dac_data = handle->dac_data;
    // Check that handle is open
    PX_DBG_ASSERT(dac_data != NULL);
    PX_DBG_ASSERT(dac_data->open_counter != 0);

    // Get DAC peripheral base register address
    dac_base_adr = dac_data->dac_base_adr;

    // Ouput data
    switch(channel)
    {
    case PX_DAC_CHANNEL_1:
        dac_base_adr->DHR12R1 = data;
        break;
    case PX_DAC_CHANNEL_2:
        dac_base_adr->DHR12R2 = data;
        break;
    default:
        PX_DBG_ERR("Invalid DAC channel specified");
        break;
    }
}
