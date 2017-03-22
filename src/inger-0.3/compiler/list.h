/*************************************************
 *                                               *
 *  Module: list.h                               *
 *  Description:                                 *
 *      Interface to an abstract linked list.    *
 *  Author: T.H. Janssen                         *
 *          Van Oostenrijk, A.C.                 *
 *  Modifications:                               *
 *                                               *
 *************************************************
 *                                               *
 *   This program is free software; you can      *
 *   redistribute it and/or modify  it under     *
 *   the terms of the GNU General Public         *
 *   License as published by the Free            *
 *   Software Foundation; either version 2       *
 *   of the License, or (at your option) any     *
 *   later version.                              *
 *                                               *
 *************************************************/

#ifndef LIST_H
#define LIST_H

#include "defs.h"

/*************************************************
 *                                               *
 *  TYPES                                        *
 *                                               *
 *************************************************/

/* Type specifier of the data cleanup function
 * which is used by the linked list when removing 
 * nodes.
 * 
 * example: 
 * BOOL remove (void *data);
 */
typedef BOOL ( *DeleteFunction )( void * );

/* 
 * Definition of a node for an abstract double
 * linked list *
 */
typedef struct ListNode
{
    void *data;
    struct List *list;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

/* 
 * Every list is stored in its own instance of 
 * this structure.
 */
typedef struct List 
{
    ListNode *first;
    ListNode *last;
    ListNode *current;
    DeleteFunction deleteFunction;
} List;

/*************************************************
 *                                               *
 *  FUNCTION DECLARATIONS                        *
 *                                               *
 *************************************************/

/* 
 * Allocates resources for a list and intializes it
 * to no items (empty).
 *
 * Returns a pointer to the newly created list.
 */
List *ListInit( DeleteFunction deleteFunction );

/* 
 * This function cleans up the entire list using either the
 * default clean callback (if the second parameter is NULL) or 
 * the given callback.
 *
 * Pre:  [list] is a valid linked list.
 *
 * Post: The linked list is empty, and the element memory
 *       released. 
 *       IMPORTANT: The list structure itself
 *                  is also deleted!
 */
void ListPurge( List *list, DeleteFunction deleteFunction );

/* 
 * This function cleans up the entire list, but does not
 * free the memory for the elements (it unlinks them).
 *
 * Pre:  [list] is a valid linked list.
 *
 * Post: The linked list is empty. 
 *       IMPORTANT: The list structure itself
 *                  is also deleted!
 */
void ListClear( List *list );

/* 
 * Inserts a new node in front of current node. 
 *
 * Always returns TRUE.
 *
 * Pre:  [list] is a valid linked list.
 */
BOOL ListInsert( List *list, void *data );

/* 
 * Appends a new node at the back of list.
 *
 * Always returns TRUE.
 *
 * Pre:  [list] is a valid linked list.
 */
BOOL ListAppend( List *list, void *data );

/*
 * Removes current node from list [list], but 
 * does not delete its data.
 *
 * Returns the data pointer. 
 *
 * Pre:  [list] is a valid linked list.
 */
void *ListUnlink( List *list );

/* 
 * Removes current node from list. If no clean callback is 
 * provided the default is used, which is either the callback
 * specified at ListInit or free().
 *
 * Returns TRUE on success, or FALSE if there was no 
 * current node to delete.
 *
 * Pre:  [list] is a valid linked list.
 */
BOOL ListRemove( List *list, DeleteFunction deleteFunction );

/* 
 * Moves the current node pointer to the last
 * node in the list. 
 *
 * Pre:  [list] is a valid linked list.
 */
void ListLast( List *list );

/*
 * Moves the current node pointer to the first 
 * node in the list. 
 *
 * Pre:  [list] is a valid linked list.
 */
void ListFirst( List *list );

/* 
 * Moves the current node pointer one node 
 * further up the list.
 * 
 * Pre:  [list] is a valid linked list.
 */
void ListNext( List *list );

/* 
 * Moves the current node pointer one node
 * down the list.
 *
 * Pre:  [list] is a valid linked list.
 */
void ListPrev( List *list );

/*
 * Returns the data pointer of the current 
 * node.
 *
 * Pre:  [list] is a valid linked list.
 */
void *ListGet( List *list );

/* 
 * Returns the number of nodes in the list.
 *
 * Pre:  [list] is a valid linked list.
 */
int ListSize( List *list );

/*************************************************
 *                                               *
 *  EXTENDED LINKED LIST FUNCTIONS               *
 *                                               *
 *************************************************/

/*
 * Return first ListNode in list [list]. If the list is
 * empty, return NULL. The returned list node
 * may be used to iterate through the list
 * using ListNextEx.
 *
 * Pre:  [list] if a valid linked list.
 */
ListNode *ListFirstEx( List *list );

/*
 * Return last ListNode in list [list]. If the list is
 * empty, return NULL. The returned list node
 * may be used to iterate through the list
 * using ListNextEx.
 *
 * Pre:  [list] if a valid linked list.
 */
ListNode *ListLastEx( List *list );

/*
 * Return the ListNode after the supplied
 * ListNode [node]. If there is no next node,
 * ListNextEx returns NULL.
 *
 * Pre:  [node] is a valid ListNode.
 */
ListNode *ListNextEx( ListNode *node );

/*
 * Return the ListNode before the supplied
 * ListNode [node]. If there is no prev node,
 * ListPrevEx returns NULL.
 *
 * Pre:  [node] is a valid ListNode.
 */
ListNode *ListPrevEx( ListNode *node );

/* 
 * Delete node [node] from list, and return
 * next node. Return NULL if there is no 
 * next node.
 * This function deletes the node contents.
 *
 * Pre:  [node] is a valid ListNode.
 */
ListNode *ListRemoveEx( ListNode *node );

/* Unlink node [node] from list.
 * This function returns the node contents.
 *
 * Pre:  [node] is a valid ListNode.
 */
void *ListUnlinkEx( ListNode *node );

/*************************************************
 *                                               *
 *  LIST TEST CODE                               *
 *                                               *
 *************************************************/

/* 
 * Tests the linked list. Returns TRUE if the
 * test was successful, or FALSE if it failed
 * (something went wrong).
 */
BOOL TestList();


#endif

