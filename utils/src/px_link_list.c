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
    list->item_first = NULL;
    list->item_last  = NULL;
    list->item_count = 0;
    list->items_max  = max_nr_of_items;
}

void px_link_list_item_init(px_link_list_t *      list,
                            px_link_list_item_t * item)
{
    item->item_prev = NULL;
    item->item_next = NULL;
}

bool px_link_list_is_empty(px_link_list_t * list)
{
    if(list->item_first == NULL)
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
    return list->item_first;
}

px_link_list_item_t * px_link_list_get_item_last(px_link_list_t * list)
{
    return list->item_last;
}

px_link_list_item_t * px_link_list_get_item_next(px_link_list_t *      list,
                                                 px_link_list_item_t * item)
{
    return item->item_next;
}

px_link_list_item_t * px_link_list_get_item_prev(px_link_list_t *      list,
                                                 px_link_list_item_t * item)
{
    return item->item_prev;
}

bool px_link_list_insert_item_start(px_link_list_t *      list,
                                    px_link_list_item_t * item_insert)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->item_first = item_insert;
        list->item_last  = item_insert;
        item->item_next  = NULL;
        item->item_prev  = NULL;
    }
    else
    {
        // Insert new item before first item
        item->item_prev             = NULL;
        item->item_next             = list->item_first;
        list->item_first->item_prev = item_insert;
        list->item_first            = item_insert;
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_end(px_link_list_t *      list,
                                  px_link_list_item_t * item_insert)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->item_first = item_insert;
        list->item_last  = item_insert;
        item->item_next  = NULL;
        item->item_prev  = NULL;
    }
    else
    {
        // Append new item to last item
        item->item_prev            = list->item_last;
        item->item_next            = NULL;
        list->item_last->item_next = item_insert;
        list->item_last            = item_insert;
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_before(px_link_list_t *      list,
                                     px_link_list_item_t * item_insert,
                                     px_link_list_item_t * item_pos)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->item_first = item_insert;
        list->item_last  = item_insert;
        item->item_next  = NULL;
        item->item_prev  = NULL;
    }
    else
    {
        // Is this the first item in the list
        if(list->item_first == item_pos)
        {
            // Yes. Insert first
            list->item_first       = item_insert;
            item_insert->item_prev = NULL;
            item_insert->item_next = item_pos;
            item_pos->item_prev    = item_insert;
        }
        else
        {
            // No. Insert before
            item_insert->item_prev         = item_pos->item_prev;
            item_insert->item_next         = item_pos;
            item_pos->item_prev->item_next = item_insert;
            item_pos->item_prev            = item_insert;
        }
    }
    // Increment item count
    list->item_count++;

    return true;
}

bool px_link_list_insert_item_after(px_link_list_t *      list,
                                    px_link_list_item_t * item_insert,
                                    px_link_list_item_t * item_pos)
{
    if(px_link_list_is_full(list))
    {
        return false;
    }
    if(px_link_list_is_empty(list))
    {
        // Add first item
        list->item_first = item_insert;
        list->item_last  = item_insert;
        item->item_next  = NULL;
        item->item_prev  = NULL;
    }
    else
    {
        // Is this the last item in the list
        if(list->item_last == item_pos)
        {
            // Yes. Insert last
            list->item_last        = item_insert;
            item_insert->item_prev = item_pos;
            item_insert->item_next = NULL;
            item_pos->item_next    = item_insert;
        }
        else
        {
            // No. Insert after
            item_insert->item_prev         = item_pos;
            item_insert->item_next         = item_pos->item_next;
            item_pos->item_next->item_prev = item_insert;
            item_pos->item_next            = item_insert;
        }
    }
    // Increment item count
    list->item_count++;

    return true;
}

px_link_list_item_t * px_link_list_remove_item_first(px_link_list_t * list)
{
    px_link_list_item_t * item = list->item_first;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }
    // See if there is only one item
    if(list->item_first == list->item_last)
    {
        list->item_first = NULL;
        list->item_last  = NULL;
    }
    else
    {
        // The next item become the first one in the list
        list->item_first           = item->item_next;
        item->item_next->item_prev = NULL;
    }
    // Clear links of removed item
    item->item_prev = NULL;
    item->item_next = NULL;
    // Decrement item count
    list->item_count--;

    return item;
}

px_link_list_item_t * px_link_list_remove_item_last(px_link_list_t * list)
{
    px_link_list_item_t * item = list->item_last;

    // See if list is empty
    if(px_link_list_is_empty(list))
    {
        return NULL;
    }
    // See if there is only one item
    if(list->item_first == list->item_last)
    {
        list->item_first = NULL;
        list->item_last  = NULL;
    }
    else
    {
        // The previous item become the last one in the list
        list->item_last            = item->item_prev;
        item->item_prev->item_next = NULL;
    }
    // Clear links of removed item
    item->item_prev = NULL;
    item->item_next = NULL;
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
    if(item == list->item_first)
    {
        px_link_list_remove_item_first(list);
        return;
    }

    // See if this is the last item in the list
    if(item == list->item_last)
    {
        px_link_list_remove_item_last(list);
        return;
    }
    // Link previous and next item to each other
    item->item_prev->item_next = item->item_next;
    item->item_next->item_prev = item->item_prev;
    // Clear links of item
    item->item_prev = NULL;
    item->item_next = NULL;
    // Decrement item count
    list->item_count--;
}

bool px_link_list_has_item(px_link_list_t *      list,
                           px_link_list_item_t * item)
{
    // Start at first item in the list
    px_link_list_item_t * item_in_list = list->item_first;

    // Search all items in the list
    while(item_in_list != NULL)
    {
        if(item_in_list == item)
        {
            // Item is in the list
            return true;
        }
        // Next item
        item_in_list = item_in_list->item_next;
    }
    // Item is not in the list
    return false;
}

