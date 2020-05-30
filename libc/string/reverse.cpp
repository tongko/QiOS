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
//   reverse.cpp                                                              //
//   -  QiOS LibK reverse function implementation.                            //
//	 Apparently, LibC doesn't contain any function name reverse nor it has    //
//   any function does that.                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - reverse.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - reverse.cpp] requires __IS_KERNEL, define 1."
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

void reverse(char *str) {

	size_t len = strlen(str);
	char   aux;
	int	   i, j;

	for (i = len - 1, j = 0; i > j; i--, j++) {
		aux	   = str[i];
		str[i] = str[j];
		str[j] = aux;
	}
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End reverse.cpp                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif