/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 *----------------------------------------------------------------------------*/
/******************************************************************************/
/*                                                                            */
/* string.h                                                                   */
/* The string.h header defines one variable type, one macro, and various      */
/* functions for manipulating arrays of characters.                           */
/*                                                                            */
/******************************************************************************/
#ifndef __STRING_H_
#define __STRING_H_ 1

#include <sys/cdefs.h>

#include <stddef.h>

__BEGIN_DECLS

#ifndef ISDIGIT
#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif

/******************************************************************************
 * The C library function void *memchr(const void *str, int c, size_t n)      *
 * searches for the first occurrence of the character c (an unsigned char) in *
 * the first n bytes of the string pointed to, by the argument str.           *
 *****************************************************************************/
void *memchr(const void *str, int c, size_t n);

/******************************************************************************
 * The C library function                                                     *
 * int memcmp(const void *str1, const void *str2, size_t n)) compares the     *
 * first n bytes of memory area str1 and memory area str2.                    *
 *****************************************************************************/
int memcmp(const void *str1, const void *str2, size_t n);

/******************************************************************************
 * The C library function void *memcpy(void *str1, const void *str2, size_t n)*
 * copies n characters from memory area str2 to memory area str1.             *
 *****************************************************************************/
void *memcpy(void *str1, const void *str2, size_t n);

/******************************************************************************
 * The C library function                                                     *
 * void *memmove(void *str1, const void *str2, size_t n) copies n characters  *
 * from str2 to str1, but for overlapping memory blocks, memmove() is a safer *
 * approach than memcpy().                                                    *
 *****************************************************************************/
void *memmove(void *str1, const void *str2, size_t n);

/******************************************************************************
 * The C library function void *memset(void *str, int c, size_t n) copies the *
 * character c (an unsigned char) to the first n characters of the string     *
 * pointed to, by the argument str.                                           *
 *****************************************************************************/
void *memset(void *str, int c, size_t n);

/******************************************************************************
 * The C library function char *strcat(char *dest, const char *src) appends   *
 * the string pointed to by src to the end of the string pointed to by dest.  *
 *****************************************************************************/
char *strcat(char *dest, const char *src);

/******************************************************************************
 * The C library function char *strncat(char *dest, const char *src, size_t n)*
 * appends the string pointed to by src to the end of the string pointed to by*
 * dest up to n characters long.                                              *
 *****************************************************************************/
char *strncat(char *dest, const char *src, size_t n);

/******************************************************************************
 * The C library function char *strchr(const char *str, int c) searches for   *
 * the first occurrence of the character c (an unsigned char) in the string   *
 * pointed to by the argument str.                                            *
 *****************************************************************************/
char *strchr(const char *str, int c);

/******************************************************************************
 * The C library function int strcmp(const char *str1, const char *str2)      *
 * compares the string pointed to, by str1 to the string pointed to by str2.  *
 *****************************************************************************/
int strcmp(const char *str1, const char *str2);

/******************************************************************************
 * The C library function int strncmp(const char *, const char *, size_t)     *
 * compares at most the first n bytes of str1 and str2.                       *
 *****************************************************************************/
int strncmp(const char *str1, const char *str2, size_t n);

/*******************************************************************************
 * The C library function char *strcpy(char *dest, const char *src) copies the *
 * string pointed to, by src to dest.                                          *
 ******************************************************************************/
char *strcpy(char *dest, const char *src);

/*******************************************************************************
 * The C library function char *strncpy(char *, const char *, size_t) copies   *
 * up to n characters from the string pointed to, by src to dest. In a case    *
 * where the length of src is less than that of n, the remainder of dest will  *
 * be padded with null bytes.                                                  *
 ******************************************************************************/
char *strncpy(char *dest, const char *src, size_t n);

/******************************************************************************
 * The C library function size_t strcspn(const char *str1, const char *str2)   *
 * calculates the length of the initial segment of str1, which consists        *
 * entirely of characters not in str2.                                         *
 *****************************************************************************/
size_t strcspn(const char *str1, const char *str2);

/******************************************************************************
 * The C library function size_t strlen(const char *str) computes the length  *
 * of the string str up to, but not including the terminating null character. *
 *****************************************************************************/
size_t strlen(const char *str);

//void reverse(char *str);

/******************************************************************************
 *****************************************************************************/
__END_DECLS

#endif  //	__KSTR_H_