/************************************************
*                                               *
*  Module: switchcheck.c                        *
*  Description:                                 *
*      Check switch statements for duplicate    *
*      case values.                             *
*  Author: Haaring, J.W.                        *
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
************************************************/

/* malloc need this */
#include <stdlib.h>
/* Include assertion macro. */
#include <assert.h>
/* Need strcmp */
#include <string.h>
/* Standard macros. */
#include "defs.h"
/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"
/* Include func param interface */
#include "switchcheck.h"
/* Include for AddError() function. */
#include "errors.h"

extern int numErrors;

List * switchList;
List * caseList;

BOOL CheckSwitchStatements( TreeNode * ast )
{
    ListNode *listNode;
    TreeNode *switchNode;

    switchList = ListInit( NULL );

    /*
     * Get a list of all switch nodes
    */
    SearchSwitches( ast );

    listNode = ListFirstEx( switchList );
    while( listNode != NULL  )
    {
        switchNode = (TreeNode *) listNode->data;
        caseList = ListInit( NULL );
        AnalyseSwitch( switchNode );
        CheckSwitchStatement( switchNode ) ;
        ListClear( caseList );
	    listNode = ListNextEx( listNode );
    }

    /*
     * Clear the list with switch nodes
    */
    ListClear ( switchList );

    return( TRUE );
}



void SearchSwitches( TreeNode * switchNode )
{
    ListNode * listNode;
    AstNode * astNode = (AstNode *) switchNode->data;
    BOOL bAppendResult = FALSE;

    assert( switchNode != NULL );
    assert( astNode != NULL );

    if( astNode->id == NODE_SWITCH )
    {
        /* look for switch nodes and put them in a list */
        bAppendResult = ListAppend( switchList, switchNode );
        assert( bAppendResult == TRUE );
    }

    listNode = ListFirstEx( switchNode->children );
    while( listNode != NULL )
    {
        /* Get all symbols from this child node */
        SearchSwitches( (TreeNode *) listNode->data );
        listNode = ListNextEx( listNode );
    }
}


void AnalyseSwitch( TreeNode * switchNode )
{

    ListNode * listNode;
    AstNode * astNode = (AstNode *) switchNode->data;
    BOOL bAppendResult = FALSE;

    assert( switchNode != NULL );
    assert( astNode != NULL );

    switch( astNode->id )
    {
        case NODE_CASE:

            /* Case statement found, append the statement to the current switchlist */
            bAppendResult = ListAppend( caseList, switchNode );
            assert( bAppendResult == TRUE );
            break;

        default:
            listNode = ListFirstEx( switchNode->children );
            while( listNode != NULL )
            {
              /* Get all symbols from this child node */
              AnalyseSwitch( (TreeNode *) listNode->data );
              listNode = ListNextEx( listNode );
            }
            break;
    }
}

void CheckSwitchStatement( TreeNode * switchNode )
{
    ListNode * listNode;
    TreeNode * headerNode;

    assert( switchNode != NULL );

    listNode = ListFirstEx( caseList );
    while( listNode != NULL  )
    {
        headerNode = (TreeNode *) listNode->data;
        if( CaseOccurenceCount( TokenvalueToString( 
                        TOAST( headerNode )->id,
                        TOAST( headerNode )->val ) ) != 1  )
        {
            AddError( "duplicate case value",
                        TOAST( headerNode )->lineno );
        }
	    listNode = ListNextEx( listNode );
    }
}


int CaseOccurenceCount( char *TokenValueString )
{
    ListNode * listNode;
    TreeNode * tempNode;
    int duplicateCases = 0;
    char * stringvalue = malloc( sizeof( TokenValueString ) );

    assert( TokenValueString != NULL );
    memcpy( stringvalue, TokenValueString, sizeof( TokenValueString ) );

    listNode = ListFirstEx( caseList );
    while( listNode != NULL  )
    {
        tempNode = (TreeNode *) listNode->data;
        if( strcmp( stringvalue, TokenvalueToString( 
                                  TOAST( tempNode )->id,
                                  TOAST( tempNode )->val ) ) == 0 )
        {
            duplicateCases++;
        }

	    listNode = ListNextEx( listNode );
    }

    free( stringvalue );
    return( duplicateCases );
}
