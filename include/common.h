#ifndef COMMON_H
#define COMMON_H

#ifndef NULL
#define NULL 0
#endif

#include "include_asm.h"

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef u8 unk_data;
typedef unsigned int* unk_ptr;

#define LEN(x) ((s32)(sizeof(x) / sizeof(*(x))))

#endif
