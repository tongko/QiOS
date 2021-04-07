#pragma once

#define E_FAILED_MASK 0x80000000

#define FAILED(x)  (x & E_FAILED_MASK)
#define SUCCEED(x) (!(x & E_FAILED_MASK))

#define S_OK 0x00000000

#define E_FAC_GENERIC	 0x00010000	   //	Generic facility code.
#define E_ARG_INVL		 0x80010001	   //	Invalid argument specified.
#define E_ARG_OUTOFRANGE 0x80010002	   //	Specified arguments out of range.
#define E_ARG_NULL		 0x80010003	   //	Specified argument is null.

#define E_FAC_MEMMAN	  0x00020000	//	Memory management module facility code.
#define E_MEMMAN_OUTOFMEM 0x80020001	//	Out of memory!
#define E_MEMMAN_VIRTNOTMAPPED \
	0x80020002	  //	Virtual address not mapped ot any physical address.
#define E_MEMMAN_ADDR_UNALIGNED 0x80020003	  //	Address is unaligned
#define E_MEMMAN_ENTRY_EXISTS \
	0x80020004	  //	Entry already mapped to another physical address
#define E_MEMMAN_VIRTNOTEXISTS \
	0x80020005	  // Virtual address does not exist, or not in range.
#define E_MEMMAN_VIRTNOTINUSED \
	0x80020006	  // Virtual address not in used, possible multiple free calls
				  // for same range.
#define E_MEMMAN_AR_EMPTY	   0x80020007	 // Address range pool empty.
#define E_MEMMAN_PHYSNOTMAPPED 0x8002008	 // Physical address is free

#include <cdefs.h>

namespace qkrnl {

_NORETURN_ void KernelPanic(const char *fmt, ...);

}