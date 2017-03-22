/*************************************************
 *                                               *
 *  Module: parser.c                             *
 *  Description:                                 *
 *      Main module for syntax checking.         *
 *  Author: Van Oostenrijk, A.C.                 *
 *  Modifications:                               *
 *    DM: Added redirection of source files to   *
 *        parser.                                *
 *    FJF: Added error recovery with             *
 *        synchronization.                       *
 *    AO: Added abstract syntax tree generation. *
 *    JWH: Renamed "parser.h" to "tokens.h"      *
 *    JWH: Modified ParseSwitchCase()            *
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
/* Include utility declarations from defs.h */
#include "defs.h"
/* Include token #defines from tokens.h */
#include "tokens.h"
/* Include error reporting. */
#include "errors.h"
/* include token names module */
#include "tokennames.h"
/* include node names module */
#include "nodenames.h"
/* include abstract syntax tree module */
#include "ast.h"
/* include options.h to access command line options. */
#include "options.h"

/******************************************************
 *                                                    *
 * EXTERNALS                                          *
 *                                                    *
 ******************************************************/

/*
 *  Import lexer function defined in lexer.l
 *  (there is no header file).
 */
extern int yylex();

/******************************************************
 *                                                    *
 * GLOBALS                                            *
 *                                                    *
 ******************************************************/

/* 
 * The current token is always held in token.
 */
static int token;

/* 
 * Borrow current line number and charPos from lexer. 
 */
extern int lineCount;
extern int charPos;


/******************************************************
 *                                                    *
 * MACROS                                             *
 *                                                    *
 ******************************************************/


#define SET_TYPES        KW_BOOL, KW_CHAR, KW_FLOAT, KW_INT, KW_UNTYPED
#define SET_LITERALS     LIT_BOOL, LIT_CHAR, LIT_FLOAT, LIT_INT, LIT_STRING
#define SET_INSTRUCTIONS KW_LABEL, KW_BREAK, KW_CONTINUE, KW_IF, KW_GOTO, \
                         KW_WHILE, KW_DO, KW_SWITCH, KW_RETURN, SEMICOLON
#define SET_UNARIES      OP_BITWISE_AND, OP_MULTIPLY, OP_BITWISE_COMPLEMENT, \
                         OP_ADD, OP_SUBTRACT, OP_NOT


#define MAKEFIRST(args...) static int first[] = { args, -1 }
#define MAKEFOLLOW(args...) static int follow[] = { args, -1 }
#define SYNC(message) if ( !Sync( first, follow, message ) ) { return( NULL ); }
#define SYNC_ERROR(message) { AddPosError( message, lineCount, charPos ); SyncOut( follow ); return( NULL ); }
#define SYNC_WARN(message) { AddPosWarning( message, lineCount, charPos ); SyncOut( follow ); }

/* Requests a new token from the lexer. 
   The new token is stored in 'token', but
   also returned to the caller. */
int GetToken()
{
    token = yylex();
    return( token );
}


/* Checks whether [t] is in [tokenset].
 * Returns TRUE if it is, FALSE if not.
 */
BOOL Element( int t, int tokenset[] )
{
    int i = 0;

    while( tokenset[i] != -1 )
    {
        if( t == tokenset[i] ) return( TRUE );
        i++;
    }
    return( FALSE );
}

/*!SYNC*/
/* Forward declarations. */
/* If current token is not in FIRST set, display
 * specified error.
 * Skip tokens until current token is in FIRST
 * or in FOLLOW set.
 * Return TRUE if token is in FIRST set, FALSE
 * if it is in FOLLOW set.
 */
BOOL Sync( int first[], int follow[], char *error )
{
    if( !Element( token, first ) )
    {
        AddPosError( error, lineCount, charPos );
    }
    
    while( !Element( token, first ) && !Element( token, follow ) )
    {
        GetToken();
        /* If EOF reached, stop requesting tokens and just
         * exit, claiming that the current token is not
         * in the FIRST set. */
        if( token == 0 )
        {
            return( FALSE );
        }
    }

    /* Return TRUE if token in FIRST set, FALSE
     * if token in FOLLOW set. 
     */
    return( Element( token, first ) );
}
/*!*/

