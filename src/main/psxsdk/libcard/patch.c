//! PSYQ=3.3
#include <common.h>

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcard/patch", _dsr_start);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcard/patch", _patch_card);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcard/patch", _patch_wait_start);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcard/patch", _patch_card2);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcard/patch", _wait);
