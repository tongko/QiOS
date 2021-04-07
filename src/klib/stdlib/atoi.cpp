/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <stdlib.hpp>

namespace qklib {

uint32_t atoi(const char *str) {
	if (!str) {
		return 0;
	}

	uint32_t result = 0;

	for (int32_t i = 0; str[i] != '\0'; i++) {
		if (!ISDIGIT(str[i])) {
			return result;
		}
		result = result * 10 + str[i] - 48;	   // 48 == '0'
	}

	return result;
}
}	 // namespace qklib