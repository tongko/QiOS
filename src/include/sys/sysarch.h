/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __SYS_SYSARCH_H_
#define __SYS_SYSARCH_H_

#ifdef __i386__
#undef _ELF_TARGET_ARCH
#define _ELF_TARGET_ARCH 3  // EM_386
//#elif (define )
#endif

#endif  //	__SYS_SYSARCH_H_