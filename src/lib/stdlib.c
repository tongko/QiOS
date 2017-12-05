/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <stdlib.h>
#include <string.h>

int32_t __inline__ abs(int32_t x) {
	int32_t y = x >> 31;
	return (x ^ y) - y;
}

unsigned char *itoa(uint32_t value, unsigned char *str, int base) {
	//	Validate base
	if (base < 2 || base > 16) {
		*str = '\0';
		return str;
	}

	unsigned char *p = str;
	uint32_t quotient = value;

	//	Conversion. Number is reversed.
	do {
		const uint32_t tmp = quotient / base;
		*p++ = "0123456789ABCDEF"[quotient - (tmp * base)];
		quotient = tmp;
	} while (quotient);

	*p = '\0';
	reverse(str);

	return str;
}