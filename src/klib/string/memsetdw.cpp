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

void *memsetdw(void *str, unsigned int c, size_t n) {

	unsigned int *s = (unsigned int *) str;
	unsigned int  ch = c;

	while (n--) {
		*s++ = ch;
	}

	return str;
}
}	 // namespace qklib