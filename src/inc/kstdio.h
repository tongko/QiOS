#ifndef		__STDIO_H_
#define		__STDIO_H_

#include <stdint.h>
#include "stdarg.h"

#define MAXLEN 1024

typedef	uint32_t	size_t;
typedef int32_t		ssize_t;

void _putc(uint8):
uint32_t _printf(const uint8_t*, ...);
uint32_t _vsprintf(uint8_t*, const uint8_t*, va_list);

#endif	//	__STDIO_H_