/*!SYNCOUT*/
/* Call this when an unexpected token occurs halfway a
 * nonterminal function. It prints an error, then
 * skips tokens until it reaches an element of the
 * current nonterminal's FOLLOW set. */
void SyncOut( int follow[] )
{
    /* Skip tokens until current token is in FOLLOW set. */
    while( !Element( token, follow ) )
    {
        GetToken();
        /* If EOF is reached, stop requesting tokens and
         * exit. */
        if( token == 0 ) return;
    }
}
/*!*/


/********************************************************
 *                                                      *
 *  NONTERMINAL FUNCTIONS                               *
 *                                                      *
 ********************************************************/


/* Forward declarations. */
TreeNode *ParseBlock();
TreeNode *ParseExpression();


TreeNode *ParseFactor()
{
    TreeNode *node;

    MAKEFIRST( SET_LITERALS, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT,
                OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
                OP_MODULUS );
    SYNC( "literal, ( or identifier expected" );

    node = CreateAstNode( NODE_FACTOR, lineCount );

    if( token == LPAREN )
    {
        GetToken();
        AddAstChild( node, ParseExpression() );
        if( token != RPAREN )
        {
            RemoveAstNode( node );
            SYNC_ERROR( ") expected" );
        }
        GetToken();
    }
    else if( token == IDENTIFIER )
    {
        AddAstChild( node, CreateAstNodeVal( NODE_LIT_IDENTIFIER, tokenvalue, lineCount ) ); // IDENTIFIER
        GetToken();
    }
    else if(    token == LIT_BOOL
             || token == LIT_CHAR
             || token == LIT_FLOAT
             || token == LIT_INT
             || token == LIT_STRING )
    {
        switch( token )
        {
        case LIT_BOOL:
            AddAstChild( node, CreateAstNodeVal( NODE_LIT_BOOL, tokenvalue, lineCount) );
            break;
        case LIT_CHAR:
            AddAstChild( node, CreateAstNodeVal( NODE_LIT_CHAR, tokenvalue, lineCount) );
            break;
        case LIT_FLOAT:
            AddAstChild( node, CreateAstNodeVal( NODE_LIT_FLOAT, tokenvalue, lineCount ) );
            break;
        case LIT_INT:
            AddAstChild( node, CreateAstNodeVal( NODE_LIT_INT, tokenvalue, lineCount ) );
            break;
        case LIT_STRING:
            AddAstChild( node, CreateAstNodeVal( NODE_LIT_STRING, tokenvalue, lineCount ) );
            break;
        }
        GetToken();
    }

    return( node );
}

