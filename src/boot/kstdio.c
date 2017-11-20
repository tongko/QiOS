#include "kstdio.h"
#include "kstdint.h"

static char *itoa(uint32_t value, char *str, int base);
static void reverse(char *begin, char *end);
static void format_str(char *str, const char *format, va_list args);

_printf(const char *format, ...) {
	char buffer[MAXLEN] = {0};
	va_list arg;
	uint32_t i = 0;

	va_start(arg, format);
	_vsprintf(buffer, format, arg);
	va_end(arg);

	while (buffer[i] != '\0') {
		_putc(buffer[i++]);
	}
	_set_cursor(_cursor_x, _cursor_y);

	return i;
}

static char *itoa(uint32_t value, char *str, int base) {
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
	reverse(str, p - 1);

	return str;
}

static void reverse(char *begin, char *end) {
	char aux;
	while (end > begin) {
		aux = *end, *end-- = *begin, *begin++ = aux;
	}
}
