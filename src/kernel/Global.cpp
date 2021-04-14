#include <cdefs.h>
#include <kernel/Logger.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/mm/KHeap.hpp>
#include <kernel/mm/mm_common.hpp>

//	All global variables should be placed here.

__vma_t				 g_LastUsed;
size_t				 g_nHeapSize;
qkrnl::mm::PageSizes g_ePageSize;
qkrnl::mm::KHeap *	 g_pHeap;
qkrnl::Logger *		 g_pLogger;
qkrnl::BootParams *	 g_pBootParams;
qkrnl::Mb2MMapEntry *g_pBootInfo;
