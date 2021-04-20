/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_spi.h : SPI Peripheral Driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_spi.h"
#include "px_board.h"
#include "px_lib_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_spi");

/// Internal data for each SPI peripheral
typedef struct px_spi_per_s
{
    SPI_TypeDef *         spi_base_adr;     ///< SPI peripheral base register address
    DMA_Channel_TypeDef * dma_rx_base_adr;  ///< DMA RX channel base register address
    DMA_Channel_TypeDef * dma_tx_base_adr;  ///< DMA TX channel base register address
    px_spi_nr_t           spi_nr;           ///< Peripheral number
    uint8_t               open_counter;     ///< Number of open handles referencing peripheral
} px_spi_per_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled SPI peripheral
#if PX_SPI_CFG_SPI1_EN
static px_spi_per_t px_spi_per_1;
#endif

#if PX_SPI_CFG_SPI2_EN
static px_spi_per_t px_spi_per_2;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_spi_init_peripheral(SPI_TypeDef * spi_base_adr,
                                   px_spi_nr_t   spi_nr,
                                   uint32_t      spi_cr1_val)
{
    // Enable peripheral clocks
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_NR_1:
        // Enable peripheral clocks
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        // Configure DMA channel 2 (SPI1_RX)
        LL_DMA_SetPeriphAddress(DMA1, 2, (uint32_t)&SPI1->DR);
        DMA1_Channel2->CCR = DMA_CCR_MINC;
#if defined(STM32L0) || defined(STM32G0)
        LL_DMA_SetPeriphRequest(DMA1, 2, LL_DMA_REQUEST_1);
#endif
        // Configure DMA channel 3 (SPI1_TX)
        LL_DMA_SetPeriphAddress(DMA1, 3, (uint32_t)&SPI1->DR);
        DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_DIR;
#if defined(STM32L0) || defined(STM32G0)
        LL_DMA_SetPeriphRequest(DMA1, 3, LL_DMA_REQUEST_1);
#endif
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_NR_2:
        // Enable peripheral clocks
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
        // Configure DMA channel 4 (SPI2_RX)
        LL_DMA_SetPeriphAddress(DMA1, 4, (uint32_t)&SPI2->DR);
        DMA1_Channel4->CCR = DMA_CCR_MINC;
#if defined(STM32L0) || defined(STM32G0)
        LL_DMA_SetPeriphRequest(DMA1, 4, LL_DMA_REQUEST_2);
#endif
        // Configure DMA channel 5 (SPI2_TX)
        LL_DMA_SetPeriphAddress(DMA1, 5, (uint32_t)&SPI2->DR);
        DMA1_Channel5->CCR = DMA_CCR_MINC | DMA_CCR_DIR;
#if defined(STM32L0) || defined(STM32G0)
        LL_DMA_SetPeriphRequest(DMA1, 5, LL_DMA_REQUEST_2);
#endif
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return;
    }
    // Disable peripheral
    LL_SPI_Disable(spi_base_adr);
    // Set SPI Control Register 1:
    // master, data order, clock divisor, mode (clock polarity & phase)
    spi_base_adr->CR1 = spi_cr1_val;
    // Enable peripheral
    LL_SPI_Enable(spi_base_adr);
}

static void px_spi_update_cfg(SPI_TypeDef * spi_base_adr, uint32_t spi_cr1_val)
{
    // Must communication parameters change?
    if((spi_base_adr->CR1 & (~SPI_CR1_SPE_Msk)) != spi_cr1_val)
    {
        // Disable peripheral
        LL_SPI_Disable(spi_base_adr);
        // Change communication parameters
        spi_base_adr->CR1 = spi_cr1_val;
        // Enable peripheral
        LL_SPI_Enable(spi_base_adr);
    }
}

