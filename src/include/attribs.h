/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __ATTRIBS_H_
#define __ATTRIBS_H_

#ifndef __sect
#define __sect(S) __attribute__((section(#S)))
#endif

#define __early __sect(.early)
#define __earlydata __sect(.earlydata)

#ifndef __align
#define __align(x) __attribute__((aligned(x)))
#endif

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#ifndef __cdecl
#define __attribute__(cdecl)
#endif

#endif  //	__ATTRIBS_H_