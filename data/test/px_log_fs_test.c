#include <stdio.h>
#include "px_board.h"
#include "px_spi.h"
#include "px_at25s.h"
#include "px_log_fs.h"

// Define record data content
typedef struct
{
    int16_t temperature;
} px_log_fs_data_t;

static px_spi_handle_t    px_at25s_spi_handle;
static px_log_fs_handle_t px_log_fs_handle;
static px_log_fs_data_t   px_log_fs_data;

int main(void)
{
    uint8_t         i;
    px_log_fs_err_t px_log_fs_err;

    // Initialise modules
    px_board_init();
    px_spi_init();
    px_spi_open2(&px_at25s_spi_handle, PX_SPI_PER_1, PX_BOARD_SPI_CS_AT25S,
                 PX_AT25S_MAX_SPI_CLOCK_HZ, PX_AT25S_SPI_MODE, 
                 PX_AT25S_SPI_DATA_ORDER, 0x00);
    px_at25s_init(&px_at25s_spi_handle);
    // Initialise file system
    px_log_fs_init(&px_log_fs_handle, 0, PX_AT25S_PAGES - 1);

    // Write 8 records
    px_log_fs_data.temperature = 160;   // 16.0 deg Celsius
    for(i=0; i<8; i++)
    {
        // Write record
        px_log_fs_record_wr(&px_log_fs_handle, 
                            &px_log_fs_data,
                            sizeof(px_log_fs_data));
        // Increment temperature by 1.3 deg
        px_log_fs_data.temperature += 13;
    }    

    // Read first record
    px_log_fs_err = px_log_fs_rd_first(&px_log_fs_data, sizeof(px_log_fs_data));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display record data
        printf("%d\n", px_log_fs_data.temp);
        // Read next record
        px_log_fs_err = px_log_fs_rd_next(&px_log_fs_data, sizeof(px_log_fs_data));
    }
}
