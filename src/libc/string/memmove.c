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
 * C library function - memmove()                                              *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function memmove(void *, const void *, size_t) copies n       *
 * characters from str2 to str1, but for overlapping memory blocks, memmove()  *
 * is a safer approach than memcpy().                                          *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * void *memmove(void *str1, const void *str2, size_t n)                       *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str1 − This is a pointer to the destination array where the content is to *
 *          be copied, type-casted to a pointer of type void*.                 *
 *                                                                             *
 * > str2 − This is a pointer to the source of data to be copied, type-casted  *
 *          to a pointer of type void*.                                        *
 *                                                                             *
 * > n − This is the number of bytes to be copied.                             *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns a pointer to the destination, which is str1.          *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

void *memmove(void *str1, const void *str2, size_t n) {
	unsigned char *d = (unsigned char *)str1;
	const unsigned char *s = (const unsigned char *)str2;

	if (d < s) {
		for (size_t i = 0; i < n; i++) {
			d[i] = s[i];
		}
	} else {
		for (size_t i = n; i != 0; i--) {
			d[i - 1] = s[i - 1];
		}
	}

	return str1;
}