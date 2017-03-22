/*************************************************
 *                                               *
 *  Module: tree.c                               *
 *  Description:                                 *
 *      Implementation of abstract data type     *
 *      n-ary tree.                              *
 *  Author: Van Oostenrijk, A.C.                 *
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "defs.h"
#include "options.h"

/*************************************************
 *                                               *
 *  MACROS                                       *
 *                                               *
 *************************************************/

#define LEFT_OFFSET     1
#define CHILD_WIDTH     12
#define HORBAR          '-'
#define VERTBAR         '|'

/*************************************************
 *                                               *
 *  ERROR MESSAGES                               *
 *                                               *
 *************************************************/
#define ERR_OPEN \
    "tree dump failed: cannot open output file %s.\n"
#define MSG_DUMP_COMPLETE \
    "tree dump completed to file \"%s\".\n"


/*************************************************
 *                                               *
 *  GLOBALS                                      *
 *                                               *
 *************************************************/

/*
 *  currentX is used to keep of track of cursor
 *  during printing.
 */
static int currentX = 1;


/*************************************************
 *                                               *
 *  FUNCTIONS                                    *
 *                                               *
 *************************************************/


/*
 *  Reserve [size] bytes of memory, and initialize
 *  the reserved memory to 0.
 */
static void *MallocEx( size_t size )
{
    void *ptr;
    
    ptr = malloc( size );
    memset( ptr, 0, size );
    return( ptr );
}

TreeNode *CreateTreeNode( void *data )
{
    TreeNode* node;

    /*
     *  Reserve memory for new tree node structure.
     */
    node = (TreeNode *) MallocEx( sizeof( TreeNode ) );

    /*
     *  Initialize new tree node structure.
     */
    node->children = ListInit( NULL );
    node->parent = NULL;
    node->data = data;
    node->screenX = 0;

    return( node );
}

void AddTreeChild( TreeNode *parentnode, TreeNode *node )
{
    /* Do not act on an empty node. */
    if( node == NULL ) return;

    /* If the tree is empty, add the first root node. */
    if( parentnode == NULL )
    {
       node->parent = NULL;
    }
    else
    /* Tree is not empty. Add the new node to [parentnode]'s
     * children list. */
    {
       node->parent = parentnode;

       ListAppend( parentnode->children, node );
    }
}

TreeNode* AddNewTreeNode( TreeNode *parentnode, void *data )
{
    TreeNode *node;

    node = CreateTreeNode( data );
    AddTreeChild( parentnode, node );

    return( node );
}

int GetNrOfChildren( TreeNode *node )
{
    return( ListSize( node->children ) );
}

        
TreeNode *GetTreeChild( TreeNode *parentnode, int childNum )
{
    int size;
    int i;
    ListNode *listNode;

    size = ListSize( parentnode->children );
    assert( childNum >= 0 && childNum < size );

    listNode = ListFirstEx( parentnode->children );
    for( i = 0; i < childNum; i++ )
    {
        listNode = ListNextEx( listNode );
    }
    return( ( TreeNode * ) listNode->data );
}


void RemoveTreeChildren( TreeNode *parentnode )
{
    ListNode *listNode;

    /*  Do not act on an empty node. */
    if( parentnode == NULL ) return;

    /*
     *  Loop through [parentnode]'s children list, and call
     *  RemoveTreeChildren recursively for each child.
     */
    listNode = ListFirstEx( parentnode->children );
    while( listNode != NULL )
    {
        RemoveTreeChildren( (TreeNode *) (listNode->data) );
        listNode = ListNextEx( listNode );
    }
    
    ListPurge( parentnode->children, NULL );
}


void RemoveTreeNode( TreeNode *node )
{
    TreeNode *parent;
    TreeNode *child;
    int i;

    /*  Do not act on an empty (sub)tree. */
    if( node == NULL ) return;

    /* Kill the node's children. */
    RemoveTreeChildren( node );

    /*
     *  Remove node from parent's children list.
     *  Cannot use extended list functions here
     *  because ListRemove needs the 'current'
     *  pointer.
     */
    if( node->parent != NULL )
    {
        parent = node->parent;
        ListFirst( parent->children );
        for( i = 0; i < ListSize( parent->children ); i++ )
        {
            child = ListGet( parent->children );
            if( child == node )
            {
                ListRemove( parent->children, NULL );
                break;
            }
            ListNext( parent->children );
        }
    }
}

