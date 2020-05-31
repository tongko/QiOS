#include <stddef.h>
#include <stdint.h>
#include <system/MemLib.h>
#include <system/ccdefs>

namespace System {

size_t MemLength(uintptr_t ptr) {

	if (ptr == 0)
		return 0;

	byte * d = reinterpret_cast<byte *>(ptr);
	size_t l = 0;

	while (*d++ != 0 && ++l) {}

	return l;
}
}	 // namespace System
