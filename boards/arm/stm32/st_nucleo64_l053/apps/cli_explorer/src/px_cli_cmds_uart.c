/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_uart.h : CLI commands for UART
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "main.h"
#include "px_uart.h"
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_systmr.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli_cmds_uart");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_uart_handle_t  px_uart_handle;
static bool              px_uart_handle_open;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_uart_cfg(uint8_t argc, char* argv[])
{
    px_uart_nr_t        per;
    uint32_t            baud_rate_hz;
    px_uart_data_bits_t data_bits = PX_UART_DATA_BITS_8;
    px_uart_parity_t    parity    = PX_UART_PARITY_NONE;
    px_uart_stop_bits_t stop_bits = PX_UART_STOP_BITS_1;

    // <per>
    switch(argv[0][0])
    {
    case '1':
        per = PX_UART_NR_1;
        break;
    default:
        return "Error. <per> must be 1";
    }

    // <baud>
    if(!px_cli_util_argv_to_u32(1, 300, PX_BOARD_PER_CLK_HZ/16))
    {
        printf("Error. <baud> must be 300 to %lu\n", PX_BOARD_PER_CLK_HZ/16);
        return NULL;
    }
    baud_rate_hz = px_cli_argv_val.u32;

    // [data]>
    if(argc >= 3)
    {
        switch(argv[2][0])
        {
        case '7':
            data_bits = PX_UART_DATA_BITS_7;
            break;
        case '8':
            data_bits = PX_UART_DATA_BITS_8;
            break;
        case '9':
            data_bits = PX_UART_DATA_BITS_9;
            break;
        default:
            return "Error. [data] must be 7, 8 or 9";
        }
    }

    // [parity]
    if(argc >= 4)
    {
        switch(argv[3][0])
        {
        case 'n':
            parity = PX_UART_PARITY_NONE;
            break;
        case 'o':
            parity = PX_UART_PARITY_ODD;
            break;
        case 'e':
            parity = PX_UART_PARITY_EVEN;
            break;
        default:
            return "Error. [parity] must be n (none), o (odd) or e (even)";
        }
    }

    // [stop]
    if(argc >= 5)
    {
        switch(argv[4][0])
        {
        case '1':
            stop_bits = PX_UART_STOP_BITS_1;
            break;
        case '2':
            stop_bits = PX_UART_STOP_BITS_2;
            break;
        default:
            return "Error. [stop] must be 1 or 2";
        }
    }

    // Handle already open?
    if(px_uart_handle_open)
    {
        // Close handle first
        px_uart_close(&px_uart_handle);
    }

    // Open UART device handle
    if(!px_uart_open2(&px_uart_handle, 
                      per,
                      baud_rate_hz,
                      data_bits,
                      parity,
                      stop_bits))
    {
        return "Error. Incorrect parameters; unable to initialise UART";
    }

    // Set flag
    px_uart_handle_open = true;
    return NULL;
}

static const char* px_cli_cmd_fn_uart_bridge(uint8_t argc, char* argv[])
{
    uint8_t     data;

    // Button is pressed?
    while(!PX_USR_PB_IS_PRESSED())
    {
        // Any byte received from console?
        if(px_uart_rd_u8(&px_uart2_handle, &data))
        {
            // Send byte to UART
            px_uart_put_char(&px_uart_handle, data);
        }
        // Any byte received from UART?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Send byte to console
			while(!px_uart_wr_u8(&px_uart2_handle, data))
            {
                ;
            }
        }
    }

    // Wait until button is released
    while(PX_USR_PB_IS_PRESSED())
    {
        ;
    }

    putchar('\n');
    return NULL;
}

static const char* px_cli_cmd_fn_uart_write(uint8_t argc, char* argv[])
{
    uint8_t     i;    

    // <data>
    for(i=0; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        px_uart_put_char(&px_uart_handle, (char)px_cli_argv_val.u8);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_uart_receive(uint8_t argc, char* argv[])
{
    uint8_t     i;
    uint8_t     nr_of_bytes;
    uint16_t    timeout_ms;
    uint8_t     data[16];
    px_systmr_t tmr;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u8(0, 0, 16))
    {
        return "Error. <nr of bytes> must be 0 to 16";
    }
    nr_of_bytes = px_cli_argv_val.u8;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(1, 0, 65535))
    {
        return "Error. <timeout ms> must be 0 to 65535";
    }
    timeout_ms = px_cli_argv_val.u16;

    // Start timeout
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(timeout_ms));

    // Try to receive specified number of bytes
    i=0;
    while(!px_systmr_has_expired(&tmr))
    {
        if(px_uart_rd_u8(&px_uart_handle, &data[i]))
        {
            if(++i >= nr_of_bytes)
            {
                break;
            }
        }
    }
    nr_of_bytes = i;

    // Report received bytes
    for(i=0; i<nr_of_bytes; i++)
    {
        printf("0x%02x ", data[i]);
    }
    putchar('\n');

    return NULL;
}

