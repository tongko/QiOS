/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
/*-----------------------------------------------------------------------------*
 *                                                                             *
 *    Paging Implementation                                                    *
 *    ---------------------                                                    *
 *    This module implements the interfaces define in <mem/paging.h>. This     *
 *    implementation assume host system supports PAE, thus will initialize     *
 *    the paging to create a page directory pointer which hold 3 page          *
 *    directory. Each of these directory consists of 4096 page table. Each     *
 *    page table in turn consists of 4096 entries. Each entries point to a     *
 *    physical address which is the begining of a 4096 KiB block.              *
 *                                                                             *
 * ---------------------------------------------------------------------------*/
#include <asm.h>
#include <kstr.h>
#include <mem/paging.h>
#include <sys/panic.h>

#define PAGE_DIR_PTR_SIZE 4
#define PAGE_DIR_SIZE 512
#define PAGE_TAB_SIZE 4096
#define ENTRY_MASK 0xFFFFFFFFFF000UL  // 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 0000 0000 0000
#define PDPTR_MASK 0xC0000000         // 1100 0000 0000 0000 0000 0000 0000 0000
#define PD_MASK 0x3FE00000            // 0011 1111 1110 0000 0000 0000 0000 0000
#define PT_MASK 0x001FF000            // 0000 0000 0001 1111 1111 0000 0000 0000
#define PO_MASK 0x00000FFF            // 0000 0000 0000 0000 0000 1111 1111 1111
#define PAGE_SIZE 0x00001000          // 4 KIB
#define ENTRY_SIZE 0x00000200         // 512 entries
#define PT_SIZE_IN_BYTE (ENTRY_SIZE * sizeof(uint64_t))
#define PD_SIZE_IN_BYTE (ENTRY_SIZE * PT_SIZE_IN_BYTE)
#define KERNEL_VIRTUAL_BASE 0xC0100000
#define KERNEL_PHYSICAL_BASE 0x00100000

//	PDPTE flags
#define PDPTE_FLAG_PRESENT 0x01

//	Round up to nearest y
#define ROUNDUP(x, y) (x % y ? x + (y - (x % y)) : x)
//	Round down to nearest y
#define ROUNDDW(x, y) x - (x % y)

#define __to_entry(x) ((uint64_t)((uint32_t)x & ENTRY_MASK))
#define __tabn(x) (uint32_t)(((x & PT_MASK) >> 9) & ~3)
#define __dirn(x) (uint32_t)(((x & PD_MASK) >> 18) & ~3)
#define __ptrn(x) (uint32_t)((x & PDPTR_MASK) >> 30)
#define __pfn(x) (uint32_t)(x & 0xFFF)
#define __getd(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __dirn(y))
#define __gett(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __tabn(y))
#define __getp(x, y) (paddr_t)((x & ENTRY_MASK) | __pfn(y))

typedef struct {
	uint64_t entries[PAGE_DIR_PTR_SIZE];
} pdpte_t;

typedef struct {
	pg_dir_e_t entries[PAGE_DIR_PTR_SIZE];
} pg_dir_t;

typedef uint64_t pg_dir_e_t;

typedef struct {
	pg_tab_e_t entries[PAGE_TAB_SIZE];
} pg_tab_t;

typedef uint64_t pg_tab_e_t;

//	Current Page Directory
pg_dir_t *_curr_dir __earlydata;
//	Current Page Table
pg_tab_t *_curr_tab __earlydata;
//	PDPTE consists of 4 entries that each point to a page directory
pdpte_t pg_dir_ptr __earlydata;

/*--------------------------------------------------------------
*	Function pg_switch
*	Param:
*	paddr_t new_paging - The physical address of a new PDPTE
*	Return:
*	The physical address of existing PDPTE
---------------------------------------------------------------*/
paddr_t __early pg_load(paddr_t new_paging) {
	uint32_t old_addr;
	_asm(
	    "mov %0, cr3\n"
	    "mov eax, %1\n"
	    "mov cr3, eax\n"
	    : "=r"(old_addr)
	    : "r"(new_paging));

	return old_addr;
}

/*--------------------------------------------------------------
*	Function pg_enable
*	Param:
*	bool enable - true to enable paging, else, disable paging.
--------------------------------------------------------------*/
void __early pg_enable(bool enable) {
	_asm(
	    "mov eax, cr4\n"
	    "or eax, 0x20\n"  //	set bit 5
	    "mov cr4, eax\n"
	    "mov eax, cr0\n"
	    "cmp %0, 0\n"
	    "je  disable\n"
	    "jmp enable\n"
	    "disable:\n"
	    "and eax, 0x7FFFFFFF\n"  //	Clear bit 31
	    "mov cr0, eax\n"
	    "jmp done\n"
	    "enable:\n"
	    "or eax, 0x80000000\n"  //	Set Bit 31
	    "mov cr0, eax\n"
	    "done:" ::"r"(enable));
}

