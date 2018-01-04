/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/

#ifndef __KARG_H_
#define __KARG_H_

#include <attribs.h>

// These functions are from the official GNU stdarg.h file
typedef char *va_list;

// Amount of space required in an argument list for an arg of type TYPE.
// TYPE may alternatively be an expression whose type is used.

#define __va_rounded_size(TYPE) \
	(((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(AP, LASTARG) \
	(AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))

extern void va_end(va_list);
#define va_end(AP)  //  Does Nothing

#define va_arg(AP, TYPE) (AP += __va_rounded_size(TYPE), \
                          *((TYPE *)(AP - __va_rounded_size(TYPE))))

#endif  //	__KARG_H_