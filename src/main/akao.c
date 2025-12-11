//! PSYQ=3.3 CC1=2.6.3

#include "common.h"
#include "psxsdk/libspu.h"

typedef struct {
    s32 unk0;
    s8 unk4;
} Unk8002C5A8;

typedef struct {
    u8* addr;
    u8* loop_addr[4];
    u8* drum_addr;
    u8 pad1[0xC];
    u32 overlay_voice;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s8 unk3C[0x24];
    s16 unk60;
    s16 unk62;
    s16 unk64;
    u16 unk66;
    u32 unk68;
    s16 unk6C;
    s16 unk6E;
    u8 unk70[0x20];
    s16 unk90;
    s16 unk92;
    s16 unk94;
    s16 unk96;
    s16 unk98;
    s16 unk9A;
    s16 unk9C;
    s16 unk9E;
    s16 unkA0;
    s16 unkA2;
    s16 unkA4;
    s16 unkA6;
    s16 unkA8;
    s16 unkAA;
    s16 unkAC;
    s16 unkAE;
    u8 unkB0[0x10];
    u8 unkC0[0x2];
    u16 unkC2;
    u16 unkC4;
    u8 unkC6[0x6];
    s16 unkCC;
    s16 unkCE;
    s32 unkD0;
    s16 unkD4;
    s16 unkD6;
    s16 unkD8;
    s16 unkDA;
    s32 unkDC;
    s32 unkE0;
} AKAO_TRACK;

typedef struct {
    u8 unk0[0x48];
    u16 tempo_slide_length;
    u16 song_id;
    u16 last_condition;
    u16 condition;
    u16 reverb_depth_slide_length;
    u16 noise_clock;
    u16 field_54;
    u16 beats_per_measure;
    u16 beat;
    u16 ticks_per_beat;
    u16 tick;
    u16 measure;
} AKAO_CONFIG;

extern s16 D_80062F40;
extern s16 D_80062F48;
extern s32 D_80062FE4;
extern s32 D_80062FE8;
extern s16 D_8008337E;
extern s16 D_800833DE;
extern s32 D_80099788;
extern s32 D_80099998;
extern u8 D_80099BA8[];
extern s32 D_80099DB8;

extern u32 g_ReverbMode;
extern SpuReverbAttr g_ReverbAttr;

#define READ_S8(addr) ((s8)(*(addr)++))
#define READ_S16(addr) ((s16)(*(addr)++ | (*(addr)++ << 8)))

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800293D0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800293F4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029424);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029464);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800294A4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800294BC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800297A4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029818);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002988C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029998);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800299C8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029A50);

void SetReverbMode(s32 in_ReverbMode) {
    func_80029A50();
    SpuGetReverbModeParam(&g_ReverbAttr);
    if (g_ReverbAttr.mode != in_ReverbMode) {
        g_ReverbMode = in_ReverbMode;
        SpuSetReverb(SPU_OFF);
        g_ReverbAttr.mode = in_ReverbMode | SPU_REV_MODE_CLEAR_WA;
        g_ReverbAttr.mask = SPU_REV_MODE;
        SpuSetReverbModeParam(&g_ReverbAttr);
        SpuSetReverb(SPU_ON);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029B78);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029BAC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029C48);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029E98);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80029F44);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A094);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A28C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A43C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A510);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A6C4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A748);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A798);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A7E8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A958);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002AABC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002AFB8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B1A8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B1F8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B2F8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B3B4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B5A8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B608);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B668);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B6AC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B730);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B7E0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B8B4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B904);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B958);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B9AC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BA08);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BA5C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BA98);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BB20);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BBB4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BBEC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BC58);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BCCC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BD04);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BDCC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BE2C);

void func_8002BE8C(s32 arg0) { func_8002BCCC(arg0, D_80099BA8); }

void func_8002BEB4(s32 arg0) { func_8002BD04(arg0, D_80099BA8); }

void func_8002BEDC(s32 arg0) { func_8002BCCC(arg0, &D_80099998); }

void func_8002BF04(s32 arg0) { func_8002BD04(arg0, &D_80099998); }

void func_8002BF2C(s32 arg0) { func_8002BCCC(arg0, &D_80099788); }

