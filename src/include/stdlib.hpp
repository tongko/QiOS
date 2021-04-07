#pragma once
/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <kernel/types.hpp>
#include <stddef.h>
#include <stdint.h>

namespace qklib {

extern "C" {
__attribute__((__noreturn__)) void abort(void);

int abs(int_t x);

#ifndef ISDIGIT
	#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif	  //	ISDIGIT

size_t	 itoa(uint32_t value, char *str, int base, const char *digit_str);
uint32_t atoi(const char *str);

#define MAX(a, b)                              \
	({                                         \
		__typeof__(a) _a = (__typeof__(a))(a); \
		__typeof__(a) _b = (__typeof__(a))(b); \
		_a > _b ? _a : _b;                     \
	})

#define MIN(a, b)                              \
	({                                         \
		__typeof__(a) _a = (__typeof__(a))(a); \
		__typeof__(a) _b = (__typeof__(a))(b); \
		_a < _b ? _a : _b;                     \
	})

dword_t min(dword_t a, dword_t b);
dword_t max(dword_t a, dword_t b);
}
}	 // namespace qklib