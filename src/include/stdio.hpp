#pragma once

/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <cdefs.h>
#include <stdarg.h>

#ifndef EOF
	#define EOF (-1)
#endif

namespace qklib {

extern int printf(const char *__restrict format, ...);
extern int sprintf(char *__restrict str, const char *__restrict format, ...);
extern int vsprintf(char *__restrict str,
					const char *__restrict format,
					va_list arg);
extern int putchar(int);
extern int puts(const char *);

}	 // namespace qklib