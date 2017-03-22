/*************************************************
 *                                               *
 *  Module: getsymbols.h                         *
 *  Description:                                 *
 *      Derives symbol information from          *
 *      an abstract syntax tree and stores it    *
 *      in a symbol table.                       *
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

#ifndef GETSYMBOLS_H
#define GETSYMBOLS_H


/* Include the Abstract Syntax Tree interface. */
#include "ast.h"


/*
 * This function is called to create the 
 * symbol table from an Abstract Syntax Tree.
 *
 * Pre: [ast] is a valid root node of an
 *      Abstract Syntax Tree.
 */
extern void CreateSymbolTable( TreeNode *ast  );


#endif
