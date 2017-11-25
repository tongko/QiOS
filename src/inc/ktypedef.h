#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

#ifdef _NEED_SIZE_T
typedef unsigned long size_t;
#undef _NEED_SIZE_T
#endif  // _NEED_SIZE_T

#ifdef _NEED_NULL
#define NULL (void *)0
#undef _NEED_NULL
#endif  //	_NEED_NULL

#endif /* __TYPEDEF_H_ */