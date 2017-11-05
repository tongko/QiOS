/* Surely you will remove the processor conditionals and this comment
appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#include "typedef.h"
#include "idt.h"

extern uint32_t PAGE_TABLE[1024 * 1024];		/* # of entries/page table * total # of page tables 
													actual size = 4194304 bytes = 4MiB, represents 4GiB in physical memory (size of unsigned int = 4 bytes)
													ie. each 4 byte entry represent 4 KiB in physical memory */
extern uint32_t PAGE_DIRECTORY[1024 * 1];  	// # of pages tables * # of directory (4096 bytes = 4 KiB)
extern uint32_t IDT_CONTENTS;
extern uint32_t IDT_POINTER;

#define KERNEL_VIRTUAL_BASE			0xC0000000					// Constant declaring base of Higher-half kernel (from Kernel.asm)
#define KERNEL_PAGE_TABLE			(KERNEL_VIRTUAL_BASE >> 22)	// Constant declaring Page Table index in virtual memory (from Kernel.asm)
#define DISPLAY_SIZE				2000						// 2000 = 80 x 25 Characters - VGA Text-mode Display size

/* For debugging purpose */
void outb(unsigned short port, unsigned char value) 
{
	__asm__
	(
		"mov al, [ebp+12]\n"
		"mov dx, [ebp+8]\n"
		"outb dx, al\n"
	);
}

void clearScreen(uint8_t bgColor, uint8_t fgColor) {
	uint8_t color = ((bgColor << 4) & 0xF0) | (fgColor & 0x0F);
	uint16_t* lpVideoBuffer = (uint16_t*) 0xB8000;
	for (uint32_t i = 0; i < DISPLAY_SIZE; i++) {
		lpVideoBuffer[i] = ((uint16_t) color << 8) | 0x00;
	}
}

void shiftDisplayUp() {
	__asm__
	(
		"mov edi, 0xB8000\n"		// origin of video mem
		"mov esi, 0xB80A0\n"		//	2nd line
		"mov ecx, 0x780\n"			//	VGA_WIDTH * 24
		"rep movsw\n"
	);
}

void print(const char* text) {
	// pos = y * 80 + x;
	// x = pos % 80;
	// y = pos / 80;
	static uint16_t currentPosition;
	uint16_t* lpVideoBuffer = (uint16_t*) 0xB8000;

	for (int i = 0; text[i] > 0; i++) {
		if (text[i] == '\n') {
			currentPosition += 80 - (currentPosition % 80);
			if (currentPosition >= DISPLAY_SIZE) {
				shiftDisplayUp();
				currentPosition = DISPLAY_SIZE - 80;
			}
		}
		else if (text[i] == '\t') {
			for (int space = 0; space < 4; space++) {
				lpVideoBuffer[currentPosition] = (uint16_t) (0x0200 | ' ');
				if (currentPosition >= DISPLAY_SIZE) {
					shiftDisplayUp();
					currentPosition = DISPLAY_SIZE - 80;
				}
			}
		}
		else if (text[i] == '\b' && currentPosition > 0) {
			lpVideoBuffer[--currentPosition] = (uint16_t) (0x200 | ' ');
		}
		else {
			lpVideoBuffer[currentPosition++] = (uint16_t) (0x200 | text[i]);
			if (currentPosition >= DISPLAY_SIZE) {
				shiftDisplayUp();
				currentPosition = DISPLAY_SIZE - 80;
			}
		}

		outb(0x3D4, 14);
		outb(0x3D5, (uint8_t)(currentPosition >> 8));
		outb(0x3D4, 15);
		outb(0x3D5, (uint8_t)(currentPosition));
	}
}

//void CommonInterruptHandler(int handlerNum) {
//}

