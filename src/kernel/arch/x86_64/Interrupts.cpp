#include <cdefs.h>
#include <kernel/system/interrupts/interrupts.hpp>
#include <kernel/system/processor/Processor.hpp>

namespace qkrnl { namespace system { namespace interrupts {

void InterruptsEnable() {

	_ASM_("sti");
}

void InterruptsDisable() {

	_ASM_("cli");
}

bool InterruptsAreEnabled() {

	dword_t eflags = processor::ProcessorReadEFlags();
	return eflags & (1 << 9);
}

}}}	   // namespace qkrnl::system::interrupts