/*************************************************
 *                                               *
 *  Module: ast.c                                *
 *  Description:                                 *
 *      Implementation of abstract syntax tree.  *
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
#include "ast.h"
#include "defs.h"
#include "nodenames.h"
#include "tokenvalue.h"
#include "typenames.h"

/*************************************************
 *                                               *
 *  MACROS                                       *
 *                                               *
 *************************************************/

/*************************************************
 *                                               *
 *  GLOBALS                                      *
 *                                               *
 *************************************************/

/*************************************************
 *                                               *
 *  FUNCTIONS                                    *
 *                                               *
 *************************************************/

static void *MallocEx( size_t size )
{
    void *ptr;
    
    ptr = malloc( size );
    memset( ptr, 0, size );
    return( ptr );
}

static char *GetAstNodeData( TreeNode *treeNode, int level )
{
    AstNode *astNode;
    static char linenoStr[10];


    assert( treeNode != NULL );
    assert( level >= 0 && level <= 3 );

    astNode = (AstNode *) treeNode->data;
    assert( astNode != NULL );

    if( level == 0 )
    {
        return( GetNodeName( astNode->id ) );
    }
    if( level == 1 )
    {
        return( TokenvalueToString( TOAST( treeNode )->id, TOAST( treeNode )->val ) );
    }
    if( level == 2 )
    {
        
        return( GetTypeName( astNode->type ) );
    }
    if( level == 3 )
    {
	    sprintf( linenoStr, "%d", astNode->lineno );
        return( linenoStr );
    }

    /* Control should never come here. */
    assert( 0 );
    return( 0 );
}

TreeNode *CreateAstNode( int id, int lineno )
{
    TreeNode *treeNode;
    AstNode *astNode;

    astNode = (AstNode *) MallocEx( sizeof( AstNode ) );
    astNode->id = id;
    astNode->lineno = lineno;
    astNode->val.uintvalue = 0;
    astNode->type = NULL;
    treeNode = CreateTreeNode( astNode );

    return( treeNode );
}

TreeNode *CreateAstNodeVal( int id, Tokenvalue val, int lineno )
{
    TreeNode *treeNode;
    AstNode *astNode;

    astNode = (AstNode *) MallocEx( sizeof( AstNode ) );
    astNode->id = id;
    astNode->lineno = lineno;
    astNode->val = val;
    astNode->type = NULL;
    treeNode = CreateTreeNode( astNode );

    return( treeNode );
}

TreeNode *AddNewAstNode( TreeNode *parentnode, int id, int lineno )
{
    TreeNode *treeNode;

    treeNode = CreateAstNode( id, lineno );
    AddTreeChild( parentnode, treeNode );

    return( treeNode );
}

void AddAstChild( TreeNode *parentnode, TreeNode* node )
{
    AddTreeChild( parentnode, node );
}

void RemoveAstNode( TreeNode *node )
{
    RemoveTreeNode( node );
}

/*
 * PRINTING ROUTINES
 */
void PrintAst( TreeNode *source )
{
    PrintTree( source, GetAstNodeData, 4 );
}

void DumpAst( char *filename, TreeNode *source )
{
    DumpTree( filename, source, GetAstNodeData, 4 );
}

void SimplifyAst( TreeNode *source )
{
    TreeNode *child;
    ListNode *listNode;
 
    switch( TOAST( source )->id )
    {
    case NODE_STATEMENT:
    case NODE_FACTOR:
        // printf( "%s\n", GetAstNodeData( source, 0 ) );
        child = UnlinkTreeNode( source, GetAstNodeData );
        /* printf( "%s\n", GetAstNodeData( child,  0 ) ); */
        SimplifyAst( child );
        break;
    default:
        listNode = ListFirstEx( source->children );
        while( listNode != NULL )
        {
            SimplifyAst( ( TreeNode * ) listNode->data );
            listNode = ListNextEx( listNode );
        }
    }
}



/*
 * Converts a Ast Type Node to a simple type.
 *  [node] : Ast Type Node id
 */
SimpleType NodeToSimpleType( int id )
{
    switch( id )
    {
    case NODE_INT:
        return INT;
    case NODE_FLOAT:
        return FLOAT;
    case NODE_BOOL:
        return BOOLEAN;
    case NODE_CHAR:
        return CHAR;
    }

    /* Control should never come here. */
    assert( 0 );
    return( 0 );
}

/*
 * 
 * ACCESSORS.
 *
 */


int GetLineNumberFromNode( TreeNode *node )
{
    return( TOAST( node )->lineno );
}

SimpleType GetTypeFromDecl( TreeNode *declNode )
{
    return( NodeToSimpleType( TOAST( GetTreeChild( declNode, 0 ) )->id  ) );
}

