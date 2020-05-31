#include <system/MemLib.h>
#include <system/ccdefs>

namespace System {

uintptr_t SetMemory(uintptr_t dst, byte c, size_t len) {

	byte *d = reinterpret_cast<byte *>(dst);

	while (len > 0 && len % 64) {
		*d++ = c;
		len--;
	}

	if (len) {
		uint64_t *pD = reinterpret_cast<uint64_t *>(d);
		uint64_t  lc = (uint64_t) c;
		uint64_t  ch = lc | lc << 8 | lc << 16 | lc << 24 | lc << 32 | lc << 40
					  | lc << 48 | lc << 56;

		len /= 64;
		while (len--) {
			*pD++ = ch;
		}
	}

	return dst;
}

uintptr_t SetMemoryWord(uintptr_t dst, uint16_t word, size_t len) {

	uint16_t *d = reinterpret_cast<uint16_t *>(dst);
	while (len > 0 && len % 32) {
		*d++ = word;
		len--;
	}

	if (len) {
		uint64_t *pD	= reinterpret_cast<uint64_t *>(d);
		uint64_t  qword = (uint64_t) word;
		uint64_t  ch	= qword | qword << 16 | qword << 32 | qword << 48;

		len = (len * 2) / 64;
		while (len--) {
			*pD++ = ch;
		}
	}

	return dst;
}

}	 // namespace System
