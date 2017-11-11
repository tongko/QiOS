#include "boot.h"

__inline__ uint32_t inb(uint32_t port)
{
		uint32_t ret_val;
		__asm__ __volatile__("inb %w1,%b0"	: "=a"(ret_val) : "d"(port));
		uint32_t ret_val;
}

__inline__ void outb(uint32_t port, uint32_t val)
{
		__asm__ __volatile__("outb %b0,%w1" :: "a"(val), "d"(port));
}	

__inline__ void sti()
{
		__asm__("sti");	  // Enable ints
}

__inline__ void cli()
{
		__asm__("cli");	  // Disable ints
}

__inline__ void hlt()
{
		__asm__("hlt");   // Halt the CPU
}	

__inline__ void idle()
{
		cli();
		hlt();
}

__inline__ void reboot()
{
		int temp;		
				
		// Get ready for reboot...flush the keyboard controller
		do
		{
			temp = inb(0x64);
			
			if(temp & 1)
            {
				inb(0x60);
            }
		}
		while(temp & 2);

		// Reboot the computer...
		outb(0x64, 0xFE);
}

//	io.c