#pragma once

#include <cstddef>
#include <cstdint>
#include <mm/QMemManager.h>
#include <system/system.h>

System::ITerm * Term;
QMemory::IHeap *KHeap;

inline void *operator new(size_t size) {
	return KHeap->Alloc(size);
}

inline void *operator new[](size_t size) {
	return KHeap->Alloc(size);
}

inline void operator delete(void *p) {
	KHeap->Free(p);
}

inline void operator delete[](void *p) {
	KHeap->Free(p);
}

inline void *operator new(size_t, void *p) throw() {
	return p;
}

inline void *operator new[](size_t, void *p) throw() {
	return p;
}

inline void operator delete(void *, void *) throw() {};

inline void operator delete[](void *, void *) throw() {};
