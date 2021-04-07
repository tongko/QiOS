#pragma once

#include <kernel/types.hpp>

using namespace qklib;

//	prototypes
namespace qklib {
class Mutex;
}

namespace qkrnl { namespace mm {

//	prototypes
enum class PageSizes;
struct HeapChunk;

struct HeapBlock {
	HeapBlock *Prev;
	HeapBlock *Next;
	size_t	   Size;
	size_t	   Used;
	HeapChunk *FirstChunk;
	__vma_t	   VirtualAddress;
};

struct HeapChunk {
	HeapChunk *Prev;
	HeapChunk *Next;
	HeapBlock *Owner;
	dword_t	   Magic;
	size_t	   Size;
	size_t	   Requested;
};

class KHeap {

public:	   //	Constructors
	/**
	 * Initialize an instance of KHeap with specified page size, minimum
	 * allocation page count, and virtual address space fot the heap.
	 *
	 * @param tBlockSize Page size of heap.
	 * @param tMinAlloc Minimum allocation page count.
	 * @param tBaseAddr Optional. Specified the heap's base address. If
	 * `tBaseLength` is 0, this parameter is ignore.
	 * @param tBaseLength Optional. Specified the heap's size, in bytes. If
	 * `tBaseAddr` is nullptr, this parameter is ignore.
	 */
	KHeap(PageSizes tBlockSize,
		  int_t		tMinAlloc,
		  __vma_t	tBaseAddr = 0,
		  size_t	tBaseLength = 0);
	/**
	 * Destructor
	 */
	~KHeap();

public:
	//	Operations

	/**
	 * Allocate memory the size of `tSize` long.
	 *
	 * @param tSize The size of the memory requesting.
	 * @return Pointer of the allocated memory.
	 */
	void *Alloc(size_t);
	/**
	 * De-allocate memory allocated via `Alloc(size_t)`.
	 *
	 * @param tAddr Pointer of the memory to be freed.
	 * @remarks `tAddr` must be previously allocated via `Alloc(size_t)`
	 * function.
	 */
	void Free(void *);
	/**
	 * Change the size of `tAddr` to `tSize` while leaving content of `tAddr` as
	 * is.
	 *
	 * @param tAddr Pointer of the memory to be expand.
	 * @param tSize The new size of `tAddr`.
	 * @return Pointer of the expended/shrink memory.
	 * @remarks If `tSize` is less than original size to tAddr, there are no
	 * guranteed of the content which located after the tSize remain intact.
	 */
	void *ReAlloc(void *, size_t);
	/**
	 * Allocate a specified amount of memory and then initialize it to zero.
	 *
	 * @param tnCount Number of element to be allocated.
	 * @param tSizeOfElem The size of each individual element.
	 * @return Pointer to the allocated memory.
	 * @example The following will allocate memory for array of int which has 5
	 * elements and initial value set to 0.
	 *  int* a = (int *)MemoryManager::AllocInit(5, sizeof(int));
	 */
	void *AllocInit(size_t, size_t);

private:
	HeapBlock *AllocNewPage(size_t);

private:	//	Members
	PageSizes m_pageSize;
	int_t	  m_nMinAllocPages;
	size_t	  m_nAllocated;
	size_t	  m_nInUsed;
	size_t	  m_nWarnCount;
	size_t	  m_nErrCount;
	size_t	  m_nPossibleOverRuns;
	Mutex *	  m_pMutex;

	HeapBlock *m_pRoot;
	HeapBlock *m_pBestBet;
};

}}	  // namespace qkrnl::mm