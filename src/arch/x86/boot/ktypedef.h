/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __KTYPEDEF_H_
#define __KTYPEDEF_H_

#include <stdint.h>

#ifdef _need_SIZE_T
typedef uint32_t size_t;
#undef _need_SIZE_T
#endif  // _need_SIZE_T

#ifdef _need_NULL
#define NULL (void *)0;
#undef _need_NULL
#endif  // _need_NULL

//	type represent virtual address
typedef uint32_t vaddr_t;
//	type represent physical address
typedef uint32_t paddr_t;

#endif  // __KTYPEDEF_H_