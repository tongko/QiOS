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

void reverse(char *str) {
	char *s = str;
	size_t	len = strlen(s), i, j;
	char	aux;

	for (i = len - 1, j = 0; i > j; i--, j++) {
		aux = s[i];
		s[i] = s[j];
		s[j] = aux;
	}
}
}	 // namespace qklib