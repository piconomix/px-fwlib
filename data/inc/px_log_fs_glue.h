#ifndef __PX_LOG_FS_GLUE_H__
#define __PX_LOG_FS_GLUE_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_log_fs_glue.h : Physical device glue layer for Log FS
    Author(s):      Pieter Conradie
    Creation Date:  2019-01-11

============================================================================= */

/** 
 *  @addtogroup PX_LOG_FS 
 *
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Read data from Serial Flash.
 *  
 *  This glue function reads data from Serial Flash and stores it in the 
 *  specified buffer.
 *  
 *  @param buffer               Buffer to store read data
 *  @param page                 PX_LOG_FS_CFG_PAGE_START to PX_LOG_FS_CFG_PAGE_END 
 *  @param start_byte_in_page   0 to PX_LOG_FS_CFG_PAGE_SIZE - 1
 *  @param nr_of_bytes          1 to PX_LOG_FS_CFG_PAGE_SIZE  
 *  
 */
void px_log_fs_glue_rd(void *   buffer, 
                       uint16_t page,
                       uint16_t start_byte_in_page,
                       uint16_t nr_of_bytes);

/**
 *  Writes data to Serial Flash.
 *  
 *  This glue function write data from Serial Flash and stores it in the
 *  specified buffer.
 *  
 *  @param buffer               Buffer to store read data
 *  @param page                 PX_LOG_FS_CFG_PAGE_START to PX_LOG_FS_CFG_PAGE_END 
 *  @param start_byte_in_page   0 to PX_LOG_FS_CFG_PAGE_SIZE - 1
 *  @param nr_of_bytes          1 to PX_LOG_FS_CFG_PAGE_SIZE 
 *  
 */
void px_log_fs_glue_wr(const void * buffer,
                       uint16_t     page,
                       uint16_t     start_byte_in_page,
                       uint16_t     nr_of_bytes);

/**
 *  Erases data in Serial Flash.
 *  
 *  This glue function erases a block of pages in Serial Flash.
 *  
 *  @param page                 PX_LOG_FS_CFG_PAGE_START to PX_LOG_FS_CFG_PAGE_END
 *  
 */
void px_log_fs_glue_erase_block(uint16_t page);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
