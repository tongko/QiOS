/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include "paging.h"
#include "asm.h"

//static const uint64_t kvirt_base = 0xC0000000;
//static const uint32_t kpage_num = 768;

//	id_paging
//	@param1 - First page table entry to begin mapping with
//	@param2 - Virtual address to begin with
//	@param3 - Number of bytes to map
void id_paging(uint64_t *first_pg_entry, vaddr_t from, size_t count) {
	//	mask out unwanted bits
	from = from & 0xFFFFF000;
	for (; count > 0; from += 0x1000, count -= 0x1000, first_pg_entry++) {
		*first_pg_entry = from | 1;  // mark page present
	}
}

void map_page(vaddr_t from, size_t count, paddr_t physical) {
	// Page schema for 32 bits PAE set, PSE unset:
	//	2 | 9 | 9 | 12

	for (; count > 0; from += 0x1000, physical += 0x1000, count -= 0x1000) {
		uint32_t p = (from & PDPTR_MASK) >> 30;
		uint32_t d = (from & PD_MASK) >> 21;
		uint32_t g = (from & PT_MASK) >> 12;

		page_dir_ptr_tab[p] |= 1;
		page_dir[ENTRY_SIZE * p + d] |= 3;
		uint64_t *pgaddr = &(page_dir[ENTRY_SIZE * p + d]);
		pgaddr[g] = physical | 1;
	}
	// uint32_t ptr_no = (from & PDPTR_MASK) >> 30;
	// uint32_t dir_no = (ENTRY_SIZE * ptr_no)(from & PD_MASK) >> 21;
	// uint32_t pt_no = (from & PT_MASK) >> 12;

	// page_dir_ptr_tab[ptr_no] |= 1;  // mark present
	// page_dir[ENTRY_SIZE * ptr_no + dir_no] |= 3;
	// //uint64_t *pg_dir = (uint64_t *)(uint32_t)page_dir_ptr_tab[ptr_no];  //(uint64_t *)((uint32_t)&page_dir + (ENTRY_SIZE * ptr_no));
	// //pg_dir[dir_no] |= 3;                                                // mark present + R/W
	// uint64_t *pg_tab = page_tab[(ENTRY_SIZE * ptr_no) + (ENTRY_SIZE * dir_no)];
	// //uint64_t *pg_tab = (uint64_t *)(uint32_t)pg_dir[dir_no];  //(uint64_t *)((uint32_t)&page_tab + (ENTRY_SIZE * ENTRY_SIZE * dir_no));
	// //pg_tab += pt_no;                                          // goto page number
	// for (; count > 0; from += 0x1000, physical += 0x1000, count -= 0x1000, pg_tab++) {
	// 	*pg_tab = physical | 1;  // set physical addr and mark R/W
	// }
}

__attribute__((section(".boot"))) static uint64_t virt_to_phys(vaddr_t vaddr) {
	uint64_t *page_dir = (uint64_t *)(uint32_t)(page_dir_ptr_tab[(vaddr & PDPTR_MASK) >> 30] & ~1);
	uint64_t *page_tab = (uint64_t *)(uint32_t)(page_dir[(vaddr & PD_MASK) >> 21] & ~1);
	uint64_t ret = page_tab[(vaddr & PT_MASK) >> 12] + (vaddr & 0xFFF);
	return ret;
}

void early_init_paging(kernel_mem_info_t kmem_info) {
	// init paging
	uint32_t i, j;
	uint64_t *pd_addr;
	uint64_t *pt_addr;
	for (i = 0; i < 4; i++) {
		pd_addr = (uint64_t *)(&(page_dir) + (ENTRY_SIZE * i));
		page_dir_ptr_tab[i] = (uint32_t)pd_addr;
		pt_addr = (uint64_t *)(&(page_tab) + (i * ENTRY_SIZE * ENTRY_SIZE));
		for (j = 0; j < ENTRY_SIZE; j++, pd_addr++, pt_addr += ENTRY_SIZE) {
			*(pd_addr) = (uint32_t)(pt_addr);
		}
	}
	// Since these tables are mark 4B and 4KiB aligned respectively, no masking are needed
	page_dir_ptr_tab[0] |= 1;  // mark first page dir page present
	page_dir[0] |= 3;          // mark first page table R/W + present

	// identity map first 4MiB
	id_paging(page_tab, (vaddr_t)0, 0x00400000);
	if (virt_to_phys(0x180000) != 0x180000) {
		while (1) {
		}  // error
	}

	uint64_t *pg_dir = (uint64_t *)(uint32_t)(page_dir_ptr_tab[3] & ~1);  // get the page directory (you should 'and' the flags away)
	pg_dir[511] = (uint64_t)(uint32_t)page_dir;                           // map pd to itself
	pg_dir[510] = page_dir_ptr_tab[2];                                    // map pd3 to it
	pg_dir[509] = page_dir_ptr_tab[1];                                    // map pd2 to it
	pg_dir[508] = page_dir_ptr_tab[0];                                    // map pd1 to it
	pg_dir[507] = (uint64_t)(uint32_t)&page_dir_ptr_tab;                  // map the PDPT to the directory

	//	map higher half to page, start from 0xC0100000
	uint32_t offset = kmem_info.physical_end - kmem_info.physical_start;
	map_page((vaddr_t)KERNEL_VIRTUAL_BASE, offset, kmem_info.physical_start);
	if (virt_to_phys(KERNEL_VIRTUAL_BASE) != kmem_info.physical_start) {
		while (1) {
		}  //error
	}

	// Enable paging
	set_cr3((uint32_t)&page_dir_ptr_tab);

	uint32_t cr4 = get_cr4();
	cr4 |= 0x20;
	set_cr4(cr4);

	uint32_t cr0 = get_cr0();
	cr0 |= 0x80000000;
	set_cr0(cr0);
}