void func_8002BF54(s32 arg0) { func_8002BD04(arg0, &D_80099788); }

void func_8002BF7C(s32 arg0) { func_8002BCCC(arg0, &D_80099DB8); }

void func_8002BFA4(s32 arg0) { func_8002BD04(arg0, &D_80099DB8); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BFCC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C004);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C0CC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C12C);

void func_8002C18C(s32 arg0) { func_8002BFCC(arg0, D_80099BA8); }

void func_8002C1B4(s32 arg0) { func_8002C004(arg0, D_80099BA8); }

void func_8002C1DC(s32 arg0) { func_8002BFCC(arg0, &D_80099998); }

void func_8002C204(s32 arg0) { func_8002C004(arg0, &D_80099998); }

void func_8002C22C(s32 arg0) { func_8002BFCC(arg0, &D_80099788); }

void func_8002C254(s32 arg0) { func_8002C004(arg0, &D_80099788); }

void func_8002C27C(s32 arg0) { func_8002BFCC(arg0, &D_80099DB8); }

void func_8002C2A4(s32 arg0) { func_8002C004(arg0, &D_80099DB8); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C2CC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C300);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C3A8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C408);

void func_8002C468(s32 arg0) { func_8002C2CC(arg0, D_80099BA8); }

void func_8002C490(s32 arg0) { func_8002C300(arg0, D_80099BA8); }

void func_8002C4B8(s32 arg0) { func_8002C2CC(arg0, &D_80099998); }

void func_8002C4E0(s32 arg0) { func_8002C300(arg0, &D_80099998); }

void func_8002C508(s32 arg0) { func_8002C2CC(arg0, &D_80099788); }

void func_8002C530(s32 arg0) { func_8002C300(arg0, &D_80099788); }

void func_8002C558(s32 arg0) { func_8002C2CC(arg0, &D_80099DB8); }

void func_8002C580(s32 arg0) { func_8002C300(arg0, &D_80099DB8); }

void func_8002C5A8(Unk8002C5A8* arg0) {
    s32 n = arg0->unk4;
    D_80062F48 = 0;
    D_80062FE8 = n << 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C5C8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C634);

void func_8002C6A8(Unk8002C5A8* arg0) {
    s32 n = arg0->unk4;
    D_80062F40 = 0;
    D_80062FE4 = n << 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C6C8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C734);

void func_8002C7A8(void) { func_80029F44(); }

void func_8002C7C8(void) { func_8002A43C(); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C7E8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C81C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C850);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C884);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C8C4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C8DC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C9E4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CA84);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CB78);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CC18);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CC44);

void func_8002CCBC(void) { D_8008337E = 0; }

void func_8002CCCC(void) { D_800833DE = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CCDC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CDD0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CEC0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CF1C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CF78);

void func_8002CF98(void) {}

void func_8002CFA0() { SpuSetTransferCallback(0); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CFC0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D1E4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D2D4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D410);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D4A0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D530);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D668);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D7A0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D8E8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002DA30);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002DA7C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002DF88);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002E1A8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002E23C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002E428);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002E478);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002E954);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002ED34);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002F24C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002F738);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002F848);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002FDA0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002FE48);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002FF4C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030038);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030148);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030234);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030380);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800308D4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030E7C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031820);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800318BC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031A70);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031AB0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031AFC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031BA0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031BE4);

void func_80031C88(AKAO_TRACK* track) {
    track->unk2C = *track->addr++;
    track->unkE0 |= 3;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031CB0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031CE0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031D6C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031E98);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031EEC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031F30);

void func_80031FC0(AKAO_TRACK* track) {
    track->unk60 = *track->addr++ << 8;
    track->unk62 = 0;
    track->unkE0 |= 3;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80031FF0);

void func_80032078(AKAO_TRACK* track) { track->unk66 = *track->addr++; }

void func_80032094(AKAO_TRACK* track) {
    track->unk66 = (track->unk66 + 1) & 0xF;
}

void func_800320AC(AKAO_TRACK* track) {
    track->unk66 = (track->unk66 + 0xFFFF) & 0xF;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800320C4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032274);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800323CC);

void func_800324D8(AKAO_TRACK* track) { track->unkCC = (s8)*track->addr++; }

