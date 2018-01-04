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
 * C library function - strcpy()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function char *strcpy(char *dest, const char *src) copies the *
 * string pointed to, by src to dest.                                          *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * char *strcpy(char *dest, const char *src)                                   *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > dest − This is the pointer to the destination array where the content is  *
 *          to be copied.                                                      *
 *                                                                             *
 * > src − This is the string to be copied.                                    *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This returns a pointer to the destination string dest.                      *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

char *strcpy(char *dest, const char *src) {
	char *d = dest;
	const char *s = src;

	while (*s) {
		*d++ = *s++;
	}

	*d = '\0';
	return dest;
}