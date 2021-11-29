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
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_i2c");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void px_i2c_gpio_enable(void)
{
    px_gpio_mode_set(&px_gpio_i2c1_scl, PX_GPIO_MODE_OUT);
    px_gpio_mode_set(&px_gpio_i2c1_sda, PX_GPIO_MODE_OUT);
}

static void px_i2c_gpio_disable(void)
{
    px_gpio_mode_set(&px_gpio_i2c1_scl, PX_GPIO_MODE_AF);
    px_gpio_mode_set(&px_gpio_i2c1_sda, PX_GPIO_MODE_AF);
}

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_i2c_check_idle_state(void)
{
    px_i2c_gpio_enable();
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SCL))
    {
        printf("Error. SCL is 0\n");
    }
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SDA))
    {
        printf("Error. SDA is 0. Try I2C Bus Reset procedure\n");
    }
    px_i2c_gpio_disable();

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_info(uint8_t argc, char * argv[])
{
    px_i2c_gpio_enable();
    printf("SCL = ");
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SCL))
    {
        printf("0\n");
    }
    else
    {
        printf("1\n");
    }
    printf("SDA = ");
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SDA))
    {
        printf("0\n");
    }
    else
    {
        printf("1\n");
    }

    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SCL))
    {
        printf("Error. SCL is 0\n");
    }
    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SDA))
    {
        printf("Error. SDA is 0. Try I2C Bus Reset procedure\n");
    }

    px_i2c_gpio_disable();

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_reset(uint8_t argc, char * argv[])
{
    uint8_t i;

    px_i2c_gpio_enable();

    if(PX_GPIO_IN_IS_LO(PX_GPIO_I2C1_SCL))
    {
        px_i2c_gpio_disable();
        return "Error. SCL is 0. I2C Bus reset requires SCL to be released";
    }

    // Clock SCL 8 times
    for(i = 8; i != 0; i--)
    {
        PX_GPIO_OUT_SET_LO(PX_GPIO_I2C1_SCL);
        px_board_delay_us(1000);
        PX_GPIO_OUT_SET_HI(PX_GPIO_I2C1_SCL);
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
            printf("%02X W\n", adr);
        }
        // SLA+R
        if(px_i2c_rd(&px_i2c_handle, &data, 1, PX_I2C_FLAG_START_AND_STOP))
        {
            printf("%02X R\n", adr);            
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
        return "Error. <adr> must be 0 to 127";
    }
    adr = px_cli_argv_val.u8;

    // <data>
    nr_of_bytes = argc - 1;
    if(nr_of_bytes > 8)
    {
        return "Error. Number of bytes to write must be 8 or less";
    }

    for(i = 0; i < nr_of_bytes; i++)
    {
        if(!px_cli_util_argv_to_u8(i + 1, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i + 1);
            return NULL;
        }
        data[i] = px_cli_argv_val.u8;
    }

    px_i2c_change_slave_adr(&px_i2c_handle, adr);
    if(!px_i2c_wr(&px_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_START_AND_STOP))
    {
        return "Error!";
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
        return "Error. <adr> must be 0 to 127";
    }
    adr = px_cli_argv_val.u8;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u8(1, 1, 16))
    {
        return "Error. <nr of bytes> must be 1 to 16";
    }
    nr_of_bytes = px_cli_argv_val.u8;

    px_i2c_change_slave_adr(&px_i2c_handle, adr);
    if(!px_i2c_rd(&px_i2c_handle, data, nr_of_bytes, PX_I2C_FLAG_START_AND_STOP))
    {
        return "Error!";
    }    

    for(i = 0; i < nr_of_bytes; i++)
    {
        printf("%02X ", (unsigned int)data[i]);
    }
    putchar('\n');

    return NULL;
}

static const char * px_cli_cmd_fn_i2c_wr_rd(uint8_t argc, char * argv[])
{
    uint8_t i;
    uint8_t adr;
    uint8_t data[16];
    uint8_t rd_nr_of_bytes;
    uint8_t wr_nr_of_bytes;

    // <adr>
    if(!px_cli_util_argv_to_u8(0, 0, 127))
    {
        return "Error. <adr> must be 0 to 127";
    }
    adr = px_cli_argv_val.u8;

    // <rd nr of bytes>
    if(!px_cli_util_argv_to_u8(1, 1, 16))
    {
        return "Error. <rd nr of bytes> must be 1 to 16";
    }
    rd_nr_of_bytes = px_cli_argv_val.u8;

    // <data>
    wr_nr_of_bytes = argc - 2;
    if(wr_nr_of_bytes > 8)
    {
        return "Error. Number of bytes to write must be 8 or less";
    }
    for(i = 0; i < wr_nr_of_bytes; i++)
    {
        if(!px_cli_util_argv_to_u8(i + 2, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i + 2);
            return NULL;
        }
        data[i] = px_cli_argv_val.u8;
    }

    px_i2c_change_slave_adr(&px_i2c_handle, adr);
    if(!px_i2c_wr(&px_i2c_handle, data, wr_nr_of_bytes, PX_I2C_FLAG_START_AND_END))
    {
        return "Write error!";
    }
    if(!px_i2c_rd(&px_i2c_handle, data, rd_nr_of_bytes, PX_I2C_FLAG_REP_START_AND_STOP))
    {
        return "Read error!";
    }
    for(i = 0; i < rd_nr_of_bytes; i++)
    {
        printf("%02X ", (unsigned int)data[i]);
    }
    putchar('\n');

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_info,        "info", 0,  0,  "",                                       "Report status of SCL and SDA pins")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_reset,       "rst",  0,  0,  "",                                       "Perform I2C bus reset procedure")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_scan,        "scan", 0,  0,  "",                                       "Scan for all connected I2C slaves")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_wr,          "w",    1,  9,  "<adr> <d0> .. [d7]",                     "Write up to 8 bytes of data to an I2C slave")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_rd,          "r",    2,  2,  "<adr> <nr of bytes>",                    "Read up to 16 bytes of data from an I2C slave")
PX_CLI_CMD_CREATE(px_cli_cmd_i2c_wr_rd,       "w+r",  3, 10,  "<adr> <rd nr of bytes> <d0> .. [d7]",    "Write up to 8 bytes to an I2C slave and read up to 16 bytes (REP START)")

PX_CLI_GROUP_CREATE(px_cli_group_i2c, "i2c")
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_info,        px_cli_cmd_fn_i2c_info)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_reset,       px_cli_cmd_fn_i2c_reset)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_scan,        px_cli_cmd_fn_i2c_scan)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_wr,          px_cli_cmd_fn_i2c_wr)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_rd,          px_cli_cmd_fn_i2c_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_i2c_wr_rd,       px_cli_cmd_fn_i2c_wr_rd)
PX_CLI_GROUP_END()
