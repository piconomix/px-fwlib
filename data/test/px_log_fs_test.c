#include <stdio.h>
#include "px_board.h"
#include "px_spi.h"
#include "px_at45d.h"
#include "px_log_fs.h"

// Define record data content
typedef struct
{
    int16_t temperature;
} px_log_fs_data_t;

static px_log_fs_data_t       px_log_fs_data;
static px_log_fs_time_stamp_t time;
static px_spi_handle_t        px_at45d_spi_handle;

int main(void)
{
    uint8_t         i;
    px_log_fs_err_t px_log_fs_err;

    // Initialise modules
    px_board_init();
    px_spi_init();
    px_spi_open2(&px_at45d_spi_handle,
                 PX_SPI_PER_0,
                 PX_BOARD_SPI_CS_AT45D,
                 PX_AT45D_MAX_SPI_CLOCK_HZ, 
                 PX_AT45D_SPI_MODE, 
                 PX_AT45D_SPI_DATA_ORDER);
    px_at45d_init(&px_at45d_spi_handle);

    // Initialise file system
    px_log_fs_init();

    // Set file creation time
    time.year  = 16;
    time.month = 10;
    time.day   = 24;
    time.hour  = 12;
    time.min   = 28;
    time.sec   = 0;

    // Create a new log file
    px_log_fs_create(&time);

    // Populate data
    px_log_fs_data.temperature = 160;     // 16.0 deg Celsius

    // Write 8 records
    for(i=0; i<8; i++)
    {
        // Write record
        px_log_fs_record_wr(&px_log_fs_data,  sizeof(px_log_fs_data));
        // Increment temperature by 1.3 deg
        px_log_fs_data.temperature += 13;
    }    

    // Read first record
    px_log_fs_err = px_log_fs_record_rd_first(&px_log_fs_data, sizeof(px_log_fs_data));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display record data
        printf("%d\n", px_log_fs_data.temp);
        // Read next record
        px_log_fs_err = px_log_fs_record_rd_next(&px_log_fs_data, sizeof(px_log_fs_data));
    }
}
