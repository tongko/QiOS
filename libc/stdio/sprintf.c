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
//   sprintf.c                                                                //
//   -  QiOS LibK sprintf function implementations.                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - sprintf.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - sprintf.cpp] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

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

int sprintf(char *str, const char *format, ...) {

	va_list arg;
	va_start(arg, format);
	int ret = vsprintf(str, format, arg);
	va_end(arg);

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End sprintf.c                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif