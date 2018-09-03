#include "px_board.h"
#include "px_spi.h"
#include "px_at45d.h"

static uint8_t         data[PX_AT45D_PAGE_SIZE];
static px_spi_handle_t px_at45d_spi_handle;

int main(void)
{
    uint8_t  status;
    uint16_t i;

    // Initialise modules
    px_board_init();
    px_spi_init();
    px_spi_open2(&px_at45d_spi_handle,
                 BOARD_SPI_CS_AT45D,
                 PX_AT45D_MAX_SPI_CLOCK_HZ, 
                 PX_AT45D_SPI_MODE, 
                 PX_AT45D_SPI_DATA_ORDER,
                 0x00);
    px_at45d_init(&px_at45d_spi_handle);
  
#if PX_AT45D_POWER_OF_TWO_PAGE_SIZE
    // Make sure page size is correct
    if(!px_at45d_page_size_is_pwr_of_two())
    {
        // Set page size to a power of two.
        px_at45d_set_page_size_to_pwr_of_two();

        // Device must be power cycled for setting to take effect
        for(;;)
        {
            ;
        }
    }
#endif

    // Write a page of DataFlash
    for(i = 0; i<PX_AT45D_PAGE_SIZE; i++)
    {
        data[i] = (uint8_t)(i&0xff);
    }
    px_at45d_wr_page(data, 0);

    // Read a page of DataFlash
    px_at45d_rd_page(data, 0);
}
