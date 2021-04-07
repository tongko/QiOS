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
	: m_bAcquired(false)
	, m_bSerialEnabled(false)
	//	by default, logging to console is on
	, m_bConsoleEnabled(true) {

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

void Logger::PrintWithLock(const char *tMsg, ...) {

	Lock();

	va_list arg;
	va_start(arg, tMsg);

	char buffer[MAX_LEN];
	int	 len = vsprintf(buffer, tMsg, arg);
	va_end(arg);

	TextModePutString(buffer);
	m_pCom->Write((const byte_t *) buffer, len);

	Unlock();
}

void Logger::Print(const char *tMsg, ...) {

	va_list arg;
	va_start(arg, tMsg);

	char buffer[MAX_LEN];
	int	 len = vsprintf(buffer, tMsg, arg);
	va_end(arg);

	TextModePutString(buffer);
	m_pCom->Write((const byte_t *) buffer, len);
}

}	 // namespace qkrnl