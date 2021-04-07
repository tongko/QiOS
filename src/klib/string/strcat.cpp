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

char *strcat(char *dest, const char *src) {
	char *	  p = dest;
	const char *s = src;
	while (*p++) {}
	p--;

	while (*s != '\0') {
		*(p++) = *(s++);
	}

	*p = '\0';
	return dest;
}
}	 // namespace qklib