#ifndef __KERNEL_
#define __KERNEL_
#endif

#include "kstdlib.h"
#include "kstring.h"

uint32_t _atoi(const char *str) {
	uint32_t result = 0;

	for (int32_t i = 0; str[i] != '\0'; i++) {
		result = result * 10 + str[i] - 48;  // 48 == '0'
	}

	return result;
}

char *_itoa(uint32_t value, char *str, int32_t base) {
	//	Validate base
	if (base < 2 || base > 16) {
		*str = '\0';
		return str;
	}

	char *p = str;
	uint32_t quotient = value;

	//	Conversion. Number is reversed.
	do {
		const uint32_t tmp = quotient / base;
		*p++ = "0123456789ABCDEF"[quotient - (tmp * base)];
		quotient = tmp;
	} while (quotient);

	*p = '\0';
	_reverse(str);

	return str;
}