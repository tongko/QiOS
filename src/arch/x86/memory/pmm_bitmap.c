/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <kstr.h>
#include <mem/pmm.h>
#include <stdbool.h>

static uint32_t __earlydata _pmm_mem_size;
static uint32_t __earlydata _pmm_used_blocks;
static uint32_t __earlydata _pmm_max_blocks;
static uint32_t __earlydata *_pmm_mem_map;

static __inline__ void __early mem_set(uint32_t bit) {
	_pmm_mem_map[bit / 32] |= (1 << (bit % 32));
}

static __inline__ void __early mem_unset(uint32_t bit) {
	_pmm_mem_map[bit / 32] &= ~(1 << (bit % 32));
}

static __inline__ bool __early mem_test(uint32_t bit) {
	return _pmm_mem_map[bit / 32] & (1 << (bit % 32));
}

static __inline__ uint32_t __early get_free_block_count() {
	return _pmm_max_blocks - _pmm_used_blocks;
}

static uint32_t __early get_free_block() {
	for (uint32_t i = 0; i < get_block_count() / 32; i++) {
		if (_pmm_mem_map[i] != 0xFFFFFFFF) {
			for (uint32_t j = 0; j < 32; j++) {
				int bit = 1 << j;
				if (!(_pmm_mem_map[i] & (1 << j))) {
					return i * 4 * 8 + j;
				}
			}
		}
	}

	return -1;
}

static uint32_t __early get_free_blocks(size_t size) {
	if (size == 0) {
		return -1;
	}

	if (size == 1) {
		return get_free_block();
	}
	size_t remain = size;
	uint32_t base;
	for (uint32_t i = 0; i < get_block_count() / 32; i++) {
		if (_pmm_mem_map[i] != 0xFFFFFFFF) {
			for (uint32_t j = 0; j < 32; j++) {
				int bit = 1 << j;
				if (!(_pmm_mem_map[i] & bit)) {
					uint32_t start = i * 32 + j;
					size_t remain = size;
					for (size_t count = 0; count <= size; count++) {
						if (!mem_test(start + count)) {
							remain--;
						} else {
							i = (start + count) / 32;
							j = 32;
							break;  // not enough space in these 32 bit region, skip to the next free region
						}

						if (!remain) {
							return start;
						}
					}
				}
			}
		}
	}

	return -1;
}

uint32_t pmm_get_used_block() {
	return _pmm_used_blocks;
}

uint32_t pmm_get_block_count() {
	return _pmm_max_blocks;
}

void pmm_init(size_t mem_size, paddr_t bitmap) {
	_pmm_mem_size = mem_size;
	_pmm_mem_map = (uint32_t *)bitmap;
	_pmm_max_blocks = _pmm_mem_size * 1024 / PMM_BLOCK_SIZE;
	_pmm_used_blocks = _pmm_max_blocks;

	//	By default all memory are in used.
	memset(_pmm_mem_map, 0xf, _pmm_max_blocks / PMM_BLOCKS_PER_BYTE);
}

void pmm_init_region(paddr_t base, size_t size, bool set) {
	uint32_t align = base / PMM_BLOCK_SIZE;
	uint32_t blocks = size / PMM_BLOCK_SIZE;

	while (blocks-- > 0) {
		if (set) {
			mem_unset(align++);
			_pmm_used_blocks--;
		} else {
			mem_set(align++);
			_pmm_used_blocks++;
		}
	}

	mem_set(0);  // first block is always set.
}

void *pmm_alloc_block(void) {
	if (get_free_block_count() <= 0) {
		return 0;  // Out of Memory
	}

	uint32_t frame = get_free_block();
	if (frame == -1) {
		return 0;  //	Out of Memory
	}

	mem_set(frame);

	paddr_t addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks++;

	return (void *)addr;
}

void *pmm_alloc_blocks(size_t size) {
	if (get_free_block_count() < size) {
		return 0;  //	Out of Memory
	}

	uint32_t frame = get_free_blocks(size);
	if (frame == -1) {
		return 0;  //	Out of Memory
	}

	for (uint32_t i = 0; i < size; i++) {
		mem_set(frame + i);
	}

	paddr_t addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks += size;

	return (void *)addr;
}

void pmm_free_block(void *p) {
	if (!p) {
		return;
	}

	paddr_t addr = (paddr_t)p;
	uint32_t frame = addr / PMM_BLOCK_SIZE;

	mem_unset(frame);

	_pmm_used_blocks--;
}

void pmm_free_blocks(void *p, size_t size) {
	paddr_t addr = (paddr_t)p;
	uint32_t frame = addr / PMM_BLOCK_SIZE;

	for (size_t i = 0; i < size; i++) {
		mem_unset(frame + i);
	}

	_pmm_used_blocks -= size;
}