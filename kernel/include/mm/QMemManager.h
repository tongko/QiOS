#pragma once

#include <stddef.h>
#include <stdint.h>

namespace QMemory {

#define KERNEL_VAS_OFFSET 0xFFFFFFFF80000000;

class IHeap {

public:
	//	Operations
	virtual void *Alloc(size_t)			  = 0;
	virtual void  Free(void *)			  = 0;
	virtual void *ReAlloc(void *, size_t) = 0;
	virtual void *CAlloc(size_t, size_t)  = 0;
};

}	 // namespace QMemory
