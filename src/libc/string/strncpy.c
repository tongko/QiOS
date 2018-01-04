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
 * C library function - strncpy()                                              *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function char *strncpy(char *, const char *, size_t) copies   *
 * up to n characters from the string pointed to, by src to dest. In a case    *
 * where the length of src is less than that of n, the remainder of dest will  *
 * be padded with null bytes.                                                  *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * char *strncpy(char *dest, const char *src, size_t n);                       *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > dest − This is the pointer to the destination array where the content is  *
 *          to be copied.                                                      *
 *                                                                             *
 * > src − This is the string to be copied.                                    *
 *                                                                             *
 * > n − The number of characters to be copied from source.                    *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns the final copy of the copied string.                  *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

char *strncpy(char *dest, const char *src, size_t n) {
	char *d = dest;
	const char *s = src;

	if (d > s) {
		for (size_t i = 0; i < n; i++) {
			d[i] = s[i];
		}
	} else {
		for (size_t i = n; i; i--) {
			d[i - 1] = s[i - 1];
		}
	}

	return dest;
}