/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __STDDEF_H_
#define __STDDEF_H_

/* This is the signed integral type and is the result of subtracting two
 * pointers. */
#ifndef _PTRDIFF_T_DECLARED
#define _PTRDIFF_T_DECLARED
#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ long int
#endif
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#endif /* _PTRDIFF_T_DECLARED */

/* This is the unsigned integral type and is the result of the sizeof keyword. */
#ifndef __SIZE_T_DECLARED
#define __SIZE_T_DECLARED
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ long unsigned int
#endif
typedef __SIZE_TYPE__ size_t;
#endif

/* This is an integral type of the size of a wide character constant. */
#ifndef __WCHAR_TYPE__
#define __WCHAR_TYPE__ int
#endif
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

/* This macro is the value of a null pointer constant.  */
#ifndef __cplusplus
#define NULL ((void *)0)
#else /* C++ */
#define NULL 0
#endif /* C++ */

/*
This results in a constant integer of type size_t which is the offset
in bytes of a structure member from the beginning of the structure.
The member is given by member-designator, and the name of the structure
is given in type.
*/
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#endif //	__STDDEF_H_