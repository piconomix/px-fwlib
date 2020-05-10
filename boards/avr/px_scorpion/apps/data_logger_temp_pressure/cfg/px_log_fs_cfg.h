#ifndef __PX_LOG_FS_CFG_H__
#define __PX_LOG_FS_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_log_fs_cfg.h : Log file system configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-09-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_at45d.h"
#include "px_rtc_util.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Record data content
typedef struct
{
    px_rtc_sec_since_y2k_t timestamp;    
    int32_t                bmp280_temp;
    int32_t                bmp280_press;
    int16_t                ds18b20_temp;
} px_log_fs_record_data_t;

/// Page size for file system
#define PX_LOG_FS_CFG_PAGE_SIZE         PX_AT45D_PAGE_SIZE

/// Erase block size (in pages) for file system
#define PX_LOG_FS_CFG_ERASE_BLOCK_SIZE  1

/// Record data size (total record size = PX_LOG_FS_CFG_REC_DATA_SIZE + 3 bytes overhead)
#define PX_LOG_FS_CFG_REC_DATA_SIZE     (sizeof(px_log_fs_record_data_t))

/// Stop writing when full (1) or erase oldest records and continue writing (0)
#define PX_LOG_FS_CFG_STOP_WR_WHEN_FULL 1

/// @}
#endif // #ifndef __PX_LOG_FS_CFG_H__
