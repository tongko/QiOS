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

void *memsetw(void *str, unsigned short c, size_t n) {

	unsigned short *s = (unsigned short *) str;
	unsigned short	ch = c;

	while (n--) {
		*s++ = ch;
	}

	return str;
}
}	 // namespace qklib