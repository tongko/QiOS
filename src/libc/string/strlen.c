/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *----------------------------------------------------------------------------*/
/*******************************************************************************
 *                                                                             *
 * C library function - strlen()                                               *
 * -------------------------------------------------------------------------   *
 * Description                                                                 *
 * ===========                                                                 *
 * The C library function size_t strlen(const char *str) computes the length   *
 * of the string str up to, but not including the terminating null character.  *
 *                                                                             *
 * Declaration                                                                 *
 * ===========                                                                 *
 * size_t strlen(const char *str)                                              *
 *                                                                             *
 * Parameters                                                                  *
 * ==========                                                                  *
 * > str − This is the string whose length is to be found.                     *
 *                                                                             *
 * Return Value                                                                *
 * ============                                                                *
 * This function returns the length of string.                                 *
 *                                                                             *
 ******************************************************************************/
#include <string.h>

size_t strlen(const char *str) {
	size_t len = 0;
	while (*str++) {
		len++;
	}

	return len;
}