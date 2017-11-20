#ifndef __STDIO_H_
#define __STDIO_H_

#ifdef __KERNEL_
#include "kstdarg.h"
#include "kstdint.h"

#define MAXLEN 1024

typedef uint32_t size_t;

// void _set_color(unsigned char fg, unsigned char bg);
// void _clear(void);
// void _putc(uchar) : uint32_t _printf(const unsigned char*, ...);

uint32_t _vsprintf(char *str, const char *format, va_list arg);
uint32_t _printf(const char *format, ...);

#endif  //	__KERNEL_
#endif  //	__STDIO_H_