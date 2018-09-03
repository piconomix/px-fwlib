/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_circ_buffer.h : FIFO circular buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_circ_buffer.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static inline px_circ_buf_idx_t px_circ_buf_next(px_circ_buf_t *   px_circ_buf,
                                                 px_circ_buf_idx_t idx)
{
    if(++idx >= px_circ_buf->buf_size)
    {
        // Wrap index to start of buffer
        idx = 0;
    }

    return idx;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_circ_buf_init(px_circ_buf_t *   px_circ_buf,
                      uint8_t *         buf,
                      px_circ_buf_idx_t buf_size)
{
    // Initialise the circular buffer structure to be empty
    px_circ_buf->buf      = buf;
    px_circ_buf->buf_size = buf_size;
    px_circ_buf->wr       = 0;
    px_circ_buf->rd       = 0;
}

bool px_circ_buf_empty(px_circ_buf_t * px_circ_buf)
{
    return (px_circ_buf->rd == px_circ_buf->wr);
}

bool px_circ_buf_full(px_circ_buf_t * px_circ_buf)
{
    // Calculate next position
    px_circ_buf_idx_t next = px_circ_buf_next(px_circ_buf, px_circ_buf->wr);

    return(next == px_circ_buf->rd);
}

bool px_circ_buf_wr_u8(px_circ_buf_t * px_circ_buf, 
                       const uint8_t   data)
{
    // Calculate next position
    px_circ_buf_idx_t next = px_circ_buf_next(px_circ_buf, px_circ_buf->wr);

    // Buffer full?
    if (next == px_circ_buf->rd)
    {
        // Yes. Discard byte
        return false;
    }

    // Add data to buffer
    px_circ_buf->buf[px_circ_buf->wr] = data;

    // Advance index
    px_circ_buf->wr = next;

    return true;
}

px_circ_buf_idx_t px_circ_buf_wr(px_circ_buf_t * px_circ_buf, 
                                 const void *    data, 
                                 size_t          nr_of_bytes_to_wr)
{
    px_circ_buf_idx_t next;
    px_circ_buf_idx_t bytes_written = 0;
    uint8_t *         data_u8       = (uint8_t *)data;

    while(nr_of_bytes_to_wr)
    {
        // Calculate next position
        next = px_circ_buf_next(px_circ_buf, px_circ_buf->wr);
        // Buffer full?
        if (next == px_circ_buf->rd)
        {
            // Yes. Discard rest of data
            break;
        }

        // Add data to buffer
        px_circ_buf->buf[px_circ_buf->wr] = *data_u8++;

        // Advance index
        px_circ_buf->wr = next;

        // Next byte
        bytes_written++;
        nr_of_bytes_to_wr--;
    }

    return bytes_written;
}

bool px_circ_buf_rd_u8(px_circ_buf_t * px_circ_buf,
                       uint8_t       * data)
{
    // See if there is data in the buffer
    if(px_circ_buf->rd == px_circ_buf->wr)
    {
        // Buffer is empty
        return false;
    }

    // Fetch data
    *data = px_circ_buf->buf[px_circ_buf->rd];

    // Advance index
    px_circ_buf->rd = px_circ_buf_next(px_circ_buf, px_circ_buf->rd);

    return true;
}

px_circ_buf_idx_t px_circ_buf_rd(px_circ_buf_t * px_circ_buf,
                                 void *          data,
                                 size_t          nr_of_bytes_to_rd)
{
    px_circ_buf_idx_t bytes_read = 0;
    uint8_t *         data_u8    = (uint8_t *)data;

    while(nr_of_bytes_to_rd)
    {
        // See if there is data in the buffer
        if (px_circ_buf->rd == px_circ_buf->wr)
        {
            // Buffer is empty
            break;
        }

        // Fetch data
        *data_u8++ = px_circ_buf->buf[px_circ_buf->rd];

        // Advance index
        px_circ_buf->rd = px_circ_buf_next(px_circ_buf, px_circ_buf->rd);

        // Next byte
        bytes_read++;
        nr_of_bytes_to_rd--;
    }

    return bytes_read;
}
