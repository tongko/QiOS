#pragma once

#include <cdefs.h>
#include <kernel/types.hpp>

namespace qkrnl { namespace mm {

/**
 * Bitmap page frame allocator.
 */
class BmpPageAllocator {

public:	   // Attirbutes
	static int_t GetTotalBlocks() __THROW;
	static int_t GetUsedBlocks() __THROW;

public:	   //	Operations
	static kresult_t Initialize(__vma_t &bitmapBuffer) __THROW;
	static kresult_t SetRegion(__pma_t tAddr, size_t tnSize) __THROW;
	static kresult_t UnsetRegion(__pma_t tAddr, size_t tnSize) __THROW;
	static kresult_t Alloc(size_t tnSize, __pma_t *tAddr) __THROW;
	static kresult_t Free(__pma_t tAddr, size_t tnSize) __THROW;

private:	// Constructors
	BmpPageAllocator() {};
};

}}	  // namespace qkrnl::mm