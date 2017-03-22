/*************************************************
 *                                               *
 *  Module: tokens.h                             *
 *  Description:                                 *
 *      Contains token definitions.              *
 *  Author: Van Oostenrijk, A.C.                 *
 *  Modifications:                               *
 *  AO: Rearranged keywords in alphabetic        *
 *      fashion.                                 *
 *  AO: Keyword 'then' removed.                  *
 *  AO: Renamed QUESTIONMARK to OP_TERNARY_IF.   *
 *  AO: Renamed PARENTHESIS_LEFT to LPAREN.      *
 *  AO: Renamed PARENTHESIS_RIGHT to RPAREN.     *
 *  AO: Renamed BRACKET_LEFT to LBRACKET.        *
 *  AO: Renamed BRACKET_RIGHT to RBRACKET.       *
 *  AO: Renamed BLOCK_START to LBRACE.           *
 *  AO: Renamed BLOCK_END to RBRACE.             *
 *  AO: Keyword 'extern' added.                  *
 *  JWH: Renamed "parser.h" to "tokens.h"        *
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

/*!TOKENSH*/
#ifndef TOKENS_H
#define TOKENS_H

#include "defs.h"
/* #include "type.h" */
#include "tokenvalue.h"
#include "ast.h"

/*
 *
 *  MACROS 
 *
 */

/* Define where a line starts (at position 1)
 */
#define LINECOUNTBASE 1
/* Define the position of a first character of a line.
 */
#define CHARPOSBASE 1
/* Define the block size with which strings are allocated.
 */
#define STRING_BLOCK 100 

/*
 *
 *  TYPES
 *
 */

/* This enum contains all the keywords and operators
 * used in the language.
 */
enum
{
    /* Keywords */
    KW_BREAK            = 1000, /* "break" keyword */
    KW_CASE,                    /* "case" keyword */
    KW_CONTINUE,                /* "continue" keyword */
    KW_DEFAULT,                 /* "default" keyword */
    KW_DO,                      /* "do" keyword */
    KW_ELSE,                    /* "else" keyword */
    KW_EXTERN,                  /* "extern" keyword */
    KW_GOTO,                    /* "goto" keyword */
    KW_IF,                      /* "if" keyword */
    KW_LABEL,                   /* "label" keyword */
    KW_MODULE,                  /* "module" keyword */
    KW_RETURN,                  /* "return"keyword */
    KW_START,                   /* "start" keyword */
    KW_SWITCH,                  /* "switch" keyword */
    KW_WHILE,                   /* "while" keyword */

    /* Type identifiers */
    KW_BOOL,                    /* "bool" identifier */
    KW_CHAR,                    /* "char" identifier */
    KW_FLOAT,                   /* "float" identifier */
    KW_INT,                     /* "int" identifier */
    KW_UNTYPED,                 /* "untyped" identifier */
    KW_VOID,                    /* "void" identifier */

    /* Variable lexer tokens */
    LIT_BOOL,                   /* bool constant */
    LIT_CHAR,                   /* character constant */
    LIT_FLOAT,                  /* floating point constant */
    LIT_INT,                    /* integer constant */
    LIT_STRING,                 /* string constant */
    IDENTIFIER,                 /* identifier */

    /* Operators */
    OP_ADD,                     /* "+"  */
    OP_ASSIGN,                  /* "="  */
    OP_BITWISE_AND,             /* "&" */
    OP_BITWISE_COMPLEMENT,      /* "~"  */
    OP_BITWISE_LSHIFT,          /* "<<" */
    OP_BITWISE_OR,              /* "|" */
    OP_BITWISE_RSHIFT,          /* ">>" */
    OP_BITWISE_XOR,             /* "^"  */
    OP_DIVIDE,                  /* "/"  */
    OP_EQUAL,                   /* "==" */
    OP_GREATER,                 /* ">"  */
    OP_GREATEREQUAL,            /* ">=" */
    OP_LESS,                    /* "<"  */
    OP_LESSEQUAL,               /* "<=" */
    OP_LOGICAL_AND,             /* "&&" */
    OP_LOGICAL_OR,              /* "||" */
    OP_MODULUS,                 /* "%"  */
    OP_MULTIPLY,                /* "*"  */
    OP_NOT,                     /* "!"  */
    OP_NOTEQUAL,                /* "!=" */
    OP_SUBTRACT,                /* "-"  */
    OP_TERNARY_IF,              /* "?"  */
   
    /* Delimiters */
    ARROW,                      /* "->" */
    LBRACE,                     /* "{"  */
    RBRACE,                     /* "}"  */
    LBRACKET,                   /* "["  */
    RBRACKET,                   /* "]"  */
    COLON,                      /* ":"  */
    COMMA,                      /* ","  */
    LPAREN,                     /* "("  */
    RPAREN,                     /* ")"  */
    SEMICOLON                   /* ";"  */
}
tokens;
 
/*
 *
 *  FUNCTION DECLARATIONS
 *
 */

TreeNode *Parse();

/*
 *
 *  GLOBALS
 *
 */

extern Tokenvalue tokenvalue;

#endif
/*!*/

