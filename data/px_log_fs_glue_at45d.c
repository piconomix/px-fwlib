/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_log_fs_glue.h : Physical device glue layer for Log FS
    Author(s):      Pieter Conradie
    Creation Date:  2019-01-11

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log_fs_glue.h"
#include "px_log_fs_cfg.h"
#include "px_at45d.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_log_fs_glue");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_log_fs_glue_rd(void *   buffer, 
                       uint16_t page,
                       uint16_t start_byte_in_page,
                       uint16_t nr_of_bytes)
{
    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);    
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(start_byte_in_page < PX_LOG_FS_CFG_PAGE_SIZE);
    PX_DBG_ASSERT(nr_of_bytes <= (PX_LOG_FS_CFG_PAGE_SIZE - start_byte_in_page));    

    // Read data from Serial Flash
    px_at45d_rd_page_offset(buffer, page, start_byte_in_page, nr_of_bytes);
}

void px_log_fs_glue_wr(const void * buffer,
                       uint16_t     page,
                       uint16_t     start_byte_in_page,
                       uint16_t     nr_of_bytes)
{
    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);    
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(start_byte_in_page < PX_LOG_FS_CFG_PAGE_SIZE);
    PX_DBG_ASSERT(nr_of_bytes <= (PX_LOG_FS_CFG_PAGE_SIZE - start_byte_in_page));    

    // Write data to Serial Flash
    px_at45d_wr_page_offset(buffer, page, start_byte_in_page, nr_of_bytes);
}

void px_log_fs_glue_erase_block(uint16_t page)
{
    uint8_t i;

    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(page & (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1) == 0);
    
    for(i = PX_LOG_FS_CFG_ERASE_BLOCK_SIZE; i != 0; i--)
    {
        // Erase page of Serial Flash
        px_at45d_erase(page++);
    }
}
