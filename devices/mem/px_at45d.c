/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2010 Pieter Conradie <https://piconomix.com>
 
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

    Title:          px_at45d.h : Adesto AT45D Serial DataFlash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2010-04-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_at45d.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name Read commands
//@{
#define PX_AT45D_CMD_CONTINUOUS_ARRAY_READ             0xe8
#define PX_AT45D_CMD_MAIN_MEMORY_PAGE_READ             0xd2
#define PX_AT45D_CMD_STATUS_REGISTER_READ              0xd7
//@}

/// @name Program and Erase commands
//@{
#define PX_AT45D_CMD_BUFFER1_WRITE                     0x84
#define PX_AT45D_CMD_BUFFER2_WRITE                     0x87

#define PX_AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_W_ERASE     0x83
#define PX_AT45D_CMD_BUF2_TO_MAIN_PAGE_PRG_W_ERASE     0x86

#define PX_AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_WO_ERASE    0x88
#define PX_AT45D_CMD_BUF2_TO_MAIN_PAGE_PRG_WO_ERASE    0x89

#define PX_AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1        0x82
#define PX_AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF2        0x85

#define PX_AT45D_CMD_PAGE_ERASE                        0x81
//@}

/// @name Additional commands
//@{
#define PX_AT45D_CMD_MAIN_MEM_PAGE_TO_BUF1             0x53
#define PX_AT45D_CMD_MAIN_MEM_PAGE_TO_BUF2             0x55
//@}

#define PX_AT45D_CMD_MAIN_POWER_DOWN                   0xb9
#define PX_AT45D_CMD_MAIN_RESUME_FROM_POWER_DOWN       0xab

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static bool              px_at45d_ready_flag;
static px_spi_handle_t * px_at45d_spi_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#if ((PX_AT45D_PAGE_SIZE != 256) && (PX_AT45D_PAGE_SIZE != 264))
#error "This driver does not work for any other page size yet. See comment!"
/*
   Unfortunately this driver still needs work to cater for other page sizes. 
    
   When the address is sent, the specified page value is shifted by a fixed amount, 
   but actually the amount of shift depends on the page size (?) 
 */
#endif

static void px_at45d_tx_adr(uint16_t page, uint16_t start_byte_in_page)
{
    uint8_t data[3];

#if PX_VAL_IS_PWR_OF_TWO(PX_AT45D_PAGE_SIZE)
    data[0] = (uint8_t)(page>>8)&0xFF;
    data[1] = (uint8_t)(page&0xFF);
    data[2] = (uint8_t)(start_byte_in_page&0xFF);
#else
    data[0] = (page>>7)&0xFF;
    data[1] = ((page<<1)|(start_byte_in_page>>8))&0xFF;
    data[2] = start_byte_in_page&0xFF;
#endif

    px_spi_wr(px_at45d_spi_handle, data, 3, 0);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_at45d_init(px_spi_handle_t * handle)
{
    // Set flag
    px_at45d_ready_flag = true;
    // Save SPI slave device handle
    px_at45d_spi_handle = handle;
}

void px_at45d_power_down(void)
{
    uint8_t data[1];

    data[0] = PX_AT45D_CMD_MAIN_POWER_DOWN;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START_AND_STOP);
}

void px_at45d_resume_from_power_down(void)
{
    uint8_t data[1];

    data[0] = PX_AT45D_CMD_MAIN_RESUME_FROM_POWER_DOWN;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START_AND_STOP);
}

uint16_t px_at45d_rd(void *         buffer,
                     px_at45d_adr_t address,
                     uint16_t       nr_of_bytes)
{
    px_at45d_adr_t max_bytes_to_read;
    uint16_t       page;
    uint16_t       start_byte_in_page;
    uint8_t        data[4];

    // See if specified address is out of bounds
    if(address > PX_AT45D_ADR_MAX)
    {
        return 0;
    }

    // See if "number of bytes to read" should be clipped
    max_bytes_to_read = PX_AT45D_ADR_MAX - address + 1;
    if(nr_of_bytes > max_bytes_to_read)
    {
        nr_of_bytes = max_bytes_to_read;
    }

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_CONTINUOUS_ARRAY_READ;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Calculate page, offset and number of bytes remaining in page
#if PX_VAL_IS_PWR_OF_TWO(PX_AT45D_PAGE_SIZE)
    page               = address >> 8;
    start_byte_in_page = address & 0xff;
#else
    page               = address / PX_AT45D_PAGE_SIZE;
    start_byte_in_page = address % PX_AT45D_PAGE_SIZE;
#endif
    
    // Send address
    px_at45d_tx_adr(page, start_byte_in_page);

    // Send dont-care bits
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    px_spi_wr(px_at45d_spi_handle, data, 4, 0);    

    // Read data
    px_spi_rd(px_at45d_spi_handle, buffer, nr_of_bytes, PX_SPI_FLAG_STOP);

    return nr_of_bytes;
}

