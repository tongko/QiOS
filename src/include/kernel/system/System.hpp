#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace system {

class System {

public:	   //	Operations
	static void InitBootStrapProcessor(__pma_t);
};

}}	  // namespace qkrnl::system