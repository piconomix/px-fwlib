/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_dac.h : DAC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_dac.h"
#include "px_board.h"
#include "px_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_dac");

/// Internal data for each DAC handle
typedef struct px_dac_per_s
{
    DAC_TypeDef * dac_base_adr;     ///< DAC peripheral base register address
    px_dac_nr_t   dac_nr;           ///< Peripheral
    uint8_t       open_counter;     ///< Number of open handles referencing peripheral
} px_dac_per_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled DAC peripheral
#if PX_DAC_CFG_DAC1_EN
static px_dac_per_t px_dac_per_1;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_dac_init_per(DAC_TypeDef * dac_base_adr,
                            px_dac_nr_t   dac_nr)
{
    uint32_t dac_channel = 0;

    // Enable peripheral clock
    switch(dac_nr)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_NR_1:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
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


static void px_dac_init_per_data(px_dac_nr_t    dac_nr,
                                 px_dac_per_t * dac_per)
{
    // Set peripheral
    dac_per->dac_nr = dac_nr;
    // Set peripheral base address
    switch(dac_nr)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_NR_1:
        dac_per->dac_base_adr = DAC1;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return;
    }
    // Clear reference counter
    dac_per->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_dac_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_DAC_CFG_DAC1_EN
    px_dac_init_per_data(PX_DAC_NR_1, &px_dac_per_1);
#endif
}

bool px_dac_open(px_dac_handle_t * handle,
                 px_dac_nr_t       dac_nr)
{
    px_dac_per_t * dac_per;

#if PX_LOG
    // Verify that pointer to handle is not NULL
    if(handle == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif

    // Handle not initialised
    handle->dac_per = NULL;
    // Set pointer to peripheral data
    switch(dac_nr)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_NR_1:
        dac_per = &px_dac_per_1;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral specified");
        return false;
    }
#if PX_LOG
    // Check that px_adc_init() has been called
    if(dac_per->dac_base_adr == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif
    // Initialise peripheral
    px_dac_init_per(dac_per->dac_base_adr, dac_nr);
    // Point handle to peripheral
    handle->dac_per = dac_per;
    // Success
    dac_per->open_counter++;
    return true;
}

bool px_dac_close(px_dac_handle_t * handle)
{
    px_dac_per_t * dac_per;
    DAC_TypeDef *  dac_base_adr;
    uint32_t       dac_channel = 0;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->dac_per               == NULL)
       ||(handle->dac_per->dac_base_adr == NULL)
       ||(handle->dac_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif

    // Set pointer to peripheral
    dac_per = handle->dac_per;
    // Get DAC peripheral base register address
    dac_base_adr = dac_per->dac_base_adr;
    // Decrement open count
    dac_per->open_counter--;
    // More open handles referencing peripheral?
    if(dac_per->open_counter != 0)
    {        
        // Close handle
        handle->dac_per = NULL;
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
    switch(handle->dac_per->dac_nr)
    {
#if PX_DAC_CFG_DAC1_EN
    case PX_DAC_NR_1:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_DAC1);
        break;
#endif
    default:
        break;
    }
    // Close handle
    handle->dac_per = NULL;
    // Success
    return true;
}

void px_dac_wr(px_dac_handle_t * handle, 
               px_dac_channel_t  channel,
               uint16_t          data)
{
    px_dac_per_t * dac_per;
    DAC_TypeDef *  dac_base_adr;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->dac_per               == NULL)
       ||(handle->dac_per->dac_base_adr == NULL)
       ||(handle->dac_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return;
    }
#endif

    // Set pointer to peripheral
    dac_per = handle->dac_per;
    // Get DAC peripheral base register address
    dac_base_adr = dac_per->dac_base_adr;
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
        PX_LOG_E("Invalid DAC channel specified");
        break;
    }
}
