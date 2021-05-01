#include <kernel/KTextMode.hpp>
#include <kernel/Logger.hpp>
#include <kernel/system/SerialPort.hpp>
#include <mutex.hpp>
#include <new>
#include <stdarg.h>
#include <stdio.hpp>
#include <string.hpp>

using namespace qklib;
using namespace qkrnl::system;

extern __vma_t g_LastUsed;

namespace qkrnl {

static Mutex s_Mutex;

Logger::Logger()
	: Logger(LogLevel::Error) {}

Logger::Logger(LogLevel teLevel)
	: m_bAcquired(false)
	, m_bSerialEnabled(false)
	, m_bConsoleEnabled(true)
	, m_eLevel(teLevel) {

	//	by default, logging to Serial Port is on
	m_pCom = new SerialPort(ComPorts::COM1, BaudRates::BR115200);
	m_bSerialEnabled = true;
}

Logger::~Logger() {

	if (m_bAcquired) {

		s_Mutex.Release();
	}
}

bool Logger::GetSerialPortEnabled() {

	return m_bSerialEnabled;
}

void Logger::SetSerialPortEnabled(bool tValue) {

	m_bSerialEnabled = tValue;
	if (!tValue && m_pCom != nullptr) {

		delete m_pCom;
	}

	if (tValue && m_pCom == nullptr) {

		m_pCom = new SerialPort(ComPorts::COM1, BaudRates::BR115200);
	}
}

bool Logger::GetConsoleEnabled() {

	return m_bConsoleEnabled;
}

void Logger::SetConsoleEnabled(bool tValue) {

	m_bConsoleEnabled = tValue;
}

LogLevel Logger::GetLoggingLevel() {

	return m_eLevel;
}

void Logger::SetLoggingLevel(LogLevel teLevel) {

	m_eLevel = teLevel;
}

void Logger::Debug(const char *tMsg, ...) {

	if (!DebugEnabled())
		return;

	va_list args;
	va_start(args, tMsg);
	PrintWithLock(tMsg, args);
	va_end(args);
}

void Logger::Info(const char *tMsg, ...) {

	if (!InfoEnabled())
		return;

	va_list args;
	va_start(args, tMsg);
	Print(tMsg, args);
	va_end(args);
}

void Logger::Warn(const char *tMsg, ...) {

	if (!WarnEnabled())
		return;

	va_list args;
	va_start(args, tMsg);
	PrintWithLock(tMsg, args);
	va_end(args);
}

void Logger::Error(const char *tMsg, ...) {

	if (!ErrorEnabled())
		return;

	va_list args;
	va_start(args, tMsg);
	PrintWithLock(tMsg, args);
	va_end(args);
}

void Logger::Lock() {

	if (m_bAcquired) {

		return;
	}

	s_Mutex.Acquire();
	m_bAcquired = true;
}

void Logger::Unlock() {

	if (!m_bAcquired) {

		return;
	}

	s_Mutex.Release();
	m_bAcquired = false;
}

void Logger::PrintWithLock(const char *__restrict tFormat, va_list tArgs) {

	Lock();

	Print(tFormat, tArgs);

	Unlock();
}

void Logger::Print(const char *__restrict tFormat, va_list tArgs) {

	char buffer[MAX_LEN];
	int	 len = vsprintf(buffer, tFormat, tArgs);

	if (GetConsoleEnabled())
		TextModePutString(buffer);
	if (GetSerialPortEnabled())
		m_pCom->Write((const byte_t *) buffer, len);
}

}	 // namespace qkrnl