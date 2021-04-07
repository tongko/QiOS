#include <cdefs.h>
#include <kernel/bootinfo.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/mm/Paging.hpp>
#include <kernel/mm/StackPageAllocator.hpp>
#include <kernel/mm/mm_common.hpp>
#include <kernel/types.hpp>
#include <new>
#include <string.hpp>

extern qkrnl::BootParams *g_pBootParams;
extern __vma_t			  g_LastUsed;
extern const __vma_t	  g_KERNEL_VIRT_START;

namespace qkrnl { namespace mm {

#define ADDR_MASK 0xFFFFFFFFFFFFF000UL

const dword_t PAGING_SIZE = 0x1000;
/**
 * QiOS Paging
 * P4 Entry [511] is map to 0x0000000000000000
 * P4 Entry [510] is Recursive entry
 * P4 Entry [509] is Kernel Heap and others.
 */
const __vma_t P1_RECURSIVE_ENTRY = 0xFFFFFF0000000000UL;
const __vma_t P2_RECURSIVE_ENTRY = 0xFFFFFF7F80000000UL;
const __vma_t P3_RECURSIVE_ENTRY = 0xFFFFFF7FBFC00000UL;
const __vma_t P4_RECURSIVE_ENTRY = 0xFFFFFF7FBFDFF000UL;

#define GET_PAGE_TABLE(tVirt)                                  \
	(P1_RECURSIVE_ENTRY | (((size_t) GET_P4_IDX(tVirt)) << 30) \
	 | (((size_t) GET_P3_IDX(tVirt)) << 21)                    \
	 | (((size_t) GET_P2_IDX(tVirt)) << 12))

static __pma_t GetNewPage() {

	__pma_t newPage;
	if (!MMHasInitialized()) {
		g_LastUsed = ALIGN(g_LastUsed, PAGING_SIZE);
		newPage = g_LastUsed - g_pBootParams->VirtualOffset;
		g_LastUsed += PAGING_SIZE;
	}
	else {
		newPage = MemoryManager::PhysAlloc(1, PageSizes::FourK);
	}

	if (newPage) {
		memset((void *) newPage, 0, PAGING_SIZE);
	}

	return newPage;
}

void InvalidatePage(void *addr) {

	_ASMV_("invlpg [%0]" : : "b"(addr) : "memory");
}

kresult_t PagingGetCurrentDir(__pma_t *tPhys) {

	if (!tPhys) {
		return E_ARG_NULL;
	}

	qword_t directory;
	_ASMV_("mov %0, CR3" : "=r"(directory));
	*tPhys = directory;

	return S_OK;
}

kresult_t PagingVirtToPhys(__vma_t tVirt, __pma_t *tPhys) {

	if (!(tVirt || tPhys)) {
		return E_ARG_NULL;
	}

	//	Get the address of Page Table via recursive entry
	__vma_t p1Addr = GET_PAGE_TABLE(tVirt);

	if (!p1Addr) {
		return E_MEMMAN_VIRTNOTMAPPED;
	}

	//	Cast the Page Table entry to page_tbl_t type.
	page_tbl_t ptr = reinterpret_cast<page_tbl_t>(p1Addr);

	//	Get the physical address in the entry
	*tPhys = static_cast<__pma_t>((ptr[GET_P1_IDX(tVirt)] & ADDR_MASK)
								  + (tVirt & ALIGN_MASK));
	return S_OK;
}

kresult_t PagingUnmapPage(__vma_t tVirt) {

	if (!tVirt) {
		return E_ARG_NULL;
	}

	//	Get the address of Page Table via recursive entry
	__vma_t p1Addr = GET_PAGE_TABLE(tVirt);
	if (!p1Addr) {
		return E_MEMMAN_VIRTNOTMAPPED;
	}

	//	Cast the Page Table entry to page_tbl_t type.
	page_tbl_t ptr = reinterpret_cast<page_tbl_t>(p1Addr);

	//	Point that page to null and clear attribute (!(PRESENT + etc))
	ptr[GET_P1_IDX(tVirt)] = 0;
	//	Invalidate the page in TLB
	InvalidatePage((void *) (tVirt & ADDR_MASK));

	return S_OK;
}

kresult_t PagingMapPage(__vma_t tVirt,
						__pma_t tPhys,
						dword_t tTblFlags,
						dword_t tPgFlags,
						bool	tCanOverride) {

	if (!(tVirt || tPhys)) {
		return E_ARG_NULL;
	}

	if ((tVirt & ALIGN_MASK) || (tPhys & ALIGN_MASK)) {
		return E_MEMMAN_ADDR_UNALIGNED;
	}

	__vma_t recursives[3]
		= {P4_RECURSIVE_ENTRY, P3_RECURSIVE_ENTRY, P2_RECURSIVE_ENTRY};
	int idxs[3] = {GET_P4_IDX(tVirt), GET_P3_IDX(tVirt), GET_P2_IDX(tVirt)};
	page_dir_t ptr;
	kresult_t  kr;
	__pma_t	   newPage;
	int		   idx;
	for (int i = 0; i < 3; i++) {
		ptr = reinterpret_cast<page_dir_t>(recursives[i]);
		idx = idxs[i];
		if (!(ptr[idx])) {
			//	Entry not set, get a new page and set to this entry
			newPage = GetNewPage();
			if (newPage == 0) {
				KernelPanic("[Paging] Out of memory. Cannot allocate new "
							"physical memory for paging.");
			}

			ptr[idx] = newPage | tTblFlags;
		}
	}

	//	Cast the page table entry to page_tbl_t type.
	ptr = reinterpret_cast<page_tbl_t>(P1_RECURSIVE_ENTRY);
	int p1Idx = GET_P1_IDX(tVirt);
	if (!(ptr[p1Idx]) || tCanOverride) {
		ptr[p1Idx] = tPhys | tPgFlags;
	}
	else {
		return E_MEMMAN_ENTRY_EXISTS;
	}

	InvalidatePage((void *) (tVirt & ADDR_MASK));

	return S_OK;
}

void PagingInitialize() {

	// We want to properly setup the paging, again, despite the
	// loader already setup one before calling into C++ code. We will
	// mapped 0 to kernel end + the space needed to keep these page
	// entries. This allow us to kick start in setting up memory management.
	// For now, we still cannot utilized Paging module as Paging module rely on
	// proper memory allocation, which still not available at this point.
	__vma_t pageAddr = ALIGN(g_LastUsed, 0x1000);
	size_t	virOffset = g_pBootParams->VirtualOffset & ADDR_MASK;
	__vma_t krnlStart = g_pBootParams->KernelStart & ADDR_MASK;
	__vma_t krnlEnd = pageAddr - virOffset;

	page_dir_t p4Dir = (page_dir_t) g_pBootParams->PageDirectory;
	page_dir_t p3Dir = p4Dir + GET_P4_IDX(krnlStart);	 // p4Idx should be 511
	if (*p3Dir == 0) {									 //	Not mapped
		*p3Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}
	page_dir_t p2Dir = (page_dir_t)(*p3Dir & ADDR_MASK) + GET_P3_IDX(krnlStart);
	if (*p2Dir == 0) {
		*p2Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}

	//	Mapping from kernel image start to end.
	page_dir_t p1Dir;
	page_tbl_t phys;
	for (__vma_t v = krnlStart; v < (pageAddr - 1); v += 0x1000) {
		p1Dir = (page_dir_t)(*p2Dir & ADDR_MASK) + GET_P2_IDX(v);
		if (*p1Dir == 0) {
			*p1Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
			memsetdw((void *) krnlEnd, 0, 1024);
			krnlEnd += 0x1000;
		}

		phys = (page_tbl_t)(*p1Dir & ADDR_MASK) + GET_P1_IDX(v);
		*phys = (v - virOffset) | DEFAULT_PAGE_FLAGS;
	}

	//	We need to help KHeap to allocate 32 MiB, because if heap not
	//	prepared, we can't run MapPage properly.
	__vma_t v = g_KERNEL_VIRT_START;
	p3Dir = p4Dir + GET_P4_IDX(v);	  // P4 idx should be 509
	if (*p3Dir == 0) {
		*p3Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}
	p2Dir = ((page_dir_t)(*p3Dir & ADDR_MASK)) + GET_P3_IDX(v);
	if (*p2Dir == 0) {
		*p2Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}
	for (; v < g_KERNEL_VIRT_START + 0x2000000; v += 0x1000) {
		p1Dir = ((page_dir_t)(*p2Dir & ADDR_MASK)) + GET_P2_IDX(v);
		if (*p1Dir == 0) {
			*p1Dir = krnlEnd | DEFAULT_TABLE_FLAGS;
			memsetdw((void *) krnlEnd, 0, 1024);
			krnlEnd += 0x1000;
		}

		phys = ((page_tbl_t)(*p1Dir & ADDR_MASK)) + GET_P1_IDX(v);
		*phys = 0;	  // No need to map to physical for now, only do it until it
					  // is needed
	}

	g_LastUsed = krnlEnd + virOffset;

	//	Flush TLB
	_ASMV_("mov rax, CR3\n"
		   "mov CR3, rax");
}

}}	  // namespace qkrnl::mm