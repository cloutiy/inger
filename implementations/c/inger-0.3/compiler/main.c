/************************************************
*                                               *
*  Module: main.c                               *
*  Description:                                 *
*      Main program for compiler.               *
*  Author: Van Oostenrijk, A.C.                 *
*  Modifications:                               *
*    [JWH] Added lvalue and param count check   *
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
************************************************/

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "options.h"
#include "ast.h"
#include "preprocessor.h"
#include "tokens.h"
#include "getsymbols.h"
#include "symtab.h"
#include "tree.h"
#include "codegen.h"
#include "typechecking.h"
#include "lvalue.h"
#include "funcparams.h" 
#include "errors.h"
#include "switchcheck.h"
#include "returncheck.h"

/* File to write output code to. */
extern FILE *g_outFile;    
extern int numErrors;    /* from lexer.l */
char outFilename[100];
char *sourceFilename;


/*****************************************************
 *                                                   *
 *                     GLOBALS                       *
 *                                                   *
 *****************************************************/

/* Abstract Syntrax Tree created by the compiler. */
static TreeNode *ast;

extern FILE* yyin;



/* Entry of program. */
int main( int argc, char **argv )
{
    char *preprocessorFilename;
    BOOL result;
    FILE *fp;
    int i;

    /* Parse command line options, and count number of input files. */
    if( ParseOptions( argc, argv ) == 0 ) return( 0 );

    /* Make source file name globaly available */
    sourceFilename = (char *) malloc( strlen( argv[optind] ) );
    strcpy( sourceFilename, argv[optind] );

    /* Process input files */
    while( optind < argc )
    {
        DEBUG( "Source file: %s\n", argv[optind] );

        /* Copy input file name into new memory, and add
         * .p extension.
         */
        preprocessorFilename = (char *) malloc( strlen( argv[optind] ) + 3 );
        strcpy( preprocessorFilename, argv[optind] );
        strcat( preprocessorFilename, "_p" );

        /* Call the preprocessor. It will store its result in
         * preprocessorFilename. If the preprocessor could not
         * open the input file, skip this file.
         */
        result = Preprocess( argv[optind], preprocessorFilename );
        if( result == FALSE )
        {
            free( preprocessorFilename );
            ++optind;
            continue;
        }

        /* Open the preprocessor's file.
         */
        fp = fopen( preprocessorFilename, "rt" );
        if ( fp != NULL )
        {
            /* Set the input of the lexer to the current preprocessed file. */
            yyin = fp;

            /* Reset error/warning report. */
            InitializeReport();

            /* Parse the file and create the abstract syntax tree. */
            ast = Parse();

            /* Process ONLY if no parse errors occurred. */
            if( GetErrorCount() == 0 )
            {
                /* Create the symbol table. */
                CreateSymbolTable( ast );
		if( WantSymbolTable() == TRUE )
		{
		    DumpSymbolTable();
		}

                if( GetErrorCount() == 0 )
                {

                    /* Semantic checks */
                    CheckLeftValues( ast );
                    CheckArgCount( ast );
                    CheckSwitchStatements( ast );
                    CheckFunctionReturns( ast );

                    if( GetErrorCount() == 0 )
                    {
                        DecorateAstWithTypes( ast );
                    }
    		    
                    if( GetErrorCount() == 0 )  
                    {
                        /* Dump AST to console. */
                        if( WantAstDump() == TRUE )
                        {
                            if( GetErrorCount() > 0 )
                            {
                                fprintf( stdout, "There were errors; the AST may not be correct.\n" );
                            }
                            PrintAst( ast );
                        }

                        /* Dump AST to file. */
                        if( WantAstFileDump() == TRUE )
                        {
                            DumpAst( GetAstFilename(), ast );
                        }

                        /* Construct output file name. */
                        for( i = strlen( argv[optind] ) - 1; i >= 0; i-- )
                        {
                            if( argv[optind][i] == '.' ) break;
                        }
    
                        strncpy( outFilename, argv[optind], i );
                        strcat( outFilename, ".s" );

                        /* Open output file for writing. */
                        g_outFile = fopen( outFilename, "w" );
                        if( g_outFile == NULL )
                        {
                            printf( "Error: could not open %s for writing.\n", outFilename );
                            exit(1);
                        }

                        /* Generate some code. */
                        GenerateCode( ast );
                        
                        /* Close output file. */
                        fclose( g_outFile );
                    }
                }
            }

            /* Print errors and warnings. */
            PrintReport();
    
            /* Done, close the input file. */
            fclose( fp );
        }
        else
        {
            printf( "Error: %s not found.\n", preprocessorFilename );
        }

        free( preprocessorFilename );
        optind++;
    }

    /* TODO: Make a better return value for the shell. */
    return( 0 );
}
