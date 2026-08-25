//! PSYQ=3.6 G=8
#include <common.h>

// Largely copied from unzip.c and inflate.c of version 1.2.4 of gzip.
// FF7's implementation uses a 16k buffer on the stack to store hufts instead of
// heap memory. Comments in /* */ are from inflate.c.

#define GZIP_MAGIC "\037\213" // Magic header for gzip files, 1F 8B.
#define DEFLATED 8            // Only compression method used in FF7.
#define INFLATE_MEM_SIZE 4096
#define WSIZE 32768 /* window size--must be a power of two, and */
                    /*  at least 32K for zip's deflate method */

#define NEXTBYTE()                                                             \
    (inbuf[inptr++]) // FF7's implementation stores the entire file in memory so
                     // logic for filling inbuf from file is removed.

#define NEEDBITS(n)                                                            \
    {                                                                          \
        while (k < (n)) {                                                      \
            b |= ((u32)NEXTBYTE()) << k;                                       \
            k += 8;                                                            \
        }                                                                      \
    }

#define DUMPBITS(n)                                                            \
    {                                                                          \
        b >>= (n);                                                             \
        k -= (n);                                                              \
    }

typedef struct huft huft;

struct huft {
    u8 e; // Number of extra bits or operation.
    u8 b; // Number of bits in this code or subcode.
    union {
        u16 n;   // Literal, length base, or distance base.
        huft* t; // Pointer to next level of table.
    } v;
};

static u32 wp; // Window position.
static u32 inptr;
static u8* inbuf;
static u8* slide;
static s32* inflateMem;
static u32 bb;    // Bit buffer.
static u32 bk;    // Bits in bit buffer.
static u32 hufts; // Track memory usage.

/* Tables for deflate from PKZIP's appnote.txt. */
/* Order of the bit length code lengths */
static u32 border[] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
/* Copy lengths for literal codes 257..285 */
static u16 cplens[] = {
    3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0};
