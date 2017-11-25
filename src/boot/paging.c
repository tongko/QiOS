#include "paging.h"
#include "multiboot.h"
#include "multiboot_info.h"

multiboot_tag_mmap_t *get_mmap_table(void);
uint32_t free_count;
uint32_t page_bitmap[BITMAP_SIZE];

enum page_status_t { FREE,
	                 USED };
static void mark_page(uint32_t page_number, enum page_status_t status) {
	uint32_t index = page_number >> 5;
	uint32_t bit = page_number & 0b11111;
	uint32_t value = page_bitmap[index];
	uint32_t flags_bit = 1 << bit;

	if (status == FREE) {
		//	check previous page status
		if ((value & flags_bit) != FREE) {
			free_count++;
		}

		value &= ~flags_bit;  // set the flags bit to FREE (0)
	} else {
		if ((value & flags_bit) == FREE) {
			free_count--;
		}

		value |= flags_bit;  //	set the flags bit to USED (1)
	}

	page_bitmap[index] = value;
}

static __inline__ uint32_t page_directory_offset(const void *vir_addr) {
	return ((uint32_t)vir_addr) >> PAGE_OFFSET_BITS >> PAGE_TABLE_OFFSET_BITS;
}

static __inline__ uint32_t page_table_offset(const void *vir_addr) {
	return (((uint32_t)vir_addr) >> PAGE_OFFSET_BITS) & 0x3FF;
}

static __inline__ uint32_t page_offset(const void *vir_addr) {
	return ((uint32_t)vir_addr & 0x00000FFF);
}

__inline__ uint32_t physical_to_virtual(uint32_t phys_addr) {
	return phys_addr + (uint32_t)&KERNEL_VIRTUAL_BASE;
}

static __inline__ bool get_present_from_pte(uint32_t pte) {
	return (pte & 0b1);
}

static __inline__ bool get_present_from_pde(uint32_t pde) {
	return (pde & 0b1);
}

// Assumes we are using recursive page tables.
// Last entry in page directory points to itself
static void *page_table_virtual_address(uint16_t page_table_number) {
	// First 10 bits are set to 1
	uint32_t virtual_address = 0xFFC00000;
	// Next 10 bits index into page directory
	virtual_address |= (page_table_number << PAGE_OFFSET_BITS);
	return (void *)virtual_address;
}

static uint32_t round_up_to_nearest_page_start(uint32_t addr) {
	if ((addr & 0xFFFFF000) != addr) {
		addr &= 0xFFFFF000;
		addr += 0x00001000;
	}

	return addr;
}

static __inline__ uint32_t round_down_to_nearest_page_start(uint32_t addr) {
	return addr & 0xFFFFF000;
}

static __inline__ uint32_t page_number(uint32_t addr) {
	return addr >> PAGE_OFFSET_BITS;
}

// 1 page = 1024 * 4 bytes = 4 kB
// 1 page table = 1024 pages = 4 mB
// 1 page directory = 1024 page tables = 4 gB

// Find a free physical page and return it's physical address
// This does NOT zero out the page
void *allocate_physical_page() {
	for (uint32_t index = 0; index < BITMAP_SIZE; index++) {
		if (page_bitmap[index] != 0) {
			// There is at least one free page in this chunk
			for (uint8_t bit = 0; bit < 32; bit++) {
				if ((page_bitmap[index] & (1 << bit)) != 0) {
					uint32_t page_number = index * 32 + bit;
					mark_page(page_number, USED);
					void *page_start = (void *)(page_number << PAGE_OFFSET_BITS);
					return page_start;
				}
			}
		}
	}

	// Out of physical memory
	// TODO: Evict a page
	return 0;
}

uint32_t make_page_table_entry(void *page_frame_address,
                               bool global,
                               bool cache_disabled,
                               bool write_through,
                               enum page_privilege_t privelage,
                               enum page_permissions_t permissions,
                               bool present) {
	uint32_t entry = (uint32_t)page_frame_address;
	entry |= global << 8;
	entry |= cache_disabled << 6;
	entry |= write_through << 3;
	entry |= privelage << 2;
	entry |= permissions << 1;
	entry |= present;

	return entry;
}

