/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_spi.h : SPI Peripheral Driver
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-23

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_spi.h"
#include "px_spi_defs.h"
#include "px_board.h"
#include "px_dbg.h"

// Issue warning that SS pin must be an output or input with a pull-up
#warning "There is a caveat with the SS pin when using the SPI in master mode! See documentation in 'px_spi.h'"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("spi");

/// Internal data for each SPI handle
typedef struct px_spi_per_s
{
    px_spi_nr_t spi_nr;     ///< SPI peripheral number
    uint8_t open_counter;   /// Number of open handles referencing peripheral    
} px_spi_per_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled SPI peripheral
#if PX_SPI_CFG_SPI0_EN
static px_spi_per_t px_spi_per_0;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_spi_init_peripheral(px_spi_nr_t spi_nr,
                                   uint8_t     spcr,
                                   bool        use_spi2x)
{
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI0_EN
    case PX_SPI_NR_0:
        // Initialise SPI Control Register
        SPCR = spcr;
        // Use 2x SPI Clock rate?
        if(use_spi2x)
        {
            PX_BIT_SET_HI(SPSR, SPI2X);
        }
        else
        {
            PX_BIT_SET_LO(SPSR, SPI2X);
        }
        break;
#endif
    default:
        PX_DBG_E("Invalid peripheral");
        return;
    }
}

static void px_spi_update_cfg(px_spi_nr_t spi_nr,
                              uint8_t     spcr,
                              bool        use_spi2x)
{
    px_spi_init_peripheral(spi_nr, spcr, use_spi2x);
}

static void px_spi_init_peripheral_data(px_spi_nr_t    spi_nr,
                                        px_spi_per_t * spi_data)
{
    // Set peripheral
    spi_data->spi_nr = spi_nr;
    // Clear reference counter
    spi_data->open_counter = 0;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_spi_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_SPI_CFG_SPI0_EN
    px_spi_init_peripheral_data(PX_SPI_NR_0, &px_spi_per_0);
#endif
}

