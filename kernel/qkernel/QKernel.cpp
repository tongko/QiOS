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

#include "../system/QTerm.h"

#include <kernel/QKernel.h>
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

using namespace System;

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
void qkernel_main(__vma mbAddr, __vma kernelEnd) {

	uintptr_t kend = kernelEnd + KERNEL_VAS_OFFSET;

	QTerm term;
	Term = &term;

	Term->SetForeColor(COLOR::GRAY);
	Term->SetBackColor(COLOR::BLACK);
	Term->Clear();
	Term->Print((int8_t *) TXT_LOGO);
	Term->Print((int8_t *) "\n");
	Term->Print((int8_t *) WELCOME_STR);
	Term->Print((int8_t *) "\n");
	Term->Print((int8_t *) COPYRIGHT);

	while (true) {}
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End QKernel.cpp                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

}	 // namespace QKernel