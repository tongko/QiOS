/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <string.hpp>

namespace qklib {

void *memmove(void *str1, const void *str2, size_t n) {
	unsigned char *		 dst = (unsigned char *) str1;
	const unsigned char *src = (const unsigned char *) str2;

	if (dst < src) {
		for (size_t i = 0; i < n; i++)
			dst[i] = src[i];
	}
	else {
		for (size_t i = n; i != 0; i--)
			dst[i - 1] = src[i - 1];
	}

	return str1;
}
}	 // namespace qklib