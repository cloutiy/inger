/*************************************************
 *                                               *
 *  Module: symtab.c                             *
 *  Description:                                 *
 *      Implements the symbol table. The module  *
 *      keeps track of the symbol table and its  *
 *      current scope. The scopes can be walked  *
 *      through using the Enter- and ExitScope   *
 *      functions                                *
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


/* TODO: Add more documentation */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symtab.h"
#include "defs.h"
#include "options.h"
#include "typenames.h"

typedef struct ScopeNode
{
    struct ScopeNode *parent;
    List *symbols;
    List *children;
} ScopeNode;

ScopeNode *g_symtab;
ScopeNode *g_scope;

static void
PrintTree( ScopeNode * node)
{
    static int depth = 0;
    ScopeNode *n;

    /* DEBUG( "node[%p]parent[%p] has %d symbols and %d children\n", node, node->parent, 
     ListSize( node->symbols ), ListSize( node->children ) ); */
    
    ListFirst( node->children );
    while ( ( n = ListGet( node->children ) ) )
    {
        depth++;
        PrintTree( n );
        depth--;
        ListNext( node->children );
    }
}

/* Interface to printtree  */
void
PrintSymbolTable( )
{
    PrintTree( g_symtab );    
}

/* Show the symbol table in a better way than PrintSymbolTable. */
void ShowSymbolTableNode( ScopeNode *node, int depth )
{
    ScopeNode *child;
    Symbol *sym;
    char spaces[512];
    int i = 0, j = 0;

    /* Create initial spacing string. */
    strcpy( spaces, "" );
    for( j = 0; j < depth; j++ )
    {
        strcat( spaces, "|     " );
    }
    
    /* Display symbols in this node. */
    ListFirst( node->symbols );
    for( i = 0; i < ListSize( node->symbols ); i++ )
    {
        sym = (Symbol*)ListGet( node->symbols );
        printf( "%s|\n", spaces );
        printf( "%s+--| %s ", spaces, sym->name );
        
        ListFirst( sym->types );
        for( j = 0 ; j < ListSize( sym->types ) ; j++ )
        {
            printf( "-> %s ", GetTypeName( (Type *)ListGet( sym->types ) ) );
            ListNext( sym->types );
        }
        printf( "\n" );
        
        ListNext( node->symbols );
    }

    /* Recurse. */
    ListFirst( node->children );
    for( i = 0; i < ListSize( node->children ); i++ )
    {
        printf( "%s|\n", spaces );
        printf( "%s+-----+\n", spaces );
        child = (ScopeNode*)ListGet( node->children );
        ShowSymbolTableNode( child, depth+1 );
        ListNext( node->children );
    }
}

void DumpSymbolTable()
{
    ShowSymbolTableNode( g_symtab, 0 );
}

/* Callback function that deletes a symbol struct */
static BOOL DeleteSymbol( void * data )
{
    Symbol * symbol = ( Symbol * ) data;

    assert( data != NULL );

    if( symbol->name )
        free( symbol->name );
    
    if( symbol->types )
        ListPurge( symbol->types, NULL );


    free( symbol );

    return( TRUE );
}

/* Callback function that cleans up a scopenode. Indirect 
 * recusion trough ListPurge takes care of all children */
static BOOL
DeleteScopeNode( void * data )
{
    ScopeNode * node = ( ScopeNode * ) data ;

    assert( data != NULL );

    if( node->children )
        ListPurge( node->children, NULL ); 

    if( node->symbols )
        ListPurge( node->symbols, NULL );
    
    free( node ); 
    return( TRUE );
}

/* Allocates memory for the table's root node (global scope) 
 * and initializes all pointers to NULL */
BOOL InitSymbolTable( )
{
    /* Cannot intialize table if it already exists */
    if( g_symtab )
        return( FALSE );

    /* Allocate memory for root node */
    g_symtab = ( ScopeNode * ) malloc( sizeof( ScopeNode ) );
    if ( !g_symtab )
        BAILOUT( ERR_NOMEM );

    /* Set parent to NULL for root node */
    g_symtab->parent = NULL;

    /* Create list of symbols for the global scope */
    g_symtab->symbols = ListInit( DeleteSymbol );
    if ( !g_symtab->symbols )
        BAILOUT( "List initialization failed." );

    /* Create list of children */
    g_symtab->children = ListInit( DeleteScopeNode );
    if ( !g_symtab->children )
        BAILOUT( "List initialization failed." );

    /* glocal scope is current scope */
    g_scope = g_symtab;

    return( TRUE );

}

/* Deletes the scope tree */
BOOL PurgeSymbolTable( )
{
    /* abuse the callback to purge the tree */
    return( DeleteScopeNode( ( void * ) g_symtab ) );
}

BOOL AddSymbol( Symbol * symbol )
{
    assert( g_symtab != NULL );
    assert( g_scope != NULL );
    assert( symbol != NULL );
    
    return( ListAppend( g_scope->symbols, symbol ) );
}

Symbol *CreateSymbol( char * name )
{
    Symbol * symbol;

    assert( name != NULL );
    
    symbol = ( Symbol * ) malloc( sizeof( Symbol ) );
    if( !symbol )
        BAILOUT( ERR_NOMEM );

    symbol->types = ListInit( DeleteType );
    if( ! symbol->types )
        BAILOUT( "List initialization failed." );

    symbol->name = strdup( name );
    if( !symbol->name )
        BAILOUT( ERR_NOMEM );

    return( symbol );
}

