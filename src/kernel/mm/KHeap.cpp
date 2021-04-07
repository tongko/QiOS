#include <kernel/mm/KHeap.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/mm/mm_common.hpp>
#include <mutex.hpp>
#include <new>
#include <string.hpp>

namespace qkrnl { namespace mm {

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5
#define QALLOC_MAGIC 0xFEE1F3EE
#define QALLOC_DEAD	 0xDEADFEED

static const byte_t ALIGNMENT = 0x10;
#define ALIGN_TYPE byte_t
#define ALIGN_INFO sizeof(ALIGN_TYPE) * ALIGNMENT

//	this macro will conveniently align our pointer upwards
#define HEAP_ALIGN(ptr)                                            \
	if (ALIGNMENT > 1) {                                           \
		ptr = (void *) ((__vma_t) ptr + ALIGN_INFO);               \
		__vma_t pPtr = (__vma_t) ptr;                              \
		__vma_t diff = pPtr & (ALIGNMENT - 1);                     \
		if (diff != 0) {                                           \
			diff = ALIGNMENT - diff;                               \
			ptr = (void *) (pPtr + diff);                          \
		}                                                          \
		*((ALIGN_TYPE *) (pPtr - ALIGN_INFO)) = diff + ALIGN_INFO; \
	}

#define UNALIGN(ptr)                                                   \
	if (ALIGNMENT > 1) {                                               \
		__vma_t diff = *((ALIGN_TYPE *) ((__vma_t) ptr - ALIGN_INFO)); \
		if (diff < (ALIGNMENT + ALIGN_INFO)) {                         \
			ptr = (void *) ((__vma_t) ptr - diff);                     \
		}                                                              \
	}

#define CHECK_OVERRUN(p)                         \
	((p & 0xFFFFFF) == (QALLOC_MAGIC & 0xFFFFFF) \
	 || (p & 0xFFFF) == (QALLOC_MAGIC & 0xFFFF)  \
	 || (p & 0xFF) == (QALLOC_MAGIC & 0xFF))

/**
 * Constructor
 */
KHeap::KHeap(PageSizes tBlockSize,
			 int_t	   tMinAlloc,
			 __vma_t   tBaseAddr,
			 size_t	   tBaseLength)
	: m_pageSize(tBlockSize)
	, m_nMinAllocPages(tMinAlloc)
	, m_nAllocated(0)
	, m_nInUsed(0)
	, m_nWarnCount(0)
	, m_nErrCount(0)
	, m_nPossibleOverRuns(0)
	, m_pRoot(0)
	, m_pBestBet(0) {

	HeapBlock *block;

	if (tBaseAddr) {
		__vma_t base = tBaseAddr;

		m_pMutex = new ((byte_t *) base) qklib::Mutex();
		base += sizeof(qklib::Mutex);

		block = reinterpret_cast<HeapBlock *>(base);
		block->Size = tBaseLength - sizeof(qklib::Mutex);
	}
	else {
		size_t size
			= 0x1000000 / GetPageSize(m_pageSize);	  // KHeap default is 16 MiB
		m_pMutex = new qklib::Mutex();
		block = reinterpret_cast<HeapBlock *>(
			MemoryManager::VirtualAlloc(size, m_pageSize));
		block->Size = size;
	}

	block->Prev = block->Next = nullptr;
	block->Used = sizeof(HeapBlock);
	block->FirstChunk = nullptr;
	block->VirtualAddress = reinterpret_cast<__vma_t>(block);

	m_nAllocated += block->Size;
	m_pRoot = block;
}
/**
 * Destructor
 */
KHeap::~KHeap() {
	// Release the initial block
	if (m_pRoot) {
		MemoryManager::VirtualFree((void *) m_pRoot, m_pRoot->Size, m_pageSize);
	}
}
/**
 * AllocNewPage()
 */
HeapBlock *KHeap::AllocNewPage(size_t size) {

	// This is how much space is required.
	size_t st = size + sizeof(HeapBlock);
	st += sizeof(HeapChunk);

	int_t pageSize = GetPageSize(m_pageSize);
	if ((st % pageSize) == 0)
		st = st / pageSize;
	else
		st = st / pageSize + 1;

	// Make sure it's >= the min size;
	if ((int_t) st < m_nMinAllocPages)
		st = m_nMinAllocPages;

	HeapBlock *block
		= (HeapBlock *) MemoryManager::VirtualAlloc(st, m_pageSize);

	if (block == nullptr) {
		m_nWarnCount++;

		return nullptr;
	}

	block->Prev = block->Next = nullptr;
	block->Size = st * pageSize;
	block->Used = sizeof(HeapBlock);
	block->FirstChunk = nullptr;

	m_nAllocated += block->Size;

	return block;
}
/**
 * Alloc()
 */
void *KHeap::Alloc(size_t reqSize) {

	size_t size = reqSize;
	if (ALIGNMENT > 1) {
		size += ALIGNMENT + ALIGN_INFO;
	}

	m_pMutex->Acquire();

	if (size == 0) {
		m_nWarnCount++;
		m_pMutex->Release();
		return nullptr;
	}

	if (m_pRoot == nullptr) {
		m_pRoot = this->AllocNewPage(size);
		if (m_pRoot == nullptr) {
			m_pMutex->Acquire();
			return nullptr;
		}
	}

	HeapBlock *block = m_pRoot;
	int_t	   started_bet = 0;
	size_t	   best_size = 0;

	if (m_pBestBet != nullptr) {
		best_size = m_pBestBet->Size - m_pBestBet->Used;

		if (best_size > (size + sizeof(HeapChunk))) {
			block = m_pBestBet;
			started_bet = 1;
		}
	}

	void * p = nullptr;
	size_t diff = 0;
	while (block != nullptr) {
		diff = block->Size - block->Used;

		if (best_size < diff) {
			m_pBestBet = block;
			best_size = diff;
		}

#ifdef USE_CASE1

		// CASE 1: There is not enough space in this HeapBlock.
		if (diff < (size + sizeof(HeapChunk))) {

			//	Maybe the Next HeapBlock will have enough?
			if (block->Next != nullptr) {
				block = block->Next;
				continue;
			}

			// If we started at the best bet (which could be in the middle),
			// let's start all over again.
			if (started_bet == 1) {
				block = m_pRoot;
				started_bet = 0;
				continue;
			}

			// Create a new HeapBlock Next to this one and...
			block->Next = this->AllocNewPage(size);
			if (block->Next == nullptr)
				break;	  // Out of memory
			block->Next->Prev = block;
			block = block->Next;

			// Fall through CASE 2...
		}

#endif

#ifdef USE_CASE2

		// CASE 2: It's a brand new block.
		if (block->FirstChunk == nullptr) {
			block->FirstChunk
				= (HeapChunk *) ((__vma_t) block + sizeof(HeapBlock));

			block->FirstChunk->Magic = QALLOC_MAGIC;
			block->FirstChunk->Prev = nullptr;
			block->FirstChunk->Next = nullptr;
			block->FirstChunk->Owner = block;
			block->FirstChunk->Size = size;
			block->Used += size + sizeof(HeapChunk);

			m_nInUsed += size;

			p = (void *) ((__vma_t)(block->FirstChunk) + sizeof(HeapChunk));

			HEAP_ALIGN(p);

			m_pMutex->Release();
			return p;
		}

#endif

#ifdef USE_CASE3

		// CASE 3: Block in use and enough space at the start of the block.
		diff = (__vma_t)(block->FirstChunk);
		diff -= (__vma_t) block;
		diff -= sizeof(HeapBlock);

		if (diff >= (size + sizeof(HeapChunk))) {
			block->FirstChunk->Prev
				= (HeapChunk *) ((__vma_t) block + sizeof(HeapBlock));
			block->FirstChunk->Prev->Next = block->FirstChunk;
			block->FirstChunk = block->FirstChunk->Prev;

			block->FirstChunk->Magic = QALLOC_MAGIC;
			block->FirstChunk->Prev = nullptr;
			block->FirstChunk->Owner = block;
			block->FirstChunk->Size = size;
			block->FirstChunk->Requested = reqSize;
			block->Used += size + sizeof(HeapChunk);

			m_nInUsed += size;

			p = (void *) ((__vma_t)(block->FirstChunk) + sizeof(HeapChunk));
			HEAP_ALIGN(p);

			m_pMutex->Release();
			return p;
		}

#endif

#ifdef USE_CASE4

		// CASE 4: There is enough space in this block. But is it contiguoes?
		HeapChunk *chunk = block->FirstChunk;

		while (chunk != nullptr) {
			// CASE 4.1: End of chunks in block. Space from last and end?
			if (chunk->Next == nullptr) {
				// the rest of this block is free... is it big enough?
				diff = (__vma_t) block + block->Size;
				diff -= (__vma_t) chunk;
				diff -= sizeof(HeapChunk);
				diff -= chunk->Size;

				if (diff >= (size + sizeof(HeapChunk))) {
					chunk->Next
						= (HeapChunk *) ((__vma_t) chunk + sizeof(HeapChunk)
										 + chunk->Size);
					chunk->Next->Prev = chunk;
					chunk = chunk->Next;
					chunk->Next = nullptr;
					chunk->Magic = QALLOC_MAGIC;
					chunk->Owner = block;
					chunk->Size = size;
					chunk->Requested = reqSize;
					block->Used += size + sizeof(HeapChunk);

					m_nInUsed += size;

					p = (void *) ((__vma_t) chunk + sizeof(HeapChunk));
					HEAP_ALIGN(p);

					m_pMutex->Release();
					return p;
				}
			}

			// CASE 4.2: Is there space between two chunks?
			if (chunk->Next != nullptr) {
				// is the difference between here and Next big enough?
				diff = (__vma_t) chunk->Next;
				diff -= (__vma_t) chunk;
				diff -= sizeof(HeapChunk);
				diff -= chunk->Size;

				HeapChunk *newChunk;
				if (diff >= size + sizeof(HeapChunk)) {
					newChunk
						= (HeapChunk *) ((__vma_t) chunk + sizeof(HeapChunk)
										 + chunk->Size);
					newChunk->Magic = QALLOC_MAGIC;
					newChunk->Next = chunk->Next;
					newChunk->Prev = chunk;
					newChunk->Size = size;
					newChunk->Requested = reqSize;
					newChunk->Owner = block;

					chunk->Next->Prev = newChunk;
					chunk->Next = newChunk;
					block->Used += size + sizeof(HeapChunk);

					m_nInUsed += size;

					p = (void *) ((__vma_t) newChunk + sizeof(HeapChunk));
					HEAP_ALIGN(p);

					m_pMutex->Release();
					return p;
				}
			}	 // chunk->Next != nullptr

			chunk = chunk->Next;
		}	 // while (chunk != nullptr)

#endif

#ifdef USE_CASE5

		// CASE 5: Block full! Ensure Next block and loop
		if (block->Next == nullptr) {
			if (started_bet == 1) {
				block = m_pRoot;
				started_bet = 0;
				continue;
			}

			// we've run out. we need more
			// Next one guaranteed to be ok, or not.
			block->Next = this->AllocNewPage(size);
			if (block->Next == nullptr)
				break;	  // out of memory
			block->Next->Prev = block;
		}

#endif

		block = block->Next;
	}

	m_pMutex->Release();

	return nullptr;
}
/**
 * Free()
 */
void KHeap::Free(void *ptr) {

	if (ptr == nullptr) {
		m_nWarnCount++;
		return;
	}

	UNALIGN(ptr);

	m_pMutex->Acquire();

	HeapChunk *chunk = (HeapChunk *) ((__vma_t) ptr - sizeof(HeapChunk));

	if (chunk->Magic != QALLOC_MAGIC) {
		m_nErrCount++;

		// Check for overrun errors. For all bytes of QALLOC_MAGIC
		if CHECK_OVERRUN (chunk->Magic) {
			m_nPossibleOverRuns++;
		}

		m_pMutex->Release();
		return;
	}

	HeapBlock *block = chunk->Owner;
	m_nInUsed -= chunk->Size;

	block->Used -= chunk->Size + sizeof(HeapChunk);
	chunk->Magic = QALLOC_DEAD;

	if (chunk->Next != nullptr)
		chunk->Next->Prev = chunk->Prev;
	if (chunk->Prev != nullptr)
		chunk->Prev->Next = chunk->Next;

	if (chunk->Prev == nullptr)
		block->FirstChunk = chunk->Next;

	// Block completely unused
	if (block->FirstChunk == nullptr) {
		if (m_pRoot == block)
			m_pRoot = block->Next;
		if (m_pBestBet == block)
			m_pBestBet = nullptr;
		if (block->Prev != nullptr)
			block->Prev->Next = block->Next;
		if (block->Next != nullptr)
			block->Next->Prev = block->Prev;
		m_nAllocated -= block->Size;

		MemoryManager::VirtualFree((void *) block, block->Size, m_pageSize);
	}
	else {
		if (m_pBestBet != nullptr) {
			size_t best_size = m_pBestBet->Size - m_pBestBet->Used;
			size_t block_size = block->Size - block->Used;

			if (block_size > best_size)
				m_pBestBet = block;
		}
	}

	m_pMutex->Release();
}
/**
 * Realloc()
 */
void *KHeap::ReAlloc(void *p, size_t size) {

	// Honour the case of size == 0 => free old and return nullptr
	if (size == 0) {
		this->Free(p);
		return nullptr;
	}

	// In the case of a nullptr, return a simple alloc.
	if (p == nullptr)
		return this->Alloc(size);

	void *ptr = p;
	UNALIGN(ptr);

	m_pMutex->Acquire();

	HeapChunk *chunk = (HeapChunk *) ((__vma_t) ptr - sizeof(HeapChunk));

	// Ensure it is a valid structure.
	if (chunk->Magic != QALLOC_MAGIC) {
		m_nErrCount++;

		// Check for overrun errors. For all bytes of QALLOC_MAGIC
		if CHECK_OVERRUN (chunk->Magic) {
			m_nPossibleOverRuns++;
		}

		m_pMutex->Release();
		return nullptr;
	}

	// Definitely a memory block.
	size_t real_size = chunk->Requested;
	if (real_size >= size) {
		chunk->Requested = size;
		m_pMutex->Release();
		return p;
	}

	m_pMutex->Release();

	// If we got here then we're reallocating to a block bigger than us.
	ptr = this->Alloc(size);
	qklib::memcpy((void *) ptr, (void *) p, real_size);
	this->Free(p);

	return ptr;
}
/**
 * AllocInit()
 */
void *KHeap::AllocInit(size_t obj, size_t size) {

	size_t real_size = obj * size;
	void * p = this->Alloc(real_size);

	qklib::memset((void *) p, (byte_t) 0, real_size);

	return p;
}

}}	  // namespace qkrnl::mm