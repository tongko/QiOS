/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __VMM_H_
#define __VMM_H_

#include <stdint.h>

//	Kernel mode allocate new memory

//	Initialize Virtual Memory Manager
void vmm_init();

#endif  //	__VMM_H_