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
/*-----------------------------------------------------------------------------*
*																			   *
*	For Debugging sake, this code is not being optimize at all.				   *
*	1. Can directly obtain entry address instead of                            *
*		a. Get array                                                           *
*		b. Get entry base on offset           							       *
*																			   *
*-----------------------------------------------------------------------------*/

#include <asm.h>
#include <kstr.h>
#include <mem/paging.h>
#include <sys/panic.h>

#define PAGE_SHIFT 12  // We want a 4 KiB page!
#define PAGE_DIR_PTR_SIZE 4
#define PAGE_DIR_SIZE 512
#define PAGE_TAB_SIZE 4096
#define ENTRY_MASK 0xFFFFFFFFFF000UL  // 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 0000 0000 0000
#define PDPTR_MASK 0xC0000000         // 1100 0000 0000 0000 0000 0000 0000 0000
#define PD_MASK 0x3FE00000            // 0011 1111 1110 0000 0000 0000 0000 0000
#define PT_MASK 0x001FF000            // 0000 0000 0001 1111 1111 0000 0000 0000
#define PO_MASK 0x00000FFF            // 0000 0000 0000 0000 0000 1111 1111 1111
#define ENTRY_SIZE 0x00000200         // 512 entries
#define PT_SIZE_IN_BYTE (PAGE_DIR_SIZE * sizeof(uint64_t))
#define PD_SIZE_IN_BYTE (PAGE_TAB_SIZE * sizeof(uint64_t))
#define KERNEL_VIRTUAL_BASE 0xC0100000
#define KERNEL_PHYSICAL_BASE 0x00100000

//	PDPTE flags
#define PG_PRESENT 0x0001
#define PG_READWRITE 0x0002
#define PG_USERACCESS 0x0004
#define PG_PWT 0x0008
#define PG_PCD 0x0010
#define PG_ACCESSED 0x0020
#define PG_DIR_PAGESIZE 0x0080
#define PG_TAB_DIRTY 0x0040
#define PG_TAB_PAT 0x0080
#define PG_TAB_GLOBAL 0x0100

//	Round up to nearest y
#define ROUNDUP(x, y) (x % y ? x + (y - (x % y)) : x)
//	Round down to nearest y
#define ROUNDDW(x, y) x - (x % y)

#define _TO_ENTRY(x) ((uint64_t)((uint32_t)x & ENTRY_MASK))
//#define _TAB_ENTRY_NO(x) (uint32_t)(((x & PT_MASK) >> 9) & ~3)
#define _TAB_ENTRY_OFFSET(x) (uint32_t)((x & PT_MASK) >> 12)
//#define _DIR_ENTRY_NO(x) (uint32_t)(((x & PD_MASK) >> 18) & ~3)
#define _DIR_ENTRY_OFFSET(x) (uint32_t)((x & PD_MASK) >> 21)
#define _DIR_PTR_NO(x) (uint32_t)((x & PDPTR_MASK) >> 30)
#define _PGE_ENTRY_NO(x) (uint32_t)(x & 0xFFF)

#define _GET_DIR(x) (pg_dir_t *)(uint32_t)(x & ENTRY_MASK)
#define _GET_TAB(x) (pg_tab_t *)(uint32_t)(x & ENTRY_MASK)

//#define _GET_DIR_ENTRY(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | _DIR_ENTRY_NO(y))
//#define _GET_TAB_ENTRY(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | _TAB_ENTRY_NO(y))
#define _GET_PGE_ENTRY(x, y) (paddr_t)((x & ENTRY_MASK) | _PGE_ENTRY_NO(y))

typedef struct {
	uint64_t entries[PAGE_DIR_PTR_SIZE];
} pdpte_t;

typedef uint64_t pg_dir_e_t;

typedef struct {
	pg_dir_e_t entries[PAGE_DIR_PTR_SIZE];
} pg_dir_t;

typedef uint64_t pg_tab_e_t;

typedef struct {
	pg_tab_e_t entries[PAGE_TAB_SIZE];
} pg_tab_t;

//	Current Page Directory
pg_dir_t *_curr_dir;
//	Current Page Table
pg_tab_t *_curr_tab;
//	PDPTE consists of 4 entries that each point to a page directory
pdpte_t _pg_dir_ptr;

