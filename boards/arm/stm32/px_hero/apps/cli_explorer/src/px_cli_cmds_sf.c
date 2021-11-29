/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_sf.h : CLI commands for AT25S Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_at25s.h"
#include "px_xmodem.h"
#include "px_board.h"
#include "main.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_sf");

// Make sure buffer is big enough to hold a page of data
#if (MAIN_BUFFER_SIZE < (PX_AT25S_PAGE_SIZE-1))
#error "Make MAIN_BUFFER_SIZE larger"
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint16_t px_cli_cmd_sf_page;
static uint16_t px_cli_cmd_sf_end_page;
static uint16_t px_cli_cmd_sf_offset;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_fn_sf_info(uint8_t argc, char * argv[])
{
    uint8_t  status;
    uint8_t data[3];

    px_at25s_rd_man_and_dev_id(data);
    printf("Manufacturer and Device ID: %02X %02X %02X\n",  data[0], data[1], data[2]);

    printf("Size: %lu KB\n", PX_AT25S_FLASH_SIZE_BYTES / 1024);
    printf("Pages: %lu\n", PX_AT25S_PAGES);
    printf("Page size: %u bytes\n", PX_AT25S_PAGE_SIZE);
    printf("Blocks: %lu\n", PX_AT25S_FLASH_SIZE_BYTES / PX_AT25S_BLOCK_SIZE_4KB);
    printf("Block size: 4KB\n");

    status = px_at25s_rd_status_reg1();
    printf("Status: 0x%02X\n", status);
    printf("- SRP0 = %d\n", ((status & PX_AT25S_STATUS_REG1_SRP0) != 0));
    printf("- SEC  = %d\n", ((status & PX_AT25S_STATUS_REG1_SEC)  != 0));
    printf("- TB   = %d\n", ((status & PX_AT25S_STATUS_REG1_TB)   != 0));
    printf("- BP2  = %d\n", ((status & PX_AT25S_STATUS_REG1_BP2)  != 0));
    printf("- BP1  = %d\n", ((status & PX_AT25S_STATUS_REG1_BP1)  != 0));
    printf("- BP0  = %d\n", ((status & PX_AT25S_STATUS_REG1_BP0)  != 0));
    printf("- WEL  = %d\n", ((status & PX_AT25S_STATUS_REG1_WEL)  != 0));
    printf("- BSY  = %d\n", ((status & PX_AT25S_STATUS_REG1_BSY)  != 0));

    return NULL;
}

static const char * px_cli_cmd_fn_sf_erase(uint8_t argc, char * argv[])
{
    uint16_t page_start = 0;
    uint16_t page_end   = PX_AT25S_PAGES - 1;

    // [start page]
    if(argc >= 1)
    {
        if(!px_cli_util_argv_to_u16(0, 0, (PX_AT25S_PAGES - 1)))
        {
            printf("Error. [start page] must be 0 to %lu\n", (PX_AT25S_PAGES - 1));
            return NULL;
        }
        page_start = px_cli_argv_val.u16;
        page_end   = page_start + PX_AT25S_PAGES_PER_BLOCK_4KB - 1;
    }
    if(page_start % PX_AT25S_PAGES_PER_BLOCK_4KB != 0)
    {
        return "Error. Start page not on the start of a 4KB boundary";
    }

    // [end page]
    if(argc == 2)
    {
        if(!px_cli_util_argv_to_u16(1, 0, (PX_AT25S_PAGES - 1)))
        {
            printf("Error. [end page] must be 0 to %lu\n", PX_AT25S_PAGES - 1);
            return NULL;
        }
        page_end = px_cli_argv_val.u16;
    }
    if(page_end % PX_AT25S_PAGES_PER_BLOCK_4KB != (PX_AT25S_PAGES_PER_BLOCK_4KB - 1))
    {
        return "Error. End page not on the end of a 4KB boundary";
    }

    printf("Erasing %d to %d", page_start, page_end);
    do
    {
        px_at25s_erase(PX_AT25S_BLOCK_4KB, page_start);
        printf(".");
        page_start += PX_AT25S_PAGES_PER_BLOCK_4KB;
    }
    while(page_start <= page_end);
    printf(" done\n");
    
    return NULL;
}

static const char * px_cli_cmd_fn_sf_rd(uint8_t argc, char * argv[])
{
    uint32_t adr;
    uint16_t nr_of_bytes;

    // <adr>
    if(!px_cli_util_argv_to_u32(0, 0,(PX_AT25S_FLASH_SIZE_BYTES - 1)))
    {
        printf("Error. <adr> must be 0 to 0x%08lX\n", PX_AT25S_FLASH_SIZE_BYTES - 1);
        return NULL;
    }
    adr = px_cli_argv_val.u32;

    // <nr of bytes>
    if(!px_cli_util_argv_to_u16(1, 1,(PX_AT25S_PAGE_SIZE - 1)))
    {
        printf("Error. <nr of bytes> must be 1 to %d\n", PX_AT25S_PAGE_SIZE - 1);
        return NULL;
    }
    nr_of_bytes = px_cli_argv_val.u16;

    px_at25s_rd(main_buffer, adr, nr_of_bytes);
    px_cli_util_disp_buf(main_buffer, nr_of_bytes);
    return NULL;
}

