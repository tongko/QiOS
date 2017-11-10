#include "boot.h"

uint32_t strlen(const uint32_t* str) {
	uint8_t* p = str;
	uint32_t i = 0;
	while p[i++] != '\0') {
	}

	return i;
}

uint8_t* strcat(uint8_t* dest, const uint8_t* src) {
	for (; *dest != '\0'; dest++) {
	}

	while (*src != '\0') {
		*(dest++) = *(src++);
		*dest = '\0';
	}

	return dest;
}