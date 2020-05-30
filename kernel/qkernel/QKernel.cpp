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
//   QKernel.cpp                                                              //
//   -  Kernel real entry point.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - qkernel.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - qkernel.cpp] requires __is_kernel__, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <system/cattribs>
#include <system/ccdefs>
#include <system/cversion>

// this function needs to be exists in order to be called when pure virtual
// function cannot be called.
// see https://wiki.osdev.org/C%2B%2B
extern "C" void __cxa_pure_virtual() {
	// Do nothing or print an error message.
}

namespace QKernel {

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

// Up to this point, we are expecting that boot loader have already setup the
// GDT and Paging, and also kernel has been map to higher half VAS.
void qkernel_main(System::VirtAddr mbAddr
				  /*,System::VirtAddr	kiAddr
				  ,System::PhysAddr *usedPageFrames
				  ,uint64_t			pageFrameCount*/
) {
	uint16_t *video_mem = reinterpret_cast<uint16_t *>(0xFFFFFFFFC00B8000);
	for (int i = 0; i < 2000; i += 2) {
		video_mem[i] = ((0x17) << 8) | ' ';	   // F4 - Light gray on Red
	}
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End QKernel.cpp                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

}	 // namespace QKernel