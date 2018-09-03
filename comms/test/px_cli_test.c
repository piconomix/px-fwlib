#include <string.h>

#include "px_defines.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_pgm_P.h"
#include "px_cli.h"

// LED command handlers
static const char* px_cli_cmd_led_on_fn(uint8_t argc, char* argv[])
{
    PX_LED_ON();

    return NULL;
}
static const char* px_cli_cmd_led_off_fn(uint8_t argc, char* argv[])
{
    PX_LED_OFF();

    return NULL;
}

// Buzzer command handler
static const char* px_cli_cmd_buzzer_fn(uint8_t argc, char* argv[])
{
    uint16_t delay_ms;

    // <on time ms>
    if(px_cli_util_argv_to_u16(0, 0, 1000))
    {
        delay_ms = px_cli_argv_val.u16;        
    }
    else
    {
        return PX_PGM_STR("Error: <on time ms> must be from 0 to 1000 ms");
    }

    BUZZER_ON();
    px_board_delay_ms(delay_ms);
    BUZZER_OFF();

    return NULL;
}

// Create CLI LED command structures
PX_CLI_CMD_CREATE(px_cli_cmd_led_on,    "on",    0, 0, "",    "Switch LED on")
PX_CLI_CMD_CREATE(px_cli_cmd_led_off,   "off",   0, 0, "",    "Switch LED off")

// Create CLI LED group and add commands
PX_CLI_GROUP_CREATE(px_cli_group_led, "led")
    PX_CLI_CMD_ADD(px_cli_cmd_led_on,  px_cli_cmd_led_on_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_led_off, px_cli_cmd_led_off_fn)
PX_CLI_GROUP_END()
    
// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_buzzer,    "buzzer",   1, 1,   "<on time ms>",             "Switch buzzer on for specified number of milliseconds")
PX_CLI_CMD_CREATE(px_cli_cmd_help,      "help",     0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Declare CLI command list and add commands and groups
PX_CLI_CMD_LIST_CREATE()
    PX_CLI_CMD_ADD  (px_cli_cmd_buzzer, px_cli_cmd_buzzer_fn)
    PX_CLI_GROUP_ADD(px_cli_group_led)
    PX_CLI_CMD_ADD  (px_cli_cmd_help,   px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()


// Declare UART handle structure
static px_uart_handle_t px_uart_handle;

int main(void)
{
    uint8_t data;
 
    // Initialise modules
    px_board_init();
    px_uart_init();    

    // Open UART0 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_PER_0, 
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Direct stdio to UART0
    px_uart_stdio_init(&px_uart_handle);

    // Enable interrupts
    px_interrupts_enable();

    // Initialise CLI (after STDIO is ready)
    px_cli_init(PX_PGM_STR("CLI Example\n\n"));

    // Repeat forever
    for(;;)
    {
        // Byte received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Pass received byte on to CLI
            px_cli_on_rx_char((char)data);
        }
    }
}
