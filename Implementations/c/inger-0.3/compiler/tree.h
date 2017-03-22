/*************************************************
 *                                               *
 *  Module: tree.h                               *
 *  Description:                                 *
 *      Abstract data type                       *
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

#ifndef TREE_H
#define TREE_H

#include "defs.h"
#include "list.h"


/*************************************************
 *                                               *
 *  TYPES                                        *
 *                                               *
 *************************************************/

/*
 *  The tree ADT consists of a collection of tree
 *  nodes. Each node holds untyped (void*) information,
 *  to which higher level modules can give meaning.
 */
typedef struct TreeNode
{
    void            *data;
    int              screenX;
    struct TreeNode *parent;
    List            *children;
} TreeNode;

/*
 *  When printing a tree, the tree code gets the
 *  data to print through a callback of type
 *  DataFunction. There may be multiple levels of
 *  data to print (node name, node type, etc.)
 *  which are identified using the int argument.
 */
typedef char * ( *DataFunction )( TreeNode *node, int );

/*************************************************
 *                                               *
 *  FUNCTION DECLARATIONS                        *
 *                                               *
 *************************************************/

/*
 *  Create a new node with the specified
 *  contents. The new node is returned.
 */
TreeNode *CreateTreeNode( void *data );

/*
 *  Add [node] as a child to [parentnode].
 *
 *  Pre:  [parentnode] may be NULL, in which case
 *        a new root is created.
 *
 *  Post: [node] was added to [parentnode]'s children
 *        list.
 */
void AddTreeChild( TreeNode *parentnode, TreeNode *node );

/* 
 * Return the # of children for given node.
 *
 * Pre:   [node] must be a valid node
 */
int GetNrOfChildren( TreeNode *node );

/*
 *  Return child #[childNum] of current node.
 *
 *  Pre:  [childNum] is in range of existing children.
 */
TreeNode *GetTreeChild( TreeNode *parentnode, int childNum );

/*
 *  Create a new node with the specified data and add
 *  it to the tree as a child of [parentnode].
 *
 *  Pre:  [parentnode] may be NULL, in which case
 *        a new root is created.
 *
 *  Post: new node with [data] was added to [parentnode]'s
 *        children list.
 */
TreeNode* AddNewTreeNode( TreeNode *parentnode, void *data );



/*
 *  Remove node [node] and all its children
 *  from the tree.
 */
void RemoveTreeNode( TreeNode *node );

TreeNode *InsertAboveTreeNode( TreeNode *node, TreeNode *new );

TreeNode* UnlinkTreeNode( TreeNode *node, DataFunction dataFunction );


/*
 *  Print tree to console, starting with node
 *  [source]. Use the function callback to
 *  allow the caller to supply custom drawing
 *  functions.
 */
void PrintTree( TreeNode *source, DataFunction dataFunction, int levels );

/*
 *  Dump the tree to a file on disk, so that it may be read
 *  back in and drawn on the screen by an external program.
 *  Start the dump with node [source].
 */
void DumpTree( char *filename, TreeNode *source, DataFunction dataFunction, int levels );

/* Test tree module.
 * Post: Returns TRUE if test was successful,
 *       FALSE if it failed.
 */
BOOL TestTree();


#endif
