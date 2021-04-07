// this function needs to be exists in order to be called when pure virtual
// function cannot be called.
// see https://wiki.osdev.org/C%2B%2B
extern "C" void __cxa_pure_virtual() {
	// Do nothing
}

#if defined(__GNUG__) && (__cplusplus)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-value"
	#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

extern "C" int __cxa_atexit(void (*destructor)(void *), void *arg, void *dso) {
	(arg);
	(dso);
	return 0;
}

extern "C" void __cxa_finalize(void *f) {
	(f);
}

#if defined(__GNUG__) && (__cplusplus)
	#pragma GCC diagnostic pop
#endif