static const char * px_cli_cmd_fn_sf_rd_page(uint8_t argc, char * argv[])
{
    uint16_t page;

    // <page>
    if(!px_cli_util_argv_to_u16(0, 0,(PX_AT25S_PAGES-1)))
    {
        printf("Error. <page> must be 0 to %lu\n", PX_AT25S_PAGES-1);
        return NULL;
    }
    page = px_cli_argv_val.u16;

    px_at25s_rd_page(main_buffer, page);
    px_cli_util_disp_buf(main_buffer, PX_AT25S_PAGE_SIZE);
    return NULL;
}

static bool px_xmodem_on_tx_data(uint8_t * data, uint8_t bytes_to_send)
{
    uint16_t nr_of_bytes;

    while(bytes_to_send != 0)
    {
        // Calculate maximum remaining bytes that can be read from page
        nr_of_bytes = PX_AT25S_PAGE_SIZE - px_cli_cmd_sf_offset;
        // More than enough bytes left in page?
        if(nr_of_bytes > bytes_to_send)
        {
            // Only read requested number of bytes
            nr_of_bytes = bytes_to_send;
        }
        // Read data from Serial Flash
        px_at25s_rd_page_offset(data,
                             px_cli_cmd_sf_page,
                             px_cli_cmd_sf_offset,
                             nr_of_bytes);
        // Advance offset (and page)
        px_cli_cmd_sf_offset += nr_of_bytes;
        while(px_cli_cmd_sf_offset >= PX_AT25S_PAGE_SIZE)
        {
            px_cli_cmd_sf_offset -= PX_AT25S_PAGE_SIZE;
            // Next page
            px_cli_cmd_sf_page++;
            // Transfer finished?
            if(px_cli_cmd_sf_page > px_cli_cmd_sf_end_page)
            {
                return false;
            }
        }
        // Advance data buffer pointer
        data += nr_of_bytes;
        // Calculate remain number of bytes to read
        bytes_to_send -= nr_of_bytes;
    }

    return true;
}

static const char * px_cli_cmd_fn_sf_rd_xmodem(uint8_t argc, char * argv[])
{
    uint16_t start_page;
    uint16_t end_page;

    // [start page]
    if(argc > 0)
    {
        if(!px_cli_util_argv_to_u16(0, 0,(PX_AT25S_PAGES-1)))
        {
            printf("Error. [start page] must be 0 to %lu\n", PX_AT25S_PAGES-1);
            return NULL;
        }
        start_page = px_cli_argv_val.u16;
    }
    else
    {
        start_page = 0;
    }

    // [end page]
    if(argc > 1)
    {
        if(!px_cli_util_argv_to_u16(1, 0,(PX_AT25S_PAGES-1)))
        {
            printf("Error. [end page] must be 0 to %lu\n", PX_AT25S_PAGES-1);
            return NULL;
        }
        end_page = px_cli_argv_val.u16;
    }
    else
    {
        end_page = PX_AT25S_PAGES-1;
    }

    // Wait until button is pressed
    printf("Press button when host is ready to receive file (after XMODEM transfer has been started)...\n");
    while(!PX_USR_PB_IS_PRESSED()) {;}

    // Send Serial Flash content using XMODEM protocol
    px_cli_cmd_sf_page     = start_page;
    px_cli_cmd_sf_end_page = end_page;
    px_cli_cmd_sf_offset   = 0;
    px_xmodem_send_file(&px_xmodem_on_tx_data);

    return NULL;
}

static const char * px_cli_cmd_fn_sf_wr_page(uint8_t argc, char * argv[])
{
    uint16_t page;
    uint16_t offset;
    uint8_t  data;
    uint8_t  i;

    // <page>
    if(!px_cli_util_argv_to_u16(0, 0,(PX_AT25S_PAGES-1)))
    {
        printf("Error. <page> must be 0 to %lu\n", PX_AT25S_PAGES-1);
        return NULL;
    }
    page = px_cli_argv_val.u16;

    // <offset>
    if(!px_cli_util_argv_to_u16(1, 0,(PX_AT25S_PAGE_SIZE-1)))
    {
        printf("Error. <offset> must be 0 to %d\n", PX_AT25S_PAGE_SIZE-1);
        return NULL;
    }
    offset = px_cli_argv_val.u16;

    // <data>
    for(i = 2; i < argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255))
        {
            printf("Error. <data> at index %d invalid\n", i);
            return NULL;
        }
        data = px_cli_argv_val.u8;

        px_at25s_wr_page_offset(&data, page, offset, 1);
        if(++offset >= PX_AT25S_PAGE_SIZE)
        {
            printf("Error. Page size exceeded. <data> after index %d ignored\n", i-1);
            return NULL;
        }
    }

    return NULL;
}

