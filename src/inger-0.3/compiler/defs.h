/*************************************************
 *                                               *
 *  Module: defs.h                               *
 *  Description:                                 *
 *      General macros and typedefs.             *
 *  Author: Van Oostenrijk, A.C.                 *
 *  Modifications:                               *
 *    [JWH] Added SEMERR macro to report	 *
 *          semantic errors			 *
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

#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>

/*
 * Define some standard error messages as constants
 * to avoid duplication
 */
#define ERR_NOMEM "Memory allocation failed."
 

/*
 *  C does not have a boolean type. Define
 *  one here.
 */
typedef enum
{
    FALSE   = 0,
    TRUE    = 1
}
BOOL;

extern char *sourceFilename;

/*
 *  Some C implementations do not support the
 *  NULL macro. If this is the case, define it
 *  here.
 */
#ifndef NULL
    #define NULL 0x00000000
#endif

/*
 *  Provide a wrapper to output debug information.
 *  This macro checks that the debug flag was supplied
 *  on the command line. If it was, DEBUG works
 *  like fprintf( stderr, ... ).
 */
#define DEBUG(fmt, args...) \
    { \
        if( WantInternalDebug() == TRUE ) \
        { \
            fprintf( stderr, "(debug) [%s:%s (%d)] "fmt, __FILE__, __FUNCTION__, __LINE__ , ##args );  \
        } \
    }

#define SEMERR(fmt, args...) \
    { \
        fprintf( stderr, "%s: "fmt, sourceFilename, ##args );  \
    }

#define BAILOUT(fmt, args...) \
    { \
        fprintf( stderr, "(error) [%s:%s (%d)] "fmt, __FILE__, __FUNCTION__, __LINE__ , ##args ); \
        exit( -1 ); \
    }

/* Author of following story must take care not to confuse CPP with 
 * C++ a preprocessor is not a language
 */

/* NOTE: The comma between __LINE__ and ##args must be removed if args is empty. In C++,
 * this can be done by using the ## (concatenation) operator as a unary operator
 * to args. In GCC, this ONLY works if the comma is preceded by a space.
 *
 * The named variable argument list args is required by GCC. GCC does not support
 * unnamed variable argument lists (...) (referred to using __VA_ARGS__) even though
 * named variable argument lists are C++ only.
 *
 * The ## operator is also a C++ feature. In strict C, we must leave it out and
 * create two macros: one for no arguments, and one for one or more arguments. Still,
 * to support C99, we must also use unnamed argument lists and GCC does not
 * support this through all versions.
 *
 * See: http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

#endif

