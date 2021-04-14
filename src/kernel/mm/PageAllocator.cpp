#include <kernel/Logger.hpp>
#include <kernel/mm/BmpPageAllocator.hpp>
#include <kernel/mm/PageAllocator.hpp>
#include <kernel/mm/StackPageAllocator.hpp>
#include <kernel/mm/mm_common.hpp>
#include <mutex.hpp>

#define LOCK   m_pMutex->Acquire()
#define UNLOCK m_pMutex->Release()

extern qkrnl::Logger *g_pLogger;

namespace qkrnl { namespace mm {

PageAllocator::PageAllocator(__vma_t &bitmapBuffer) {

	g_pLogger->Print("[PageAllocator::Ctor] Initializing PageAllocator...\n");
	//	Initialize Mutex
	m_pMutex = new qklib::Mutex();

	//	Initialized bitmap page allocator
	BmpPageAllocator::Initialize(bitmapBuffer);
	//	Initialized stack page allocator of 32 MiB
	g_pLogger->Print("[PageAllocator] Initializing stack page allocator...\n");
	m_pStack = new StackPageAllocator(0x2000);
}

size_t PageAllocator::GetTotalBytes() {

	return BmpPageAllocator::GetTotalBlocks() * BLOCK_SIZE;
}

size_t PageAllocator::GetUsedBytes() {

	LOCK;

	return (BmpPageAllocator::GetUsedBlocks() - m_pStack->GetTotalPages())
		   * BLOCK_SIZE;

	UNLOCK;
}

__pma_t PageAllocator::Alloc(dword_t tnPageCount) {

	LOCK;

	__pma_t addr;
	if (tnPageCount == 1) {
		addr = m_pStack->Alloc();
	}
	else {
		kresult_t kr = BmpPageAllocator::Alloc(tnPageCount, &addr);
		if (FAILED(kr)) {
			addr = 0;
		}
	}

	UNLOCK;

	return addr;
}

void PageAllocator::Free(__pma_t tAddr, dword_t tnPageCount) {

	LOCK;

	if (tnPageCount == 1) {
		m_pStack->Free(tAddr);
	}
	else {
		BmpPageAllocator::Free(tAddr, tnPageCount);
	}

	UNLOCK;
}

}}	  // namespace qkrnl::mm