/*************************************************
 *                                               *
 *  Module: errors.c                             *
 *  Description:                                 *
 *      Errors reporting module. This module     *
 *      collects error/warning messages in a     *
 *      list, marking them with the line number  *
 *      on which they occurred. They can then    *
 *      be sorted and printed as a batch.        *
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

/* printf, NULL */
#include <stdio.h>
/* malloc, free */
#include <stdlib.h>
/* assert macro */
#include <assert.h>
/* General macros. */
#include "defs.h"
/* Include linked list interface. */
#include "list.h"

/*************************************************
 *                                               *
 *  MACROS                                       *
 *                                               *
 *************************************************/

/* 
 * Define the character used to terminate messages. Can be
 * the empty string.
 */
#define MESSAGEDOT "."

/*************************************************
 *                                               *
 *  TYPES                                        *
 *                                               *
 *************************************************/

/*
 *  This structure holds a single error/warning
 *  message (a string) and the line number on which
 *  it occurred.
 */
typedef struct
{
    int lineno;
    char *text;
} ReportEntry;

/*************************************************
 *                                               *
 *  GLOBALS                                      *
 *                                               *
 *************************************************/

static int numErrors = 0, numWarnings = 0;
static List *report = NULL;


/*************************************************
 *                                               *
 *  FUNCTION DEFNIITIONS                         *
 *                                               *
 *************************************************/

/*
 *  Free specified instance of the ReportEntry
 *  structure. This callback function is used
 *  by [report], the list of Report Entries.
 */
static BOOL DeleteReportEntry( void *entry )
{
    free( ( (ReportEntry *) entry)->text );
    free( entry );
    return( TRUE );
}

/*
 * Add a ReportEntry to the report, with the
 * specified [prefix] ("error" or "warning"),
 * the message [message] and the line number
 * [lineno]. Include the character position
 * [charpos] if it is not -1.
 */
static void AddText( char *prefix, char *message, int lineno, int charpos )
{
    ReportEntry *entry;

    assert( prefix != NULL );
    assert( message != NULL );

    entry = (ReportEntry*) malloc( sizeof( ReportEntry ) );
    entry->text = (char*) malloc( strlen(message) + 40 );

    if( charpos == -1 )
    {
        sprintf( entry->text, "%d - %s: %s%s", lineno, prefix,
            message, MESSAGEDOT );
        /* DEBUG( "%d - %s: %s%s\n", lineno, prefix,
            message, MESSAGEDOT ); */
    }
    else
    {
        sprintf( entry->text, "%d:%d - %s: %s%s", lineno,
            charpos, prefix, message, MESSAGEDOT );
        /* DEBUG( "%d:%d - %s: %s%s\n", lineno,
            charpos, prefix, message, MESSAGEDOT ); */
    }

    entry->lineno = lineno;
    ListAppend( report, entry );
}

void AddPosError( char *message, int lineno, int charpos )
{
    numErrors++;
    AddText( "error", message, lineno, charpos );
}

void AddError( char *message, int lineno )
{
    numErrors++;
    AddText( "error", message, lineno, -1 );
}

void AddPosWarning( char *message, int lineno, int charpos )
{
    numWarnings++;
    AddText( "warning", message, lineno, charpos );
}

void AddWarning( char *message, int lineno )
{
    numWarnings++;
    AddText( "warning", message, lineno, -1 );
}

int GetErrorCount()
{
    return( numErrors );
}

int GetWarningCount()
{
    return( numWarnings );
}

void InitializeReport()
{
    numErrors = 0;
    numWarnings = 0;

    if( report != NULL )
    {
        ListPurge( report, DeleteReportEntry );
    }
    report = ListInit( DeleteReportEntry );
}

void PrintReport()
{
    List *sortedReport;
    int lowestLine;
    int targetEntry;
    int i, lineno;
    void *entry;
    ListNode *node;

    sortedReport = ListInit( DeleteReportEntry );

    /*
     * Remove entries from report and put them
     * in sortedReport (sorted).
     */
    while( ListSize( report ) > 0 )
    {
        lowestLine = 1 << 30;
        targetEntry = -1;

        /* Find lowest line number */
        ListFirst( report );
        for( i = 0; i < ListSize( report ); i++ )
        {
            lineno = ( ( ReportEntry* ) ListGet( report ) )->lineno;
            if( lineno < lowestLine )
            {
                targetEntry = i;
                lowestLine = lineno;
            }
            ListNext( report );
        }

        /* Go to lowest entry. */
        ListFirst( report );
        for( i = 0; i < targetEntry; i++ )
        {
            ListNext( report );
        }

        /* Remove entry from report, add to sortedReport */
        entry = ListGet( report );
        ListUnlink( report );
        ListAppend( sortedReport, entry );
    }
    ListPurge( report, DeleteReportEntry );

    /* Print sortedReport. */
    node = ListFirstEx( sortedReport );
    while( node != NULL )
    {
        fprintf( stderr, "%s\n", ((ReportEntry*)node->data)->text );
        node = ListNextEx( node );
    }

    /* Throw away list and contents. */
    ListPurge( sortedReport, DeleteReportEntry );

    fprintf( stdout, "%d errors, %d warnings.\n", GetErrorCount(), GetWarningCount() );
}

