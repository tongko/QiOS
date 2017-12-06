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

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef ISDIGIT
#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif

size_t strlen(const unsigned char *str);
string_t *makestr(string_t *str1, const unsigned char *str2);
string_t *_strcat(string_t *dest, const string_t *src);
unsigned char *strcat(unsigned char *dest, const unsigned char *src);
int _strcmp(const string_t *str1, const string_t *str2);
int strcmp(const unsigned char *str1, const unsigned char *str2);
void *memcpy(void *dest, const void *src, size_t n);
void _reverse(string_t *str);
void reverse(unsigned char *str);
unsigned char *strchr(const unsigned char *s, int c_in);

#endif  //	__STRING_H_