Symbol *AddType( Symbol *symbol, Type *type )
{
    assert( symbol != NULL );
    assert( type != NULL );

    ListAppend( symbol->types, type );
    
    return( symbol );
}
/*
Symbol *AddModifier( Symbol *symbol, Modifier modifier )
{
    int *ptr;
    
    assert( symbol != NULL );

    ptr = ( int * ) malloc( sizeof( int ) );
    if( !ptr )
        BAILOUT( ERR_NOMEM );

    *ptr = modifier;

    ListAppend( symbol->modifiers, ptr );

    return( symbol );
}
*/


/* Finds a symbol in a list of symbols */
Symbol *FindInSymbolList( char *name, List * symbols )
{
    Symbol * symbol;
    ListNode *node;
       
    assert( name != NULL );
    assert( symbols != NULL );
    
    node = ListLastEx( symbols );
    while( node != NULL )
    {
	symbol = ( Symbol * ) node->data;
	if( strcmp( symbol->name, name ) == 0 )
	{
	    return( symbol );
	}
	node = ListPrevEx( node );
    }
    return( NULL );
}

/*
 * Finds a symbol in the tree by recursively going up the tree.
 */
static Symbol *FindSymbolR( char *name, ScopeNode *node )
{
    Symbol *symbol;

    assert( name != NULL );
        
    /* if node arg was NULL, we default to g_scope */
    if ( node == NULL )
    {
        node = g_scope;
    }
    
    symbol = FindInSymbolList( name, node->symbols );
    if ( symbol == NULL )
    {
        if ( node->parent != NULL )
        {
            return( FindSymbolR( name, node->parent ) );
	}
	else
	{
	    /* 
	     * This happens when the symbol 
	     * is not found.
	     */
	    return( NULL );
	}
    }

    /* Set a flag if the symbol came from the root node. */
    if( node == g_symtab )
    {
        symbol->global = TRUE;
    }
    else
    {
	symbol->global = FALSE;
    }
    
    return( symbol );
}

/* interface to FindSymbolR */
Symbol *FindSymbol( char *name )
{
    assert( name != NULL );
    assert( g_symtab != NULL );
    return( FindSymbolR( name, g_scope ) );
}

void GotoSymbolRoot()
{
  assert( g_symtab != NULL );
  
  g_scope = g_symtab;
  ListFirst( g_scope->children );
}

BOOL IsCurrentScopeTheRoot()
{
    if( g_scope == g_symtab )
    {
        return TRUE;
    }
    else
    {
	return FALSE;
    } 
}
	

BOOL EnterScope() {
    assert( g_symtab != NULL );

    if( !g_scope )
    {
        g_scope = g_symtab;
        ListFirst( g_scope->children );
    } else {
        /* Grab current child from node */
        ScopeNode * node = ListGet( g_scope->children );
        if( !node )
        {
          return( FALSE );
        }
        /* for this node make sure that it starts at the first child */
        ListFirst( node->children );

        /* make the next child the current child for subsequent calls */
        ListNext( g_scope->children );

        /* make the earlier retreived node the current scope */
        g_scope = node;
    }
  return( TRUE );
}

/* allocates memory for a new scope, adds it as one of the
 * current scope's children and sets it as the new current scope */
BOOL CreateScope( )
{
    ScopeNode * node;
    assert( g_symtab != NULL );
   
    if ( !g_scope )
        g_scope = g_symtab;
    
    node = ( ScopeNode * ) malloc( sizeof( ScopeNode ) );
    if ( !node )
        BAILOUT( ERR_NOMEM );

    node->symbols = ListInit( DeleteSymbol );
    node->children = ListInit( DeleteScopeNode );
    node->parent = g_scope;
    
    /* add new scope to the current scope node */
    if ( !ListAppend( g_scope->children, node ) )
        return( FALSE );

    g_scope = node;

    return( TRUE );
}

/* moves up a node in the scope tree */
BOOL ExitScope( )
{
    assert( g_symtab != NULL );

    if ( !g_scope->parent )
        return( FALSE );

    g_scope = g_scope->parent;
    return( TRUE );
}


/* TODO: Create more extensive test */
void TestSymbolTable( )
{
    Symbol *s;
    int i, j;
    const int scopeTest = 5, symbolTest = 2;

    InitSymbolTable( );
    for( i = 0 ; i < scopeTest ; i++ )
    {
        CreateScope();
        for( j = 0 ; j < symbolTest ; j++ )
        {
            s = CreateSymbol( "a" );
            AddType( s, AddSimpleType( CreateType( ), INT ) );
            AddSymbol( s );
        }
        /* ExitScope(); */
    }
    DumpSymbolTable();

    i = j = 0;
    GotoSymbolRoot();
    while( EnterScope( ) )
    {
        i++;
        j += ListSize( g_scope->symbols );
    }

    printf( "%d:%d\n", i, j );
    
    if( i == scopeTest && j == ( symbolTest * scopeTest ) )
    {
    	printf( "Symbol table test passed.\n" );
    } else {
        printf( "Symbol table test failed.\n" );
    }

    
    PurgeSymbolTable( );
}

/*
 *  Accessor functions
 */

void AccessorTest()
{
    Type *type;
    SimpleType stype;

    char *naam = "f";

    type = GetSymbolType( naam );
    stype = GetSimpleType( type );
    printf( "symbolname: %d\n", stype );
}

/* no scopelevel needed */
Type *GetSymbolType( char *symbolname )
{
    Symbol *symbol;
    assert( symbolname != NULL );
  
    symbol = FindSymbol( symbolname );
    if( symbol == NULL )
    {
        return( NULL );
    }
  
    /* typeless symbol bad */
    assert( symbol->types != NULL );
    assert( ListSize( symbol->types ) > 0 );

    ListFirst( symbol->types );

    return( (Type *) ListGet( symbol->types ) );
}
