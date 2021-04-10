#include <cdefs.h>
#include <kernel/Logger.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/mm/BmpPageAllocator.hpp>
#include <kernel/mm/Paging.hpp>
#include <kernel/mm/mm_common.hpp>
#include <kernel/types.hpp>
#include <mutex.hpp>
#include <string.hpp>

using namespace qklib;
using namespace qkrnl;

//	Global variables
extern __vma_t			  g_LastUsed;
extern qkrnl::BootParams *g_pBootParams;
extern qkrnl::Logger *	  g_pLogger;

#define LOCK   s_Mutex.Acquire()
#define UNLOCK s_Mutex.Release()

namespace qkrnl { namespace mm {

//	Initialized flag
static bool s_bInitialized {false};
//	Pointer to the bitmap
static dword_t *s_pBmp {nullptr};
//	Used blocks
static int_t s_nUsed {0};
//	Total blocks
static int_t s_nTotal {0};
//	Total available blocks, i.e. installed RAM.
static int_t s_nAvailable {0};
//	Block sizes
static int_t s_nSize {0x1000};

static Mutex s_Mutex;

/**
 * Test the status of a page specified by bit.
 */
static inline bool TestBit(int_t bit) {

	return s_pBmp[bit / 32] & (1 << (bit % 32));
}
/**
 * Set page specified by bit as used.
 */
static inline void Set(int_t bit) {

	bool bOld = TestBit(bit);
	s_pBmp[bit / 32] |= (1 << (bit % 32));
	if (!bOld)
		s_nUsed++;
}
/**
 * Set page specified by bit as unused.
 */
static inline void Unset(int_t bit) {

	bool bOld = TestBit(bit);
	s_pBmp[bit / 32] &= ~(1 << (bit % 32));
	if (bOld)
		s_nUsed--;
}
/**
 * Find a free page by traversing from begin of bitmap to the end.
 */
static int_t FirstFree() {

	for (int_t i = 0; i < s_nTotal; i++) {

		if (s_pBmp[i] != 0xFFFFFFFF) {
			for (int_t j = 0; j < 32; j++) {

				int_t bit = 1 << j;
				if (!(s_pBmp[i] & bit))
					return i * 4 * 8 + j;
			}
		}
	}

	return -1;
}
/**
 * Find number specified by `tnSize` of free page by traversing from begin to
 * the end.
 */
static int_t FirstFree(int_t tnSize) {

	int_t size = tnSize;

	if (size == 0) {
		return -1;
	}

	if (size == 1) {
		return FirstFree();
	}

	for (int_t i = 0; i < s_nTotal; i++) {

		if (s_pBmp[i] != 0xFFFFFFFF) {

			for (int_t j = 0; j < 32; j++) {

				int_t bit = 1 << j;
				if (!(s_pBmp[i] & bit)) {

					int_t startBit = i * 32;
					//	get the free bit in the dword at index i
					startBit += bit;

					// loop through each bit to see if is fit
					int_t free = 0;
					for (int_t count = 0; count <= size; count++) {

						if (!TestBit(startBit + count))
							free++;	   // this bit is clear (free frame)

						if (free == size)
							return i * 4 * 8 + j;	 // free count==size needed;
													 // return index
					}
				}
			}
		}
	}

	return -1;
}

/**
 * GetTotalBlocks()
 */
int_t BmpPageAllocator::GetTotalBlocks() {

	return s_nAvailable;
}

/**
 * GetUsedBlocks()
 */
int_t BmpPageAllocator::GetUsedBlocks() {

	return s_nUsed;
}

/**
 * Initialize()
 */
kresult_t BmpPageAllocator::Initialize(__vma_t &bitmapBuffer) {

	if (s_bInitialized) {
		return S_OK;
	}

	Logger *pLog = g_pLogger;
	pLog->Print("[BmpPageAllocator::Initialize] Initializing bitmap page "
				"allocator...\n");
	//	find out total memory
	MemMapEntry *entries;
	int_t		 nNeeded;

	LOCK;

	s_bInitialized = true;

	//	Allocate free mem from g_LastUsed
	entries = (MemMapEntry *) g_LastUsed;
	//	Enumerate mem map entry into `entries`.
	if (!BootInfoEnumMemMap(entries, 0x1000, &nNeeded) || nNeeded <= 0) {
		KernelPanic("[BmpPageAllocator::Initialize] Failed to obtain memory "
					"map info from BootInfo module.");
	}
	//	Advance g_LastUsed
	g_LastUsed += nNeeded;

	int_t  nEntries = nNeeded / sizeof(MemMapEntry);
	size_t nTotalBytes {0};
	int_t  nPgSize {BLOCK_SIZE};

	//	Get total installed memory.
	size_t nNextStart = 0;
	for (int i = 0; i < nEntries; i++) {
		if (i == 0 || entries[i].BaseAddress() == nNextStart) {
			nTotalBytes += entries[i].Length();
		}
		else {
			nTotalBytes += (entries[i].Length() + entries[i].BaseAddress()
							- nNextStart);
		}

		nNextStart = entries[i].Length() + entries[i].BaseAddress();
	}

	pLog->Print("[BmpPageAllocator] Total physical memory detected: 0x%x KiB\n",
				nTotalBytes / 0x400);
	//	Compute required block size.
	s_nTotal = nTotalBytes / nPgSize;
	size_t byteNeeded = s_nTotal / BLOCKS_PER_BYTE;
	//	Allocate new.
	s_pBmp = reinterpret_cast<dword_t *>(bitmapBuffer);
	for (__vma_t v = bitmapBuffer; v < (bitmapBuffer + byteNeeded);
		 v += 0x1000) {
		PagingMapPage(v, v - g_pBootParams->VirtualOffset);
	}
	//	Advance g_LastUsed
	bitmapBuffer += byteNeeded;
	//	By default, all page are set
	memset(s_pBmp, 0xFF, byteNeeded);

	// lets find out which part in memory are available
	// for free use.
	for (int i = 0; i < nEntries; i++) {
		if (entries[i].Type() != MMapEntryType::Available) {
			continue;
		}

		s_nAvailable += entries[i].Length();
		UnsetRegion(entries[i].BaseAddress(), entries[i].Length());
	}
	s_nUsed = 0;

	// Reclaim usable Low Memory
	// Typically in x86 machine, the low mem begin from 0x500, but we want this
	// to be page aligned, thus let it start from 0x1000.
	UnsetRegion(LOW_MEM_START, LOW_MEM_LEN);

	//	for which kernel occupied, we need to set it to used.
	__pma_t physStart
		= g_pBootParams->KernelStart - g_pBootParams->VirtualOffset;
	__pma_t physEnd
		= ALIGN(bitmapBuffer, nPgSize) - g_pBootParams->VirtualOffset;
	size_t len = physEnd - physStart;
	SetRegion(physStart, len);

	UNLOCK;

	return S_OK;
}

kresult_t BmpPageAllocator::SetRegion(__pma_t tAddr, size_t tnSize) {

	LOCK;

	int_t align = tAddr / s_nSize;
	int_t blocks = tnSize / s_nSize;

	for (; blocks; blocks--) {
		Set(align++);
	}

	UNLOCK;

	return S_OK;
}

kresult_t BmpPageAllocator::UnsetRegion(__pma_t tAddr, size_t tnSize) {

	LOCK;

	int_t align = tAddr / s_nSize;
	int_t blocks = tnSize / s_nSize;

	for (; blocks; blocks--) {
		Unset(align++);
	}

	Set(0);	   // First block is always set. This is to ensure allocs can't be 0

	UNLOCK;

	return S_OK;
}

kresult_t BmpPageAllocator::Alloc(size_t tnSize, __pma_t *tAddr) {

	LOCK;

	if ((size_t)(s_nTotal - s_nUsed) < tnSize) {
		//	OOM
		s_Mutex.Release();
		return E_MEMMAN_OUTOFMEM;
	}

	int_t frame = FirstFree(tnSize);

	if (frame == -1) {
		//	OOM
		s_Mutex.Release();
		return E_MEMMAN_OUTOFMEM;
	}

	for (size_t i = 0; i < tnSize; i++) {
		Set(frame + i);
	}

	*tAddr = (__pma_t) frame * s_nSize;

	UNLOCK;

	return S_OK;
}

kresult_t BmpPageAllocator::Free(__pma_t tAddr, size_t tnSize) {

	LOCK;

	int_t frame = tAddr / s_nSize;

	for (size_t i = 0; i < tnSize; i++) {

		Unset(frame + i);
	}

	UNLOCK;

	return S_OK;
}

}}	  // namespace qkrnl::mm