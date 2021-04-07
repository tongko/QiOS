#pragma once

#include <kernel/mm/mm_common.hpp>
#include <kernel/types.hpp>
#include <mutex.hpp>

//	prototypes
namespace qklib {
class Mutex;
}

namespace qkrnl { namespace mm {

//	prototypes
class AddressPool;

// Has the virtual address mapped to physical memory?
#define ARF_MAPPED	  0x01
#define ARF_ALLOCATED 0x02

/**
 * Address range
 */
struct AddressRange {
	AddressRange *Next;
	uintptr_t	  Base;
	dword_t		  Pages;
	byte_t		  Flags;
	bool		  Used;
	AddressPool * Owner;

	inline size_t	 Size();
	inline uintptr_t Tail();
};

/**
 * Address Pool
 * Contains address ranges to be allocate by memory manager.
 */
class AddressPool {

public:	   //	Constructors
	AddressPool(PageSizes tePageSize);

public:	   //	Attributes
	PageSizes GetPageSize();

public:	   // Operations
	/**
	 * Add new address range to the pool
	 *
	 * @param tStart Starting address, must be page size aligned.
	 * @param tEnd Ending address, `tEnd` - `tStart` must be page size aligned.
	 * @remarks If `tStart` or `tEnd` - `tStart` not aligned to page size, the
	 * return result is E_MEMMAN_ADDR_UNALIGNED, otherwise, return result is
	 * S_OK.
	 */
	kresult_t AddRange(uintptr_t tStart, uintptr_t tEnd);
	/**
	 * Clone the address ranges from `tOther`.
	 *
	 * @param tOther The address pool to clone from.
	 */
	kresult_t CloneRanges(AddressPool &tOther);
	/**
	 * Release all address ranges and clear the pool.
	 */
	void ReleaseRanges();
	/**
	 * Merge contiguos ranges.
	 */
	void Merge();
	/**
	 * Allocate new address range.
	 *
	 * @param tPages Number of pages to allocate.
	 * @param tFlags Address range attributes.
	 * @param[out] tBase Place holder for the allocated address.
	 */
	kresult_t Allocate(dword_t tPages, byte_t tFlags, uintptr_t &tBase);
	/**
	 * Release allocated address range.
	 *
	 * @param tBase Allocated address to be freed.
	 * @param[in,out] ttpPagesFreed On enter, this is the number to be free. On
	 * exit, this is the number of pages freed.
	 */
	kresult_t Free(uintptr_t tBase, int_t &tpPagesFreed);
	/**
	 * Get the address ranges in this pool.
	 *
	 * @param tpRanges Place holder for the address aranges.
	 */
	void GetRanges(AddressRange &tpRanges);
	/**
	 * Find an address range.
	 */
	kresult_t Find(uintptr_t tBase);
	/**
	 * For debugging.
	 */
	kresult_t Dump(bool tOnlyFree = false);

private:
	PageSizes	  m_ePgSize;
	AddressRange *m_pFirst;
	qklib::Mutex *m_pMutex;
};

}}	  // namespace qkrnl::mm