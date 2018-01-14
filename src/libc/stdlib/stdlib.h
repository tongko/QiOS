/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 *----------------------------------------------------------------------------*/
#ifndef __STDLIB_H_

/*	Need szie_t, NULL, and wchar_t */
#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

#define __STDLIB_H_ 1

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

/* Convert a string to an integer.  */
extern int atoi(const char *str)
    __THROW __attribute_pure__ __nonnull((1)) __wur;

/* Allocate SIZE bytes of memory.  */
extern void *malloc(size_t size) __THROW __attribute_malloc__ __wur;

/* Free a block allocated by `malloc', `realloc' or `calloc'.  */
extern void free(void *ptr) __THROW;

/* Abort execution and generate a core-dump.  */
extern void abort(void) __THROW __attribute__((__noreturn__));

/* Register a function to be called when `exit' is called.  */
extern int atexit(void (*func)(void)) __THROW __nonnull((1));

/* Return the value of envariable NAME, or NULL if it doesn't exist.  */
extern char *getenv(const char *name) __THROW __nonnull((1)) __wur;

__END_DECLS

#endif  //	__STDLIB_H_