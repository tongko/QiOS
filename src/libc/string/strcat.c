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
 * C library function - strcat()                                               *
 * --------------------------------------------------------------------------- *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function char *strcat(char *dest, const char *src) appends    *
 * the string pointed to by src to the end of the string pointed to by dest.   *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * char *strcat(char *dest, const char *src)                                   *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > dest − This is pointer to the destination array, which should contain a C *
 *          string, and should be large enough to contain the concatenated     *
 *          resulting string.                                                  *
 *                                                                             *
 * > src − This is the string to be appended. This should not overlap the      *
 *         destination.                                                        *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns a pointer to the resulting string dest.               *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

char *strcat(char *dest, const char *src) {
	if (!dest) {
		return NULL;
	}

	if (!src) {
		return NULL;
	}

	char *p = dest;
	const char *s = src;

	while (*p) {
		p++;
	}

	while (*s) {
		*p++ = *s++;
	}

	*p = '\0';
	return dest;
}