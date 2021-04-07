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
	Function	- memcpy
	Description	- compares the first n bytes of memory area str1 and memory
				  area str2.
	Declaration	- int memcmp(const void *str1, const void *str2, size_t n)
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

void *memcpy(void *__restrict dest, const void *__restrict src, size_t n) {
	size_t i = 0;
	size_t t = 0;

	void *pDest = dest;
	if (n % 4 == 0) {
		unsigned int *		id = (unsigned int *) pDest;
		const unsigned int *is = (const unsigned int *) src;
		t = n / 4;
		while (i++ < t) {
			*(id++) = *(is++);
		}
	}
	else if (n % 2 == 0) {
		unsigned short *	  sd = (unsigned short *) pDest;
		const unsigned short *ss = (const unsigned short *) src;
		t = n / 2;
		while (i++ < t) {
			*(sd++) = *(ss++);
		}
	}
	else {
		unsigned char *		 d = (unsigned char *) pDest;
		const unsigned char *s = (const unsigned char *) src;
		while (i++ < n) {
			*(d++) = *(s++);
		}
	}

	return dest;
}
}	 // namespace qklib