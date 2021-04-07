#pragma once

#include <kernel/types.hpp>

using namespace qklib;

//	prototypes
namespace qklib {
class Mutex;
}

namespace qkrnl { namespace mm {

//	Prototypes
struct AddressRange;

struct PageFrame {
	PageFrame *Next;
	size_t	   Length;
	__pma_t	   PhysAddr;
};

class StackPageAllocator {

public:	   // Constructors
	StackPageAllocator(size_t tInitSizeInPages);
	~StackPageAllocator();

public:	   //	Attribtues
	dword_t GetTotalPages();

public:	   //	Operations
	__pma_t Alloc() __THROW;
	void	Free(__pma_t tAddr) __THROW;

protected:
	__vma_t		  TemporaryMapToPage(__pma_t tAddr);
	AddressRange *GetPageFromBitmap(AddressRange *tpCurrent, int tnPgCount);

private:
	Mutex *		  m_pMutex;
	AddressRange *m_NextFreePage;
};

}}	  // namespace qkrnl::mm