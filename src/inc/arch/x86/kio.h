#ifndef __ARCH_X86_KIO_H_
#define __ARCH_X86_KIO_H_

#include <stdint.h>

// inportb: Get a byte from the selected port
uint8_t _inb(uint16_t port);

// outportb: Send the selected byte to the selected port
void _outb(uint16_t port, uint8_t val);

// write_nmi: Write Non Maskable Interrupt
//void write_nmi(unsigned char nmi);

// enable_nmi: Enable Non Maskable Interrupt
//void enable_nmi(void);

// sti: Enable Interrupts:
void _sti();

// cli: Disable Interrupts:
void _cli();

// hlt: Halt the CPU:
void _hlt();

// idle: Freeze the Computer:
void _idle();

// reboot: Reboot the Computer:
void _reboot();

#endif  //	__ARCH_X86_KIO_H_