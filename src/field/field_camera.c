//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

const u32 D_800A00DC[] = {0x00000000};
INCLUDE_ASM("asm/us/field/nonmatchings/field_camera", FieldModelLoadAndInit);

INCLUDE_ASM("asm/us/field/nonmatchings/field_camera", HandleKawaiDataInModel);

// Possable Debug routine. Ran at beginning of every main field loop. (FPS?)
void DebugRunEveryLoop(void) {}

void FieldCameraAssign(void) {
    if (D_80114488 == 0 || D_8009AC2D == 1) {
        D_80071E40 = *D_80083578;
    } else {
        D_80071E40 = D_80083270;
    }
}

INCLUDE_ASM("asm/us/field/nonmatchings/field_camera", FieldUpdateMovieStream);
