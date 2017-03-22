/************************************************
*                                               *
*  Module: lvalue.h                             *
*  Description:                                 *
*      Interface to lvalue check.               *
*  Author: Haaring, J.W.                        *
*  Modifications:                               *
*    [JWH] Added comment 			*
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

#ifndef LVALUE_H
#define LVALUE_H

/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"

/*
*  Check all assignment operators "=" for valid lvalue
*  by calling GetValues( ast ) which will search the
*  tree recursively.
*
*  Pre:  [ast] is not NULL and a valid TreeNode
*
*  Post: TRUE is returned if no invalid lvalues were found,
*        otherwise the return value is FALSE
*
*/
BOOL CheckLeftValues( TreeNode * ast );

/*
*  Check all assignment operators "=" for valid lvalue
*  by going down the AST recursively. 
*
*  Pre:  [node] is not NULL and a valid TreeNode
*
*  Post: AST is checked for invalid lvalues. If any found
*        error messages will be generated.
*/
void GetValues( TreeNode * node );

#endif
