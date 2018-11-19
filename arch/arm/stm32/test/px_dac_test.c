#include "px_dac.h"
#include "px_board.h"

// Create DAC handle object
static px_dac_handle_t px_dac_handle;

int main(void)
{
    uint16_t data;

    // Initialise board
    px_board_init();

    // Initialise DAC driver
    px_dac_init();

    // Open handle to DAC peripheral
    px_dac_open(&px_dac_handle, PX_DAC_PER_1);

    // Perform an DAC measurement on channel 0
    data = px_dac_sample(&px_dac_handle, PX_DAC_CH0);

    // Close DAC Handle
    px_dac_close(px_dac_handle);
}