TreeNode *ParseArguments()
{
    BOOL loop;
    TreeNode *node;

    MAKEFIRST( LPAREN );
    MAKEFOLLOW( RPAREN );
    SYNC( "an application must begin with (" );

    GetToken();
    node = CreateAstNode( NODE_ARGUMENTS, lineCount );

    if( token != RPAREN )
    {
      do
      {
          AddAstChild( node, ParseExpression() );

          /* Expect COMMA or something else. */
          loop = ( token == COMMA );
          if( loop ) GetToken();
      }
      while( loop );
    }

    if( token != RPAREN )
    {
        RemoveAstNode( node );
        SYNC_ERROR( ") expected to terminate application argument list" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseExprUnary1()
{
    TreeNode *node;
    TreeNode *childNode;

    MAKEFIRST( SET_LITERALS, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT,
                OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
                OP_MODULUS );
    SYNC( "a factor must start with a literal, ( or identifier" );

    node = ParseFactor();

    while( token == LPAREN || token == LBRACKET )
    {
        if( token == LPAREN )
        {
            childNode = node;
            node = CreateAstNode( NODE_APPLICATION, lineCount);
            AddAstChild( node, childNode );
            if( token != RPAREN )
            {
                AddAstChild( node, ParseArguments() );
            }
        }
        if( token == LBRACKET )
        {
            GetToken();
            childNode = node;
            node = CreateAstNode( NODE_INDEXER, lineCount  );
            AddAstChild( node, childNode );
            AddAstChild( node, ParseExpression() );
            if( token != RBRACKET )
            {
                RemoveAstNode( node );
                SYNC_ERROR( "] expected to close array index" );
            }
            GetToken();
        }
    }

    return( node );
}


TreeNode *ParseExprUnary2()
{
    TreeNode *node;

    MAKEFIRST( SET_LITERALS, OP_ADD, OP_SUBTRACT, OP_NOT, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT,
                OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
                OP_MODULUS );
    SYNC( "expression must start with literal, unary operator (+, - or !), ( or identifier" );

    if( token == OP_ADD
     || token == OP_SUBTRACT
     || token == OP_NOT )
    {
        switch( token )
        {
        case OP_ADD:
            node = CreateAstNode( NODE_UNARY_ADD, lineCount );
            break;
        case OP_SUBTRACT:
            node = CreateAstNode( NODE_UNARY_SUBTRACT, lineCount );
            break;
        case OP_NOT:
            node = CreateAstNode( NODE_NOT, lineCount );
            break;
        }
        GetToken();
        AddAstChild( node, ParseExprUnary2() );
    }
    else
    {
        node = ParseExprUnary1();
    }

    return( node );
}

TreeNode *ParseExprUnary3()
{
    TreeNode *node;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT,
                OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE,
                OP_MODULUS );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    if( token == OP_BITWISE_AND
     || token == OP_MULTIPLY
     || token == OP_BITWISE_COMPLEMENT )
    {
        switch( token )
        {
        case OP_BITWISE_AND:
            node = CreateAstNode( NODE_ADDRESS, lineCount );
            break;
        case OP_MULTIPLY:
            node = CreateAstNode( NODE_DEREFERENCE, lineCount );
            break;
        case OP_BITWISE_COMPLEMENT:
            node = CreateAstNode( NODE_BITWISE_COMPLEMENT, lineCount );
            break;
        }
        GetToken();
        AddAstChild( node, ParseExprUnary3() );
    }
    else
    {
        node = ParseExprUnary2();
    }

    return( node );
}

TreeNode *ParseExprMultiplication()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT,
                OP_ADD, OP_SUBTRACT );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprUnary3();

    while( token == OP_MULTIPLY || token == OP_DIVIDE || token == OP_MODULUS )
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprUnary3() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprAddition()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL,
                OP_BITWISE_LSHIFT, OP_BITWISE_RSHIFT );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprMultiplication();

    while( token == OP_ADD || token == OP_SUBTRACT )
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprMultiplication() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprShift()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL, OP_GREATER,
                OP_GREATEREQUAL, OP_LESS, OP_LESSEQUAL );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprAddition();

    while( token == OP_BITWISE_LSHIFT || token == OP_BITWISE_RSHIFT )
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprAddition() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprRelation()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND,
                OP_EQUAL, OP_NOTEQUAL );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprShift();

    while( token == OP_GREATER || token == OP_GREATEREQUAL
        || token == OP_LESS || token == OP_LESSEQUAL )
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprShift() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprEquality()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_AND );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprRelation();

    while( token == OP_EQUAL || token == OP_NOTEQUAL )
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprRelation() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprBitwiseAnd()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR, OP_BITWISE_XOR );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprEquality();

    while( token == OP_BITWISE_AND )
    {
        opNode = CreateAstNode( NODE_BITWISE_AND, lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprEquality() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprBitwiseXor()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND, OP_BITWISE_OR );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprBitwiseAnd();

    while( token == OP_BITWISE_XOR )
    {
        opNode = CreateAstNode( NODE_BITWISE_XOR, lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprBitwiseAnd() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprBitwiseOr()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR,
                OP_LOGICAL_AND );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprBitwiseXor();

    while( token == OP_BITWISE_OR )
    {
        opNode = CreateAstNode( NODE_BITWISE_OR, lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprBitwiseXor() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprLogicalAnd()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN, OP_LOGICAL_OR );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprBitwiseXor();

    while( token == OP_LOGICAL_AND )
    {
        opNode = CreateAstNode( NODE_LOGICAL_AND, lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprBitwiseOr() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExprLogicalOr()
{
    TreeNode *childNode;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON, OP_ASSIGN );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    childNode = ParseExprLogicalAnd();

    while( token == OP_LOGICAL_OR )
    {
        opNode = CreateAstNode( NODE_LOGICAL_OR, lineCount );
        AddAstChild( opNode, childNode );
        GetToken();

        AddAstChild( opNode, ParseExprLogicalAnd() );
        childNode = opNode;
    }

    return( childNode );
}

TreeNode *ParseExpression()
{
    TreeNode *node;
    TreeNode *opNode;

    MAKEFIRST( SET_LITERALS, SET_UNARIES, LPAREN, IDENTIFIER );
    MAKEFOLLOW( RPAREN, COMMA, SEMICOLON );
    SYNC( "expression must start with literal, unary operator, ( or identifier" );

    node = ParseExprLogicalOr();
    if( token != OP_ASSIGN )
    {
        return( node );
    }
    else
    {
        opNode = CreateAstNode( GetNodeFromToken(token), lineCount );
        GetToken(); /* eat operator */
        AddAstChild( opNode, node );
        AddAstChild( opNode, ParseExpression() );
        return( opNode );
    }
}

TreeNode *ParseIndex()
{
    TreeNode *node;

    MAKEFIRST( LBRACKET );
    MAKEFOLLOW( SEMICOLON, COMMA, OP_ASSIGN );
    SYNC( "index must start with [" );

    /* Eat [ */
    GetToken();

    /* Expect integer immediate. */
    if( token != LIT_INT ) SYNC_ERROR( "integer number expected in array dimension" );

    //intNode->type = CreateType( TYPE_INT );
    node = CreateAstNodeVal( NODE_INDEX, tokenvalue, lineCount  );
     
    GetToken();

    /* Expect closing ] */
    if( token != RBRACKET )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "] expected to terminate array dimension" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseReference()
{
    TreeNode *node;
    int count = 0;

    node = CreateAstNode( NODE_REFERENCE, lineCount );
    
    while( token == OP_MULTIPLY )
    {
        count++;
        GetToken();
    }

    TOAST( node )->val.uintvalue = count;

    return( node );
}

TreeNode *ParseIndexBlock()
{
    TreeNode *node;

    node = CreateAstNode( NODE_INDEXBLOCK, lineCount );
    while( token == LBRACKET ) AddAstChild( node, ParseIndex() );

    return( node );
}

TreeNode *ParseInitializer()
{
    TreeNode *node;

    node = CreateAstNode( NODE_INITIALIZER, lineCount );

    if( token == OP_ASSIGN )
    {
        GetToken();
        AddAstChild( node, ParseExpression() );
    }

    return( node );
}

TreeNode *ParseDeclaration( int typeToken )
{
    TreeNode *node;

    MAKEFIRST( OP_MULTIPLY, IDENTIFIER );
    MAKEFOLLOW( COMMA, SEMICOLON );
    SYNC( "variable identifier or * expected in declaration" );

    node = CreateAstNode( NODE_DECLARATION, lineCount );

    /* Add type as a child to declaration. */
    AddAstChild( node, CreateAstNode( GetNodeFromToken(typeToken), lineCount ) );

    /* Eat all the *'s we find. */
    AddAstChild( node, ParseReference() );

    /* expect identifier. */
    if( token != IDENTIFIER )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "identifier expected in declaration list" );
    }
    AddAstChild( node, CreateAstNodeVal( NODE_LIT_IDENTIFIER, tokenvalue, lineCount ) );
    GetToken();

    AddAstChild( node, ParseIndexBlock() );

    AddAstChild( node, ParseInitializer() );
    
    return( node );
}

TreeNode *ParseDeclarationBlock()
{
    TreeNode *node;
    int typeToken;
    BOOL loop;

    MAKEFIRST( KW_BOOL, KW_CHAR, KW_FLOAT, KW_INT, KW_UNTYPED );
    MAKEFOLLOW( SEMICOLON );
    SYNC( "declaration must start with type name" );

    node = CreateAstNode( NODE_DECLBLOCK, lineCount );
    typeToken = token;
    GetToken();

    do
    {
        AddAstChild( node, ParseDeclaration( typeToken ) );

        /* Expect COMMA or something else. */
        loop = ( token == COMMA );
        if( loop ) GetToken();
    }
    while( loop );

    return( node );
}


TreeNode *ParseStatementBreak()
{
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );
    if( token != SEMICOLON ) SYNC_ERROR( "; expected after BREAK" );
    GetToken();
    return( CreateAstNode( NODE_BREAK, lineCount ) );
}

TreeNode *ParseStatementContinue()
{
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );
    if( token != SEMICOLON ) SYNC_ERROR( "; expected after CONTINUE" );
    GetToken();
    return( CreateAstNode( NODE_CONTINUE, lineCount ) );
}

TreeNode *ParseStatementReturn()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );
    

    node = CreateAstNode( NODE_RETURN, lineCount );

    /* 
     * An expression between parantheses is 
     * optional.
     */
    if( token == LPAREN )
    {
        GetToken();
        AddAstChild( node, ParseExpression() );

        if( token != RPAREN )
        {
            RemoveAstNode( node );
            SYNC_ERROR( "must terminate RETURN expression with )" );
        }
        GetToken();
    }
    
    if( token != SEMICOLON )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "must terminate RETURN with ;" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseStatementLabel()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );

    if( token != IDENTIFIER ) SYNC_ERROR( "identifier expected after LABEL keyword" );
    node = CreateAstNode( NODE_LABEL, lineCount );
    AddAstChild( node, CreateAstNode( NODE_LIT_IDENTIFIER, lineCount ) );

    GetToken();
    if( token != SEMICOLON )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "; expected after label identifier" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseStatementIf()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );

    if( token != LPAREN ) SYNC_ERROR( "( expected after IF" );

    node = CreateAstNode( NODE_IF, lineCount );
    GetToken();
    AddAstChild( node, ParseExpression() );

    if( token != RPAREN )
    {
        RemoveAstNode( node );
        SYNC_ERROR( ") expected after IF condition" );
    }
    GetToken();

    AddAstChild( node, ParseBlock() );

    if( token == KW_ELSE )
    {
        GetToken();
        AddAstChild( node, ParseBlock() );
    }

    return( node );
}

