/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <string.h>

#include <stdlib.h>

int32_t __inline__ abs(int32_t x) {
	int32_t y = x >> 31;
	return (x ^ y) - y;
}

size_t itoa(uint32_t value, char *str, int base, const char *digit_str) {
	//	Validate base
	if (base < 2 || base > 16) {
		*str = '\0';
		return str;
	}

	char *p = str;
	uint32_t quotient = value;
	size_t sz = 0;

	//	Conversion. Number is reversed.
	do {
		const uint32_t tmp = quotient / base;
		*p++ = digit_str[quotient - (tmp * base)];
		quotient = tmp;
		sz++;
	} while (quotient);

	*p = '\0';
	reverse(str);

	return sz;
}

uint32_t atoi(const char *str) {
	if (!str) {
		return 0;
	}

	uint32_t result = 0;

	for (int32_t i = 0; str[i] != '\0'; i++) {
		if (!ISDIGIT(str[i])) {
			return result;
		}
		result = result * 10 + str[i] - 48;  // 48 == '0'
	}

	return result;
}