static void px_xmodem_on_rx_data(const uint8_t * data, uint8_t bytes_received)
{
    uint16_t nr_of_bytes;

    while(bytes_received != 0)
    {
        // Calculate maximum remaining bytes that can be written to page
        nr_of_bytes = PX_AT25S_PAGE_SIZE - px_cli_cmd_sf_offset;
        // More than enough bytes left in page?
        if(nr_of_bytes > bytes_received)
        {
            // Only write number of bytes received
            nr_of_bytes = bytes_received;
        }
        // Start of new block?
        if(  (px_cli_cmd_sf_offset == 0)
           &&((px_cli_cmd_sf_page % PX_AT25S_PAGES_PER_BLOCK_4KB) == 0))
        {
            // Erase block first
            px_at25s_erase(PX_AT25S_BLOCK_4KB, px_cli_cmd_sf_page);
        }
        // Write data to Serial Flash
        px_at25s_wr_page_offset(data,  px_cli_cmd_sf_page, px_cli_cmd_sf_offset, nr_of_bytes);
        // Advance offset
        px_cli_cmd_sf_offset += nr_of_bytes;
        while(px_cli_cmd_sf_offset >= PX_AT25S_PAGE_SIZE)
        {
            px_cli_cmd_sf_offset -= PX_AT25S_PAGE_SIZE;
            // Next page
            px_cli_cmd_sf_page++;
            // End of Serial Flash reached?
            if(px_cli_cmd_sf_page >= PX_AT25S_PAGES)
            {
                return;
            }            
        }
        // Advance data buffer pointer
        data += nr_of_bytes;
        // Calculate remain number of bytes to write to Serial Flash
        bytes_received -= nr_of_bytes;
    }
}

static const char * px_cli_cmd_fn_sf_wr_xmodem(uint8_t argc, char * argv[])
{
    uint16_t start_page;

    // [start page]
    if(argc > 0)
    {
        if(!px_cli_util_argv_to_u16(0, 0,(PX_AT25S_PAGES-1)))
        {
            printf("Error. [start page] must be 0 to %lu\n", PX_AT25S_PAGES-1);
            return NULL;
        }
        start_page = px_cli_argv_val.u16;
    }
    else
    {
        start_page = 0;
    }

    // Wait until button is pressed
    printf("Press button when host is ready to send file (after XMODEM transfer has been started)...\n");
    while(!PX_USR_PB_IS_PRESSED()) {;}

    // Receive Serial Flash content using XMODEM protocol
    px_cli_cmd_sf_page     = start_page;
    px_cli_cmd_sf_offset   = 0;
    px_xmodem_receive_file(&px_xmodem_on_rx_data);

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_sf_info,      "info", 0, 0,   "",                              "Report Serial Flash info")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_erase,     "e",    0, 2,   "[start page] [end page]",       "Erase Serial Flash 4 KB block(s)")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_rd,        "r",    2, 2,   "<adr> <nr of bytes>",           "Read Serial Flash data")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_rd_page,   "rp",   1, 1,   "<page>",                        "Read Serial Flash page")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_rd_xmodem, "rx",   0, 2,   "[start page] [end page]",       "Read Serial Flash data and send using XMODEM")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_wr_page  , "wp",   3, 10,  "<page> <offset> <d0> .. [d7]",  "Write up to 8 bytes of data to Serial Flash")
PX_CLI_CMD_CREATE(px_cli_cmd_sf_wr_xmodem, "wx",   0, 1,   "[start page]",                  "Receive data using XMODEM and write to Serial Flash")

PX_CLI_GROUP_CREATE(px_cli_group_sf, "sf")
    PX_CLI_CMD_ADD(px_cli_cmd_sf_info,      px_cli_cmd_fn_sf_info)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_erase,     px_cli_cmd_fn_sf_erase)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_rd,        px_cli_cmd_fn_sf_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_rd_page,   px_cli_cmd_fn_sf_rd_page)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_rd_xmodem, px_cli_cmd_fn_sf_rd_xmodem)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_wr_page  , px_cli_cmd_fn_sf_wr_page)
    PX_CLI_CMD_ADD(px_cli_cmd_sf_wr_xmodem, px_cli_cmd_fn_sf_wr_xmodem)
PX_CLI_GROUP_END()
