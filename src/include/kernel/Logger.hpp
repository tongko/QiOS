#pragma once

#include <common.hpp>
#include <kernel/types.hpp>

namespace qkrnl {

namespace system {
//	Prototypes
class SerialPort;
}	 // namespace system

class Logger {

public:	   //	Constructors
	Logger();
	~Logger();

public:	   //	Attributes
	PROPERTY_DECL(bool, SerialPortEnabled);
	PROPERTY_DECL(bool, ConsoleEnabled);

public:	   //	Operations
	void PrintWithLock(const char *, ...) __attribute__((nothrow));
	void Print(const char *, ...) __attribute__((nothrow));
	void Lock() __attribute__((nothrow));
	void Unlock() __attribute__((nothrow));

private:	// members
	bool m_bAcquired;
	bool m_bSerialEnabled;
	bool m_bConsoleEnabled;

	qkrnl::system::SerialPort *m_pCom;
};

}	 // namespace qkrnl