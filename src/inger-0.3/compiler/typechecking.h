/*************************************************
 *                                               *
 *  Module: typechecking.h                       *
 *  Description:                                 *
 *      ...                                      *
 *  Author: Janssen. T.H.                        *
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

#ifndef TYPECHECKING_H
#define TYPECHECKING_H 1

#include "ast.h"

void DecorateAstWithTypes( TreeNode *ast );

#endif
