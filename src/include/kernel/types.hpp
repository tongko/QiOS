#pragma once

#include <kernel/Error.hpp>
#include <stdint.h>

#if !defined(size_t)
	#if defined(__x86_64__)
typedef unsigned long size_t;
	#else
typedef unsigned int size_t;
	#endif
#endif

typedef uintptr_t __vma_t;
typedef uintptr_t __pma_t;
typedef uint8_t	  byte_t;
typedef uint16_t  word_t;
typedef uint32_t  dword_t;
typedef uint64_t  qword_t;
typedef int8_t	  char_t;
typedef int16_t	  short_t;
typedef int32_t	  int_t;
typedef int64_t	  long_t;

// Function's return result
typedef dword_t kresult_t;

// Process id
typedef dword_t proc_id_t;