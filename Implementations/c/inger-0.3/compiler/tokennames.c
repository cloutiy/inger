/*************************************************
 *                                               *
 *  Module: tokennames.h                         *
 *  Description:                                 *
 *  Contains a function GetTokenName, which      *
 *  returns the name of a given token.           *
 *  Author: Meffert, D.                          *
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

#include <stdio.h>
#include "tokennames.h"

/* Names of the tokens */
char tokenNames[][24] = { 
    "KW_BREAK",
    "KW_CASE", 
    "KW_CONTINUE", 
    "KW_DEFAULT", 
    "KW_DO",
    "KW_ELSE",
    "KW_EXTERN",
    "KW_GOTO",
    "KW_IF", 
    "KW_LABEL", 
    "KW_MODULE", 
    "KW_RETURN",
    "KW_START",
    "KW_SWITCH",
    "KW_WHILE",


    "KW_BOOL",
    "KW_CHAR",
    "KW_FLOAT", 
    "KW_INT", 
    "KW_POINTER", 
    "KW_STRING", 
    "KW_VOID",

    "LIT_BOOL",
    "LIT_CHAR",
    "LIT_FLOAT",
    "LIT_INT",
    "LIT_STRING",
    "IDENTIFIER",

    "OP_ADD",
    "OP_ASSIGN", 
    "OP_BITWISE_AND", 
    "OP_BITWISE_COMPLEMENT",
    "OP_BITWISE_LSHIFT", 
    "OP_BITWISE_OR", 
    "OP_BITWISE_RSHIFT",
    "OP_BITWISE_XOR", 
    "OP_DIVIDE", 
    "OP_EQUAL", 
    "OP_GREATER",
    "OP_GREATEREQUAL", 
    "OP_LESS", 
    "OP_LESSEQUAL", 
    "OP_LOGICAL_AND", 
    "OP_LOGICAL_OR",
    "OP_MODULUS",
    "OP_MULTIPLY", 
    "OP_NOT", 
    "OP_NOTEQUAL",
    "OP_SUBTRACT",
    "OP_TERNARY_IF",

    "ARROW",
    "LBRACE", 
    "RBRACE", 
    "LBRACKET",
    "RBRACKET", 
    "COLON", 
    "COMMA",
    "LPAREN", 
    "RPAREN",
    "SEMICOLON"
};

char* GetTokenName( int token )
{
    static char tokenName[8];

    /* Character */
    if ( token < 256 )
    {
        sprintf( tokenName, "%c", token );
        return tokenName;
    }
    else if( token >= 1000 && token - 1000 < sizeof(tokenNames ) / 24 )
    {
        return( tokenNames[token - 1000] );
    }
    else
    {
        sprintf( tokenName, "unknown" );
        return tokenName;
    }
}


