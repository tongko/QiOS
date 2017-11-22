#ifndef __K_STDINT_H_
#define __K_STDINT_H_

#ifdef __KERNEL_
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#endif  //	__KERNEL_
#endif  //	__K_STDINT_H_
