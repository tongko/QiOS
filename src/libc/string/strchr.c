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
 * C library function - strchr()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function char *strchr(const char *str, int c) searches for    *
 * the first occurrence of the character c (an unsigned char) in the string    *
 * pointed to by the argument str.                                             *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * char *strchr(const char *str, int c)                                        *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str − This is the C string to be scanned.                                 *
 *                                                                             *
 * > c − This is the character to be searched in str.                          *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This returns a pointer to the first occurrence of the character c in the    *
 * string str, or NULL if the character is not found.                          *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

char *strchr(const char *str, int c) {
	if (!str) {
		return NULL;
	}

	const char *s = str;
	unsigned char ch = (unsigned char)c;
	while (*s) {
		if (*s == ch) {
			return s;
		}

		s++;
	}

	return NULL;
}