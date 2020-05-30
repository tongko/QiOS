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
//   strlen.c                                                                 //
//   -  Implementation of LicK strlen function                                //
//   Computes the length of the string str up to, but not including the       //
//   terminating null character.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - strlen.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - strlen.cpp] requires __IS_KERNEL, define 1."
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

size_t strlen(const char *str) {
	size_t i;
	for (i = 0; *str++ != '\0'; i++) {}

	return i;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End strlen.c                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
}	 // namespace std
#endif