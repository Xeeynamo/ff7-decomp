//! PSYQ=3.3 CC1=2.7.2 G=0
#include <common.h>

// Memory card state owned by the PsyQ card library. These live in .data rather
// than .bss even though they start zeroed, so they are written out explicitly.
s32 D_80048CE0 = 0;
s32 D_80048CE4[3] = {0, 0, 0};
s32 D_80048CF0[3] = {0, 0, 0};
