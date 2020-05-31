#include "QAlloc.h"

#include <cstddef>
#include <cstdint>
#include <mm/QMemManager.h>
#include <system/ccdefs>

namespace QMemory {

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

#define ALIGNMENT  16
#define ALIGN_TYPE unsigned char
#define ALIGN_INFO sizeof(ALIGN_TYPE) * ALIGNMENT

//	these macro will conveniently align our pointer upwards
#define ALIGN(ptr)                                                 \
	if (ALIGNMENT > 1) {                                           \
		ptr			   = (void *) ((uintptr_t) ptr + ALIGN_INFO);  \
		uintptr_t pPtr = (uintptr_t) ptr;                          \
		uintptr_t diff = pPtr & (ALIGNMENT - 1);                   \
		if (diff != 0) {                                           \
			diff = ALIGNMENT - diff;                               \
			ptr	 = (void *) (pPtr + diff);                         \
		}                                                          \
		*((ALIGN_TYPE *) (pPtr - ALIGN_INFO)) = diff + ALIGN_INFO; \
	}

#define UNALIGN(ptr)                                                       \
	if (ALIGNMENT > 1) {                                                   \
		uintptr_t diff = *((ALIGN_TYPE *) ((uintptr_t) ptr - ALIGN_INFO)); \
		if (diff < (ALIGNMENT + ALIGN_INFO)) {                             \
			ptr = (void *) ((uintptr_t) ptr - diff);                       \
		}                                                                  \
	}

#define CHECK_OVERRUN(p)                         \
	((p & 0xFFFFFF) == (QALLOC_MAGIC & 0xFFFFFF) \
	 || (p & 0xFFFF) == (QALLOC_MAGIC & 0xFFFF)  \
	 || (p & 0xFF) == (QALLOC_MAGIC & 0xFF))

#define QALLOC_MAGIC 0xFEE1F3EE
#define QALLOC_DEAD	 0xDEADFEED

DECLARE_LOCK(QMEM_LOCK);

QAlloc::QAlloc(int pgSize, int pgCount, uintptr_t heapBase)
	: m_pageSize(pgSize)
	, m_pageCount(pgCount)
	, m_allocated(0)
	, m_inUse(0)
	, m_warnCount(0)
	, m_errCount(0)
	, m_possiOverRuns(0)
	, m_root(0)
	, m_bestBet(0) {}

}	 // namespace QMemory