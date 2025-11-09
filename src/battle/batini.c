#include "battle.h"

extern Unk801B2308 D_80163624;
extern u16 D_8016376C;
void func_800A3354(void); // battle callback for batini, move to battle.h
void func_801B2308(void);

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B0050);

static void func_801B23E0(s32 sceneID, void (*cb)(void));
void func_801B0490(s32 sceneID) {
    s32 i;
    s32 var_s1;
    s8* temp;

    var_s1 = 4;
    if (D_8016376C) {
        var_s1 = 0;
        func_800A7254(0, 0, 15, 0);
        func_800A7254(0, 0, 14, 0);
    }
    for (i = 0; i < 0x40; i++) {
        if (D_800F6936[i][0] >= var_s1) {
            D_800F6936[i][0] = -1;
        }
    }
    for (i = 0; i < 3; i++) {
        D_800F5E66[i][0] = 0;
    }
    if (D_8016376C) {
        func_801B0F08();
    }
    func_801B23E0(sceneID, func_800A3354);
    func_801B1E0C();
    D_800F83AE[0][0] = 0;
    for (i = 0; i < 10; i++) {
        func_800AE954(i);
        temp = (s8*)&D_800F83AE[i][0x19];
        if (temp[8] != -1) {
            D_800F83AE[0][0] |= 1 << i;
        }
    }
    D_800F83CC = sceneID;
    D_800F83A8 = D_80163624.unk2;
    func_801B19AC();
    func_800A4540();
    func_801B2308();
    func_800A4540();
    for (i = 4; i < 10; i++) {
        D_800F5BBC[i][0] = ((u8)func_80014BA8(0x40) + 0x80) << 8;
        func_800B108C(i);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B0668);

void func_801B085C(s32 arg0) {
    D_800F5F44.D_800F7DA6 = 0x10000 / ((arg0 * 480 / 256 + 0x78) * 2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B08C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B0F08);

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1120);

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B11BC);

void func_801B137C(s32 arg0) {
    s32 i;
    Unk8009D84C* unk;

    unk = &D_8009D84C[arg0];
    unk->unk21 = 1;
    for (i = 1; i < 4; i++) {
        unk->un4C[i][0] = 0xFF;
        unk->un4C[i][1] = 0;
        unk->un4C[i][2] = 0;
        unk->un4C[i][3] = 3;
        unk->un4C[i][4] = 0;
        unk->un4C[i][5] = 0;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B13DC);

s32 func_801B14E8(u32 arg0) {
    u8 temp_v1;
    s32 ret;

    temp_v1 = arg0;
    ret = 0;
    if (temp_v1 != 0xFF && ((D_800730DD[temp_v1][0] & 0xF) == 7)) {
        ret = (arg0 >> 8) | 0x80000000;
    }
    return ret;
}

s32 func_801B1530(u32* arg0) {
    s32 ret;
    s32 i;

    ret = 0;
    for (i = 0; i < 8; i++) {
        ret |= func_801B14E8(arg0[0x10 + i]);
        ret |= func_801B14E8(arg0[0x18 + i]);
    }
    return ret;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1598);

const s32 D_801B001C[] = {0x0000, 0x1000, 0x0008, 0x0800};
const s32 D_801B002C[] = {0x0000, 0x000A, 0x0027, 0x000A};
INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1734);

typedef struct {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u8 unk18[0x1C];
} Unk801B18F8; // size:0x34
void func_801B18F8(Unk8009D84C* arg0, Unk801B18F8* arg1, Unk800F83E0* arg2) {
    arg2->unk14 = arg0->unk6;
    arg2->unk15 = arg0->unk7;
    arg2->unk30 = arg0->unk12;
    arg2->unk2A = arg0->unk16;
    arg2->unkD = arg0->unk8;
    arg2->unkE = arg0->unkC;
    arg2->unk20 = arg0->unkA;
    arg2->unk22 = arg0->unkE;
    if (arg2->unkD == 0) {
        arg2->unkD = 1;
    }
    arg1->unk12 = arg2->unk30;
    arg1->unk10 = arg2->unk2A;
    if (arg0->unk23 & 8) {
        arg1->unk16 = 999;
        arg1->unk14 = 9999;
    } else {
        arg1->unk16 = 9999;
        arg1->unk14 = 999;
    }
}

const u8 D_801B003C[] = {0xFF, 0x32, 0x33, 0x34, 0x35, 0xFF, 0x48, 0x07};
INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B19AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1CB0);

INCLUDE_ASM("asm/us/battle/nonmatchings/batini", func_801B1E0C);

void func_801B2308(void) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (D_80163624.unk34[i].unk0 != -1) {
            func_800A6000(i + 4, 0, 0);
        }
    };
    for (i = 0; i < 6; i++) {
        D_80163624.unk34[i].unkC = D_800F83E0[4 + i].unk4;
        D_80163624.unk94[4 + i][1] = D_800F83E0[4 + i].unk10;
        D_800F83E0[4 + i].unk44[0] = D_800F83E0[4 + i].unk0;
    }
}

