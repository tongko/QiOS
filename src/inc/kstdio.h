#ifndef __STDIO_H_
#define __STDIO_H_

#include <stdint.h>
#include "kstdarg.h"
#define	_NEED_SIZE_T
#include "ktypedef.h"

#ifndef MAXLEN
#define MAXLEN 1024
#endif

uint32_t _vsprintf(char *str, const char *format, va_list arg);

#endif  //	__STDIO_H_