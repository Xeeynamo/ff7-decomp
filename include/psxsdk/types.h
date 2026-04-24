#ifndef TYPES_H
#define TYPES_H

#ifndef __psyz
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef unsigned int size_t;
#else
#include <sys/types.h>
#endif

#ifdef _MSC_VER
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long long u_long;
#endif

typedef signed char byte;
#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

#ifndef NULL
#define NULL (0)
#endif

#endif
