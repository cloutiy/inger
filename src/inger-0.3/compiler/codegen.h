/*************************************************
 *                                               *
 *  Module: codegen.h                            *
 *  Description:                                 *
 *      Interface to the x86 code generator.     *
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


#ifndef CODEGEN_H
#define CODEGEN_H

#include "tree.h" /* tree traversal routines */

void GenerateCode( TreeNode *node );


#endif
