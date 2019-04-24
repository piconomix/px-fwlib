#ifndef __PX_CIRC_BUFFER_MACROS_H__
#define __PX_CIRC_BUFFER_MACROS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_circ_buffer_macros.h : Tiny, macro based, circular buffer implementation
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-10

============================================================================= */

/** 
 * @ingroup UTILS 
 * @defgroup PX_CIRC_BUFFER_MACROS px_circ_buffer_macros.h : Tiny, macro based, circular buffer implementation
 *  
 * Tiny, macro based, circular buffer implementation.
 *  
 * File(s):
 * - utils/px_circ_buffer_macros.h
 *  
 * Source:
 *  
 * - http://idlehands.typepad.com/idle-hands/2009/11/let-freedom-circular.html
 * - http://idlehands.typepad.com/idle-hands/2009/11/buffers-continued.html
 * - http://idlehands.typepad.com/files/2009/11/buffers-continued/cq_lib.h
 *  
 * License:
 *  
 * Copyright (c) 2009, Brian Orr
 * All rights reserved.
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 *  
 *     Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *     Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *     Neither the name of the author nor the names of any contributors may 
 *     be used to endorse or promote products derived from this software 
 *     without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *  
 * @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef uint8_t px_circ_buf_index_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
/**
 * Declare a static circular buffer structure.
 *  
 * @param px_circ_buf          Name of the circular buffer structure
 * @param px_circ_buf_size     Data buffer size, which *MUST* be a multiple of 2
 *                          and equal to or less than MAX_OF_TYPE(px_circ_buf_index_t),
 *                          e.g. 2, 4, 8, 16, 32, ...
 */
#define PX_CIRC_BUF_DECLARE(px_circ_buf, px_circ_buf_size) \
struct \
{ \
    volatile px_circ_buf_index_t read_index; \
    volatile px_circ_buf_index_t write_index; \
    uint8_t                      buffer[(px_circ_buf_size)]; \
} px_circ_buf

/**
 * Initialise the circular buffer structure to be empty. 
 *  
 * @param px_circ_buf      Name of the circular buffer structure 
 */
#define PX_CIRC_BUF_INIT(px_circ_buf) \
   do \
   { \
      (px_circ_buf).read_index  = 0; \
      (px_circ_buf).write_index = 0; \
   } \
   while(0)

/**
 *  Test to see if the circular buffer is empty.
 *  
 *  @param px_circ_buf     Name of the circular buffer structure 
 */
#define PX_CIRC_BUF_EMPTY(px_circ_buf) \
   ((px_circ_buf).read_index == (px_circ_buf).write_index)


/**
 *  Test to see if the circular buffer is full.
 *  
 *  @param px_circ_buf     Name of the circular buffer structure 
 */
#define PX_CIRC_BUF_FULL(px_circ_buf) \
   ((px_circ_buf).read_index == (((px_circ_buf).write_index + 1) & (PX_SIZEOF_ARRAY((px_circ_buf).buffer)-1)))

/**
 *  Reads the next available byte from the circular buffer.
 *  
 *  The caller is responsible for making sure the circular buffer is not empty
 *  before the call. See PX_CIRC_BUF_EMPTY()
 *  
 *  @param px_circ_buf     Name of the circular buffer structure
 *  @param data         The variable that the byte will be stored in
 */
#define PX_CIRC_BUF_READ(px_circ_buf, data) \
   do \
   { \
      px_circ_buf_index_t index = (px_circ_buf).read_index; \
      (data) = (px_circ_buf).buffer[index]; \
      index = (index+1) & (PX_SIZEOF_ARRAY((px_circ_buf).buffer)-1); \
      (px_circ_buf).read_index = index; \
   } while(0)

/**
 *  Writes a byte into the circular buffer.
 *  
 *  The caller is responsible for making sure the circular buffer is not full before
 *  the call. See PX_CIRC_BUF_FULL()
 *  
 *  @param px_circ_buf     Name of the circular buffer structure
 *  @param data         The byte that will be written to the circular buffer
 */
#define PX_CIRC_BUF_WRITE(px_circ_buf, data) \
   do \
   { \
      px_circ_buf_index_t index = (px_circ_buf).write_index; \
      (px_circ_buf).buffer[index] = (data); \
      index = (index+1) & (PX_SIZEOF_ARRAY((px_circ_buf).buffer)-1); \
      (px_circ_buf).write_index = index; \
   } while(0)

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_CIRC_BUFFER_MACROS_H__