void px_at45d_rd_page(void * buffer, uint16_t page)
{
    uint8_t data[4];

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_MAIN_MEMORY_PAGE_READ;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send address
    px_at45d_tx_adr(page, 0);

    // Send dont-care bits
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    px_spi_wr(px_at45d_spi_handle, data, 4, 0);    

    // Read data
    px_spi_rd(px_at45d_spi_handle, buffer, PX_AT45D_PAGE_SIZE, PX_SPI_FLAG_STOP);
}

void px_at45d_rd_page_offset(void *   buffer,
                             uint16_t page,
                             uint16_t start_byte_in_page,
                             uint16_t nr_of_bytes)
{
    uint8_t data[4];

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_CONTINUOUS_ARRAY_READ;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send address
    px_at45d_tx_adr(page, start_byte_in_page);

    // Send dont-care bits
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    px_spi_wr(px_at45d_spi_handle, data, 4, 0);

    // Read data
    px_spi_rd(px_at45d_spi_handle, buffer, nr_of_bytes, PX_SPI_FLAG_STOP);
}

void px_at45d_wr_page(const void * buffer, uint16_t page)
{
    uint8_t data[1];

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send address
    px_at45d_tx_adr(page, 0);

    // Send data to be written
    px_spi_wr(px_at45d_spi_handle, buffer, PX_AT45D_PAGE_SIZE, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_at45d_ready_flag = false;
}

void px_at45d_wr_page_offset(const void * buffer,
                             uint16_t     page,
                             uint16_t     start_byte_in_page,
                             uint16_t     nr_of_bytes)
{
    uint16_t  i;
    uint8_t * bufffer_u8 = (uint8_t *)buffer;
    uint8_t   data[3];

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }   

    // Send command
    data[0] = PX_AT45D_CMD_BUFFER1_WRITE;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send start byte in buffer
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    px_spi_wr(px_at45d_spi_handle, data, 3, 0);

    // Fill buffer with data to be written (other bytes are 0xFF to leave them unchanged)
    for(i=0; i<PX_AT45D_PAGE_SIZE; i++)
    {
        if(  (i >= start_byte_in_page              )
           &&(i  < start_byte_in_page + nr_of_bytes)  )
        {
            px_spi_wr(px_at45d_spi_handle, bufffer_u8++, 1, 0);
        }
        else
        {
            // Leave unchanged
            data[0] = 0xff;
            px_spi_wr(px_at45d_spi_handle, data, 1, 0);
        }
    }

    // Deselect DataFlash
    px_spi_wr(px_at45d_spi_handle, NULL, 0, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_at45d_ready_flag = false;

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_WO_ERASE;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send address
    px_at45d_tx_adr(page, 0);

    // Deselect DataFlash
    px_spi_wr(px_at45d_spi_handle, NULL, 0, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_at45d_ready_flag = false;
}

void px_at45d_erase(uint16_t page)
{
    uint8_t data[1];

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = PX_AT45D_CMD_PAGE_ERASE;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Send address
    px_at45d_tx_adr(page, 0);

    // Deselect DataFlash
    px_spi_wr(px_at45d_spi_handle, NULL, 0, PX_SPI_FLAG_STOP);

    // Set flag to busy
    px_at45d_ready_flag = false;
}

bool px_at45d_ready(void)
{
    uint8_t data;

    // If flag has already been set, then take short cut
    if(px_at45d_ready_flag)
    {
        return true;
    }

    // Get DataFlash status
    data = px_at45d_get_status();

    // See if DataFlash is ready
    if(PX_BIT_IS_HI(data, PX_AT45D_STATUS_READY))
    {
        // Set flag
        px_at45d_ready_flag = true;
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t px_at45d_get_status(void)
{
    uint8_t data[1];

    // Send command
    data[0] = PX_AT45D_CMD_STATUS_REGISTER_READ;
    px_spi_wr(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_START);

    // Read status
    px_spi_rd(px_at45d_spi_handle, data, 1, PX_SPI_FLAG_STOP);

    return data[0];
}

bool px_at45d_page_size_is_pwr_of_two(void)
{
    uint8_t data = px_at45d_get_status();

    if(PX_BIT_IS_HI(data, PX_AT45D_STATUS_PAGE_SIZE))
    {
        // Page size is a power of two
        return true;
    }
    else
    {
        // Page size is not a power of two
        return false;
    }
}

bool px_at45d_set_page_size_to_pwr_of_two(void)
{
    uint8_t data[4];

    if(px_at45d_page_size_is_pwr_of_two())
    {
        // Page size is already a power of two
        return false;
    }

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    // Send command
    data[0] = 0x3d;
    data[1] = 0x2a;
    data[2] = 0x80;
    data[3] = 0xa6;
    px_spi_wr(px_at45d_spi_handle, data, 4, PX_SPI_FLAG_START_AND_STOP);

    // Wait until DataFlash is not busy
    while(!px_at45d_ready())
    {
        ;
    }

    return true;
}
