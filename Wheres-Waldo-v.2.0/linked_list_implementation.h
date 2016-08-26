//
//  linked_list_implementation.h
//  Wheres-Waldo-v.2.0
//
//  Created by Thomas Hoinacki on 7/30/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

#ifndef __Wheres_Waldo_v_2_0__linked_list_implementation__
#define __Wheres_Waldo_v_2_0__linked_list_implementation__

// C Library Headers
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <sys/syslimits.h>
#include <fcntl.h>

struct linkedlist_item_t {
    struct linkedlist_item_t *next;
    struct linkedlist_item_t *previous; // I added
};

struct linkedlist_t {
    int lliSizeInBytes;
    struct linkedlist_item_t *head;
};

void linkedlist_init(struct linkedlist_t *list, int lliSizeInBytes);
void linkedlist_free(struct linkedlist_t *list);

struct linkedlist_item_t * linkedlist_append_new_item(struct linkedlist_t *list);
void linkedlist_delete_item(struct linkedlist_t *list, struct linkedlist_item_t *item);



/*
 
 
 
 
 
 struct lli_waldo_item_t {
    // This must be the first item in the LLI
    struct linkedlist_item_t core;
 
    int childrenToCreate;
    char path[PATH_MAX];
 };

 linkedlist_t waldo_linkedlist;
 linkedlist_init(&waldo_linkedlist, sizeof(struct lli_waldo_item_t));
 
 struct lli_waldo_item_t *newItem = (struct lli_waldo_item_t *) linkedlist_append_new_item(&waldo_linkedlist);
 strncpy(newItem->
 
 
 
 */





#endif /* defined(__Wheres_Waldo_v_2_0__linked_list_implementation__) */