bool px_spi_open(px_spi_handle_t * handle,
                 px_spi_nr_t      spi_nr,
                 uint8_t           cs_id)
{
    px_spi_per_t * spi_per;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->spi_per = NULL;
    // Set pointer to peripheral data
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI0_EN
    case PX_SPI_NR_0:
        spi_per = &px_spi_per_0;
        break;
#endif
    default:
        PX_DBG_E("Invalid peripheral specified");
        return false;
    }

    // Save Chip Select GPIO ID
    handle->cs_id = cs_id;
    // Use default value for SPI Control Register
    handle->spcr = PX_SPI_CFG_SPCR;
    // Use default value for "Use SPI 2x clock rate"
    handle->use_spi2x = PX_SPI_CFG_USE_SPI2X;
    // Set default value for "Master Out dummy byte"
    handle->mo_dummy_byte = 0x00;

    // Initialise peripheral
    px_spi_init_peripheral(spi_nr,
                           handle->spcr,
                           handle->use_spi2x);
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
    uint8_t         spcr = 0;
    bool            use_spi2x;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->spi_per = NULL;
    // Set pointer to peripheral data
    switch(spi_nr)
    {
#if PX_SPI_CFG_SPI0_EN
    case PX_SPI_NR_0:
        spi_per = &px_spi_per_0;
        break;
#endif
    default:
        PX_DBG_E("Invalid peripheral specified");
        return false;
    }

    // Save Chip Select GPIO ID
    handle->cs_id = cs_id;
    // Initial value for SPCR
    spcr =   (0<<SPIE) | (1<<SPE)  | (0<<DORD) | (1<<MSTR)
           | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
    // Set baud
    use_spi2x = false;
    switch(baud)
    {
    case PX_SPI_BAUD_CLK_DIV_2:     spcr |= (0<<SPR1) | (0<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_4:     spcr |= (0<<SPR1) | (0<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_8:     spcr |= (0<<SPR1) | (1<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_16:    spcr |= (0<<SPR1) | (1<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_32:    spcr |= (1<<SPR1) | (0<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_64:    spcr |= (1<<SPR1) | (0<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_128:   spcr |= (1<<SPR1) | (1<<SPR0);
                                    break;
    default:
        break;
    }
    // Set SPI Mode (0,1,2 or 3)
    spcr |= (((uint8_t)mode)&3) << CPHA;
    // Set data order
    if(data_order == PX_SPI_DATA_ORDER_LSB)
    {
        PX_BIT_SET_HI(spcr, DORD);
    }
    // Save value for SPI Control Register
    handle->spcr = spcr;
    // Save value for "Use SPI 2x clock rate"
    handle->use_spi2x = use_spi2x;
    // Set value for "Master Out dummy byte"
    handle->mo_dummy_byte = mo_dummy_byte;

    // Initialise peripheral
    px_spi_init_peripheral(spi_nr,
                           handle->spcr,
                           handle->use_spi2x);
    // Point handle to peripheral
    handle->spi_per = spi_per;
    // Success
    spi_per->open_counter++;
    return true;
}

bool px_spi_close(px_spi_handle_t * handle)
{
    px_spi_per_t * spi_per;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Check that handle is open
    PX_DBG_ASSERT(spi_per != NULL);
    PX_DBG_ASSERT(spi_per->open_counter != 0);

    // Decrement open count
    spi_per->open_counter--;

    // More open handles referencing peripheral?
    if(spi_per->open_counter != 0)
    {
        // Close handle
        handle->spi_per = NULL;
        // Success
        return true;
    }

    // Disable peripheral
    switch(spi_per->spi_nr)
    {
#if PX_SPI_CFG_SPI0_EN
    case PX_SPI_NR_0:
        SPCR = 0x00;
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
    const uint8_t * data_u8 = (const uint8_t *)data;
    uint8_t         data_rx;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Check that handle is open
    PX_DBG_ASSERT(spi_per != NULL);
    PX_DBG_ASSERT(spi_per->open_counter != 0);

    // Update communication parameters (if different)
    px_spi_update_cfg(spi_per->spi_nr,
                      handle->spcr,
                      handle->use_spi2x);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    while(nr_of_bytes)
    {
        // Buffer transmit data
        SPDR = *data_u8++;
        // Wait until byte has been transfered
        PX_WAIT_UNTIL_BIT_IS_HI(SPSR, SPIF);
        // Read received byte(and discard)
        data_rx = SPDR;
        // Avoid compiler warning that variable was set but not used
        (void)data_rx;
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
    px_spi_per_t * spi_per;
    uint8_t *      data_u8 = (uint8_t *)data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Check that handle is open
    PX_DBG_ASSERT(spi_per != NULL);
    PX_DBG_ASSERT(spi_per->open_counter != 0);

    // Update communication parameters (if different)
    px_spi_update_cfg(spi_per->spi_nr,
                      handle->spcr,
                      handle->use_spi2x);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    // Repeat until all of the bytes have been read
    while(nr_of_bytes)
    {
        // Buffer dummy byte to transmit
        SPDR = handle->mo_dummy_byte;
        // Wait until byte has been transfered
        PX_WAIT_UNTIL_BIT_IS_HI(SPSR, SPIF);
        // Read received byte
        *data_u8++ = SPDR;
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
    px_spi_per_t *  spi_per;
    const uint8_t * data_wr_u8 = (const uint8_t *)data_wr;
    uint8_t *       data_rd_u8 = (uint8_t *)data_rd;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Check that handle is open
    PX_DBG_ASSERT(spi_per != NULL);
    PX_DBG_ASSERT(spi_per->open_counter != 0);

    // Update communication parameters (if different)
    px_spi_update_cfg(spi_per->spi_nr,
                      handle->spcr,
                      handle->use_spi2x);

    if(flags & PX_SPI_FLAG_START)
    {
        // Take Chip Select Low
        PX_SPI_CFG_CS_LO(handle->cs_id);
    }

    // Repeat until all of the bytes have been exchanged
    while(nr_of_bytes)
    {
        // Buffer byte to transmit
        SPDR = *data_wr_u8++;
        // Wait until byte has been transfered
        PX_WAIT_UNTIL_BIT_IS_HI(SPSR, SPIF);
        // Read received byte
        *data_rd_u8++ = SPDR;
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
    px_spi_per_t * spi_per;
    uint8_t        spcr;
    bool           use_spi2x;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    spi_per = handle->spi_per;
    // Check that handle is open
    PX_DBG_ASSERT(spi_per != NULL);
    PX_DBG_ASSERT(spi_per->open_counter != 0);

    // Clear SPI Clock divisor bits
    spcr = handle->spcr & (~((1<<SPR1) | (1<<SPR0)));

    // Set new clock divisor
    use_spi2x = false;
    switch(baud)
    {
    case PX_SPI_BAUD_CLK_DIV_2:     spcr |= (0<<SPR1) | (0<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_4:     spcr |= (0<<SPR1) | (0<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_8:     spcr |= (0<<SPR1) | (1<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_16:    spcr |= (0<<SPR1) | (1<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_32:    spcr |= (1<<SPR1) | (0<<SPR0); use_spi2x = true;
                                    break;
    case PX_SPI_BAUD_CLK_DIV_64:    spcr |= (1<<SPR1) | (0<<SPR0);
                                    break;
    case PX_SPI_BAUD_CLK_DIV_128:   spcr |= (1<<SPR1) | (1<<SPR0);
                                    break;
    default:
        break;
    }

    // Update value for SPI Control Register
    handle->spcr = spcr;
    // Update value for "Use SPI 2x clock rate"
    handle->use_spi2x = use_spi2x;

    // Initialise peripheral
    px_spi_update_cfg(spi_per->spi_nr, 
                      handle->spcr, 
                      handle->use_spi2x);
}

px_spi_baud_t px_spi_util_baud_hz_to_clk_div(uint32_t baud_hz)
{
    uint32_t      actual_bit_rate_hz;
    px_spi_baud_t baud;

    /* 
     *  Calculate clock divisor that generates a bit rate closest to the desired 
     *  bit rate (equal or less)
     */
    actual_bit_rate_hz = F_CPU >> 1;
    baud               = PX_SPI_BAUD_CLK_DIV_2;

    // Repeat until target is reached
    while(actual_bit_rate_hz > baud_hz)
    {
        // Divide actual bit rate by two
        actual_bit_rate_hz >>= 1;
        // Stop if divisor reaches maximum
        if(++baud == PX_SPI_BAUD_CLK_DIV_128)
        {
            break;
        }
    };

    return baud;
}

uint32_t px_spi_util_clk_div_to_baud_hz(px_spi_baud_t baud)
{
    return (((uint32_t)F_CPU) >> (baud+1));
}
