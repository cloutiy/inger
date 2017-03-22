/*************************************************
 *                                               *
 *  Module: getsymbols.c                         *
 *  Description:                                 *
 *      Derives symbol information from          *
 *      an abstract syntax tree and stores it    *
 *      in a symbol table.                       *
 *  Author: Meffert, D.                          *
 *  Modifications:                               *
 *    Janssen,T.H.: Modified code to use the new *
 *                  recursive type functionality *
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

/* Include standard input/output functions. */
#include <stdio.h>
/* Include assertion macro. */
#include <assert.h>
/* Include common definitions. */
#include "defs.h"
/* Include the symbol table interface. */
#include "symtab.h"
/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"
/* For ShowSymbolTable(). */
#include "symtab.h"



/*
 *  Creates a symbol from a variable declaration.
 *
 *  Pre: [node] is a valid 'declaration' node 
 *       in the Abstract Syntax Tree.
 */
void ProcessDeclaration( TreeNode *node )
{
    int i = 0;
    Type *type;
    Symbol *symbol;

    assert( node != NULL );

    /* Get the name of the variable. */
    symbol = CreateSymbol( GetNameFromDecl( node ) );

    /* Create a new complex type. */
    type = CreateType();
    
    /* Determine type of variable. */
    AddSimpleType( type, GetTypeFromDecl(node) );
    
    /* Determine level of indirection. */
    for( i = 0; i < GetDimensionsFromDecl( node ); i++ )
    {
        AddDimension( type, GetDimensionSizeFromDecl( node, i ) );
    }
    
    /* Add the type to the symbol */
    AddType( symbol, type );
    
    /* Add the symbol to the table. */
    AddSymbol( symbol ); 
}



/* 
 *  Creates a symbol from a function header.
 *
 *  Pre: [node] is a valid 'functionheader'
 *       node in the Abstract Syntax Tree.
 */
void ProcessHeader( TreeNode *node )
{
    /* Determine name and intialize iterators. */
    int i = 0, j = 0;
    Type *type;
    Symbol *symbol;

    assert( node != NULL );
    
    
    /* Get the name of this function. */
    symbol = CreateSymbol( GetNameFromHeader( node ) );
    
    /* Create return type. */
    type = CreateType();
    /* Check to see if the return type is a void. */
    if( GetReturnTypeFromHeader( node ) ==  NODE_VOID )
    {
        /* Return type is void, add type to symbol to
         * distinct from variables. */
        AddSimpleType( type, VOID );
    }
    else
    {
        /* Determine return type and add that to the function symbol. */
        AddSimpleType( type, NodeToSimpleType( GetReturnTypeFromHeader(node) ) );

        /* Determine level of indirection. */
        for( i = 0; i < GetReturnTypeDimensionsFromHeader( node ); i++ )
        {
            AddDimension( type, 1 );
        }
    }
    
    /* Add modifiers to the return type. */
    if( IsStartFromHeader( node ) )
    {
        AddModifier( type, START );
    }
    if( IsExternFromHeader( node ) )
    {
        AddModifier( type, EXTERN );
    }

    /* Add return type to the symbol */
    AddType( symbol, type );
    
    /* Add parameter types. */
    if( GetParamCountFromHeader( node ) == 0 )
    {
        /* Add void type to symbol  */
        type = CreateType();
        AddSimpleType( type, VOID );
        AddType( symbol, type );
    }
    else
    {
        for( i = 0; i < GetParamCountFromHeader( node ); i++ )
        {
            /* Create a new type for the parameter. */
            type = CreateType();
            AddSimpleType( type, NodeToSimpleType( GetParamTypeFromHeader(node, i) ) );

            /* Add dimensioning. */
            for( j = 0; j < GetParamDimensionsFromHeader( node, i ); j++ )
            {
                AddDimension( type, 1 );
            }
            AddType( symbol, type );
        }
    }

    /* finally, Add the symbol to the table. */
    AddSymbol( symbol );
}



/* 
 * Create symbols from the function parameters. 
 * 
 * Pre: [node] is a valid 'header' node 
 *      in the Abstract Syntax Tree. 
 */
void ProcessParameters( TreeNode *node )
{
    Symbol *symbol;
    Type *type;
    int i = 0, j = 0;

    assert( node != NULL );
    
    /* Process the parameters. */
    for( i = 0; i < GetParamCountFromHeader( node ); i++ )
    {
        /* Create a new symbol. */
        symbol = CreateSymbol( GetParamNameFromHeader( node, i ) );

        /* Determine the type of the symbol. */
        type = CreateType();
        AddSimpleType( type, NodeToSimpleType( GetParamTypeFromHeader(node, i) ) );

        /* Determine the dimensioning. */
        for( j = 0; j < GetParamDimensionsFromHeader( node, i ); j++ )
        {
            AddDimension( type, 1 );
        }

        /* Add the type to the symbol. */
        AddType( symbol, type );

        /* Add the symbol to the tree. */
        AddSymbol( symbol );
    }
}



/* 
 *  Recursive function wich traverses the 
 *  Abstract Syntax Tree to collect all
 *  symbols.
 *
 *  Pre: [node] is a valid node in the
 *       Abstract Syntax Tree.
 */
void GetSymbols( TreeNode *node )
{
    int i = 0;
    AstNode *astNode = (AstNode*)node->data;

    assert( node != NULL );
    
    /* Determine node type and process it
     * for symbols where necessary. */
    switch( astNode->id )
    {

    /* Process variable declaration. */
    case NODE_DECLARATION:
        ProcessDeclaration( node );
        break;

    /* Process function declaration/definition. */
    case NODE_FUNCTION:
        /* Create a symbol from the header for the current scope. */
        ProcessHeader( GetHeaderFromFunction( node ) );

        /* Note that this may just a function header (if it
         * was extern) and has no block.
         */
        if( GetBlockFromFunction( node ) != NULL )
        {
            /* Create a new scope for the function definition. */
            CreateScope();

            /* Create symbols from the parameter list and add them
             * to the new scope. */
            ProcessParameters( GetHeaderFromFunction( node ) );

            /* Recurse for this function.  */
            GetSymbols( GetBlockFromFunction( node ) );
        }

        /* Leave the function scope. */
        ExitScope();
        break;

    /* Recurse for all children. */
    default:
        ListFirst( node->children );
        for( i = 0; i < ListSize( node->children ); i++ )
        {
            /* Get all symbols from this child node. */
            GetSymbols( (TreeNode*)ListGet( node->children ) );

            /* Try next child. */
            ListNext( node->children );
        }
        break;
    }
}



/* 
 * Initializes the symbol table and starts 
 * the traversal.
 *
 * Pre: [ast] is the root node of the 
 *      Abstract Syntax Tree.
 */
void CreateSymbolTable( TreeNode *ast )
{
    /* Clear the symbol table and perform intialization. */
    InitSymbolTable();

    /* Gather all symbols from the AST. */
    GetSymbols( ast );
}
