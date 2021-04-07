/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <stdint.h>
#include <string.hpp>

namespace qklib {

int strcmp(const char *str1, const char *str2) {
	const char *a = str1;
	const char *b = str2;

	for (size_t i = 0;; i++) {
		if (!a[i] && !b[i])
			return 0;
		if (!a[i] || a[i] < b[i])
			return -1;
		if (!b[i] || b[i] < a[i])
			return 1;
	}

	return 0;
}
}	 // namespace qklib