/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __MEM_VMM_H_
#define __MEM_VMM_H_

#include <stdint.h>

//	Align to next y bytes
#define _ALIGN_UP(x, y) (x % y ? x + (y - (x % y)) : x)
//	Align to previous y bytes
#define _ALIGN_DW(x, y) x - (x % y)

//	Allocate new blocks for virtual address
uint32_t vmm_alloc_blocks(vaddr_t virt, uint32_t block_count);
//	Initialize Virtual Memory Manager
void vmm_init();

#endif  //	__MEM_VMM_H_