TreeNode *ParseStatementGoto()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );

    if( token != IDENTIFIER ) SYNC_ERROR( "label identifier expected after 'goto'" );
    node = CreateAstNode( NODE_GOTO, lineCount );
    AddAstChild( node, CreateAstNode( NODE_LIT_IDENTIFIER, lineCount ) );

    GetToken();
    if( token != SEMICOLON )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "; expected after label identifier" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseStatementWhile()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );

    if( token != LPAREN ) SYNC_ERROR( "( expected after WHILE" );

    node = CreateAstNode( NODE_WHILE, lineCount );
    GetToken();
    AddAstChild( node, ParseExpression() );

    if( token != RPAREN )
    {
        RemoveAstNode( node );
        SYNC_ERROR(") expected after WHILE condition" );
    }

    GetToken();
    if( token != KW_DO )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "DO expected after WHILE condition" );
    }

    GetToken();
    AddAstChild( node, ParseBlock() );

    return( node );
}

TreeNode *ParseSwitchCase()
{
    TreeNode *node;
    MAKEFIRST( LIT_INT );
    MAKEFOLLOW( KW_CASE, KW_DEFAULT );
    SYNC( "integer literal expected after CASE" );

    node = CreateAstNodeVal( NODE_CASE, tokenvalue, lineCount  );
    GetToken();
    AddAstChild( node, ParseBlock() );

    return( node );
}

