#include <kernel/portio.hpp>
#include <kernel/system/SerialPort.hpp>
#include <mutex.hpp>

using namespace qklib;

namespace qkrnl { namespace system {

#define MAXLEN							4096
#define SERIAL_DATA_PORT(base)			(word_t)((int_t) base)
#define SERIAL_FIFO_COMMAND_PORT(base)	(word_t)((int_t) base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)	(word_t)((int_t) base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (word_t)((int_t) base + 4)
#define SERIAL_LINE_STATUS_PORT(base)	(word_t)((int_t) base + 5)

static const byte_t SERIAL_LINE_ENABLE_DLAB {0x80};
static Mutex s_mutex;	 // This mutex is shared among instances of SerailPorts

static void InitPort(SerialPort &sp) {

	ComPorts com = sp.GetPort();
	byte_t	 divisor = (byte_t) sp.GetBaudRate();

	outportb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
	//	Set HI byte of divisor
	outportb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0xFF);
	//	Set LO byte of divisor
	outportb(SERIAL_DATA_PORT(com), divisor & 0xFF);
	// 8 bits, no parity, one stop bit
	outportb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
	// Enable FIFO, clear them, with 14-byte threshold
	outportb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
	// IRQs enabled, RTS/DSR set
	outportb(SERIAL_MODEM_COMMAND_PORT(com), 0x0B);
}

SerialPort::SerialPort(ComPorts port, BaudRates rate)
	: m_Port(port)
	, m_Rate(rate) {

	InitPort(*this);
}

bool SerialPort::QueueIsEmpty() {

	return (inportb(SERIAL_LINE_STATUS_PORT(m_Port)) & 0x20);
}

bool SerialPort::DataReceived() {

	return (inportb(SERIAL_LINE_STATUS_PORT(m_Port)) & 0x01);
}

ComPorts SerialPort::GetPort() {

	return m_Port;
}

void SerialPort::SetPort(ComPorts port) {

	m_Port = port;
	InitPort(*this);
}

BaudRates SerialPort::GetBaudRate() {

	return m_Rate;
}

void SerialPort::SetBaudRate(BaudRates rate) {

	m_Rate = rate;
	InitPort(*this);
}

size_t SerialPort::Write(const byte_t *buff, size_t len) {

	s_mutex.Acquire();

	for (size_t i = 0; i < len; i++) {
		while (!QueueIsEmpty()) {}	  // Block until buffer is not full

		outportb(SERIAL_DATA_PORT(m_Port), buff[i]);
	}

	s_mutex.Release();
	return len;
}

size_t SerialPort::Read(byte_t *buff, size_t len) {

	s_mutex.Acquire();

	for (size_t i = 0; i < len; i++) {
		while (!DataReceived()) {}	  //	Block until buffer received

		buff[i] = inportb(SERIAL_DATA_PORT(m_Port));
	}

	s_mutex.Release();
	return len;
}

}}	  // namespace qkrnl::system