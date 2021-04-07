#include <cdefs.h>
#include <kernel/system/processor/Processor.hpp>
#include <kernel/types.hpp>

namespace qkrnl { namespace system { namespace processor {

dword_t ProcessorReadEFlags() {

	dword_t eflags;
	_ASMV_("pushf\n"
		   "pop [%0]"
		   : "=g"(eflags));

	return eflags;
}

}}}	   // namespace qkrnl::system::processor