//
//  SortedList.c
//  lab2a
//
//  Created by Tom Zhang on 10/27/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include "SortedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

//#define	INSERT_YIELD	0x01	// yield in insert critical section
//#define	DELETE_YIELD	0x02	// yield in delete critical section
//#define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    SortedListElement_t* curr = list;
    if (curr == NULL || element == NULL)
        return;
    if(opt_yield & INSERT_YIELD){
        sched_yield();
    }
    while(curr->next != NULL) {
        if (curr->next->key < element->key)
            curr = curr->next;
        else {
            element->next = curr->next;
            element->prev = curr;
            curr->next->prev = element;
            curr->next = element;
            return;
        }
    }
    if (curr->next == NULL) {
        curr->next = element;
        element->prev = curr;
        return;
    }
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element) {
    if (element == NULL)
        return 1;
    if (element->prev != NULL && element->prev->next != element)
        return 1;
    if (element->next != NULL && element->next->prev != element)
        return 1;
    if (opt_yield & DELETE_YIELD){
        sched_yield();
    }
    if (element->next != NULL)
        element->next->prev = element->prev;
    if (element->prev != NULL)
        element->prev->next = element->next;
    return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    SortedListElement_t* curr = list;
    if (curr == NULL)
        return NULL;
    if (curr->next != NULL)
        curr = curr->next;
    while (curr != NULL) {
        if (curr->key == key)
            return curr;
        else {
            if (opt_yield & LOOKUP_YIELD) {
                sched_yield();
            }
            curr = curr->next;
        }
    }
    return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list) {
    SortedListElement_t* curr = list;
    if (curr == NULL)
        return -1;
    if (curr->next != NULL)
        curr = curr->next;
    else
        return 0;
    int size = 0;
    while (curr != NULL) {
        size++;
        if (opt_yield & LOOKUP_YIELD) {
            sched_yield();
        }
        if (curr->next != NULL && curr->next->prev != curr)
            return -1;
        if (curr->prev == NULL || curr->prev->next != curr)
            return -1;
        curr = curr->next;
    }
    return size;
}
