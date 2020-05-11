#ifndef __PX_LINK_LIST_H__
#define __PX_LINK_LIST_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_link_list.h : Linked List
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-27

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_LINK_LIST px_link_list.h : Linked List
 *  
 *  A double (forward and backward) linked list using pointers.
 *  
 *  File(s):
 *  - utils/inc/px_link_list.h
 *  - utils/src/px_link_list.c
 *  
 *  @see http://en.wikipedia.org/wiki/Linked_list
 *  
 *  Example:
 *  
 *      @code
 *      No items in list:
 *  
 *      [first = NULL] [last = NULL]
 *  
 *      One item in list:
 *  
 *      [first] -> [prev = NULL][next = NULL] <- [last]
 *  
 *      Two items in list:
 *  
 *      [first] -> [prev = NULL][next] <-> [prev][next = NULL]  <- [last]
 *  
 *      Three items in list:
 *  
 *      [first] -> [prev = NULL][next] <-> [prev][next] <-> [prev][next = NULL]  <- [last]
 *      @endcode
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
/// Link structure that must be at the head of each item in the list
typedef struct px_link_list_item_s
{
    struct px_link_list_item_s * next_item;
    struct px_link_list_item_s * previous_item;
} px_link_list_item_t;

/// Linked list structure
typedef struct
{
    struct px_link_list_item_s * first_item;        ///< Pointer to first item in the list
    struct px_link_list_item_s * last_item;         ///< Pointer to last item in the list
    size_t                       nr_of_items;       ///< Counter for number of items in the list
    size_t                       max_nr_of_items;   ///< Maximum number of items allowed in list; 0 means no limit
} px_link_list_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialises a linked list structure
 *   
 *  @param list              Pointer to the linked list
 *  @param max_nr_of_items   Maximum number of items allowed in list; 0 means no limit
 */
void px_link_list_init(px_link_list_t * list,
                       size_t           max_nr_of_items);

/** 
 *  Initialises a list item.
 *   
 *  Initialises the item structure to indicate that it is not in the list.
 *  @sa px_link_list_item_in_list()
 *   
 *  @param list     Pointer to the linked list
 *  @param item     Pointer to specified item
 */
void px_link_list_item_init(px_link_list_t      * list,
                            px_link_list_item_t * item);

/** 
 *  See if the list is empty
 *   
 *  @param list     Pointer to the linked list
 *   
 *  @return true    List is empty 
 *  @return false   List contains one or more items
 */
bool px_link_list_is_empty(px_link_list_t * list);

/** 
 *  See if the list is full
 *   
 *  @param list     Pointer to the linked list
 *   
 *  @retval true    The list is full
 *  @retval false   The list is not full, or there is no limit (max_nr_of_items = 0)
 */
bool px_link_list_is_full(px_link_list_t * list);

/** 
 *  Get the number of items in the list
 *  
 *  @param list     Pointer to the linked list
 *  
 *  @returns size_t The number of items in the list 
 */
size_t px_link_list_nr_of_items(px_link_list_t * list);

/** 
 *  Get a pointer to the first item in the list
 *   
 *  @param list                     Pointer to the linked list
 *   
 *  @returns px_link_list_item_t *  Pointer to the first item in the list;
 *                                  NULL will be returned if the list is empty.
 */
px_link_list_item_t * px_link_list_first_item(px_link_list_t * list);

/** 
 *  Get a pointer to the last item in the list
 *   
 *  @param list                     Pointer to the linked list
 *   
 *  @returns px_link_list_item_t *  Pointer to the last item in the list;
 *                                  NULL will be returned if the list is empty.
 */
px_link_list_item_t * px_link_list_last_item(px_link_list_t * list);

/** 
 *  Get a pointer to the next item in the list (after the specified item).
 *   
 *  @param list                     Pointer to the linked list 
 *  @param item                     Current item
 *   
 *  @returns px_link_list_item_t *  Pointer to the next item in the list;
 *                                  NULL will be returned if the specified item
 *                                  is the last one in the list.
 */
px_link_list_item_t * px_link_list_next_item(px_link_list_t *      list,
                                             px_link_list_item_t * item);

/** 
 *  Get a pointer to the previous item in the list (before the specified item).
 *   
 *  @param list                     Pointer to the linked list 
 *  @param item                     Current item 
 *   
 *  @returns px_link_list_item_t *  Pointer to the next item in the list;
 *                                  NULL will be returned if the specified item
 *                                  is the first one in the list.
 */
px_link_list_item_t * px_link_list_previous_item(px_link_list_t *      list,
                                                 px_link_list_item_t * item);

/** 
 *  Insert item to the start of the list.
 *   
 *  @param list      Pointer to the linked list
 *  @param item      Item to be inserted
 *   
 *  @retval true     Item has been inserted
 *  @retval false    List is full
 */
bool px_link_list_add_to_start(px_link_list_t *      list,
                               px_link_list_item_t * item);

/** 
 *  Add item to the end of the list.
 *   
 *  @param list     Pointer to the linked list
 *  @param item     Item to be inserted
 *   
 *  @retval true    Item has been inserted
 *  @retval false   List is full
 */
bool px_link_list_add_to_end(px_link_list_t *      list,
                             px_link_list_item_t * item);

/** 
 *  Remove first item from the list
 *   
 *  @param list                     Pointer to the linked list
 *   
 *  @return px_link_list_item_t *   Pointer to the (old) first item;
 *                                  NULL will be returned if the list is empty.
 */
px_link_list_item_t * px_link_list_remove_first_item(px_link_list_t * list);

/** 
 *  Remove last item from the list
 *   
 *  @param list                     Pointer to the linked list
 *   
 *  @return px_link_list_item_t *   Pointer to the (old) last item;
 *                                  NULL will be returned if the list is empty.
 */
px_link_list_item_t * px_link_list_remove_last_item(px_link_list_t * list);

/** 
 *  Remove item from the list
 *   
 *  @param list      Pointer to the linked list
 *  @param item      Item to be removed from the list
 *   
 */
void px_link_list_remove_item(px_link_list_t *      list,
                              px_link_list_item_t * item);

/** 
 *  See if item is in the list
 *   
 *  @param list     Pointer to the linked list
 *  @param item     Pointer to specified item
 *   
 *  @retval true    Item is in the list
 *  @retval false   Item is not in the list
 */
bool px_link_list_item_in_list(px_link_list_t *      list,
                               px_link_list_item_t * item);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_LINK_LIST_H__
