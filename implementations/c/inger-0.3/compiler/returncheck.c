/************************************************
*                                               *
*  Module: returncheck.c                        *
*  Description:                                 *
*      Check function code block for return     *
*      keyword as last statement in block       *
*  Author: Haaring, J.W.                        *
*  Modifications:                               *
*    [JWH] : Added unreachable code check       *
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
#include "returncheck.h"
/* Include for AddError() function. */
#include "errors.h"
#include "options.h"

void CheckFunctionReturns( TreeNode * ast )
{
    ListNode *listNode = NULL;
    TreeNode *codeNode = NULL;

    assert( ast != NULL );

    switch( TOAST( ast )->id )
    {
        case NODE_FUNCTION:
            codeNode = GetBlockFromFunction( ast );
            if( codeNode != NULL )
            {
                if( GetReturnTypeFromHeader( GetHeaderFromFunction( ast ) ) != NODE_VOID )
                {
                    if( CheckReturnCall( codeNode ) == FALSE )
                    {
                        AddWarning( "control reaches end of non-void function",
                                        TOAST( codeNode )->lineno );
                    }
                }
                if( HasUnreachableCode( codeNode ) == TRUE )
                {
                    AddWarning( "function has unreachable code", TOAST( codeNode )->lineno );
                }
            }
            break;

        default:
            listNode = ListFirstEx( ast->children );
            while( listNode != NULL )
            {
              /* Get all symbols from this child node */
              CheckFunctionReturns( (TreeNode *) listNode->data );
              listNode = ListNextEx( listNode );
            }
            break;
    }
}

BOOL CheckReturnCall( TreeNode * codeNode )
{
    ListNode *listNode = NULL, *caseNode = NULL;
    TreeNode *tempNode = NULL, *lastNode = NULL,
             *thenNode = NULL, *elseNode = NULL;

    assert( codeNode != NULL );

    listNode = ListLastEx( codeNode->children );
    lastNode = ( TreeNode * ) listNode->data;

    listNode = ListFirstEx( codeNode->children );
    while( listNode != NULL )
    {
        tempNode = ( TreeNode * ) listNode->data;
        switch( TOAST( tempNode )->id )
        {
            case NODE_RETURN:
                return( TRUE );

            case NODE_IF:
                thenNode = GetThenBlockFromIf( tempNode );
                elseNode = GetElseBlockFromIf( tempNode );

                if( ( thenNode != NULL ) && ( elseNode != NULL ) )
                {
                    if( ( CheckReturnCall( thenNode ) == TRUE ) &&
                        ( CheckReturnCall( elseNode ) == TRUE ) )
                    {
                        return( TRUE );
                    }
                }
                else if( ( thenNode != NULL ) && ( elseNode == NULL ) )
                {
                    if( CheckReturnCall( thenNode ) == TRUE )
                    {
                        return( TRUE );
                    }
                }
                else if( ( thenNode == NULL ) && ( elseNode != NULL ) )
                {
                    if( CheckReturnCall( elseNode ) == TRUE )
                    {
                        return( TRUE );
                    }
                }
                break;

            case NODE_SWITCH:
                elseNode = GetTreeChild( tempNode, 2 );
                caseNode = ListFirstEx( elseNode->children ); 
                thenNode = (TreeNode *) caseNode->data;
                printf( "nodename of caseNode: %s\n", GetNodeName( TOAST( thenNode )->id ) );
                
                caseNode = ListNextEx( caseNode );
                thenNode = (TreeNode *) caseNode->data;
                printf( "nodename of caseNode: %s\n", GetNodeName( TOAST( thenNode )->id ) );

                if( elseNode != NULL )
                {
                    printf( "nodeNode for elseNode = %s\n", GetNodeName( TOAST( elseNode )->id ) );
                } 
                printf( "switch found\n" );
                break;

            default:
                break;
                
        }
        listNode = ListNextEx( listNode );
    }

    return( FALSE );
}



BOOL HasUnreachableCode( TreeNode * codeNode )
{
    ListNode *listNode = NULL;
    TreeNode *tempNode = NULL;
    TreeNode *lastChild = NULL;

    if( codeNode == NULL )
    {
        return( FALSE );
    }

    listNode = ListLastEx( codeNode->children );
    lastChild = ( TreeNode * ) listNode->data;

    listNode = ListFirstEx( codeNode->children );

    while( listNode != NULL )
    {
        tempNode = ( TreeNode * ) listNode->data;
        switch( TOAST( tempNode )->id )
        {
            case NODE_RETURN:
                if( tempNode != lastChild )
                {
                    return( TRUE );
                }
                break;

            case NODE_IF:
                if( HasUnreachableCode( GetThenBlockFromIf( tempNode ) ) == TRUE )
                {
                    AddWarning( "unreachable code in then block", TOAST( tempNode )->lineno );
                }
                if( HasUnreachableCode( GetElseBlockFromIf( tempNode ) ) == TRUE )
                {
                    AddWarning( "unreachable code in else block", TOAST( tempNode )->lineno );
                }
                break;

            default:
                break;
        } 
        listNode = ListNextEx( listNode );
    }

    return( FALSE );
}

