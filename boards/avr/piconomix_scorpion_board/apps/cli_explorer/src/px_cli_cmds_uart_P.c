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
PX_DBG_DECL_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_cli_cmd_uart_flush_rx(void)
{
    uint8_t data;

    while(px_uart_rd_u8(&px_uart_handle, (uint8_t *)&data))
    {
        ;
    }
}

static const char* px_cli_cmd_fn_uart_write(uint8_t argc, char* argv[])
{
    uint8_t     i;    

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch to other UART
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // <data>
    for(i=0; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            PX_PRINTF_P("Error. <w data> at index %d invalid\n", i);
            return NULL;
        }
        putchar(px_cli_argv_val.u8);
    }

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch UART back
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // Flush receive buffer
    px_cli_cmd_uart_flush_rx();

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
        return PX_PGM_STR("Error. <r nr of bytes> must be 0 to 16");
    }
    nr_of_bytes = px_cli_argv_val.u8;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(1, 0, 65535))
    {
        return PX_PGM_STR("Error. <r timeout ms> must be 0 to 65535");
    }
    timeout_ms = px_cli_argv_val.u16;

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch to other UART
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

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

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch UART back
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // Flush receive buffer
    px_cli_cmd_uart_flush_rx();

    // Report received bytes
    for(i=0; i<nr_of_bytes; i++)
    {
        PX_PRINTF_P("0x%02x ", data[i]);
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
        return PX_PGM_STR("Error. <r nr of bytes> must be 0 to 16");
    }
    nr_of_bytes = px_cli_argv_val.u8;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(1, 0, 65535))
    {
        return PX_PGM_STR("Error. <r timeout ms> must be 0 to 65535");
    }
    timeout_ms = px_cli_argv_val.u16;

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch to other UART
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // <data>
    for(i=2; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            PX_PRINTF_P("Error. <w data> at index %d invalid\n", i);
            return NULL;
        }
        putchar(px_cli_argv_val.u8);
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

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch UART back
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // Flush receive buffer
    px_cli_cmd_uart_flush_rx();

    // Report received bytes
    for(i=0; i<nr_of_bytes; i++)
    {
        PX_PRINTF_P("0x%02x ", data[i]);
    }
    putchar('\n');

    return NULL;
}

static const char* px_cli_cmd_fn_uart_str(uint8_t argc, char* argv[])
{
    uint8_t     i;
    uint16_t    timeout_ms;
    char        data[33];
    char        rx_char;
    px_systmr_t tmr;

    // <timeout ms>
    if(!px_cli_util_argv_to_u16(0, 0, 65535))
    {
        return PX_PGM_STR("Error. <r timeout ms> must be 0 to 65535");
    }
    timeout_ms = px_cli_argv_val.u16;

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }

    // Switch to other UART
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // <"w str">
    printf(argv[1]);

    // [cr|lf|crlf]
    if(argc > 2)
    {
        switch(px_cli_util_argv_to_option(2, PX_PGM_STR("cr\0lf\0crlf\0")))
        {
        case 0:
            putchar('\r');
            break;
        case 1:
            putchar('\n');
            break;
        case 2:
            putchar('\r');
            putchar('\n');
            break;
        default:
            return PX_PGM_STR("Error: [cr|lf|crlf] must be cr or lf or crlf");
        }
    }

    // Start timeout
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(timeout_ms));

    // Try to receive specified number of bytes
    i=0;
    while(!px_systmr_has_expired(&tmr))
    {
        if(px_uart_rd_u8(&px_uart_handle, (uint8_t *)&rx_char))
        {
            // End of string?
            if( (rx_char == '\r') || (rx_char == '\n') || (rx_char == '\0') )
            {
                break;
            }
            // Save received byte
            data[i] = rx_char;
            // Maximum reached?
            if(++i >= 32)
            {
                break;
            }
        }
    }
    // Terminate string
    data[i] = '\0';

    // Wait until last byte has been transmitted
    while(!px_uart_wr_finished(&px_uart_handle))
    {
        ;
    }    

    // Switch UART back
    PX_GPIO_PIN_TOGGLE(PX_GPIO_UART_SEL);

    // Flush receive buffer
    px_cli_cmd_uart_flush_rx();

    // Report received string
    printf(data);
    putchar('\n');

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_uart_write,      "w",    1, 16,  "<d0> .. [d15]",                              "Write 1 to 16 bytes of data")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_receive,    "r",    2, 2,   "<nr of bytes> <timeout ms>",                 "Receive up to 16 bytes of data or until timeout")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_exchange,   "x",    3, 18,  "<nr of bytes> <timeout ms> <0> .. [d15]",    "Write 1 to 16 bytes of data and receive up to 16 bytes of data")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_str,        "s",    2, 3,   "<timeout ms> <\"w str\"> [cr|lf|crlf]",      "Write str and receive str. Use quotes if str has spaces")

PX_CLI_GROUP_CREATE(px_cli_group_uart, "uart")
    PX_CLI_CMD_ADD(px_cli_cmd_uart_write,     px_cli_cmd_fn_uart_write)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_receive,   px_cli_cmd_fn_uart_receive)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_exchange,  px_cli_cmd_fn_uart_exchange)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_str,       px_cli_cmd_fn_uart_str)
PX_CLI_GROUP_END()
