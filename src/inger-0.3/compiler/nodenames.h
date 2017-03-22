/*************************************************
 *                                               *
 *  Module: nodenames.c                          *
 *  Description:                                 *
 *      Contains all node names.                 *
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


#ifndef NODENAMES_H
#define NODENAMES_H


#define NR_OF_NODES 77


enum NodeNames 
{
    NODE_MODULE = 0,
    NODE_START,
    NODE_EXTERN,
    NODE_GLOBAL,
    NODE_FUNCTION,
    NODE_FUNCTIONHEADER,
    NODE_MODIFIERS,
    NODE_PARAMLIST,
    NODE_PARAMBLOCK,
    NODE_PARAM,
    NODE_RETURNTYPE,
    NODE_DIMENSION,
    NODE_DIMENSIONBLOCK,
    NODE_BLOCK,
    NODE_STATEMENT,
    NODE_SWITCH,
    NODE_CASES,
    NODE_CASE,
    NODE_WHILE,
    NODE_GOTO,
    NODE_LABEL,
    NODE_IF,
    NODE_IDENT,
    NODE_RETURN,
    NODE_CONTINUE,
    NODE_BREAK,
    NODE_DECLBLOCK,
    NODE_DECLARATION,
    NODE_INITIALIZER,
    NODE_INDEXBLOCK,
    NODE_REFERENCE,
    NODE_INDEX,
    NODE_EXPRESSION,
    NODE_LOGICAL_OR,
    NODE_LOGICAL_AND,
    NODE_BITWISE_OR,
    NODE_BITWISE_XOR,
    NODE_BITWISE_AND,
    NODE_EQUAL,
    NODE_NOTEQUAL,
    NODE_GREATER,
    NODE_GREATEREQUAL,
    NODE_LESS,
    NODE_LESSEQUAL,
    NODE_BITWISE_LSHIFT,
    NODE_BITWISE_RSHIFT,
    NODE_ASSIGN,
    NODE_BINARY_ADD,
    NODE_BINARY_SUBTRACT,
    NODE_UNARY_ADD,
    NODE_UNARY_SUBTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULUS,
    NODE_BITWISE_COMPLEMENT,
    NODE_ADDRESS,
    NODE_DEREFERENCE,
    NODE_NOT,
    NODE_APPLICATION,
    NODE_INDEXER,
    NODE_ARGUMENTS,
    NODE_FACTOR,

    NODE_BOOL,
    NODE_CHAR,
    NODE_FLOAT,
    NODE_INT,
    NODE_UNTYPED,
    NODE_VOID,

    NODE_LIT_BOOL,
    NODE_LIT_CHAR,
    NODE_LIT_FLOAT,
    NODE_LIT_INT,
    NODE_LIT_STRING,
    NODE_LIT_IDENTIFIER,

    NODE_INT_TO_FLOAT,
    NODE_CHAR_TO_INT,
    NODE_CHAR_TO_FLOAT,

    NODE_UNKNOWN = -1
};

char* GetNodeName( int node );
int GetNodeFromToken( int token );

#endif 
