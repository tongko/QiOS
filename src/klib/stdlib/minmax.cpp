/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
#include <cdefs.h>
#include <stdlib.hpp>

namespace qklib {

dword_t min(dword_t a, dword_t b) {

	return a < b ? a : b;
}

dword_t max(dword_t a, dword_t b) {

	return a > b ? a : b;
}
}	 // namespace qklib