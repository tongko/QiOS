#ifndef __KMEMORY_H
#define __KMEMORY_H

#include <stdint.h>
#include "ktypedef.h"

typedef struct tagMEMORY_MAP_ENTRY {
	uint32_t entry_start;
	uint32_t entry_end;
	size_t length;
	uint32_t type;
} MEMORY_MAP_ENTRY, *LPMEMORY_MAP_ENTRY;

typedef struct tagMEMORY_MAP {
	int32_t	entry_count;
	MEMORY_MAP_ENTRY entries[0];
} MEMORY_MAP, *LPMEMORY_MAP;

MEMORY_MAP mem_map;

#endif //	__KMEMORY_H