/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 *----------------------------------------------------------------------------*/
/*******************************************************************************
 *                                                                             *
 * C library function - memcmp()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function memcmp(const void *, const void *, size_t)) compares *
 * the first n bytes of memory area str1 and memory area str2.                 *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * int memcmp(const void *str1, const void *str2, size_t n)                    *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str1 − This is the pointer to a block of memory.                          *
 * > str2 − This is the pointer to a block of memory.                          *
 * > n − This is the number of bytes to be compared.                           *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * > if Return value < 0 then it indicates str1 is less than str2.             *
 *                                                                             *
 * > if Return value > 0 then it indicates str2 is less than str1.             *
 *                                                                             *
 * > if Return value = 0 then it indicates str1 is equal to str2.              *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

int memcmp(const void *str1, const void *str2, size_t n) {
	int i = 0;
	const unsigned char *s1 = str1;
	const unsigned char *s2 = str2;

	for (; *s1 && *s2 && i < n; str1++, str2++, i++) {
		if (*s1 == *s2) {
			continue;
		}

		if (*s1 > *s2) {
			return 1;
		}

		return -1;
	}

	return 0;
}