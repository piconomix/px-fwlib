/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_ow.h : CLI commands for 1-Wire related operations
    Author(s):      Pieter Conradie
    Creation Date:  2019-08-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_cli_cmds_ow.h"
#include "px_one_wire.h"
#include "px_ds18b20.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli_cmds_ow");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_ow_error_to_str(px_one_wire_error_t error)
{
    switch(error)
    {
    case PX_ONE_WIRE_ERR_NONE:                  return "None";
    case PX_ONE_WIRE_ERR_NO_DEVICES_PRESENT:    return "No devices present";
    case PX_ONE_WIRE_ERR_CRC_CHECK_FAILED:      return "CRC check failed";
    case PX_ONE_WIRE_ERR_FAMILY_CODE_IS_ZERO:   return "Family code is zero";
    case PX_ONE_WIRE_ERR_LAST_DEVICE:           return "Last device";
    default:                                    return "Unknown error code";
    }
}

static const char* px_cli_cmd_fn_ds18b20_error_to_str(px_ds18b20_error_t error)
{
    switch(error)
    {
    case PX_DS18B20_ERR_NONE:               return "None";
    case PX_DS18B20_ERR_NO_DEVICES_PRESENT: return "No devices present";
    case PX_DS18B20_ERR_CRC_CHECK_FAILED:   return "CRC check failed";
    default:                                return "Unknown error code";
    }
}

static const char* px_cli_cmd_fn_ow_info(uint8_t argc, char* argv[])
{
    // Initialise 1-wire
    px_one_wire_init();

    printf("1-Wire (GPIO0) = ");
    if(PX_GPIO_PIN_IS_LO(PX_GPIO_0))
    {
        printf("0\n");
    }
    else
    {
        printf("1\n");
    }    

    return NULL;
}

static const char* px_cli_cmd_fn_ow_reset(uint8_t argc, char* argv[])
{
    // Initialise 1-wire
    px_one_wire_init();

    // Perform 1-Wire reset procedure
    if(px_one_wire_reset())
    {
        return "Device(s) present";
    }
    else
    {
        return "No device(s) present";
    }

    return NULL;
}

static const char* px_cli_cmd_fn_ow_scan(uint8_t argc, char* argv[])
{
    px_one_wire_error_t  one_wire_error;
    px_one_wire_search_t one_wire_search;

    // Initialise 1-wire
    px_one_wire_init();
    
    // Find first 1-wire device
    one_wire_error = px_one_wire_search_rom_first(&one_wire_search);
    while(one_wire_error == PX_ONE_WIRE_ERR_NONE)
    {
        // Report device
        printf("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
               one_wire_search.rom.data[0],
               one_wire_search.rom.data[1],
               one_wire_search.rom.data[2],
               one_wire_search.rom.data[3],
               one_wire_search.rom.data[4],
               one_wire_search.rom.data[5],
               one_wire_search.rom.data[6],
               one_wire_search.rom.data[7]);            
        // Find next 1-wire device
        one_wire_error = px_one_wire_search_rom_next(&one_wire_search);
    }
	return NULL;
}

