/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
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

int32_t printf(const unsigned char *format, ...);

#endif  //	__STDIO_H_