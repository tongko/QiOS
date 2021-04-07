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
static const __vma_t P1_RECURSIVE_ENTRY = 0xFFFFFF0000000000UL;
static const __vma_t P2_RECURSIVE_ENTRY = 0xFFFFFF7F80000000UL;
static const __vma_t P3_RECURSIVE_ENTRY = 0xFFFFFF7FBFC00000UL;
static const __vma_t P4_RECURSIVE_ENTRY = 0xFFFFFF7FBFDFF000UL;
/**
 * The following virtual address is being used to temporary mapped
 * a physical page so paging could zero init the page.
 */
static const __vma_t PAGING_TEMP_VIRT = 0xFFFFFFFFFFFFF000UL;

static __vma_t s_Recursives[3]
	= {P4_RECURSIVE_ENTRY, P3_RECURSIVE_ENTRY, P2_RECURSIVE_ENTRY};

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
		// Temporary map the newPage to PAGING_TEMP_VIRT
		page_tbl_t p1Addr
			= reinterpret_cast<page_tbl_t>(GET_PAGE_TABLE(PAGING_TEMP_VIRT));
		p1Addr[0] = newPage | DEFAULT_TABLE_FLAGS;
		memset((void *) PAGING_TEMP_VIRT, 0, PAGING_SIZE);
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

/**
 * PagingMapPage()
 */
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

	// To map a virtual address to physical address, we must first examine the
	// existant of each page directory level. In AMD64 architecture, this is
	// namely PML4, PDPE, PDE, and PTE respectively. Although AMD64 paging
	// support large page size, QiOS only support 4K page size at this moment.
	int idxs[3] = {GET_P4_IDX(tVirt), GET_P3_IDX(tVirt), GET_P2_IDX(tVirt)};
	page_dir_t ptr;
	kresult_t  kr;
	__pma_t	   newPage;
	int		   idx;
	for (int i = 0; i < 3; i++) {
		ptr = reinterpret_cast<page_dir_t>(s_Recursives[i]);
		idx = idxs[i];
		if (ptr[idx] == 0) {
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
	if (ptr[p1Idx] == 0 || tCanOverride) {
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

	// Copy current PML4
	page_dir_t p4Dir = (page_dir_t) krnlEnd;
	krnlEnd += 0x1000;
	memcpy((void *) p4Dir, (void *) g_pBootParams->PageDirectory, 0x1000);
	// p4Idx should be 511, krnlEnd is the new p3Dir
	p4Dir[GET_P4_IDX(krnlStart)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;
	page_dir_t p3Dir = (page_dir_t)(p4Dir[GET_P4_IDX(krnlStart)] & ADDR_MASK);
	//	p3Idx should be 510, krnlEnd is the new p2Dir
	p3Dir[GET_P3_IDX(krnlStart)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;
	page_dir_t p2Dir = (page_dir_t)(p3Dir[GET_P3_IDX(krnlStart)] & ADDR_MASK);
	//	p2Idx should be 0, krnlEnd is the new p1Tbl
	p2Dir[GET_P2_IDX(krnlStart)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;
	//	Mapping from kernel image start to end.
	page_dir_t p1Tbl;
	page_tbl_t phys;
	for (__vma_t v = krnlStart; v < (pageAddr - 1); v += 0x1000) {
		if (p2Dir[GET_P2_IDX(v)] == 0) {
			p2Dir[GET_P2_IDX(v)] = krnlEnd | DEFAULT_TABLE_FLAGS;
			memsetdw((void *) krnlEnd, 0, 1024);
			krnlEnd += 0x1000;
		}

		p1Tbl = (page_dir_t)(p2Dir[GET_P2_IDX(v)] & ADDR_MASK);
		p1Tbl[GET_P1_IDX(v)] = (v - virOffset) | DEFAULT_PAGE_FLAGS;
	}

	//	The special virtual address for temporary used.
	if (p3Dir[511] == 0) {
		p3Dir[511] = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}
	p2Dir = (page_dir_t)(p3Dir[511] & ADDR_MASK);
	if (p2Dir[511] == 0) {
		p2Dir[511] = krnlEnd | DEFAULT_TABLE_FLAGS;
		memsetdw((void *) krnlEnd, 0, 1024);
		krnlEnd += 0x1000;
	}

	//	We need to help KHeap to allocate 32 MiB, because if heap not
	//	prepared, we can't run MapPage properly.
	__vma_t v = g_KERNEL_VIRT_START;
	// P4 idx should be 509
	p4Dir[GET_P4_IDX(v)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;
	p3Dir = (page_dir_t)(p4Dir[GET_P4_IDX(v)] & ADDR_MASK);
	p3Dir[GET_P3_IDX(v)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;
	p2Dir = (page_dir_t)(p3Dir[GET_P3_IDX(v)] & ADDR_MASK);
	p2Dir[GET_P2_IDX(v)] = krnlEnd | DEFAULT_TABLE_FLAGS;
	memsetdw((void *) krnlEnd, 0, 1024);
	krnlEnd += 0x1000;

	for (; v < g_KERNEL_VIRT_START + 0x2000000; v += 0x1000) {
		if (p2Dir[GET_P2_IDX(v)] == 0) {
			p2Dir[GET_P2_IDX(v)] = krnlEnd | DEFAULT_TABLE_FLAGS;
			memsetdw((void *) krnlEnd, 0, 1024);
			krnlEnd += 0x1000;
		}
		// No need to map to physical for now, only do it until it is needed
	}

	g_LastUsed = krnlEnd + virOffset;

	//	We also want to disable the 1 GiB Identity map made in loader.
	p4Dir[0] = 0;
	//	Copy back to current PML4
	memcpy((void *) g_pBootParams->PageDirectory, (void *) p4Dir, 0x1000);

	//	Flush TLB
	_ASMV_("mov rax, CR3\n"
		   "mov CR3, rax");
}

}}	  // namespace qkrnl::mm