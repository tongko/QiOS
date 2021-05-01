#include <cdefs.h>
#include <kernel/KTextMode.hpp>
#include <kernel/Logger.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/mm/AddressPool.hpp>
#include <kernel/mm/BmpPageAllocator.hpp>
#include <kernel/mm/KHeap.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/mm/PageAllocator.hpp>
#include <kernel/mm/Paging.hpp>
#include <kernel/types.hpp>
#include <new>

//	Global variables
extern qkrnl::BootParams *g_pBootParams;
extern __vma_t			  g_LastUsed;
extern qkrnl::mm::KHeap * g_pHeap;
extern qkrnl::Logger *	  g_pLogger;

namespace qkrnl { namespace mm {

//	Safe guard for re-initialize.
static bool s_bInitialized {false};
//	Page Frame Allocator
static PageAllocator *s_pPageAlloc;
//	Virtual address pool
static AddressPool *s_pPool;

static inline dword_t ConvertPageCount(int_t	 tnPageCount,
									   PageSizes tePageSize) {

	dword_t nPgSize = GetPageSize(tePageSize);
	//	We only have a 4K address pool, so convert to 4K page count
	return (tnPageCount * nPgSize) / GetPageSize(PageSizes::FourK);
}

static void SetupKernelHeap(byte_t *tpHeapBuff) {

	size_t	nHeapSize = 0x2000000;	  // 32 MiB Heap, extendable
	__vma_t heapStart = g_KERNEL_VIRT_START;
	size_t	size = nHeapSize;

	//	LastUsed will be used to map the g_KERNEL_VIRT_START
	__pma_t addr = MemoryManager::PhysAlloc(
		nHeapSize / 0x1000,
		PageSizes::FourK);	  // g_LastUsed - g_pBootParams->VirtualOffset;
	//	Map to 32 MiB to Virtual Address
	for (__vma_t v = heapStart; v < (heapStart + nHeapSize) - 1;
		 v += 0x1000, addr += 0x1000) {
		PagingMapPage(v, addr);
	}

	// g_LastUsed += nHeapSize;

	byte_t *buff = tpHeapBuff;
	g_pHeap
		= new (buff) KHeap(PageSizes::FourK,	//	Page size is 0x1000
						   16,	  // Minimum allocation for new page is 16 pages
						   heapStart,	 // Base address
						   size);		 // Heap size

	// From this point onwards, we can start using the opreator
	// `new` to allocate new memory, just remember to delete it
	// after use.
}

static void SetupVirtualMemoryManager() {}

/**
 * MMHasInitialized()
 */
bool MMHasInitialized() {
	return s_bInitialized;
}

/**
 * Initialize
 */
kresult_t MemoryManager::Initialize() {

	if (s_bInitialized) {
		return S_OK;
	}

	Logger *pLog = g_pLogger;
	pLog->Info("[MemoryManager::Initialize] Initializing MemoryManager...\n");
	//	Validation
	BootParams *pParams = g_pBootParams;
	if (pParams->KernelStart == 0 || pParams->KernelEnd == 0
		|| pParams->VirtualOffset == 0) {
		return E_ARG_NULL;
	}

	if (pParams->KernelStart & 0xFFF || pParams->KernelEnd & 0xFFF) {
		return E_MEMMAN_ADDR_UNALIGNED;
	}

	//	Reserved some space for KHeap
	byte_t *pHeapBuff = (byte_t *) g_LastUsed;
	g_LastUsed += sizeof(KHeap);

	//	Setup a 4K kernel virtual addresss range
	s_pPool = new AddressPool(PageSizes::FourK);
	s_pPool->AddRange(g_KERNEL_VIRT_START,
					  g_KERNEL_VIRT_START + g_KERNEL_VIRT_LENGTH - 1);

	//	Setup Page table
	__vma_t newPageBuffer = ALIGN(g_LastUsed, 0x1000);
	PagingInitialize(newPageBuffer);
	//	Set the new VIDEO MEM.
	TextModeSetBuffer(0xFFFFFFFFFFFFD000);

	//	Setup Page Frame Allocator
	s_pPageAlloc = new PageAllocator(newPageBuffer);

	g_LastUsed = newPageBuffer;

	//	Kernel heap
	SetupKernelHeap(pHeapBuff);
	//	Virtual memory
	SetupVirtualMemoryManager();

	s_bInitialized = true;
	return S_OK;
}

/**
 * VirtualAlloc()
 */
void *MemoryManager::VirtualAlloc(int_t		tnPageCount,
								  PageSizes tePageSize,
								  bool		tbNoMap) {

	int_t nPgCount = ConvertPageCount(tnPageCount, tePageSize);

	__vma_t	  addr;
	kresult_t kr = s_pPool->Allocate(nPgCount, ARF_ALLOCATED, addr);
	if (FAILED(kr)) {
		KernelPanic("[MemoryManager::VirtualAlloc] Out of memory.");
	}

	if (!tbNoMap) {
		__pma_t pAddr = s_pPageAlloc->Alloc(nPgCount);
		PagingMapPage(addr, pAddr);
	}

	return (void *) addr;
}

/**
 * VirtualFree()
 */
void MemoryManager::VirtualFree(void *	  tpAddr,
								int_t	  tnPageCount,
								PageSizes tePageSize) {

	if (!(tpAddr && tnPageCount)) {
		return;
	}

	int_t nPgCount = ConvertPageCount(tnPageCount, tePageSize);

	__pma_t pAddr;
	PagingVirtToPhys((__vma_t) tpAddr, &pAddr);
	s_pPool->Free((__vma_t) tpAddr, nPgCount);
	s_pPageAlloc->Free(pAddr, nPgCount);
}

__pma_t MemoryManager::PhysAlloc(int_t tnPageCount, PageSizes tePageSize) {

	if (!tnPageCount) {
		return 0;
	}

	int_t nPgCount = ConvertPageCount(tnPageCount, tePageSize);

	return s_pPageAlloc->Alloc(nPgCount);
}

void MemoryManager::PhysFree(__pma_t   tpAddr,
							 int_t	   tnPageCount,
							 PageSizes tePageSize) {

	if (!(tpAddr && tnPageCount)) {
		return;
	}

	int nPgCount = ConvertPageCount(tnPageCount, tePageSize);

	s_pPageAlloc->Free(tpAddr, nPgCount);
}

}}	  // namespace qkrnl::mm