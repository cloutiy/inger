/*************************************************
 *                                               *
 *  Module: typelookup.h                         *
 *  Description:                                 *
 *     ...                                       *
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


#ifndef TYPELOOKUP_H
#define TYPELOOKUP_H 1

#include "types.h"
#include "nodenames.h"

#define OPERATOR    0
#define TYPE        1
#define RESULT      2

#define TYPE_FROM   0
#define TYPE_TO     1
#define NODEID      2

const int g_coercion_table[][3] = 
{
    { INT, FLOAT, NODE_INT_TO_FLOAT },
    { CHAR, INT, NODE_CHAR_TO_INT },
    { CHAR, FLOAT, NODE_CHAR_TO_FLOAT },
    { -1 }
};

const int g_binary_table[][3] =
{
    { NODE_LOGICAL_OR,      BOOLEAN, UNKNOWN },
    { NODE_LOGICAL_AND,     BOOLEAN, UNKNOWN },
    { NODE_BITWISE_OR,      INT, UNKNOWN },
    { NODE_BITWISE_XOR,     INT, UNKNOWN },
    { NODE_BITWISE_AND,     INT, UNKNOWN },
    { NODE_EQUAL,           INT, BOOLEAN },
    { NODE_NOTEQUAL,        INT, BOOLEAN },
    { NODE_GREATER,         INT, BOOLEAN },
    { NODE_GREATEREQUAL,    INT, BOOLEAN },
    { NODE_LESS,            FLOAT, BOOLEAN },
    { NODE_LESS,            INT, BOOLEAN },
    { NODE_LESS,            CHAR, BOOLEAN },
    { NODE_LESSEQUAL,       FLOAT, BOOLEAN },
    { NODE_LESSEQUAL,       INT, BOOLEAN },
    { NODE_LESSEQUAL,       CHAR, BOOLEAN },
    { NODE_BITWISE_LSHIFT,  INT, UNKNOWN },
    { NODE_BITWISE_RSHIFT,  INT, UNKNOWN },
    { NODE_ASSIGN,          FLOAT, UNKNOWN },
    { NODE_ASSIGN,          INT, UNKNOWN },
    { NODE_ASSIGN,          CHAR, UNKNOWN },
    { NODE_BINARY_SUBTRACT, FLOAT, UNKNOWN },
    { NODE_BINARY_SUBTRACT, INT, UNKNOWN },
    { NODE_BINARY_ADD,      FLOAT, UNKNOWN },
    { NODE_BINARY_ADD,      INT, UNKNOWN },
    { NODE_MULTIPLY,        FLOAT, UNKNOWN },
    { NODE_MULTIPLY,        INT, UNKNOWN },
    { NODE_MULTIPLY,        CHAR, UNKNOWN },
    { NODE_DIVIDE,          INT, UNKNOWN },
    { NODE_MODULUS,         INT, UNKNOWN },
    { -1, -1, UNKNOWN }
};

const int g_unary_table[][3] =
{
    { NODE_UNARY_ADD,       INT, UNKNOWN },
    { NODE_UNARY_SUBTRACT,  INT, UNKNOWN },
    { NODE_DEREFERENCE, CHAR, UNKNOWN },
    { NODE_ADDRESS, CHAR, UNKNOWN },
    { NODE_NOT, BOOLEAN, UNKNOWN },
    { -1, -1, UNKNOWN }
};


#endif

