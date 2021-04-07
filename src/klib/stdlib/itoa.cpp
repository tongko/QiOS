/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <stdlib.hpp>
#include <string.hpp>

namespace qklib {

size_t itoa(uint32_t value, char *str, int base, const char *digit_str) {
	//	Validate base
	if (base < 2 || base > 16) {
		*str = '\0';
		return 0;
	}

	char * p = str;
	uint32_t quotient = value;
	size_t	 sz = 0;

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
}	 // namespace qklib