/*************************************************
 *                                               *
 *  Module: types.h                              *
 *  Description:                                 *
 *      Interface to the type system. Defines    *
 *      wrapper functions to shield internal     *
 *      workings of the type structures          *
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


#ifndef TYPES_H
#define TYPES_H 1

#include "list.h"

#define NR_OF_SIMPLETYPES 7

typedef enum
{
    CHAR = 0,
    INT,
    FLOAT,
    BOOLEAN,
    VOID,
    UNTYPED,
    UNKNOWN
} SimpleType;


typedef enum
{
    CONST,
    START,
    EXTERN
} Modifier;

typedef struct Type
{
    SimpleType type;
    List *modifiers;
    List *dimensions;
} Type;

/* Create an empty type. It has no properties yet
 * 
 */
Type *CreateType( );

/* Adds a simple type to this complex type
 *
 */
Type *AddSimpleType( Type *type, SimpleType simpleType );

/* Adds another dimension to the complex type
 * eg:
 *
 * char acbuffers[5][128] 
 * would result in two subsequent calls to AddDimension as follows:
 * 
 *      AddDimension( type, 5 );
 *      AddDimension( type, 128 );
 *      
 * or a recursive alternative: 
 * 
 *      AddDimension( Adddimension( type, 5 ), 128 );
 *      
 * Dimension information can be retreived with GetDimension()
 */
Type *AddDimension( Type *type, int size );

/* Returns the simple type this type encapsulates */
SimpleType GetSimpleType( Type *type );

/* Returns the size of the dimension identified by offset. If
 * acTokens was defined as 'char acTokens[128];' this function
 * will return 128 if called with offset = 0
 */
int GetDimension( Type *type, int offset );

/* Adds a modifier to a type and returns the type
 */
Type *AddModifier( Type *type, Modifier modifier );

/*
 *  TODO: Where's the documentation?
 */
BOOL DeleteType( void *data );

Type * CopyType( Type *type );

BOOL RemoveOneDimension( Type * type );

/* Test routine */
void TypeTest( );

#endif

