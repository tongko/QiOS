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
/* assert.h                                                                   */
/* The assert.h header file of the C Standard Library provides a macro called */
/* assert which can be used to verify assumptions made by the program and     */
/* print a diagnostic message if this assumption is false.                    */
/*                                                                            */
/* The defined macro assert refers to another macro NDEBUG which is not a     */
/* part of <assert.h>. If NDEBUG is defined as a macro name in the source     */
/* file, at the point where <assert.h> is included, the assert macro is       */
/* defined as follows −                                                       */
/*                                                                            */
/*    #define assert(ignore) ((void)0)                                        */
/*                                                                            */
/******************************************************************************/
#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <sys/cdefs.h>

#define __ASSERT_VOID_CAST (void)

/******************************************************************************
 * void assert (int expression);                                              *
 *                                                                            *
 * If NDEBUG is defined, do nothing.                                          *
 * If not, and EXPRESSION is zero, print an error message and abort.          *
 *****************************************************************************/

#ifdef NDEBUG

#define assert(expr) (__ASSERT_VOID_CAST(0))

#else

__BEGIN_DECLS

/* This prints an "Assertion failed" message and aborts.  */
extern void __assert_fail(const char *assertion, const char *file,
                          unsigned int line, const char *function)
    __THROW __attribute__((__noreturn__));

__END_DECLS

#ifdef __cplusplus
#define assert(expr) (static_cast<bool>(expr) \
                          ? void(0)           \
                          : __assert_fail(#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))
#else
#define assert(expr) ((expr) \
						? __ASSERT_VOID_CAST (0)
						: __assert_fail(#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))
#endif  // __cplusplus

#ifdef __cplusplus
#define __ASSERT_FUNCTION __extension__ __PRETTY_FUNCTION__
#else
#define __ASSERT_FUNCTION __func__
#endif

#endif  //	NDEBUG

#endif  // __ASSERT_H_