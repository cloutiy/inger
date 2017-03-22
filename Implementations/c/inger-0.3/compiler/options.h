/*************************************************
 *                                               *
 *  Module: options.h                            *
 *  Description:                                 *
 *      Interface to the command line options    *
 *      parser.                                  *
 *  Author: Van Oostenrijk, A.C.                 *
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


#ifndef OPTIONS_H
#define OPTIONS_H

#include "defs.h"           /* BOOL type */

/*
 *  Parses command line options. Option values are
 *  stored in the global [options] array. Option values
 *  are accessed by external modules through accessor
 *  functions (below).
 *  This function's parameters are the same as the
 *  parameters for a C program's 'main' function.
 *
 *  Return values:
 *  TRUE  - all options were parsed successfully, and there
 *          is at least one input file specified.
 *  FALSE - an options failed to parse, or there are no input
 *          files specified.
 */
BOOL ParseOptions( int argc, char **argv );

/*
 *  Checks that --symbols option was supplied.
 *
 *  Return values:
 *  TRUE  - --symbols was supplied
 *  FALSE - --symbols was not supplied.
 */
BOOL WantSymbolTable();

/*
 *  Checks whether --ast option was supplied.
 *
 *  Return values:
 *  TRUE  - --ast was supplied
 *  FALSE - --ast was not supplied.
 */
BOOL WantAstDump();

/*
 * Checks whether --astfile option was supplied.
 * 
 * Return values:
 * TRUE  - --astfile was supplied.
 * FALSE - --astfile was not supplied.
 */
BOOL WantAstFileDump();

/*
 * If an AST dump to file was requested
 * (WantAstFileDump() == TRUE), then
 * GetAstFilename() will return the target
 * file name.
 */
char *GetAstFilename();

/*
 *  Checks whether --debug option was supplied.
 *
 *  Return values:
 *  TRUE  - --debug was supplied
 *  FALSE - --debug was not supplied.
 */
BOOL WantInternalDebug();

#endif

