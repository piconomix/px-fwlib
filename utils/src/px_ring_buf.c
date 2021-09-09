/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_ring_buf.h : FIFO circular buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ring_buf.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static inline px_ring_buf_idx_t px_ring_buf_next(const px_ring_buf_t * px_ring_buf,
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
    // Initialise the circular buffer structure to be empty
    px_ring_buf->buf      = buf;
    px_ring_buf->buf_size = buf_size;
    px_ring_buf->wr       = 0;
    px_ring_buf->rd       = 0;
}

bool px_ring_buf_is_empty(const px_ring_buf_t * px_ring_buf)
{
    return (px_ring_buf->rd == px_ring_buf->wr);
}

bool px_ring_buf_is_full(const px_ring_buf_t * px_ring_buf)
{
    // Calculate next position
    px_ring_buf_idx_t next = px_ring_buf_next(px_ring_buf, px_ring_buf->wr);

    return(next == px_ring_buf->rd);
}

void px_ring_buf_flush(px_ring_buf_t * px_ring_buf)
{
    px_ring_buf->rd = px_ring_buf->wr;
}

bool px_ring_buf_wr_u8(px_ring_buf_t * px_ring_buf, 
                       const uint8_t   data)
{
    // Calculate next position
    px_ring_buf_idx_t next = px_ring_buf_next(px_ring_buf, px_ring_buf->wr);
    // Buffer full?
    if (next == px_ring_buf->rd)
    {
        // Yes. Discard byte
        return false;
    }
    // Add data to buffer
    px_ring_buf->buf[px_ring_buf->wr] = data;
    // Advance index
    px_ring_buf->wr = next;

    return true;
}

px_ring_buf_idx_t px_ring_buf_wr(px_ring_buf_t * px_ring_buf, 
                                 const void *    data, 
                                 size_t          nr_of_bytes_to_wr)
{
    px_ring_buf_idx_t next;
    px_ring_buf_idx_t bytes_written = 0;
    uint8_t *         data_u8       = (uint8_t *)data;

    while(nr_of_bytes_to_wr)
    {
        // Calculate next position
        next = px_ring_buf_next(px_ring_buf, px_ring_buf->wr);
        // Buffer full?
        if (next == px_ring_buf->rd)
        {
            // Yes. Discard rest of data
            break;
        }
        // Add data to buffer
        px_ring_buf->buf[px_ring_buf->wr] = *data_u8++;
        // Advance index
        px_ring_buf->wr = next;
        // Next byte
        bytes_written++;
        nr_of_bytes_to_wr--;
    }

    return bytes_written;
}

bool px_ring_buf_rd_u8(px_ring_buf_t * px_ring_buf,
                       uint8_t       * data)
{
    // See if there is data in the buffer
    if(px_ring_buf->rd == px_ring_buf->wr)
    {
        // Buffer is empty
        return false;
    }
    // Fetch data
    *data = px_ring_buf->buf[px_ring_buf->rd];
    // Advance index
    px_ring_buf->rd = px_ring_buf_next(px_ring_buf, px_ring_buf->rd);

    return true;
}

px_ring_buf_idx_t px_ring_buf_rd(px_ring_buf_t * px_ring_buf,
                                 void *          data,
                                 size_t          nr_of_bytes_to_rd)
{
    px_ring_buf_idx_t bytes_read = 0;
    uint8_t *         data_u8    = (uint8_t *)data;

    while(nr_of_bytes_to_rd)
    {
        // See if there is data in the buffer
        if (px_ring_buf->rd == px_ring_buf->wr)
        {
            // Buffer is empty
            break;
        }
        // Fetch data
        *data_u8++ = px_ring_buf->buf[px_ring_buf->rd];
        // Advance index
        px_ring_buf->rd = px_ring_buf_next(px_ring_buf, px_ring_buf->rd);
        // Next byte
        bytes_read++;
        nr_of_bytes_to_rd--;
    }

    return bytes_read;
}

px_ring_buf_idx_t px_ring_buf_peek(px_ring_buf_t * px_ring_buf, 
                                 void *          data,
                                 size_t          nr_of_bytes_to_peek,
                                 size_t          nr_of_bytes_to_skip)
{
    px_ring_buf_idx_t bytes_read     = 0;
    px_ring_buf_idx_t current_rd_idx = px_ring_buf->rd;
    uint8_t *         data_u8        = (uint8_t *)data;
    px_ring_buf_idx_t full           = px_ring_buf_full(px_ring_buf);
    
    if (nr_of_bytes_to_skip >= full)
    {
        return 0;
    }

    while (nr_of_bytes_to_skip)
    {
        current_rd_idx = px_ring_buf_next(px_ring_buf, current_rd_idx);
        nr_of_bytes_to_skip--;
    }

    while(nr_of_bytes_to_peek)
    {
        // See if there is data in the buffer
        if (current_rd_idx == px_ring_buf->wr)
        {
            // Buffer is empty
            break;
        }
        // Fetch data
        *data_u8++ = px_ring_buf->buf[current_rd_idx];
        // Advance index
        current_rd_idx = px_ring_buf_next(px_ring_buf, current_rd_idx);
        // Next byte
        bytes_read++;
        nr_of_bytes_to_peek--;
    }

    return bytes_read;
}

px_ring_buf_idx_t px_ring_buf_free(px_ring_buf_t * px_ring_buf)
{
    px_ring_buf_idx_t size;
    
    px_ring_buf_idx_t wr = px_ring_buf->wr;
    px_ring_buf_idx_t rd = px_ring_buf->rd;

    if (wr == rd)
    {
        size = px_ring_buf->buf_size;
    }
    else if (rd > wr)
    {
        size = rd - wr;
    }
    else {
        size = px_ring_buf->buf_size - (wr - rd);
    }

    // Free size is always 1 less than actual size for this implementation
    return size - 1;
}

px_ring_buf_idx_t px_ring_buf_full(px_ring_buf_t * px_ring_buf)
{
    px_ring_buf_idx_t size;
    
    px_ring_buf_idx_t wr = px_ring_buf->wr;
    px_ring_buf_idx_t rd = px_ring_buf->rd;

    if (wr == rd)
    {
        size = 0;
    }
    else if (wr > rd)
    {
        size = wr - rd;
    }
    else
    {
        size = px_ring_buf->buf_size - (rd - wr);
    }

    return size;
}
