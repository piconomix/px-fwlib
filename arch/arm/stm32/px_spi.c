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
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_spi");

/// Internal data for each SPI handle
typedef struct px_spi_data_s
{
    /// SPI peripheral base register address
    SPI_TypeDef * spi_base_adr;
    /// Peripheral
    px_spi_per_t peripheral;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
} px_spi_data_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled SPI peripheral
#if PX_SPI_CFG_SPI1_EN
static px_spi_data_t px_spi1_data;
#endif

#if PX_SPI_CFG_SPI2_EN
static px_spi_data_t px_spi2_data;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_spi_init_peripheral(SPI_TypeDef * spi_base_adr,
                                   px_spi_per_t  peripheral,
                                   uint32_t      spi_cr1_val)
{
    // Enable peripheral clock
    switch(peripheral)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_PER_1:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_PER_2:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Disable peripheral
    LL_SPI_Disable(spi_base_adr);
    // Set SPI Control Register 1:
    // master, data order, clock divisor, mode (clock polarity & phase)
    spi_base_adr->CR1 = spi_cr1_val;
#if 0
    // Set SS output enable
    spi_base_adr->CR2 |= SPI_CR2_SSOE;
#endif
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

static void px_spi_init_peripheral_data(px_spi_per_t    peripheral,
                                        px_spi_data_t * spi_data)
{
    // Set peripheral
    spi_data->peripheral = peripheral;
    // Set peripheral base address
    switch(peripheral)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_PER_1:
        spi_data->spi_base_adr = SPI1;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_PER_2:
        spi_data->spi_base_adr = SPI2;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return;
    }
    // Clear reference counter
    spi_data->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_spi_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_SPI_CFG_SPI1_EN
    px_spi_init_peripheral_data(PX_SPI_PER_1, &px_spi1_data);
#endif
#if PX_SPI_CFG_SPI2_EN
    px_spi_init_peripheral_data(PX_SPI_PER_2, &px_spi2_data);
#endif
}

bool px_spi_open(px_spi_handle_t * handle,
                   px_spi_per_t    peripheral,
                   uint8_t         cs_id)
{
    px_spi_data_t * spi_data;
    uint32_t        spi_cr1_val = 0;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Handle not initialised
    handle->spi_data = NULL;
    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_PER_1:
        spi_data = &px_spi1_data;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_PER_2:
        spi_data = &px_spi2_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

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

    // Initialise peripheral
    px_spi_init_peripheral(spi_data->spi_base_adr,
                           peripheral,
                           spi_cr1_val);
    // Point handle to peripheral
    handle->spi_data = spi_data;

    // Success
    spi_data->open_counter++;
    return true;
}

bool px_spi_open2(px_spi_handle_t * handle,
                  px_spi_per_t      peripheral,
                  uint8_t           cs_id,
                  px_spi_baud_t     baud, 
                  px_spi_mode_t     mode,
                  px_spi_dord_t     data_order,
                  uint8_t           mo_dummy_byte)
{
    px_spi_data_t * spi_data;
    uint32_t        spi_cr1_val = 0;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Handle not initialised
    handle->spi_data = NULL;
    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_PER_1:
        spi_data = &px_spi1_data;
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_PER_2:
        spi_data = &px_spi2_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

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

    // Initialise peripheral
    px_spi_init_peripheral(spi_data->spi_base_adr,
                           peripheral,
                           spi_cr1_val);
    // Point handle to peripheral
    handle->spi_data = spi_data;

    // Success
    spi_data->open_counter++;
    return true;
}

bool px_spi_close(px_spi_handle_t * handle)
{
    px_spi_data_t * spi_data;
    SPI_TypeDef *   spi_base_adr;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_data = handle->spi_data;
    // Check that handle is open
    PX_DBG_ASSERT(spi_data != NULL);
    PX_DBG_ASSERT(spi_data->open_counter != 0);

    // Get SPI peripheral base register address
    spi_base_adr = spi_data->spi_base_adr;
    // Decrement open count
    spi_data->open_counter--;
    // More open handles referencing peripheral?
    if(spi_data->open_counter != 0)
    {        
        // Close handle
        handle->spi_data = NULL;
        // Success
        return true;
    }
    // Disable peripheral
    LL_SPI_Disable(spi_base_adr);
    // Disable peripheral clock
    switch(handle->spi_data->peripheral)
    {
#if PX_SPI_CFG_SPI1_EN
    case PX_SPI_PER_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
        break;
#endif
#if PX_SPI_CFG_SPI2_EN
    case PX_SPI_PER_2:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_SPI2);
        break;
#endif
    default:
        break;
    }
    // Close handle
    handle->spi_data = NULL;

    // Success
    return true;
}