TreeNode *InsertAboveTreeNode( TreeNode *node, TreeNode *new )
{
    TreeNode *parent;
    int i = 0;

    assert( node != NULL );
    
    parent = node->parent;

    
    /* find node under consideration in parent child list */
    ListFirst( parent->children );
    for( i = 0 ; i < ListSize( parent->children ) ; i++ )
    {
        if( ListGet( parent->children ) == node )
        {
            /* unlink child from parent child list */
            ListUnlink( parent->children );
            ListAppend( parent->children, new );
            AddTreeChild( new, node );
            return( node );
        }
        ListNext( parent->children );
    }
    
    return( node );
}

TreeNode *UnlinkTreeNode( TreeNode *node, DataFunction dataFunction )
{
    TreeNode *parent;
    TreeNode *child;
    int i;
    
    /* 	Grab node's parent. */
    parent = node->parent;

    /* 
     *	Move parent child pointer to node 
     * 	under consideration. 
     */
    ListFirst( parent->children );
    for( i = 0; i < ListSize( parent->children ); i++ )
    {
        if( ListGet( parent->children ) == node )
        {
            break;
        }
        ListNext( parent->children );
    }

    /*
     * 	Grab node under consideration's lone child.
     * 	Make it point to [parent]. Add it to parent's
     * 	children list.
     */
    ListFirst( node->children );
    child = (TreeNode*) ListGet( node->children );
    child->parent = parent;
    ListInsert( parent->children, child );

    /* 
     * Remove node under consideration from parent's
     * children list.
     */
    ListNext( parent->children );
    ListUnlink( parent->children );

    /* Unlink node's children. */
    ListFirst( node->children );
    ListUnlink( node->children );
    ListPurge( node->children, NULL );
    free( node );

    return( child );
}


/**********************************************************
 *                                                        *
 * TREE PRINT CODE                                        *
 *                                                        *
 **********************************************************/

/*
 *  Print x instances of character c.
 *  If x == 0, print nothing.
 */
static void PrintChars( int x, char c )
{
    int i;

    for( i = 0; i < x; i++ )
    {
        printf( "%c", c );
    }
}

/*
 *  Determine the x-location of each node in the tree,
 *  starting with node [source]. The left margin used by
 *  the entire tree is passed in [leftleaves], in units
 *  of CHILD_WIDTH spaces.
 */
static int LayoutTree( TreeNode * source, int leftleaves )
{
    int leaves = 0;
    ListNode *listNode;

    /*
     *  For an empty node, return 0 (no effect).
     */
    if( source == NULL )
    {
        return( 0 );
    }

    if( ListSize( source->children ) == 0 )
    {
        source->screenX = leftleaves * CHILD_WIDTH;
        return( 1 );
    }
    else
    {
        leaves = 0;
        listNode = ListFirstEx( source->children );
        while( listNode != NULL )
        {
            leaves += LayoutTree( ( TreeNode * ) listNode->data,
                      leftleaves + leaves );
            listNode = ListNextEx( listNode );
        }

        source->screenX = leftleaves * CHILD_WIDTH;
        if( leaves > 0 )
        {
            source->screenX += ( ( leaves - 1 )
                               * CHILD_WIDTH ) / 2;
        }

        return( leaves );
    }
}

/*
 *  Print tree node [source] on the console.
 *  Pre: [depth] is the current depth in the tree.
 *       [printDepth] is the depth where printing occurs.
 *       [mode] selects what to print:
 *             0 = horizontal node bars.
 *             1 = node branches entering nodes.
 *             2 = custom data for specified level.
 *             3 = node branches leaving nodes.
 *  Post: Returns TRUE if there are deeper nodes
 *        to print, or FALSE if printing is complete.
 */
