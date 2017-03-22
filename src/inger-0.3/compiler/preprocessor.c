/*************************************************
 *                                               *
 *  Module: preprocessor.c                       *
 *  Description:                                 *
 *      Simple preprocessor with only an         *
 *      import statement to include files.       *
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
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "defs.h"


/*************************************************
 *                                               *
 *  MACROS                                       *
 *                                               *
 *************************************************/
#define PREPROCESSOR_PREFIX '#'
#define SKIPDELIMITERS while( IsDelimiter( c = NEXTCHAR ) )
#define SKIPLINE \
        { \
            do \
            {\
                c = NEXTCHAR; \
            } \
            while ( c != '\n' && c != EOF ); \
            return( 0 ); \
        }
#define UNEXPECTED_EOF \
        { \
            printf( "error: unexpected end of file.\n" ); \
            return( EOF ); \
        }
#define NEXTCHAR GetChar(ptr)

/*************************************************
 *                                               *
 *  ERROR MESSAGES                               *
 *                                               *
 *************************************************/
#define ERR_CIRCULAR \
    "warning: circular import reference in %s.\n"
#define ERR_OPEN \
    "error: cannot open import file %s.\n"
#define ERR_MALFORMED_IMPORT \
    "%s:%d - error: string constant expected after %cimport.\n"
#define ERR_FILENAME_TOO_LONG \
    "%s:%d - string constant too large.\n"
#define ERR_NEWLINE_IN_FILENAME \
    "%s:%d - newline in string constant.\n"
#define ERR_STRAY_TOKENS \
    "%s:%d - warning: unexpected tokens following preprocessor \
    directive - expected a newline\n"
#define ERR_UNKNOWN_DIRECTIVE \
    "%s:%d - unknown preprocessor directive '%c%s'\n"



/*************************************************
 *                                               *
 *  TYPES                                        *
 *                                               *
 *************************************************/

/*
 *  ImportFile Struct.
 *  Contains the name of an imported file.
 */
typedef struct ImportFile
{
    char *fileName;
    struct ImportFile *next;
} ImportFile;


/*************************************************
 *                                               *
 *  FORWARD DECLARATIONS                         *
 *                                               *
 *************************************************/

int ProcessLine( char *ptr, FILE *fpOut );


/*************************************************
 *                                               *
 *  GLOBALS                                      *
 *                                               *
 *************************************************/

/*
 *  List to keep track of imported files in order to
 *  prevent multiple imports of the same file.
 */
static ImportFile *importedFiles;

/*
 *  List to keep track of recursively imported files
 *  to prevent cirfular references.
 */
static ImportFile *importedFilesLocal;

/*
 *  Numbers of character read in the current input
 *  file buffer.
 */
static int charsRead;

/*
 *  Size of the current file.
 */
static int fileSize;

/*
 *  Keep track of the line number being preprocessed.
 */
static int lineNumber;

/*
 *  Name of the file currently being preprocessed.
 */
static char *currentFile;

/*************************************************
 *                                               *
 *  FUNCTIONS                                    *
 *                                               *
 *************************************************/

/*
 *  Checks to see if [filename] is already in
 *  linked list [importList].
 *
 *  Pre: [filename] is not the NULL string.
 *
 *  Return values:
 *  TRUE  - [filename] is in [importList].
 *  FALSE - [filename] is not in [importList].
 */
static BOOL IsInImportList(
    char *fileName,
    ImportFile *importList )
{
    ImportFile *itr;

    assert( fileName != NULL );

    /* Iterate through importList, looking for
     * filename. */
    itr = importList;
    while( itr )
    {
        if( strcmp( fileName, itr->fileName ) == 0 )
        {
            return( TRUE );
        }
        itr = itr->next;
    }
    return( FALSE );
}

/*
 *  Adds ImportFile structure [importFile] to
 *  linked list [importList].
 *
 *  Return value: none.
 */
static void ImportListPush(
    ImportFile *importFile,
    ImportFile **importList )
{
    ImportFile *itr;
    
    /* Search for end of the list and add the reference. */
    itr = *importList;
    if( itr )
    {
        while( itr->next )
        {
            itr = itr->next;
        }
        itr->next = importFile;
    }
    else
    {
        *importList = importFile;
    }
}

/*
 *  Removes the lastmost ImportFile structure from
 *  linked list [importList].
 *
 *  Return values:
 *  TRUE  - item removed, associated memory freed.
 *  FALSE - there were no items in the list.
 */
