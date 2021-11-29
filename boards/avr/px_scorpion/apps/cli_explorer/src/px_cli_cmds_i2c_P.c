/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_i2c.h : CLI commands for I2C
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_i2c.h"
#include "main.h"
#include "px_pgm_P.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_i2c");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void px_i2c_gpio_enable(void)
{
    px_i2c_close(&px_i2c_handle);
}

static void px_i2c_gpio_disable(void)
{
    px_i2c_open(&px_i2c_handle, PX_I2C_NR_0, 0x00);
}

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_i2c_check_idle_state(void)
{
    px_i2c_gpio_enable();
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SCL))
    {
        PX_PRINTF_P("Error. SCL is 0\n");
    }
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SDA))
    {
        PX_PRINTF_P("Error. SDA is 0. Try I2C Bus Reset procedure\n");
    }
    px_i2c_gpio_disable();

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_info(uint8_t argc, char * argv[])
{
    px_i2c_gpio_enable();
    PX_PRINTF_P("SCL = %c\n", PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SCL)? '0' : '1');
    PX_PRINTF_P("SDA = %c\n", PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SDA)? '0' : '1');
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SCL))
    {
        PX_PRINTF_P("Error. SCL is 0\n");
    }
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SDA))
    {
        PX_PRINTF_P("Error. SDA is 0. Try I2C Bus Reset procedure\n");
    }
    px_i2c_gpio_disable();

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_reset(uint8_t argc, char * argv[])
{
    uint8_t i;

    px_i2c_gpio_enable();

    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C_SCL))
    {
        px_i2c_gpio_disable();
        return PX_PGM_STR("Error. SCL is 0. I2C Bus reset requires SCL to be released");
    }

    // Clock SCL 8 times
    for(i = 8; i != 0; i--)
    {
        PX_GPIO_OUT_SET_LO(PX_GPIO_I2C_SCL);
        px_board_delay_us(1000);
        PX_GPIO_OUT_SET_HI(PX_GPIO_I2C_SCL);
        px_board_delay_us(1000);
    }

    px_i2c_gpio_disable();

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_scan(uint8_t argc, char * argv[])
{
    uint8_t      adr;
    uint8_t      data;
    const char * result;

    // Make sure I2C bus is in correct state
    result = px_i2c_check_idle_state();
    if(result != NULL)
    {
        return result;
    }

    // Scan all 7-bit addresses
    for(adr = 0; adr < 128; adr++)
    {
        px_i2c_change_slave_adr(&px_i2c_handle, adr);
        // SLA+W
        if(px_i2c_wr(&px_i2c_handle, NULL, 0, PX_I2C_FLAG_START_AND_STOP))
        {
            PX_PRINTF_P("%02X W\n", adr);
        }
        // SLA+R
        if(px_i2c_rd(&px_i2c_handle, &data, 1, PX_I2C_FLAG_START_AND_STOP))
        {
            PX_PRINTF_P("%02X R\n", adr);            
        }        
    }

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_wr(uint8_t argc, char * argv[])
{
    uint8_t i;
    uint8_t adr;
    uint8_t data[8];
    uint8_t nr_of_bytes;

    // <adr>
    if(!px_cli_util_argv_to_u8(0, 0, 127))
    {
        return PX_PGM_STR("Error. <adr> must be 0 to 127");
    }
    adr = px_cli_argv_val.u8;

    // <data>
    nr_of_bytes = argc - 1;
    for(i = 1; i < argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            PX_PRINTF_P("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        data[i-1] = px_cli_argv_val.u8;
    }

    px_i2c_change_slave_adr(&px_i2c_handle, adr);
    if(!px_i2c_wr(&px_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_START_AND_STOP))
    {
        return PX_PGM_STR("Error!");
    }

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_rd(uint8_t argc, char * argv[])
{
    uint8_t i;
    uint8_t adr;
    uint8_t data[16];
    uint8_t nr_of_bytes;

    // <adr>
    if(!px_cli_util_argv_to_u8(0, 0, 127))
    {
        return PX_PGM_STR("Error. <adr> must be 0 to 127");
    }
    adr = px_cli_argv_val.u8;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u8(1, 1, 16))
    {
        return PX_PGM_STR("Error. <nr of bytes> must be 1 to 16");
    }
    nr_of_bytes = px_cli_argv_val.u8;

    px_i2c_change_slave_adr(&px_i2c_handle, adr);
    if(!px_i2c_rd(&px_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_START_AND_STOP))
    {
        return PX_PGM_STR("Error!");
    }    

    for(i = 0; i < nr_of_bytes; i++)
    {
        PX_PRINTF_P("%02X ", (unsigned int)data[i]);
    }
    putchar('\n');

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_info,        "info", 0, 0,   "",                       "Report status of SCL and SDA pins")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_reset,       "rst",  0, 0,   "",                       "Perform I2C bus reset procedure")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_scan,        "scan", 0, 0,   "",                       "Scan for all connected I2C slaves")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_wr,          "w",    1, 9,   "<adr> <d0> .. [d7]",     "Write up to 8 bytes of data to an I2C slave")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_rd,          "r",    2, 2,   "<adr> <nr of bytes>",    "Read up to 16 bytes of data from an I2C slave")

PX_CLI_GROUP_CREATE(px_cli_group_i2c, "i2c")
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_info,        px_cli_cmd_fn_i2c_info)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_reset,       px_cli_cmd_fn_i2c_reset)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_scan,        px_cli_cmd_fn_i2c_scan)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_wr,          px_cli_cmd_fn_i2c_wr)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_rd,          px_cli_cmd_fn_i2c_rd)
PX_CLI_GROUP_END()
