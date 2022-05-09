#ifndef __PX_QUEUE_H__
#define __PX_QUEUE_H__
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

/** 
 *  @ingroup UTILS
 *  @defgroup PX_QUEUE px_queue.h : A FIFO item queue
 *  
 *  A FIFO (First In First Out) queue of fixed sized items using zero based indexes.
 *  
 *  File(s):
 *  - utils/inc/px_queue.h
 *  - utils/src/px_queue.c
 *  
 *  @see https://en.wikipedia.org/wiki/Queue_(abstract_data_type)
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Queue index size definition
typedef uint8_t px_queue_idx_t;

/// Queue item size definition
typedef uint8_t px_queue_item_size_t;

/// Queue structure
typedef struct
{
    uint8_t *            buf;           ///< Buffer to hold item data
    px_queue_idx_t       idx_rd;        ///< Read index
    px_queue_idx_t       idx_wr;        ///< Write index
    px_queue_idx_t       item_count;    ///< Number of items stored in queue
    px_queue_idx_t       items_max;     ///< Maximum number of items that can be stored in queue
    px_queue_item_size_t item_size;     ///< Size of each item
} px_queue_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialises a queue structure.
 *  
 *  @param queue        Pointer to a queue object
 *  @param buf          Buffer to hold items (must be at least item_size * items_max bytes)
 *  @param item_size    Fixed size of each item
 *  @param items_max    Maximum number of items that can be stored in array
 */
void px_queue_init(px_queue_t *         queue,
                   void *               buf,
                   px_queue_idx_t       items_max,
                   px_queue_item_size_t item_size);

/** 
 *  See if the queue is empty.
 *   
 *  @param queue        Pointer to a queue object
 *   
 *  @return true        Queue is empty
 *  @return false       Queue contains one or more items
 */
inline bool px_queue_is_empty(px_queue_t * queue)
{
    return (queue->item_count == 0);
}

/** 
 *  See if the queue is full.
 *   
 *  @param queue        Pointer to a queue object
 *   
 *  @retval true        The queue is full
 *  @retval false       The queue has one or more empty slots
 */
inline bool px_queue_is_full(px_queue_t * queue)
{
    return (queue->item_count == queue->items_max);
}

/** 
 *  Get the number of items in the queue.
 *  
 *  @param queue        Pointer to a queue object
 *  
 *  @returns px_queue_idx_t The number of items in the queue
 */
inline px_queue_idx_t px_queue_get_item_count(px_queue_t * queue)
{
    return (queue->item_count);
}

/**
 *  Write a new item to the queue.
 *  
 *  The item data is copied into the array.
 *  
 *  @param queue        Pointer to a queue object.
 *  @param item_data    Pointer to item data.
 *  
 *  @retval true        Item added to the queue
 *  @retval false       Queue is full and item was not added
 */
bool px_queue_wr(px_queue_t * queue, const void * item_data);

/**
 *  Read the oldest item from the queue.
 *
 *  The item data is copied into the specified buffer
 *
 *  @param queue        Pointer to a queue object.
 *  @param item_data    Pointer to item data buffer.
 *
 *  @retval true        Item copied and removed from queue
 *  @retval false       Queue is empty
 */
bool px_queue_rd(px_queue_t * queue, void * item_data);

/**
 *  Remove oldest item from queue.
 *
 *  @param queue        Pointer to a queue object.
 *
 *  @retval true        Oldest item removed
 *  @retval false       Queue is empty
 */
bool px_queue_discard_oldest(px_queue_t * queue);

/**
 *  Remove all items from queue.
 *  
 *  @param queue        Pointer to queue object.
 */
void px_queue_flush(px_queue_t * queue);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
