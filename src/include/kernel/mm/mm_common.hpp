#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace mm {

//	Low memory start
// Should be 0x500, but we want page size aligned
const __pma_t LOW_MEM_START {0x1000};
//	Low memory length (Free mem length)
const size_t LOW_MEM_LEN {0x7F000};
//	Block size
const int_t BLOCK_SIZE {0x1000};
//	Blocks per byte
const int_t	  BLOCKS_PER_BYTE {8};
const __vma_t g_KERNEL_VIRT_START {0xFFFFFE8000000000};
// Last page will be used as Phys Mem Stack pointer
const __vma_t g_KERNEL_VIRT_LENGTH {0x0000008000000000};

enum class PageSizes { Unknown, FourK, TwoM, OneG };

inline dword_t GetPageSize(PageSizes tpgSize) {

	switch (tpgSize) {
	case PageSizes::FourK: return 0x1000;
	case PageSizes::TwoM: return 0x200000;
	case PageSizes::OneG: return 0x40000000;
	default: return 0;
	}
}

/**
 * Get a bool value indicate MemoryManager has been initialized.
 *
 * @return true if MemoryManager has been initialized, otherwise, false.
 */
bool MMHasInitialized();

}}	  // namespace qkrnl::mm