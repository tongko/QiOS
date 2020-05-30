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
//   memset.cpp                                                               //
//   -  QiOS LibK memset function implementation.                             //
//   Copies the character c (an unsigned char) to the first n characters of   //
//   the string pointed to, by the argument str.                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - memset.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - memset.cpp] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

void *memset(void *str, int c, size_t n) {

	unsigned char *s  = (unsigned char *) str;
	unsigned char  ch = c;

	while (n-- > 0) {
		*s++ = ch;
	}

	return str;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End memset.cpp                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}	 // namespace std
}
#endif