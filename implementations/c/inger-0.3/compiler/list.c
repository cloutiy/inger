/*************************************************
 *                                               *
 *  Module: list.c                               *
 *  Description:                                 *
 *      Simple abstract double linked list       *
 *  Author: Janssen, T.H.,                       *
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

/*
 * $Log: list.c,v $
 * Revision 1.21  2003/01/10 10:30:48  dmeffert
 * Fixed segfault bug in code generation
 *
 * Revision 1.20  2002/11/27 18:04:01  vanoostenrijk
 * Added more comments and removed all routine variables. We now use __FUNCTION__.
 *
 * Revision 1.19  2002/11/27 15:14:58  jeeweee
 * Improved ListClear function
 *
 * Revision 1.18  2002/11/27 14:53:00  jeeweee
 * Modified ListClear implementation and used it in CheckFunctionHeader
 *
 * Revision 1.17  2002/11/27 14:31:07  vanoostenrijk
 * New linked list code: ListClear
 *
 * Revision 1.16  2002/11/27 10:51:41  vanoostenrijk
 * Added line numbers to AST.
 *
 * Revision 1.15  2002/11/15 19:04:44  vanoostenrijk
 * Wasted many hours updating header comment blocks. Updated header template.
 *
 * Revision 1.14  2002/11/15 18:41:05  vanoostenrijk
 * Changed all tabs I could find to 4 spaces.
 * Added some comments.
 *
 * Revision 1.13  2002/11/15 18:21:06  vanoostenrijk
 * Guys, compile with -Wall! Removed as many warnings as I could.
 *
 * Revision 1.12  2002/11/14 19:43:17  vanoostenrijk
 * Replaced string literals with macros for easy translation / changing.
 *
 * Revision 1.11  2002/11/14 17:29:30  vanoostenrijk
 * Additional code for extended linked list operations.
 *
 * Revision 1.10  2002/11/14 15:53:35  vanoostenrijk
 * Added new linked list accessors, restyled code, added test code.
 *
 * Revision 1.9  2002/11/14 13:17:33  vanoostenrijk
 * Added code for tree compaction.
 *
 * Revision 1.8  2002/11/10 14:50:36  vanoostenrijk
 * Cosmetic changes.
 *
 * Revision 1.7  2002/11/10 11:59:48  vanoostenrijk
 * Fixed some warnings.
 *
 * Revision 1.6  2002/11/07 10:11:50  tjanssen
 * Added missing types.c
 *
 * Revision 1.5  2002/10/17 13:16:23  tjanssen
 * modified header again
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "options.h"
#include "defs.h"

/*************************************************
 *                                               *
 *  STRINGS                                      *
 *                                               *
 *************************************************/

#define ERR_LISTALLOC "memory allocation for new list failed\n"
#define ERR_NODEALLOC "failed to allocate memory for new node in list %p\n"
#define MSG_TEST "Testing list ADT...\n"
#define MSG_LIST_ELEMENTS "Listing elements (should list 2x5 elements): \n"
#define MSG_LIST_ELEMENTS_1 "List style 1:"
#define MSG_LIST_ELEMENTS_2 "List style 2:"
#define MSG_REMOVED_ELEMENTS "Removed all elements, elements left:"
#define MSG_TEST_COMPLETE "List test completed successfully.\n\n"

List *ListInit( DeleteFunction deleteFunction )
{
    List *list;
    
    /*
     * Allocate memory for list structure.
     */
    list = ( List * )malloc( sizeof( List ) );
    if ( list == NULL )
    {
        BAILOUT( ERR_LISTALLOC );
    }
    
    /*
     * Initialize new list structure.
     */
    list->deleteFunction = deleteFunction;
    list->first = list->last = list->current = NULL;

    return( list ); 
}


void ListPurge( List *list, DeleteFunction deleteFunction )
{
    assert( list != NULL );
    
    /* Remove items one by one. */
    ListFirst( list ); 
    while ( ListRemove( list, deleteFunction ) ) 
        ;

    /* Free the list structure itself. */
    free( list );
} 

