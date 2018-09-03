#include "px_spi.h"
#include "px_board.h"

// Create SPI Slave handle object
static px_spi_handle_t px_spi_handle;

int main(void)
{
    uint8_t data;

    // Initialise board
    px_board_init();

    // Initialise SPI driver
    px_spi_init();

    // Open handle to SPI slave device
    px_spi_open2(&px_spi_handle,
                 PX_SPI_PER_0,
                 PX_BOARD_SPI_CS_DF,
                 PX_SPI_BAUD_CLK_DIV_2, 
                 PX_SPI_MODE0, 
                 PX_SPI_DATA_ORDER_MSB);

    // Take Chip Select low and send command byte
    data = 0xd7;
    px_spi_wr(px_at45d_spi_handle, &data, 1, PX_SPI_FLAG_START);

    // Read status byte and take Chip Select high
    px_spi_rd(px_at45d_spi_handle, &data, 1, PX_SPI_FLAG_STOP);

    // Close SPI Handle
    px_spi_close(px_spi_handle);
}