/* Extra bits for literal codes 257..285 */
static u16 cplext[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2,  2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99}; /* 99==invalid */
/* Copy offsets for distance codes 0..29 */
static u16 cpdist[] = {
    1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
    33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
    1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
/* Extra bits for distance codes */
static u16 cpdext[] = {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
                       6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
static u16 mask_bits[] = {
    0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff};
static u32 inflateMemAlloced = 0;
static s32 lbits = 9; /* bits in base literal/length lookup table */
static s32 dbits = 6; /* bits in base distance lookup table */

static s32 inflate();
static s32 inflate_block(s32* e);
static s32 inflate_stored();
static s32 inflate_fixed();
static s32 huft_build(u32* b, u32 n, u32 s, u16* d, u16* e, huft** t, s32* m);
static void* inflate_malloc(u32 size);
static s32 huft_free(huft* t);
static s32 inflate_codes(huft* tl, huft* td, s32 bl, s32 bd);
static s32 inflate_dynamic();

/* If BMAX needs to be larger than 16, then h and dr[] should be ulg. */
#define BMAX 16   /* maximum bit length of any code (16 for explode) */
#define N_MAX 288 /* maximum number of codes in any set */

static void* inflate_malloc(u32 size) {
    u32 roundedSize;
    s32* mem;

    roundedSize = (size + 3) >> 2;
    if (inflateMemAlloced + roundedSize <= INFLATE_MEM_SIZE) {
        mem = &inflateMem[inflateMemAlloced];
        inflateMemAlloced += roundedSize;
    } else {
        inflateMemAlloced = 0;
        mem = inflate_malloc(size);
    }
    return mem;
}

static s32 huft_build(u32* b, u32 n, u32 s, u16* d, u16* e, huft** t, s32* m) {
    u32 a;           /* counter for codes of length k */
    u32 c[BMAX + 1]; /* bit length count table */
    s32 clear;       /* counter for clearing c[] */
    u32 f;           /* i repeats in table every f entries */
    s32 g;           /* maximum code length */
    s32 h;           /* table level */
    u32 i;           /* counter, current code */
    u32 j;           /* counter */
    s32 k;           /* number of bits in current code */
    s32 l;           /* bits per table (returned in m) */
    u32* p;          /* pointer into c[], b[], or v[] */
    huft* q;         /* points to current table */
    huft r;          /* table entry for structure assignment */
    huft* u[BMAX];   /* table stack */
    u32 v[N_MAX];    /* values in order of bit length */
    s32 w;           /* bits before this table == (l * h) */
    u32 x[BMAX + 1]; /* bit offsets, then code stack */
    u32* xp;         /* pointer into dr */
    s32 y;           /* number of dummy codes added */
    u32 z;           /* number of entries in current table */

    /* Generate counts for each bit length */
    for (clear = BMAX; clear >= 0; clear--)
        c[clear] = 0;
    p = b;
    i = n;
    do {
        c[*p]++; /* assume all entries <= BMAX */
        p++;
    } while (--i);
    if (c[0] == n) { /* null input--all zero length codes */
        *t = NULL;
        *m = 0;
        return 0;
    }

    /* Find minimum and maximum length, bound *m by those */
    l = *m;
    for (j = 1; j <= BMAX; j++)
        if (c[j] != 0)
            break;
    k = j; /* minimum code length */
    if ((u32)l < j)
        l = j;
    for (i = BMAX; i != 0; i--)
        if (c[i] != 0)
            break;
    g = i; /* maximum code length */
    if ((u32)l > i)
        l = i;
    *m = l;

    /* Adjust last length count to fill out codes, if needed */
    for (y = 1 << j; j < i; j++, y <<= 1)
        if ((y -= c[j]) < 0)
            return 2; /* bad input: more codes than bits */
    if ((y -= c[i]) < 0)
        return 2;
    c[i] += y;

    /* Generate starting offsets into the value table for each length */
    x[1] = j = 0;
    p = c + 1;
    xp = x + 2;
    while (--i != 0) /* note that i == g from above */
        *xp++ = (j += *p++);

    /* Make a table of values in order of bit lengths */
    p = b;
    i = 0;
    do {
        if ((j = *p++) != 0)
            v[x[j]++] = i;
    } while (++i < n);

    /* Generate the Huffman codes and for each, make the table entries */
    x[0] = i = 0; /* first Huffman code is zero */
    p = v;        /* grab values in bit order */
    h = -1;       /* no tables yet--level -1 */
    w = -l;       /* bits decoded == (l * h) */
    u[0] = NULL;  /* just to keep compilers happy */
    q = NULL;     /* ditto */
    z = 0;        /* ditto */

    /* go through the bit lengths (k already is bits in shortest code) */
    for (; k <= g; k++) {
        a = c[k];
        while (a--) {
            /* here i is the Huffman code of length k bits for value *p */
            /* make tables up to required level */
            while (k > w + l) {
                h++;
                w += l; /* previous table always l bits */

                /* compute minimum size table less than or equal to l bits */
                z = (z = g - w) > (u32)l ? l
                                         : z; /* upper limit on table size */
                if ((f = 1 << (j = k - w)) > a + 1) { /* try a k-w bit table */
                    /* too few codes for k-w bit table */
                    f -= a + 1; /* deduct codes from patterns left */
                    xp = c + k;
                    while (++j < z) { /* try smaller tables up to db bits */
                        if ((f <<= 1) <= *++xp)
                            break; /* enough codes to use up j bits */
                        f -= *xp;  /* else deduct codes from patterns */
                    }
                }
                z = 1 << j; /* table entries for j-bit table */

                /* allocate and link in new table */
                // FF7 uses own malloc replacement here as hufts are stored on
                // the stack.
                if ((q = (huft*)inflate_malloc((z + 1) * sizeof(huft))) ==
                    NULL) {
                    if (h != 0)
                        huft_free(u[0]);
                    return 3; /* not enough memory */
                }
                hufts += z + 1; /* track memory usage */
                *t = q + 1;     /* link to list for huft_free() */
                *(t = &(q->v.t)) = NULL;
                u[h] = ++q; /* table starts after link */

                /* connect to last table, if there is one */
                if (h != 0) {
                    x[h] = i;           /* save pattern for backing up */
                    r.b = (u8)l;        /* bits to dump before this table */
                    r.e = (u8)(16 + j); /* bits in this table */
                    r.v.t = q;          /* pointer to this table */
                    j = i >> (w - l);   /* (get around Turbo C bug) */
                    u[h - 1][j] = r;    /* connect to last table */
                }
            }

            /* set up table entry in r */
            r.b = (u8)(k - w);
            if (p >= v + n)
                r.e = 99; /* out of values--invalid code */
            else if (*p < s) {
                r.e = (u8)(*p < 256 ? 16 : 15); /* 256 is end-of-block code */
                r.v.n = (u16)(*p); /* simple code is just the value */
                p++;               /* one compiler does not like *p++ */
            } else {
                r.e = (u8)e[*p - s]; /* non-simple--look up in lists */
                r.v.n = d[*p++ - s];
            }

            /* fill code-like entries with r */
            f = 1 << (k - w);
            for (j = i >> w; j < z; j += f)
                q[j] = r;

            /* backwards increment the k-bit code i */
            for (j = 1 << (k - 1); i & j; j >>= 1)
                i ^= j;
            i ^= j;

            /* backup over finished tables */
            while ((i & ((1 << w) - 1)) != x[h]) {
                h--; /* don't need to update q */
                w -= l;
            }
        }
    }

    /* Return true (1) if we were given an incomplete table */
    return y != 0 && g != 1;
}

// As FF7 uses stack memory to store hufts, there's nothing to free. We just
// iterate back through the linked list as is expected of the caller.
static s32 huft_free(huft* t) {
    while (t != NULL)
        t = (--t)->v.t;
    return 0;
}

static s32 inflate_codes(huft* tl, huft* td, s32 bl, s32 bd) {
    /* inflate (decompress) the codes in a deflated (compressed) block.
    Return an error code or zero if it all goes ok. */
    // FF7's version lacks code to flush and refill window as it fills inbuf
    // with the entire file prior to calling Unzip().

    u32 e;      /* table entry flag/number of extra bits */
    u32 n, d;   /* length and index for copy */
    u32 w;      /* current window position */
    huft* t;    /* pointer to table entry */
    u32 ml, md; /* masks for bl and bd bits */
    u32 b;      /* bit buffer */
    u32 k;      /* number of bits in bit buffer */

    /* make local copies of globals */
    b = bb; /* initialize bit buffer */
    k = bk;
    w = wp; /* initialize window position */

    /* inflate the coded data */
    ml = mask_bits[bl]; /* precompute masks for speed */
    md = mask_bits[bd];

    for (;;) { /* do until end of block */
        NEEDBITS((u32)bl);
        if ((e = (t = tl + ((u32)b & ml))->e) > 16) {
            do {
                if (e == 99)
                    return 1;
                DUMPBITS(t->b);
                e -= 16;
                NEEDBITS(e);
            } while ((e = (t = t->v.t + ((u32)b & mask_bits[e]))->e) > 16);
        }
        DUMPBITS(t->b);
        if (e == 16) { /* then it's a literal */
            slide[w++] = (u8)t->v.n;
        } else { /* it's an EOB or a length */
            /* exit if end of block */
            if (e == 15)
                break;

            /* get length of block to copy */
            NEEDBITS(e);
            n = t->v.n + ((u32)b & mask_bits[e]);
            DUMPBITS(e);

            /* decode distance of block to copy */
            NEEDBITS((u32)bd);
            if ((e = (t = td + ((u32)b & md))->e) > 16)
                do {
                    if (e == 99)
                        return 1;
                    DUMPBITS(t->b);
                    e -= 16;
                    NEEDBITS(e);
                } while ((e = (t = t->v.t + ((u32)b & mask_bits[e]))->e) > 16);
            DUMPBITS(t->b);
            NEEDBITS(e);
            d = w - t->v.n - ((u32)b & mask_bits[e]);
            DUMPBITS(e);

            /* do the copy */
            e = n;
            if (e != 0) {
                do {
                    slide[w++] = slide[d++];
                } while (--e);
            }
            n = 0;
        }
    }

    /* restore the globals from the locals */
    wp = w; /* restore global window pointer */
    bb = b; /* restore global bit buffer */
    bk = k;

    /* done */
    return 0;
}

static s32 inflate_stored() {
    /* "decompress" an inflated type 0 (stored) block. */
    u32 n; /* number of bytes in block */
    u32 w; /* current window position */
    u32 b; /* bit buffer */
    u32 k; /* number of bits in bit buffer */

    /* make local copies of globals */
    b = bb; /* initialize bit buffer */
    k = bk;
    w = wp; /* initialize window position */

    /* go to byte boundary */
    n = k & 7;
    DUMPBITS(n);

    /* get the length and its complement */
    NEEDBITS(16);
    n = ((u32)b & 0xffff);
    DUMPBITS(16);
    NEEDBITS(16);
    if (n != (u32)((~b) & 0xffff))
        return 1; /* error in compressed data */
    DUMPBITS(16);

    /* read and output the compressed data */
    while (n--) {
        NEEDBITS(8);
        slide[w++] = (u8)b;
        DUMPBITS(8);
    }

    /* restore the globals from the locals */
    wp = w; /* restore global window pointer */
    bb = b; /* restore global bit buffer */
    bk = k;
    return 0;
}

static s32 inflate_fixed() {
    /* decompress an inflated type 1 (fixed Huffman codes) block.  We should
       either replace this with a custom decoder, or at least precompute the
       Huffman tables. */
    int i;           /* temporary variable */
    struct huft* tl; /* literal/length code table */
    struct huft* td; /* distance code table */
    int bl;          /* lookup bits for tl */
    int bd;          /* lookup bits for td */
    unsigned l[288]; /* length list for huft_build */

    /* set up literal table */
    for (i = 0; i < 144; i++)
        l[i] = 8;
    for (; i < 256; i++)
        l[i] = 9;
    for (; i < 280; i++)
        l[i] = 7;
    for (; i < 288; i++) /* make a complete, but wrong code set */
        l[i] = 8;
    bl = 7;
    if ((i = huft_build(l, 288, 257, cplens, cplext, &tl, &bl)) != 0)
        return i;

    /* set up distance table */
    for (i = 0; i < 30; i++) /* make an incomplete code set */
        l[i] = 5;
    bd = 5;
    if ((i = huft_build(l, 30, 0, cpdist, cpdext, &td, &bd)) > 1) {
        huft_free(tl);
        return i;
    }

    /* decompress until an end-of-block code */
    if (inflate_codes(tl, td, bl, bd) != 0)
        return 1;

    /* free the decoding tables, return */
    huft_free(tl);
    huft_free(td);
    return 0;
}

static s32 inflate_dynamic() {
    /* decompress an inflated type 2 (dynamic Huffman codes) block. */
    // The code for dealing with errors from huft_build() is different than the
    // implementation from gzip.
    s32 i; /* temporary variables */
    u32 j;
    u32 l;            /* last length */
    u32 m;            /* mask for bit lengths table */
    u32 n;            /* number of lengths to get */
    huft* tl;         /* literal/length code table */
    huft* td;         /* distance code table */
    s32 bl;           /* lookup bits for tl */
    s32 bd;           /* lookup bits for td */
    u32 nb;           /* number of bit length codes */
    u32 nl;           /* number of literal/length codes */
    u32 nd;           /* number of distance codes */
    u32 ll[286 + 30]; /* literal/length and distance code lengths */
    u32 b;            /* bit buffer */
    u32 k;            /* number of bits in bit buffer */

    /* make local bit buffer */
    b = bb;
    k = bk;

    /* read in table lengths */
    NEEDBITS(5);
    nl = 257 + ((u32)b & 0x1f); /* number of literal/length codes */
    DUMPBITS(5);
    NEEDBITS(5);
    nd = 1 + ((u32)b & 0x1f); /* number of distance codes */
    DUMPBITS(5);
    NEEDBITS(4);
    nb = 4 + ((u32)b & 0xf); /* number of bit length codes */
    DUMPBITS(4);

    if (nl > 286 || nd > 30)
        return 1; /* bad lengths */

    /* read in bit-length-code lengths */
    for (j = 0; j < nb; j++) {
        NEEDBITS(3);
        ll[border[j]] = (u32)b & 7;
        DUMPBITS(3);
    }
    for (; j < 19; j++)
        ll[border[j]] = 0;

    /* build decoding table for trees--single level, 7 bit lookup */
    bl = 7;
    if ((i = huft_build(ll, 19, 19, NULL, NULL, &tl, &bl)) != 0) {
        if (i == 1)
            huft_free(tl);
        return i; /* incomplete code set */
    }

    /* read in literal and distance code lengths */
    n = nl + nd;
    m = mask_bits[bl];
    i = l = 0;
    while ((u32)i < n) {
        NEEDBITS((u32)bl);
        j = (td = tl + ((u32)b & m))->b;
        DUMPBITS(j);
        j = td->v.n;
        if (j < 16)          /* length of code in bits (0..15) */
            ll[i++] = l = j; /* save last length in l */
        else if (j == 16) {  /* repeat last length 3 to 6 times */
            NEEDBITS(2);
            j = 3 + ((u32)b & 3);
            DUMPBITS(2);
            if ((u32)i + j > n)
                return 1;
            while (j--)
                ll[i++] = l;
        } else if (j == 17) { /* 3 to 10 zero length codes */
            NEEDBITS(3);
            j = 3 + ((u32)b & 7);
            DUMPBITS(3);
            if ((u32)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        } else { /* j == 18: 11 to 138 zero length codes */
            NEEDBITS(7);
            j = 11 + ((u32)b & 0x7f);
            DUMPBITS(7);
            if ((u32)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
    }

    /* free decoding table for trees */
    huft_free(tl);

    /* restore the global bit buffer */
    bb = b;
    bk = k;

    /* build the decoding tables for literal/length and distance codes */
    bl = lbits;
    if ((i = huft_build(ll, nl, 257, cplens, cplext, &tl, &bl)) != 0) {
        if (i == 1) { // incomplete literal tree
            return -1;
        }
        return i; /* incomplete code set */
    }
    bd = dbits;
    if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &td, &bd)) != 0) {
        if (i == 1) { // incomplete distance tree
            return -1;
        }
        huft_free(tl);
        return i; /* incomplete code set */
    }

    /* decompress until an end-of-block code */
    if (inflate_codes(tl, td, bl, bd) != 0)
        return 1;

    /* free the decoding tables, return */
    huft_free(tl);
    huft_free(td);
    return 0;
}

static s32 inflate_block(s32* e) {
    u32 t; /* block type */
    u32 b; /* bit buffer */
    u32 k; /* number of bits in bit buffer */

    /* make local bit buffer */
    b = bb;
    k = bk;

    /* read in last block bit */
    NEEDBITS(1);
    *e = (s32)b & 1;
    DUMPBITS(1);

    /* read in block type */
    NEEDBITS(2);
    t = (u32)b & 3;
    DUMPBITS(2);

    /* restore the global bit buffer */
    bb = b;
    bk = k;

    /* inflate that block type */
    if (t == 2) // Dynamic Huffman compressed block.
        return inflate_dynamic();
    if (t == 0) // A stored block, no compression.
        return inflate_stored();
    if (t == 1) // Static huffman compressed block.
        return inflate_fixed();

    /* bad block type */
    return 2;
}

static s32 inflate() {
    s32 e; /* last block flag */
    s32 r; /* result code */

    /* initialize window, bit buffer */
    wp = 0;
    bk = 0;
    bb = 0;

    /* decompress until the last block */
    do {
        hufts = 0;
        if ((r = inflate_block(&e)) != 0)
            return r;
    } while (e == 0);

    /* Undo too much lookahead. The next read will be byte aligned so we
     * can discard unused bits in the last meaningful byte.
     */
    while (bk >= 8) {
        bk -= 8;
        inptr--;
    }

    return 0;
}

s32 Unzip(u8* src, u8* dst) {
    u8 buf[8];
    s32 cache[INFLATE_MEM_SIZE];
    s32 magic1;
    s32 magic2;
    u8 compMethod;
    s32 res;
    s32 i;
    u32 crc = 0;
    u32 size = 0;

    inbuf = src;
    slide = dst;
    inptr = 0;
    wp = 0;
    inflateMem = cache;

    magic1 = NEXTBYTE() != GZIP_MAGIC[0];
    magic2 = NEXTBYTE() != GZIP_MAGIC[1];
    if (magic1 | magic2)
        return -1;

    compMethod = NEXTBYTE();
    if (compMethod != DEFLATED)
        return -1;

    // Flags, check that none are set as they're not supported.
    if (NEXTBYTE() != 0)
        return -1;

    // Move position beyond 10 byte gzip header to start of compressed data.
    NEXTBYTE();
    NEXTBYTE();
    NEXTBYTE();
    NEXTBYTE();
    NEXTBYTE();
    NEXTBYTE();

    res = inflate();
    if (res == 3) // Out of memory.
        return -1;
    if (res != 0) // Invalid compressed data--format violated.
        return -1;

    // Read CRC32 and size of uncompressed data.
    for (i = 0; i < 8; i++)
        buf[i] = NEXTBYTE();

    crc = buf[0] | (buf[1] << 8);
    crc |= (buf[2] | (buf[3] << 8)) << 16;
    size = buf[4];
    size |= buf[5] << 8;
    size |= (buf[6] | (buf[7] << 8)) << 16;
    if (size != wp)
        return -1;
    // Return size if unzip is successful. Will cause overflow if size is larger
    // than 2^31 - 1.
    return size;
}
