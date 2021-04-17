#pragma once

#include <cdefs.h>
#include <stddef.h>

#if defined(__cplusplus)
	#define BEGIN_EXT_C extern "C" {
	#define END_EXT_C	}	 // extern "C"
#else
	#define BEGIN_EXT_C
	#define END_EXT_C
#endif

/* #define DECLARE_LOCK(name) volatile int name##Locked
// #define LOCK(name)                                             \
// 	while (!__sync_bool_compare_and_swap(&name##Locked, 0, 1)) \
// 		;                                                      \
// 	__sync_synchronize()
// #define UNLOCK(name)      \
// 	__sync_synchronize(); \
// 	name##Locked = 0
*/

//	Only for C++
#if defined(__cplusplus)
	#define PROPERTY_DECL(type, varname) \
		type varname##_get();            \
		void varname##_set(type);

	#define STATIC_PROP_DECL(type, varname) \
		static type varname##_get();        \
		static void varname##_set(type);

	#define PROP_DECL_IMPL(type, varname) \
		type varname##_get() override;    \
		void varname##_set(type) override;
#endif

#define MAX_LEN 256

namespace qklib {

/**
 * Generic type for storing range of value
 */
template <typename T>
struct range_t {
	T	   Start;
	size_t Length;

	T			GetEnd() { return (T)(Start + Length); }
	inline bool Intersect(range_t<T> &other) {
		return Start <= other.GetEnd() && GetEnd() >= other.Start;
	}
	void Advance(size_t tSize) {
		Start += tSize;
		Length -= tSize;
	}
	void Shrink(size_t tSize) { Length -= tSize; }
};

}	 // namespace qklib