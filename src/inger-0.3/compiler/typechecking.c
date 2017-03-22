/*************************************************
 *                                               *
 *  Module: typechecking.c                       *
 *  Description:                                 *
 *      Provides functionality to guarantee all  *
 *      operations in the AST have correct types *
 *  Author: Janssen. T.H.                        *
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
#include <stdlib.h>
#include <string.h>

#include "typechecking.h"
#include "typelookup.h"
#include "nodenames.h"
#include "options.h"
#include "symtab.h"
#include "typenames.h"
#include "errors.h"

#define ERRLEN  80

/*************************************************
 *                                               *
 *  FORWARD DECLARATIONS                         *
 *                                               *
 *************************************************/
static Type *GetTypeForUnaryExpression( TreeNode *node );
static Type *GetTypeForBinaryExpression( TreeNode *node );
static Type *GetTypeForApplication( TreeNode *node );
static void CheckTypes( TreeNode *node );
static BOOL Coerce( TreeNode *node, SimpleType simpleType );


/*************************************************
 *                                               *
 *  EXPORTED FUNCTION DEFINITIONS                *
 *                                               *
 *************************************************/

void DecorateAstWithTypes( TreeNode *ast )
{
    assert( ast != NULL );
    GotoSymbolRoot( );
    CheckTypes( ast );
}

/*************************************************
 *                                               *
 *  STATIC FUNCTION DEFINITIONS                  *
 *                                               *
 *************************************************/

static BOOL IsIn( const int table[][3], const int nodetype )
{
    int i = 0;

    assert( table != NULL );

    while( table[i][OPERATOR] != -1 )
    {
        if( table[i][OPERATOR] == nodetype )
        {
            return( TRUE );
        }
        i++;
    }
    return( FALSE );
}

static BOOL Coerce( TreeNode *node, SimpleType simpleType )
{
    int i = 0;
    TreeNode *new = NULL;
        
    assert( node != NULL );
    assert( simpleType >= 0 && simpleType < UNKNOWN );
   
    i = 0;
    while( g_coercion_table[i][ TYPE_FROM ] != TOAST( node )->type->type &&
           g_coercion_table[i][ 0 ] != -1 )
    {
        i++;
    }
    
    while( ( g_coercion_table[i][ TYPE_TO ] != simpleType ) &&
             g_coercion_table[i][ TYPE_FROM ] == TOAST( node )->type->type ) 
    {
        i++;
    }

    if( g_coercion_table[i][0] == -1 
            || g_coercion_table[i][ TYPE_FROM ] != TOAST( node )->type->type )
    {
        return( FALSE );
    }
   
    /* Coercion is legal */
    new = CreateAstNode( g_coercion_table[i][ NODEID ], TOAST( node )->lineno );
    TOAST( new )->type = CopyType( TOAST( node )->type );
    TOAST( new )->type->type = simpleType;       
    
    InsertAboveTreeNode( node, new ); 
    return( TRUE );
}

static Type *GetTypeForExpression( TreeNode *node )
{
    int AstNodeID;
    
    assert( node != NULL );
    
    AstNodeID = TOAST( node )->id;
    
    if( IsIn( g_binary_table, AstNodeID ) )
    {
        return( GetTypeForBinaryExpression( node ) );
    } 
    
    if( IsIn( g_unary_table, AstNodeID ) )
    {
        return( GetTypeForUnaryExpression( node ) );
    }
   
    if( AstNodeID == NODE_APPLICATION )
    {
        return( GetTypeForApplication( node ) );
    }
    
    return( NULL );
}