static const s8 D_801B0044[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x03, 0x03, 0x03, 0x05, 0x6E, 0x64, 0x62};
static void func_801B23E0(s32 sceneID, void (*cb)(void)) {
    u8 dummy[0x100];
    UnkSceneStuff sp110;
    s32 chunkID;
    s32 temp_s1;
    s32 formationIndex;
    s32 var_s3;
    s32* var_s2;
    s32* var_s5;
    s32* var_s3_2;

    var_s5 = (s32*)0x801C0000;
    chunkID = sceneID / 4;
    temp_s1 = func_801B2738(chunkID); // sector modified based on the Chunk ID
    func_80033E34(                    // load file from disk
        func_800144D8(BATTLE_SCENE) +
            temp_s1 * 4, // Disk sector where to load the file from
        0x800 * 4,       // Size in bytes to copy
        (u_long*)var_s5, // Destination
        NULL);
    formationIndex = chunkID - D_80083184[temp_s1];
    func_800145BC(cb); // wait until all data is read, keep executing the vsync
                       // callback until then
    var_s3 = var_s5[formationIndex];
    var_s3_2 = &var_s5[var_s3];
    var_s2 = (s32*)sp110.unk0;
    func_80017108( // gzip decompress
        var_s3_2,  // src
        var_s2);   // dst
    formationIndex = sceneID - chunkID * 4;
    func_80014A00(D_8016360C.unk0, sp110.unk0, sizeof(sp110.unk0));
    func_80014A00((s32*)&D_8016360C.unk8, &sp110.unk8[formationIndex],
                  sizeof(Unk8016360C_1));
    func_80014A00((s32*)&D_8016360C.unk1C, &sp110.unk58[formationIndex],
                  sizeof(Unk8016360C_2));
    func_80014A00((s32*)&D_8016360C.unk4C, &sp110.unk118[formationIndex],
                  sizeof(Unk8016360C_3));
    func_80014A00(
        (s32*)&D_800F5F44.enemy, &sp110.sp3A8, sizeof(Unk800F5F44_1) * 3);
    func_80014A00((s32*)&D_800F5F44._2, &sp110.sp5D0, sizeof(Unk800F5F44_2));
    func_80014A00((s32*)&D_800F5F44._3, &sp110.sp950, sizeof(Unk800F5F44_3));
    func_80014A00((s32*)&D_800F5F44._4, &sp110.sp990, sizeof(Unk800F5F44_4));
    func_80014A00((s32*)&D_800F5F44._5, &sp110.spD90, sizeof(Unk800F5F44_5));
    func_80014A00((s32*)&D_800F5F44._6, &sp110.spF90, sizeof(Unk800F5F44_6));
    if (D_8016376A & 4 && D_8016360C.unk8.D_80163624 & 0x10) {
        if (D_8016360C.unk8.D_80163626 == 0) {
            D_8016360C.unk8.D_80163626 = 1;
        }
    }
    D_800F5F44.D_800F7DC8 = (u8)D_801B0044[D_8016360C.unk8.D_80163626];
    if (D_8016376A & 0x40) {
        D_8016360C.unk8.D_80163614 = 0x25;
        D_8016360C.unk8.D_80163624 |= 4;
        D_8016360C.unk8.D_80163627 = (func_80014B70() & 3) + 0x60;
        D_8016360C.unk8.D_80163618 = 1;
        for (var_s3 = 0; var_s3 < 3; var_s3++) {
            D_800F5F44.enemy[var_s3].unk90[5] *= 2;
            D_800F5F44.enemy[var_s3].strength =
                func_801B2770(D_800F5F44.enemy[var_s3].strength);
            D_800F5F44.enemy[var_s3].magic =
                func_801B2770(D_800F5F44.enemy[var_s3].magic);
        }
    } else if (D_8016376A & 8) {
        D_8016360C.unk8.D_80163624 &= ~4;
    }
    if (!(D_8016360C.unk8.D_80163624 & 4)) {
        D_8016376A |= 8;
    }
    D_800F5F44.D_800F7DB2 = D_8016360C.unk8.D_80163618;
    if (D_800F5F44.D_800F7DC8 == 1 || D_800F5F44.D_800F7DC8 == 3) {
        D_800F5F44.D_800F7DB2 = 1;
    }
    D_800F5F44.D_800F7DB6 = D_800F5F44.D_800F7DB2;
}

s32 func_801B2738(s32 arg0) {
    u32 i;

    for (i = 1; i < LEN(D_80083184); i++) {
        if (arg0 < D_80083184[i]) {
            break;
        }
    }
    return i - 1;
}

s32 func_801B2770(s32 arg0) {
    arg0 = (arg0 * 125) / 100;
    if (arg0 > 255) {
        arg0 = 255;
    }
    return arg0;
}
