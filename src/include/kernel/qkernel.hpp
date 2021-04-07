#pragma once

#include <cdefs.h>
#include <kernel/Logger.hpp>
#include <kernel/types.hpp>

namespace qkrnl {

extern void KernelPanic(const char *fmt, ...);

}	 // namespace qkrnl

extern "C" void _kmain(__vma_t tBootParams);