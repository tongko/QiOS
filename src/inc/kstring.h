#ifndef __KSTRING_H_
#define __KSTRING_H_

#ifdef __KERNEL_
#include "kstdint.h"
#define _NEED_SIZE_T
#define _NEED_NULL
#include "ktypedef.h"

char *_strcat(char *dest, const char *src);
int32_t _strcmp(const char *str1, const char *str2);
size_t _strlen(const char *str);

#endif  //	__KERNEL_
#endif  //	__KSTRING_H_