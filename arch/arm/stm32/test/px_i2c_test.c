#include "px_i2c.h"
#include "px_board.h"

// Bosch BMP280 I2C Slave Address
#define I2C_SLA_ADR   0x76

// Create I2C Slave handle object
px_i2c_handle_t px_i2c_handle;

int main(void)
{
    uint8_t data[1];

    // Initialise board
    px_board_init();

    // Initialise I2C driver
    px_i2c_init();

    // Open handle to I2C slave device
    px_i2c_open(&px_i2c_handle, PX_I2C_PER_1, I2C_SLA_ADR);

    // START I2C write transaction and write register address
    data[0] = 0xd0;
    i2c_wr(&px_i2c_handle, 
           data,
           1,
           PX_I2C_FLAG_START_AND_END);

    // REPEATED START I2C read transaction and read register value
    i2c_rd(&px_i2c_handle, 
           data,
           1,
           PX_I2C_FLAG_REP_START_AND_STOP);

    // Close I2C Handle
    px_i2c_close(&px_i2c_handle);
}
