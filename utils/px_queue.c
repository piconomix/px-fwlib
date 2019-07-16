/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_queue.h : A FIFO data queue
    Author(s):      Pieter Conradie
    Creation Date:  2019-07-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_queue.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_queue");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void * px_queue_next(px_queue_t * queue, void * item)
{
    // End of array
    if(item == queue->item_last)
    {
        // Wrap pointer to start of array
        item = queue->item_first;
    }
    else
    {
        // Advance pointer
        item += queue->item_size;
    }
    return item;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_queue_init(px_queue_t * queue,
                   void *       item_array,
                   size_t       item_size,
                   size_t       max_nr_of_items)
{
    // Sanity checks
    PX_DBG_ASSERT(queue           != NULL);
    PX_DBG_ASSERT(item_array      != NULL);
    PX_DBG_ASSERT(item_size       != 0);
    PX_DBG_ASSERT(max_nr_of_items != 0);
    // Initialise queue
    queue->item_first      = item_array;
    queue->item_last       = item_array + item_size * (max_nr_of_items - 1);
    queue->item_wr         = queue->item_first;
    queue->item_rd         = queue->item_first;
    queue->item_size       = item_size;
    queue->nr_of_items     = 0;
    queue->max_nr_of_items = max_nr_of_items;
}

bool px_queue_is_empty(px_queue_t * queue)
{
    PX_DBG_ASSERT(queue != NULL);

    if(queue->nr_of_items == 0)
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
    PX_DBG_ASSERT(queue != NULL);

    if(queue->nr_of_items == queue->max_nr_of_items)
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t px_queue_nr_of_items(px_queue_t * queue)
{
    PX_DBG_ASSERT(queue != NULL);

    // Return item count
    return (queue->nr_of_items);
}

void * px_queue_rd(px_queue_t * queue)
{
    void * item;

    PX_DBG_ASSERT(queue != NULL);

    // Queue empty?
    if(px_queue_is_empty(queue))
    {
        return NULL;
    }
    // Save pointer to item data
    item = queue->item_rd;
    // Decrement item count
    queue->nr_of_items--;
    // Next pointer (wrap if going past end)
    queue->item_rd = px_queue_next(queue, queue->item_rd);
    // Return pointer to item data
    return item;
}

bool px_queue_wr(px_queue_t * queue, const void * item)
{
    PX_DBG_ASSERT(queue != NULL);
    PX_DBG_ASSERT(item  != NULL);

    // Queue full?
    if(px_queue_is_full(queue))
    {
        return false;
    }
    // Copy item data into array
    memcpy(queue->item_wr, item, queue->item_size);
    // Increment item count
    queue->nr_of_items++;
    // Next pointer (wrap if going past end)
    queue->item_wr = px_queue_next(queue, queue->item_wr);

    return true;
}

void px_queue_flush(px_queue_t * queue)
{
    queue->item_wr         = queue->item_first;
    queue->item_rd         = queue->item_first;
    queue->nr_of_items     = 0;
}

