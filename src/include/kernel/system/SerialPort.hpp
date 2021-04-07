#pragma once

#include <common.hpp>
#include <kernel/types.hpp>

namespace qkrnl { namespace system {

enum class ComPorts { COM1 = 0x3F8, COM2 = 0x2F8, COM3 = 0x3E8, COM4 = 0x2E8 };

enum class BaudRates {
	BR115200 = 1,
	BR57600 = 2,
	BR38400 = 3,
	BR19200 = 4,
	BR9600 = 5
};

class SerialPort {

public:	   //	Constructors
	SerialPort(ComPorts, BaudRates);
	~SerialPort() {};

public:	   //	Attributes
	PROPERTY_DECL(ComPorts, Port);
	PROPERTY_DECL(BaudRates, BaudRate);

public:	   //	Operations
	size_t Read(byte_t *, size_t);
	size_t Write(const byte_t *, size_t);

private:	//	Operations
	__attribute__((always_inline)) inline bool QueueIsEmpty();
	__attribute__((always_inline)) inline bool DataReceived();

private:
	ComPorts  m_Port;
	BaudRates m_Rate;
};
}}	  // namespace qkrnl::system