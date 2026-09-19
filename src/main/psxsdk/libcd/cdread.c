//! PSYQ=3.3
#include <common.h>

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", cb_read);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", cd_read_retry);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", CdReadBreak);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", CdRead);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", CdReadSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libcd/cdread", CdReadCallback);