void ListClear( List *list )
{
    assert( list != NULL );
    
    /* Remove items one by one. */
    while( ListSize( list ) > 0 )
    {
	ListLast( list );
	ListUnlink( list );
    }

    /* Free the list structure itself. */
    free( list );
}


BOOL ListInsert( List *list, void *data )
{
    ListNode *n, *p, *newNode;
        
    assert( list != NULL );
    
    n = list->current;
    if ( n )
    {
        p = n->prev;
    }
    else
    {
        p = NULL;
    }

    newNode = ( ListNode * )malloc( sizeof( ListNode ) );
    if ( newNode == NULL )
    {
        BAILOUT( ERR_NODEALLOC, list );
    }
 
    newNode->list = list;   
    newNode->next = n;
    newNode->prev = p;
    newNode->data = data;

    /* DEBUG( "new[%p,%p,%p]\n", newNode, newNode->next, newNode->prev ); */
   
    /* fix next node */ 
    if ( n != NULL )
    {
        n->prev = newNode;
    }
    
    /* fix previous node */
    if ( p != NULL )
    {
        p->next = newNode;
    }

    /* if null or next fix the lists first ptr */
    if ( list->first == NULL || list->first == newNode->next )
    {
        list->first = newNode;
    }

    /* if null, fix the lists last ptr */
    if ( list->last == NULL )
    {
        list->last = newNode;
    }

    /* newly inserted node becomes current ptr */
    list->current = newNode;

    /* DEBUG( "f[%p], l[%p], c[%p]\n", list->first, list->last, list->current ); */
    return( TRUE );
}

BOOL ListAppend( List *list, void *data )
{
    ListNode *newNode;
    
    assert( list != NULL );
    
    newNode = ( ListNode * )malloc( sizeof( ListNode ) );
    if ( !newNode )
    {
        BAILOUT( ERR_NODEALLOC, list );
    }
    
    newNode->list = list;
    newNode->next = NULL;
    newNode->prev = list->last;
    newNode->data = data;

    if ( list->last != NULL )
    {
        list->last->next = newNode;
    }

    list->last = newNode;

    if ( list->first == NULL )
    {
        list->first = newNode;
    }
   
    if ( list->current == NULL )
    {
        list->current = newNode;
    }

    return( TRUE );
}

void *ListUnlink( List *list )
{
    ListNode *n, *p, *c;

    assert( list != NULL );

    c = list->current;
    n = c->next;
    p = c->prev;

    /* fix first or last ptr if either was removed */
    if ( list->first == c )
    {
        list->first = n;
    }
    
    if ( list->last == c )
    {
        list->last = p;
    }
    
    /* take out the node by altering the neighbours */
    if ( p != NULL )
    {
        p->next = n;
    }
    if ( n != NULL )
    {
        n->prev = p;
    }

    list->current = (n != NULL) ? n : p;

    return( c );
}

BOOL ListRemove( List *list, DeleteFunction deleteFunction )
{
    ListNode *n, *p, *c;

    assert( list != NULL );

    if ( list->current == NULL )
    {
        return( FALSE );
    }
    
    c = list->current;
    n = c->next;
    p = c->prev;

    /* fix first or last ptr if either was removed */
    if ( list->first == c )
    {
        list->first = n;
    }
    
    if ( list->last == c )
    {
        list->last = p;
    }
    
    /* take out the node by altering the neighbours */
    if ( p != NULL )
    {
        p->next = n;
    }
    if ( n != NULL )
    {
        n->prev = p;
    }

    /* free resources held by data ptr */
    if ( c->data != NULL)
    {
        if ( deleteFunction != NULL )
        {
            deleteFunction( c->data );
        }
        else if ( list->deleteFunction != NULL )
        {
            list->deleteFunction( c->data );
        }
        else 
        {
            free( c->data );
        }
    }
  
    /* remove the node itself */ 
    free( c ); 
    
    list->current = (n != NULL) ? n : p;

    return( TRUE );
}

void ListLast( List *list )
{
    assert( list != NULL );

    list->current = list->last;    
}