static Type *GetTypeForBinaryExpression( TreeNode *node )
{
    TreeNode *lchild = NULL, *rchild = NULL;
    AstNode *astNode = NULL;
    Type *type = NULL;
    int i;
    SimpleType wantedSimpleType = UNKNOWN,
               resultType = UNKNOWN;
    
    char err[ERRLEN];
    BOOL goodCoercion = TRUE;

    assert( node != NULL );
 
    
    lchild = GetTreeChild( node, 0 );
    assert( lchild != NULL );
    
    rchild = GetTreeChild( node, 1 ); 
    assert( rchild != NULL );
    
    astNode = TOAST( node );
   
    if( astNode->id == NODE_ASSIGN )
    { /* left hand has uber-priority in assignment */
        wantedSimpleType = TOAST( lchild )->type->type;
    } else {
        /* find first occurence of this operator node in the table */
        i = 0;
        while( g_binary_table[i][OPERATOR] != astNode->id &&
               g_binary_table[i][OPERATOR] != -1 )
        {
            i++;
        }
    
        /* search for first type that matches */
        while( g_binary_table[i][OPERATOR] == astNode->id )
        {
            if(    TOAST( lchild )->type->type == g_binary_table[i][TYPE] 
                || TOAST( rchild )->type->type == g_binary_table[i][TYPE] )
            {
                wantedSimpleType = g_binary_table[ i ][ TYPE ];
                resultType = g_binary_table[ i ][ RESULT ];
                break;
            }
            i++;
        }
    }   
    
    if( wantedSimpleType == UNKNOWN )
    {
        AddError( "Invalid types for binary expression", astNode->lineno );
        return( NULL );
    }

    /* coerce the operand with the smallest priority if required */
    if( TOAST( lchild )->type->type != wantedSimpleType )
    {
        goodCoercion = Coerce( lchild, wantedSimpleType );
    } 
    else if( TOAST( rchild )->type->type != wantedSimpleType )
    {
        goodCoercion = Coerce( rchild, wantedSimpleType );
    }
   
    if( goodCoercion == FALSE )
    {
        snprintf( err, ERRLEN, "Invalid operands for '%s' operator", 
                       GetNodeName( TOAST( node )->id ) );
        
        AddError( err, TOAST( node )->lineno );
    }
       
    if( resultType != UNKNOWN )
    {
        TOAST( node )->type = CreateType();
        AddSimpleType( TOAST( node )->type, resultType );
    } else {
        type = TOAST( lchild )->type;
        assert( type != NULL );
        /* make our own copy of the type */
        TOAST( node )->type = CopyType( type );
        assert( TOAST( node )->type != NULL );
        TOAST( node )->type->type = wantedSimpleType;
    }
    
    return( TOAST( node )->type );
}

static Type *GetTypeForUnaryExpression( TreeNode *node )
{
    TreeNode *child;
    Type *type;

    assert( node != NULL );

    child = GetTreeChild( node, 0 );

    type = TOAST( child )->type;
    assert( type != NULL );

    /* TODO: Check if operator in conjunction with type is 
     *       a valid combination */
    
    /* make our own copy of the type */
    TOAST( node )->type = CopyType( type );
    assert( TOAST( node )->type != NULL );

    /* add or strip a dimension for dereferencing 
     * or addressof operators */
    if( TOAST( node )->id == NODE_DEREFERENCE )
    {
        if( RemoveOneDimension( TOAST( node )->type ) == FALSE )
        {
            AddError( "Dereferencing non-pointer.", 
                      TOAST( node )->lineno );
        }
    }
    else if ( TOAST( node )->id == NODE_ADDRESS )
    {
        AddDimension( TOAST( node )->type, 0 );
    }

    return( TOAST( node )->type );
}

static Type *GetTypeForApplication( TreeNode *node )
{
    Type *type;

    assert( node != NULL );

    type = TOAST( GetTreeChild( node, 0 ) )->type;
    TOAST( node )->type = CopyType( type );

    return( TOAST( node )->type );
}

static Type *GetTypeForLiteralNode( TreeNode *node )
{
    Type * type;

    assert( node != NULL );
    
    if( TOAST( node )->type != NULL )
    {
        return( TOAST( node )->type ); 
    }

    switch( TOAST( node )->id )
    {
    case NODE_LIT_BOOL:
        type = CreateType( );
        AddSimpleType( type, BOOLEAN );
        break;
    case NODE_LIT_CHAR:
        type = CreateType( );
        AddSimpleType( type, CHAR );
        break; 
    case NODE_LIT_FLOAT:
        type = CreateType( );
        AddSimpleType( type, FLOAT);
        break;
    case NODE_LIT_INT:
        type = CreateType( );
        AddSimpleType( type, INT );
        break;
    case NODE_LIT_STRING:
        type = CreateType( );
        AddSimpleType( type, CHAR );
        AddDimension( type, 0 );
        break;
    case NODE_LIT_IDENTIFIER:
        type = CopyType( 
                GetSymbolType( TOAST( node )->val.identifier ) 
               );
        break;
    default:
        return( NULL );
    }
    return( type );
}

