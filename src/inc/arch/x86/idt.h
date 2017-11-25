#ifndef		__IDT_H_
#define		__IDT_H_

#include <stdint.h>

#define STI \
__asm__\
( \
	"sti\n"\
	"nop\n"\
);

#define CLI \
__asm__\
( \
	"cli\n"\
	"nop\n"\
);

// a pointer to the interrupt descriptor table
// passed by reference to the LIDT instruction
struct tagIDT_DESCRIPTION {
  uint16_t Limit; // in bytes
  uint32_t Base;
} __attribute__((packed)) IDT_DESCRIPTION;

// See http://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_IA-32
struct tagINTERRUPT_DESCRIPTOR {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_high;
} __attribute__((packed));

void init_idt();
void pic_acknowledge();
void init_pic();

#endif