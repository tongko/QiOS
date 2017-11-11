// #include "kio.h"
// #include "irq.h"

// void init_pics(void) {
// 	// ICW1
// 	outportb(0x20, 0x11); 
// 	outportb(0xA0, 0x11);
// 	// ICW2 : re-maps pics
// 	outportb(0x21, 0x20);  // Irqs 0-07 = ints 20h-27h 
// 	outportb(0xA1, 0x28);  // Irqs 8-15 = ints 28h-2Fh
// 	// ICW3
// 	outportb(0x21, 0x04); 
// 	outportb(0xA1, 0x02);
// 	// ICW4
// 	outportb(0x21, 0x01); 
// 	outportb(0xA1, 0x01);
// 	// Disable all exceptions/interrupts
// 	outportb(0x21, 0xFF);
// 	outportb(0xA1, 0xFF); 	
// }

// void init_irq(void) {
// 	// Initiate Re-map of Exceptions/Interrupts
// 	init_pics();
	
// 	// Install Exception Handlers
// 	load_exceptions();
	
// 	// Point the IDT register to our IDT array
// 	load_idt();

// 	// Enable Interrupts   	    
// 	sti();	
// }

// void load_exceptions(void) {		
// 	install_handlers(0,  fault_handler, 0);	  // __isr00
// 	install_handlers(1,  fault_handler, 0);	  // __isr01
// 	install_handlers(2,  fault_handler, 0);   // __isr02
// 	install_handlers(3,  fault_handler, 0);	  // __isr03
// 	install_handlers(4,  fault_handler, 0);	  // __isr04
// 	install_handlers(5,  fault_handler, 0);   // __isr05		
// 	install_handlers(6,  fault_handler, 0);	  // __isr06
// 	install_handlers(7,  fault_handler, 0);   // __isr07
// 	install_handlers(8,  fault_handler, 0);	  // __isr08
// 	install_handlers(9,  fault_handler, 0);   // __isr09 
// 	install_handlers(10, fault_handler, 0);   // __isr10
// 	install_handlers(11, fault_handler, 0);   // __isr11
// 	install_handlers(12, fault_handler, 0);   // __isr12
// 	install_handlers(13, fault_handler, 0);   // __isr13 
// 	install_handlers(14, fault_handler, 0);   // __isr14
// 	install_handlers(15, fault_handler, 0);   // __isr15
// 	install_handlers(16, fault_handler, 0);   // __isr16
// 	install_handlers(17, fault_handler, 0);   // __isr17
// }

// idtr IDTR;
// x86_int IDT[256]; // = (x86_interrupt*)0xD00;

// void load_idt() {
// 	IDTR.limit  = 256*(sizeof(x86_int)-1);
// 	IDTR.base   = IDT;

// 	idtr *IDTRptr = &IDTR;

// 	// Load IDTR with lidt 
// 	__asm__ __volatile__("LIDT (%0) ": :"p" (IDTRptr));
// }

// void install_handlers(uint32_t number, void (*handler)(), uint32_t dpl) {
// 		uint16_t selector = 0x08;
// 		uint16_t settings;
// 		uint32_t offset = (uint32_t)handler;

// 		// Get CS selector 
// 		__asm__ __volatile__("movw %%cs,%0" :"=g"(selector));

// 		// Set settings options depending on dpl 
// 		switch(dpl) {
// 			case 0:
// 				settings = INT_0; 
// 				break;
// 			case 1:
// 			case 2:
// 			case 3:
// 				settings = INT_3;
// 				break;
// 		}
	 
// 		// Set actual values of int 
// 		IDT[number].low_offset   = (offset & 0xFFFF);
// 		IDT[number].selector     = selector;
// 		IDT[number].settings     = settings;
// 		IDT[number].high_offset  = (offset >> 16);
// }

// // Sends command to PIC to mask an IRQ
// void enable_irq(unsigned short irq_number) {
// 	irq_mask &= ~(1 << irq_number);
// 	if(irq_number >= 8) {
// 		irq_mask &= ~(1 << 2);
// 	}

// 	outb(0x21, irq_mask & 0xFF);
// 	outb(0xA1, (irq_mask >> 8) & 0xFF);
// }

// // Sends command to PIC to unmask an IRQ 
// void disable_irq(unsigned short irq_number) {
// 	irq_mask |= (1 << irq_number);
// 	if((irq_mask & 0xFF00)==0xFF00) {
// 		irq_mask |= (1 << 2);
// 	}

// 	outportb(0x21, irq_mask & 0xFF);
// 	outportb(0xA1, (irq_mask >> 8) & 0xFF);	
// }