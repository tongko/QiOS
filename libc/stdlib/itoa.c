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
//   itoa.c                                                                   //
//   -  QiOS LibK itoa function implementation.                               //
//   itoa is non-standard extension to the standard C programming language.   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - itoa.c] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - itoa.c] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
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

size_t itoa(int value, char *str, int base, const char *digit_str) {

	//	Validate base
	if (base < 2 || base > 16) {
		*str = '\0';
		return 0;
	}

	char *	 p		  = str;
	int quotient = value;
	size_t	 sz		  = 0;

	//	Conversion. Number is reversed.
	do {
		const int tmp = quotient / base;
		*p++			   = digit_str[quotient - (tmp * base)];
		quotient		   = tmp;
		sz++;
	} while (quotient);

	*p = '\0';
	reverse(str);

	return sz;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End itoa.c                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}	 // namespace std
}
#endif
