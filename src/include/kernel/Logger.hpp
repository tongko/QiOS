#pragma once

#include <Enums.hpp>
#include <common.hpp>
#include <kernel/types.hpp>
#include <stdarg.h>

using namespace qklib;

namespace qkrnl {
namespace system {

//	Prototypes
class SerialPort;
}	 // namespace system

enum class LogLevel {
	Information = 0x01,
	Warning = 0x02,
	Error = 0x04,
	Debug = 0x08
};

inline bool operator<(LogLevel &lhs, LogLevel &rhs) {
	return (int) lhs < (int) rhs;
}
inline bool operator<=(LogLevel &lhs, LogLevel &rhs) {
	return (int) lhs <= (int) rhs;
}
inline bool operator>(LogLevel &lhs, LogLevel &rhs) {
	return (int) lhs > (int) rhs;
}
inline bool operator>=(LogLevel &lhs, LogLevel &rhs) {
	return (int) lhs >= (int) rhs;
}

class Logger {

public:	   //	Constructors
	Logger();
	Logger(LogLevel);
	~Logger();

public:	   //	Attributes
	PROPERTY_DECL(bool, SerialPortEnabled);
	PROPERTY_DECL(bool, ConsoleEnabled);
	PROPERTY_DECL(LogLevel, LoggingLevel);
	inline bool DebugEnabled() { return LogLevel::Debug < m_eLevel; }
	inline bool InfoEnabled() { return LogLevel::Information < m_eLevel; }
	inline bool WarnEnabled() { return LogLevel::Warning < m_eLevel; }
	inline bool ErrorEnabled() { return LogLevel::Error < m_eLevel; }

public:	   //	Operations
	void Debug(const char *, ...);
	void Info(const char *, ...);
	void Warn(const char *, ...);
	void Error(const char *, ...);
	void Lock() __attribute__((nothrow));
	void Unlock() __attribute__((nothrow));

protected:	  //	Operations
	void PrintWithLock(const char *__restrict tFormat, va_list tArgs)
		__attribute__((nothrow));
	void Print(const char *__restrict tFormat, va_list tArgs)
		__attribute__((nothrow));

private:	// members
	bool	 m_bAcquired;
	bool	 m_bSerialEnabled;
	bool	 m_bConsoleEnabled;
	LogLevel m_eLevel;

	qkrnl::system::SerialPort *m_pCom;
};

}	 // namespace qkrnl