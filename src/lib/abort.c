/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <kio.h>
#include <klib.h>

__attribute__((__noreturn__)) void abort() {
	printf("kenel: panic: abort()\n");

	while (1) {
	}
	__builtin_unreachable();
}