#ifndef __PX_DBG_CFG_H__
#define __PX_DBG_CFG_H__
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
    
    Title:          px_dbg_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef PX_DBG
/// Flag to disable (PX_DBG=0) or enable (PX_DBG=1) debug.
#define PX_DBG 0
#endif

#ifndef PX_DBG_CFG_MSG_LEVEL
/**
 * Global debug output level.
 *  
 * It is a bitmask that sets which debug info will be emmitted. E.g.
 * - PX_DBG_CFG_MSG_LEVEL = PX_DBG_CFG_MSG_LEVEL_NONE : No debug output
 * - PX_DBG_CFG_MSG_LEVEL = PX_DBG_CFG_MSG_LEVEL_ERR  : Report errors only
 * - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR|PX_DBG_CFG_MSG_LEVEL_WARN) : Report errors + warnings
 * - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR|PX_DBG_CFG_MSG_LEVEL_WARN|PX_DBG_CFG_MSG_LEVEL_INFO) : Report errors + warnings + info
 */
#define PX_DBG_CFG_MSG_LEVEL PX_DBG_CFG_MSG_LEVEL_ERR
#else
#warning "PX_DBG_CFG_MSG_LEVEL already defined"
#endif

#ifndef PX_DBG_CFG_NAME_LINE_ONLY
/// Option to decrease debug footprint by displaying name and line only
#define PX_DBG_CFG_NAME_LINE_ONLY 0
#else
#warning "PX_DBG_CFG_NAME_LINE_ONLY already defined"
#endif

/// Debug output string buffer size
#define PX_DBG_CFG_BUF_SIZE 32

#endif // #ifndef __PX_DBG_CFG_H__
