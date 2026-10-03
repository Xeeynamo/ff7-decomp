#include <game.h>

// C version of the streaming LZS extractor found in src/main/lzss.s.
// The disc buffer is refilled between calls, so the decoder keeps its state.
#define CHUNK_LEN 0x4800 // bytes consumed per call, same size as the disc buffer
#define N 4096           // Size of ring buffer
#define F 18             // Upper limit for match_length
#define THRESHOLD 2      // Shortest match that is encoded as position and length

typedef enum {
    LZS_HEADER,  // read the compressed length
    LZS_TOKEN,   // pick the next step from the flag bits
    LZS_FLAGS,   // read a new flag byte
    LZS_LITERAL, // copy one byte as is
    LZS_REF_LO,  // read the first byte of a back reference
    LZS_REF_HI,  // read the second byte, then copy the match
} LzsStep;

u8* D_80034CF0; // lzs extract source, streaming callers set it again before every chunk

static LzsStep s_step;
static u8* s_dst;
static u8* s_dstStart;
static u32 s_remaining; // compressed bytes left
static s32 s_flags;
static s32 s_flagCount;
static s32 s_refLo;

void SysCdromSetLzsExtract(void* src, void* dst) {
    D_80034CF0 = src;
    s_dst = dst;
    s_dstStart = dst;
    s_step = LZS_HEADER;
}

// Returns 1 if the chunk ran out and the next one must be read, 0 once done
s32 func_80034D5C(void) {
    u8* src = D_80034CF0;
    u8 *copy, *copyEnd;
    s32 avail = CHUNK_LEN;
    s32 i;
    u8 ch;

    if (s_step == LZS_HEADER) {
        s_remaining = *(u32*)src;
        src += 4;
        avail -= 4;
        s_flagCount = 0;
        s_step = LZS_TOKEN;
    }
    for (;;) {
        if (s_step == LZS_TOKEN) {
            if (!s_flagCount) {
                s_step = LZS_FLAGS;
            } else if (s_flags & 1) {
                s_step = LZS_LITERAL;
            } else {
                s_step = LZS_REF_LO;
            }
        }
        ch = *src++;
        switch (s_step) {
        case LZS_FLAGS:
            s_flags = ch;
            s_flagCount = 8;
            s_step = LZS_TOKEN;
            break;
        case LZS_REF_LO:
            s_refLo = ch;
            s_step = LZS_REF_HI;
            break;
        case LZS_REF_HI:
            i = s_refLo | ((ch & 0xF0) << 4);
            copyEnd = s_dst + (ch & 0x0F) + THRESHOLD + 1;
            copy = s_dst - ((s_dst - s_dstStart - (i - (N - F))) & (N - 1));
            for (; copy < s_dstStart; copy++) {
                *s_dst++ = 0;
            }
            for (; s_dst < copyEnd; copy++) {
                *s_dst++ = *copy;
            }
            s_flags >>= 1;
            s_flagCount--;
            s_step = LZS_TOKEN;
            break;
        default: // LZS_LITERAL
            *s_dst++ = ch;
            s_flags >>= 1;
            s_flagCount--;
            s_step = LZS_TOKEN;
            break;
        }
        if (--s_remaining == 0) {
            return 0;
        }
        if (--avail == 0) {
            D_80034CF0 = src;
            return 1;
        }
    }
}
