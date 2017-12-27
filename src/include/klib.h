/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __KLIB_H_
#define __KLIB_H_

#include <attribs.h>
#include <kdef.h>
#include <stdint.h>

#ifndef MAX
#define MAX(x, y) (x > y ? x : y)
#endif  //	MAX

#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif  //	MIN

#ifndef ISDIGIT
#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif  //	ISDIGIT

__early int32_t abs(int32_t x);
__early size_t itoa(uint32_t value, char *str, int base, const char *digit_str);
__early uint32_t atoi(const char *str);

#endif  // __KLIB_H_