/*-----------------------------------------------------------
*	function pg_flush_tlb_entry
*	Param:
*	vaddr_t vaddr - The virtual address to flush from TLB
-----------------------------------------------------------*/
void __early pg_flush_tlb_entry(vaddr_t vaddr) {
	if (!vaddr) {
		return;
	}

	_asm(
	    "cli\n"
	    "invlpg %0\n"
	    "sti\n" ::"r"(vaddr));
}

/*------------------------------------------------------------
*	function pg_map_addr
*	Param:
*	vaddr_t vaddr - The virtual address which to map to physical address
*	paddr_t paddr - The physical address which to be map to
*	uint32_t length - The length to map.
*	all the parameters value must be page size aligned, i.e. starting
*	at page boundry.
-------------------------------------------------------------------*/
void pg_map_addr(vaddr_t vaddr, paddr_t paddr, uint32_t length) {
	uint32_t v = (uint32_t)vaddr;
	if ((v % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Virtual address '%#x' not aligned to page boundry.", v);
	}

	uint32_t p = (uint32_t)paddr;
	if ((p % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Physical address '%#x' not aligned to page boundry.", p);
	}

	if ((length % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Length '%d' not aligned to page boundry.", length);
	}

	for (; length > 0; v += 0x1000, p += 0x1000, length -= 0x1000) {
		int p = __ptrn(v);
		uint64_t pdptr_i = pg_dir_ptr.entries[p];
		// check PDPTEi, if P flag is not 1, or pdpte is zero
		if (!(pdptr_i & PDPTE_FLAG_PRESENT && pdptr_i & ENTRY_MASK)) {
			//	PDPTR not define, let's make 1 for it.
			_curr_dir = pmm_alloc_block();
			pg_dir_ptr.entries[p] = __to_entry(_curr_dir) | PDPTE_FLAG_PRESENT;
		}
		//	retrieve page dir entry base on dir first element and [from]
	}
}

/*-------------------------------------------------------------
*	function pg_init
*	Before we can enable paging, we must ensure proper paging
*	has been constructed and place in CR3. Failing to do so
*	will send the kernel to hell immediately.
*
*	1) Setup identity page for first 4 MiB which includes all
*		BIOS area and the kernel space.
*	2) Setup Higher Half for kernel
*	3) Setup for modules
*	4) Call pg_load by passing the PDPTE we've just setup
*	5) Call pg_enable passing true.
-------------------------------------------------------------*/
void __early pg_init() {
	//	Initialize pdpte to zero
	memset(&pg_dir_ptr, 0, sizeof(pdpte_t));
	//	allocate first dir, this is for first 4 MiB ID mapped
	pg_dir_t *dir = (pg_dir_t *)pmm_alloc_block();
	pg_dir_ptr.entries[0] = __to_entry(dir) | PDPTE_FLAG_PRESENT;
	_curr_dir = dir;

	//	allocate first page table, this is for first 4 MiB ID mapped
	pg_tab_t *tab = (pg_tab_t *)pmm_alloc_blocks(8);  //	Need 8 blocks per page.
	dir->entries[0] = __to_entry(tab) | 3;

	//	First 4 MiB are identity mapped
	vaddr_t v = 0;
	for (int i = 0; i < 1024; i++, v += 4096) {
		tab->entries[0] = __to_entry(v) | 3;
	}
}

// uint64_t __align(0x1000) __earlydata page_dir[ENTRY_SIZE * 4];  // must be aligned to page boundry
// uint64_t __align(0x1000) __earlydata page_tab[ENTRY_SIZE * ENTRY_SIZE * 4];

void map_page(vaddr_t from, size_t count, paddr_t physical) {
	size_t c = count;
	// Page schema for 32 bits PAE set, PSE unset:
	//	2 | 9 | 9 | 12
	for (; c > 0; from += 0x1000, physical += 0x1000, c -= 0x1000) {
		uint32_t p = (from & PDPTR_MASK) >> 30;  // _ptrn(from & PDPTR_MASK);
		uint64_t pdpte_i = page_dir_ptr_tab[p];
		// check PDPTEi, if P flag is not 1, or pdpte is zero
		if (!(pdpte_i & 1) || !(pdpte_i & ENTRY_MASK)) {
			pdpte_i = page_dir_ptr_tab[p] = __to_entry(last_page_dir) | 1;
			last_page_dir += ENTRY_SIZE;
		}
		//	retrieve page dir entry base on dir first element and [from]
		uint64_t *pg_dir_entry = __getd(pdpte_i, from);
		//	Page table should not be zero
		if (!(*pg_dir_entry & 1) || !(*pg_dir_entry & ENTRY_MASK)) {
			*pg_dir_entry = __to_entry(last_page_tab) | 3;
			last_page_tab += ENTRY_SIZE;
		}
		uint64_t *pg_tab_entry = __gett(*pg_dir_entry, from);
		*pg_tab_entry = __to_entry(physical) | 3;
	}
}

static __early paddr_t virt_to_phys(vaddr_t vaddr) {
	uint64_t pdptr_e = page_dir_ptr_tab[__ptrn(vaddr)];
	if ((pdptr_e & 1) == 0) {  // directory not present
		return 0;
	}
	uint64_t *pg_dir = __getd(pdptr_e, vaddr);
	if ((*pg_dir & 1) == 0) {
		return 0;
	}
	uint64_t *pg_tab = __gett(*pg_dir, vaddr);
	if ((*pg_tab & 1) == 0) {
		return 0;
	}
	paddr_t ret = __getp(*pg_tab, vaddr);
	return ret;
}

void early_init_paging(kernel_mem_info_t kmem_info, uint32_t mb2_addr) {
	last_page_dir = page_dir;
	last_page_tab = page_tab;

	uint32_t ksize = kmem_info.physical_end - kmem_info.physical_start;
	// identity map first 1 MiB + kernel size.
	// round up to nearest page boundry
	uint32_t offset = ROUNDUP(0x100000 + ksize, 4096);
	map_page((vaddr_t)0, offset, (paddr_t)0);
	if (virt_to_phys(0x130000) != 0x130000) {
		while (1) {
		}  // error
	}

	//	identity map multiboot info loaded address.
	uint32_t mb2_loaded = ROUNDDW(mb2_addr, 4096);
	uint32_t size = *((uint32_t *)((void *)mb2_addr));
	offset = ROUNDUP(size, 4096);
	map_page((vaddr_t)mb2_loaded, offset, (paddr_t)mb2_loaded);

	//	map higher half to page, start from 0xC0100000
	offset = ROUNDUP((kmem_info.physical_end - kmem_info.physical_start) + 0xB00000, 4096);  // + 0x100000;
	vaddr_t kvb = (vaddr_t)ROUNDUP(KERNEL_VIRTUAL_BASE, 4096);
	paddr_t pss = (paddr_t)ROUNDUP(kmem_info.physical_start, 4096);
	map_page(kvb, offset, pss);
	if (virt_to_phys(KERNEL_VIRTUAL_BASE) != kmem_info.physical_start) {
		while (1) {
		}  //error
	}

	if (virt_to_phys(0xc0403c4c) != 0x00403c4c) {
		while (1) {
		}  // error
	}

	// move PDPTR to CR3
	set_cr3((uint32_t)&page_dir_ptr_tab);

	// set CR4.PAE bit
	uint32_t cr4 = get_cr4();
	cr4 |= 0x20;
	set_cr4(cr4);

	// set CR0.PG bit
	uint32_t cr0 = get_cr0();
	cr0 |= 0x80000000;
	set_cr0(cr0);
}

// __earlydata uint32_t efer;
// uint32_t __early debug(uint32_t eflags, uint32_t cpuid_enabled) {
// 	if (!cpuid_enabled) {
// 		return eflags;
// 	}

// 	_asm(
// 	    "mov eax, 0x00;\n"
// 	    "cpuid;\n" ::);
// 	_asm(
// 	    "mov eax, 0x80000008;\n"
// 	    "cpuid;\n" ::);
// 	efer = 0;
// 	_asm(
// 	    "mov ecx, 0xC0000080;\n"
// 	    "rdmsr;\n"
// 	    : "=r"(efer)
// 	    :);
// 	uint32_t efer_lma = ~(1U << 10);
// 	_asm(
// 	    "and eax, %0;\n"
// 	    "wrmsr"
// 	    :
// 	    : "r"(efer_lma)
// 	    :);
// 	// disable EFER.LMA
// 	_asm("nop;\n" ::);

// 	return eflags;
// }