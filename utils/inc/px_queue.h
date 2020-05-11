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
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_queue.h : A FIFO data queue
    Author(s):      Pieter Conradie
    Creation Date:  2019-07-15

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_QUEUE px_queue.h : A FIFO item queue
 *  
 *  A FIFO (First In First Out) queue of fixed sized items using pointers.
 *  
 *  File(s):
 *  - utils/inc/px_queue.h
 *  - utils/src/px_queue.c
 *  
 *  @see https://en.wikipedia.org/wiki/Queue_(abstract_data_type)
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Queue structure
typedef struct
{
    void * item_first;          ///< Pointer to first item in the array
    void * item_last;           ///< Pointer to last item in the array
    void * item_wr;             ///< Pointer to position where next item will be written to
    void * item_rd;             ///< Pointer to oldest item in the queue
    size_t item_size;           ///< Size of each item in the array
    size_t nr_of_items;         ///< Counter for number of items in the queue
    size_t max_nr_of_items;     ///< Maximum number of items allowed in queue
} px_queue_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialises a queue structure.
 *  
 *  @param queue            Pointer to a queue object
 *  @param item_array       Pointer to an item array to buffer items
 *  @param item_size        Fixed size of each item
 *  @param max_nr_of_items  Maximum number of items that can be stored in array
 */
void px_queue_init(px_queue_t * queue,
                   void *       item_array,
                   size_t       item_size,
                   size_t       max_nr_of_items);

/** 
 *  See if the queue is empty.
 *   
 *  @param queue        Pointer to a queue object
 *   
 *  @return true        Queue is empty
 *  @return false       Queue contains one or more items
 */
bool px_queue_is_empty(px_queue_t * queue);

/** 
 *  See if the queue is full.
 *   
 *  @param queue        Pointer to a queue object
 *   
 *  @retval true        The queue is full
 *  @retval false       The queue has one or more empty slots
 */
bool px_queue_is_full(px_queue_t * queue);

/** 
 *  Get the number of items in the queue.
 *  
 *  @param queue        Pointer to a queue object
 *  
 *  @returns size_t     The number of items in the queue 
 */
size_t px_queue_nr_of_items(px_queue_t * queue);

/**
 *  Read the oldest item from the queue.
 *  
 *  @param queue    Pointer to a queue object.
 *  
 *  @returns        Pointer to item data; NULL if queue is empty
 */
void * px_queue_rd(px_queue_t * queue);

/**
 *  Write a new item to the queue.
 *  
 *  The item data is copied into the array.
 *  
 *  @param queue    Pointer to a queue object.
 *  @param item     Pointer to item data.
 *  
 *  @retval true    Item added to the queue
 *  @retval false   Queue is full and item was not added.
 */
bool px_queue_wr(px_queue_t * queue, const void * item);

/**
 *  Remove all items from queue.
 *  
 *  @param queue Pointer to queue object.
 */
void px_queue_flush(px_queue_t * queue);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_QUEUE_H__
