/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_cat25m.h : On Semiconductor CAT25M SPI EEPROM Driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-10-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cat25m.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name commands
/// @{
#define PX_CAT25M_CMD_WREN     0x06    ///< Enable Write Operations
#define PX_CAT25M_CMD_WRDI     0x04    ///< Disable Write Operations
#define PX_CAT25M_CMD_RDSR     0x05    ///< Read Status Register
#define PX_CAT25M_CMD_WRSR     0x01    ///< Write Status Register
#define PX_CAT25M_CMD_READ     0x03    ///< Read Data from Memory
#define PX_CAT25M_CMD_WRITE    0x02    ///< Write Data to Memory
/// @}

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static bool              px_cat25m_ready_flag;
static px_spi_handle_t * px_cat25m_spi_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_cat25m_init(px_spi_handle_t * handle)
{
    // Set flag
    px_cat25m_ready_flag = true;
    // Save SPI slave device handle
    px_cat25m_spi_handle = handle;
}

void px_cat25m_rd(void *          buffer,
                  px_cat25m_adr_t address,
                  size_t          nr_of_bytes)
{
    uint8_t  spi_data[4];
    uint8_t *bufffer_u8 = (uint8_t *)buffer;

    // See if specified address is out of bounds
    if(address > PX_CAT25M_ADR_MAX)
    {
        return;
    }

    // Wait until EEPROM is not busy
    while(!px_cat25m_ready())
    {
        ;
    }

    // Send command
    spi_data[0] = PX_CAT25M_CMD_READ; 
    spi_data[1] = PX_U32_MH8(address);
    spi_data[2] = PX_U32_ML8(address);
    spi_data[3] = PX_U32_LO8(address);
    px_spi_wr(px_cat25m_spi_handle, spi_data, 4, PX_SPI_FLAG_START);

    // Read data
    px_spi_rd(px_cat25m_spi_handle, buffer, nr_of_bytes, PX_SPI_FLAG_STOP);
}

void px_cat25m_rd_page(void * buffer, uint16_t page)
{
    uint16_t  i;
    uint8_t * bufffer_u8 = (uint8_t *)buffer;
    uint8_t   spi_data[4];

    // Wait until EEPROM is not busy
    while(!px_cat25m_ready())
    {
        ;
    }

    // Send command
    spi_data[0] = PX_CAT25M_CMD_READ; 
    spi_data[1] = PX_U16_HI8(page);
    spi_data[2] = PX_U16_LO8(page);
    spi_data[3] = 0;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 4, PX_SPI_FLAG_START);
    
    // Read data
    px_spi_rd(px_cat25m_spi_handle, buffer, PX_CAT25M_PAGE_SIZE, PX_SPI_FLAG_STOP);
}

void px_cat25m_rd_page_offset(void *   buffer,
                              uint16_t page,
                              uint8_t  start_byte_in_page,
                              size_t   nr_of_bytes)
{
    uint8_t * bufffer_u8 = (uint8_t *)buffer;
    uint8_t   spi_data[4];

    // Wait until EEPROM is not busy
    while(!px_cat25m_ready())
    {
        ;
    }

    // Send command
    spi_data[0] = PX_CAT25M_CMD_READ; 
    spi_data[1] = PX_U16_HI8(page);
    spi_data[2] = PX_U16_LO8(page);
    spi_data[3] = start_byte_in_page;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 4, PX_SPI_FLAG_START);
    
    // Read data
    px_spi_rd(px_cat25m_spi_handle, buffer, nr_of_bytes, PX_SPI_FLAG_STOP);
}

void px_cat25m_wr_page(const void * buffer, uint16_t page)
{
    uint16_t  i;
    uint8_t * bufffer_u8 = (uint8_t *)buffer;
    uint8_t   spi_data[4];

    // Wait until EEPROM is not busy
    while(!px_cat25m_ready())
    {
        ;
    }

    // Enable write
    px_cat25m_wr_en();

    // Send command
    spi_data[0] = PX_CAT25M_CMD_WRITE; 
    spi_data[1] = PX_U16_HI8(page);
    spi_data[2] = PX_U16_LO8(page);
    spi_data[3] = 0;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 4, PX_SPI_FLAG_START);

    // Write data
    px_spi_wr(px_cat25m_spi_handle, buffer, PX_CAT25M_PAGE_SIZE, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_cat25m_ready_flag = false;    
}

void px_cat25m_wr_page_offset(const void * buffer,
                              uint16_t     page,
                              uint8_t      start_byte_in_page,
                              size_t       nr_of_bytes)
{
    uint8_t * bufffer_u8 = (uint8_t *)buffer;
    uint8_t   spi_data[4];

    // Wait until EEPROM is not busy
    while(!px_cat25m_ready())
    {
        ;
    }

    // Enable write
    px_cat25m_wr_en();

    // Send command
    spi_data[0] = PX_CAT25M_CMD_WRITE; 
    spi_data[1] = PX_U16_HI8(page);
    spi_data[2] = PX_U16_LO8(page);
    spi_data[3] = start_byte_in_page;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 4, PX_SPI_FLAG_START);

    // Write data
    px_spi_wr(px_cat25m_spi_handle, buffer, nr_of_bytes, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_cat25m_ready_flag = false;    
}

bool px_cat25m_ready(void)
{
    if(  (px_cat25m_status_rd() & PX_CAT25M_STATUS_RDY)  == 0)
    {
        return true;
    }
    else
    {
        return false;
    }    
}

uint8_t px_cat25m_status_rd(void)
{
    uint8_t spi_data[1];

    // Send command
    spi_data[0] = PX_CAT25M_CMD_RDSR;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 1, PX_SPI_FLAG_START);

    // Read status
    px_spi_rd(px_cat25m_spi_handle, spi_data, 1, PX_SPI_FLAG_STOP);

    return spi_data[0];
}

void px_cat25m_status_wr(uint8_t status)
{
    uint8_t spi_data[2];

    // Send command
    spi_data[0] = PX_CAT25M_CMD_WRSR; 
    spi_data[1] = status;
    px_spi_wr(px_cat25m_spi_handle, spi_data, 2, PX_SPI_FLAG_START_AND_STOP);
}

void px_cat25m_wr_en(void)
{
    uint8_t spi_data[1];

    // Send command
    spi_data[0] = PX_CAT25M_CMD_WREN; 
    px_spi_wr(px_cat25m_spi_handle, spi_data, 1, PX_SPI_FLAG_START_AND_STOP);
}

void px_cat25m_wr_dis(void)
{
    uint8_t spi_data[1];

    // Send command
    spi_data[0] = PX_CAT25M_CMD_WRDI; 
    px_spi_wr(px_cat25m_spi_handle, spi_data, 1, PX_SPI_FLAG_START_AND_STOP);
}