void ListFirst( List *list )
{
    assert( list != NULL );

    list->current = list->first;
}
 
void ListNext( List *list )
{
    assert( list != NULL );

    if ( list->current != NULL )
    {
        list->current = list->current->next;
    }
}
        
void ListPrev( List *list )
{
    assert( list != NULL );
    
    if ( list->current != NULL )
    {
        list->current = list->current->prev;
    }
}

void *ListGet( List *list )
{
    assert( list != NULL );

    if ( list->current != NULL )
    {
        return( list->current->data );
    }
    
    return( NULL );
}


int ListSize( List *list )
{
    int i = 0;
    ListNode *node;

    assert( list != NULL );
    
    node = list->first;
    /* DEBUG( "f[%p],l[%p],c[%p]\n", list->first, list->last, list->current ); */
    while ( node != NULL ) 
    {
        /* DEBUG( "iteration: %d,self[%p],prev[%p],next[%p]->'%s'\n", i, n, n->prev, n->next, (char *)n->data );  */
        i++;
        node = node->next;
    };

    return( i );
}

/*************************************************
 *                                               *
 *  EXTENDED LINKED LIST FUNCTIONS               *
 *                                               *
 *************************************************/

ListNode *ListFirstEx( List *list )
{
    if( list == NULL ) return NULL;
    return( list->first );
}

ListNode *ListLastEx( List *list )
{
    assert( list != NULL );

    return( list->last );
}

ListNode *ListNextEx( ListNode *node )
{
    assert( node != NULL );

    return( node->next );
}

ListNode *ListPrevEx( ListNode *node )
{
    assert( node != NULL );

    return( node->prev );
}

ListNode *ListRemoveEx( ListNode *node )
{
    ListNode *next;

    next = node->next;

    ListFirst( node->list );
    while( ListGet( node->list ) != node )
    {
        ListNext( node->list );
    }

    ListRemove( node->list, NULL );

    return( next );
}

void *ListUnlinkEx( ListNode *node )
{
    ListFirst( node->list );
    while( ListGet( node->list ) != node )
    {
        ListNext( node->list );
    }

    return( ListUnlink( node->list ) );
}



BOOL TestList()
{
    List *list;
    ListNode *node;
    int i;
    int *a; 

    fprintf( stdout, MSG_TEST );

    /* Create a new linked list. */
    list = ListInit( NULL );

    /* Add 4 elements. */
    for( i = 1; i < 5; i++ )
    {
        a = (int *) malloc( sizeof( int ) );
        *a = i;
        ListAppend( list, a );
    }
    
    /* Add one element in front. */
    a = (int *) malloc( sizeof( int ) );
    *a = 0;
    ListFirst( list );
    ListInsert( list, a );

    /* List elements. */
    fprintf( stdout, MSG_LIST_ELEMENTS );
    fprintf( stdout, MSG_LIST_ELEMENTS_1 );
    fprintf( stdout, " [" );
    ListFirst( list );
    for( i = 0; i < ListSize( list ); i++ )
    {
        a = ( int * ) ListGet( list );
        printf( "%d, ", *a );
        ListNext( list );
    }
    printf( "]\n" );
    
    fprintf( stdout, MSG_LIST_ELEMENTS_2 );
    fprintf( stdout, " [" );
    node = ListFirstEx( list );
    while( node != NULL )
    {
        a = ( int * ) node->data;
        printf( "%d, ", *a );	
        node = ListNextEx( node );
    }
    printf( "]\n" );

    /* Remove all elements. */
    while( ListSize( list ) > 0 )
    {
        ListFirst( list );
        ListRemove( list, NULL );
    }

    fprintf( stdout, MSG_REMOVED_ELEMENTS );
    fprintf( stdout, " [" );
    node = ListFirstEx( list );
    while( node != NULL )
    {
        a = ( int * ) node->data;
        printf( "%d, ", *a );	
        node = ListNextEx( node );
    }
    printf( "]\n" );
    
    fprintf( stdout, MSG_TEST_COMPLETE );

    return( TRUE );
}

/* EOF */
