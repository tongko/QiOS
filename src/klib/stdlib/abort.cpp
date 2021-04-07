/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
/*******************************************************************************
	Function	- abort
	Description	- abort the program execution and comes out directly from the
				  place of the call.
	Declaration	- void abort(void)
	Parameters	-
		NA

	Returns		- This function does not returns any value.
*******************************************************************************/
#include <stdio.hpp>
#include <stdlib.hpp>

namespace qklib {

__attribute__((__noreturn__)) void abort(void) {
#if defined(__is_libk)
	// TODO: Add proper kernel panic.
	printf("kernel: panic: abort()\n");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	printf("abort()\n");
#endif
	while (1) {}
	__builtin_unreachable();
}
}	 // namespace qklib
