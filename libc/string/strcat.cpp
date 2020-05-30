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
//   strcat.cpp                                                               //
//   -  QiOS2 LibK strcat function implementations.                           //
//   Appends the string pointed to by src to the end of the string pointed    //
//   to by dest up to n characters long.                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - strcat.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - strcat.cpp] requires __IS_KERNEL, define 1."
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

char *strcat(char *dest, const char *src) {

	char *p = dest;
	while (*(p++)) {}
	p--;

	while (*src != '\0') {
		*(p++) = *(src++);
	}

	*p = '\0';
	return dest;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End strcat.cpp                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif