/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <attribs.h>
#include <kernel/errors.h>
#include <mem/paging.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <stdbool.h>

uint32_t vmm_alloc_blocks(vaddr_t virt, uint32_t block_count) {
	if (block_count == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	//	addr must be align to PAGE_SIZE
	vaddr_t v = _ALIGN_DW(virt, PAGE_SIZE);
	paddr_t p;

	if (block_count == 1) {
		p = pmm_alloc_block();
	} else {
		p = pmm_alloc_blocks(block_count);
	}

	pg_map_addr(v, p, block_count);
}

void vmm_init() {
	pg_init();
	pg_enable(true);
	pg_flush_tlb();
}
