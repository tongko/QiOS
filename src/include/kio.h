/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __KIO_H_
#define __KIO_H_

#include <attribs.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef MAXLEN
#define MAXLEN 4096
#endif

uint32_t sprintf(char *str, const char *format, ...);
uint32_t vsprintf(char *str, const char *format, va_list arg);
uint32_t printf(const char *format, ...);

#endif  //	__KIO_H_