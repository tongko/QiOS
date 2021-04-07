#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace system { namespace interrupts {

void InterruptsEnable();

void InterruptsDisable();

bool InterruptsAreEnabled();

}}}	   // namespace qkrnl::system::interrupts