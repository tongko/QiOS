/*== QiOS =====================================================================+
|                                                                              |
|    Author: Liew Tze Wei                                                      |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   stdio.h                                                                  //
//   -  Provide minimum libc                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - stdio.h] requires x86_64 architecture. Please use the right compiler options."
#endif
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE REQUIRED HEADERS                                              //
////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//    STRUCTURE PROTOTYPE                                                     //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE DEFINITIONS/ENUMERATIONS/SIMPLE TYPEDEFS                      //
////////////////////////////////////////////////////////////////////////////////

#ifndef EOF
	#define EOF (-1)
#endif

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE STRUCTURES                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE FUNCTION PROTOTYPES                                           //
////////////////////////////////////////////////////////////////////////////////

extern int printf(const char *__restrict format, ...);
extern int sprintf(char *__restrict str, const char *__restrict format, ...);
extern int vsprintf(char *__restrict str,
                    const char *__restrict format,
                    va_list arg);
extern int putchar(int);
extern int puts(const char *);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End stdio.h                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif