#include "px_board.h"
#include "px_one_wire.h"
#include "px_ds18b20.h"

int main(void)
{
    px_one_wire_rom_t       rom;
    px_ds18b20_scratchpad_t scratchpad;
    int16_t                 temp_deci_deg;

    // Initialise board
    px_board_init();
    px_one_wire_init();

    // Read one-wire device's ROM to verify that it is present and functioning
    px_one_wire_rd_rom(&rom);
    // Start conversion
    px_ds18b20_start_temp_conv();
    // Wait 750 ms for a 12-bit conversion
    px_board_delay_ms(PX_DS18B20_CONV_TIME_MS_12BIT);
    // Read scratchpad
    px_ds18b20_rd_scratchpad(&scratchpad);
    // Convert result
    temp_deci_deg = px_ds18b20_util_convert_t_to_deci_deg(scratchpad.temp_msb,
                                                          scratchpad.temp_lsb,
                                                          scratchpad.cfg_reg);
}