static const char* px_cli_cmd_fn_uart_exchange(uint8_t argc, char* argv[])
{
    uint8_t     i;
    uint8_t     nr_of_bytes;
    uint16_t    timeout_ms;
    uint8_t     data[16];
    px_systmr_t tmr;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u8(0, 0, 16))
    {
        return "Error. <r nr of bytes> must be 0 to 16";
    }
    nr_of_bytes = px_cli_argv_val.u8;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(1, 0, 65535))
    {
        return "Error. <r timeout ms> must be 0 to 65535";
    }
    timeout_ms = px_cli_argv_val.u16;

    // <data>
    for(i=2; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        px_uart_put_char(&px_uart_handle, (char)px_cli_argv_val.u8);
    }

    // Start timeout
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(timeout_ms));

    // Try to receive specified number of bytes
    i=0;
    while(!px_systmr_has_expired(&tmr))
    {
        if(px_uart_rd_u8(&px_uart_handle, &data[i]))
        {
            if(++i >= nr_of_bytes)
            {
                break;
            }
        }
    }
    nr_of_bytes = i;

    // Report received bytes
    for(i=0; i<nr_of_bytes; i++)
    {
        printf("0x%02x ", data[i]);
    }
    putchar('\n');

    return NULL;
}

static const char* px_cli_cmd_fn_uart_str(uint8_t argc, char* argv[])
{
    uint16_t    timeout_ms;
    char *      tx_char;
    char        rx_char;
    px_systmr_t tmr;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(0, 0, 65535))
    {
        return "Error. <r timeout ms> must be 0 to 65535";
    }
    timeout_ms = px_cli_argv_val.u16;

    // <"w str">
    tx_char = argv[1];
    while(*tx_char != '\0')
    {
        px_uart_put_char(&px_uart_handle, *tx_char++);
    }

    // [cr|lf|crlf]
    if(argc == 3)
    {
        switch(px_cli_util_argv_to_option(2, "cr\0lf\0crlf\0"))
        {
        case 0:
            px_uart_put_char(&px_uart_handle, '\r');
            break;
        case 1:
            px_uart_put_char(&px_uart_handle, '\n');
            break;
        case 2:
            px_uart_put_char(&px_uart_handle, '\r');
            px_uart_put_char(&px_uart_handle, '\n');
            break;
        default:
            return "Error: [cr|lf|crlf] must be cr (Carriage Return) or lf (Line Feed) or crlf (CR+LF)";
        }
    }

    // Start timeout
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(timeout_ms));

    // Receive characters until timeout or NULL, CR of LF is received
    while(!px_systmr_has_expired(&tmr))
    {
        if(px_uart_rd_u8(&px_uart_handle, (uint8_t *)&rx_char))
        {
            // End of string?
            if( (rx_char == '\r') || (rx_char == '\n') || (rx_char == '\0') )
            {
                break;
            }
            // Report received byte
            putchar(rx_char);            
        }
    }
    // Terminate string
    putchar('\n');

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_uart_cfg,      "cfg",  2, 5,   "<per> <baud> [data] [parity] [stop]",        "Configure UART")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_bridge,   "b",    0, 0,   "",                                           "Bridge Console<->UART until button is pressed")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_write,    "w",    1, 16,  "<d0> .. [d15]",                              "Write 1 to 16 bytes of data")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_receive,  "r",    2, 2,   "<nr of bytes> <timeout ms>",                 "Receive up to 16 bytes of data or until timeout")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_exchange, "x",    3, 18,  "<nr of bytes> <timeout ms> <d0> .. [d15]",   "Write 1 to 16 bytes of data and receive up to 16 bytes of data")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_str,      "s",    2, 3,   "<timeout ms> <\"w str\"> [cr|lf|crlf]",      "Write str and receive str. Use quotes if str has spaces")

PX_CLI_GROUP_CREATE(px_cli_group_uart, "uart")
    PX_CLI_CMD_ADD(px_cli_cmd_uart_cfg,       px_cli_cmd_fn_uart_cfg)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_bridge,    px_cli_cmd_fn_uart_bridge)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_write,     px_cli_cmd_fn_uart_write)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_receive,   px_cli_cmd_fn_uart_receive)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_exchange,  px_cli_cmd_fn_uart_exchange)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_str,       px_cli_cmd_fn_uart_str)
PX_CLI_GROUP_END()
