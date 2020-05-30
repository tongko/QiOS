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
//   string.h                                                                 //
//   -  LibK for QiOS2.                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - string.h] requires x86_64 architecture. Please use the right compiler options."
#endif
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE REQUIRED HEADERS                                              //
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>

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

#ifndef ISDIGIT
	#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#endif

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE STRUCTURES                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE FUNCTION PROTOTYPES                                           //
////////////////////////////////////////////////////////////////////////////////

extern int	  memcmp(const void *str1, const void *str2, size_t n);
extern void * memcpy(void *dest, const void *src, size_t n);
extern void * memmove(void *dest, const void *src, size_t n);
extern void * memset(void *str, int c, size_t n);
extern void	  reverse(char *str);
extern size_t strlen(const char *str);
extern char * strcat(char *dest, const char *src);
extern char * strchr(const char *s, int c_in);
extern int	  strcmp(const char *str1, const char *str2);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End string.h                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
}	 // namespace std
#endif