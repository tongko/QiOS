#include "kio.h"
#include "irq.h"
#include "time.h"

void init_timer(void) {		
	// Initialize Timer Interrupt
	install_handlers(20, timer_handler, 0);
	enable_irq(20);

	timer_handler(100);
}

void timer_handler(uint32_t hz) {
	uint16_t temp = (3579545L / 3) / hz;
		
	// Re-program the 8253 timer chip to run at 'HZ', instead of 18 Hz 
	outb(0x43, 0x36);		 // Channel 0, LSB/MSB, mode 3, binary
	outb(0x40, temp & 0xFF); // LSB
	outb(0x40, temp >> 8);	 // MSB
}