TreeNode *ParseSwitchCases()
{
    TreeNode *node;

    node = CreateAstNode( NODE_CASES, lineCount );

    while( token == KW_CASE )
    {
        GetToken();
        AddAstChild( node, ParseSwitchCase() );
    }

    return( node );
}

TreeNode *ParseStatementSwitch()
{
    TreeNode *node;
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );

    /* Expect LPAREN */
    if( token != LPAREN ) SYNC_ERROR( "( expected after switch" );

    node = CreateAstNode( NODE_SWITCH, lineCount );

    GetToken();
    AddAstChild( node, ParseExpression() );

    if( token != RPAREN )
    {
        RemoveAstNode( node );
        SYNC_ERROR( ") expected after switch expression" );
    }

    GetToken();
    if( token != LBRACE )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "{ expected to open switch case block" );
    }
    GetToken();
    AddAstChild( node, ParseSwitchCases() );

    if( token != KW_DEFAULT )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "must have a default alternative in switch block" );
    }

    GetToken();
    AddAstChild( node, ParseBlock() );

    if( token != RBRACE )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "} expected to close switch case block" );
    }
    GetToken();

    return( node );
}

TreeNode *ParseStatement()
{
    TreeNode *node;
    MAKEFIRST( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
               SET_LITERALS, LPAREN, IDENTIFIER );
    MAKEFOLLOW( SET_INSTRUCTIONS, SET_UNARIES, SET_TYPES,
                SET_LITERALS, LPAREN, IDENTIFIER,
                LBRACE, RBRACE );
    SYNC( "statement must start with an instruction, a unary operator, a type, a literal, (, {, } or an identifier" );

    node = CreateAstNode( NODE_STATEMENT, lineCount );

    switch( token )
    {
    case SEMICOLON:
        GetToken();
        RemoveAstNode( node );
        /* Prevent empty statement from showing up in the
         * AST by returning NULL (but no error). */
        return( NULL );
        break;
    case KW_BREAK:
        GetToken();
        AddAstChild( node, ParseStatementBreak() );
        return( node );
        break;
    case KW_CONTINUE:
        GetToken();
        AddAstChild( node, ParseStatementContinue() );
        return( node );
        break;
    case KW_RETURN:
        GetToken();
        AddAstChild( node, ParseStatementReturn() );
        return( node );
        break;
    case KW_LABEL:
        GetToken();
        AddAstChild( node, ParseStatementLabel() );
        return( node );
        break;
    case KW_IF:
        GetToken();
        AddAstChild( node, ParseStatementIf() );
        return( node );
        break;
    case KW_GOTO:
        GetToken();
        AddAstChild( node, ParseStatementGoto() );
        return( node );
        break;
    case KW_WHILE:
        GetToken();
        AddAstChild( node, ParseStatementWhile() );
        return( node );
        break;
    case KW_SWITCH:
        GetToken();
        AddAstChild( node, ParseStatementSwitch() );
        return( node );
        break;
    case KW_BOOL:
    case KW_CHAR:
    case KW_FLOAT:
    case KW_INT:
    case KW_UNTYPED:
        AddAstChild( node, ParseDeclarationBlock() );
        if( token != SEMICOLON )
        {
            RemoveAstNode( node );
            SYNC_ERROR( "; expected after declaration" );
        }
        GetToken();
        return( node );
        break;
    default: /* this is the remained, we already checked the first set. */
        AddAstChild( node, ParseExpression() );
        if( token != SEMICOLON )
        {
            RemoveAstNode( node );
            SYNC_ERROR( "; expected after immediate expression" );
        }
        GetToken();
        return( node );
        break;
    }
}