char* GetInitializerFromDecl( TreeNode *declNode )
{
    TreeNode *initNode;
    static char initializer[256];
    
    /* Default the initializer to "". */
    initializer[0] = 0;

    /* Get the initializer node from the declaration node. */
    initNode = GetTreeChild( declNode, 4 );

    /* If there is no initializing return the empty string. */
    if( ListSize( initNode->children ) == 0 )
    {
        return NULL;
    }
    
    /* Determine the time of the declaration and set the
     * initializer string to the right initializing value. */
    switch( GetTypeFromDecl( declNode ) )
    {
        case INT: sprintf( initializer, "%ld", TOAST(GetTreeChild( initNode, 0 ))->val.uintvalue ); break;
        case FLOAT: sprintf( initializer, "%f", TOAST(GetTreeChild( initNode, 0 ))->val.floatvalue ); break;
    }

    /* Return the initializer string. */
    return initializer;
}


char *GetNameFromDecl( TreeNode *declNode )
{
    return( TOAST( GetTreeChild( declNode, 2 ) )->val.identifier );
}

int GetDimensionsFromDecl( TreeNode *declNode )
{
    return( TOAST(GetTreeChild( declNode, 1 ))->val.uintvalue +
	    TOAST(GetTreeChild( declNode, 3 ))->val.uintvalue );
}

char* GetNameOfIdentifier( TreeNode *identNode )
{
    return( TOAST( identNode )->val.identifier );
}

int GetDimensionSizeFromDecl( TreeNode *declNode, int index )
{
    int total;

    total = TOAST( GetTreeChild( declNode, 1 ) )->val.uintvalue;
    if( index < total )
    {
        return( 1 );
    }
    else
    {
        index = index - total;
    }

    return( TOAST( GetTreeChild( GetTreeChild( declNode, 3 ), index ) )->val.uintvalue );
}

char *GetNameFromHeader( TreeNode *headerNode )
{
    return( TOAST( GetTreeChild( headerNode, 1 ) )->val.identifier );
}

BOOL IsStartFromHeader( TreeNode *headerNode )
{
    TreeNode *treeNode;
    int i;

    treeNode = GetTreeChild( headerNode, 0 );
    
    ListFirst( treeNode->children );
    for( i = 0; i < ListSize( treeNode->children ); i++ )
    {
        if( TOAST( GetTreeChild( treeNode, i ) )->id == NODE_START )
        {
            return( TRUE );
        }
        ListNext( treeNode->children );
    }

    return( FALSE );   
}

// TODO: INCORRECT!!!!!!!! Extern is in GLOBAL, NOT IN HEADER.
BOOL IsExternFromHeader( TreeNode *headerNode )
{ 
    TreeNode *treeNode;
    int i;

    treeNode = GetTreeChild( headerNode, 0 );
    
    ListFirst( treeNode->children );
    for( i = 0; i < ListSize( treeNode->children ); i++ )
    {
        if( TOAST( GetTreeChild( treeNode, i ) )->id == NODE_EXTERN )
        {
            return( TRUE );
        }
        ListNext( treeNode->children );
    }

    return( FALSE );   
}

BOOL IsExternFromGlobal( TreeNode *globalNode )
{
    TreeNode *treeNode;

    treeNode = GetTreeChild( globalNode, 0 );
    return( TOAST( treeNode )->id == NODE_EXTERN ? TRUE : FALSE );
}

int GetReturnTypeFromHeader( TreeNode *headerNode )
{
    if( TOAST( GetTreeChild( headerNode, 3  ) )->id == NODE_VOID )
    {
        return( NODE_VOID );
    }
    else
    {
        return( TOAST( GetTreeChild( GetTreeChild( headerNode, 3 ), 0 ) )->id ); 
    }
}

int GetReturnTypeDimensionsFromHeader( TreeNode *headerNode )
{
    return( TOAST(GetTreeChild( GetTreeChild( headerNode, 3 ), 1 ) )->val.uintvalue 
          + TOAST(GetTreeChild( GetTreeChild( headerNode, 3 ), 2 ) )->val.uintvalue  );
}

int GetParamCountFromHeader( TreeNode *headerNode )
{
    TreeNode *treeNode;

    treeNode = GetTreeChild( headerNode, 2 );
    assert( ListSize( treeNode->children ) > 0 );
    if( TOAST( GetTreeChild( treeNode, 0 ) )->id == NODE_VOID )
    {
        return 0;
    }
    else
    {
	return( ListSize( treeNode->children ) );
    }
}

int GetParamTypeFromHeader( TreeNode *headerNode, int index )
{
    TreeNode *treeNode;

    treeNode = GetTreeChild( headerNode, 2 );
    assert( ListSize( treeNode->children ) > index );

    return( TOAST( GetTreeChild( GetTreeChild( treeNode, index ), 0 ) )->id );
}

