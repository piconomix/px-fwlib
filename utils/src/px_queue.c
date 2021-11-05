/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_queue.h : A FIFO data queue
    Author(s):      Pieter Conradie
    Creation Date:  2019-07-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_queue.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_queue");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static inline px_queue_idx_t px_queue_idx_next(const px_queue_t * queue, px_queue_idx_t idx)
{
    // End of array?
    if(++idx == queue->items_max)
    {
        // Wrap to start of array
        idx = 0;
    }
    return idx;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_queue_init(px_queue_t *         queue,
                   void *               item_buf,
                   px_queue_idx_t       items_max,
                   px_queue_item_size_t item_size)
{
    // Sanity checks
    PX_LOG_ASSERT(queue     != NULL);
    PX_LOG_ASSERT(item_buf  != NULL);
    PX_LOG_ASSERT(item_size != 0);
    PX_LOG_ASSERT(items_max != 0);
    // Initialise queue
    queue->item_buf   = (uint8_t *)item_buf;
    queue->idx_rd     = 0;
    queue->idx_wr     = 0;
    queue->item_count = 0;
    queue->items_max  = items_max;
    queue->item_size  = item_size;
}

bool px_queue_is_empty(px_queue_t * queue)
{
    PX_LOG_ASSERT(queue != NULL);

    if(queue->item_count == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_queue_is_full(px_queue_t * queue)
{
    PX_LOG_ASSERT(queue != NULL);

    if(queue->item_count == queue->items_max)
    {
        return true;
    }
    else
    {
        return false;
    }
}

px_queue_idx_t px_queue_get_item_count(px_queue_t * queue)
{
    PX_LOG_ASSERT(queue != NULL);

    // Return item count
    return (queue->item_count);
}

bool px_queue_wr(px_queue_t * queue, const void * item_data)
{
    PX_LOG_ASSERT(queue     != NULL);
    PX_LOG_ASSERT(item_data != NULL);

    // Queue full?
    if(px_queue_is_full(queue))
    {
        return false;
    }
    // Copy item data into array
    memcpy(queue->item_buf + queue->item_size * queue->idx_wr, item_data, queue->item_size);
    // Increment item count
    queue->item_count++;
    // Next index (wrap if going past end)
    queue->idx_wr = px_queue_idx_next(queue, queue->idx_wr);
    // Success
    return true;
}

bool px_queue_rd(px_queue_t * queue, void * item_data)
{
    PX_LOG_ASSERT(queue     != NULL);
    PX_LOG_ASSERT(item_data != NULL);

    // Queue empty?
    if(px_queue_is_empty(queue))
    {
        return NULL;
    }
    // Copy item data
    memcpy(item_data, queue->item_buf + queue->item_size * queue->idx_rd, queue->item_size);
    // Decrement item count
    queue->item_count--;
    // Next index (wrap if going past end)
    queue->idx_rd = px_queue_idx_next(queue, queue->idx_rd);
    // Success
    return true;
}

bool px_queue_discard_oldest(px_queue_t * queue)
{
    PX_LOG_ASSERT(queue != NULL);

    // Queue empty?
    if(px_queue_is_empty(queue))
    {
        return NULL;
    }
    // Decrement item count
    queue->item_count--;
    // Next pointer (wrap if going past end)
    queue->idx_rd = px_queue_next(queue, queue->idx_rd);
    // Success
    return true;
}

void px_queue_flush(px_queue_t * queue)
{
    PX_LOG_ASSERT(queue != NULL);

    queue->idx_rd     = 0;
    queue->idx_wr     = 0;
    queue->item_count = 0;
}

