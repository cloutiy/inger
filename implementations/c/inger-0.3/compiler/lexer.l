/*************************************************
 *                                               *
 *  Module: lexer.l                              *
 *  Description:                                 *
 *      Tokenizes source input and returns token *
 *      to parser on request.                    *
 *  Authors: Benders, F.J.F, Meffert, D.         *
 *           Van Oostenrijk, A.C.                *
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

/*!LEXERL*/
%{

/* Include stdlib for string to number conversion routines. */
#include <stdlib.h>
/* Include errno for errno system variable. */
#include <errno.h>
/* Include string.h to use strtoul(). */
#include <string.h>
/* include assert.h for assert macro. */
#include <assert.h>
/* Include global definitions. */
#include "defs.h"
/* The token #defines are defined in tokens.h. */
#include "tokens.h"
/* Include error/warning reporting module. */
#include "errors.h"
/* Include option.h to access command line option. */
#include "options.h"

/*
 *
 * MACROS
 *
 */
#define INCPOS charPos += yyleng;


/*
 *
 * FORWARD DECLARATIONS
 *
 */
char SlashToChar( char str[] );
void AddToString( char c );

/*
 *
 * GLOBALS
 *
 */

/*
 * Tokenvalue (declared in tokens.h) is used to pass
 * literal token values to the parser.
 */
Tokenvalue tokenvalue;

/*
 * lineCount keeps track of the current line number
 * in the source input file.
 */
int lineCount;

/*
 * charPos keeps track of the current character
 * position on the current source input line.
 */
int charPos;

/* 
 * Counters used for string reading
 */
static int stringSize, stringPos;

/* 
 * commentsLevel keeps track of the current
 * comment nesting level, in order to ignore nested
 * comments properly.
 */
static int commentsLevel = 0;


%}

 /*
  *
  * LEXER STATES
  *
  */

/* Exclusive state in which the lexer ignores all input
   until a nested comment ends. */
%x STATE_COMMENTS
/* Exclusive state in which the lexer returns all input
   until a string terminates with a double quote. */
%x STATE_STRING


%pointer

 /*
  *
  * REGULAR EXPRESSIONS
  *
  */
%%

 /*
  *
  * KEYWORDS
  *
  */

start                    { INCPOS; return KW_START; }

bool                     { INCPOS; return KW_BOOL; }
char                     { INCPOS; return KW_CHAR; }
float                    { INCPOS; return KW_FLOAT; }
int                      { INCPOS; return KW_INT; }
untyped                  { INCPOS; return KW_UNTYPED; }
void                     { INCPOS; return KW_VOID; }

break                    { INCPOS; return KW_BREAK; }
case                     { INCPOS; return KW_CASE; }
default                  { INCPOS; return KW_DEFAULT; }
do                       { INCPOS; return KW_DO; }
else                     { INCPOS; return KW_ELSE; }
extern                   { INCPOS; return KW_EXTERN; }
goto_considered_harmful  { INCPOS; return KW_GOTO; }
if                       { INCPOS; return KW_IF; }
label                    { INCPOS; return KW_LABEL; }
module                   { INCPOS; return KW_MODULE; }
return                   { INCPOS; return KW_RETURN; }
switch                   { INCPOS; return KW_SWITCH; }
while                    { INCPOS; return KW_WHILE; }


 /*
  *
  * OPERATORS
  *
  */

"->"                     { INCPOS; return ARROW; }
"=="                     { INCPOS; return OP_EQUAL; }
"!="                     { INCPOS; return OP_NOTEQUAL; }
"&&"                     { INCPOS; return OP_LOGICAL_AND; }
"||"                     { INCPOS; return OP_LOGICAL_OR; }
">="                     { INCPOS; return OP_GREATEREQUAL; }
"<="                     { INCPOS; return OP_LESSEQUAL; }
"<<"                     { INCPOS; return OP_BITWISE_LSHIFT; }
">>"                     { INCPOS; return OP_BITWISE_RSHIFT; }
"+"                      { INCPOS; return OP_ADD; }
"-"                      { INCPOS; return OP_SUBTRACT; }
"*"                      { INCPOS; return OP_MULTIPLY; }

"/"                      { INCPOS; return OP_DIVIDE; }
"!"                      { INCPOS; return OP_NOT; }
"~"                      { INCPOS; return OP_BITWISE_COMPLEMENT; }
"%"                      { INCPOS; return OP_MODULUS; }
"="                      { INCPOS; return OP_ASSIGN; }

">"                      { INCPOS; return OP_GREATER; }
"<"                      { INCPOS; return OP_LESS; }
"&"                      { INCPOS; return OP_BITWISE_AND; }
"|"                      { INCPOS; return OP_BITWISE_OR; }

"^"                      { INCPOS; return OP_BITWISE_XOR; }
"?"                      { INCPOS; return OP_TERNARY_IF; }


 /*
  *
  * DELIMITERS
  *
  */

