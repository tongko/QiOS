// #ifndef __ARCH_X86_BOOT_H_
// #define __ARCH_X86_BOOT_H_

// #include <stdint.h>

// typedef struct tagKERNEL_MEMORY_DESCRIPTOR {
//   uint32_t kernel_virtual_start;
//   uint32_t kernel_virtual_end;
//   uint32_t kernel_physical_start;
//   uint32_t kernel_physical_end;
// } KERNEL_MEMORY_DESCRIPTOR;

// #define MILLISECONDS_PER_TICK 10
// #define freq 100

// // //	Defines a GDT entry. Use packed, because it prevents the compiler from doing things that it thinks
// // //	is best: Prevent compiler "optimization" by packing
// // typedef struct _gdt_entry {
// // 	uint16_t limit_low;
// // 	uint16_t base_low;
// // 	unsigned char base_middle;
// // 	unsigned char access;
// // 	unsigned char granularity;
// // 	unsigned char base_high;
// // } __attribute__((packed)) gdt_entry;

// // //	Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this
// // //	NEEDS to be packed
// // typedef struct _gdt_ptr
// // {
// // 	uint16_t limit;
// // 	uint32_t base;
// // } __attribute__((packed)) gdt_ptr;

// //	Defines an IDT entry.
// // typedef struct _idt_entry {
// // 	uint16_t	lower_func_ptr;
// // 	uint16_t	cs_offset;
// // 	unsigned char		reserved;
// // 	unsigned char		flags;
// // 	uint16_t	upper_func_ptr;
// // } __attribute__((packed)) idt_entry;

// // //	Our GDT, with 3 entries, and finally our special GDT pointer
// // gdt_entry gdt[6];
// // gdt_ptr gp;
// //idt_entry idt[256];

// ////	This will be a function in multiboot.asm. We use this to properly reload the new segment registers
// //extern void gdt_flush(gdt_ptr* ptr);
// ////	Setup a descriptor in the GDT.
// //void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, unsigned char access, unsigned char gran);
// ////	Initialize GDT.
// //void gdt_init(void);

// //	Initialize IDT.
// void idt_init(void);

// // // inb: Get a byte from the selected port
// // uint8_t inb(uint16_t port);
// // // outb: Send the selected byte to the selected port
// // void outb(uint16_t port, uint8_t val);
// // write_nmi: Write Non Maskable Interrupt
// //void write_nmi(unsigned char nmi);
// // enable_nmi: Enable Non Maskable Interrupt
// //void enable_nmi(void);
// // sti: Enable Interrupts:
// void sti();
// // cli: Disable Interrupts:
// void cli();
// // hlt: Halt the CPU:
// void hlt();
// // idle: Freeze the Computer:
// void idle();
// // reboot: Reboot the Computer:
// void reboot();

// //	Initialize timer
// void init_timer(void);
// //	Timer handler
// void timer_handler(uint32_t hz);
// //	Delay
// void delay(uint32_t milliseconds);

// //	Print multiboot info
// void print_multiboot_info(uint32_t mbi_addr);

// #endif  //	__ARCH_X86_BOOT_H_