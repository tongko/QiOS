// https://en.wikipedia.org/wiki/Serial_port
#include "serial_port.h"
#include "kio.h"
#include "kstdarg.h"
#include "kstdio.h"

#define SERIAL_COM1						0x3F8		/* COM1 base port */
/* The I/O ports */
/* All the I/O ports are calculated relative to the data port. This is because
*  all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
*  order, but they start at different values.
*/
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* Port commands */
/* SERIAL_LINE_ENABLE_DLAB:
*  Tells the serial port to expect first the highest 8 bits on the data port,
*  then the lowest 8 bits will follow
*/
#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
*   Sets the speed of the data being sent. The default speed of a serial
*   port is 115200 bits/s. The argument is a divisor of that number, hence
*   the resulting speed becomes (115200 / divisor) bits/s.
*
*   @param com      The COM port to configure
*   @param divisor  The divisor
*/
static void serial_configure_baud_rate(uint16_t com, uint16_t divisor)
{
    _outb(SERIAL_LINE_COMMAND_PORT(com),
         SERIAL_LINE_ENABLE_DLAB);
    _outb(SERIAL_DATA_PORT(com),
         (divisor >> 8) & 0x00FF);
    _outb(SERIAL_DATA_PORT(com),
         divisor & 0x00FF);
}

/** serial_configure_line:
*   Configures the line of the given serial port. The port is set to have a
*   data length of 8 bits, no parity bits, one stop bit and break control
*   disabled.
*
*   @param com  The serial port to configure
*/
static void serial_configure_line(uint16_t com)
{
	/* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
	*  Content: | d | b | prty  | s | dl  |
	*  Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
	*/
	_outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

static void serial_configure_fifo(uint16_t com)
{
    /* Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
    *  Content: | lvl | bs | r | dma | clt | clr | e |
    *  Value:   | 11  | 0  | 0 | 0   | 1   | 1   | 1 |
    */
    _outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

static void serial_configure_modem(uint16_t com)
{
    /* Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
    *  Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
    *  Value:   | 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1   |
    */
    _outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void init_serial_port() {
    serial_configure_baud_rate(SERIAL_COM1, 2);
    serial_configure_line(SERIAL_COM1);
    serial_configure_fifo(SERIAL_COM1);
    serial_configure_modem(SERIAL_COM1);
}

/** serial_is_transmit_fifo_empty:
*   Checks whether the transmit FIFO queue is empty or not for the given COM
*   port.
*
*   @param  com The COM port
*   @return 0 if the transmit FIFO queue is not empty
*           1 if the transmit FIFO queue is empty
*/
static int serial_is_transmit_fifo_empty(uint16_t com)
{
    /* 0x20 = 0010 0000 */
    return _inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_putc(char b) {
    // Block until buffer is not full
    while (!serial_is_transmit_fifo_empty(SERIAL_COM1)) {}

    _outb(SERIAL_DATA_PORT(SERIAL_COM1), b);
}

void serial_write_bytes(char *c, int32_t n) {
    for (int32_t i = 0; i < n; i++) {
        serial_putc(c[i]);
    }
}

void serial_print(const char *format, ...) {
	char buffer[MAXLEN] = {0};
	va_list arg;
	int32_t i = 0;

	va_start(arg, format);
	_vsprintf(buffer, format, arg);
	va_end(arg);

	while (buffer[i] != '\0') {
		serial_putc(buffer[i++]);
	}
}