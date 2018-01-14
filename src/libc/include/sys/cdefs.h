/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __SYS_CDEFS_H_
#define __SYS_CDEFS_H_ 1

#define __QIOS_LIBC 1

#ifndef __cplusplus
#define __THROW __attribute__((__nothrow__, __LEAF))
#define __THROWNL __attribute__((__nothrow__))
#define __NTH(fct) __attribute__((__nothrow__, __LEAF)) fct
#define __NTHNL(fct) __attribute__((__nothrow__)) fct
#else
#define __THROW throw()
#define __THROWNL throw()
#define __NTH(fct) __LEAF_ATTR fct throw()
#define __NTHNL(fct) fct throw()
#endif  //	__cplusplus

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#define __attribute_malloc__ __attribute__((__malloc__))

/* If fortification mode, we warn about unused results of certain
   function calls which can lead to problems.  */
#define __attribute_warn_unused_result__ \
	__attribute__((__warn_unused_result__))
#if __USE_FORTIFY_LEVEL > 0
#define __wur __attribute_warn_unused_result__
#endif  //	__USE_FORTIFY_LEVEL > 0
#ifndef __wur
#define __wur /* Ignore */
#endif        //	__wur

#endif  //	__SYS_CDEFS_H_