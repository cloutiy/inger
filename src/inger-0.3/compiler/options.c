/*************************************************
 *                                               *
 *  Module: options.c                            *
 *  Description:                                 *
 *      Parses command line options using        *
 *      GNU getopt_long.                         *
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

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <string.h> /* strdup */
#include "defs.h"
#include "options.h"
#include "ast.h"
#include "tree.h"
#include "list.h"
#include "symtab.h"

char *astfile;

/*
 *  option_order contains all the flags that the
 *  program supports.
 */
enum
{
    OPTION_SYMBOLS,
    OPTION_HELP,
    OPTION_TEST,
    OPTION_AST,
    OPTION_ASTFILE,
    OPTION_INTERNAL_DEBUG
} option_order;

/*
 *  Array of 'option' structs used by getopt_long.
 *  Must end with one empy option structure.
 */
static struct option longoptions[] =
{
    { "symbols",    0, 0, OPTION_SYMBOLS },
    { "help",       0, 0, OPTION_HELP },
    { "test",       0, 0, OPTION_TEST },
    { "ast",        0, 0, OPTION_AST },
    { "astfile",    1, 0, OPTION_ASTFILE },  /* has file argument */
    { "debug",      0, 0, OPTION_INTERNAL_DEBUG },
    { 0,0,0,0 }
};

/*
 *  Actual option values (boolean: on or off),
 *  initially set to default values (all off).
 */
BOOL options[] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

/*
 *  Prints help on command line flags and arguments.
 *  Displays [programName] for the program's executable
 *  file name.
 *
 *  Pre: programName is not the NULL string.
 */
void ShowHelp( char *programName )
{
    assert( programName != NULL );

    fprintf( stdout,
      "Usage: %s [options] file... \n" \
      "Options:\n" \
      "-h, --help         Display this information\n" \
      "-s, --symbols      Dump symbol table to console\n" \
      "-t, --test         Perform all sorts of selftests\n" \
      "-a, --ast          Dump abstract syntax tree to console\n" \
      "    --astfile file Dump abstract syntax tree to file\n" \
      "-d, --debug        Output compiler debug information\n" \
      "\n", programName
    );
}

/*
 *  Something went wrong; tell the user how to get help.
 *  Displays [programName] for the program's executable
 *  file name.
 *
 *  Pre: programName is not the NULL string.
 */
void AdviseHelp( char *programName )
{
    assert( programName != NULL );

    fprintf( stdout, "Try '%s --help' for more information.\n",
        programName );
}


/*
 *  Parses command line options. Option values are
 *  stored in the global [options] array. Option values
 *  are accessed by external modules through accessor
 *  functions (below).
 *  This function's parameters are the same as the
 *  parameters for a C program's 'main' function.
 *
 *  Return values:
 *  TRUE  - all options were parsed successfully, and there
 *          is at least one input file specified.
 *  FALSE - an options failed to parse, or there are no input
 *          files specified.
 */
BOOL ParseOptions( int argc, char **argv )
{
    int opt;

    while( ( opt = getopt_long( argc, argv, "shtad?",
        longoptions, NULL ) ) != -1 )
    {
        /*  Note: do not allow -? to be used for help.
         *  This is exactly the character that getopt_long
         *  returns when no option matches.
         */

        if( opt == '?' )
        {
            AdviseHelp( argv[0] );
            return( FALSE );
        }

        /* Convert short option values (chars) to long
           option values (enum). */
        if( opt == 's' ) opt = OPTION_SYMBOLS;
        if( opt == 'h' ) opt = OPTION_HELP;
        if( opt == 't' ) opt = OPTION_TEST;
        if( opt == 'a' ) opt = OPTION_AST;
        if( opt == 'd' ) opt = OPTION_INTERNAL_DEBUG;

        /* Take appropriate action for each option. */
        switch( opt )
        {
        case OPTION_SYMBOLS:
            fprintf( stdout, "--symbols: will dump symbol "
                    "table to console.\n" );
            options[opt] = TRUE;
            break;
        case OPTION_HELP:
            ShowHelp( argv[0] );
            return( FALSE );
            break;
        case OPTION_TEST:
            fprintf( stdout, "--test: will perform selftest "
                "(all other commands ignored).\n" );
            TestList();
            TestTree();
            TestAst();
            TestSymbolTable();
            printf( "[Self test complete]\n" );
            return( FALSE );
            break;
        case OPTION_AST:
            fprintf( stdout, "--ast: will dump abstract "
                "syntax tree to console.\n" );
            options[opt] = TRUE;
            break;
        case OPTION_ASTFILE:
            fprintf( stdout, "--astfile: will dump abstract "
                "syntax tree to file \"%s\".\n", optarg );
            options[opt] = TRUE;
            astfile = strdup( optarg );
            break;
        case OPTION_INTERNAL_DEBUG:
            fprintf( stdout, "--debug: printing internal "
                "debug information.\n" );
            options[opt] = TRUE;
            break;
        default:
            fprintf( stderr, "Warning: option "
                "not implemented.\n" );
        }
    }

    /* No non-flag arguments at all? */
    if( optind >= argc )
    {
        fprintf( stderr,
            "%s: no source file(s) specified.\n", argv[0] );
        AdviseHelp( argv[0] );
        return( FALSE );
    }

    /* All is well. */
    return( TRUE );
}

BOOL WantSymbolTable()
{
    return( options[OPTION_SYMBOLS] == TRUE );
}

BOOL WantAstDump()
{
    return( options[OPTION_AST] == TRUE );
}

BOOL WantAstFileDump()
{
    return( options[OPTION_ASTFILE] == TRUE );
}

char *GetAstFilename()
{
    return( astfile );
}

BOOL WantInternalDebug()
{
    return( options[OPTION_INTERNAL_DEBUG] == TRUE );
}

