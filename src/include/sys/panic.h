/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __PANIC_H_
#define __PANIC_H_

#include <attribs.h>
#include <karg.h>

__early void kernel_panic(const char *format, ...);

#endif  //	__PANIC_H_