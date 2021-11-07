#include "px_board.h"
#include "px_spi.h"
#include "px_at25s.h"

static uint8_t         data[PX_AT25S_PAGE_SIZE];
static px_spi_handle_t px_sf_spi_handle;

int main(void)
{
    uint8_t  status;
    uint16_t i;

    // Initialise modules
    px_board_init();
    px_spi_init();
    px_spi_open2(&px_sf_spi_handle,
                 BOARD_SPI_CS_SF,
                 PX_AT25S_MAX_SPI_CLOCK_HZ, 
                 PX_AT25S_SPI_MODE, 
                 PX_AT25S_SPI_DATA_ORDER,
                 0x00);
    px_at25s_init(&px_at25s_spi_handle);
  
    // Write a page of Serial Flash
    for(i = 0; i < PX_AT25S_PAGE_SIZE; i++)
    {
        data[i] = (uint8_t)(i&0xff);
    }
    px_at25s_wr_page(data, 0);

    // Read a page of Serial Flash
    px_at25s_rd_page(data, 0);
}
