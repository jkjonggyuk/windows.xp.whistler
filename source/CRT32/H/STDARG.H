/***
*stdarg.h - defines ANSI-style macros for variable argument functions
*
*	Copyright (c) 1985-1993, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	This file defines ANSI-style macros for accessing arguments
*	of functions which take a variable number of arguments.
*	[ANSI]
*
*Revision History:
*	05-03-89  JCR	Added _INTERNAL_IFSTRIP for relinc usage
*	08-15-89  GJF	Cleanup, now specific to OS/2 2.0 (i.e., 386 flat model)
*	10-30-89  GJF	Fixed copyright
*	11-02-89  JCR	Changed "DLL" to "_DLL"
*	01-05-90  JCR	Added NULL definition
*	03-02-90  GJF	Added #ifndef _INC_STDARG stuff. Also, removed some
*			(now) useless preprocessor directives.
*	05-29-90  GJF	Replaced sizeof() with _INTSIZEOF() and revised the
*			va_arg() macro (fixes PTM 60)
*	05-31-90  GJF	Revised va_end() macro (propagated 5-25-90 change to
*			crt7 version by WAJ)
*	10-30-90  GJF	Moved the real definitions into cruntime.h (for NT
*			folks) and relinc.sed (to release ANSI compatible
*			version). Ugly compromise.
*	08-20-91  JCR	C++ and ANSI naming
*	11-01-91  GDP	MIPS Compiler support. Moved real definitions back here
*	10-16-92  SKS	Replaced "#ifdef i386" with "#ifdef _M_IX86".
*	11-03-92  GJF	Fixed several conditionals, dropped _DOSX32_ support.
*	01-03-93  SRW	Fold in ALPHA changes
*       01-09-93  SRW   Remove usage of MIPS and ALPHA to conform to ANSI
*			Use _MIPS_ and _ALPHA_ instead.
*	01-25-93  GJF	Fix va_list definition.
*       10-04-93  SRW   Fix ifdefs for MIPS and ALPHA to only check for _M_?????? defines
*
****/

#ifndef _INC_STDARG

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _INTERNAL_IFSTRIP_
#include <cruntime.h>
#endif	/* _INTERNAL_IFSTRIP_ */


#ifndef _VA_LIST_DEFINED
#ifdef _M_ALPHA
typedef struct {
	char *a0;	/* pointer to first homed integer argument */
	int offset;	/* byte offset of next parameter */
} va_list;
#else
typedef char *	va_list;
#endif
#define _VA_LIST_DEFINED
#endif

#ifdef _M_IX86

/* Use these types and definitions if generating code for x86 */

#define _INTSIZEOF(n)	( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) 

#define va_start(ap,v)	( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)	( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)	( ap = (va_list)0 )

#endif

#ifdef _M_MRX000

/* Use these types and definitions if generating code for MIPS */

#define va_start(ap,v) ap  = (va_list)&v + sizeof(v)
#define va_end(list)
#define va_arg(list, mode) ((mode *)(list =\
 (char *) ((((int)list + (__builtin_alignof(mode)<=4?3:7)) &\
 (__builtin_alignof(mode)<=4?-4:-8))+sizeof(mode))))[-1]

/*  +++++++++++++++++++++++++++++++++++++++++++
    Because of parameter passing conventions in C:
    use mode=int for char, and short types
    use mode=double for float types
    use a pointer for array types
    +++++++++++++++++++++++++++++++++++++++++++ */


#endif /* _M_MRX000 */

#ifdef _M_ALPHA

/* Use these types and definitions if generating code for ALPHA */

/*
 * The Alpha compiler supports two builtin functions that are used to
 * implement stdarg/varargs.  The __builtin_va_start function is used
 * by va_start to initialize the data structure that locates the next
 * argument.  The __builtin_isfloat function is used by va_arg to pick
 * which part of the home area a given register argument is stored in.
 * The home area is where up to six integer and/or six floating point
 * register arguments are stored down (so they can also be referenced
 * by a pointer like any arguments passed on the stack).
 */

#ifdef _CFRONT
extern __builtin_va_start(va_list, ...);
#define __builtin_isfloat(a) __builtin_alignof(a)
#elif defined(__cplusplus)
extern __builtin_va_start(va_list, ...);
#endif

#define va_start(list, v) __builtin_va_start(list, v, 1)
#define va_end(list)
#define va_arg(list, mode) \
    ( *(        ((list).offset += ((int)sizeof(mode) + 7) & -8) , \
        (mode *)((list).a0 + (list).offset - \
                    ((__builtin_isfloat(mode) && (list).offset <= (6 * 8)) ? \
                        (6 * 8) + 8 : ((int)sizeof(mode) + 7) & -8) \
                ) \
       ) \
    )

#endif /* _M_ALPHA */

#ifdef __cplusplus
}
#endif

#define _INC_STDARG
#endif	/* _INC_STDARG */
