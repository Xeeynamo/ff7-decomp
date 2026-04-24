#ifndef COMMON_H
#define COMMON_H

#ifndef NULL
#define NULL 0
#endif

#ifdef SKIP_ASM
#undef USE_INCLUDE_ASM
#endif

#ifdef USE_INCLUDE_ASM
__asm__(".include \"macro.inc\"\n");
#define INCLUDE_ASM(FOLDER, NAME)                                              \
    void __maspsx_include_asm_hack_##NAME() {                                  \
        __asm__(".text # maspsx-keep \n"                                       \
                "\t.align\t2 # maspsx-keep\n"                                  \
                "\t.set noreorder # maspsx-keep\n"                             \
                "\t.set noat # maspsx-keep\n"                                  \
                ".include \"" FOLDER "/" #NAME ".s\" # maspsx-keep\n"          \
                "\t.set reorder # maspsx-keep\n"                               \
                "\t.set at # maspsx-keep\n");                                  \
    }
#else
#define INCLUDE_ASM(FOLDER, NAME)
#endif

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