static __inline__ void set_entry_present(uint64_t *entry, bool set) {
	if (set) {
		*entry |= PG_PRESENT;
	} else {
		*entry &= ~PG_PRESENT;
	}
}

static __inline__ void set_entry_usermode(uint64_t *entry, bool has_access) {
	if (has_access) {
		*entry |= PG_USERACCESS;
	} else {
		*entry &= ~PG_USERACCESS;
	}
}

static __inline__ bool entry_is_dirty(pg_tab_e_t *entry) {
	return *entry & PG_TAB_DIRTY;
}

static __inline__ bool entry_is_accessed(pg_tab_e_t *entry) {
	return *entry & PG_ACCESSED;
}

static __inline__ void reset_entry(pg_tab_e_t *entry) {
	*entry &= ~PG_TAB_DIRTY;
	*entry &= ~PG_ACCESSED;
	_asm(
	    "mov eax, cr3\n"
	    "mov cr3, eax\n" ::);
}

static void *init_page(int size) {
	uintptr_t page = 0;
	int blocks = size / PMM_BLOCK_SIZE;

	if (size % PMM_BLOCK_SIZE) {
		blocks++;
	}

	if (blocks == 1) {
		page = pmm_alloc_block();
	} else {
		page = pmm_alloc_blocks(blocks);
	}

	if (!page) {
		kernel_panic("Out of memory.\nAllocating size: %#.8x Bytes\n", size);
	}

	memset((void *)page, 0, blocks * PMM_BLOCK_SIZE);
	return (void *)page;
}

paddr_t virt_to_phys(vaddr_t vaddr) {
	uint64_t pdptr_e = _pg_dir_ptr.entries[_DIR_PTR_NO(vaddr)];
	if (!(pdptr_e & PG_PRESENT)) {  // directory not present
		return 0;
	}

	pg_dir_t *dir = _GET_DIR(pdptr_e);
	if (!dir) {
		return 0;
	}

	pg_dir_e_t *dir_e = &(dir->entries[_DIR_ENTRY_OFFSET(vaddr)]);
	if (!dir_e || !(*dir_e & PG_PRESENT)) {
		return 0;
	}

	pg_tab_t *tab = _GET_TAB(*dir_e);
	if (!tab) {
		return 0;
	}

	pg_tab_e_t *tab_e = &(tab->entries[_TAB_ENTRY_OFFSET(vaddr)]);
	if (!tab_e || !(*tab_e & PG_PRESENT)) {
		return 0;
	}

	paddr_t ret = _GET_PGE_ENTRY(*tab_e, vaddr);
	return ret;
}