TreeNode *ParseBlock()
{
    TreeNode *node;
    TreeNode *result;

    MAKEFIRST( LBRACE );
    MAKEFOLLOW( SET_TYPES, SET_LITERALS, SET_INSTRUCTIONS,
                SET_UNARIES, 0, KW_START, IDENTIFIER, LBRACE,
                RBRACE, LPAREN );
    SYNC( "a code block must start with {" );

    node = CreateAstNode( NODE_BLOCK, lineCount );

    GetToken();

    while( token != RBRACE  )
    {
        if( token == LBRACE )
        {
            result = ParseBlock();
            AddAstChild( node, result );
        }
        else
        {
            result = ParseStatement();
            AddAstChild( node, result );
        }

	if( result == NULL ) break;
    }
    GetToken();

    return( node );
}

TreeNode *ParseDimensionBlock()
{
    TreeNode *node;
    int count = 0;
    MAKEFOLLOW( SEMICOLON, COMMA, ARROW );

    node = CreateAstNode( NODE_DIMENSIONBLOCK, lineCount );

    while( token == LBRACKET )
    {
        GetToken();
        if( token != RBRACKET )
        {
            RemoveAstNode( node );
            SYNC_ERROR( "] expected after [ in parameter dimension" );
        }
        GetToken();
        // AddAstChild( node, CreateAstNode( NODE_DIMENSION, lineCount ) );
        count++;
    }

    TOAST( node )->val.uintvalue = count;

    return( node );
}

