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
 * C library function - strncmp()                                              *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function int strncmp(const char *, const char *, size_t)      *
 * compares at most the first n bytes of str1 and str2.                        *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * int strncmp(const char *str1, const char *str2, size_t n)                   *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str1 − This is the first string to be compared.                           *
 *                                                                             *
 * > str2 − This is the second string to be compared.                          *
 *                                                                             *
 * > n − The maximum number of characters to be compared.                      *
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

int strncmp(const char *str1, const char *str2, size_t n) {
	const char *s1 = str1;
	const char *s2 = str2;

	for (size_t i = 0; i < n; i++) {
		if (s1[i] == s2[i]) {
			if (!s1[i]) {
				//	both str1 and str2 reaches end ('\0'), terminate.
				return 0;
			}

			continue;
		}

		if (*s1 > *s2) {
			return 1;
		}

		return -1;
	}
}