/*--------------------------------------------------------------
*	Function pg_switch
*	Param:
*	paddr_t new_paging - The physical address of a new PDPTE
*	Return:
*	The physical address of existing PDPTE
---------------------------------------------------------------*/
paddr_t pg_load(paddr_t new_paging) {
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
void pg_enable(bool enable) {
	_asm(
	    //"mov eax, cr4\n"
	    "mov	eax, 0x20\n"  //	set bit 5
	    "mov	cr4, eax\n"
	    "mov	eax, cr0\n"
	    "cmp	%0, 0\n"
	    "je		disable\n"
	    "jmp	enable\n"
	    "disable:\n"
	    "and	eax, 0x7FFFFFFF\n"  //	Clear bit 31
	    "mov	cr0, eax\n"
	    "jmp	done\n"
	    "enable:\n"
	    "or		eax, 0x80000000\n"  //	Set Bit 31
	    "mov	cr0, eax\n"
	    "done:" ::"r"(enable)
	    : "memory");
}

/*----------------------------------------------------------
*	Function pg_flush_tlb
-----------------------------------------------------------*/
void pg_flush_tlb() {
	_asm(
	    "mov eax, cr3\n"
	    "mov cr3, eax\n" ::);
}

/*-----------------------------------------------------------
*	function pg_flush_tlb_entry
*	Param:
*	vaddr_t vaddr - The virtual address to flush from TLB
-----------------------------------------------------------*/
void pg_flush_tlb_entry(vaddr_t vaddr) {
	if (!vaddr) {
		return;
	}

	_asm(
	    "cli\n"
	    "invlpg [%0]\n"
	    "sti\n"
	    :
	    : "r"(vaddr)
	    : "memory");
}

/*------------------------------------------------------------
*	function pg_map_addr
*	Param:
*	vaddr_t vaddr - The virtual address which to map to physical address
*	paddr_t paddr - The physical address which to be map to
*	uint32_t page_count - Number of pages to map.
*	all the parameters value must be page size aligned, i.e. starting
*	at page boundry.
-------------------------------------------------------------------*/
void pg_map_addr(vaddr_t vaddr, paddr_t paddr, uint32_t page_count) {
	uint32_t v = (uint32_t)vaddr;
	if ((v % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Virtual address '%#x' not aligned to page boundry.", v);
	}

	uint32_t p = (uint32_t)paddr;
	if ((p % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Physical address '%#x' not aligned to page boundry.", p);
	}

	if ((page_count % PAGE_SIZE) != 0) {
		kernel_panic("pg_map_addr: Length '%d' not aligned to page boundry.", page_count);
	}

	for (; page_count > 0; v += 0x1000, p += 0x1000, page_count -= 0x1000) {
		int p = _DIR_PTR_NO(v);
		//	Get PDPTE entry;
		uint64_t *pdptr_e = &(_pg_dir_ptr.entries[p]);
		pg_dir_t *dir = NULL;

		// check PDPTE entry, if P flag is not mark present, or pdpte is zero
		if (!(*pdptr_e & PG_PRESENT && *pdptr_e & ENTRY_MASK)) {
			//	PDPTR not define, let's make 1 for it.
			dir = (pg_dir_t *)init_page(PD_SIZE_IN_BYTE);
			*pdptr_e = _TO_ENTRY(dir);
			set_entry_present(pdptr_e, true);
			_curr_dir = dir;
		} else {
			dir = _GET_DIR(*pdptr_e);
		}

		//	retrieve page dir entry base on dir first element and [v]
		p = _DIR_ENTRY_OFFSET(v);
		pg_dir_e_t *dir_e = &(dir->entries[p]);
		pg_tab_t *tab = NULL;

		if (!(*dir_e & PG_PRESENT && *dir_e & ENTRY_MASK)) {
			//	page table not define, make 1 for it.
			tab = (pg_tab_t *)init_page(PT_SIZE_IN_BYTE);
			*dir_e = _TO_ENTRY(tab);
			set_entry_present(dir_e, true);
			//	allow user mode access
			set_entry_usermode(dir_e, true);
			_curr_tab = tab;
		} else {
			tab = _GET_TAB(*dir_e);
		}

		//	retrieve page table entry base on tab first element and [v]
		p = _TAB_ENTRY_OFFSET(v);
		pg_tab_e_t *tab_e = &(tab->entries[p]);

		if (!(*tab_e & PG_PRESENT && *tab_e & ENTRY_MASK)) {
			tab->entries[p] = _TO_ENTRY(paddr);
			set_entry_present(tab_e, true);
			//	allow user mode access
			set_entry_usermode(tab_e, true);
		}
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
void pg_init() {
	//	Initialize pdpte to zero
	memset(&_pg_dir_ptr, 0, sizeof(pdpte_t));

	//	allocate dir, this is for higher half
	pg_dir_t *dir = (pg_dir_t *)pmm_alloc_block();
	memset(dir, 0, PAGE_SIZE);

	_pg_dir_ptr.entries[3] = _TO_ENTRY(dir);
	set_entry_present(&(_pg_dir_ptr.entries[3]), true);
	_curr_dir = dir;

	//	allocate table, this is for higher half
	pg_tab_t *tab = (pg_tab_t *)pmm_alloc_blocks(8);
	memset(tab, 0, PAGE_SIZE * 8);

	dir->entries[0] = _TO_ENTRY(tab);
	set_entry_present(&(dir->entries[0]), true);
	set_entry_usermode(&(dir->entries[0]), true);
	_curr_tab = tab;

	//	0xC0100000 mapped to 0x100000
	paddr_t p = 0x100000;
	for (int i = 256; i < 1024; i++, p += 4096) {
		tab->entries[i] = _TO_ENTRY(p);
		set_entry_present(&(tab->entries[i]), true);
		set_entry_usermode(&(tab->entries[i]), true);
	}

	//	load the new PDPTR to CR03
	pg_load((paddr_t)&_pg_dir_ptr);
}

//	This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
//	All bits are clear except the following:
//	bit 7: PS The kernel page is 4MB.
//	bit 1: RW The kernel page is read/write.
//	bit 0: P  The kernel page is present.
//	This entry must be set -- otherwise the kernel will crash immediately after paging is
//	enabled because it can't fetch the next instruction! It's ok to unmap this page later.
void __early pg_early_init() {
	//	hard code page directory at offset 0x6400000 (100 MiB)
	uint32_t *dir = (uint32_t *)0x6400000;

	//	Map first 4MiB
	dir[0] = 0x83;  //	4MiB page, Present, Read Write
	//	Map 0x100000 to 0xC0000000
	dir[768] = 0x83;  // Present, Read Write

	//	Enable Paging
	_asm(
	    "mov	ecx, %0\n"
	    "mov	cr3, ecx\n"
	    "mov	ecx, cr4\n"
	    "or		ecx, 0x00000010\n"
	    "mov	cr4, ecx\n"
	    "mov	ecx, cr0\n"
	    "or		ecx, 0x80000000\n"
	    "mov	cr0, ecx\n"
	    :
	    : "r"(0x6400000)
	    : "memory");
}

/*
uint64_t __align(0x1000) page_dir[ENTRY_SIZE * 4];  // must be aligned to page boundry
uint64_t __align(0x1000) page_tab[ENTRY_SIZE * ENTRY_SIZE * 4];

void map_page(vaddr_t from, size_t count, paddr_t physical) {
	size_t c = count;
	// Page schema for 32 bits PAE set, PSE unset:
	//	2 | 9 | 9 | 12
	for (; c > 0; from += 0x1000, physical += 0x1000, c -= 0x1000) {
		uint32_t p = (from & PDPTR_MASK) >> 30;  // _ptrn(from & PDPTR_MASK);
		uint64_t pdpte_i = page_dir_ptr_tab[p];
		// check PDPTEi, if P flag is not 1, or pdpte is zero
		if (!(pdpte_i & 1) || !(pdpte_i & ENTRY_MASK)) {
			pdpte_i = page_dir_ptr_tab[p] = _TO_ENTRY(last_page_dir) | 1;
			last_page_dir += ENTRY_SIZE;
		}
		//	retrieve page dir entry base on dir first element and [from]
		uint64_t *pg_dir_entry = _GET_DIR_ENTRY(pdpte_i, from);
		//	Page table should not be zero
		if (!(*pg_dir_entry & 1) || !(*pg_dir_entry & ENTRY_MASK)) {
			*pg_dir_entry = _TO_ENTRY(last_page_tab) | 3;
			last_page_tab += ENTRY_SIZE;
		}
		uint64_t *pg_tab_entry = _GET_TAB_ENTRY(*pg_dir_entry, from);
		*pg_tab_entry = _TO_ENTRY(physical) | 3;
	}
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
	set_cr3((uint32_t)&_pg_dir_ptr);

	// set CR4.PAE bit
	uint32_t cr4 = get_cr4();
	cr4 |= 0x20;
	set_cr4(cr4);

	// set CR0.PG bit
	uint32_t cr0 = get_cr0();
	cr0 |= 0x80000000;
	set_cr0(cr0);
}

__earlydata uint32_t efer;
uint32_t debug(uint32_t eflags, uint32_t cpuid_enabled) {
	if (!cpuid_enabled) {
		return eflags;
	}

	_asm(
	    "mov eax, 0x00;\n"
	    "cpuid;\n" ::);
	_asm(
	    "mov eax, 0x80000008;\n"
	    "cpuid;\n" ::);
	efer = 0;
	_asm(
	    "mov ecx, 0xC0000080;\n"
	    "rdmsr;\n"
	    : "=r"(efer)
	    :);
	uint32_t efer_lma = ~(1U << 10);
	_asm(
	    "and eax, %0;\n"
	    "wrmsr"
	    :
	    : "r"(efer_lma)
	    :);
	// disable EFER.LMA
	_asm("nop;\n" ::);

	return eflags;
}
*/