/*************************************************
 *                                               *
 *  Module: tokenvalue.h                         *
 *  Description:                                 *
 *      Defines the Tokenvalue union.            *
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

#ifndef TOKENVALUE_H
#define TOKENVALUE_H

#include "defs.h"

typedef union
{
    unsigned long  uintvalue;
    BOOL    boolvalue;
    char   *stringvalue;
    char    charvalue;
    float   floatvalue;
    char   *identifier;
} Tokenvalue;

char *TokenvalueToString( int node, Tokenvalue tokenvalue );

#endif
