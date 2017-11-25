#ifndef __KERNEL_
#define __KERNEL_
#endif

#include "kstring.h"

size_t _strlen(const char *str) {
	size_t i = 0;
	while (str[i++] != '\0') {
	}

	return i - 1;
}

char *_strcat(char *dest, const char *src) {
	char *p = dest;
	while (*(p++) != '\0') {
	}
	p--;

	while (*src != '\0') {
		*(p++) = *(src++);
	}

	*p = '\0';
	return dest;
}

int32_t _strcmp(const char *str1, const char *str2) {
	int32_t result = 0;

	for (; *str1 != '\0' && *str2 != '\0'; str1++, str2++) {
		if (*str1 == *str2) {
			continue;
		}

		if (*str1 > *str2) {
			return 1;
		}

		result = -1;
	}

	return result;
}

void *_memcpy(void *dest, const void *src, size_t n) {
	size_t i = 0;
	size_t t = 0;
	if (n % 4 == 0) {
		uint32_t *id = (uint32_t *)dest;
		uint32_t *is = (uint32_t *)src;
		t = n / 4;
		while (i < t) {
			*(id++) = *(is++);
		}
	} else if (n % 2 == 0) {
		uint16_t *sd = (uint16_t *)dest;
		uint16_t *ss = (uint16_t *)src;
		t = n / 2;
		while (i < t) {
			*(sd++) = *(ss++);
		}
	} else {
		char *d = (char *)dest;
		char *s = (char *)src;
		while (i < n) {
			*(d++) = *(s++);
		}
	}

	return dest;
}

void _reverse(char *str) {
	size_t len = _strlen(str);
	char aux;
	int32_t i, j;

	for (i = len - 1, j = 0; i > j; i--, j++) {
		aux = str[i];
		str[i] = str[j];
		str[j] = aux;
	}
}