TreeNode *ParseReturnType()
{
    TreeNode *node;
    
    MAKEFIRST( SET_TYPES );
    MAKEFOLLOW( SEMICOLON, LBRACE );
    SYNC( "return type must start with a type name" );

    node = CreateAstNode( NODE_RETURNTYPE, lineCount );

    /* Add type name as child. */
    AddAstChild( node, CreateAstNode( GetNodeFromToken(token), lineCount ) );
    GetToken();

    AddAstChild( node, ParseReference() );
    AddAstChild( node, ParseDimensionBlock() );
    
    return( node );
}

TreeNode *ParseParam( int typeToken )
{
    TreeNode *node;

    MAKEFIRST( OP_MULTIPLY, IDENTIFIER );
    MAKEFOLLOW( COMMA, SEMICOLON, ARROW );
    SYNC( "variable identifier or * expected in formal parameter" );

    node = CreateAstNode( NODE_PARAM, lineCount );

    /* Add type as a child. */
    AddAstChild( node, CreateAstNode( GetNodeFromToken(typeToken), lineCount ) );

    /* Add reference block as a child. */
    AddAstChild( node, ParseReference() );

    /* expect identifier and add as child. */
    if( token != IDENTIFIER )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "identifier expected in formal parameter list" );
    }
    AddAstChild( node, CreateAstNodeVal( NODE_LIT_IDENTIFIER, tokenvalue, lineCount ) );
    GetToken();

    /* Add dimension block as child. */
    AddAstChild( node, ParseDimensionBlock() );

    return( node );
}  

TreeNode *ParseParamList()
{
    BOOL loop;
    TreeNode *node;
    int typeToken;

    MAKEFIRST( SET_TYPES, KW_VOID );
    MAKEFOLLOW( ARROW );
    SYNC( "void or a type name expected in formal parameter list" );

    node = CreateAstNode( NODE_PARAMLIST, lineCount );

    if( token == KW_VOID )
    {
        AddAstChild( node, CreateAstNode( NODE_VOID, lineCount ) );
        GetToken();
        return( node );
    }

    do
    {
        /* Read type token and store it for later use. */
        typeToken = token;
        GetToken();
	
        do
        {
            AddAstChild( node, ParseParam( typeToken ) );
            loop = ( token == COMMA );
            if( loop ) GetToken();
        }
        while( loop );

        // AddAstChild( node, ParseParamBlock() );
        loop = ( token == SEMICOLON );
        if( loop ) GetToken();
    }
    while( loop );

    return( node );
}

TreeNode *ParseModifiers()
{
    TreeNode *node;

    node = CreateAstNode( NODE_MODIFIERS, lineCount );

    while( token == KW_START )
    {
        AddAstChild( node, CreateAstNode( GetNodeFromToken(token), lineCount ) );
        GetToken();
    }

    return( node );
}