void px_spi_wr(px_spi_handle_t * handle,
               const void *      data,
               size_t            nr_of_bytes,
               uint8_t           flags)
{
    px_spi_data_t * spi_data;
    SPI_TypeDef *   spi_base_adr;
    const uint8_t * data_wr_u8 = (const uint8_t *)data;
    uint8_t         data_rx_u8;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_data = handle->spi_data;
    // Check that handle is open
    PX_DBG_ASSERT(spi_data != NULL);
    PX_DBG_ASSERT(spi_data->open_counter != 0);

    // Get SPI peripheral base register address
    spi_base_adr = spi_data->spi_base_adr;
    PX_DBG_ASSERT(spi_base_adr != NULL);
    // Update communication parameters (if different)
    px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    while(nr_of_bytes)
    {
        // Wait until peripheral is ready to accept new transmit data
        while(!LL_SPI_IsActiveFlag_TXE(spi_base_adr))
        {
            ;
        }
        // Buffer byte to be sent
        LL_SPI_TransmitData8(spi_base_adr, *data_wr_u8++);
        // Wait until byte has been transfered
        while(!LL_SPI_IsActiveFlag_RXNE(spi_base_adr))
        {
            ;
        }
        // Read received byte (and discard)
        data_rx_u8 = LL_SPI_ReceiveData8(spi_base_adr);
        // Avoid compiler warning that variable was set but not used
        (void)data_rx_u8;
        // Next byte
        nr_of_bytes--;
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
    px_spi_data_t * spi_data;
    SPI_TypeDef *   spi_base_adr;
    uint8_t *       data_rd_u8 = (uint8_t *)data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_data = handle->spi_data;
    // Check that handle is open
    PX_DBG_ASSERT(spi_data != NULL);
    PX_DBG_ASSERT(spi_data->open_counter != 0);

    // Get SPI peripheral base register address
    spi_base_adr = spi_data->spi_base_adr;
    PX_DBG_ASSERT(spi_base_adr != NULL);
    // Update communication parameters (if different)
    px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    // Repeat until all of the bytes have been read
    while(nr_of_bytes)
    {
        // Wait until peripheral is ready to accept new transmit data
        while(!LL_SPI_IsActiveFlag_TXE(spi_base_adr))
        {
            ;
        }
        // Buffer dummy byte to be sent
        LL_SPI_TransmitData8(spi_base_adr, handle->mo_dummy_byte);
        // Wait until byte has been transfered
        while(!LL_SPI_IsActiveFlag_RXNE(spi_base_adr))
        {
            ;
        }
        // Read received byte
        *data_rd_u8++ = LL_SPI_ReceiveData8(spi_base_adr);
        // Next byte
        nr_of_bytes--;
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
    px_spi_data_t * spi_data;
    SPI_TypeDef *   spi_base_adr;
    const uint8_t * data_wr_u8 = (const uint8_t *)data_wr;
    uint8_t *       data_rd_u8 = (uint8_t *)data_rd;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_data = handle->spi_data;
    // Check that handle is open
    PX_DBG_ASSERT(spi_data != NULL);
    PX_DBG_ASSERT(spi_data->open_counter != 0);

    // Get SPI peripheral base register address
    spi_base_adr = spi_data->spi_base_adr;
    PX_DBG_ASSERT(spi_base_adr != NULL);
    // Update communication parameters (if different)
    px_spi_update_cfg(spi_base_adr, handle->spi_cr1_val);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    // Repeat until all of the bytes have been exchanged
    while(nr_of_bytes)
    {
        // Wait until peripheral is ready to accept new transmit data
        while(!LL_SPI_IsActiveFlag_TXE(spi_base_adr))
        {
            ;
        }
        // Buffer byte to be sent
        LL_SPI_TransmitData8(spi_base_adr, *data_wr_u8++);
        // Wait until byte has been transfered
        while(!LL_SPI_IsActiveFlag_RXNE(spi_base_adr))
        {
            ;
        }
        // Read received byte
        *data_rd_u8++ = LL_SPI_ReceiveData8(spi_base_adr);
        // Next byte
        nr_of_bytes--;
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
    px_spi_data_t * spi_data;
    SPI_TypeDef *   spi_base_adr;
    uint32_t        spi_cr1_val;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_data = handle->spi_data;
    // Check that handle is open
    PX_DBG_ASSERT(spi_data != NULL);
    PX_DBG_ASSERT(spi_data->open_counter != 0);

    // Get SPI peripheral base register address
    spi_base_adr = spi_data->spi_base_adr;
    PX_DBG_ASSERT(spi_base_adr != NULL);
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