static void px_spi_init_peripheral_data(px_spi_nr_t    spi_nr,
                                        px_spi_per_t * spi_per)
{
    // Set peripheral number
    spi_per->spi_nr = spi_nr;
    // Set peripheral base address
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_NR_1:
        spi_per->spi_base_adr    = SPI1;
        spi_per->dma_rx_base_adr = DMA1_Channel2;
        spi_per->dma_tx_base_adr = DMA1_Channel3;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_NR_2:
        spi_per->spi_base_adr    = SPI2;
        spi_per->dma_rx_base_adr = DMA1_Channel4;
        spi_per->dma_tx_base_adr = DMA1_Channel5;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return;
    }
    // Clear open counter
    spi_per->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_spi_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_SPI_CFG_SPI1_EN
    px_spi_init_peripheral_data(PX_SPI_NR_1, &px_spi_per_1);
#endif
#if PX_SPI_CFG_SPI2_EN
    px_spi_init_peripheral_data(PX_SPI_NR_2, &px_spi_per_2);
#endif
}

bool px_spi_open(px_spi_handle_t * handle,
                   px_spi_nr_t     spi_nr,
                   uint8_t         cs_id)
{
    px_spi_per_t * spi_per;
    uint32_t       spi_cr1_val = 0;

#if PX_LOG
    // Verify that pointer to handle is not NULL
    if(handle == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif
    // Handle not initialised
    handle->spi_per = NULL;
    // Set pointer to peripheral data
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_NR_1:
        spi_per = &px_spi_per_1;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_NR_2:
        spi_per = &px_spi_per_2;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral specified");
        return false;
    }
#if PX_LOG
    // Check that px_spi_init() has been called
    if(spi_per->spi_base_adr == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif
    // Set SPI data order (MSB or LSB first)
    spi_cr1_val |= PX_SPI_CFG_DEFAULT_DATA_ORDER << SPI_CR1_LSBFIRST_Pos;
    // Set SPI clock divisor
    spi_cr1_val |= PX_SPI_CFG_DEFAULT_BAUD << SPI_CR1_BR_Pos;
    // Set SPI mode (clock polarity and phase)
    spi_cr1_val |= PX_SPI_CFG_DEFAULT_MODE << SPI_CR1_CPHA_Pos;
    // Set master mode of operation (not slave)
    spi_cr1_val |= SPI_CR1_MSTR;
    // Enable software NSS management
    spi_cr1_val |= SPI_CR1_SSM | SPI_CR1_SSI;
    // Save value for SPI Control Register 1 (SPI_CR1)
    handle->spi_cr1_val = spi_cr1_val;

    // Save Chip Select GPIO ID
    handle->cs_id = cs_id;
    // Set default Master Out dummy byte
    handle->mo_dummy_byte = 0x00;

    // Peripheral already initialized?
    if(spi_per->open_counter == 0)
    {
        // No. Initialise peripheral
        px_spi_init_peripheral(spi_per->spi_base_adr,
                               spi_nr,
                               spi_cr1_val);
    }
    // Point handle to peripheral
    handle->spi_per = spi_per;

    // Success
    spi_per->open_counter++;
    return true;
}

bool px_spi_open2(px_spi_handle_t * handle,
                  px_spi_nr_t       spi_nr,
                  uint8_t           cs_id,
                  px_spi_baud_t     baud,
                  px_spi_mode_t     mode,
                  px_spi_dord_t     data_order,
                  uint8_t           mo_dummy_byte)
{
    px_spi_per_t * spi_per;
    uint32_t       spi_cr1_val = 0;

#if PX_LOG
    // Verify that pointer to handle is not NULL
    if(handle == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif

    // Handle not initialised
    handle->spi_per = NULL;
    // Set pointer to peripheral data
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_NR_1:
        spi_per = &px_spi_per_1;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_NR_2:
        spi_per = &px_spi_per_2;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral specified");
        return false;
    }
#if PX_LOG
    // Check that px_spi_init() has been called
    if(spi_per->spi_base_adr == NULL)
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif

    // Set SPI data order (MSB or LSB first)
    if(data_order == PX_SPI_DATA_ORDER_LSB)
    {
        spi_cr1_val |= SPI_CR1_LSBFIRST;
    }
    // Set SPI clock divisor
    spi_cr1_val |= baud << SPI_CR1_BR_Pos;
    // Set SPI mode (clock polarity and phase)
    spi_cr1_val |= mode << SPI_CR1_CPHA_Pos;
    // Set master mode of operation (not slave)
    spi_cr1_val |= SPI_CR1_MSTR;
    // Enable software NSS management
    spi_cr1_val |= SPI_CR1_SSM | SPI_CR1_SSI;
    // Save value for SPI Control Register 1 (SPI_CR1)
    handle->spi_cr1_val = spi_cr1_val;

    // Save Chip Select GPIO ID
    handle->cs_id = cs_id;
    // Save Master Out dummy byte
    handle->mo_dummy_byte = mo_dummy_byte;

    // Peripheral already initialized?
    if(spi_per->open_counter == 0)
    {
        // No. Initialise peripheral
        px_spi_init_peripheral(spi_per->spi_base_adr,
                               spi_nr,
                               spi_cr1_val);
    }
    // Point handle to peripheral
    handle->spi_per = spi_per;

    // Success
    spi_per->open_counter++;
    return true;
}

bool px_spi_close(px_spi_handle_t * handle)
{
    px_spi_per_t * spi_per;
    SPI_TypeDef *  spi_base_adr;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->spi_per               == NULL)
       ||(handle->spi_per->spi_base_adr == NULL)
       ||(handle->spi_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return false;
    }
#endif
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Get SPI peripheral base register address
    spi_base_adr = spi_per->spi_base_adr;
    // Decrement open count
    spi_per->open_counter--;
    // More open handles referencing peripheral?
    if(spi_per->open_counter != 0)
    {
        // Yes. Only close handle
        handle->spi_per = NULL;
        // Success
        return true;
    }
    // Disable peripheral
    LL_SPI_Disable(spi_base_adr);
    // Disable peripheral clock
    switch(spi_per->spi_nr)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_NR_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_NR_2:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI2);
        break;
