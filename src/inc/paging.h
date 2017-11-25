#ifndef __PAGING_H_
#define __PAGING_H_

#include <stdbool.h>
#include <stdint.h>
#include "multiboot2.h"

#define PAGE_DIRECTORY_OFFSET_BITS 10
#define PAGE_TABLE_OFFSET_BITS 10
#define PAGE_OFFSET_BITS 12

#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_DWORDS 1024

#define KERNEL_PAGE_TABLE_NUMBER 768

#define BITMAP_SIZE 4096

#define UPPER_GB_START (void *)0xC0000000
#define TMP_PAGE_0 (void *)0xFFBFB000
#define TMP_PAGE_1 (void *)0xFFBFC000
#define TMP_PAGE_2 (void *)0xFFBFD000
#define TMP_PAGE_3 (void *)0xFFBFE000
#define TMP_PAGE_4 (void *)0xFFBFF000
#define FIRST_PAGE_TABLE_ADDRESS (void *)0xFFC00000  // 4 MB from end
#define PAGE_DIRECTORY_ADDRESS (void *)0xFFFFF000    // 4 KB from end

// #define BITMAP_SIZE 32768
// Enough room for 512 MB of RAM
// TODO: Find a more efficient way to initialize page allocator
//static const int32_t BITMAP_SIZE = 4096;

struct kmemory_descriptor {
	uint32_t kernel_virtual_start;
	uint32_t kernel_virtual_end;
	uint32_t kernel_physical_start;
	uint32_t kernel_physical_end;
};

typedef uint32_t *page_directory_t;
typedef uint32_t *page_table_t;

enum page_permissions_t { READ_ONLY,
	                      READ_WRITE };
enum page_privilege_t { SUPERVISOR,
	                    USER };
enum page_size_t { FOUR_KB,
	               FOUR_MB };

// void map_kernel_into_page_directory(page_directory_t new_pd);

// page_directory_t initialize_kernel_page_directory();

uint32_t init_page_allocator(struct kmemory_descriptor kernel_memory);

uint32_t make_page_directory_entry(void *page_table_physical_address,
                                   enum page_size_t page_size,
                                   bool cache_disabled,
                                   bool write_through,
                                   enum page_privilege_t privelage,
                                   enum page_permissions_t permissions,
                                   bool present);

uint32_t make_page_table_entry(void *page_frame_address,
                               bool global,
                               bool cache_disabled,
                               bool write_through,
                               enum page_privilege_t privelage,
                               enum page_permissions_t permissions,
                               bool present);

// uint32_t num_present_pages(page_directory_t pd);

// uint32_t page_directory_offset(const void* virtual_address);

void page_in(const void *virtual_address);

// uint32_t page_table_offset(const void* virtual_address);

// void print_page_table(FILE file, const uint32_t *);

void *virtual_to_physical(const void *addr);

uint32_t physical_to_virtual(uint32_t addr);

#endif  //	__PAGING_H_