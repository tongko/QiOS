/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
/*******************************************************************************
	Function	- memcmp
	Description	- compares the first n bytes of memory area str1 and memory
				  area str2.
	Declaration	- int memcmp(const void *str1, const void *str2, size_t n);
	Parameters	-
		str1	: This is the pointer to a block of memory.
		str2	: This is the pointer to a block of memory.
		n		: This is the number of bytes to be compared.

	Returns		:
		- if Return value < 0 then it indicates str1 is less than str2.
		- if Return value > 0 then it indicates str2 is less than str1.
		- if Return value = 0 then it indicates str1 is equal to str2.

*******************************************************************************/
#include <string.hpp>

namespace qklib {

int memcmp(const void *str1, const void *str2, size_t size) {
	const unsigned char *a = (const unsigned char *) str1;
	const unsigned char *b = (const unsigned char *) str2;

	for (size_t i = 0; i < size; i++) {
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
