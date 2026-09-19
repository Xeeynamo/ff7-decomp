//! PSYQ=3.3
#include <common.h>

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_init);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_writeByIO);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FiDMA);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_r_);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_t);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_write);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_read);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FsetRXX);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FsetRXXa);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FgetRXXa);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FsetPCR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FsetDelayW);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FsetDelayR);

INCLUDE_ASM("asm/us/main/nonmatchings/psxsdk/libspu/spu", _spu_FwaitFs);