static BOOL ImportListPop( ImportFile **importList )
{
    ImportFile *itr, *prev;

    itr = *importList;
    prev = NULL;

    if( itr != NULL )
    {
        /* Find last element in the list. */
        while( itr->next != NULL )
        {
            prev = itr;
            itr = itr->next;
        }
        /* Free the element's memory. */
        free( itr );

        /* Remove reference, if the item is not the
         * first item. */
        if( prev != NULL )
        {
            prev->next = NULL;
            return( TRUE );
        }
        else
        {
            return( FALSE );
        }
    }

    /* There were no items in the list. */
    else
    {
        return( FALSE );
    }
}

/*
 *  Gets a character from file buffer [buf].
 *
 *  Pre: [buf] is not NULL.
 *  Pre: global charsRead does not exceed the size of [buf].
 *
 *  Return values:
 *  EOF  - End of buffer reached.
 *  !EOF - return value is character read.
 */
static int GetChar( const char *buf )
{
    if( charsRead == fileSize )
    {
        return( EOF );
    }

    assert( buf != NULL );

    return( buf[charsRead++] );
}

/*
 *  Checks to see if character [c] is whitespace
 *  (space or tab).
 *
 *  Return values:
 *  TRUE  - [c] is whitespace.
 *  FALSE - [c] is not whitespace.
 */
static BOOL IsDelimiter( const char c )
{
    return( c == ' ' || c == '\t' ) ? TRUE : FALSE;
}

/*
 *  Opens an import file and copies it in the output file.
 */
static void Import( char *fileName, FILE *fpOut )
{
    FILE *fpIn;
    struct ImportFile *importFile, *tmp;
    int tmpFileSize, tmpCharsRead, tmpLineNumber;
    char *fileBuffer, *tmpFile;

    /* Check for circular reference.
     * If a circular reference is detected, abort the
     * import. */
    if( IsInImportList( fileName, importedFilesLocal ) )
    {
        fprintf( stderr, ERR_CIRCULAR, currentFile );
        return;
    }
    
     /* Has this file already been imported?
      * If it has, abort the import. */
    if( IsInImportList( fileName, importedFiles ) )
    {
        return;
    }

    /* Open file to import for binary reading. */
    fpIn = fopen( fileName, "rb" );

    /* Is it a valid file? If it is not,
     * abort the import. */
    if( fpIn == NULL )
    {
        fprintf( stderr, ERR_OPEN, fileName );
        return;
    }

    /* Create new import file structure for the list. */
    importFile = ( ImportFile * )
        malloc( sizeof( ImportFile ) );
    importFile->fileName = strdup( fileName );
    importFile->next = NULL;
    tmp = importFile;

    /* Put the new entry in the lists. */
    ImportListPush( importFile, &importedFiles );
    importFile = ( ImportFile * )
        malloc( sizeof( ImportFile ) );
    memcpy( importFile, tmp, sizeof( ImportFile) );
    ImportListPush( importFile, &importedFilesLocal );
    
    /* Save this buffer info. */
    tmpFileSize = fileSize;
    tmpCharsRead = charsRead;
    tmpFile = currentFile;
    tmpLineNumber = lineNumber;
    
    /* Get length of input file. */
    fseek( fpIn, 0, SEEK_END );
    fileSize = ftell( fpIn );
    fseek( fpIn, 0, SEEK_SET );

    /* Read the entire file into memory. */
    fileBuffer = ( char * )malloc( fileSize );
    fread( fileBuffer, fileSize, 1, fpIn );

     /* Process all lines. */
    charsRead = 0;
    lineNumber = 0;
    currentFile = strdup( fileName );
    while( ProcessLine( fileBuffer, fpOut ) != EOF );

    /* Close the import file and free the input file buffer. */
    fclose( fpIn );
    free( fileBuffer );

    /* Restore buffer info. */
    fileSize = tmpFileSize;
    charsRead = tmpCharsRead;
    lineNumber = tmpLineNumber;
    free( currentFile );
    currentFile = tmpFile;
    ImportListPop( &importedFilesLocal );
}

/*
 *  Reads a line from the input file and handles it if
 *  it is a preprocessor directive.
 *
 *  Return values:
 *  EOF  - End of file, no more lines to read.
 *  !EOF - There are more lines to read.
 */
