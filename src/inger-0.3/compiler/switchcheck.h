/************************************************
*                                               *
*  Module: switchcheck.h                        *
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
*************************************************/

#ifndef SWITCHCHECK_H 
#define SWITCHCHECK_H   1

/* Include type structure interface */
#include "types.h"
/* Include functions to convert node IDs to names. */
#include "nodenames.h"
/* Include Abstract Syntax Tree interface. */
#include "ast.h"


BOOL CheckSwitchStatements( TreeNode * ast );
void SearchSwitches( TreeNode * switchNode );
void AnalyseSwitch( TreeNode * switchNode );
void CheckSwitchStatement( TreeNode * switchNode );
int CaseOccurenceCount( char * TokenValueString );

#endif
