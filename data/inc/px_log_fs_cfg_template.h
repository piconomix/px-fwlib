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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_fs_cfg.h : Log file system configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-09-09

============================================================================= */

/**
 *  @addtogroup PX_LOG_FS
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_at25s.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Config version
#define PX_LOG_FS_CFG_VER               2

/// Page size for file system
#define PX_LOG_FS_CFG_PAGE_SIZE         PX_AT25S_PAGE_SIZE

/// Erase block size (in pages) for file system
#define PX_LOG_FS_CFG_ERASE_BLOCK_SIZE  PX_AT25S_PAGES_PER_BLOCK_4KB

/// Record data size (total record size = PX_LOG_FS_CFG_REC_DATA_SIZE + 3 bytes overhead)
#define PX_LOG_FS_CFG_REC_DATA_SIZE     13

/// Stop writing when full (1) or erase oldest records and continue writing (0)
#define PX_LOG_FS_CFG_STOP_WR_WHEN_FULL 0

/// @}
#endif
