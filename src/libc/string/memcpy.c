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
 * C library function - memcpy()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function void *memcpy(void *str1, const void *str2, size_t n) *
 * copies n characters from memory area str2 to memory area str1.              *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * void *memcpy(void *str1, const void *str2, size_t n)                        *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str1 − This is pointer to the destination array where the content is to   *
 *          be copied, type-casted to a pointer of type void*.                 *
 *                                                                             *
 * > str2 − This is pointer to the source of data to be copied, type-casted to *
 *          a pointer of type void*.                                           *
 *                                                                             *
 * > n − This is the number of bytes to be copied.                             *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns a pointer to destination, which is str1.              *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

void *memcpy(void *str1, const void *str2, size_t n) {
	size_t i = 0;
	size_t t = 0;
	if (n % 4 == 0) {
		unsigned int *id = (unsigned int *)str1;
		unsigned int *is = (unsigned int *)str2;
		t = n / 4;
		while (i < t) {
			*(id++) = *(is++);
		}
	} else if (n % 2 == 0) {
		unsigned short *sd = (unsigned short *)str1;
		unsigned short *ss = (unsigned short *)str2;
		t = n / 2;
		while (i < t) {
			*(sd++) = *(ss++);
		}
	} else {
		unsigned char *d = (unsigned char *)str1;
		unsigned char *s = (unsigned char *)str2;
		while (i < n) {
			*(d++) = *(s++);
		}
	}

	return str1;
}