void _kernel_main(void) {
	// DisplayColour and Delay methods cannot be used until virtual addressing is set up i.e. until PG bit of CR0 is on.
	// Use inline assembly colour method for debugging.
	// This is because until virtual addressing is set up, code is only accessible through physical addresses but method calls 
	// are based on virtual addresses (because of the link script).
	
	uint32_t	physicalAddressAndFlags = 7;			// ; 0b111 - Setting Page Table flags (Present: ON, Read/Write: ON, User/Supervisor: ON)
	uint32_t	numberOfPageTables = 4;					// 4 is arbitrary to cover 16MiB
	uint32_t	entriesPerPageTable = 1024;				// There is always 1024 pages (4KiB/Page)
	uint32_t	startPageTableEntryIndex = 0;
	uint32_t	sizeOfPageTables = numberOfPageTables * entriesPerPageTable;
	uint32_t	index = 0;
	uint32_t*	lpPageTable1Physical = (uint32_t*)((uint32_t)PAGE_TABLE - KERNEL_VIRTUAL_BASE);
	uint32_t*	lpPageDirectoryPhysical = (uint32_t*)((uint32_t)PAGE_DIRECTORY - KERNEL_VIRTUAL_BASE);
	
	//Setting up identity mapping
	for	(index = 0; index < (sizeOfPageTables + startPageTableEntryIndex); index++) {
		lpPageTable1Physical[index] = physicalAddressAndFlags;
		physicalAddressAndFlags += 4096;
	}
	
	physicalAddressAndFlags = 7;
	index = (KERNEL_PAGE_TABLE * 1024);
	startPageTableEntryIndex = (KERNEL_PAGE_TABLE * 1024);
	
	for (index = KERNEL_PAGE_TABLE * 1024; index < (sizeOfPageTables + startPageTableEntryIndex); index++) {
		lpPageTable1Physical[index] = physicalAddressAndFlags;
		physicalAddressAndFlags += 4096;
	}
	
	physicalAddressAndFlags = (uint32_t)&lpPageTable1Physical[0];
	physicalAddressAndFlags = physicalAddressAndFlags | 7;	//0b111 - Setting Page Table flags (Present: ON, Read/Write: ON, User/Supervisor: ON)

	uint32_t entriesOfPageDirectory = 1024;
	index = 0; 	// Setting index to 0 to be used with Page Directory
	
	for (index = 0; index < entriesOfPageDirectory; index++) {
		lpPageDirectoryPhysical[index] = physicalAddressAndFlags;
		physicalAddressAndFlags += 4096;
	}

	// Set virtual addressing via control register CR3 
	// high 20 bits is Page directory Base Register i.e physical address of first page directory entry
	__asm__ 
	(
		"lea ecx, [Page_Directory - 0xC0000000]\n" // 0xC0000000 = KERNEL_VIRTUAL_BASE
		"mov cr3, ecx\n"
	);	
	
	// Switch on paging via control register CR0
	__asm__ 
	(
		"mov ecx, cr0\n"
		"or ecx, 0x80000000\n"	// Set PG bit in CR0 to enable paging.
		"mov cr0, ecx\n"
	);

	// At only this point we are in physical higher-half mode
	// Jump to higher-half
	__asm__
	(
		"lea ecx, [startInHigherHalf]\n"
		"jmp ECX\n"
		"StartInHigherHalf:\n"
		"nop\n"
	);

	clearScreen(0x3, 0xF);	// DisplayColour can also be used for debugging
	; // END - Set Virtual Memory

	// BEGIN - Interrupt Descriptor Table setup
	IDT_Entry_t* IDT_Ptr = (IDT_Entry_t*)IDT_CONTENTS;

	int i = 0;
	for (i = 0; i < 256; i++) {
		uint32_t handlerPtr = (uint32_t)&CommonInterruptHandler(i);

		IDT_Ptr[i].HandlerPtr_Low = (uint16_t) (handlerPtr & 0x0000FFFF);
		IDT_Ptr[i].HandlerPtr_High = (uint16_t) ((handlerPtr >> 16) & 0x0000FFFF);
		IDT_Ptr[i].Selector = 0x8;
		IDT_Ptr[i].Reserved = 0x0;
		IDT_Ptr[i].GateType = 0xE;
		IDT_Ptr[i].StorageSegment = 0x0;
		IDT_Ptr[i].DPL = 0x0;
		IDT_Ptr[i].Present = 0x1;
	}

	IDT_Pointer_t* IDT_Ptr_Ptr = (IDT_Pointer_t*)IDT_POINTER;
	IDT_Ptr_Ptr->TableSize = (uint16_t)((256 * sizeof(IDT_Entry_t)) - 1);
	IDT_Ptr_Ptr->IDT_Ptr = IDT_Ptr;
	
	__asm__
	(
		"lea eax, [IDT_POINTER]\n"  // Load address of (/pointer to) IDT pointer structure
		"lidt [eax]\n"						// Load IDT register with pointer to IDT pointer structure
	);
	
	// END - Interrupt descriptor Table setup
	
	// BEGIN - PIC setup and remap
	
	// Remap IRQs 0-7    to    ISRs 32-39
	// and   IRQs 8-15   to    ISRs 40-47
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	
	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);

	STI
		
	// END - PIC setup and remap
	// BEGIN - Configure PIT (The Timer)
	CLI
	
	__asm__
	(
		"in al, 0x21\n"
		"and al, 0xfc\n" // Also enables keyboard interrupt (IRQ 1)
		"out 0x21, al\n"
	);
	
	outb(0x43, 0x34);
	outb(0x40, 0xFF);
	outb(0x40, 0xFF);
	
	STI
	// END - Configure PIT (The Timer)
	
	while (1)
	{
    	;		
	}	
}