char *GetParamNameFromHeader( TreeNode *headerNode, int index )
{
    TreeNode *treeNode;

    treeNode = GetTreeChild( headerNode, 2 );
    assert( ListSize( treeNode->children ) > index );

    return( TOAST( GetTreeChild( GetTreeChild( treeNode, index ), 2 ) )->val.identifier );
}

int GetParamDimensionsFromHeader( TreeNode *headerNode, int index )
{
    TreeNode *treeNode;

    treeNode = GetTreeChild( headerNode, 2 );
    assert( ListSize( treeNode->children ) > index );

    return( TOAST(GetTreeChild( GetTreeChild( treeNode, index ), 1 ) )->val.uintvalue  
	  + TOAST(GetTreeChild( GetTreeChild( treeNode, index ), 3 ) )->val.uintvalue );
}

TreeNode *GetHeaderFromFunction( TreeNode *funcNode )
{
    return( GetTreeChild( funcNode, 0 ) );
}

TreeNode *GetBlockFromFunction( TreeNode *funcNode )
{
    if( ListSize( funcNode->children ) < 2 )
    {
        return( NULL );
    }
    return( GetTreeChild( funcNode, 1 ) );
}

char* GetNameFromApplication( TreeNode *appNode )
{
    return TOAST( GetTreeChild( appNode, 0 ) )->val.identifier;
}

int GetArgumentCountFromApplication( TreeNode *appNode )
{
    TreeNode *argsNode = GetTreeChild( appNode, 1 );
    assert( ListSize( appNode->children ) == 2 );
    return( ListSize( argsNode->children ) );
}

TreeNode* GetArgumentFromApplication( TreeNode *appNode, int index )
{
    return GetTreeChild( GetTreeChild( appNode, 1 ), index );
}

int GetLeftOperand( TreeNode * opNode )
{
    AstNode  *astNode = (AstNode *) opNode->data;
    TreeNode *childNode;
    AstNode  *tmpNode;

    assert( opNode != NULL );
    assert( astNode != NULL );

    /*
     *  Return the first child,
     *  this is the left operand.
     */
    ListFirst( opNode->children );
    childNode = GetTreeChild( opNode, 0 );
    tmpNode = (AstNode *) childNode->data;

    return( tmpNode->id );
}


TreeNode* GetExpressionFromIf( TreeNode *ifNode )
{
    return( GetTreeChild( ifNode, 0 ) );

}

TreeNode* GetThenBlockFromIf( TreeNode *ifNode )
{
    return( GetTreeChild( ifNode, 1 ) );
}

TreeNode* GetElseBlockFromIf( TreeNode *ifNode )
{
    if( ListSize( ifNode->children ) > 2 )
        return( GetTreeChild( ifNode, 2 ) );
    else
	return NULL;
}

TreeNode* GetExpressionFromWhile( TreeNode *whileNode )
{
    return( GetTreeChild( whileNode, 0 ) );
}

TreeNode* GetBlockFromWhile( TreeNode* whileNode )
{
    return( GetTreeChild( whileNode, 1 ) ) ;
}

char* GetIdentifierFromAssignment( TreeNode *assignNode )
{
    return( TOAST(GetTreeChild( assignNode, 0 ))->val.identifier );
}
	
	


/*************************************************
 *                                               *
 *  TEST CODE                                    *
 *                                               *
 *************************************************/

BOOL TestAst()
{
    TreeNode *root = NULL;
    TreeNode *node, *node2, *node3, *node4 = NULL;

    printf( "Testing abstract syntax tree...\n" );

    root = AddNewAstNode( NULL, NODE_MODULE,0 );
    node = AddNewAstNode( root, NODE_GLOBAL,0 );
    node2 = AddNewAstNode( node, NODE_START,0 );
    node2 = AddNewAstNode( node, NODE_EXTERN,0 );
    node2 = AddNewAstNode( node, NODE_IF,0 );
    node2 = AddNewAstNode( node, NODE_BINARY_ADD,0 );
    node2 = AddNewAstNode( root, NODE_BITWISE_LSHIFT,0 );
    node3 = AddNewAstNode( node2, NODE_LOGICAL_AND,0 );
    node4 = AddNewAstNode( node3, NODE_LIT_IDENTIFIER,0 );
    node4 = AddNewAstNode( node3, NODE_LIT_INT,0 );
    node3 = AddNewAstNode( node2, NODE_SWITCH,0 );

    PrintAst( root );
    RemoveTreeNode( node );
    PrintAst( root );
    RemoveTreeNode( root );

    node2 = CreateAstNode( NODE_FUNCTIONHEADER,0 );
    node3 = CreateAstNode( NODE_BLOCK,0 );
    node = CreateAstNode( NODE_FUNCTION,0 );
    AddTreeChild( node, node2 );
    AddTreeChild( node, node3 );
    root = CreateAstNode( NODE_MODULE,0 );
    AddTreeChild( root, node );

    PrintAst( root );
    RemoveTreeNode( root );

    return( TRUE );
}
