/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __TYPES_H_
#define __TYPES_H_

#include <stddef.h>

typedef struct string {
	size_t length;
	char *value;
} string_t;

#endif  //	__TYPES_H_