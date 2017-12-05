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

int abs(int x);
unsigned char *itoa(uint32_t value, unsigned char *str, int base);
#endif  // __STDLIB_H_