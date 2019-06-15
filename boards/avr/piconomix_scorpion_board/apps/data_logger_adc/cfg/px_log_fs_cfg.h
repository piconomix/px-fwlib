#ifndef __PX_LOG_FS_CFG_H__
#define __PX_LOG_FS_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_log_fs_cfg.h : Log file system configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_at45d.h"
#include "px_adc.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Page size for file system
#define PX_LOG_FS_CFG_PAGE_SIZE        PX_AT45D_PAGE_SIZE

/// Start page for file system (must be integer multiple of PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)
#define PX_LOG_FS_CFG_PAGE_START       0

/// End page for file system (must be integer multiple of PX_LOG_FS_CFG_ERASE_BLOCK_SIZE)
#define PX_LOG_FS_CFG_PAGE_END         (PX_AT45D_PAGES-1)

/// Erase block size (in pages) for file system
#define PX_LOG_FS_CFG_ERASE_BLOCK_SIZE 1

/// Record data size (total record size = PX_LOG_FS_CFG_REC_DATA_SIZE + 3 bytes overhead)
#define PX_LOG_FS_CFG_REC_DATA_SIZE    (sizeof(px_adc_data_t) * PX_ADC_NR_OF_CHANNELS)

/// File type (PX_LOG_FS_CFG_TYPE_LINEAR or PX_LOG_FS_CFG_TYPE_CIRCULAR)
#define PX_LOG_FS_CFG_TYPE             PX_LOG_FS_CFG_TYPE_LINEAR

/// Maximum number of pages allocated to file. 0 means no limit
#define PX_LOG_FS_CFG_MAX_PAGES        0

/// @}
#endif // #ifndef __PX_LOG_FS_CFG_H__
