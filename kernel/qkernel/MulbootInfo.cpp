/*== QiOS =====================================================================+
|                                                                              |
|    Author: Liew Tze Wei                                                      |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   MulbootInfo.cpp                                                          //
//   -  Provider service to access multiboot information provided by GRUB.    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - MulbootInfo.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - MulbootInfo.cpp] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <cstdbool>
#include <multiboot/MultibootInfo>
#include <stddef.h>
#include <system/ccdefs>
using namespace System;

namespace QKernel { namespace Multiboot {

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////

//	Short hand for multiboot info structs
typedef multiboot_tag		 MBTag;
typedef multiboot_mmap_entry MMEntry;

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER //
////////////////////////////////////////////////////////////////////////////////

static bool				   _initialized = 0;
static VirtAddr			   _pBootInfo;
static uint32_t			   _nInfoSize;
static multiboot_tag_mmap *_pMemMap;

static void LoadMultibootInfo(VirtAddr pMbInfo) {

	_pBootInfo = pMbInfo;
	_nInfoSize = *((uint32_t *) pMbInfo);

	MBTag *tag;
	for (tag = (MBTag *) (pMbInfo + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
		 tag = (MBTag *) (uint8_t *) tag + ((tag->size + 7) & ~7)) {
		//	For now, we only interested in memory map
		switch (tag->type) {
		case MULTIBOOT_TAG_TYPE_MMAP: {
			_pMemMap = (multiboot_tag_mmap *) tag;
			// uint32_t			  i = 0;
			// MMEntry *mmap;
			// for (mmap = ((multiboot_tag_mmap *) tag)->entries;
			// 	 (uint8_t *) mmap < (uint8_t *) tag + tag->size;
			// 	 mmap = (MMEntry *) ((uintptr_t) mmap
			// 									  + ((multiboot_tag_mmap *) tag)
			// 											->entry_size),
			// 	i++) {
			// 	_mmap[i].addr = mmap->addr;
			// 	_mmap[i].len  = mmap->len;
			// 	_mmap[i].type = mmap->type;
			// 	_mmap[i].zero = mmap->zero;
			// }

			return;
		}
		default: continue;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE/CLASSES IMPLEMENTATIONS                                       //
////////////////////////////////////////////////////////////////////////////////

// MemoryMap::MemoryMap()
// 	: m_pCurrent(0)
// 	, m_pPtr(_pMemMap) {}

// MMEntry &MemoryMap::Current() const {

// 	return (MMEntry &const) m_pCurrent;
// }

// bool MemoryMap::MoveNext() {

// 	multiboot_tag_mmap *tag = (multiboot_tag_mmap *) m_pPtr;

// 	if (m_pCurrent == NULL)
// 		m_pCurrent = tag->entries;
// 	else
// 		m_pCurrent = (MMEntry *) (tag->entry_size + ((uint8_t *) m_pCurrent));

// 	if (((uint8_t *) m_pCurrent) >= ((uint8_t *) tag) + tag->size)
// 		return false;

// 	return true;
// }

// void MemoryMap::Reset() {
// 	m_pCurrent = NULL;
// }

// MulbootInfo::MulbootInfo()
// 	: m_mb2i(0) {}

// void MulbootInfo::set_MBInfoAddr(uintptr_t pMbInfo) {
// 	if (!pMbInfo || pMbInfo & 7)
// 		return;

// 	m_mb2i = pMbInfo;

// 	//	nullified _mmap
// 	unsigned char *p = (unsigned char *) &_mmap[0];
// 	for (short i = 0; i < MAX_MMAP; i++) {
// 		*p++ = 0;
// 	}

// 	LoadMultibootInfo(m_mb2i);
// }

// MMEntry *MulbootInfo::GetMemoryMap(void) const {
// 	return &_mmap[0];
// }

// void MulbootInfo::RelocateTo(uintptr_t newLocation) {
// 	//	Currently not supported
// 	return;
// }

void MBInitInfo(VirtAddr memInfo) {

	if (_initialized)
		return;

	LoadMultibootInfo(memInfo);

	_initialized = true;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End MulbootInfo.cpp                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

}}	  // namespace QKernel::Multiboot
