#pragma once

// Implementation of these functions should be found in assembly code located
// under the arch/XXX folder where XXX denotes the CPU architecture

#include <kernel/types.hpp>

extern "C" {

extern void outportb(word_t port, byte_t value);

extern void outportw(word_t port, word_t value);

extern byte_t inportb(word_t port);

extern word_t inportw(word_t port);

}