#pragma once

#include <kernel/types.hpp>

//	prototypes
namespace qklib {
class Mutex;
}

namespace qkrnl { namespace mm {

//	Prototypes
class BmpPageAllocator;
class StackPageAllocator;

class PageAllocator {

public:	   //	Constructor
	PageAllocator();

public:	   //	Attributes
	/**
	 * Get total installed memory on this machine, in bytes.
	 */
	size_t GetTotalBytes();
	/**
	 * Get used memory in bytes
	 */
	size_t GetUsedBytes();

public:	   //	Operatinos
	/**
	 * Allocate free page specified by `tnPageCount`
	 *
	 * @param tnPageCount Number of free page to be allocated.
	 */
	__pma_t Alloc(dword_t tnPageCount);
	/**
	 * Free allocated pages.
	 *
	 * @param tAddr Physical address of the allocated pages.
	 * @param tnPageCount Number of pages to be freed.
	 */
	void Free(__pma_t tAddr, dword_t tnPageCount = 1);

private:
	qklib::Mutex *		m_pMutex;
	StackPageAllocator *m_pStack;
};

}}	  // namespace qkrnl::mm