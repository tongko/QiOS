#pragma once

namespace qklib {

//	A very simple mutex object.

class Mutex {

public:	   // Constructor
	Mutex();
	~Mutex();

public:	   // Operations
	void Acquire(bool tSmp = true);
	bool TryAcquire(bool tSmp = true);
	void Release(bool tSmp = true);

private:
	volatile int m_nInitialized;
	volatile int m_nLock;
	int			 m_nDepth;
	unsigned int m_dwOwner;
};

}	 // namespace qklib