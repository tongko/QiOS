#pragma once

#include <common.hpp>

/* All functions, except those with callbacks or those that
   synchronize memory, are leaf functions.  */
#define __LEAF		, __leaf__
#define __LEAF_ATTR __attribute__((__leaf__))

/* GCC can always grok prototypes.  For C++ programs we add throw()
   to help it optimize the function calls.  But this works only with
   gcc 2.8.x and egcs.  For gcc 3.2 and up we even mark C functions
   as non-throwing using a function attribute since programs can use
   the -fexceptions options for C code as well.  */
#define __THROW		 __attribute__((nothrow))
#define __THROWNL	 __attribute__((nothrow))
#define __NTH(fct)	 __LEAF_ATTR fct throw()
#define __NTHNL(fct) fct throw()

#define __glibc_clang_has_extension(ext) 0

/* For these things, GCC behaves the ANSI way normally,
   and the non-ANSI way under -traditional.  */

#define __CONCAT(x, y) x##y
#define __STRING(x)	   #x

/* This is not a typedef so `const __ptr_t' does the right thing.  */
#define __ptr_t void *

#define _SECT_(s)					__attribute__((section(#s)))
#define _ALIGNED_(x)				__attribute__((aligned(x)))
#define _PACKED_					__attribute__((packed))
#define _ASM_(x...)					__asm__(x)
#define _ASMV_(x...)				__asm__ volatile(x)
#define _FORCE_INLINE_				__attribute__((always_inline))
#define _COMPARE_EXCHANGE_(x, y, z) __sync_bool_compare_and_swap(x, y, z)
#define _RESTRICT_					__restrict
#define _NORETURN_					__attribute__((noreturn))
#define _CODE_UNREACHABLE_			__builtin_unreachable();
#define ALIGN(x, y)                               \
	({                                            \
		__typeof__(x) _x = (__typeof__(x)) x;     \
		__typeof__(x) _y = (__typeof__(x)) y;     \
		((_x % _y) ? (_x + _y) & ~(_y - 1) : _x); \
	})
