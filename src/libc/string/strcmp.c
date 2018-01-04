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
 * C library function - strcmp()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function int strcmp(const char *str1, const char *str2)       *
 * compares the string pointed to, by str1 to the string pointed to by str2.   *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * int strcmp(const char *str1, const char *str2)                              *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str1 − This is the first string to be compared.                           *
 *                                                                             *
 * > str2 − This is the second string to be compared.                          *
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

int strcmp(const char *str1, const char *str2) {
	const char *s1 = str1;
	const char *s2 = str2;

	for (; *s1 && *s2; s1++, s2++) {
		if (*s1 == *s2) {
			continue;
		}

		if (*s1 > *s2) {
			return 1;
		}

		return -1;
	}

	//	if both *s1 and *s2 are '\0' than return equals
	if (*s1 == *s2) {
		return 0;
	} else if (*s1) {
		//	str1 is longer than str2, so return greater than
		return 1;
	}

	//	str2 is longer than str1, so return lesser than
	return -1;
}