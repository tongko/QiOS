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
//   CopyMemory.cpp                                                           //
//   -  This function copy content of memory from source to destination.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - CopyMemory.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - CopyMemory.cpp] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <system/MemLib.h>
#include <system/ccdefs>

namespace System {

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

uintptr_t CopyMemory(uintptr_t src, uintptr_t dst, size_t count) {

	byte * s = reinterpret_cast<byte *>(src);
	byte * d = reinterpret_cast<byte *>(dst);
	size_t c = count;

	// Copy byte by byte until we reach 64 bits boundry
	while (c > 0 && c % 64) {
		*d++ = *s++;
		c--;
	}

	//	If we have remaining bytes to copy...
	if (c) {
		uint64_t *pS = reinterpret_cast<uint64_t *>(s);
		uint64_t *pD = reinterpret_cast<uint64_t *>(d);
		c /= 64;
		if (c) {
			while (c-- > 0) {
				*pD++ = *pS++;
			}
		}
	}

	return dst;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End CopyMemory.cpp                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

}	 // namespace System