static const char* px_cli_cmd_fn_rd_rom(uint8_t argc, char* argv[])
{
    uint8_t             i;
    px_one_wire_rom_t   rom;
    px_one_wire_error_t error;

    // [ROM0] .. [ROM7]
    for(i=0; i < 8; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        rom.data[i] = px_cli_argv_val.u8;
    }

    // Initialise 1-wire
    px_one_wire_init();

    // Issue Read ROM
    error = px_one_wire_rd_rom(&rom);
    if(error != PX_ONE_WIRE_ERR_NONE)
    {
        return px_cli_cmd_fn_ow_error_to_str(error);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_skip_rom(uint8_t argc, char* argv[])
{
    px_one_wire_error_t error;

    // Initialise 1-wire
    px_one_wire_init();

    // Issue SKIP ROM
    error = px_one_wire_skip_rom();
    if(error != PX_ONE_WIRE_ERR_NONE)
    {
        return px_cli_cmd_fn_ow_error_to_str(error);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_match_rom(uint8_t argc, char* argv[])
{
    uint8_t             i;
    px_one_wire_rom_t   rom;
    px_one_wire_error_t error;

    // [ROM0] .. [ROM7]
    for(i=0; i < 8; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        rom.data[i] = px_cli_argv_val.u8;
    }

    // Initialise 1-wire
    px_one_wire_init();

    // Issue MATCH ROM
    error = px_one_wire_match_rom(&rom);
    if(error != PX_ONE_WIRE_ERR_NONE)
    {
        return px_cli_cmd_fn_ow_error_to_str(error);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_ow_wr(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t data[8];
    uint8_t nr_of_bytes;

    // <data>
    nr_of_bytes = argc;
    for(i=0; i < argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        data[i] = px_cli_argv_val.u8;
    }

    for(i=0; i < nr_of_bytes; i++)
    {
        px_one_wire_wr_u8(data[i]);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_ow_rd(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t data;
    uint8_t nr_of_bytes;

    // [nr of bytes]
    if(!px_cli_util_argv_to_u8(0, 1, 16))
    {
        return "Error. <nr of bytes> must be 1 to 16";
    }
    nr_of_bytes = px_cli_argv_val.u8;

    for(i=0; i < nr_of_bytes; i++)
    {
        data = px_one_wire_rd_u8();
        printf("%02X ", (unsigned int)data);
    }
    putchar('\n');

    return NULL;
}

static const char* px_cli_cmd_fn_ds18b20_temp(uint8_t argc, char* argv[])
{
    uint8_t                 i;
    px_one_wire_rom_t       rom;
    px_ds18b20_scratchpad_t scratchpad;
    int16_t                 temp;
    int16_t                 frac;
    px_ds18b20_error_t      error;

    if(argc != 0)
    {
        // <ROM0> .. <ROM7>
        for(i=0; i < 8; i++)
        {
            if(!px_cli_util_argv_to_u8(i, 0, 255))
            {
                printf("Error. <data> at index %d invalid\n", i);
                return NULL;
            }
            rom.data[i] = px_cli_argv_val.u8;
        }
    }

    // Initialise 1-wire
    px_one_wire_init();

    // Start temperature conversion
    if(argc != 0)
    {
        error = px_ds18b20_start_temp_conversion(&rom);
    }
    else
    {
        error = px_ds18b20_start_temp_conversion(NULL);
    }    
    if(error != PX_DS18B20_ERR_NONE)
    {
        return px_cli_cmd_fn_ds18b20_error_to_str(error);
    }
    // Wait until conversion has finished
    while(!px_ds18b20_temp_conversion_finished())
    {
        ;
    }
    // Read scratchpad
    if(argc != 0)
    {
        error = px_ds18b20_rd_scratchpad(&rom, &scratchpad);
    }
    else
    {
        error = px_ds18b20_rd_scratchpad(NULL, &scratchpad);
    }
    if(error != PX_DS18B20_ERR_NONE)
    {
        return px_cli_cmd_fn_ds18b20_error_to_str(error);
    }
    // Convert temp
    temp = px_ds18b20_util_convert_t_to_deci_deg(scratchpad.temp_msb, scratchpad.temp_lsb, scratchpad.cfg_reg);
    frac = temp % 10;
    temp = temp / 10;
    // Report
    printf("0x%02X%02X %d.%1d deg C\n", scratchpad.temp_msb, scratchpad.temp_lsb, temp, frac);

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_ow_info,           "info",         0, 0,   "",                     "Report status of 1-Wire line")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_reset,          "rst",          0, 0,   "",                     "Perform 1-Wire reset procedure")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_scan,           "scan",         0, 0,   "",                     "Discover all connected 1-Wires devices")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_rd_rom,         "rd_rom",       8, 8,   "[ROM0] ... [ROM7]",    "Issue Read ROM using 8-byte adr")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_skip_rom,       "skip_rom",     0, 0,   "",                     "Issue Skip ROM")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_match_rom,      "match_rom",    8, 8,   "[ROM0] ... [ROM7]",    "Issue Match ROM using 8-byte adr")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_wr,             "w",            1, 9,   "<d0> .. [d7]",         "Write up to 8 bytes of data to a 1-Wire device")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_rd,             "r",            1, 1,   "[nr of bytes]",        "Read up to 16 bytes of data from a 1-Wire device")
PX_CLI_CMD_CREATE(px_cli_cmd_ow_ds18b20_temp,   "ds18b20_temp", 0, 8,   "<ROM0> ... <ROM7>",    "Read DS18B20 temperature (8-byte adr optional)")

PX_CLI_GROUP_CREATE(px_cli_group_ow, "ow")
    PX_CLI_CMD_ADD(px_cli_cmd_ow_info,          px_cli_cmd_fn_ow_info)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_reset,         px_cli_cmd_fn_ow_reset)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_scan,          px_cli_cmd_fn_ow_scan)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_rd_rom,        px_cli_cmd_fn_rd_rom)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_skip_rom,      px_cli_cmd_fn_skip_rom)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_match_rom,     px_cli_cmd_fn_match_rom)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_wr,            px_cli_cmd_fn_ow_wr)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_rd,            px_cli_cmd_fn_ow_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_ow_ds18b20_temp,  px_cli_cmd_fn_ds18b20_temp)
PX_CLI_GROUP_END()
