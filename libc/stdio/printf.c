/*== QiOS =====================================================================+
|                                                                              |
|    Author: Liew Tze Wei                                                      |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   printf.c                                                                 //
//   -  printf function Implementation                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - printf.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////

#ifndef MAXLEN
	#define MAXLEN 4096
#endif

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

int printf(const char *restrict format, ...) {
	char buffer[MAXLEN] = {0};

	va_list arg;
	va_start(arg, format);
	uint32_t ret = vsprintf(buffer, format, arg);
	va_end(arg);

	uint32_t i = 0;
	while (buffer[i] != '\0') {
		putchar(buffer[i]);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End printf.c                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
