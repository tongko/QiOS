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
//   memcpy.cpp                                                               //
//   -  QiOS2 LibK memcpy function implementation.                            //
//   Copies n characters from memory area src to memory area dest.            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - memcpy.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - memcpy.cpp] requires __IS_KERNEL, define 1."
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
//    PRIVATE DEFINES AND TYPES //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS //
////////////////////////////////////////////////////////////////////////////////

void *memcpy(void *dest, const void *src, size_t n) {

	size_t i = 0;
	size_t t = 0;

	if (n % 4 == 0) {
		unsigned int *id = (unsigned int *) dest;
		unsigned int *is = (unsigned int *) src;

		t = n / 4;
		while (i < t) {
			*(id++) = *(is++);
		}
	}
	else if (n % 2 == 0) {
		unsigned short *sd = (unsigned short *) dest;
		unsigned short *ss = (unsigned short *) src;

		t = n / 2;
		while (i < t) {
			*(sd++) = *(ss++);
		}
	}
	else {
		char *d = (char *) dest;
		char *s = (char *) src;
		while (i < n) {
			*(d++) = *(s++);
		}
	}

	return dest;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End memcpy.cpp //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
}	 // namespace std
#endif