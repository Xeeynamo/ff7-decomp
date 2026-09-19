//! PSYQ=3.3
#include <common.h>

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", ResetCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", InterruptCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", DMACallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", VSyncCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", VSyncCallbacks);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", StopCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", RestartCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", CheckCallback);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", GetIntrMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", SetIntrMask);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", startIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", trapIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", setIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", stopIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", restartIntr);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr", memclr);
