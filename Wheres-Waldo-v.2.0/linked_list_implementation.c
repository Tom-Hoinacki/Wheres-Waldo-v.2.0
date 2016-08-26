//
//  linked_list_implementation.c
//  Wheres-Waldo-v.2.0
//
//  Created by Thomas Hoinacki on 7/30/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

#include "linked_list_implementation.h"


void linkedlist_init(struct linkedlist_t *list, int lliSizeInBytes)
{
    list->head = NULL;
    list->lliSizeInBytes = lliSizeInBytes;
}


struct linkedlist_item_t * linkedlist_append_new_item(struct linkedlist_t *list)
{
    struct linkedlist_item_t *newItem = (struct linkedlist_item_t *) malloc(list->lliSizeInBytes);
    newItem->next = NULL;
    
    if (list->head == NULL) {
        list->head = newItem;
    } else {
        struct linkedlist_item_t *temp = list->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        
        temp->next = newItem;
    }
    
    return newItem;
}

void linkedlist_free(struct linkedlist_t *list)
{
    //TODO add code logic from project to start with head and clear the list...
}