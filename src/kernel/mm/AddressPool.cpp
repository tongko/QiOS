#include <kernel/Error.hpp>
#include <kernel/Logger.hpp>
#include <kernel/mm/AddressPool.hpp>
#include <kernel/mm/KHeap.hpp>

#define LOCK   m_pMutex->Acquire()
#define UNLOCK m_pMutex->Release()

//	Global variable
extern qkrnl::mm::KHeap *g_pHeap;
extern qkrnl::Logger *	 g_pLogger;

namespace qkrnl { namespace mm {

/**
 * MakeRange()
 */
static AddressRange *MakeRange(__vma_t		tBase,
							   size_t		tLen,
							   byte_t		tFlags,
							   PageSizes	tePgSize,
							   AddressPool *owner) {

	AddressRange *pAr = new AddressRange();
	pAr->Base = tBase;
	pAr->Next = nullptr;
	pAr->Pages = tLen / GetPageSize(tePgSize);
	pAr->Used = false;
	pAr->Flags = tFlags;
	pAr->Owner = owner;

	return pAr;
}

size_t AddressRange::Size() {
	return Pages * GetPageSize(Owner->GetPageSize());
}

__vma_t AddressRange::Tail() {
	return Base + Size() - 1;
}

/**
 * Constructors
 */
AddressPool::AddressPool(PageSizes tePgSize)
	: m_ePgSize(tePgSize)
	, m_pFirst(0) {

	m_pMutex = new Mutex();
}

/**
 * GetPageSize()
 */
PageSizes AddressPool::GetPageSize() {

	return m_ePgSize;
}

/**
 * AddRange()
 */
kresult_t AddressPool::AddRange(uintptr_t tStart, uintptr_t tEnd) {

	if (tStart % qkrnl::mm::GetPageSize(m_ePgSize)) {
		return E_MEMMAN_ADDR_UNALIGNED;
	}

	size_t len = tEnd - tStart + 1;
	if (len % qkrnl::mm::GetPageSize(m_ePgSize)) {
		return E_MEMMAN_ADDR_UNALIGNED;
	}

	LOCK;
	if (m_pFirst == nullptr) {
		m_pFirst = MakeRange(tStart, len, 0, m_ePgSize, this);
	}
	else {
		AddressRange *p = m_pFirst;
		while (p->Next != nullptr) {
			p = p->Next;
		}

		p->Next = MakeRange(tStart, len, 0, m_ePgSize, this);
	}

	UNLOCK;

	return S_OK;
}

/**
 * CloneRanges()
 */
kresult_t AddressPool::CloneRanges(AddressPool &tOther) {

	LOCK;

	if (m_pFirst) {
		ReleaseRanges();
	}

	AddressRange *otherCurrent = tOther.m_pFirst;
	AddressRange *last = nullptr;
	while (otherCurrent) {
		//	Copy
		AddressRange *newRange = new AddressRange(*otherCurrent);
		newRange->Next = nullptr;

		if (last) {
			last->Next = newRange;
		}
		else {
			m_pFirst = newRange;
		}
		last = newRange;
		otherCurrent = otherCurrent->Next;
	}

	UNLOCK;

	return S_OK;
}

/**
 * ReleaseRanges()
 */
void AddressPool::ReleaseRanges() {

	LOCK;

	AddressRange *range = m_pFirst;
	while (range) {
		AddressRange *next = range->Next;
		g_pHeap->Free(range);
		range = next;
	}

	m_pFirst = nullptr;

	UNLOCK;
}

/**
 * Merge()
 */
void AddressPool::Merge() {

	LOCK;

	AddressRange *pCurr = m_pFirst;
	while (pCurr && pCurr->Next) {
		if (!pCurr->Used && !pCurr->Next->Used
			&& pCurr->Tail() == pCurr->Next->Base) {
			pCurr->Pages += pCurr->Next->Pages;

			AddressRange *pNextNext = pCurr->Next->Next;
			delete pCurr->Next;
			pCurr->Next = pNextNext;
		}
		else {
			pCurr = pCurr->Next;
		}
	}

	UNLOCK;
}

/**
 * Allocate()
 */
kresult_t AddressPool::Allocate(dword_t	   tPages,
								byte_t	   tFlags,
								uintptr_t &tBase) {

	LOCK;

	if (tPages == 0) {
		tPages = 1;
	}

	AddressRange *pRange = m_pFirst;
	while (pRange) {
		if (!pRange->Used && pRange->Pages >= tPages) {
			break;
		}

		pRange = pRange->Next;
	}

	if (pRange) {
		pRange->Used = true;
		pRange->Flags = tFlags;

		int32_t nRemain = pRange->Pages - tPages;
		if (nRemain > 0) {
			AddressRange *splitter = new AddressRange();
			splitter->Used = false;
			splitter->Pages = nRemain;
			splitter->Base = pRange->Tail() + 1;
			splitter->Flags = 0;
			splitter->Owner = this;
			splitter->Next = pRange->Next;

			pRange->Next = splitter;
			pRange->Pages = tPages;
		}
	}

	tBase = pRange->Base;

	UNLOCK;

	return S_OK;
}

/**
 * Free()
 */
kresult_t AddressPool::Free(uintptr_t tBase, int_t &tpPagesFreed) {

	LOCK;

	tpPagesFreed = 0;

	AddressRange *pRange = m_pFirst;
	while (pRange) {
		if (pRange->Base == tBase) {
			break;
		}

		pRange = pRange->Next;
	}

	if (!pRange) {
		UNLOCK;
		return E_MEMMAN_VIRTNOTEXISTS;
	}

	if (!pRange->Used) {
		UNLOCK;
		return E_MEMMAN_VIRTNOTINUSED;
	}

	pRange->Used = false;
	tpPagesFreed = pRange->Pages;
	Merge();

	UNLOCK;

	return S_OK;
}

/**
 * GetRanges()
 */
void AddressPool::GetRanges(AddressRange &tpRanges) {

	if (m_pFirst == nullptr) {
		return;
	}

	tpRanges.Base = m_pFirst->Base;
	tpRanges.Next = m_pFirst->Next;
	tpRanges.Pages = m_pFirst->Pages;
	tpRanges.Flags = m_pFirst->Flags;
	tpRanges.Used = m_pFirst->Used;
	tpRanges.Owner = this;
}

/**
 * Find()
 */
kresult_t AddressPool::Find(uintptr_t tBase) {

	LOCK;

	AddressRange *pRange = m_pFirst;
	while (pRange) {
		if (pRange->Base == tBase) {
			break;
		}

		pRange = pRange->Next;
	}

	UNLOCK;

	return S_OK;
}

/**
 * Dump()
 */
kresult_t AddressPool::Dump(bool tOnlyFree) {

	LOCK;

	Logger *pLogger = g_pLogger;
	pLogger->Debug("[DBG] Address Range structure:\n");
	if (m_pFirst == nullptr) {
		pLogger->Debug("[DBG] Dump failed, no first entry.\n");
		return E_MEMMAN_AR_EMPTY;
	}

	AddressRange *pCurr = m_pFirst;
	while (pCurr) {
		if (!tOnlyFree || !pCurr->Used) {
			size_t size = pCurr->Size();
			pLogger->Debug(
				"[DBG] Used: %c, Base: 0x%08x%08x, Pages: %i (- 0x%08x%08x)\n",
				pCurr->Used ? 't' : 'f',
				pCurr->Base >> 32,
				pCurr->Base & 0xFFFFFFFF,
				pCurr->Pages,
				size >> 32,
				size & 0xFFFFFFFF);
		}

		pCurr = pCurr->Next;
	}

	UNLOCK;

	return S_OK;
}

}}	  // namespace qkrnl::mm