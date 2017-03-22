/************************************************
*                                               *
*  Module: lvalue.c                             *
*  Description:                                 *
*      Check the abstract syntax tree for       *
*      invalid lvalue assignments i.e.          *
*      myfunc() = 4                             *
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

/* Include assertion macro. */
#include <assert.h>
/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"
/* Include lValues interface */
#include "lvalue.h"
/* Include errors.h for PrintError() function. */
#include "errors.h"

void GetValues( TreeNode * node )
{
    ListNode *listNode;
    AstNode *astNode = (AstNode *) node->data;

    assert( node != NULL );
    assert( astNode != NULL );

    switch( astNode->id )
    {
    case NODE_ASSIGN:
        switch( GetLeftOperand( node ) )
        {
        case NODE_LIT_IDENTIFIER:
            break;

        case NODE_DEREFERENCE:
            break;

        default:
            /* AddError will add 1 to numErrors for us */
            AddError( "invalid lvalue", TOAST( node )->lineno  );
        }
        break;

    default:
        listNode = ListFirstEx( node->children );
        while( listNode != NULL )
        {
            /* Get all symbols from this child node. */
            GetValues( (TreeNode *) listNode->data );
            listNode = ListNextEx( listNode );
        }
        break;
    }
}

BOOL CheckLeftValues( TreeNode * ast )
{
    GetValues( ast );

    return( GetErrorCount() == 0);
}

