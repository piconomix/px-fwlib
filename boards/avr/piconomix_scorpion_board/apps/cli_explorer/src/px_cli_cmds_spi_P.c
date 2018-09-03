/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          px_cli_cmds_spi.h : CLI commands for SPI
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_spi.h"
#include "px_gpio.h"
#include "px_cli_cmds_gpio.h"
#include "px_board.h"
#include "px_pgm_P.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("cli_cmds_spi")

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gpio_handle_t px_spi_cs_gpio;
static px_spi_handle_t  px_spi_handle;
static bool             px_spi_handle_open;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_spi_cfg(uint8_t argc, char* argv[])
{
    const char *  result;
    uint32_t      baud_rate_hz;
    px_spi_baud_t baud;
    px_spi_mode_t mode;
    px_spi_dord_t data_order;

    // [cs]
    if(argc > 0)
    {
        result = px_cli_cmd_gpio_str_to_handle(&px_spi_cs_gpio, argv[0]);
        if(result != NULL)
        {
            return result;
        }
    }
    else
    {
        px_spi_cs_gpio = px_gpio_spi_cs;
    }

    // [baud]
    if(argc > 1)
    {
        if(!px_cli_util_argv_to_u32(1, 0, F_CPU / 2))
        {
            PX_PRINTF_P("Error. [baud] must be 0 to %lu Hz\n", F_CPU / 2);
            return NULL;
        }
        baud_rate_hz = px_cli_argv_val.u32;
        baud = px_spi_util_baud_hz_to_clk_div(baud_rate_hz);
    }
    else
    {
        baud = PX_SPI_BAUD_CLK_DIV_2;
    }

    // [mode]
    if(argc > 2)
    {
        switch(argv[2][0])
        {
        case '0':
            mode = PX_SPI_MODE0;
            break;
        case '1':
            mode = PX_SPI_MODE1;
            break;
        case '2':
            mode = PX_SPI_MODE2;
            break;
        case '3':
            mode = PX_SPI_MODE3;
            break;
        default:
            return PX_PGM_STR("Error. [mode] must be 0, 1, 2 or 3");
        }
    }
    else
    {
        mode = PX_SPI_MODE0;
    }

    // [order]
    if(argc > 3)
    {
        switch(px_cli_util_argv_to_option(3, PX_PGM_STR("lsb\0msb\0")))
        {
        case 0: 
            data_order = PX_SPI_DATA_ORDER_LSB;
            break;
        case 1: 
            data_order = PX_SPI_DATA_ORDER_MSB;
            break;
        default:
            return PX_PGM_STR("Error: [order] must be lsb or msb");
        }
    }
    else
    {
        data_order = PX_SPI_DATA_ORDER_MSB;
    }

    // Initialise CS pin as an output
    px_gpio_pin_set_hi(&px_spi_cs_gpio);
    px_gpio_dir_set_out(&px_spi_cs_gpio);

    // Handle already open?
    if(px_spi_handle_open)
    {
        // Close handle first
        px_spi_close(&px_spi_handle);
    }

    // Open SPI device handle
    if(!px_spi_open2(&px_spi_handle, 
                     PX_SPI_PER_0,
                     PX_BOARD_SPI_CS_NONE,
                     baud,
                     mode,
                     data_order,
                     0x00))
    {
        return PX_PGM_STR("Error. Incorrect paramaters; unable to initialise SPI");
    }
    PX_PRINTF_P("Done. Actual baud rate = %lu Hz\n", 
           px_spi_util_clk_div_to_baud_hz(baud));

    // Set flag
    px_spi_handle_open = true;
    return NULL;
}

static const char* px_cli_cmd_fn_spi_cs(uint8_t argc, char* argv[])
{
    // <lo|hi>
    switch(px_cli_util_argv_to_option(0, PX_PGM_STR("lo\0hi\0")))
    {
    case 0: 
        px_gpio_pin_set_lo(&px_spi_cs_gpio);
        // Start SPI transaction
        px_spi_wr(&px_spi_handle, NULL, 0, PX_SPI_FLAG_START);
        break;
    case 1: 
        px_gpio_pin_set_hi(&px_spi_cs_gpio);
        // Stop SPI transaction
        px_spi_wr(&px_spi_handle, NULL, 0, PX_SPI_FLAG_STOP);
        break;
    default:
        return PX_PGM_STR("Error: Invalid parameter");
    }

    return NULL;
}

static const char* px_cli_cmd_fn_spi_wr(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t data[8];

    // <data>
    for(i=0; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            PX_PRINTF_P("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        data[i] = px_cli_argv_val.u8;
    }

    // Write data to SPI slave
    px_spi_wr(&px_spi_handle, data, argc, 0);

    return NULL;
}

static const char* px_cli_cmd_fn_spi_rd(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t nr_of_bytes;
    uint8_t data[16];

    // <nr of bytes>
    if(!px_cli_util_argv_to_u8(0, 1, 16))
    {
        return PX_PGM_STR("Error. <nr of bytes> must be 1 to 16");
    }
    nr_of_bytes = px_cli_argv_val.u8;

    // Read data from SPI slave
    px_spi_rd(&px_spi_handle, data, nr_of_bytes, 0);

    // Report received data
    for(i=0; i<nr_of_bytes; i++)
    {
        PX_PRINTF_P("0x%02x ", data[i]);
    }
    putchar('\n');

    return NULL;
}

static const char* px_cli_cmd_fn_spi_exchange(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t data[16];

    // <data>
    for(i=0; i<argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            PX_PRINTF_P("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        data[i] = px_cli_argv_val.u8;
    }

    // Exchange data with SPI slave
    px_spi_xc(&px_spi_handle, data, data, argc, PX_SPI_FLAG_START_AND_STOP);

    // Report received data
    for(i=0; i<argc; i++)
    {
        PX_PRINTF_P("0x%02x ", data[i]);
    }
    putchar('\n');

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_spi_cfg,       "cfg",  0, 5,   "[cs] [baud] [mode] [order]",       "Configure SPI")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_cs,        "cs",   1, 1,   "<lo|hi>",                          "Take configured CS pin low or high")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_wr,        "w",    1, 8,   "<d0> .. [d7]",                     "Write up to 8 bytes of data to an SPI slave")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_rd,        "r",    1, 1,   "<nr of bytes>",                    "Read up to 16 bytes of data from an SPI slave")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_exchange,  "x",    1, 8,   "<d0> .. [d7]",                     "Exchange up to 8 bytes of data to an SPI slave")

PX_CLI_GROUP_CREATE(px_cli_group_spi, "spi")
    PX_CLI_CMD_ADD(px_cli_cmd_spi_cfg,      px_cli_cmd_fn_spi_cfg)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_cs,       px_cli_cmd_fn_spi_cs)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_wr,       px_cli_cmd_fn_spi_wr)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_rd,       px_cli_cmd_fn_spi_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_exchange, px_cli_cmd_fn_spi_exchange)    
PX_CLI_GROUP_END()
