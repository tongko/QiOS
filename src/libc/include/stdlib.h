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
/* stdlib.h                                                                   */
/* The stdlib.h header defines four variable types, several macros, and       */
/* various functions for performing general functions.                        */
/*                                                                            */
/******************************************************************************/
#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <sys/cdefs.h>

/*	Need szie_t, NULL, and wchar_t */
#include <stddef.h>

/* Returned by `div'.  */
typedef struct {
	int quot; /* Quotient.  */
	int rem;  /* Remainder.  */
} div_t;

/* Returned by `ldiv'.  */
#ifndef __ldiv_t_defined
typedef struct {
	long int quot; /* Quotient.  */
	long int rem;  /* Remainder.  */
} ldiv_t;
#define __ldiv_t_defined 1
#endif  //	__ldiv_t_defined

/* The largest number rand will return (same as INT_MAX).  */
#define RAND_MAX 2147483647

/* We define these the same for all machines.
   Changes from this to the outside world should be done in `_exit'.  */
#define EXIT_FAILURE 1 /* Failing exit status.  */
#define EXIT_SUCCESS 0 /* Successful exit status.  */

/* Maximum length of a multibyte character in the current locale.  */
#define MB_CUR_MAX (__ctype_get_mb_cur_max())
extern size_t __ctype_get_mb_cur_max(void) __THROW;

/* Convert a string to a floating-point number.  */
extern double atof(const char *__nptr) __THROW;

#endif  //	__STDLIB_H_