uint32_t make_page_directory_entry(void *page_table_physical_address,
                                   enum page_size_t page_size,
                                   bool cache_disabled,
                                   bool write_through,
                                   enum page_privilege_t privelage,
                                   enum page_permissions_t permissions,
                                   bool present) {
	uint32_t entry = (uint32_t)page_table_physical_address;
	entry |= page_size << 7;
	entry |= cache_disabled << 4;
	entry |= write_through << 3;
	entry |= privelage << 2;
	entry |= permissions << 1;
	entry |= present;

	return entry;
}

void page_in(const void *vir_addr) {
	uint32_t pd_offset = page_directory_offset(vir_addr);
	page_directory_t pd = (page_directory_t)PAGE_DIRECTORY_ADDRESS;
	uint32_t pde = pd[pd_offset];

	if (!get_present_from_pde(pde)) {
		// Allocate a physical page to hold the page table
		void *page_table_physical_address = allocate_physical_page();

		pde = make_page_directory_entry(
		    page_table_physical_address,
		    FOUR_KB,
		    false,
		    false,
		    SUPERVISOR,
		    READ_WRITE,
		    true);
		pd[pd_offset] = pde;
	}

	page_table_t pt = (page_table_t)page_table_virtual_address(pd_offset);
	uint32_t pt_offset = page_table_offset(vir_addr);
	uint32_t pte = pt[pt_offset];

	if (!get_present_from_pte(pte)) {
		// Allocate a physical page to hold the virtual page
		void *page_physical_address = allocate_physical_page();

		pte = make_page_table_entry(
		    page_physical_address,
		    false,
		    false,
		    false,
		    SUPERVISOR,
		    READ_WRITE,
		    true);
		pt[pt_offset] = pte;
	}
}

void *virtual_to_physical(const void *vir_addr) {
	uint32_t pd_offset = page_directory_offset(vir_addr);
	uint32_t pt_offset = page_table_offset(vir_addr);
	uint32_t p_offset = page_offset(vir_addr);

	uint32_t *page_table_addr = (uint32_t *)(FIRST_PAGE_TABLE_ADDRESS + (PAGE_SIZE_BYTES * pd_offset));
	uint32_t page_table_entry = page_table_addr[pt_offset];
	uint32_t physical_address = ((page_table_entry >> 12) << 12) | p_offset;
	return (void *)physical_address;
}

uint32_t init_page_allocator(struct kmemory_descriptor kmem) {
	multiboot_tag_mmap_t *mmap = get_mmap_table();

	// Mark physical pages as free according to memory map passed by GRUB
	multiboot_memory_map_t *entry;
	for (entry = mmap->entries;
	     (multiboot_uint8_t *)entry < (multiboot_uint8_t *)mmap + mmap->size;
	     entry = (multiboot_memory_map_t *)((unsigned long)entry + mmap->entry_size)) {
		// for (uint32_t i = 0; i < num_entries; i++) {
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
			// Available
			uint32_t first_addr = (uint32_t)entry->addr & 0xffffffff;
			uint32_t one_past_last_addr = first_addr + ((uint32_t)entry->len & 0xffffffff);
			uint32_t first_full_page = page_number(round_up_to_nearest_page_start(first_addr));
			uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(one_past_last_addr));

			for (uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
				if (i > PAGE_SIZE_DWORDS) {  // First 4MB are where the kernel lives
					mark_page(i, FREE);
				}
			}
		} else {
			// Unavailable
			// Not currently keeping track of these regions
		}
	}

	// Mark physical pages as unavailable where kernel code is loaded
	uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kmem.kernel_physical_start));
	uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kmem.kernel_physical_end));

	for (uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
		mark_page(i, USED);
	}

	return free_count;
}
