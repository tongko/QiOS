#pragma once
/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <cdefs.h>
#include <stddef.h>

namespace qklib {

extern int	   memcmp(const void *str1, const void *str2, size_t n);
extern void *  memcpy(void *__restrict dest,
					  const void *__restrict src,
					  size_t n);
extern void *  memmove(void *dest, const void *src, size_t n);
extern void *  memset(void *str, int c, size_t n);
extern void *  memsetw(void *str, unsigned short c, size_t n);
extern void *  memsetdw(void *str, unsigned int c, size_t n);
extern void	   reverse(char *str);
extern char *strcat(char *dest, const char *src);
extern char *strchr(const char *s, int c);
extern int	   strcmp(const char *str1, const char *str2);
extern size_t  strlen(const char *str);

}	 // namespace qklib