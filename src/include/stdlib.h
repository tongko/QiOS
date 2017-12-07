/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __STDLIB_H_
#define __STDLIB_H_

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

int abs(int x);
size_t itoa(uint32_t value, unsigned char *str, int base);
uint32_t atoi(unsigned char *str);
#endif  // __STDLIB_H_