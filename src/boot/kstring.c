#include "boot.h"

size_t _strlen(const char *str) {
	size_t i = 0;
	while (str[i++] != '\0') {
	}

	return i - 1;
}

char *_strcat(char *dest, const char *src) {
	while (*(dest++) != '\0') {
	}

	while (*src != '\0') {
		*(dest++) = *(src++);
		*dest = '\0';
	}

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
}