#include "boot.h"

uint32_t strlen(const int32_t* str) {
	char *p = str;
	uint32_t i = 0;
	while p[i++] != '\0') {
	}

	return i;
}

unsigned char* strcat(char *dest, const char *src) {
	for (; *dest != '\0'; dest++) {
	}

	while (*src != '\0') {
		*(dest++) = *(src++);
		*dest = '\0';
	}

	return dest;
}