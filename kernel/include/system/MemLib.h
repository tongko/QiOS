#pragma once

#include <stddef.h>
#include <stdint.h>
#include <system/ccdefs>

namespace System {

// Copy memory from @P1 to @P2 for @P3 bytes.
uintptr_t CopyMemory(uintptr_t, uintptr_t, size_t);

// Set memory at @P1 to value @P2 for @P3 bytes.
uintptr_t SetMemory(uintptr_t, byte, size_t);
uintptr_t SetMemoryWord(uintptr_t, uint16_t, size_t);

// Mesure length of memory in bytes.
size_t MemLength(uintptr_t);

}	 // namespace System