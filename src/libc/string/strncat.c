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
 * C library function - strncat()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function char *strncat(char *dest, const char *src, size_t n) *
 * appends the string pointed to by src to the end of the string pointed to by *
 * dest up to n characters long.                                               *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * char *strcat(char *dest, const char *src, size_t n)                         *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > dest − This is pointer to the destination array, which should contain a C *
 *          string, and should be large enough to contain the concatenated     *
 *          resulting string which includes the additional null-character.     *
 *                                                                             *
 * > src − This is the string to be appended.                                  *
 *                                                                             *
 * > n − This is the maximum number of characters to be appended.              *                                                  *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns a pointer to the resulting string dest.               *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

char *strncat(char *dest, const char *src, size_t n) {
	if (!dest) {
		return NULL;
	}

	if (!src) {
		return NULL;
	}

	char *d = dest;
	const char *s = src;

	while (*d) {
		d++;
	}

	for (; n > 0; n--) {
		*d++ = *s++;
	}

	*d = '\0';
	return dest;
}