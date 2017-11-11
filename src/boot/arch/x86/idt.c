#include "boot.h"

void idt_init(void) {
	idt[0] = {.lower_func_ptr = (uint32_t)exception0 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception0 >> 16) & 0xFFFF};
	idt[1] = {.lower_func_ptr = (uint32_t)exception1 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception1 >> 16) & 0xFFFF};
	idt[2] = {.lower_func_ptr = (uint32_t)exception2 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception2 >> 16) & 0xFFFF};
	idt[3] = {.lower_func_ptr = (uint32_t)exception3 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception3 >> 16) & 0xFFFF};
	idt[4] = {.lower_func_ptr = (uint32_t)exception4 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception4 >> 16) & 0xFFFF};
	idt[5] = {.lower_func_ptr = (uint32_t)exception5 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception5 >> 16) & 0xFFFF};
	idt[6] = {.lower_func_ptr = (uint32_t)exception6 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception6 >> 16) & 0xFFFF};
	idt[7] = {.lower_func_ptr = (uint32_t)exception7 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception7 >> 16) & 0xFFFF};
	idt[8] = {.lower_func_ptr = (uint32_t)exception8 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception8 >> 16) & 0xFFFF};
	idt[9] = {.lower_func_ptr = (uint32_t)exception9 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception9 >> 16) & 0xFFFF};
	idt[10] = {.lower_func_ptr = (uint32_t)exception10 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception10 >> 16) & 0xFFFF};
	idt[11] = {.lower_func_ptr = (uint32_t)exception11 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception11 >> 16) & 0xFFFF};
	idt[12] = {.lower_func_ptr = (uint32_t)exception12 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception12 >> 16) & 0xFFFF};
	idt[13] = {.lower_func_ptr = (uint32_t)exception13 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception13 >> 16) & 0xFFFF};
	idt[14] = {.lower_func_ptr = (uint32_t)exception14 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception14 >> 16) & 0xFFFF};
	idt[15] = {.lower_func_ptr = (uint32_t)exception15 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception15 >> 16) & 0xFFFF};
	idt[16] = {.lower_func_ptr = (uint32_t)exception16 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception16 >> 16) & 0xFFFF};
	idt[17] = {.lower_func_ptr = (uint32_t)exception17 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception17 >> 16) & 0xFFFF};
	idt[18] = {.lower_func_ptr = (uint32_t)exception18 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception18 >> 16) & 0xFFFF};
	idt[19] = {.lower_func_ptr = (uint32_t)exception19 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception19 >> 16) & 0xFFFF};
	idt[20] = {.lower_func_ptr = (uint32_t)exception20 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception20 >> 16) & 0xFFFF};
	idt[21] = {.lower_func_ptr = (uint32_t)exception21 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception21 >> 16) & 0xFFFF};
	idt[22] = {.lower_func_ptr = (uint32_t)exception22 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception22 >> 16) & 0xFFFF};
	idt[23] = {.lower_func_ptr = (uint32_t)exception23 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception23 >> 16) & 0xFFFF};
	idt[24] = {.lower_func_ptr = (uint32_t)exception24 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception24 >> 16) & 0xFFFF};
	idt[25] = {.lower_func_ptr = (uint32_t)exception25 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception25 >> 16) & 0xFFFF};
	idt[26] = {.lower_func_ptr = (uint32_t)exception26 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception26 >> 16) & 0xFFFF};
	idt[27] = {.lower_func_ptr = (uint32_t)exception27 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception27 >> 16) & 0xFFFF};
	idt[28] = {.lower_func_ptr = (uint32_t)exception28 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception28 >> 16) & 0xFFFF};
	idt[29] = {.lower_func_ptr = (uint32_t)exception29 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception29 >> 16) & 0xFFFF};
	idt[30] = {.lower_func_ptr = (uint32_t)exception30 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception30 >> 16) & 0xFFFF};
	idt[31] = {.lower_func_ptr = (uint32_t)exception31 &0xFFFF, .cs_offset = 0x8, .reserved = 0,
				.flags = 0x8E, .upper_func_ptr = ((uint32_t) exception31 >> 16) & 0xFFFF};
}