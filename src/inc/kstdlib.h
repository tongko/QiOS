#ifndef __KSTDLIB_H
#define __KSTDLIB_H

#include <stdint.h>
#define _NEED_SIZE_T
#include "ktypedef.h"

#ifndef MAXLEN
#define MAXLEN 1024
#endif

uint32_t _atoi(const char *str);
char *_itoa(uint32_t value, char *str, int32_t base);

#endif  //	__KSTDLIB_H