#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

#ifdef _NEED_SIZE_T
typedef unsigned long size_t;
#endif  // _NEED_SIZE_T

#ifdef _NEED_NULL
#define NULL (void *)0
#endif  //	_NEED_NULL

#endif  /* __TYPEDEF_H_ */