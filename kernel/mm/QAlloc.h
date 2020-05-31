#pragma once

#include <mm/QMemManager.h>
#include <stddef.h>
#include <stdint.h>

struct tag_mem_block;
struct tag_mem_chunk;

namespace QMemory {

struct tag_mem_block;
struct tag_mem_chunk;

typedef struct tag_mem_block {
	QBLOCK *prev;
	QBLOCK *next;
	size_t	size;
	size_t	used;
	QCHUNK *fst_chunk;
} QBLOCK;

typedef struct tag_mem_chunk {
	QCHUNK * prev;
	QCHUNK * next;
	QBLOCK * owner;
	uint32_t magic;
	size_t	 size;
	size_t	 requested;
} QCHUNK;

class QAlloc : public IHeap {
	int	   m_pageSize;
	int	   m_pageCount;
	size_t m_allocated;
	size_t m_inUse;
	size_t m_warnCount;
	size_t m_errCount;
	size_t m_possiOverRuns;

	QBLOCK *m_root;
	QBLOCK *m_bestBet;

public:
	//	Constructors
	QAlloc(int, int, uintptr_t);

public:
	//	Operations
	void *Alloc(size_t);
	void  Free(void *);
	void *ReAlloc(void *, size_t);
	void *CAlloc(size_t, size_t);

private:
	QBLOCK *AllocNewPage(size_t);
};

}	 // namespace QMemory