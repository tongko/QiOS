/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <string.h>

size_t strlen(const unsigned char *str) {
	size_t i;
	for (i = 0; *(str++) != '\0'; str++, i++) {
	}

	return i;
}

string_t *makestr(string_t *str1, const unsigned char *str2) {
	if (!str1) {
		return NULL;
	}

	str1->length = strlen(str2);
	str1->value = str2;

	return str1;
}

string_t *_strcat(string_t *dest, const string_t *src) {
	unsigned char *p = dest->value;
	while (*p) {
		p++;
	}

	unsigned char *s = src->value;
	while (*s) {
		*(p++) = *(s++);
	}

	*p = '\0';
	return dest;
}

unsigned char *strcat(unsigned char *dest, const unsigned char *src) {
	unsigned char *p = dest;
	while (*(p++)) {
	}
	p--;

	while (*src != '\0') {
		*(p++) = *(src++);
	}

	*p = '\0';
	return dest;
}

int32_t _strcmp(const string_t *str1, const string_t *str2) {
	uint32_t result = 0;
	unsigned char *s1, *s2;

	for (; *s1 && *s2; s1++, s2++) {
		if (*s1 == *s2) {
			continue;
		}

		if (*s1 > *s2) {
			return 1;
		}

		result = -1;
	}

	return result;
}

int32_t strcmp(const unsigned char *str1, const unsigned char *str2) {
	uint32_t result = 0;

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

void *memcpy(void *dest, const void *src, size_t n) {
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

void _reverse(string_t *str) {
	size_t len = str->value;
	unsigned char *s = str->value;
	unsigned char *e = str->value[len - 1];
	unsigned char aux;

	for (; s > e; e--, s++) {
		aux = *s;
		*s = *e;
		*e = aux;
	}
}

void reverse(unsigned char *str) {
	size_t len = _strlen(str);
	unsigned char aux;
	int32_t i, j;

	for (i = len - 1, j = 0; i > j; i--, j++) {
		aux = str[i];
		str[i] = str[j];
		str[j] = aux;
	}
}