"("                      { INCPOS; return LPAREN; }
")"                      { INCPOS; return RPAREN; }
"["                      { INCPOS; return LBRACKET; }
"]"                      { INCPOS; return RBRACKET; }
":"                      { INCPOS; return COLON; }
";"                      { INCPOS; return SEMICOLON; }
"{"                      { INCPOS; return LBRACE; }
"}"                      { INCPOS; return RBRACE; }
","                      { INCPOS; return COMMA; }


 /*
  *
  * VALUE TOKENS
  *
  */

true       { /* boolean constant */
             INCPOS;
             tokenvalue.boolvalue = TRUE;
             return( LIT_BOOL );
           }
           
false      { /* boolean constant */
             INCPOS;
             tokenvalue.boolvalue = FALSE;
             return( LIT_BOOL );
           }
           
[0-9]+     { /* decimal integer constant */
             INCPOS;
             tokenvalue.uintvalue = strtoul( yytext, NULL, 10 );
             if( tokenvalue.uintvalue == -1 )
             {
                 tokenvalue.uintvalue = 0;
                 AddPosWarning( "integer literal value "
                       "too large. Zero used",
                       lineCount, charPos );
             }
             return( LIT_INT );
           }
           
"0x"[0-9A-Fa-f]+ { 
             /* hexidecimal integer constant */
             INCPOS;
             tokenvalue.uintvalue = strtoul( yytext, NULL, 16 );
             if( tokenvalue.uintvalue == -1 )
             {
                 tokenvalue.uintvalue = 0;
                 AddPosWarning( "hexadecimal integer literal value "
                                "too large. Zero used",
                                lineCount, charPos );
             }
             return( LIT_INT );
           }
           
[0-1]+[Bb] { /* binary integer constant */
             INCPOS;
             tokenvalue.uintvalue = strtoul( yytext, NULL, 2 );
             if( tokenvalue.uintvalue == -1 )
             {
                 tokenvalue.uintvalue = 0;
                 AddPosWarning( "binary integer literal value too "
                                "large. Zero used" ,
                                lineCount, charPos );
             }
             return( LIT_INT );
           }
             
[_A-Za-z]+[_A-Za-z0-9]* { 
 	       /* identifier */
               INCPOS;
               tokenvalue.identifier = strdup( yytext );
               return( IDENTIFIER );
             }
             
[0-9]*\.[0-9]+([Ee][+-]?[0-9]+)? { 
               /* floating point number */
               INCPOS;
               if( sscanf( yytext, "%f",
					&tokenvalue.floatvalue ) == 0 )
               {
                 tokenvalue.floatvalue = 0;
                 AddPosWarning( "floating point literal value too "
                                "large. Zero used",
                                lineCount, charPos );
               }
               return( LIT_FLOAT );
             }
               


 /*
  *
  * CHARACTERS
  *
  */

\'\\[\'\"abfnrtv]\' {
                 INCPOS;
                 yytext[strlen(yytext)-1] = '\0';
                 tokenvalue.charvalue =
					SlashToChar( yytext+1 );
                 return( LIT_CHAR );
               }

\'\\B[0-1][0-1][0-1][0-1][0-1][0-1][0-1][0-1]\' {
                 /* \B escape sequence. */
                 INCPOS;
                 yytext[strlen(yytext)-1] = '\0';
                 tokenvalue.charvalue =
					SlashToChar( yytext+1 );
                 return( LIT_CHAR );
               }

\'\\o[0-7][0-7][0-7]\' {
                 /* \o escape sequence. */
                 INCPOS;
                 yytext[strlen(yytext)-1] = '\0';
                 tokenvalue.charvalue =
					SlashToChar( yytext+1 );
                 return( LIT_CHAR );
               }

\'\\x[0-9A-Fa-f][0-9A-Fa-f]\' {
                 /* \x escape sequence. */
                 INCPOS;
                 yytext[strlen(yytext)-1] = '\0';
                 tokenvalue.charvalue =
					SlashToChar( yytext+1 );
                 return( LIT_CHAR );
               }

\'.\'          {
                 /* Single character. */
                 INCPOS;
                 tokenvalue.charvalue = yytext[1];
                 return( LIT_CHAR );
              }



 /*
  *
  * STRINGS
  *
  */

\"             { INCPOS;
                 tokenvalue.stringvalue =
                 (char*) malloc( STRING_BLOCK );
                 memset( tokenvalue.stringvalue,
				    0, STRING_BLOCK );
                 stringSize = STRING_BLOCK;
                 stringPos = 0;
                 BEGIN STATE_STRING; /* begin of string */
               }
               
<STATE_STRING>\" { 
                 INCPOS;
                 BEGIN 0;
                 /* Do not include terminating " in string */
                 return( LIT_STRING ); /* end of string */
               }
               
