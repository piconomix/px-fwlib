/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_mem.h : CLI commands for memory related operations
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_cli_cmds_mem.h"
#include "main.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_mem");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_fn_mem_rd(uint8_t argc, char * argv[])
{
    uint32_t adr;
    uint8_t  count;

    // <adr>
    if(!px_cli_util_argv_to_u32(1, 0, PX_U32_MAX))
    {
        printf("Error. <adr> must be 0 to 0x%x\n", PX_U32_MAX);
        return NULL;
    }
    adr = px_cli_argv_val.u32;

    // [count]
    if(argc > 2)
    {
        if(!px_cli_util_argv_to_u8(2, 1, 16))
        {
            return "Error. [count] must be 1 to 16";
        }
        count = px_cli_argv_val.u8;
    }
    else
    {
        count = 1;
    }

    // <b|w|l>
    switch(argv[0][0])
    {
    case 'b':
    {
        uint8_t * data = (uint8_t *)adr;
        while(count != 0)
        {
            printf("0x%02x %u\n", *data, *data);
            data++;
            count--;
        }
        break;
    }

    case 'w':
    {
        uint16_t * data = (uint16_t *)adr;
        while(count != 0)
        {
            printf("0x%04x %u\n", *data, *data);
            data++;
            count--;
        }
        break;
    }

    case 'l':
    {
        uint32_t * data = (uint32_t *)adr;
        while(count != 0)
        {
            printf("0x%08lx %lu\n", *data, *data);
            data++;
            count--;
        }
        break;
    }

    default:
        return "Error! <b|w|l> must be b (byte:8-bits), w (word:16-bits) or l (long:32-bits)"; 
    }

    return NULL;
}

static const char * px_cli_cmd_fn_mem_wr(uint8_t argc, char * argv[])
{
    uint32_t adr;
    uint8_t  count;

    // <adr>
    if(!px_cli_util_argv_to_u32(1, 0, PX_U32_MAX))
    {
        printf("Error. <adr> must be 0 to 0x%x\n", PX_U32_MAX);
        return NULL;
    }
    adr = px_cli_argv_val.u32;

    // <b|w|l>
    count = argc - 2;
    switch(argv[0][0])
    {
    case 'b':
    {
        uint8_t * data = (uint8_t *)adr;
        while(count != 0)
        {
            px_cli_util_argv_to_u8(count + 2, 0, PX_U8_MAX);
            *data++ = px_cli_argv_val.u8;
            count--;
        }
        break;
    }

    case 'w':
    {
        uint16_t * data = (uint16_t *)adr;
        while(count != 0)
        {
            px_cli_util_argv_to_u16(count + 2, 0, PX_U16_MAX);
            *data++ = px_cli_argv_val.u16;
            count--;
        }
        break;
    }

    case 'l':
    {
        uint32_t * data = (uint32_t *)adr;
        while(count != 0)
        {
            px_cli_util_argv_to_u32(count + 2, 0, PX_U32_MAX);
            *data++ = px_cli_argv_val.u32;
            count--;
        }
        break;
    }

    default:
        return "Error! <b|w|l> must be b (byte:8-bits), w (word:16-bits) or l (long:32-bits)"; 
    }

    return NULL;
}

static const char * px_cli_cmd_fn_mem_dump(uint8_t argc, char * argv[])
{
    uint32_t adr;
    uint16_t nr_of_bytes;

    // <adr>
    if(!px_cli_util_argv_to_u32(0, 0, PX_U32_MAX))
    {
        printf("Error. <adr> must be 0 to 0x%x\n", PX_U32_MAX);
        return NULL;
    }
    adr = px_cli_argv_val.u32;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u16(1, 1, MAIN_BUFFER_SIZE))
    {
        printf("Error. <nr of bytes> must be 1 to %u\n", MAIN_BUFFER_SIZE);
        return NULL;
    }
    nr_of_bytes = px_cli_argv_val.u16;

    memcpy(main_buffer, (void *)adr, nr_of_bytes);
    px_cli_util_disp_buf(main_buffer, nr_of_bytes);

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_mem_rd,    "r",    2, 3,   "<b|w|l> <adr> [count]",        "Read a byte/word/long from memory. Optionally a count can be specified")
PX_CLI_CMD_CREATE(px_cli_cmd_mem_wr,    "w",    3, 10,  "<b|w|l> <adr> <d0> .. [d7]",   "Write up to 8 bytes/words/longs to memory")
PX_CLI_CMD_CREATE(px_cli_cmd_mem_dump,  "d",    2, 2,   "<adr> <nr of bytes>",          "Dump a specified number of bytes from memory")

PX_CLI_GROUP_CREATE(px_cli_group_mem, "mem")
    PX_CLI_CMD_ADD(px_cli_cmd_mem_rd,       px_cli_cmd_fn_mem_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_mem_wr,       px_cli_cmd_fn_mem_wr)
    PX_CLI_CMD_ADD(px_cli_cmd_mem_dump,     px_cli_cmd_fn_mem_dump)
PX_CLI_GROUP_END()
