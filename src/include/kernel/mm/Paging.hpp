#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace mm {

extern const __vma_t P4_RECURSIVE_ENTRY;

typedef volatile uintptr_t *page_dir_t;
typedef volatile uintptr_t *page_tbl_t;

const dword_t PG_TBL_PRESENT = 1;
const dword_t PG_TBL_READWRITE = 2;
const dword_t PG_TBL_USERSPACE = 4;
const dword_t PG_TBL_WRITETHROUGH = 8;
const dword_t PG_TBL_CACHE_DISABLED = 16;
const dword_t PG_TBL_ACCESSED = 32;
const dword_t PG_TBL_SIZE = 64;

const dword_t PG_PRESENT = 1;
const dword_t PG_READWRITE = 2;
const dword_t PG_USERSPACE = 4;
const dword_t PG_WRITETHROUGH = 8;
const dword_t PG_CACHE_DISABLED = 16;
const dword_t PG_ACCESSED = 32;
const dword_t PG_DIRTY = 64;
const dword_t PG_GLOBAL = 128;

#define DEFAULT_TABLE_FLAGS (PG_TBL_PRESENT | PG_TBL_READWRITE)
#define DEFAULT_PAGE_FLAGS	(PG_PRESENT | PG_READWRITE | PG_GLOBAL)
#define DEFAULT_USER_TABLE_FLAGS \
	(PG_TBL_PRESENT | PG_TBL_READWRITE | PG_TBL_USERSPACE)
#define DEFAULT_USER_PAGE_FLAGS (PG_PRESENT | PG_READWRITE | PG_USERSPACE)

#define INDEX_MASK		 0x1FF
#define ALIGN_MASK		 0xFFF
#define GET_P4_IDX(addr) ((int_t)(addr >> 39) & INDEX_MASK)
#define GET_P3_IDX(addr) ((int_t)(addr >> 30) & INDEX_MASK)
#define GET_P2_IDX(addr) ((int_t)(addr >> 21) & INDEX_MASK)
#define GET_P1_IDX(addr) ((int_t)(addr >> 12) & INDEX_MASK)

/**
 * Convert virtual address to the currently mapped physical address.
 *
 * @param tVirt The virtual address to resolve.
 * @param[out] tPhys When function exits, contain physical address that 'tVirt'
 * is mapped to.
 * @return If succeed, the return result is S_OK, otherwise, check `remarks`
 * section.
 * @remarks If `tVirt` or `tPhys` is nullptr, the return result is E_ARG_NULL.
 */
kresult_t PagingVirtToPhys(__vma_t tVirt, __pma_t *tPhys);

/**
 * Get the physical address stored in CR3.
 *
 * @param[out] tPhys	When function exits, contain the physical address stored
 * in CR3.
 * @return If succeed, the return result is S_OK. If `tPhys` is nullptr, return
 * result is E_ARG_NULL.
 */
kresult_t PagingGetCurrentDir(__pma_t *tPhys);

/**
 * Maps a page to current address space.
 *
 * @param tVirt The virtual address to map.
 * @param tPhys The physical address to map.
 * @param tTblFlags The flags to add on the table entity.
 * @param tPgFlags The flags to apply on the page entity.
 * @param tCanOverride Indicate whether an existing entry may be overriten.
 * @return If function succeed, return result is S_OK, otherwise, check
 * `remarks` section.
 * @remarks If `tVirt` or `tPhys` is zero, return result is E_ARG_NULL. If
 * system cannot allocate new page for unmapped entry, return result is
 * E_MEMMAN_OUTOFMEM. If `tVirt` or `tPhys` is not aligned, the return result is
 * E_MEMMAN_ADDR_UNALIGNED.
 */
kresult_t PagingMapPage(__vma_t tVirt,
						__pma_t tPhys,
						dword_t tTblFlags = DEFAULT_TABLE_FLAGS,
						dword_t tPgFlags = DEFAULT_PAGE_FLAGS,
						bool	tCanOverride = false);

/**
 * Used to map page to directory that's not current address space.
 * The directory itself and the required tables are temporarily mapped and then
 * unmapped.
 *
 * @param tDir Physical address of the temporary directory.
 * @param tVirt The virtual address to map.
 * @param tPhys The physical address to map.
 * @param tTblFlags The flags to add on the table entity.
 * @param tPgFlags The flags to apply on the page entity.
 * @param tCanOverride Indicate whether an existing entry may be overriten.
 * @return If function succeed, the return result is S_OK, otherwise, check
 * `remarks` section.
 * @remarks If `tVirt` or `tPhys` is zero, return result is E_ARG_NULL. If
 * system cannot allocate new page for unmapped entry, return result is
 * E_MEMMAN_OUTOFMEM. If `tVirt` or `tPhys` is not aligned, the return result is
 * E_MEMMAN_ADDR_UNALIGNED.
 */
// kresult_t PagingMapPageToTemp(__pma_t tDir,
// 							  __vma_t tVirt,
// 							  __pma_t tPhys,
// 							  dword_t tTblFlags = DEFAULT_TABLE_FLAGS,
// 							  dword_t tPgFlags = DEFAULT_PAGE_FLAGS,
// 							  bool	  tCanOverride = false);

/**
 * Unmaps virtual address from the current address space.
 *
 * @param tVirt The virtual address to unmap.
 * @return If function succeed, the return result is S_OK. If `tVirt` is zero,
 * the return result is E_ARG_NULL. If `tVirt` is not mapped to any physical
 * address, the return result is E_MEMMAN_VIRTNOTMAPPED.
 */
kresult_t PagingUnmapPage(__vma_t tVirt);

// /**
//  * Add new physical pages to pool, so when new page needed, can be retrieve
//  * from the pool, instead of allocate from physical mem manage.
//  *
//  * @param tPhys The physical page to add to pool.
//  */
// kresult_t PagingAddFreePage(__pma_t tPhys, int tCount);
//
// /**
//  * Clear the free pages pool.
//  */
// kresult_t PagingClearFreePages();

void PagingInitialize();

void InvalidatePage(void *addr);

}}	  // namespace qkrnl::mm