/*************************************************
 *                                               *
 *  Module: errors.h                             *
 *  Description:                                 *
 *      Interface to error reporting module.     *
 *      This module collects error/warning       *
 *      messages in a list, marking them with    *
 *      the line number on which they occurred.  *
 *      They can then be sorted and printed as   *
 *      a batch.                                 *
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

#ifndef ERRORS_H
#define ERRORS_H 1

/*
 * Output error msg [message], on specified
 * line number [lineno], at charpos [charpos].
 *
 * Pre:  [message] is not the NULL pointer.
 */
void AddPosError( char *message, int lineno, int charpos );

/*
 * Output error msg [message], on specified
 * line number [lineno].
 *
 * Pre:  [message] is not the NULL pointer.
 */
void AddError( char *message, int lineno );

/*
 * Output warning msg [message], on specified
 * line number [lineno], at charpos [charpos].
 *
 * Pre:  [message] is not the NULL pointer.
 */
void AddPosWarning( char *messag, int lineno, int charpos );

/*
 * Output warning msg [message], on specified
 * line number [lineno].
 *
 * Pre:  [message] is not the NULL pointer.
 */
void AddWarning( char *messag, int lineno );

/*
 * Return number of errors so far.
 */
int GetErrorCount();

/*
 * Return number of warnings so far.
 */
int GetWarningCount();

/*
 * Initialize error and warning count to zero.
 * This function must be called before processing
 * a new source file.
 */
void InitializeReport();

/*
 * Print errors and warnings in report to console.
 * The report is destroyed afterwards.
 */
void PrintReport();

#endif

