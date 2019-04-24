#ifndef __PX_LOG_FS_CFG_H__
#define __PX_LOG_FS_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
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

/// Start page for file system
#define PX_LOG_FS_CFG_PAGE_START       0

/// End page for file system
#define PX_LOG_FS_CFG_PAGE_END         (PX_AT45D_PAGES-1)

/// Record data size
#define PX_LOG_FS_CFG_REC_DATA_SIZE    (sizeof(px_adc_data_t) * PX_ADC_NR_OF_CHANNELS)

/// Log type
#define PX_LOG_FS_CFG_TYPE             PX_LOG_FS_CFG_TYPE_LINEAR

/// Maximum number of pages allocated to file. 0 means no limit
#define PX_LOG_FS_CFG_MAX_PAGES        0

/// @}
#endif // #ifndef __PX_LOG_FS_CFG_H__
