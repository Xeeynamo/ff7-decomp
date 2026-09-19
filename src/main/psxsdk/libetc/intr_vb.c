//! PSYQ=3.3
#include <common.h>

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr_vb", startIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr_vb", trapIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr_vb", setIntrVSync);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libetc/intr_vb", DMA_memclr);
