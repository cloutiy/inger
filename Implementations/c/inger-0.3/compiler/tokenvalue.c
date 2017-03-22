/*************************************************
 *                                               *
 *  Module: tokenvalue.c                         *
 *  Description:                                 *
 *      Converts token values to strings.        *
 *  Author: Van Oostenrijk, A.C.                 *
 *  Modifications:                               *
 *      JWH: Added NODE_CASE to the              *
 *      TokenvalueToString function              *
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

#include "tokenvalue.h"
#include "nodenames.h"

char *TokenvalueToString( int node, Tokenvalue tokenvalue )
{
    static char value[100];

    switch( node )
    {
    case NODE_LIT_INT:
    case NODE_CASE:
        sprintf( value, "%ld", tokenvalue.uintvalue );
        break;
    case NODE_LIT_BOOL:
        sprintf( value, "%s", tokenvalue.boolvalue == TRUE ? "true" : "false" );
        break;
    case NODE_LIT_CHAR:
        sprintf( value, "'%c'", tokenvalue.charvalue );
        break; 
    case NODE_LIT_STRING:
        sprintf( value, "\"%s\"", tokenvalue.stringvalue );
        break;
    case NODE_LIT_FLOAT:
        sprintf( value, "%f", tokenvalue.floatvalue );
        break;
    case NODE_LIT_IDENTIFIER:
        sprintf( value, "%s", tokenvalue.identifier );
        break;
    case NODE_REFERENCE:
        sprintf( value, "%ld", tokenvalue.uintvalue );
        break;
    case NODE_DIMENSIONBLOCK:
        sprintf( value, "%ld", tokenvalue.uintvalue );
        break;
    case NODE_INDEX:
        sprintf( value, "%ld", tokenvalue.uintvalue );
        break;
    default:
        sprintf( value, " " );
    }

    return( value );
}
