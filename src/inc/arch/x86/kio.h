#ifndef		__ARCH_X86_KIO_H_
#define		__ARCH_X86_KIO_H_

// inportb: Get a byte from the selected port
uint32_t inb(uint32_t port);

// outportb: Send the selected byte to the selected port
void outb(uint32_t port, uint32_t val);

// write_nmi: Write Non Maskable Interrupt
//void write_nmi(uint8_t nmi);

// enable_nmi: Enable Non Maskable Interrupt
//void enable_nmi(void);

// sti: Enable Interrupts:
void sti();

// cli: Disable Interrupts:
void cli();

// hlt: Halt the CPU:
void hlt();

// idle: Freeze the Computer:
void idle();

// reboot: Reboot the Computer:
void reboot();

#endif	//	__ARCH_X86_KIO_H_