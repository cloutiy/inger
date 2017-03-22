/*************************************************
 *                                               *
 *  Module: types.c                              *
 *  Description:                                 *
 *      Interface to the type system. Provides   *
 *      wrappers to the type structure           *
 *  Author: Janssen, T.H.                        *
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

#include <assert.h>     /* Required for assert() */
#include <stdlib.h>     /* Required for malloc() */
#include "types.h"
#include "defs.h"


Type *CreateType( )
{
    Type *newType;
    
    newType = ( Type * ) malloc( sizeof( Type ) );
    if( newType == NULL )
    {
        BAILOUT( ERR_NOMEM );
    }

    newType->dimensions = ListInit( NULL );
    assert( newType->dimensions != NULL );
    
    newType->modifiers = ListInit( NULL );
    assert( newType->modifiers != NULL );

    return( newType );
}

Type *AddSimpleType( Type *type, SimpleType simpleType )
{
    /* Check pre conditions */
    assert( type != NULL );
    
    type->type = simpleType;
    return( type );
}

Type *AddDimension( Type *type, int size )
{
    int *ptr;

    /* Check pre conditions */
    assert( type != NULL );
    assert( size >= 0 );
    
    /* allocate memory to store the dimension size in */
    ptr = ( int * ) malloc( sizeof( int ) );
    if( ptr == NULL )
    {
        BAILOUT( ERR_NOMEM );
    }

    *ptr = size;

    /* add new dimension to the list */
    ListAppend( type->dimensions, ptr );

    return( type );
}

SimpleType GetSimpleType( Type *type )
{
    /* Check pre conditions */
    assert( type != NULL );
    
    return( type->type );    
}

int GetDimension( Type *type, int offset )
{
    int i;

    /* Check pre conditions */
    assert( type != NULL );
    assert( offset >= 0 );
    /* assert( offset < ListSize( type->dimensions ) ); */
    if( offset >= ListSize( type->dimensions ) )
        return( -1 );
    
    /* Goto the start of the dimension list */
    ListFirst( type->dimensions );

    /* Count untill we've reached offset */
    for( i = 0 ; i < offset ; i++ )
    {
        ListNext( type->dimensions );
    }
    /* Return the value of current node 8*/
    return( * (int *) ( ListGet( type->dimensions ) ) ); 
}

BOOL RemoveOneDimension( Type * type )
{
    assert( type != NULL );
    
    if ( ListSize( type->dimensions ) <= 0 )
    {
        fprintf( stderr, "TODO: Use proper error reporting! Dereferencing non-pointer!\n" );
        return( FALSE );
    } else {
        ListFirst( type->dimensions );
        ListRemove( type->dimensions, NULL );
    }
    return( TRUE );
}


BOOL DeleteType( void * data )
{
    Type * type = ( Type * ) data;
    assert( type != NULL );
    ListPurge( type->dimensions, NULL );
    ListPurge( type->modifiers, NULL );
    free( type );
    return( TRUE );
}

Type *AddModifier( Type *type, Modifier modifier )
{
    int *ptr;

    assert( type != NULL );

    ptr = ( int * ) malloc( sizeof( Modifier ) );
    if( !ptr )
    {
        BAILOUT( ERR_NOMEM );
    }
    
    *ptr = modifier;

    ListAppend( type->modifiers, ptr );

    return( type );
}


/*
 * Copies a type structure. Caller is responsible for freeing
 */
Type *CopyType( Type *type )
{
    Type *new = NULL;
    int i = 0;

    assert( type != NULL );

    new = CreateType( );

    AddSimpleType( new , type->type );

    /* copy all dimensions to new type */
    ListFirst( type->dimensions );
    for( i = 0 ; i < ListSize( type->dimensions ) ; i++ )
    {
        AddDimension( new, *( (int *) ListGet( type->dimensions ) ) );
        ListNext( type->dimensions );
    }
    
    /* copy all modifiers to new type */
    ListFirst( type->modifiers );
    for( i = 0 ; i < ListSize( type->modifiers ) ; i++ )
    {
        AddModifier( new, *( (Modifier *) ListGet( type->modifiers ) ) );
        ListNext( type->modifiers );
    }
    
    return( new );
}

void TypeTest( )
{
    Type *t;
    int i;
    
    assert( ( t = AddSimpleType( CreateType( ), INT ) ) != NULL );
    for( i = 0 ; i < 30 ; i++ )
    {
        assert( AddDimension( t, i ) );
    }

    assert( GetDimension( t, 5 ) == 5 );
    assert( GetDimension( t, 20 ) == 20 );
    assert( GetDimension( t, 29 ) == 29 );
    assert( GetDimension( t, 30 ) == -1 );
    
    DeleteType( t );
}
