//! PSYQ=3.3 CC1=2.6.3 G=8
#include "main_private.h"

u8 D_80062E54[8];
u8 D_80062E5C;
ActiveCharacterData* D_80062E60;
u32 D_80062E64;
u32 D_80062E68;
s16 D_80062E6C[4];
u32 D_80062E74;
u32 D_80062E78;
s32 D_80062E7C;
s32 D_80062E80;
s32 D_80062E84;
u32 D_80062E88;
u32 D_80062E8C;
u32 D_80062E90;

s32 func_8001AC9C(u8, s32);
s32 func_8001B834(s32);
void func_8001BD50(u8, u8, u8);

s32 func_80017238(u32 arg0, u32* arg1, u8* arg2) {
    *arg2 = arg0;
    *arg1 = arg0 >> 8;
    return func_8001AC9C(*arg2, *arg1);
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001726C);   

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80017678);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001786C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80017E68);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80017F38);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018028);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018220);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800182FC);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018390);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001840C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800184C0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800185A8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018630);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018834);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018934);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018A04);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018AB0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018B14);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018BB8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018C94);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018D4C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018E18);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018E90);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018ECC);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80018FC0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019064);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800190E8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800191A0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019254);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019338);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001937C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800193F4);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019440);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800194BC);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019544);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019608);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001964C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019690);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_800197B8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019978);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019D1C);

void func_80019D74(u8 arg0, u8 arg1) {
    if (arg1 == 0xB) {
        func_80019E4C(arg0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019DA0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019E4C);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019E84);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_80019F90);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A174);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A1C8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A280);

#ifndef NON_MATCHINGS
// needs BSS import for %gp regs
void func_8001A384(u8 arg0, s32 arg1);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A384);
#else
extern s8 D_80062FFC = 0;
extern u8 D_80063020 = 0;
void func_8001A384(u8 arg0, s32 arg1) {
    func_8001AC9C(arg0, arg1);
    if (D_80063020) {
        D_80062FFC = 11;
    }
}
#endif

#ifndef NON_MATCHINGS
// needs BSS import for %gp regs
void func_8001A3B8(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A3B8);
#else
extern s8 D_80062FFC = 0;
extern u8 D_80063020 = 0;
void func_8001A3B8(s32 arg0, s32 arg1, s32 arg2) {
    u8 param;
    s32 i;
    s32 enabled;
    s32 bits;

    if (D_80063020 == 0) {
        bits = arg2 & 0xFFFFFF;
        for (i = 0; i < 0x18; i++) {
            enabled = bits & 1;
            bits >>= 1;
            if (enabled) {
                param = i + 0x48;
                func_8001BD50(i, param, param);
            }
        }

        func_8001B834(13);
        return;
    }
    D_80062FFC = 8;
}
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A440);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A4A8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A518);

void func_8001A684(u8, s32);
void func_8001A780(u8, s32);
void func_8001A874(u8, s32);
void func_8001A5B4(u8 materiaSubType, u8 materiaId, s32 materiaAp) {
    u8 id;
    u8 materiaLevel;

    id = materiaId;
    materiaLevel = func_8001AC9C(id, materiaAp);
    switch (materiaSubType) {
    case 0:
        func_8001A684(id, materiaAp);
        break;
    case 2:
        func_8001A780(id, materiaAp);
        break;
    case 4:
        func_8001A874(id, materiaAp);
        break;
    case 3:
        func_80019D74(materiaLevel, id);
        break;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A684);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A780);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A874);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A980);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001A9CC);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001AB1C);

#ifndef NON_MATCHINGS
// matching with GCC 2.6.3
s32 func_8001AC9C(u8, s32);
INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001AC9C);
#else
extern Unk80062F7C* D_80062F7C = NULL; // %gp
extern s32 D_80062F10 = 0;             // %gp
extern s32 D_80062FBC = 0;             // %gp
extern u8 D_80063020 = 0;              // %gp
s32 func_8001AC9C(u8 arg0, s32 arg1) {
    s32 i;
    s32 found;
    u16 temp_a2;
    Unk80062F7C* new_var;

    found = 1;
    for (i = 3; i >= 0; i--) {
        temp_a2 = D_800730CC[arg0].unk4[i];
        if (temp_a2 == 0xFFFF || arg1 < temp_a2 * 100) {
            continue;
        }
        found = i + 2;
        break;
    }
    D_80062FBC = 1;
    for (i = 0; i < 4; i++) {
        temp_a2 = D_800730CC[arg0].unk4[i];
        if (temp_a2 != 0xFFFF) {
            D_80062FBC++;
        }
    }
    if (D_80063020) {
        temp_a2 = D_800730CC[arg0].unk4[found - 1];
        if (temp_a2 == 0xFFFF || found == D_80062FBC) {
            D_80062F10 = 0;
        } else {
            D_80062F10 = temp_a2 * 100 - arg1;
        }
        new_var = D_80062F7C;
        new_var->unk0 = found;
        new_var->unk1 = *(u8*)&D_80062FBC;
        new_var->unk4 = D_80062F10;
    }
    return found;
}
#endif

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001AE08);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001AEE4);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B4A0);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B570);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B5E4);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B704);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B834);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B8A8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001B944);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BA54);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BB30);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BC18);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BCE8);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BD50);

INCLUDE_ASM("asm/us/main/nonmatchings/17238", func_8001BDB0);
