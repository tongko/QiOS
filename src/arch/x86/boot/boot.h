/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/

#ifndef __BOOT_H_
#define __BOOT_H_

#include <multiboot/multiboot2.h>
#include <stdarg.h>
#include <stdint.h>
#include "ktypedef.h"
#include "paging.h"

//	Kernel output
int32_t printf(const unsigned char *format, ...);
//	Write string to buffer
int vsprintf(unsigned char *str, const unsigned char *format, va_list arg);

#endif  //	__BOOT_H_