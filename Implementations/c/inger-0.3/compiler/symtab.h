/*************************************************
 *                                               *
 *  Module: symtab.h                             *
 *  Description:                                 *
 *      Provides an interface to the symbol      *
 *      table                                    *
 *  Author: Janssen. T.H.                        *
 *   Modifications:                              *
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


#ifndef _SYMTAB_H
#define _SYMTAB_H 1

#include "list.h"
#include "types.h"


typedef struct
{
    char    *name;
    List    *types;
    BOOL     global;
    int      location;
/*    List    *modifiers; */
} Symbol;


BOOL InitSymbolTable( );
BOOL PurgeSymbolTable( );

BOOL AddSymbol( Symbol *symbol );

Symbol *CreateSymbol( );

Symbol *AddType( Symbol * symbol, Type * type );
/*
Symbol *AddModifier( Symbol * symbol, Modifier modifier );
*/

Symbol *FindSymbol( char * );
BOOL CreateScope( );
BOOL ExitScope( );

extern void DumpSymbolTable();


void TestSymbolTable( );

/*
 *  Accessor functions
 */
void AccessorTest( );
Type *GetSymbolType( char * symbolname );

void GotoSymbolRoot( );
BOOL IsCurrentScopeTheRoot( );
BOOL EnterScope( );

/*
    BOOL AddSymbol( char *, Type * );
    BOOL AddSymbolType( char *, Type * );
*/

/*
    type_t * NewType( simpletype_e, int );
    BOOL AddModifier( Type *, modifier_e );
*/


#endif