<STATE_STRING>\n { 
                 INCPOS;
                 AddPosWarning( "strings cannot span multiple "
                                "lines", lineCount, charPos );
                 AddToString( '\n' );
               }
               
<STATE_STRING>\\[\'\"abfnrtv] { 
                 /* Escape sequences in string. */
                 INCPOS; 
                 AddToString( SlashToChar( yytext ) );
               }

<STATE_STRING>\\B[0-1][0-1][0-1][0-1][0-1][0-1][0-1][0-1] {
                 /* \B escape sequence. */
                 INCPOS;
                 AddToString( SlashToChar( yytext ) );
               }

<STATE_STRING>\\o[0-7][0-7][0-7] {
                 /* \o escape sequence. */
                 INCPOS;
                 AddToString( SlashToChar( yytext ) );
               }

<STATE_STRING>\\x[0-9A-Fa-f][0-9A-Fa-f] {
                 /* \x escape sequence. */
                 INCPOS;
                 AddToString( SlashToChar( yytext ) );
               }

<STATE_STRING>. { 
                 /* Any other character */
                 INCPOS;
                 AddToString( yytext[0] );
               }


 /*
  *
  * LINE COMMENTS
  *
  */

"//"[^\n]*     { ++lineCount; /* ignore comment lines */ }


 /*
  *
  * BLOCK COMMENTS
  *
  */

"/*"           { INCPOS;
                 ++commentsLevel;
                 BEGIN STATE_COMMENTS;
				 /* start of comments */
               }
               
<STATE_COMMENTS>"/*" {
                INCPOS;
                ++commentsLevel; 
                /* begin of deeper nested
				   comments */
              }
              
<STATE_COMMENTS>. { INCPOS; /* ignore all characters */ }
              
<STATE_COMMENTS>\n {
                charPos = 0;
                ++lineCount; /* ignore newlines */
              }
              
<STATE_COMMENTS>"*/" { 
                INCPOS;
                if ( --commentsLevel == 0 ) 
                  BEGIN 0; /* end of comments*/
              }


 /*
  *
  * WHITESPACE
  *
  */

[\t ]         { ++charPos; /* ignore whitespaces */ }

\n            { ++lineCount;
                charPos = 0; /* ignored newlines */
              }
              
              /* unmatched character */
.             { INCPOS; return yytext[0]; }

%%


 /*
  *
  * ADDITIONAL VERBATIM C CODE
  *
  */

/*
 * Convert slashed character (e.g. \n, \r etc.) to a
 * char value.
 * The argument is a string that start with a backslash,
 * e.g. \x2e, \o056, \n, \b11011101
 * 
 * Pre:  (for \x, \B and \o): strlen(yytext) is large
 *       enough. The regexps in the lexer take care
 *       of this.
 */
char SlashToChar( char str[] )
{
    static char strPart[20];

    memset( strPart, 0, 20 );
    
    switch( str[1] )
    {
    case '\\':
        return( '\\' );
    case '\"':
        return( '\"' );
    case '\'':
        return( '\'' );
    case 'a':
        return( '\a' );
    case 'b':
        return( '\b' );
    case 'B':

        strncpy( strPart, str+2, 8 );
        return( strtoul( yytext+2, NULL, 2 ) );
    case 'f':
        return( '\f' );
    case 'n':
        return( '\n' );
    case 'o':
        strncpy( strPart, str+2, 3 );
        return( strtoul( strPart, NULL, 8 ) );
    case 't':
        return( '\t' );
    case 'r':
        return( '\r' );
    case 'v':
        return( '\v' );
    case 'x':
        strncpy( strPart, str+2, 2 );
        return( strtoul( strPart, NULL, 16 ) );
    default:
        /* Control should never get here! */
        assert( 0 );
    }
}


/*
 * For string reading (which happens on a
 * character-by-character basis), add a character to
 * the global lexer string 'tokenvalue.stringvalue'.
 */
void AddToString( char c )
{
    if( tokenvalue.stringvalue == NULL )
    {
        /* Some previous realloc() already went wrong.
         * Silently abort.
         */
        return;
    }

    if( stringPos >= stringSize - 1 )

    {
        stringSize += STRING_BLOCK;
        DEBUG( "resizing string memory +%d, now %d bytes", 
                STRING_BLOCK, stringSize );

        tokenvalue.stringvalue =
			(char*) realloc( tokenvalue.stringvalue,
		    stringSize );
        if( tokenvalue.stringvalue == NULL )
        {
            AddPosWarning( "Unable to claim enough memory "
                           "for string storage",
						   lineCount, charPos );
            return;
        }
        memset( tokenvalue.stringvalue + stringSize
				- STRING_BLOCK, 0, STRING_BLOCK );
    }

    tokenvalue.stringvalue[stringPos] = c;
    stringPos++;
}
/*!*/

