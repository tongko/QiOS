#pragma once

#include <stddef.h>
#include <stdint.h>

namespace QKernel { namespace QMemory {

class IHeap {

public:
	//	Operations
	virtual void *Alloc(size_t)			  = 0;
	virtual void  Free(void *)			  = 0;
	virtual void *ReAlloc(void *, size_t) = 0;
	virtual void *CAlloc(size_t, size_t)  = 0;
};

}}	  // namespace QKernel::QMemory