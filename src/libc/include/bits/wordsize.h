/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
/*******************************************************************************
 * 	This is copy from libgcc glibc/sysdeps/x86/bits/wordsize.h                 *
 ******************************************************************************/
/* Determine the wordsize from the preprocessor defines.  */

#if defined __x86_64__ && !defined __ILP32__
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#define __WORDSIZE32_SIZE_ULONG 0
#define __WORDSIZE32_PTRDIFF_LONG 0
#endif

#ifdef __x86_64__
#define __WORDSIZE_TIME64_COMPAT32 1
/* Both x86-64 and x32 use the 64-bit system call interface.  */
#define __SYSCALL_WORDSIZE 64
#else
#define __WORDSIZE_TIME64_COMPAT32 0
#endif