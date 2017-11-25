#include "kstdio.h"
#include "kstdlib.h"
#include "kstring.h"

uint32_t _vsprintf(char *str, const char *format, va_list arg) {
	char *p = str;
	char *s;
	uint32_t d;
	char c;
	size_t i = 0;

	*p = '\0';
	//	loop through the string until NULL terminator
	while (*format) {
		if (*format != '%') {
			//	Normal character
			p[i++] = *format;
			p[i] = '\0';
		} else {
			switch (*(format + 1)) {
				case 'c':  // Char
					c = (char)va_arg(arg, int);
					p[i++] = c;
					p[i] = '\0';
					break;
				case 's':  // String
					s = va_arg(arg, char *);
					_strcat(p, s);
					i = _strlen(p);
					break;
				case 'u':  // Unsigned integer
				case 'd':  // Integer
				case 'i':
					d = va_arg(arg, int);
					char buff1[10] = {0};  // decimal max 10 digits.
					_strcat(p, _itoa(d, buff1, 10));
					i = _strlen(p);
					break;
				case 'x':  // Hexadecimal number
					d = va_arg(arg, uint32_t);
					char buff2[8] = {0};
					// Check for leading 0
					int32_t j = 3;
					char ibuff[10] = {0};
					int32_t leading = 0;
					if (*(format + 2) == '.') {
						//	Read all digits and make it number of leading zeros
						while (*(format + j) >= '0' && *(format + j) <= '9') {
							ibuff[j - 3] = *(format + j);
							j++;
						}
						//	the . is meant for printing not formating if j == 2
						if (j > 3) {
							format += j - 2;
							leading = _atoi(ibuff);
						}
					}
					_itoa(d, buff2, 16);
					if (leading > 0) {
						char buff3[leading];
						size_t bufflen = _strlen(buff2);
						int32_t excess = leading - bufflen;
						for (int32_t j = 0; j < leading; j++) {
							if (j < excess) {
								buff3[j] = '0';
							} else {
								buff3[j] = buff2[j - excess];
								buff3[j + 1] = '\0';
							}
						}
						_strcat(p, buff3);
					} else {
						_strcat(p, buff2);
					}
					i = _strlen(p);
					break;
				default:
					// Print the % and the following character
					p[i++] = *format;
					p[i++] = *(format + 1);
					p[i] = '\0';
					break;
			}

			format++;
		}

		format++;
	}

	return i - 1;
}