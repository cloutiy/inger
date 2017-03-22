/************************************************
*                                               *
*  Module: funcparams.h                         *
*  Description:                                 *
*      Check function headers for # of params,  *
*      check for params in function calls	*
*      as well.  			        *
*  Author: Haaring, J.W.                        *
*  Modifications:                               *
*    [JWH] Added comment			*
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

#ifndef FUNCPARAMS_H
#define FUNCPARAMS_H 1

/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"

/*
*  Build a list with all function header nodes by 
*  recursively going down the tree. Print error
*  messages when 2 `main' functions are found.
*
*  Pre:  [headerNode] is not NULL and a valid TreeNode
*
*  Post: List of all function header nodes is available.
*        Error message when `main' function defined twice.
*
*/
void CheckFunctionHeader( TreeNode * headerNode );


/*
*  Check all function applications for correct
*  number of arguments. We start at the root of
*  the AST and go down, using this recursive function.
*
*  Pre:  [headerNode] is not NULL and a valid TreeNode
*
*  Post: Function applications are checked on valid # of
*        function arguments. Error messages are generated
*        when function calls have too many, or few arguments.
*        Also use non-implemented or not as extern declared 
*        functions will generate error messages.
*
*/
void CheckFunctionApp( TreeNode * headerNode );


/*
*  Search for the number of arguments of a header
*  node in the header list built by CheckFunctionHeader,
*  by name.  
*
*  Pre:  [functionName] is not NULL
*
*  Post: # of parameters of the headerNode with the name
*  	 [functionName] is returned.
*
*/
int GetArgumentCount( char * functionName );


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
BOOL CheckArgCount( TreeNode * ast );

#endif