static BOOL PrintNode(
    TreeNode *source, int depth,
    int printDepth, int mode,
    DataFunction dataFunction, int level )
{
    TreeNode *child;
    ListNode *listNode;
    BOOL result = FALSE;
    char *str;

    /* Print all direct children. */
    if( depth == printDepth && ListSize( source->children ) > 0 )
    {
        result = TRUE;
        switch( mode )
        {
        case 0: /* Print horizontal bars over nodes */
            /* Print spaces up to first child */
            listNode = ListFirstEx( source->children );
            child = ( TreeNode * ) listNode->data;
            PrintChars( child->screenX - currentX, ' ' );
            currentX = child->screenX - 1;
            /* If there is only one child, stop here. */
            if( ListSize( source->children ) == 1 )
            {
                printf( "%c", VERTBAR );
                currentX += 2;
            }
            else
            /* If there are multiple children, draw horizontal
             * bars. */
            {
                /* Move to last child. */
                listNode = ListLastEx( source->children );
                child = ( TreeNode * ) listNode->data;
                /* Print dashes up to last child. */
                PrintChars( child->screenX - currentX, HORBAR );
                currentX = child->screenX + 1;
            }
            break;
        case 1: /* Print branch entering node */
            listNode = ListFirstEx( source->children );
            while( listNode != NULL )
            {
                child = ( TreeNode * ) listNode->data;
                PrintChars( child->screenX - currentX, ' ' );
                currentX = child->screenX + 1;
                printf( "%c", VERTBAR );
                listNode = ListNextEx( listNode );
            }
            break;
        case 3: /* Print branch leaving node */
            listNode = ListFirstEx( source->children );
            while( listNode != NULL )
            {
                child = ( TreeNode * ) listNode->data;
                PrintChars( child->screenX - currentX, ' ' );
                currentX = child->screenX + 1;
                printf( "%c", ListSize( child->children ) > 0 ? VERTBAR : ' ' );
                listNode = ListNextEx( listNode );
            }
            break;
        case 2: /* Print nodes */
            listNode = ListFirstEx( source->children );
            while( listNode != NULL )
            {
                child = ( TreeNode * ) listNode->data;
                str = dataFunction( child, level );
                PrintChars( child->screenX - currentX - strlen( str ) / 2, ' ' );
                currentX = child->screenX + strlen( str ) - strlen( str ) / 2;
                printf( "%s", str );
                listNode = ListNextEx( listNode );
            }
            break;
        }
    }

    /* Walk through children (pre-order). */
    if( printDepth > depth )
    {
        listNode = ListFirstEx( source->children );
        while( listNode != NULL )
        {
            child = ( TreeNode * ) listNode->data;
            if( PrintNode( child, depth + 1, printDepth, mode, dataFunction, level ) == TRUE )
            {
                result = TRUE;
            }
            listNode = ListNextEx( listNode );
        }
    }

    return( result );
}


void PrintTree( TreeNode *source, DataFunction dataFunction, int levels )
{
    int printDepth = 0;
    BOOL loop;
    char *str;
    int i;

    /* TODO: We're going to have to make a new macro.
     * Don't use DEBUG for this.
     */
    DEBUG( "Called\n" );

    /* If tree is empty, abort. */
    if( source == NULL )
    {
        return;
    }

    /* Walk through tree to determine x-offsets for
     * each node.
     */
    LayoutTree( source, LEFT_OFFSET );

    /* Print nodes. */
    for( i = 0; i < levels; i++ )
    {
        str = dataFunction( source, i );
        PrintChars( source->screenX - strlen( str ) / 2, ' ' );
        printf( "%s\n", str );
    }
    PrintChars( source->screenX, ' ' );
    printf( "%c", VERTBAR );

    printDepth = 0;
    do
    {
        currentX = 0;
        printf("\n");
        PrintNode( source, 0, printDepth, 0, dataFunction, 0 );
        currentX = 0;
        printf("\n");
        PrintNode( source, 0, printDepth, 1, dataFunction, 0 );
        currentX = 0;
        printf("\n");
        for( i = 0; i < levels; i++ )
        {
            PrintNode( source, 0, printDepth, 2, dataFunction, i );
            currentX = 0;
            printf("\n");
        }
        loop = PrintNode( source, 0, printDepth, 3, dataFunction, 0 );
        printDepth++;
    }
    while( loop );
}

