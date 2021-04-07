#include <kernel/Logger.hpp>
#include <kernel/mm/AddressPool.hpp>
#include <kernel/mm/BmpPageAllocator.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/mm/Paging.hpp>
#include <kernel/mm/StackPageAllocator.hpp>
#include <kernel/mm/mm_common.hpp>
#include <mutex.hpp>
#include <string.hpp>

#define LOCK   m_pMutex->Acquire()
#define UNLOCK m_pMutex->Release()

extern qkrnl::Logger *g_pLogger;

typedef qkrnl::mm::BmpPageAllocator bitmap;

namespace qkrnl { namespace mm {

AddressRange *StackPageAllocator::GetPageFromBitmap(AddressRange *tpCurrent,
													int			  tnPgCount) {

	__pma_t addr = 0;

	//	Pre-allocate enough pages from bitmap
	kresult_t kr = bitmap::Alloc(tnPgCount, &addr);
	if (FAILED(kr)) {
		KernelPanic("[StackPageAllocator::Ctor] Out of memory.");
	}

	//	Write the stack info into the page.
	// PagingMapPage(s_PageFrameStackPointer, addr);
	__vma_t		  vaddr = TemporaryMapToPage(addr);
	AddressRange *p = (AddressRange *) vaddr;
	p->Base = addr;
	p->Next = tpCurrent;
	p->Pages = tnPgCount;

	return p;
}

dword_t StackPageAllocator::GetTotalPages() {

	dword_t		  nTotal = 0;
	AddressRange *p = m_NextFreePage;
	while (p) {
		nTotal += p->Pages;
		p = p->Next;
	}

	return nTotal;
}

__vma_t StackPageAllocator::TemporaryMapToPage(__pma_t tAddr) {

	__vma_t temp = reinterpret_cast<__vma_t>(
		MemoryManager::VirtualAlloc(1, PageSizes::FourK, true));
	if (temp == 0) {
		KernelPanic("[StackPageAllocator::TemporaryMapToPage] Out of memory.");
	}
	PagingMapPage(temp, tAddr);

	return temp;
}

StackPageAllocator::StackPageAllocator(size_t tInitSizeInPages)
	: m_NextFreePage(0) {

	m_pMutex = new Mutex();

	LOCK;

	if (tInitSizeInPages < 16) {
		tInitSizeInPages = 16;
	}

	m_NextFreePage = GetPageFromBitmap(nullptr, tInitSizeInPages);

	UNLOCK;
}

__pma_t StackPageAllocator::Alloc() {

	LOCK;

	if (m_NextFreePage == nullptr) {
		m_NextFreePage = GetPageFromBitmap(nullptr, 16);
	}

	__pma_t		  physAddr = m_NextFreePage->Base;
	size_t		  len = m_NextFreePage->Pages;
	AddressRange *pNext = m_NextFreePage->Next;

	// Clean the page so content from other process doesn't retain
	memset((void *) m_NextFreePage, 0, 0x1000);
	// Release virtual address map to m_NextFreePage
	MemoryManager::VirtualFree(reinterpret_cast<void *>(m_NextFreePage),
							   1,
							   PageSizes::FourK);

	if (len > 1) {
		//	Set m_NextFreePage to next page
		m_NextFreePage = (AddressRange *) TemporaryMapToPage(physAddr + 0x1000);
		m_NextFreePage->Base = physAddr + 0x1000;
		m_NextFreePage->Pages = len - 1;
		m_NextFreePage->Next = pNext;
	}
	else {
		m_NextFreePage = pNext;
	}

	UNLOCK;

	//	Return physical page.
	return physAddr;
}

void StackPageAllocator::Free(__pma_t tAddr) {

	if (tAddr & 0xFFF) {
		//	Page not align
		return;
	}

	LOCK;

	AddressRange *pPage = (AddressRange *) TemporaryMapToPage(tAddr);
	pPage->Base = tAddr;
	pPage->Pages = 1;
	pPage->Next = m_NextFreePage;

	m_NextFreePage = pPage;

	UNLOCK;
}

}}	  // namespace qkrnl::mm