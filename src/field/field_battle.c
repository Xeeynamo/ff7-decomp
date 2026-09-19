//! PSYQ=3.3 CC1=2.6.3
#include <game.h>
#include <libetc.h>
#include "field_private.h"

INCLUDE_ASM("asm/us/field/nonmatchings/field_battle", FieldGetRandomU8FromList);

INCLUDE_ASM("asm/us/field/nonmatchings/field_battle", FieldGetNextRandomU8);

INCLUDE_ASM("asm/us/field/nonmatchings/field_battle", FieldBattleCheck);
