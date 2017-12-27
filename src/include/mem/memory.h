/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __MEMORY_H_
#define __MEMORY_H_

#include <attribs.h>

/*	Defines and Type Definitions */
typedef struct {
	paddr_t physical_start;
	paddr_t physical_end;
	vaddr_t virtual_start;
	vaddr_t virtual_end;
} kernel_meminfo_t;

__early void mm_init(kernel_meminfo_t kern_mem_info);
__early uint32_t mm_total_physical(void);
__early uint32_t mm_used_physical(void);

#endif  //	__MEMORY_H_