#endif
    default:
        break;
    }
    // Close handle
    handle->spi_per = NULL;

    // Success
    return true;
}

void px_spi_wr(px_spi_handle_t * handle,
               const void *      data,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_spi_per_t *  spi_per;
    SPI_TypeDef *   spi_base_adr;
    const uint8_t * data_wr_u8 = (const uint8_t *)data;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->spi_per               == NULL)
       ||(handle->spi_per->spi_base_adr == NULL)
       ||(handle->spi_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return;
    }
#endif

    // Set pointer to peripheral
    spi_per = handle->spi_per;

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    if(nr_of_bytes != 0)
    {
        // Get SPI peripheral base register address
        spi_base_adr = spi_per->spi_base_adr;
        PX_LOG_ASSERT(spi_base_adr != NULL);
        // Update communication parameters (if different)
        px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

        // Configure and enable DMA TX channel
        spi_per->dma_tx_base_adr->CMAR   = (uint32_t)data_wr_u8;
        spi_per->dma_tx_base_adr->CNDTR  = nr_of_bytes;
        spi_per->dma_tx_base_adr->CCR   |= DMA_CCR_EN;
        // Enable DMA request for TX
        LL_SPI_EnableDMAReq_TX(spi_base_adr);

        // Block until TX DMA transfer is complete
        switch(spi_per->spi_nr)
        {
    #if PX_SPI_CFG_SPI1_EN
        case PX_SPI_NR_1:
            while(!LL_DMA_IsActiveFlag_TC3(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC3(DMA1);
            break;
    #endif
    #if PX_SPI_CFG_SPI2_EN
        case PX_SPI_NR_2:
            while(!LL_DMA_IsActiveFlag_TC5(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC5(DMA1);
            break;
    #endif
        default:
            PX_LOG_E("Invalid peripheral");
            break;
        }
        // Wait until SPI transmit of last byte is complete
        while(!LL_SPI_IsActiveFlag_TXE(spi_base_adr))
        {
            ;
        }
        while(LL_SPI_IsActiveFlag_BSY(spi_base_adr))
        {
            ;
        }
        // Clear data overrun
        LL_SPI_ClearFlag_OVR(spi_base_adr);

        // Disable DMA TX channel
        spi_per->dma_tx_base_adr->CCR &= ~DMA_CCR_EN;
        // Disable DMA request for TX
        spi_base_adr->CR2 = 0;
    }

    if(flags & PX_SPI_FLAG_STOP)
    {
        // Take Chip Select High
        PX_SPI_CFG_CS_HI(handle->cs_id);
    }
}

void px_spi_rd(px_spi_handle_t * handle,
               void *            data,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_spi_per_t *  spi_per;
    SPI_TypeDef *   spi_base_adr;
    uint8_t *       data_rd_u8 = (uint8_t *)data;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->spi_per               == NULL)
       ||(handle->spi_per->spi_base_adr == NULL)
       ||(handle->spi_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return;
    }
#endif

    // Set pointer to peripheral
    spi_per = handle->spi_per;

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    if(nr_of_bytes != 0)
    {
        // Get SPI peripheral base register address
        spi_base_adr = spi_per->spi_base_adr;
        PX_LOG_ASSERT(spi_base_adr != NULL);
        // Update communication parameters (if different)
        px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

        // Enable DMA request for RX
        LL_SPI_EnableDMAReq_RX(spi_base_adr);
        // Configure and enable DMA RX channel
        spi_per->dma_rx_base_adr->CMAR   = (uint32_t)data_rd_u8;
        spi_per->dma_rx_base_adr->CNDTR  = nr_of_bytes;
        spi_per->dma_rx_base_adr->CCR   |= DMA_CCR_EN;
        // Configure and enable DMA TX channel
        spi_per->dma_tx_base_adr->CMAR   = (uint32_t)&handle->mo_dummy_byte;
        spi_per->dma_tx_base_adr->CNDTR  = nr_of_bytes;
        spi_per->dma_tx_base_adr->CCR   &= ~DMA_CCR_MINC;
        spi_per->dma_tx_base_adr->CCR   |= DMA_CCR_EN;
        // Enable DMA request for TX
        LL_SPI_EnableDMAReq_TX(spi_base_adr);

        // Block until RX DMA transfer is complete
        switch(spi_per->spi_nr)
        {
    #if PX_SPI_CFG_SPI1_EN
        case PX_SPI_NR_1:
            while(!LL_DMA_IsActiveFlag_TC2(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC2(DMA1);
            break;
    #endif
    #if PX_SPI_CFG_SPI2_EN
        case PX_SPI_NR_2:
            while(!LL_DMA_IsActiveFlag_TC4(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC4(DMA1);
            break;
    #endif
        default:
            PX_LOG_E("Invalid peripheral");
            break;
        }

        // Disable DMA RX channel
        spi_per->dma_rx_base_adr->CCR &= ~DMA_CCR_EN;
        // Disable DMA TX channel
        spi_per->dma_tx_base_adr->CCR &= ~DMA_CCR_EN;
        spi_per->dma_tx_base_adr->CCR |= DMA_CCR_MINC;
        // Disable DMA request for RX and TX
        spi_base_adr->CR2 = 0;
    }

    if(flags & PX_SPI_FLAG_STOP)
    {
        // Take Chip Select High
        PX_SPI_CFG_CS_HI(handle->cs_id);
    }
}

void px_spi_xc(px_spi_handle_t * handle,
               const void *      data_wr,
               void *            data_rd,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_spi_per_t *  spi_per;
    SPI_TypeDef *   spi_base_adr;
    const uint8_t * data_wr_u8 = (const uint8_t *)data_wr;
    uint8_t *       data_rd_u8 = (uint8_t *)data_rd;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->spi_per               == NULL)
       ||(handle->spi_per->spi_base_adr == NULL)
       ||(handle->spi_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return;
    }
#endif

    // Set pointer to peripheral
    spi_per = handle->spi_per;

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    // Get SPI peripheral base register address
    spi_base_adr = spi_per->spi_base_adr;
    PX_LOG_ASSERT(spi_base_adr != NULL);
    // Update communication parameters (if different)
    px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

    if(nr_of_bytes != 0)
    {
        // Enable DMA request for RX
        LL_SPI_EnableDMAReq_RX(spi_base_adr);
        // Configure and enable DMA RX channel
        spi_per->dma_rx_base_adr->CMAR   = (uint32_t)data_rd_u8;
        spi_per->dma_rx_base_adr->CNDTR  = nr_of_bytes;
        spi_per->dma_rx_base_adr->CCR   |= DMA_CCR_EN;
        // Configure and enable DMA TX channel
        spi_per->dma_tx_base_adr->CMAR   = (uint32_t)data_wr_u8;
        spi_per->dma_tx_base_adr->CNDTR  = nr_of_bytes;
        spi_per->dma_tx_base_adr->CCR   |= DMA_CCR_EN;
        // Enable DMA request for TX
        LL_SPI_EnableDMAReq_TX(spi_base_adr);

        // Block until RX DMA transfer is complete
        switch(spi_per->spi_nr)
        {
    #if PX_SPI_CFG_SPI1_EN
        case PX_SPI_NR_1:
            while(!LL_DMA_IsActiveFlag_TC2(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC2(DMA1);
            break;
    #endif
    #if PX_SPI_CFG_SPI2_EN
        case PX_SPI_NR_2:
            while(!LL_DMA_IsActiveFlag_TC4(DMA1))
            {
                ;
            }
            LL_DMA_ClearFlag_TC4(DMA1);
            break;
    #endif
        default:
            PX_LOG_E("Invalid peripheral");
            break;
        }

        // Disable DMA RX channel
        spi_per->dma_rx_base_adr->CCR &= ~DMA_CCR_EN;
        // Disable DMA TX channel
        spi_per->dma_tx_base_adr->CCR &= ~DMA_CCR_EN;
        // Disable DMA request for RX and TX
        spi_base_adr->CR2 = 0;
    }

    if(flags & PX_SPI_FLAG_STOP)
    {
        // Take Chip Select High
        PX_SPI_CFG_CS_HI(handle->cs_id);
    }
}

void px_spi_ioctl_change_baud(px_spi_handle_t * handle,
                              px_spi_baud_t     baud)
{
    px_spi_per_t * spi_per;
    SPI_TypeDef *  spi_base_adr;
    uint32_t       spi_cr1_val;

#if PX_LOG
    // Check handle
    if(  (handle                        == NULL)
       ||(handle->spi_per               == NULL)
       ||(handle->spi_per->spi_base_adr == NULL)
       ||(handle->spi_per->open_counter == 0   )  )
    {
        PX_LOG_ASSERT(false);
        return;
    }
#endif

    // Set pointer to peripheral
    spi_per = handle->spi_per;

    // Get SPI peripheral base register address
    spi_base_adr = spi_per->spi_base_adr;
    PX_LOG_ASSERT(spi_base_adr != NULL);
    // Disable peripheral
    LL_SPI_Disable(spi_base_adr);
    // Set new SPI clock divisor
    spi_cr1_val  = handle->spi_cr1_val;
    spi_cr1_val &= ~SPI_CR1_BR_Msk;
    spi_cr1_val |= baud << SPI_CR1_BR_Pos;
    // Store new SPI clock divisor
    handle->spi_cr1_val = spi_cr1_val;
    // Set SPI Control Register 1 value
    spi_base_adr->CR1 = spi_cr1_val;
    // Enable peripheral
    LL_SPI_Enable(spi_base_adr);
}

px_spi_baud_t px_spi_util_baud_hz_to_clk_div(uint32_t baud_hz)
{
    uint32_t      actual_baud_hz;
    px_spi_baud_t baud;

    // Start with a divisor of 2
    actual_baud_hz = PX_BOARD_PER_CLK_HZ >> 1;
    baud           = PX_SPI_BAUD_CLK_DIV_2;

    // Closest baud equal or less than specified baud found?
    while(actual_baud_hz > baud_hz)
    {
        // Divide by two
        actual_baud_hz >>= 1;
        // Stop if divisor reaches maximum
        if(++baud == PX_SPI_BAUD_CLK_DIV_256)
        {
            break;
        }
    }
    return baud;
}

uint32_t px_spi_util_clk_div_to_baud_hz(px_spi_baud_t baud)
{
    return (((uint32_t)PX_BOARD_PER_CLK_HZ) >> (baud+1));
}
