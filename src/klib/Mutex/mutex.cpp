#include <cdefs.h>
#include <mutex.hpp>

namespace qklib {

static const int MUTEX_INITIALIZED = 0xDEAD;

static int s_nInitLock = 0;

static void Error(Mutex *mutex) {

#if defined(__GNUG__) && (__cplusplus)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-value"
#endif
	(mutex);
#if defined(__GNUG__) && (__cplusplus)
	#pragma GCC diagnostic pop
#endif

	for (;;)
		_ASMV_("hlt");
}

/**
 *	Initialized XXX
 *	@param non
 */
Mutex::Mutex()
	: m_nInitialized(1)
	, m_nLock(0)
	, m_nDepth(0)
	, m_dwOwner(-1) {

	while (!_COMPARE_EXCHANGE_(&s_nInitLock, 0, 1))
		_ASM_("pause");

	if (m_nInitialized == MUTEX_INITIALIZED) {

		return;
	}

	m_nLock = 0;
	m_nDepth = 0;
	m_dwOwner = -1;

	m_nInitialized = MUTEX_INITIALIZED;

	s_nInitLock = 0;
}

Mutex::~Mutex() {}

void Mutex::Acquire(bool tSmp) {

	if (m_nInitialized != MUTEX_INITIALIZED)
		Error(this);

	while (!TryAcquire(tSmp))
		_ASM_("pause");
}

bool Mutex::TryAcquire(bool tSmp) {

	if (m_nInitialized != MUTEX_INITIALIZED)
		Error(this);

	//	TODO: Disable interrupts
	//	...
	//	Lock editing
	while (!_COMPARE_EXCHANGE_(&m_nLock, 0, 1))
		_ASM_("pause");

	//	Update mutex
	bool success = false;
	if (m_nDepth == 0) {
		m_dwOwner = 0;	  // TODO: Get current processor id;
		m_nDepth = 1;
		if (tSmp) {
			// TODO: Edit Tasks lock state
		}
		success = true;
	}
	else if (m_dwOwner == 0) {	  // TODO: Get current processor id;
		m_nDepth++;
		success = true;
	}

	//	Allow edit again.
	m_nLock = 0;

	return success;
}

void Mutex::Release(bool tSmp) {

	if (m_nInitialized != MUTEX_INITIALIZED)
		Error(this);

	//	Lock editing
	while (!_COMPARE_EXCHANGE_(&m_nLock, 0, 1))
		_ASM_("pause");

	//	Update mutex
	if (m_nDepth > 0 && --m_nDepth == 0) {
		m_nDepth = 0;
		m_dwOwner = -1;

		if (tSmp) {
			//	TODO: Edit Task local state
		}
	}

	//	Allow edit
	m_nLock = 0;

	//	TODO: Enable interrupts
}

}	 // namespace qklib