static void CheckApplicationParameters( TreeNode *node )
{
    TreeNode *arguments = NULL;
    Symbol *function = NULL;
    Type *reftype = NULL, *argtype = NULL;
    char err[ERRLEN];
    int i = 0;
  
    assert( node != NULL );
    
    function = FindSymbol( 
                    TOAST( GetTreeChild( node, 0 ) )->val.identifier 
                         );
    
    assert( function != NULL );

    arguments = GetTreeChild( node, 1 );
    assert( arguments != NULL );

    ListFirst( function->types );
    
    /* skip return type */
    ListNext( function->types );
    for( i = 0 ; i < ListSize( function->types ) - 1 ; i++ )
    {
        reftype = ListGet( function->types ); 
        assert( reftype != NULL );
        
        argtype = TOAST( GetTreeChild( arguments, i ) )->type;
        assert( argtype != NULL );

        
        if( GetSimpleType( reftype ) != GetSimpleType( argtype ) )
        {   /* Use duplicate of GetTypeName() since we get a pointer 
               to a static buffer and two calls in snprintf would mess
               things up */
            if( Coerce( GetTreeChild( arguments, i ), 
                        GetSimpleType( reftype ) )    == FALSE )
            {
                char *argName = strdup( GetTypeName( argtype ) );
             
                snprintf( err, ERRLEN, 
                          "Type for argument(%d) does not match. "\
                          "Got '%s', expected '%s'", 
                          i+1, argName, GetTypeName( reftype ) );
                
                free( argName );
                AddError( err, TOAST( node )->lineno ); 
            }
        }
    }
}

static void CheckReturnType( TreeNode *node )
{
    TreeNode *function= NULL;
    Type *functiontype = NULL, 
         *returntype = NULL;
    
    assert( node != NULL );

    /* find (grand)parent function node */
    function = node;
    while( TOAST( function )->id != NODE_FUNCTION )
    {
        function = function->parent;
        assert( function!= NULL );
    }

    functiontype = GetSymbolType( GetNameFromHeader( GetTreeChild( function, 0 ) ) );
    assert( functiontype != NULL );

    if( functiontype->type == VOID )
    {
        if( GetNrOfChildren( node ) == 0 )
        {
            /* value-less return from void function -> GOOD */
            return;
        }
        else
        {
            AddWarning( "`return' with a value, in function returning void", TOAST( node )->lineno );
        }
    }
    
    returntype = TOAST( GetTreeChild( node, 0 ) )->type;
    assert( returntype != NULL );
    
    if( functiontype->type != returntype->type )
    {
        if( Coerce( GetTreeChild( node, 0 ), functiontype->type ) == FALSE )
        {
            AddWarning( "Invalid type for return value", TOAST( node )->lineno );
        } else {
            /* TODO: Test indirection level */
        }
    }
}

/* Actual typechecking!
 *
 *
 */
static void CheckNodeType( TreeNode *node )
{
    int AstNodeID; 
    
    assert( node != NULL );
    
    AstNodeID = TOAST( node )->id;
    
    if( IsIn( g_binary_table, AstNodeID ) )
    {
    } 
    
    if( IsIn( g_unary_table, AstNodeID ) )
    {
    }
   
    if( AstNodeID == NODE_APPLICATION )
    {
        CheckApplicationParameters( node );
    }

    if( AstNodeID == NODE_RETURN )
    {
        CheckReturnType( node );
    }
}

static void CheckTypes( TreeNode *node )
{
    int i = 0;
    AstNode *astNode;

    assert( node != NULL );
    
    astNode = TOAST( node );

    if( astNode->id == NODE_GLOBAL )
    {
        if( IsExternFromGlobal( node ) )
        {
            return;
        }
    }
  
    if( ( astNode->id == NODE_BLOCK && 
          TOAST( GetTreeChild( node->parent, 0 ) )->id 
            != NODE_FUNCTIONHEADER ) ||
        astNode->id == NODE_FUNCTIONHEADER )
    {
        EnterScope( );
    }
   
    /* Recurse trough children */
    ListFirst( node->children );
    for( i = 0 ; i < ListSize( node->children ) ; i++ )
    {
        CheckTypes( ListGet( node->children ) );
        ListNext( node->children );
    }
    /* postorder */

    /* Get type directly from node... */
    if( ( astNode->type = GetTypeForLiteralNode( node ) ) == NULL )
    { /* ...or determine it trough expression evaluation */
        GetTypeForExpression( node );
    }
    
    /* Check types for this node */
    CheckNodeType( node );
    
    if( TOAST( node )->id == NODE_BLOCK )
    {
        ExitScope( );
    }        
}
