/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
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
    list->first      = NULL;
    list->last       = NULL;
    list->item_count = 0;
    list->items_max  = max_nr_of_items;
}

void px_link_list_item_init(px_link_list_t *      list,
                            px_link_list_item_t * item)
{
    item->prev = NULL;
    item->next = NULL;
}

bool px_link_list_is_empty(px_link_list_t * list)
{
    if(list->first == NULL)
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
    if(list->items_max == 0)
    {
        return false;
    }

    if(list->item_count < list->items_max)
    {
        return false;
    }
    else
    {
        return true;
    }
}

size_t px_link_list_get_item_count(px_link_list_t * list)
{
    return list->item_count;
}

px_link_list_item_t * px_link_list_get_item_first(px_link_list_t * list)
{
    return list->first;
}

px_link_list_item_t * px_link_list_get_item_last(px_link_list_t * list)
{
    return list->last;
}

px_link_list_item_t * px_link_list_get_item_next(px_link_list_t *      list,
                                                 px_link_list_item_t * item)
{
    return item->next;
}

px_link_list_item_t * px_link_list_get_item_prev(px_link_list_t *      list,
                                                 px_link_list_item_t * item)
{
    return item->prev;
}

bool px_link_list_insert_item_start(px_link_list_t *      list,
                                    px_link_list_item_t * item)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first = item;
        list->last  = item;
        item->next  = NULL;
        item->prev  = NULL;
    }
    else
    {
        // Insert new item before first item
        item->prev        = NULL;
        item->next        = list->first;
        list->first->prev = item;
        list->first       = item;
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_end(px_link_list_t *      list,
                                  px_link_list_item_t * item)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first = item;
        list->last  = item;
        item->next  = NULL;
        item->prev  = NULL;
    }
    else
    {
        // Append new item to last item
        item->prev       = list->last;
        item->next       = NULL;
        list->last->next = item;
        list->last       = item;
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_before(px_link_list_t *      list,
                                     px_link_list_item_t * item,
                                     px_link_list_item_t * item_pos)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first = item;
        list->last  = item;
        item->next  = NULL;
        item->prev  = NULL;
    }
    else
    {
        // Is this the first item in the list
        if(list->first == item_pos)
        {
            // Yes. Insert first
            list->first    = item;
            item->prev     = NULL;
            item->next     = item_pos;
            item_pos->prev = item;
        }
        else
        {
            // No. Insert before
            item->prev           = item_pos->prev;
            item->next           = item_pos;
            item_pos->prev->next = item;
            item_pos->prev       = item;
        }
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_after(px_link_list_t *      list,
                                    px_link_list_item_t * item,
                                    px_link_list_item_t * item_pos)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->first = item;
        list->last  = item;
        item->next  = NULL;
        item->prev  = NULL;
    }
    else
    {
        // Is this the last item in the list
        if(list->last == item_pos)
        {
            // Yes. Insert last
            list->last     = item;
            item->prev     = item_pos;
            item->next     = NULL;
            item_pos->next = item;
        }
        else
        {
            // No. Insert after
            item->prev           = item_pos;
            item->next           = item_pos->next;
            item_pos->next->prev = item;
            item_pos->next       = item;
        }
    }
    // Increment item count
    list->item_count++;

    return true;
}

px_link_list_item_t * px_link_list_remove_item_first(px_link_list_t * list)
{
    px_link_list_item_t * item = list->first;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }
    // See if there is only one item
    if(list->first == list->last)
    {
        list->first = NULL;
        list->last  = NULL;
    }
    else
    {
        // The next item become the first one in the list
        list->first      = item->next;
        item->next->prev = NULL;
    }
    // Clear links of removed item
    item->prev = NULL;
    item->next = NULL;
    // Decrement item count
    list->item_count--;

    return item;
}

px_link_list_item_t * px_link_list_remove_item_last(px_link_list_t * list)
{
    px_link_list_item_t * item = list->last;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }
    // See if there is only one item
    if(list->first == list->last)
    {
        list->first = NULL;
        list->last  = NULL;
    }
    else
    {
        // The previous item become the last one in the list
        list->last            = item->prev;
        item->prev->next = NULL;
    }
    // Clear links of removed item
    item->prev = NULL;
    item->next = NULL;
    // Decrement item count
    list->item_count--;

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
    if(item == list->first)
    {
        px_link_list_remove_item_first(list);
        return;
    }

    // See if this is the last item in the list
    if(item == list->last)
    {
        px_link_list_remove_item_last(list);
        return;
    }
    // Link previous and next item to each other
    item->prev->next = item->next;
    item->next->prev = item->prev;
    // Clear links of item
    item->prev = NULL;
    item->next = NULL;
    // Decrement item count
    list->item_count--;
}

bool px_link_list_has_item(px_link_list_t *      list,
                           px_link_list_item_t * item)
{
    // Start at first item in the list
    px_link_list_item_t * item_in_list = list->first;

    // Search all items in the list
    while(item_in_list != NULL)
    {
        if(item_in_list == item)
        {
            // Item is in the list
            return true;
        }
        // Next item
        item_in_list = item_in_list->next;
    }
    // Item is not in the list
    return false;
}

