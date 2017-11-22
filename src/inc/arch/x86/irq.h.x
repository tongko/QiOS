// #ifndef		__ARCH_X86_IRQ_H_
// #define		__ARCH_X86_IRQ_H_

// #include <stdint.h>

// // Interrupt types
// #define INT_0 0x8E00                         // 1000111000000000 = present,ring0,int_gate
// #define INT_3 0xEE00                         // 1110111000000000 = present,ring3,int_gate

// // Structure for an interrupt 
// typedef struct
// {
// 	word low_offset;                         // Low nibble of offset to handler of interrupt 
// 	word selector;                           // GDT selector used 
// 	word settings;                           // Settings for int 
// 	word high_offset;                        // High nibble to handler code 
// }__attribute__ ((packed)) x86_int;

// // Structure for the IDTR 
// typedef struct
// {
//      word limit;                             // Limit or Size of IDT 
//      x86_interrupt *base;                    // A pointer to the base of the IDT 
// }__attribute__ ((packed)) idtr;

// void init_pics(void);
// void init_irq(void);
// void load_exceptions(void);
// void enable_irq(unsigned short irq_number);
// void disable_irq(unsigned short irq_number);

// #endif	//	__ARCH_X86_IRQ_H_