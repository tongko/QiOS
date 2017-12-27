/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __KBOOL_H_
#define __KBOOL_H_

#ifndef _Bool
#define _Bool unsigned int
#endif

#ifndef bool
#define bool _Bool
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true !false
#endif

#endif  //	__KBOOL_H_