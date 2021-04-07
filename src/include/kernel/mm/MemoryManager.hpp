#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace mm {

//	prototypes
enum class PageSizes;
class AddressPool;
class PageAllocator;

/**
 * Memory Management sub-module, all memory related operation should be done via
 * calling member function of this class.
 */
class MemoryManager {

public:	   //	Attributes
	/**
	 * Get total number of physical memory installed.
	 *
	 * @return Total physical memory, in bytes.
	 */
	static size_t GetTotalBytes();
	/**
	 * Get total used physical memory.
	 *
	 * @return Total used physical memory, in bytes.
	 */
	static size_t GetUsedBytes();

public:	   //	Operations
	/**
	 * Initializing Memory Manager module.
	 */
	static kresult_t Initialize();
	/**
	 * Allocate free pages specifed by `tPageCount`.
	 *
	 * @param tPageCount Number of pages to allocate.
	 * @param tPageSize Specified the size of a page.
	 * @return Pointer to the allocated memory.
	 */
	static void *VirtualAlloc(int_t		tPageCount,
							  PageSizes tPageSize,
							  bool		tbNoMap = false);
	/**
	 * Free pages previously allocated via `VirtualAlloc(int_t, PageSizes).
	 *
	 * @param tpAddr Pointer to the memory to be freed.
	 * @param tPageCount Number of pages to be freed.
	 * @param tPageSizes Size of the pages.
	 */
	static void VirtualFree(void *	  tpAddr,
							int_t	  tPageCount,
							PageSizes tPageSize);
	/**
	 * Allocate new physical page.
	 *
	 * @param tPageCount Number of pages to allocate.
	 * @return The physical address of the allocated page/s.
	 */
	static __pma_t PhysAlloc(int_t tPageCount, PageSizes tePageSizes);
	/**
	 * Free pages previous allocated via `PhysAlloc(int_t)`
	 */
	static void PhysFree(__pma_t   tAddr,
						 int_t	   tPageCount,
						 PageSizes tePageSizes);

private:	// Constructor
	MemoryManager() {};
};

}}	  // namespace qkrnl::mm