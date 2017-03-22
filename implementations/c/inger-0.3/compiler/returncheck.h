/************************************************
*                                               *
*  Module: returncheck.h                        *
*  Description:                                 *
*      Check function code block for return     *
*      keyword as last statement in block       *
*      Additionally this module checks for      *
*      unreachable code.                        *
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
*************************************************/

#ifndef RETURNCHECK_H
#define RETURNCHECK_H 1

/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"

/*
*  Check all function applications on correct number of
*  arguments, defined in the function header. Also check
*  for user of functions without an implementation or
*  extern declaration.
*
*  Pre:  [ast] is not NULL and a valid TreeNode
*
*  Post: TRUE is returned if no invalid arguments # 
*        were found, otherwise the return value is FALSE
*
*/
BOOL CheckReturnCall( TreeNode * codeNode );


void CheckFunctionReturns( TreeNode * ast );


BOOL HasUnreachableCode( TreeNode * codeNode );

#endif
