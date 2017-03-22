/*************************************************
 *                                               *
 *  Module: preprocessor.h                       *
 *  Description:                                 *
 *      Interface to the preprocessor.           *
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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

/*
 *  Call this function to preprocess a file.
 *  Pre: Both file pointers must be valid.
 */
extern BOOL Preprocess( char *fileIn, char *fileOut );

#endif

