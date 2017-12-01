/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __PAGING_H_
#define __PAGING_H_

#define _need_SIZE_T
#include "ktypedef.h"

#define PDPTR_MASK 0xC0000000  // 1100 0000 0000 0000 0000 0000 0000 0000
#define PD_MASK 0x3FE00000     // 0011 1111 1110 0000 0000 0000 0000 0000
#define PT_MASK 0x001FF000     // 0000 0000 0001 1111 1111 0000 0000 0000
#define PO_MASK 0x00000FFF     // 0000 0000 0000 0000 0000 1111 1111 1111
#define PAGE_SIZE 0x00001000   // 4 KIB
#define ENTRY_SIZE 0x00000200  // 512 entries
#define PT_SIZE_IN_BYTE (ENTRY_SIZE * sizeof(uint64_t))
#define PD_SIZE_IN_BYTE (ENTRY_SIZE * PT_SIZE_IN_BYTE)
#define KERNEL_VIRTUAL_BASE 0xC0100000
#define KERNEL_PHYSICAL_BASE 0x00100000

typedef struct kernel_mem_info {
	uint32_t physical_start;
	uint32_t physical_end;
	uint32_t virtual_start;
	uint32_t virtual_end;
} kernel_mem_info_t;

__attribute__((aligned(0x20), section(".bootdata"))) uint64_t page_dir_ptr_tab[4];
__attribute__((aligned(0x4000), section(".bootdata"))) uint64_t page_dir[ENTRY_SIZE];  // must be aligned to page boundry
__attribute__((aligned(0x200000), section(".bootdata"))) uint64_t page_tab[ENTRY_SIZE];

__attribute__((section(".boot"))) void id_paging(uint64_t *first_pg_entry, vaddr_t from, size_t count);
__attribute__((section(".boot"))) void map_page(vaddr_t from, size_t count, paddr_t physical);
__attribute__((section(".boot"))) void early_init_paging(kernel_mem_info_t kmem_info);

#endif  //	__PAGING_H_