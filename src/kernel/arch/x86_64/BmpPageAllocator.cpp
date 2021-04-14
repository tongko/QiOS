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
extern __vma_t			   g_LastUsed;
extern qkrnl::BootParams * g_pBootParams;
extern qkrnl::Logger *	   g_pLogger;
extern qkrnl::Mb2BootInfo *g_pBootInfo;

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
//	Total available bytes, i.e. installed RAM.
static size_t s_nAvailable {0};
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

	//	if needs 512 KB and above, we start the probing from 32 blocks aligned
	if (size > 128) {
		// Compute number of int_t blocks needed.
		int_t needed = size / 32;
		for (int_t i = 0; i < s_nTotal / 32; i++) {
			if (s_pBmp[i] == 0) {	 // this int_t block is empty, let's try it
				int_t j;
				for (j = 0; j < needed; j++) {
					if (s_pBmp[i + j] > 0) {
						//	too bad, doesn't meet requirement
						//	move i forward to i + j
						i = i + j;
						break;
					}
				}

				if (j == needed) {
					// we found enough full int_t blocks,
					// Any remainder block needed?
					int_t k, blocksRequired = size % 32;
					// For this to be continuos blocks, the following needs
					// to be unset from bit 0 onwards
					for (k = 0; k < blocksRequired; k++) {
						int_t bit = 1 << k;
						if (!(s_pBmp[i + j] & bit)) {
							// Good, not used, continue
							continue;
						}

						// to bad, we don't have a contiguos blocks
						break;
					}

					if (k == blocksRequired)
						return i * 32;
				}
			}
		}
	}

	int_t nRemaining = size;
	for (int_t i = 0; i < s_nTotal / 32; i++) {

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
						else {
							i = (startBit + count) / 32;
							break;
						}

						if (free == size)
							return i * 32 + j;	  // free count==size needed;
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
	int_t nNeeded;

	LOCK;

	s_bInitialized = true;

	MemMapEntry *pEntry = g_pBootInfo->pMemMapEntry;
	if (pEntry == nullptr) {
		KernelPanic("[BmpPageAllocator::Initialize] Failed to obtain memory"
					"map info from BootInfo module.\n");
	}

	//	find out total memory
	//	Get mem map from boot info
	size_t nTotalBytes {0};
	size_t nNextStart {0};
	s_pBmp = reinterpret_cast<dword_t *>(bitmapBuffer);
	// lets find out which part in memory are available
	// for free use.
	do {
		if (nNextStart == 0 || pEntry->BaseAddress() == nNextStart) {
			nTotalBytes += pEntry->Length();
		}
		else {
			nTotalBytes
				+= (pEntry->Length() + pEntry->BaseAddress() - nNextStart);
		}

		if (pEntry->Type() == MMapEntryType::Available) {
			s_nAvailable += pEntry->Length();
		}

		nNextStart = pEntry->BaseAddress() + pEntry->Length();
		pEntry = pEntry->GetNext();
	} while (pEntry != nullptr);

	pLog->Print("[BmpPageAllocator] Total physical memory detected: 0x%x KiB\n",
				s_nAvailable / 0x400);

	int_t nPgSize {BLOCK_SIZE};
	//	Compute required block size.
	s_nTotal = nTotalBytes / nPgSize;
	size_t byteNeeded = s_nTotal / BLOCKS_PER_BYTE;

	//	Map the page to it physical address.
	for (__vma_t v = bitmapBuffer; v < (bitmapBuffer + byteNeeded);
		 v += 0x1000) {
		PagingMapPage(v, v - g_pBootParams->VirtualOffset);
	}
	//	By default, all page are set
	memset(s_pBmp, 0xFF, byteNeeded);

	pEntry = g_pBootInfo->pMemMapEntry;
	do {
		if (pEntry->Type() == MMapEntryType::Available) {
			UnsetRegion(pEntry->BaseAddress(), pEntry->Length());
		}
		else {
			SetRegion(pEntry->BaseAddress(), pEntry->Length());
		}

		pEntry = pEntry->GetNext();
	} while (pEntry != nullptr);

	//	Advance g_LastUsed
	bitmapBuffer += byteNeeded;
	s_nUsed = 0;

	// Reclaim usable Low Memory
	// Typically in x86 machine, the low mem begin from 0x500, but we want this
	// to be page aligned, thus let it start from 0x1000.
	UnsetRegion(LOW_MEM_START, LOW_MEM_LEN);

	//	Mark kernel image as used.
	__pma_t physStart
		= g_pBootParams->KernelStart - g_pBootParams->VirtualOffset;
	__pma_t physEnd
		= ALIGN(bitmapBuffer, nPgSize) - g_pBootParams->VirtualOffset;
	size_t len = physEnd - physStart;
	SetRegion(physStart, len);

	//	Mark the modules range as used
	BootModule *pMod = g_pBootInfo->pBootModules;
	while (pMod != nullptr) {
		range_t<__vma_t> rangeMod;
		rangeMod.Start = pMod->BaseAddr + g_pBootParams->VirtualOffset;
		rangeMod.Length = pMod->EndAddr - pMod->BaseAddr;

		SetRegion(pMod->BaseAddr, pMod->EndAddr - pMod->EndAddr);

		pMod = pMod->pNext;
	}

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