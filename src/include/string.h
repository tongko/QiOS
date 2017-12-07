/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __STRING_H_
#define __STRING_H_

#include <stdint.h>
//#include <sys/types.h>
#include <stddef.h>

#ifndef ISDIGIT
#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif

size_t strlen(const char *str);
// string_t *makestr(string_t *str1, const char *str2);
// string_t *_strcat(string_t *dest, const string_t *src);
char *strcat(char *dest, const char *src);
// int _strcmp(const string_t *str1, const string_t *str2);
int strcmp(const char *str1, const char *str2);
void *memcpy(void *dest, const void *src, size_t n);
// void _reverse(string_t *str);
void reverse(char *str);
char *strchr(const char *s, int c_in);

#endif  //	__STRING_H_