void func_80032500(AKAO_TRACK* track) {
    track->unkCC = (s8)*track->addr++ + track->unkCC;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8003252C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8003257C);

void func_800325B8(AKAO_TRACK* track) { track->unk6C = 0; }

void func_800325C0(AKAO_TRACK* track) { track->unkCE = (s8)*track->addr++; }

void func_800325E8(AKAO_TRACK* track) {
    track->unkCE = (s8)*track->addr++ + track->unkCE;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032614);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032718);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032770);

void func_800327E0(AKAO_TRACK* track) {
    track->unkD6 = 0;
    track->unk38 &= ~1;
    track->unkE0 |= 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032804);

void func_800328D4(AKAO_TRACK* track) { track->unk90 = *track->addr++ << 8; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800328F8);

void func_80032968(AKAO_TRACK* track) {
    track->unkD8 = 0;
    track->unk38 &= ~2;
    track->unkE0 |= 3;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8003298C);

void func_80032A04(AKAO_TRACK* track) { track->unk9E = *track->addr++ << 7; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032A28);

void func_80032A98(AKAO_TRACK* track) {
    track->unkDA = 0;
    track->unk38 &= ~4;
    track->unkE0 |= 3;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032ABC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032B30);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032BB4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032C20);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032C8C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032CE8);

void func_80032D44(AKAO_TRACK* track) { track->unk6E = 1; }

void func_80032D50(void) {}

void func_80032D58(AKAO_TRACK* track) { track->unk6E = 4; }

void func_80032D64(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032D6C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032E08);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032E6C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032ED0);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032F34);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032F98);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80032FFC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033060);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800330C4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033128);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800331CC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033224);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033264);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_800332EC);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8003337C);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033420);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8003345C);

void func_80033488(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    short delta = READ_S8(track->addr);
    if (delta != 0) {
        delta += track->unkC2;
        if (delta < 1) {
            delta = 1;
        } else if (delta > 255) {
            delta = 255;
        }
    }
    track->unkC4 = delta;
}

void func_800334EC(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->drum_addr = track->addr + READ_S16(track->addr);
    track->unk38 |= 0x8;
}

void func_80033534(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 &= ~0x8;
}

void func_80033548(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    config->ticks_per_beat = *track->addr++;
    config->beats_per_measure = *track->addr++;
    config->tick = 0;
    config->beat = 0;
}

void func_80033588(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    config->measure = *track->addr++;
    config->measure |= *track->addr++ << 8;
}

void func_800335CC(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    config->field_54 = 1;
}

void func_800335D8(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    func_80032E6C(track, config, mask);
    func_80032ED0(track, config, mask);
}

void func_80033628(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    int delay = *track->addr++;
    if (delay == 0) {
        track->unkA4 = 257;
    } else {
        track->unkA4 = delay + 1;
    }
    func_80032ABC(track, config, mask);
}

void func_8003366C(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    s16 var_v0 = *track->addr++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->unkA4 = var_v0;
}

void func_80033698(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    int delay = *track->addr++;
    if (delay == 0) {
        track->unkA6 = 257;
    } else {
        track->unkA6 = delay + 1;
    }
    func_80032BB4(track, config, mask);
}

void func_800336DC(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    s16 var_v0 = *track->addr++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->unkA6 = var_v0;
}

void func_80033708(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 &= ~0x37;
    func_80032B30(track, config, mask);
    func_80032C20(track, config, mask);
    func_80032CE8(track, config, mask);
    track->unk6E &= ~0x5;
}

void func_80033788(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 |= 0x10;
}

void func_8003379C(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 &= ~0x10;
}

void func_800337B0(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 |= 0x20;
}

void func_800337C4(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->unk38 &= ~0x20;
}

void func_800337D8(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    track->addr += READ_S16(track->addr);
}

void func_80033818(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    int cond = *track->addr++;
    if (config->condition != 0 && cond <= config->condition) {
        track->addr += READ_S16(track->addr);
        config->last_condition = cond;
    } else {
        track->addr += 2;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033894);

void func_80033A70(AKAO_TRACK* track, AKAO_CONFIG* config, u32 mask) {
    func_80033894(track, config, mask);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80033A90);
