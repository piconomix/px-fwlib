#include "px_adc.h"
#include "px_board.h"

// Create ADC handle object
static px_adc_handle_t px_adc_handle;

int main(void)
{
    uint16_t data;

    // Initialise board
    px_board_init();

    // Initialise ADC driver
    px_adc_init();

    // Open handle to ADC peripheral
    px_adc_open(&px_adc_handle, PX_ADC_PER_1);

    // Perform an ADC measurement on channel 0
    data = px_adc_sample(&px_adc_handle, PX_ADC_CH0);

    // Close ADC Handle
    px_adc_close(px_adc_handle);
}
