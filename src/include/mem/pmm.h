/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __PMM_H_
#define __PMM_H_

#include <attribs.h>
#include <kbool.h>
#include <stddef.h>
#include <stdint.h>

//	Physical Memory Address
typedef uintptr_t paddr_t;

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

void pmm_init(size_t mem_size, paddr_t table);
void pmm_init_region(paddr_t base, size_t size, bool set);
paddr_t pmm_alloc_block(void);
paddr_t pmm_alloc_blocks(size_t size);
void pmm_free_block(paddr_t p);
void pmm_free_blocks(paddr_t p, size_t size);
uint32_t pmm_get_block_count(void);
uint32_t pmm_get_used_block(void);

#endif  //	__PMM_H_