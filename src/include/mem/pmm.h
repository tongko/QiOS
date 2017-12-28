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
#include <kdef.h>
#include <stdint.h>

//	Physical Memory Address
typedef void *paddr_t;

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

void __early pmm_init(size_t mem_size, paddr_t table);
void __early pmm_init_region(paddr_t base, size_t size, bool set);
void __early *pmm_alloc_block(void);
void __early *pmm_alloc_blocks(size_t size);
void __early pmm_free_block(void *p);
void __early pmm_free_blocks(void *p, size_t size);
uint32_t __early pmm_get_block_count(void);
uint32_t __early pmm_get_used_block(void);

#endif  //	__PMM_H_