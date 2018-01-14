#include <hello_world.h>
//#include <stdint.h>

static const char buff[] = "Hello World!";

static __inline__ unsigned short vga_entry(char c, const unsigned char color) {
	return (unsigned short)c | ((unsigned short)color << 8);
}

void say_hello() {
	unsigned short *video = (unsigned short *)0xB8000;

	for (int i = 0; i < 12; i++) {
		*video++ = vga_entry(buff[i], 7);
	}
}

// void main() {
// 	say_hello();
// }