/*************************************************
 *                                               *
 *  Module: nodenames.c                          *
 *  Description:                                 *
 *      Contains a function GetNodeName, which   *
 *      returns the name of a given token.       *
 *  Author: Meffert, D.                          *
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

#include <stdio.h>
#include "nodenames.h"
#include "tokens.h"

/* Names of the nodes */
char nodeNames[][40] = 
{
    "module",              /*   00   */
    "start",
    "extern",
    "global",
    "function",
    "functionheader",
    "modifiers",
    "paramlist",
    "paramblock",           
    "param",
    "ret_type",            /*   10   */
    "dim",
    "dimblock",
    "block",
    "statement",
    "switch",
    "cases",
    "case",
    "while",
    "goto",                 
    "label",               /*   20   */
    "if",
    "ident",
    "return",
    "continue",
    "break",
    "declblock",
    "declaration",
    "initializer",
    "indexblock",          
    "reference",           /*   30   */
    "index",
    "expression",
    "||",
    "&&",
    "|",
    "^",
    "&",
    "==",
    "!=",                  
    ">",                   /*   40   */
    ">=",
    "<",
    "<=",
    "<<",
    ">>",
    "=",
    "b+",
    "b-",
    "u+",
    "u-",                  /*   50   */
    "*",
    "/",                  
    "%",
    "~",
    "ADDR",
    "DeREF",
    "!",
    "application",
    "indexer",
    "arguments",          /*   60   */
    "factor",

    "bool",              
    "char",               
    "float",             
    "int",
    "untyped",
    "void",
    
    "lit_bool",
    "lit_char",
    "lit_float",
    "lit_int",
    "lit_string",
    "lit_ident",

    "int2float",
    "char2int",
    "char2float"
};


char* GetNodeName( int node )
{
    static char nodeName[50];

    if( node < 0 || node >= NR_OF_NODES )
    {
	sprintf( nodeName, "node? (%d)", node );
        return( nodeName );
    }
    else 
        return( nodeNames[node] );
}

int GetNodeFromToken( int token )
{
    switch( token )
    {
    case KW_VOID:               return NODE_VOID;
    case KW_BOOL:               return NODE_BOOL;
    case KW_CHAR:               return NODE_CHAR;
    case KW_FLOAT:              return NODE_FLOAT;
    case KW_INT:                return NODE_INT;
    case KW_UNTYPED:            return NODE_UNTYPED;
    case OP_LOGICAL_AND:        return NODE_LOGICAL_AND;
    case OP_LOGICAL_OR:         return NODE_LOGICAL_OR;
    case OP_BITWISE_AND:        return NODE_BITWISE_AND;
    case OP_BITWISE_OR:         return NODE_BITWISE_OR;
    case OP_BITWISE_XOR:        return NODE_BITWISE_XOR;
    case OP_BITWISE_LSHIFT:     return NODE_BITWISE_LSHIFT;
    case OP_BITWISE_RSHIFT:     return NODE_BITWISE_RSHIFT;
    case OP_BITWISE_COMPLEMENT: return NODE_BITWISE_COMPLEMENT;
    case OP_NOT:                return NODE_NOT;
    case OP_ADD:                return NODE_BINARY_ADD;
    case OP_SUBTRACT:           return NODE_BINARY_SUBTRACT;
    case OP_MULTIPLY:           return NODE_MULTIPLY;
    case OP_DIVIDE:             return NODE_DIVIDE;
    case OP_MODULUS:            return NODE_MODULUS;
    case OP_ASSIGN:             return NODE_ASSIGN;
    case OP_EQUAL:              return NODE_EQUAL;
    case OP_NOTEQUAL:           return NODE_NOTEQUAL;
    case OP_GREATER:            return NODE_GREATER;
    case OP_GREATEREQUAL:       return NODE_GREATEREQUAL;
    case OP_LESS:               return NODE_LESS;
    case OP_LESSEQUAL:          return NODE_LESSEQUAL;
    case KW_START:              return NODE_START;
    case KW_EXTERN:             return NODE_EXTERN;
    default:                    return NODE_UNKNOWN;
    }
}
