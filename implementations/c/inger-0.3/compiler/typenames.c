/*************************************************
 *                                               *
 *  Module: typenames.c                          *
 *  Description:                                 *
 *      Maps types to type human-readable names. *
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

#include <string.h>

#include "options.h"        /* WantInternalDebug() */
#include "types.h"
#include "typenames.h"

/*************************************************
 *                                               *
 *  GLOBALS                                      *
 *                                               *
 *************************************************/

static char *simpleTypeNames[NR_OF_SIMPLETYPES] =
{
    "char",              /*   00   */
    "int",
    "float",
    "boolean",
    "void",
    "untyped",
    "unknown"
};

/*************************************************
 *                                               *
 *  FUNCTION DEFINITIONS                         *
 *                                               *
 *************************************************/

char* GetTypeName( Type *type )
{
    static char *typeName = "<no type>";
    static char buf[32];
     
    if( type == NULL )
    {
        return( typeName );
    }

    if( type->type < 0 || type->type >= NR_OF_SIMPLETYPES )
    {
        return( typeName );
    }
    else
    {
        int i = 0;
        for( i = 0 ; i < ListSize( type->dimensions ) ; i++ )
        {
            buf[i] = '*';
        }

        strncpy( buf+i, simpleTypeNames[ type->type ], 32-i );
        return( buf );
    }
}