TreeNode *ParseFunctionHeader()
{
    TreeNode *headerNode;
    MAKEFIRST( KW_EXTERN, KW_START, IDENTIFIER );
    MAKEFOLLOW( SEMICOLON, LBRACE );
    SYNC( "start or function identifier expected" );

    headerNode = CreateAstNode( NODE_FUNCTIONHEADER, lineCount );
    
    AddAstChild( headerNode, ParseModifiers() );

    // read function identifier.
    if( token != IDENTIFIER )
    {
        RemoveAstNode( headerNode );
        SYNC_ERROR( "function identifier expected" );
    }
    AddAstChild( headerNode, CreateAstNodeVal( NODE_LIT_IDENTIFIER, tokenvalue, lineCount ) );
    GetToken();

    // read colon.
    if( token != COLON )
    {
        RemoveAstNode( headerNode );
        SYNC_ERROR( ": expected after function identifier" );
    }
    GetToken();

    AddAstChild( headerNode, ParseParamList() );

    // read arrow.
    if( token != ARROW )
    {
        RemoveAstNode( headerNode );
        SYNC_ERROR( "-> expected after formal parameter list" );
    }
    GetToken();

    // read return type.
    if( token == KW_VOID )
    {
        AddAstChild( headerNode, CreateAstNode( NODE_VOID, lineCount ) );
        GetToken();
    }
    else
    {
        AddAstChild( headerNode, ParseReturnType() );
    }

    return( headerNode );
}

TreeNode *ParseFunction()
{
    TreeNode *node;

    MAKEFIRST( KW_START, IDENTIFIER );
    MAKEFOLLOW( KW_START, IDENTIFIER, 0 ); /* follow set includes EOF. */
    SYNC( "start or function identifier expected" );

    node = CreateAstNode( NODE_FUNCTION, lineCount );
    
    AddAstChild( node, ParseFunctionHeader() );

    if( token == SEMICOLON )
    {
        // This is a function forwarding declaration.
        GetToken();
    }
    else if( token == LBRACE )
    {
        // This is a function definition. Code block follows.
        AddAstChild( node, ParseBlock() );
    }
    else
    {
        RemoveAstNode( node );
        SYNC_ERROR( "Either ; or code block expected after function header" );
    }

    return( node );
}

TreeNode *ParseGlobal()
{
    TreeNode *node;

    MAKEFIRST( SET_TYPES, KW_EXTERN, KW_START, IDENTIFIER );
    MAKEFOLLOW( SET_TYPES, KW_EXTERN, KW_START, IDENTIFIER );
    SYNC( "global variable or function declaration expected" );

    node = CreateAstNode( NODE_GLOBAL, lineCount );

    if( token == KW_EXTERN )
    {
        AddAstChild( node, CreateAstNode( GetNodeFromToken(token), lineCount ) );
        GetToken();
    }

    /* NOT ELSE IF! */
    if( token == KW_START || token == IDENTIFIER )
    {
        AddAstChild( node, ParseFunction() );
    }
    
    else if( token == KW_BOOL || token == KW_CHAR || token == KW_FLOAT
          || token == KW_INT || token == KW_UNTYPED )
    {
        AddAstChild( node, ParseDeclarationBlock() );
        if( token != SEMICOLON )
        {
            RemoveAstNode( node );
            SYNC_ERROR( "; expected to terminate global variable declaration" );
        }
        GetToken();
    }

    else
    {
	SYNC_ERROR( "function or global variable declaration expected in global scope" );
    }

    return( node );
}


TreeNode *ParseModule()
{
    TreeNode *node;
    TreeNode *child;

    MAKEFIRST( KW_MODULE );
    MAKEFOLLOW( 0 ); /* Follow set includes EOF */
    SYNC( "'module' expected" );

    GetToken();

    if( token != IDENTIFIER ) SYNC_ERROR( "module name expected" );

    node = CreateAstNode( NODE_MODULE, lineCount );

    GetToken();
    if( token != SEMICOLON )
    {
        RemoveAstNode( node );
        SYNC_ERROR( "; expected after module name" );
    }

    GetToken();
    while( token != 0 )
    {
	child = ParseGlobal();
	if( child != NULL )
	{
	    AddAstChild( node, child );
	}
    }

    return( node );
}


TreeNode *Parse()
{
    TreeNode *root;

    /* Initialize the line and character count. */
    lineCount = LINECOUNTBASE;
    charPos = CHARPOSBASE;

    /* Get first token and start parsing. */
    GetToken();
    root = ParseModule();

    if( GetErrorCount() == 0 )
    {
        /* Simplify the AST. */
        /* Remove some nodes from AST. */
        SimplifyAst( root );
    }

    /* NOTE: If #errors > 0, do not compile. */
    return( root );
}
