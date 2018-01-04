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
 * C library function - memset()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function memset(void *, int, size_t) copies the character c   *
 * (an unsigned char) to the first n characters of the string pointed to, by   *
 * the argument str.                                                           *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * void *memset(void *str, int c, size_t n)                                    *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str − This is a pointer to the block of memory to fill.                   *
 *                                                                             *
 * > c − This is the value to be set. The value is passed as an int, but the   *
 *       function fills the block of memory using the unsigned char conversion *
 *       of this value.                                                        *
 *                                                                             *
 * > n − This is the number of bytes to be set to the value.                   *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns a pointer to the memory area str.                     *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

__inline__ void *memset(void *str, int c, size_t n) {
	unsigned char ch = (unsigned char)c;
	unsigned char *d = (unsigned char *)str;
	for (; n > 0; n--) {
		*d++ = ch;
	}

	return str;
}