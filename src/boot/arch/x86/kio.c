#include "boot.h"

// __inline__ uint8_t inb(uint16_t port)
// {
// 		uint8_t ret_val;
// 		__asm__ __volatile__("in %0,%1"	: "=a"(ret_val) : "d"(port));
// 		return ret_val;
// }

// __inline__ void outb(uint16_t port, uint8_t val)
// {
// 		__asm__ __volatile__("out %1,%0" :: "a"(val), "d"(port));
// }	

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
			temp = _inb(0x64);
			
			if(temp & 1)
            {
				_inb(0x60);
            }
		}
		while(temp & 2);

		// Reboot the computer...
		_outb(0x64, 0xFE);
}

//	io.c