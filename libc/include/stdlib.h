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
//   stdlib.h                                                                 //
//   -  QiOS2 LibK stdlib interface.                                          //
//   The stdlib.h header defines four variable types, several macros, and     //
//   various functions for performing general functions.                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - stdlib.h] requires x86_64 architecture. Please use the right compiler options."
#endif
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE REQUIRED HEADERS                                              //
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>

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
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE STRUCTURES                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE FUNCTION PROTOTYPES                                           //
////////////////////////////////////////////////////////////////////////////////

extern void	  abort(void);
extern int	  abs(int x);
extern int	  atoi(const char *str);
extern size_t itoa(int value, char *str, int base, const char *digit_str);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End stdlib.h //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif