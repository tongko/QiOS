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

size_t strlen(const char *str) {
	size_t i;
	for (i = 0; *(str++) != '\0'; str++, i++) {
	}

	return i;
}

// string_t *makestr(string_t *str1, const char *str2) {
// 	if (!str1) {
// 		return NULL;
// 	}

// 	str1->length = strlen(str2);
// 	str1->value = str2;

// 	return str1;
// }

// string_t *_strcat(string_t *dest, const string_t *src) {
// 	char *p = dest->value;
// 	while (*p) {
// 		p++;
// 	}

// 	char *s = src->value;
// 	while (*s) {
// 		*(p++) = *(s++);
// 	}

// 	*p = '\0';
// 	return dest;
// }

char *strcat(char *dest, const char *src) {
	char *p = dest;
	while (*(p++)) {
	}
	p--;

	while (*src != '\0') {
		*(p++) = *(src++);
	}

	*p = '\0';
	return dest;
}

// int _strcmp(const string_t *str1, const string_t *str2) {
// 	int result = 0;
// 	char *s1, *s2;

// 	for (; *s1 && *s2; s1++, s2++) {
// 		if (*s1 == *s2) {
// 			continue;
// 		}

// 		if (*s1 > *s2) {
// 			return 1;
// 		}

// 		result = -1;
// 	}

// 	return result;
// }

int strcmp(const char *str1, const char *str2) {
	int result = 0;

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
		unsigned int *id = (unsigned int *)dest;
		unsigned int *is = (unsigned int *)src;
		t = n / 4;
		while (i < t) {
			*(id++) = *(is++);
		}
	} else if (n % 2 == 0) {
		unsigned short *sd = (unsigned short *)dest;
		unsigned short *ss = (unsigned short *)src;
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

// void _reverse(string_t *str) {
// 	size_t len = str->value;
// 	char *s = str->value;
// 	char *e = str->value[len - 1];
// 	char aux;

// 	for (; s > e; e--, s++) {
// 		aux = *s;
// 		*s = *e;
// 		*e = aux;
// 	}
// }

void reverse(char *str) {
	size_t len = _strlen(str);
	char aux;
	int i, j;

	for (i = len - 1, j = 0; i > j; i--, j++) {
		aux = str[i];
		str[i] = str[j];
		str[j] = aux;
	}
}

char *strchr(const char *s, int c_in) {
	const char *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, magic_bits, charmask;
	char c;

	c = (unsigned char)c_in;

	/* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
	for (char_ptr = (const char *)s;
	     ((unsigned long int)char_ptr & (sizeof(longword) - 1)) != 0;
	     ++char_ptr)
		if (*char_ptr == c)
			return (void *)char_ptr;
		else if (*char_ptr == '\0')
			return NULL;

	/* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

	longword_ptr = (unsigned long int *)char_ptr;

	/* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
	magic_bits = -1;
	magic_bits = magic_bits / 0xff * 0xfe << 1 >> 1 | 1;

	/* Set up a longword, each of whose bytes is C.  */
	charmask = c | (c << 8);
	charmask |= charmask << 16;
	if (sizeof(longword) > 4)
		/* Do the shift in two steps to avoid a warning if long has 32 bits.  */
		charmask |= (charmask << 16) << 16;
	if (sizeof(longword) > 8)
		return NULL;

	/* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
	for (;;) {
		/* We tentatively exit the loop if adding MAGIC_BITS to
         LONGWORD fails to change any of the hole bits of LONGWORD.

         1) Is this safe?  Will it catch all the zero bytes?
         Suppose there is a byte with all zeros.  Any carry bits
         propagating from its left will fall into the hole at its
         least significant bit and stop.  Since there will be no
         carry from its most significant bit, the LSB of the
         byte to the left will be unchanged, and the zero will be
         detected.

         2) Is this worthwhile?  Will it ignore everything except
         zero bytes?  Suppose every byte of LONGWORD has a bit set
         somewhere.  There will be a carry into bit 8.  If bit 8
         is set, this will carry into bit 16.  If bit 8 is clear,
         one of bits 9-15 must be set, so there will be a carry
         into bit 16.  Similarly, there will be a carry into bit
         24.  If one of bits 24-30 is set, there will be a carry
         into bit 31, so all of the hole bits will be changed.

         The one misfire occurs when bits 24-30 are clear and bit
         31 is set; in this case, the hole at bit 31 is not
         changed.  If we had access to the processor carry flag,
         we could close this loophole by putting the fourth hole
         at bit 32!

         So it ignores everything except 128's, when they're aligned
         properly.

         3) But wait!  Aren't we looking for C as well as zero?
         Good point.  So what we do is XOR LONGWORD with a longword,
         each of whose bytes is C.  This turns each byte that is C
         into a zero.  */

		longword = *longword_ptr++;

		/* Add MAGIC_BITS to LONGWORD.  */
		if ((((longword + magic_bits)

		      /* Set those bits that were unchanged by the addition.  */
		      ^ ~longword)

		     /* Look at only the hole bits.  If any of the hole bits
              are unchanged, most likely one of the bytes was a
              zero.  */
		     & ~magic_bits) != 0 ||

		    /* That caught zeroes.  Now test for C.  */
		    ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask)) & ~magic_bits) != 0) {
			/* Which of the bytes was C or zero?
             If none of them were, it was a misfire; continue the search.  */

			const char *cp = (const char *)(longword_ptr - 1);

			if (*cp == c)
				return cp;
			else if (*cp == '\0')
				return NULL;
			if (*++cp == c)
				return cp;
			else if (*cp == '\0')
				return NULL;
			if (*++cp == c)
				return cp;
			else if (*cp == '\0')
				return NULL;
			if (*++cp == c)
				return cp;
			else if (*cp == '\0')
				return NULL;
			if (sizeof(longword) > 4) {
				if (*++cp == c)
					return cp;
				else if (*cp == '\0')
					return NULL;
				if (*++cp == c)
					return cp;
				else if (*cp == '\0')
					return NULL;
				if (*++cp == c)
					return cp;
				else if (*cp == '\0')
					return NULL;
				if (*++cp == c)
					return cp;
				else if (*cp == '\0')
					return NULL;
			}
		}
	}

	return NULL;
}