int ProcessLine( char *ptr, FILE *fpOut )
{
    char directive[32], fileName[256];
    int c, i;

    /* Read the first character and check to see if
       this line is a preprocessor directive. */
    c = NEXTCHAR;
    if ( c == EOF )
    {
        return( EOF );
    }
    if( c == PREPROCESSOR_PREFIX )
    {
        /* get preprocessor directive. */
        i = 0;
        do
        {
            directive[i++] = NEXTCHAR;
        }
        while( !IsDelimiter( directive[i-1] )
               && i < 31
               && directive[i-1] != EOF );
        directive[i-1] = 0;

        /* Import directive? */
        if( strcmp( directive, "import" ) == 0 )
        {
            SKIPDELIMITERS;

            /* Check for EOF. */
            if( c == EOF )
            {
                UNEXPECTED_EOF;
            }

            /* Check for a valid string constant after
             * the directive. */
            if( c != '\"' )
            {
                fprintf( stderr, ERR_MALFORMED_IMPORT,
                    currentFile, lineNumber,
                    PREPROCESSOR_PREFIX );
                SKIPLINE;
            }

            /* Read the filename in the string constant. */
            i = 0;
            do
            {
                fileName[i++] = NEXTCHAR;
            }
            while( fileName[i-1] != '\"'
                   && i < 255
                   && fileName[i-1] != '\n'
                   && fileName[i-1] != EOF );
            fileName[i] = 0;

            /* Check the string size. */
            if( i == 255 && fileName[i-1] != '\"' )
            {
                fprintf( stderr, ERR_FILENAME_TOO_LONG,
                    currentFile, lineNumber );
                SKIPLINE;
            }

            /* Check for newline in string. */
            if( fileName[i-1] == '\n' )
            {
                fprintf( stderr, ERR_NEWLINE_IN_FILENAME,
                    currentFile, lineNumber );
                SKIPLINE;
            }

            /* Check for EOF in string. */
            if( fileName[i-1] == EOF )
            {
                UNEXPECTED_EOF;
            }

            /* String is valid, import the file. */
            fileName[i-1] = 0;
            Import( fileName, fpOut );

            /* Warn about any tokens after the string. */
            SKIPDELIMITERS;
            if( c != '\n' && c != EOF )
            {
                fprintf( stderr, ERR_STRAY_TOKENS,
                     currentFile, lineNumber );
                SKIPLINE;
            }
            return( 0 );
        }

        /* Unknown directive, display error and continue. */
        else
        {
            fprintf( stderr, ERR_UNKNOWN_DIRECTIVE,
                currentFile, lineNumber,
                PREPROCESSOR_PREFIX, directive );
            SKIPLINE;
        }
    }

    /* Empty line? */
    if( c == '\n' )
    {
        fputc( c, fpOut );
        return( 0 );
    }

    /* The current line contains no preprocessor directive.
       Just copy it to the output file. */
    do
    {
        fputc( c, fpOut );
        c = NEXTCHAR;
        if ( c == EOF )
        {
            return( EOF );
        }
    }
    while( c != '\n' );

    fputc( c, fpOut );

    return( 0 );
}

/*
 *  Preprocesses input file [fileIn] and writes the result
 *  to output file [fileOut].
 *
 *  Pre: [fileIn] is not the NULL string.
 *  Pre: [fileOut] is not the NULL string.
 *
 */
BOOL Preprocess( char *fileIn, char *fileOut )
{
    FILE *fpIn, *fpOut;
    char *fileBuffer;
    int sysResult;

    assert( fileIn != NULL );
    assert( fileOut != NULL );

    /* Initialize linked list of imported file names. */
    importedFiles = NULL;

    /* Open input file for binary reading. */
    fpIn = fopen( fileIn, "rb" );
    if( fpIn == NULL )
    {
        perror( fileIn );
        return( FALSE );
    }

    /* Get length of input file. */
    sysResult = fseek( fpIn, 0L, SEEK_END );
    if( sysResult != 0 )
    {
        perror( fileIn );
        fclose( fpIn );
        return( FALSE );
    }
    fileSize = ftell( fpIn );
    if( fileSize == -1 )
    {
        perror( fileIn );
        fclose( fpIn );
        return( FALSE );
    }
    sysResult = fseek( fpIn, 0L, SEEK_SET );
    if( sysResult != 0 )
    {
        perror( fileIn );
        fclose( fpIn );
        return( FALSE );
    }

    /* Read the entire file in memory. */
    fileBuffer = (char * ) malloc( fileSize );
    sysResult = fread( fileBuffer, fileSize, 1, fpIn );
    if( sysResult == 0 )
    {
        perror( fileIn );
        free( fileBuffer );
        fclose( fpIn );
        return( FALSE );
    }

    /* Close the input file. */
    fclose( fpIn );

    /* Create output file. */
    fpOut = fopen( fileOut, "wb" );
    if( fpOut == NULL )
    {
        perror( fileOut );
        free( fileBuffer );
        return( FALSE );
    }

    /* Allocate global buffer to hold input file name. */
    currentFile = strdup( fileIn );

    /* Process all lines. */
    charsRead = 0;
    lineNumber = 0;
    while( ProcessLine( fileBuffer, fpOut ) != EOF );

    /* Clean up. */
    fclose( fpOut );
    free( fileBuffer );
    free( currentFile );
    while( ImportListPop( &importedFiles ) );

    return( TRUE ); 
}

