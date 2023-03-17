/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_ring_buf.h : FIFO ring buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ring_buf.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_ring_buf");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static inline px_ring_buf_idx_t px_ring_buf_idx_next(const px_ring_buf_t * px_ring_buf,
                                                     px_ring_buf_idx_t     idx)
{
    if(++idx >= px_ring_buf->buf_size)
    {
        // Wrap index to start of buffer
        idx = 0;
    }
    return idx;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_ring_buf_init(px_ring_buf_t *   px_ring_buf,
                      uint8_t *         buf,
                      px_ring_buf_idx_t buf_size)
{
    // Initialise the ring buffer structure to be empty
    px_ring_buf->buf      = buf;
    px_ring_buf->buf_size = buf_size;
    px_ring_buf->idx_wr   = 0;
    px_ring_buf->idx_rd   = 0;
}

bool px_ring_buf_is_empty(const px_ring_buf_t * px_ring_buf);

bool px_ring_buf_is_full(const px_ring_buf_t * px_ring_buf)
{
    return (px_ring_buf_idx_next(px_ring_buf, px_ring_buf->idx_wr) == px_ring_buf->idx_rd);
}

void px_ring_buf_flush(px_ring_buf_t * px_ring_buf, size_t nr_of_bytes)
{
    px_ring_buf_idx_t idx_rd;

    // Flush everything?
    if(nr_of_bytes == 0)
    {
        // Yes
        px_ring_buf->idx_rd = px_ring_buf->idx_wr;
        return;
    }

    // Try to flush specified number of bytes
    idx_rd = px_ring_buf->idx_rd;
    while(nr_of_bytes != 0)
    {
        // Buffer empty?
        if(idx_rd == px_ring_buf->idx_wr)
        {
            // Yes
            break;
        }
        // Advance index
        idx_rd = px_ring_buf_idx_next(px_ring_buf, idx_rd);
        // Next byte
        nr_of_bytes--;
    }

    // Update read index
    px_ring_buf->idx_rd = idx_rd;
}

bool px_ring_buf_wr_u8(px_ring_buf_t * px_ring_buf, 
                       const uint8_t   data)
{
    px_ring_buf_idx_t idx_wr = px_ring_buf->idx_wr;

    // Buffer full?
    if(px_ring_buf_idx_next(px_ring_buf, idx_wr) == px_ring_buf->idx_rd)
    {
        // Yes. Discard byte
        return false;
    }

    // Add data to buffer
    px_ring_buf->buf[idx_wr] = data;
    // Advance index
    idx_wr = px_ring_buf_idx_next(px_ring_buf, idx_wr);
    // Update write index
    px_ring_buf->idx_wr = idx_wr;

    return true;
}

px_ring_buf_idx_t px_ring_buf_wr(px_ring_buf_t * px_ring_buf, 
                                 const void *    data, 
                                 size_t          nr_of_bytes)
{
    px_ring_buf_idx_t idx_wr        = px_ring_buf->idx_wr;
    px_ring_buf_idx_t bytes_written = 0;
    uint8_t *         data_u8       = (uint8_t *)data;

    while(nr_of_bytes != 0)
    {
        // Buffer full?
        if(px_ring_buf_idx_next(px_ring_buf, idx_wr) == px_ring_buf->idx_rd)
        {
            // Yes. Discard rest of data
            break;
        }
        // Add data to buffer
        px_ring_buf->buf[idx_wr] = *data_u8++;
        // Advance index
        idx_wr = px_ring_buf_idx_next(px_ring_buf, idx_wr);
        // Next byte
        bytes_written++;
        nr_of_bytes--;
    }

    // Update write index
    px_ring_buf->idx_wr = idx_wr;

    return bytes_written;
}

bool px_ring_buf_rd_u8(px_ring_buf_t * px_ring_buf,
                       uint8_t       * data)
{
    px_ring_buf_idx_t idx_rd = px_ring_buf->idx_rd;

    // Buffer empty?
    if(idx_rd == px_ring_buf->idx_wr)
    {
        // Yes
        return false;
    }
    // Fetch data
    *data = px_ring_buf->buf[idx_rd];
    // Advance index
    idx_rd = px_ring_buf_idx_next(px_ring_buf, idx_rd);
    // Update read index
    px_ring_buf->idx_rd = idx_rd;

    return true;
}

px_ring_buf_idx_t px_ring_buf_rd(px_ring_buf_t * px_ring_buf,
                                 void *          data,
                                 size_t          nr_of_bytes)
{
    px_ring_buf_idx_t idx_rd     = px_ring_buf->idx_rd;
    px_ring_buf_idx_t bytes_read = 0;
    uint8_t *         data_u8    = (uint8_t *)data;

    while(nr_of_bytes != 0)
    {
        // Buffer empty?
        if(idx_rd == px_ring_buf->idx_wr)
        {
            // Yes
            break;
        }
        // Fetch data
        *data_u8++ = px_ring_buf->buf[idx_rd];
        // Advance index
        idx_rd = px_ring_buf_idx_next(px_ring_buf, idx_rd);
        // Next byte
        bytes_read++;
        nr_of_bytes--;
    }

    // Update read index
    px_ring_buf->idx_rd = idx_rd;

    return bytes_read;
}

px_ring_buf_idx_t px_ring_buf_peek(px_ring_buf_t * px_ring_buf, 
                                   void *          data,
                                   size_t          nr_of_bytes)
{
    px_ring_buf_idx_t idx_peek   = px_ring_buf->idx_rd;
    px_ring_buf_idx_t bytes_read = 0;
    uint8_t *         data_u8    = (uint8_t *)data;

    while(nr_of_bytes != 0)
    {
        // Buffer empty?
        if(idx_peek == px_ring_buf->idx_wr)
        {
            // Yes
            break;
        }
        // Fetch data
        *data_u8++ = px_ring_buf->buf[idx_peek];
        // Advance index
        idx_peek = px_ring_buf_idx_next(px_ring_buf, idx_peek);
        // Next byte
        bytes_read++;
        nr_of_bytes--;
    }

    return bytes_read;
}

px_ring_buf_idx_t px_ring_buf_count_used(px_ring_buf_t * px_ring_buf)
{
    px_ring_buf_idx_t count_used;
    px_ring_buf_idx_t idx_wr = px_ring_buf->idx_wr;
    px_ring_buf_idx_t idx_rd = px_ring_buf->idx_rd;

    // Buffer empty?
    if(idx_rd == idx_wr)
    {
        count_used = 0;
    }
    // |________xxxxxxxxx___________|
    //          R>>>>>>>>W
    else if(idx_rd < idx_wr)
    {
        count_used = idx_wr - idx_rd;
    }
    // |xxxxxx___________________xxx|
    //  >>>>>>W                  R>>
    else
    {
        count_used = px_ring_buf->buf_size - idx_rd + idx_wr;
    }

    return count_used;
}

px_ring_buf_idx_t px_ring_buf_count_free(px_ring_buf_t * px_ring_buf)
{
    px_ring_buf_idx_t count_free;
    px_ring_buf_idx_t idx_wr = px_ring_buf->idx_wr;
    px_ring_buf_idx_t idx_rd = px_ring_buf->idx_rd;

    // Buffer empty?
    if(idx_rd == idx_wr)
    {
        count_free = px_ring_buf->buf_size - 1;
    }
    // |________xxxxxxxxx___________|
    //          R>>>>>>>>W
    else if(idx_rd < idx_wr)
    {
        count_free = px_ring_buf->buf_size - 1 - idx_wr + idx_rd;
    }
    // |xxxxxx___________________xxx|
    //  >>>>>>W                  R>>
    else
    {
        count_free = idx_rd - idx_wr - 1;
    }

    return count_free;
}

void px_ring_buf_log_report(px_ring_buf_t * px_ring_buf)
{
    PX_LOG_TRACE("WR=%u\n",   px_ring_buf->idx_wr);
    PX_LOG_TRACE("RD=%u\n",   px_ring_buf->idx_rd);
    PX_LOG_TRACE("Size=%u\n", px_ring_buf->buf_size);
    PX_LOG_TRACE("Used=%u\n", px_ring_buf_count_used(px_ring_buf));
    PX_LOG_TRACE("Free=%u\n", px_ring_buf_count_free(px_ring_buf));
}

