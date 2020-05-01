/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_link_list.h : Link List
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-27

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_link_list.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_link_list_init(px_link_list_t * list,
                       size_t           max_nr_of_items)
{
    list->first_item      = NULL;
    list->last_item       = NULL;
    list->nr_of_items     = 0;
    list->max_nr_of_items = max_nr_of_items;
}

void px_link_list_item_init(px_link_list_t *      list,
                            px_link_list_item_t * item)
{
    item->previous_item = NULL;
    item->next_item     = NULL;
}

bool px_link_list_is_empty(px_link_list_t * list)
{
    if(list->first_item == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_link_list_is_full(px_link_list_t * list)
{
    if(list->max_nr_of_items == 0)
    {
        return false;
    }

    if(list->nr_of_items < list->max_nr_of_items)
    {
        return false;
    }
    else
    {
        return true;
    }
}

size_t px_link_list_nr_of_items(px_link_list_t * list)
{
    // Return item count
    return (list->nr_of_items);
}

px_link_list_item_t * px_link_list_first_item(px_link_list_t * list)
{
    return list->first_item;
}

px_link_list_item_t * px_link_list_last_item(px_link_list_t * list)
{
    return list->last_item;
}

px_link_list_item_t * px_link_list_next_item(px_link_list_t *      list,
                                             px_link_list_item_t * item)
{
    return item->next_item;
}

px_link_list_item_t * px_link_list_previous_item(px_link_list_t *      list,
                                                 px_link_list_item_t * item)
{
    return item->previous_item;
}

bool px_link_list_add_to_start(px_link_list_t *      list,
                               px_link_list_item_t * item)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }

    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first_item    = item;
        list->last_item     = item;
        item->next_item     = NULL;
        item->previous_item = NULL;
    }
    else
    {
        // Insert new item before first item
        item->previous_item             = NULL;
        item->next_item                 = list->first_item;
        list->first_item->previous_item = item;
        list->first_item                = item;
    }

    // Increment item count
    (list->nr_of_items)++;

    return true;
}

bool px_link_list_add_to_end(px_link_list_t *      list,
                             px_link_list_item_t * item)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }

    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first_item    = item;
        list->last_item     = item;
        item->next_item     = NULL;
        item->previous_item = NULL;
    }
    else
    {
        // Append new item to last item
        item->previous_item        = list->last_item;
        item->next_item            = NULL;
        list->last_item->next_item = item;
        list->last_item            = item;
    }

    // Increment item count
    (list->nr_of_items)++;

    return true;
}

px_link_list_item_t * px_link_list_remove_first_item(px_link_list_t * list)
{
    px_link_list_item_t * item = list->first_item;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }

    // See if there is only one item
    if(list->first_item == list->last_item)
    {
        list->first_item = NULL;
        list->last_item  = NULL;
    }
    else
    {
        // The next item become the first one in the list
        list->first_item               = item->next_item;
        item->next_item->previous_item = NULL;
    }

    // Clear links of removed item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;

    return item;
}

px_link_list_item_t * px_link_list_remove_last_item(px_link_list_t * list)
{
    px_link_list_item_t * item = list->last_item;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }

    // See if there is only one item
    if(list->first_item == list->last_item)
    {
        list->first_item = NULL;
        list->last_item  = NULL;
    }
    else
    {
        // The previous item become the last one in the list
        list->last_item                = item->previous_item;
        item->previous_item->next_item = NULL;        
    }

    // Clear links of removed item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;

    return item;
}

void px_link_list_remove_item(px_link_list_t *      list,
                              px_link_list_item_t * item)
{
    // Extra sanity check
    if(px_link_list_is_empty(list))
    {
        return;
    }

    // See if this is the first item in the list
    if(item == list->first_item)
    {
        px_link_list_remove_first_item(list);
        return;
    }

    // See if this is the last item in the list
    if(item == list->last_item)
    {
        px_link_list_remove_last_item(list);
        return;
    }

    // Link previous and next item to each other
    item->previous_item->next_item = item->next_item;
    item->next_item->previous_item = item->previous_item;
    
    // Clear links of item
    item->previous_item = NULL;
    item->next_item     = NULL;

    // Decrement item count
    (list->nr_of_items)--;
}

bool px_link_list_item_in_list(px_link_list_t *      list,
                               px_link_list_item_t * item)
{
    // Start at first item in the list
    px_link_list_item_t * item_in_list = list->first_item;

    // Search all items in the list
    while(item_in_list != NULL)
    {
        if(item_in_list == item)
        {
            // Item is in the list
            return true;
        }
        // Next item
        item_in_list = item_in_list->next_item;
    }
    // Item is not in the list
    return false;
}