/*
 *  Write the contents of node [node] to file [fpOut].
 *
 *  format:
 *  #strings/node
 *  strings
 *  #children
 *
 *  Pre: [node] and [fpOut] are valid pointers.
 */
static void WriteNodeToFile( FILE *fpOut, TreeNode *node, DataFunction dataFunction, int levels )
{
    ListNode *listNode;
    int i;
    char *str;

    assert( fpOut != NULL );
    assert( node != NULL );

    /* Write node X-location */
    fprintf( fpOut, "%d\n", node->screenX );

    /* Write strings associated with node. */
    for( i = 0; i < levels; i++ )
    {
        str = dataFunction( node, i );
        fprintf( fpOut, "%s\n", str );
    }

    /* Write #children to file. */
    fprintf( fpOut, "%d\n", ListSize( node->children ) );

    /* For each of the node's children, call
     * WriteNodeToFile recursively.
     */
    listNode = ListFirstEx( node->children );
    while( listNode != NULL )
    {
        WriteNodeToFile( fpOut, (TreeNode*) listNode->data, dataFunction, levels );
        listNode = ListNextEx( listNode );
    }
}

void DumpTree( char *filename, TreeNode *source, DataFunction dataFunction, int levels )
{
    FILE *fpOut;        /* Output file pointer. */

    /* If tree is empty, abort. */
    if( source == NULL )
    {
        return;
    }

    /* Walk through tree to determine x-offsets for
     * each node.
     */
    LayoutTree( source, LEFT_OFFSET );

    /*  Attempt to open the output file. */
    fpOut = fopen( filename, "w" );

    /*  Did the open succeed? If not, abort the tree dump. */
    if( fpOut == NULL )
    {
        fprintf( stderr, ERR_OPEN, filename );
        return;
    }

    /* Write number of information levels in nodes to file. */
    fprintf( fpOut, "%d\n", levels );

    /* Write root node to file. WriteNodeToFile will call
     * itself recursively for the children.
     */
    WriteNodeToFile( fpOut, source, dataFunction, levels );

    /* Close the output file. */
    fclose( fpOut );

    fprintf( stdout, MSG_DUMP_COMPLETE, filename );
}


/*
 * Used by YestTree to supply dummy tree content.
 */
static char *TestDataFunction( TreeNode * node, int level )
{
    if( level == 0 )
    {
        return( "hello" );
    }
    if( level == 1 )
    {
        return( "world" );
    }
    return( NULL );
}


BOOL TestTree()
{
    void* data = NULL;
    TreeNode *root, *node, *node2, *node3, *node4 = NULL;

    printf( "Testing tree ADT...\n" );

    root = AddNewTreeNode( NULL, data );
    /* Add 2 nodes to root. */
    node = AddNewTreeNode( root, data );
    node = AddNewTreeNode( root, data );
    /* Add 4 nodes to rightmost node. */
    node2 = AddNewTreeNode( node, data );
    node2 = AddNewTreeNode( node, data );
    node2 = AddNewTreeNode( node, data );
    node2 = AddNewTreeNode( node, data );
    /* Add 3rd node to root. */
    node2 = AddNewTreeNode( root, data );
    /* Add node to rightmost node. */
    node3 = AddNewTreeNode( node2, data );
    /* Add two nodes to rightmost node. */
    node4 = AddNewTreeNode( node3, data );
    node4 = AddNewTreeNode( node3, data );
    /* Add another node to level two. */
    node3 = AddNewTreeNode( node2, data );

    PrintTree( root, TestDataFunction, 2 );
    RemoveTreeNode( node );
    PrintTree( root, TestDataFunction, 2 );
    RemoveTreeNode( root );

    node2 = CreateTreeNode( data );
    node3 = CreateTreeNode( data );
    node = CreateTreeNode( data );
    AddTreeChild( node, node2 );
    AddTreeChild( node, node3 );
    root = CreateTreeNode( data );
    AddTreeChild( root, node );

    PrintTree( root, TestDataFunction, 2 );
    RemoveTreeNode( root );

    return( TRUE );
}
