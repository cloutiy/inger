/************************************************
*                                               *
*  Module: funcparams.c                         *
*  Description:                                 *
*      Check function headers for # of params,  *
*      check for params in function calls       *
*      as well.                                 *
*  Author: Haaring, J.W.                        *
*  Modifications:                               *
*    [JWH] Added comment                        *
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
#include "funcparams.h"
/* Include for AddError() function. */
#include "errors.h"

extern int numErrors;
List * headerlist;
TreeNode *mainFunction;

BOOL CheckArgCount( TreeNode * ast )
{
    headerlist = ListInit( NULL );

    /* Get a list of all function headers */
    CheckFunctionHeader( ast );

    /* Check all function calls for valid # of arguments */
    CheckFunctionApp( ast );

    ListClear ( headerlist );

    return( TRUE );
}

void CheckFunctionHeader( TreeNode * headerNode )
{
    ListNode * listNode;
    AstNode * astNode = (AstNode *) headerNode->data;
    BOOL bAppendResult = FALSE;

    assert( headerNode != NULL );
    assert( astNode != NULL );

    switch( astNode->id )
    {
        case NODE_FUNCTIONHEADER:

        bAppendResult = ListAppend( headerlist, headerNode );
        assert( bAppendResult == TRUE );

        if( IsStartFromHeader( headerNode ) == TRUE )
        {
            if( mainFunction != NULL )
            {
                AddError( "redefinition of `start' function", TOAST( headerNode )->lineno );
            }
            else
            {
                mainFunction = headerNode;
            }
            }
            break;

        default:
            listNode = ListFirstEx( headerNode->children );
            while( listNode != NULL )
            {
              /* Get all symbols from this child node */
              CheckFunctionHeader( (TreeNode *) listNode->data );
              listNode = ListNextEx( listNode );
            }
            break;
    }
}

void CheckFunctionApp( TreeNode * headerNode )
{
    ListNode * listNode;
    AstNode * astNode = (AstNode *) headerNode->data;
    int iApplicationArgC = 0;
    int iFunctionArgC = 0;
    char sWarning[255];

    assert( headerNode != NULL );
    assert( astNode != NULL );

    switch( astNode->id )
    {
    case NODE_APPLICATION:
            iApplicationArgC = GetArgumentCountFromApplication( headerNode );
            iFunctionArgC = GetArgumentCount( GetNameFromApplication( headerNode ) );
            if( iFunctionArgC == -1 )
            {
                sprintf( sWarning, "undeclared function `%s'", GetNameFromApplication( headerNode ) );
                AddWarning( sWarning, TOAST( headerNode )->lineno );
            }
            if( iApplicationArgC < iFunctionArgC )
            {
                sprintf( sWarning, "too few arguments to function `%s'", GetNameFromApplication( headerNode ) );
                AddWarning( sWarning, TOAST( headerNode )->lineno );
            }
            else if( iApplicationArgC > iFunctionArgC )
            {
                sprintf( sWarning, "too many arguments to function `%s'", GetNameFromApplication( headerNode ) );
                AddWarning( sWarning, TOAST( headerNode )->lineno );
            }
            break;
        default:
            listNode = ListFirstEx( headerNode->children );
            while( listNode != NULL )
            {
                /* Get all symbols from this child node */
                CheckFunctionApp( (TreeNode *) listNode->data );
                listNode = ListNextEx( listNode );
            }
            break;
    }
}

int GetArgumentCount( char * functionName )
{
    ListNode *listNode;
    TreeNode *headerNode;

    assert( functionName != NULL );

    listNode = ListFirstEx( headerlist );
    while( listNode != NULL  )
    {
        headerNode = (TreeNode *) listNode->data;
	if( strcmp( functionName, GetNameFromHeader( headerNode ) ) == 0 )
	{
	    return( GetParamCountFromHeader( headerNode ) );
	}
	listNode = ListNextEx( listNode );
    }

    return( -1 );
}

