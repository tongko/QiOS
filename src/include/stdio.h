/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __STDIO_H_
#define __STDIO_H_

#include <stdint.h>
#include <sys/types.h>

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#include <stdarg.h>

int32_t sprintf(unsigned char *str, const char *format, ...);
int32_t vsprintf(string_t *str, const string_t *format, va_list arg);
int32_t printf(const string_t *format, ...);

#endif  //	__STDIO_H_