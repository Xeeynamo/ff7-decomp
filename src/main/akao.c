//! PSYQ=3.3 CC1=2.6.3 COMM=true

#include "common.h"
#include "game.h"
#include "libspu.h"

// 16.16 fixed point volume
typedef union {
    s32 val;
    struct {
        s16 lo;
        s16 hi;
    } i;
} AkaoCdVol; /* size = 0x4 */

typedef struct {
    s32 unk0;
    s8 unk4;
    s8 pad5[3];
    s32 unk8;
    s8 unkC;
} Unk8002C5A8;

// Field names cross-checked against the independent qgears reverse-engineering
// project's AkaoChannel struct (same source as the g_Akao*SlideStep/Steps
// naming above): https://github.com/Akari1982/q-gears_reverse,
// ffvii/DISC/SCUS_941_akao.h. Spans neither this repo nor qgears resolves
// (LFO delay/rate sub-fields, the 0xA8-0xB8 gap) are left as unkNN.
typedef struct {
    /* 0x0 */ u8* akaoSequencePointer;
    /* 0x4 */ u8* loopPoint[4];
    /* 0x14 */ u8* drumOffset;
    /* 0x18 */ u32 frequencyLfoWaveTableAddr;
    /* 0x1C */ u32 volumeLfoWaveTableAddr;
    /* 0x20 */ u32 volumePanLfoWaveTableAddr;
    /* 0x24 */ u32 overlayChannelId;
    /* 0x28 */ s32 alternativeChannelId;
    /* 0x2C */ s32 volumeMultiplier;
    /* 0x30 */ s32 basePitch;
    /* 0x34 */ s32 pitchRelated;
    /* 0x38 */ s32 updateMirror;
    /* 0x3C */ u32 pitchModifier;
    /* 0x40 */ s32 unk40;
    /* 0x44 */ s32 volumeLevel;
    /* 0x48 */ s32 volumeLevelNew;
    /* 0x4C */ s32 pitchGrowth;
    /* 0x50 */ u32 setToMinusOne;
    /* 0x54 */ u16 playingType;
    /* 0x56 */ u8 pause_length1;
    /* 0x57 */ u8 pause_length2;
    /* 0x58 */ u16 currentInstrument;
    /* 0x5A */ u16 unk5A;
    /* 0x5C */ s16 initWith0_5C;
    /* 0x5E */ s16 unk5E;
    /* 0x60 */ s16 baseVolumePan;
    /* 0x62 */ s16 setTo0_62;
    /* 0x64 */ s16 pitchChangeSteps;
    /* 0x66 */ u16 pitchCorrection;
    /* 0x68 */ u16 pitchSlideSpeed;
    /* 0x6A */ u16 unk6A;
    /* 0x6C */ s16 initWith0_6C;
    /* 0x6E */ s16 initWith0_6E;
    /* 0x70 */ u8 unk70[0xE];
    /* 0x7E */ s16 frequencyLfoMultiplierInit;
    /* 0x80 */ s16 depthFadeSpeed;
    /* 0x82 */ u8 unk82[0xE];
    /* 0x90 */ s16 volumeLfoMultiplier;
    /* 0x92 */ s16 initWith0_92;
    /* 0x94 */ s16 unk94;
    /* 0x96 */ s16 unk96;
    /* 0x98 */ s16 volumePanLfoRefreshInterval;
    /* 0x9A */ s16 volumePanLfoRefreshIntervalCounter;
    /* 0x9C */ s16 volumePanLfoTableKeyNodeIndex;
    /* 0x9E */ s16 initWith0_9E;
    /* 0xA0 */ s16 initWith0_A0;
    /* 0xA2 */ s16 unkA2;
    /* 0xA4 */ s16 noiseSwitchDelay;
    /* 0xA6 */ s16 pitchLfoSwitchDelay;
    /* 0xA8 */ s16 unkA8;
    /* 0xAA */ s16 unkAA;
    /* 0xAC */ s16 unkAC;
    /* 0xAE */ s16 unkAE;
    /* 0xB0 */ u8 unkB0[0x8];
    /* 0xB8 */ u16 lastSavedLoopPointIndex;
    /* 0xBA */ u16 loopPointState[0x4];
    /* 0xC2 */ u16 pauseStorage;
    /* 0xC4 */ u16 pauseMultiplier;
    /* 0xC6 */ s16 volumeMultiplier2;
    /* 0xC8 */ s16 unkC8;
    /* 0xCA */ s16 unkCA;
    /* 0xCC */ s16 absoluteTransposition;
    /* 0xCE */ s16 frequencyMultiplier;
    /* 0xD0 */ s16 pitchSavedParameters;
    /* 0xD2 */ s16 pitchSlideDestination;
    /* 0xD4 */ s16 unkD4;
    /* 0xD6 */ s16 pitchLfoValue;
    /* 0xD8 */ s16 volumeLfoValue;
    /* 0xDA */ s16 volumePanLfoValue;
    /* 0xDC */ s32 unkDC;
    /* 0xE0 */ s32 setFlags;
    /* 0xE4 */ u8 padE4[0x24];
} AkaoChannel;

typedef struct {
    /* 0x0 */ u8 pad0[0x2C];
    /* 0x2C */ u32 noiseMusicVoicesMask;
    /* 0x30 */ u32 reverbMusicVoicesMask;
    /* 0x34 */ u32 pitchLfoMusicVoicesMask;
    /* 0x38 */ u8 pad38[0x10];
    /* 0x48 */ u16 tempoMusicIncreaseSteps;
    /* 0x4A */ u16 akaoMusicId;
    /* 0x4C */ u16 jumpConditionStorage1;
    /* 0x4E */ u16 jumpConditionStorage2;
    /* 0x50 */ u16 reverbDepthIncrementSteps;
    /* 0x52 */ u16 noiseClock;
    /* 0x54 */ u16 unk54;
    /* 0x56 */ u16 upperTimerEqualValue;
    /* 0x58 */ u16 upperTimerValue;
    /* 0x5A */ u16 lowerTimerEqualValue;
    /* 0x5C */ u16 lowerTimerValue;
    /* 0x5E */ u16 topTimer;
} AkaoConfig;

typedef struct {
    /* 0x0 */ u8 unk0;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ s16 unk2;
    /* 0x4 */ s32 unk4;
    /* 0x8 */ s32 unk8;
    /* 0xC */ u16 unkC;
    /* 0xE */ u16 unkE;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
} Unk8002B7E0; // size:0x24

extern void (*D_80049548[])(Unk8002B7E0*);
extern u8 D_800499A8[]; // opcode lenghts
extern u8 D_80049C40[];
extern s32 g_AkaoWaveTableKey[];
s32 g_AkaoStreamMask;
s32 D_80062F08;
u16 D_80062F1E;
// Music-driver slide state: each MulMusic value is a fixed-point scalar for
// pitch/volume/tempo (current value in the upper 16 bits, lower 16 bits are
// fractional precision the driver accumulates every tick for a smooth
// ramp); *SlideStep is the per-tick delta added to it, *SlideSteps is the
// remaining tick count. Names/meaning confirmed one-off against the
// independent qgears reverse-engineering project (not part of this repo):
// https://github.com/q-gears/q-gears, src/main/SCUS_941_akao.cpp.
s32 g_AkaoPitchMulMusicSlideStep;
s32 g_AkaoVolMulMusicSlideStep;
s32 g_AkaoTempoMulMusicSlideStep;
s16 g_AkaoPitchMulMusicSlideSteps;
s16 g_AkaoVolMulMusicSlideSteps;
s16 g_AkaoTempoMulMusicSlideSteps;
s32 g_AkaoVolMulMusic;
u16 g_AkaoReverbPan;
s32 g_AkaoEffectsAll;
s32 g_AkaoEffectsAllSeq;
s32 g_AkaoMutex;
s32 D_80062FAC;
s32 D_80062FB0;
s32 g_AkaoCdVolSlideStep;
u16 g_AkaoReverbMul;
u16 g_AkaoCdVolSlideSteps;

extern AkaoCdVol g_AkaoCdVol;
extern s32 D_80062FE0;
extern s32 g_AkaoPitchMulMusic;
extern s32 g_AkaoTempoMulMusic;
extern s32 g_AkaoControlFlags;
extern s32 D_80063000;
extern u32 D_80063004;
extern s32 g_AkaoCommandQueueId; // sound message queue count
extern u8 D_800716CC;
extern u8 g_AkaoVoiceAttr[];
extern s32 g_AkaoVoiceAttrMask;
extern s32 D_8007EBEC;
extern s32 D_8007EBF0;
extern s32 D_8007EBF4;
extern s32 D_8007EBF8;
extern s32 D_8007EBFC;
extern u16 D_8007EC00;
extern u16 D_8007EC02;
extern u16 D_8007EC04;
extern u16 D_8007EC06;
extern u16 D_8007EC08;
extern u16 D_8007EC0A;
extern s16 D_8007EC0C;
extern s16 D_8007EC0E;
extern s32 D_8007EC10;
extern s32 D_800804D0;
extern Unk8002B7E0 D_80081DC8[]; // sound messages queue
extern s32 D_80083334;
extern u16 D_8008337E;
extern s32 D_80083394;
extern u16 D_800833DE;
extern s32 D_80083580[];
extern AkaoChannel g_Channel1[];
extern s32 D_80097768;
extern s32 D_80097870;
extern AkaoChannel g_Channel3[];
extern u16 D_80099E0C;
extern s32 g_Channel3ActiveMask[];
extern s32 g_Channel3OffMask;
extern s32 g_AkaoSoundActiveMaskStored;
extern s32 g_AkaoNoiseMask;
extern s32 g_AkaoReverbMask;
extern s32 g_AkaoPitchLfoMask;
extern u16 D_8009A14E;
extern s32 g_Channel1Config;
extern s32 g_AkaoMusicActiveMask;
extern s32 D_8009A10C;
extern s32 D_8009A110;
extern s32 D_8009A114;
extern s32 g_AkaoMusicActiveMaskStored;
extern s32 g_AkaoMusicOverMask;
extern s32 g_AkaoMusicAltMask;
extern s32 D_8009A13C;
extern u32 g_ReverbMode;
extern SpuReverbAttr g_ReverbAttr;
extern SpuCommonAttr g_SpuCommonAttr;

#define READ_S8(addr) ((s8)(*(addr)++))
#define READ_S16(addr) ((s16)(*(addr)++ | (*(addr)++ << 8)))

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSpuTransferComplete);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSpuTransferPrep);

static void AkaoSpuWrite(s32 addr, s32 size) {
    AkaoSpuTransferPrep();
    SpuWrite(addr, size);
}

static void AkaoSpuRead(s32 addr, s32 size) {
    AkaoSpuTransferPrep();
    SpuRead(addr, size);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSpuTransferSync);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoInitData);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadInstr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadInstr2);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoStart);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadEffect);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoDeinit);

// Key off the voices in g_AkaoStreamMask and clear the SPU transfer/IRQ callbacks.
static void func_80029A50(void) {
    SpuSetTransferCallback(0);
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    SpuSetKey(0, g_AkaoStreamMask);
    if (g_AkaoStreamMask & 0x10000) {
        D_80097768 = 0x1FF93;
    }
    if (g_AkaoStreamMask & 0x20000) {
        D_80097870 = 0x1FF93;
    }
    g_AkaoStreamMask = 0;
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
    AkaoUpdateNoiseVoices();
}

static void SetReverbMode(s32 in_ReverbMode) {
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

// Word-copies (arg1 >> 2) words from arg0 into staging buffer D_80083580.
static void AkaoCopyMusic(s32* arg0, u32 arg1) {
    s32* dst;
    u32 nwords;

    nwords = arg1 >> 2;
    dst = D_80083580;
    while (nwords != 0) {
        nwords -= 1;
        *dst = *arg0;
        arg0 += 1;
        dst += 1;
    }
}

void AkaoInstrInit(AkaoChannel*, s32);

static void SoundChannelInit(AkaoChannel* arg0, u8* arg1) {
    arg0->akaoSequencePointer = arg1;
    arg0->volumeMultiplier = 0x78;
    AkaoInstrInit(arg0, 5);
    arg0->pitchCorrection = 2;
    arg0->frequencyMultiplier = 0;
    arg0->absoluteTransposition = 0;
    arg0->initWith0_6C = 0;
    arg0->pitchRelated = 0;
    arg0->pitchSlideDestination = 0;
    arg0->pauseMultiplier = 0;
    arg0->pauseStorage = 0;
    arg0->pitchChangeSteps = 0;
    arg0->volumeLevel = 0x32000000;
    arg0->initWith0_5C = 0;
    arg0->updateMirror = 0;
    arg0->lastSavedLoopPointIndex = 0;
    arg0->initWith0_6E = 0;
    arg0->volumePanLfoValue = 0;
    arg0->initWith0_9E = 0;
    arg0->volumeLfoMultiplier = 0;
    arg0->frequencyLfoMultiplierInit = 0;
    arg0->initWith0_A0 = 0;
    arg0->initWith0_92 = 0;
    arg0->depthFadeSpeed = 0;
    arg0->pitchLfoSwitchDelay = 0;
    arg0->noiseSwitchDelay = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicChannelsInit);

// Merges newly-requested bits (g_AkaoMusicOverMask/g_AkaoMusicAltMask) into the
// pending mask g_AkaoMusicActiveMask, then for each set bit points the matching
// g_Channel1 slot at the default D_80049C40 sample and marks it (unk56 =
// 0x204), clearing the request bits as it goes.
static void AkaoMusicStopChannels1(void) {
    s32 mask;
    s32 bit;
    AkaoChannel* slot;
    s32 req0;
    s32 req1;

    if (g_AkaoMusicActiveMask != 0) {
        slot = g_Channel1;
        bit = 1;
        req0 = g_AkaoMusicOverMask;
        req1 = g_AkaoMusicAltMask;
        g_AkaoMusicAltMask = 0;
        g_AkaoMusicOverMask = 0;
        D_8009A110 = 0;
        D_8009A10C = 0;
        req0 |= req1;
        mask = g_AkaoMusicActiveMask;
        mask |= req0;
        g_AkaoMusicActiveMask = mask;
        D_8009A114 |= mask;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u16*)&slot->pause_length1 = 0x204;
                slot->akaoSequencePointer = D_80049C40;
            }
            bit *= 2;
            slot += 1;
        } while (mask != 0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicStopChannels12);

void AkaoSoundChannelsInit(u16 arg0, s32 arg1, s32 arg2, s32 arg3);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundMenuChannelsInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsStop);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsClear);

// Resolves a 10-bit note index into a pair of table entries: looks up
// g_AkaoEffectsAll[index] and g_AkaoEffectsAll[index+1] (u16), adding g_AkaoEffectsAllSeq unless the
// entry is the 0xFFFF "unused" sentinel (in which case the result is 0).
static void AkaoSoundGetSequence(s32* arg0, s32* arg1, u16 arg2) {
    u16 idx;
    s32 val0;
    s32 val1;
    u16 raw0;
    u16 raw1;

    idx = (arg2 & 0x3FF) * 2;
    raw0 = *(u16*)((idx * 2) + g_AkaoEffectsAll);
    if (raw0 != 0xFFFF) {
        val0 = raw0 + g_AkaoEffectsAllSeq;
    } else {
        val0 = 0;
    }
    *arg0 = val0;
    idx = idx + 1;
    raw1 = *(u16*)((idx * 2) + g_AkaoEffectsAll);
    if (raw1 != 0xFFFF) {
        val1 = raw1 + g_AkaoEffectsAllSeq;
    } else {
        val1 = 0;
    }
    *arg1 = val1;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicVolReset);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundVolReset);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A7E8);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002A958);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicRestoreChannelsAndConfig);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicCopyChannels1Into2);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002B1A8);

void AkaoCmd_10(Unk8002B7E0* arg0) {
    AkaoCopyMusic(arg0->unk4, arg0->unk8);
    if (D_8009A14E == 0xE) {
        func_8002A7E8();
        func_8002B1A8(&g_Channel1, &D_800804D0, &g_Channel1Config, &D_80083394);
    }
    AkaoMusicStopChannels1();
    if (D_8008337E && D_8008337E == arg0->unkC) {
        AkaoMusicRestoreChannelsAndConfig(0);
    } else if (D_800833DE && D_800833DE == arg0->unkC) {
        AkaoMusicRestoreChannelsAndConfig(1);
    } else {
        AkaoMusicChannelsInit();
    }
    D_8009A14E = arg0->unkC;
}

void AkaoCmd_14(Unk8002B7E0* arg0) {
    s32* var_a2;

    AkaoCopyMusic(arg0->unk4, arg0->unk8);
    func_8002A7E8();
    var_a2 = &g_Channel1Config;
    if (D_8009A14E) {
        if (D_8009A14E == 0xE) {
            func_8002B1A8(&g_Channel1, &D_800804D0, var_a2, &D_80083394);
        } else {
            func_8002B1A8(&g_Channel1, &D_8007EC10, var_a2, &D_80083334);
        }
    }
    AkaoMusicStopChannels1();
    AkaoMusicChannelsInit();
    D_8009A14E = arg0->unkC;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_15);

extern u16 D_80062FC8;

void AkaoCmd_18(Unk8002B7E0* arg0) {
    if (D_8009A14E) {
        D_80062FC8 = arg0->unk10 ? arg0->unk10 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_10(arg0);
}

void AkaoCmd_19(Unk8002B7E0* arg0) {
    if (D_8009A14E) {
        D_80062FC8 = arg0->unk10 ? arg0->unk10 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_14(arg0);
}

void AkaoCmd_34(Unk8002B7E0* arg0) {
    AkaoSoundChannelsClear(4, 1);
    AkaoSoundChannelsInit(0x40, 0x34, arg0->unk4, arg0->unk8);
}

void AkaoCmd_21(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(4, 2);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x32, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unkC);
    AkaoSoundChannelsInit(arg0->unk4, 0x34, sp10, sp14);
}

void AkaoCmd_22(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(4, 3);
    func_80029A50();
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x30, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unkC);
    AkaoSoundChannelsInit(arg0->unk4, 0x32, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk10);
    AkaoSoundChannelsInit(arg0->unk4, 0x34, sp10, sp14);
}

void AkaoCmd_23(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(6, 4);
    func_80029A50();
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x30, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unkC);
    AkaoSoundChannelsInit(arg0->unk4, 0x32, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk10);
    AkaoSoundChannelsInit(arg0->unk4, 0x34, sp10, sp14);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk14);
    AkaoSoundChannelsInit(arg0->unk4, 0x36, sp10, sp14);
}

void AkaoCmd_30(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk4);
    AkaoSoundMenuChannelsInit(sp10, sp14);
}

void AkaoCmd_20(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(4, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x34, sp10, sp14);
}

void AkaoCmd_29(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(2, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x32, sp10, sp14);
}

void AkaoCmd_2A(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(0, 1);
    func_80029A50();
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x30, sp10, sp14);
}

void AkaoCmd_2B(Unk8002B7E0* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->unk8);
    AkaoSoundChannelsInit(arg0->unk4, 0x36, sp10, sp14);
}

void AkaoC0VolumeSet(Unk8002B7E0* arg0) {
    g_AkaoVolMulMusicSlideSteps = 0;
    g_AkaoVolMulMusic = (arg0->unk4 & 0x7F) << 0x10;
    AkaoMusicVolReset();
}

typedef struct {
    u32 unk0;
    s32 unk4;
    s32 unk8;
} Unk8002BA98;

// Starts a volume slide from the current g_AkaoVolMulMusic toward a target
// derived from arg0, over arg0's tick count.
void AkaoC1VolumeSlideFromCurrent(Unk8002BA98* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoVolMulMusicSlideSteps = var_a1;
    g_AkaoVolMulMusicSlideStep = (((arg0->unk8 & 0x7F) << 0x10) - g_AkaoVolMulMusic) / var_a1;
    AkaoMusicVolReset();
}

typedef struct {
    u32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} Unk8002BB20;

// Starts a volume slide between two explicit targets from arg0 (rather than
// from the current g_AkaoVolMulMusic), over arg0's tick count.
void AkaoC2VolumeSlideBetweenTargets(Unk8002BB20* arg0) {
    s32 temp_v1;
    s32 var_a1;
    s32 temp_v0;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v0 = (arg0->unkC & 0x7F) << 0x10;
    temp_v1 = (arg0->unk8 & 0x7F) << 0x10;
    g_AkaoVolMulMusicSlideSteps = var_a1;
    g_AkaoVolMulMusic = temp_v1;
    g_AkaoVolMulMusicSlideStep = (temp_v0 - temp_v1) / var_a1;
    AkaoMusicVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_C8);

typedef struct {
    u32 unk0;
    s32 unk4;
    u16 unk8;
} Unk8002BBEC;

// Starts a CD-audio volume slide from the current g_AkaoCdVol toward a
// target derived from arg0, over arg0's tick count.
void AkaoC9CdVolumeSlideFromCurrent(Unk8002BBEC* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoCdVolSlideSteps = var_a1;
    g_AkaoCdVolSlideStep = ((arg0->unk8 << 0x10) - g_AkaoCdVol.val) / var_a1;
}

typedef struct {
    u32 unk0;
    s32 unk4;
    u16 unk8;
    u16 unkA;
    u16 unkC;
    u16 unkE;
} Unk8002BC58;

// Starts a CD-audio volume slide between two explicit targets from arg0
// (rather than from the current g_AkaoCdVol), over arg0's tick count.
void AkaoCACdVolumeSlideBetweenTargets(Unk8002BC58* arg0) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a1;
    s32 temp_v0_shifted;
    s32 temp_v1_shifted;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v0_shifted = arg0->unkC << 0x10;
    temp_v1_shifted = arg0->unk8 << 0x10;
    g_AkaoCdVolSlideSteps = var_a1;
    g_AkaoCdVol.val = temp_v1_shifted;
    g_AkaoCdVolSlideStep = (temp_v0_shifted - temp_v1_shifted) / var_a1;
}

// The voice record at arg1 is two identical-layout 0x108-byte halves. Write the
// note's transposed pitch (arg0+4) into a field in each half, clear another
// field in each half, and set flag bits 0x3 in each half's control word
// (+0xE0).
static void func_8002BCCC(void* arg0, void* arg1) {
    u16 val0;
    s32 v1;
    s32 v0_e0;
    AkaoChannel* voice = (AkaoChannel*)arg1;
    // The do{}while(0) affects register allocation and is required for the
    // match.
    do {
        val0 = *((u16*)((u8*)arg0 + 0x4));
        v1 = voice[1].setFlags;
        voice[1].unk5E = 0;
        voice[0].unk5E = 0;
        voice[1].volumeMultiplier2 = (s16)((val0 & 0x7F) << 8);
    } while (0);
    voice[0].volumeMultiplier2 = (s16)((val0 & 0x7F) << 8);
    v0_e0 = voice[0].setFlags;
    voice[1].setFlags = v1 | 3;
    voice[0].setFlags = v0_e0 | 3;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002BD04);

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_B8(void* arg0) {
    func_8002BCCC(arg0, &g_Channel3[6]);
    func_8002BCCC(arg0, &g_Channel3[4]);
    func_8002BCCC(arg0, &g_Channel3[2]);
    func_8002BCCC(arg0, &g_Channel3[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_B9(void* arg0) {
    func_8002BD04(arg0, &g_Channel3[6]);
    func_8002BD04(arg0, &g_Channel3[4]);
    func_8002BD04(arg0, &g_Channel3[2]);
    func_8002BD04(arg0, &g_Channel3[0]);
}

void AkaoCmd_A0(void* arg0) { func_8002BCCC(arg0, &g_Channel3[4]); }

void AkaoCmd_A4(s32 arg0) { func_8002BD04(arg0, &g_Channel3[4]); }

void AkaoCmd_A1(void* arg0) { func_8002BCCC(arg0, &g_Channel3[2]); }

void AkaoCmd_A5(s32 arg0) { func_8002BD04(arg0, &g_Channel3[2]); }

void AkaoCmd_A2(void* arg0) { func_8002BCCC(arg0, &g_Channel3[0]); }

void AkaoCmd_A6(s32 arg0) { func_8002BD04(arg0, &g_Channel3[0]); }

void AkaoCmd_A3(void* arg0) { func_8002BCCC(arg0, &g_Channel3[6]); }

void AkaoCmd_A7(s32 arg0) { func_8002BD04(arg0, &g_Channel3[6]); }

// Same shape as func_8002BCCC (two 0x108-byte halves, shared +0xE0 control
// word), at a different pitch/clear field within each half.
static void func_8002BFCC(void* arg0, void* arg1) {
    s16 temp_v0;
    s32 v1;
    AkaoChannel* voice = (AkaoChannel*)arg1;

    temp_v0 = (*(u16*)((u8*)arg0 + 0x4) & 0x7F) << 8;
    v1 = voice[1].setFlags;
    voice[1].setTo0_62 = 0;
    voice[0].setTo0_62 = 0;
    voice[1].baseVolumePan = temp_v0;
    voice[0].baseVolumePan = temp_v0;
    voice[0].setFlags = voice[0].setFlags | 3;
    voice[1].setFlags = (v1 | 3);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C004);

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BA(void* arg0) {
    func_8002BFCC(arg0, &g_Channel3[6]);
    func_8002BFCC(arg0, &g_Channel3[4]);
    func_8002BFCC(arg0, &g_Channel3[2]);
    func_8002BFCC(arg0, &g_Channel3[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BB(void* arg0) {
    func_8002C004(arg0, &g_Channel3[6]);
    func_8002C004(arg0, &g_Channel3[4]);
    func_8002C004(arg0, &g_Channel3[2]);
    func_8002C004(arg0, &g_Channel3[0]);
}

void AkaoCmd_A8(void* arg0) { func_8002BFCC(arg0, &g_Channel3[4]); }

void AkaoCmd_AC(s32 arg0) { func_8002C004(arg0, &g_Channel3[4]); }

void AkaoCmd_A9(void* arg0) { func_8002BFCC(arg0, &g_Channel3[2]); }

void AkaoCmd_AD(s32 arg0) { func_8002C004(arg0, &g_Channel3[2]); }

void AkaoCmd_AA(void* arg0) { func_8002BFCC(arg0, &g_Channel3[0]); }

void AkaoCmd_AE(s32 arg0) { func_8002C004(arg0, &g_Channel3[0]); }

void AkaoCmd_AB(void* arg0) { func_8002BFCC(arg0, &g_Channel3[6]); }

void AkaoCmd_AF(s32 arg0) { func_8002C004(arg0, &g_Channel3[6]); }

// Same shape as func_8002BCCC/func_8002BFCC (two 0x108-byte halves, shared
// +0xE0 control word), at a third pitch/clear field, setting flag bit 0x10
// instead of 0x3.
static void func_8002C2CC(void* arg0, void* arg1) {
    s32 temp_v0;
    s32 temp_v1;
    s8* arg0_bytes = (s8*)arg0;
    AkaoChannel* voice = (AkaoChannel*)arg1;

    temp_v0 = arg0_bytes[4] << 8;
    temp_v1 = voice[1].setFlags;
    voice[1].unk5A = 0;
    voice[0].unk5A = 0;
    voice[1].pitchModifier = temp_v0;
    voice[0].pitchModifier = temp_v0;
    voice[0].setFlags = voice[0].setFlags | 0x10;
    voice[1].setFlags = temp_v1 | 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002C300);

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BC(void* arg0) {
    func_8002C2CC(arg0, &g_Channel3[6]);
    func_8002C2CC(arg0, &g_Channel3[4]);
    func_8002C2CC(arg0, &g_Channel3[2]);
    func_8002C2CC(arg0, &g_Channel3[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BD(void* arg0) {
    func_8002C300(arg0, &g_Channel3[6]);
    func_8002C300(arg0, &g_Channel3[4]);
    func_8002C300(arg0, &g_Channel3[2]);
    func_8002C300(arg0, &g_Channel3[0]);
}

void AkaoCmd_B0(void* arg0) { func_8002C2CC(arg0, &g_Channel3[4]); }

void AkaoCmd_B4(s32 arg0) { func_8002C300(arg0, &g_Channel3[4]); }

void AkaoCmd_B1(void* arg0) { func_8002C2CC(arg0, &g_Channel3[2]); }

void AkaoCmd_B5(s32 arg0) { func_8002C300(arg0, &g_Channel3[2]); }

void AkaoCmd_B2(void* arg0) { func_8002C2CC(arg0, &g_Channel3[0]); }

void AkaoCmd_B6(s32 arg0) { func_8002C300(arg0, &g_Channel3[0]); }

void AkaoCmd_B3(void* arg0) { func_8002C2CC(arg0, &g_Channel3[6]); }

void AkaoCmd_B7(s32 arg0) { func_8002C300(arg0, &g_Channel3[6]); }

void AkaoCmd_D0(Unk8002C5A8* arg0) {
    s32 n = arg0->unk4;
    g_AkaoTempoMulMusicSlideSteps = 0;
    g_AkaoTempoMulMusic = n << 0x10;
}

typedef struct {
    s32 unk0;
    s32 unk4;
    s8 unk8;
} Unk8002C5C8;

// Starts a tempo slide toward a target derived from arg0, over arg0's tick
// count.
void AkaoD1TempoSlideFromCurrent(Unk8002C5C8* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoTempoMulMusicSlideStep = ((arg0->unk8 << 0x10) - g_AkaoTempoMulMusic) / var_a1;
    g_AkaoTempoMulMusicSlideSteps = var_a1;
}

// Starts a tempo slide between two explicit targets from arg0, over arg0's
// tick count.
void AkaoD2TempoSlideBetweenTargets(Unk8002C5A8* arg0) {
    long new_var;
    s32 temp_a2;
    s32 temp_v1;
    s32 var_a1;

    temp_v1 = arg0->unk8;
    temp_a2 = arg0->unk4 << 0x10;
    g_AkaoTempoMulMusic = temp_a2;
    var_a1 = 1;
    if (temp_v1 != 0) {
        var_a1 = temp_v1;
    }
    new_var = (arg0->unkC << 0x10) - temp_a2;
    g_AkaoTempoMulMusicSlideSteps = var_a1;
    g_AkaoTempoMulMusicSlideStep = new_var / var_a1;
}

void AkaoCmd_D4(Unk8002C5A8* arg0) {
    s32 n = arg0->unk4;
    g_AkaoPitchMulMusicSlideSteps = 0;
    g_AkaoPitchMulMusic = n << 0x10;
}

typedef struct {
    s32 unk0;
    s32 unk4;
    s8 unk8;
} Unk8002C6C8;

// Starts a pitch slide from the current g_AkaoPitchMulMusic toward a
// target derived from arg0, over arg0's tick count.
void AkaoD5PitchSlideFromCurrent(Unk8002C6C8* arg0) {
    s32 temp_v0;
    s32 var_a1;
    s32 temp_v1;

    temp_v0 = arg0->unk4;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v1 = ((arg0->unk8 << 0x10) - g_AkaoPitchMulMusic) / var_a1;
    g_AkaoPitchMulMusicSlideSteps = var_a1;
    g_AkaoPitchMulMusicSlideStep = temp_v1;
}

// Starts a pitch slide between two explicit targets from arg0, over arg0's
// tick count.
void AkaoD6PitchSlideBetweenTargets(Unk8002C5A8* arg0) {
    s32 new_var;
    s32 temp_a2;
    s32 temp_v1;
    s32 var_a1;

    temp_v1 = arg0->unk8;
    temp_a2 = arg0->unk4 << 0x10;
    g_AkaoPitchMulMusic = temp_a2;
    var_a1 = 1;
    if (temp_v1 != 0) {
        var_a1 = temp_v1;
    }
    new_var = (arg0->unkC << 0x10) - temp_a2;
    g_AkaoPitchMulMusicSlideSteps = var_a1;
    g_AkaoPitchMulMusicSlideStep = new_var / var_a1;
}

static void AkaoCmd_F0(void) { AkaoMusicStopChannels12(); }

static void AkaoCmd_F1(void) { AkaoSoundChannelsStop(); }

static void AkaoCmd_80(void) {
    g_Channel1Config = 1;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_82);

static void Akao81SetMonoMode(void) {
    g_Channel1Config = 2;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_90);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_92);

void AkaoUpdateChannelParamsToSpu(s32, void*);

// Moves newly-requested channels_1 voices into the active mask, resetting
// each one's SPU attributes.
void Akao9BApplyPendingMusicUpdates(void) {
    s32 savedMask;
    s32 bit;
    s32 pendingBits;
    s32 voiceIdx;

    if (g_AkaoMusicActiveMask != 0) {
        pendingBits = (g_AkaoMusicActiveMask | g_AkaoMusicOverMask | g_AkaoMusicAltMask) &
                      ~(g_Channel3ActiveMask[0] | g_AkaoStreamMask);
        if (pendingBits != 0) {
            bit = 1;
            voiceIdx = 0;
            D_8007EC0E = 0;
            D_8007EC0C = 0;
            D_8007EC08 = 0x7F;
            for (; pendingBits != 0; bit *= 2, voiceIdx += 1) {
                if (pendingBits & bit) {
                    g_AkaoVoiceAttrMask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, &g_AkaoVoiceAttr);
                    pendingBits ^= bit;
                }
            }
        }
        savedMask = g_AkaoMusicActiveMask;
        g_AkaoMusicActiveMask = 0;
        g_AkaoMusicActiveMaskStored = savedMask;
    }
    g_AkaoControlFlags |= 1;
}

void AkaoUpdateNoiseVoices();
void AkaoUpdateReverbVoices();
void AkaoUpdatePitchLfoVoices();

// Restore counterpart: moves the stored channels_1 mask back to active,
// resetting SPU attributes along the way.
void Akao9AFlushPendingMusicUpdates(void) {
    AkaoChannel* voice;
    s32 savedMask;
    unsigned int stillPending;
    s32 bit;
    s32 pendingBits;

    pendingBits = g_AkaoMusicActiveMaskStored;
    if (pendingBits != 0) {
        bit = 1;
        voice = g_Channel1;
        do {
            if (pendingBits & bit) {
                pendingBits ^= bit;
                voice->setFlags |= SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
            bit *= 2;
            voice++;
        } while (stillPending = pendingBits != 0);
        savedMask = g_AkaoMusicActiveMaskStored;
        g_AkaoMusicActiveMaskStored = 0;
        g_AkaoMusicActiveMask = savedMask;
        AkaoUpdateNoiseVoices(bit, pendingBits);
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~1;
}

// channels_3 counterpart to Akao9BApplyPendingMusicUpdates; also masks off
// the top two voices in mono mode.
void Akao9DApplyPendingSoundUpdates(void) {
    s32 savedMask;
    short cleared;
    s32 newMask;
    s32 bit;
    s32 voiceIdx;

    newMask = g_Channel3ActiveMask[0];
    savedMask = newMask;
    if (newMask != 0) {
        bit = 0x10000;
        if (D_80099E0C == 2) {
            newMask &= ~((1 << 22) | (1 << 23));
        }
        g_AkaoSoundActiveMaskStored = newMask;
        g_Channel3ActiveMask[cleared = 0] = newMask ^ savedMask;
        D_8007EC0E = cleared;
        D_8007EC0C = cleared;
        D_8007EC08 = 0x7F;
        voiceIdx = 0x10;
        if (newMask != cleared) {
            for (; newMask != 0; bit *= 2, voiceIdx += 1) {
                if (newMask & bit) {
                    g_AkaoVoiceAttrMask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, &g_AkaoVoiceAttr);
                    newMask ^= bit;
                }
            }
        }
    }
    g_AkaoControlFlags |= 2;
}

// channels_3 counterpart to Akao9AFlushPendingMusicUpdates.
void Akao9CFlushPendingSoundUpdates(void) {
    AkaoChannel* half;
    s32 savedMask;
    s32 bit;
    s32 pendingBits;

    pendingBits = g_AkaoSoundActiveMaskStored;
    if (pendingBits != 0) {
        for (bit = 0x10000, half = &g_Channel3[0]; pendingBits != 0; bit *= 2, half++) {
            if (pendingBits & bit) {
                pendingBits ^= bit;
                half->setFlags |= SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
        }
        savedMask = g_AkaoSoundActiveMaskStored;
        g_AkaoSoundActiveMaskStored = 0;
        g_Channel3ActiveMask[0] = savedMask;
        AkaoUpdateNoiseVoices(bit);
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~2;
}

typedef struct {
    u32 unk0;
    u16 unk4;
} Unk8002CC18;

static void AkaoE0SetReverbPan(Unk8002CC18* arg0) {
    g_AkaoReverbPan = arg0->unk4 & 0x7F;
    D_8009A13C |= 0x80;
}

typedef struct {
    u32 unk0;
    u8 unk4;
} Unk8002CC44;

static void AkaoE4SetReverbMul(Unk8002CC44* arg0) {
    u8 temp_v0;
    s32 var_v0;
    s32 mask;

    temp_v0 = arg0->unk4;
    g_AkaoReverbMul = (s16)temp_v0;
    mask = ~0x10;
    if (temp_v0 != 0) {
        var_v0 = g_AkaoControlFlags | 0x10;
    } else {
        var_v0 = g_AkaoControlFlags & mask;
    }
    g_AkaoControlFlags = var_v0;
    AkaoUpdateReverbVoices();
    D_8009A13C |= 0x80;
}

static void AkaoCmd_F2(void) { D_8008337E = 0; }

static void AkaoCmd_F3(void) { D_800833DE = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F5);

static void AkaoF8StreamReverbMaskClear(void) {
    s32* addr;
    s32 temp_a0;
    s32 temp_v1;

    func_8002CFC0();
    addr = g_Channel3ActiveMask;
    temp_a0 = g_AkaoReverbMask;
    temp_v1 = ~g_AkaoStreamMask;
    *addr &= temp_v1;
    g_AkaoReverbMask = temp_v1 & temp_a0;
    AkaoUpdateReverbVoices(temp_a0, addr);
}

static void AkaoF9StreamReverbMaskRestore(void) {
    s32 temp_a0;

    func_8002CFC0();
    temp_a0 = g_Channel3ActiveMask[0];
    g_Channel3ActiveMask[0] = ~g_AkaoStreamMask & temp_a0;
    g_AkaoReverbMask |= g_AkaoStreamMask;
    AkaoUpdateReverbVoices(temp_a0, g_Channel3ActiveMask, g_AkaoStreamMask);
}

static void AkaoCmd_FA(void) { func_80029A50(); }

void AkaoCmd_Null(Unk8002B7E0* arg0) {}

static void func_8002CFA0() { SpuSetTransferCallback(0); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002CFC0);

// Configures the voice-attribute block for a mono CD-stream voice (ADSR
// envelope, pan, reverb-echo work area) and applies it via AkaoUpdateChannelParamsToSpu.
static void AkaoStreamVoiceAttrMono(void) {
    g_AkaoVoiceAttrMask = 0x1FF93;
    D_8007EC02 = 0;
    D_8007EBEC = 0x77000;
    D_8007EBF0 = 0x77000;
    D_8007EC04 = 0xF;
    D_8007EC06 = 0xF;
    D_8007EC08 = 0x7F;
    D_8007EC0A = 6;
    D_8007EBF4 = 1;
    D_8007EBF8 = 3;
    D_8007EBFC = 3;
    D_8007EC0C = (D_80062FB0 ^ 0x7F) * D_80062FAC >> 7;
    D_8007EC00 = D_80062F1E;
    D_8007EC0E = D_80062FAC * D_80062FB0 >> 7;
    AkaoUpdateChannelParamsToSpu(0x10, &g_AkaoVoiceAttr);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002D2D4);

static void AkaoStreamIrqCallbackMono0(void);

// CD-stream DMA transfer-complete callback (mono case). Keys on the stream
// voice(s) in g_AkaoStreamMask; when D_80063004 (bytes remaining) is nonzero, first
// re-arms the SPU transfer IRQ with AkaoStreamIrqCallbackMono0 to continue streaming.
static void AkaoStreamTransferCallbackMono(void) {
    SpuSetTransferCallback(0);
    if (D_80063004 != 0) {
        SpuSetIRQ(0);
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
    }
    SpuSetKey(1, g_AkaoStreamMask);
    g_Channel3OffMask &= ~g_AkaoStreamMask;
}

static void AkaoStreamIrqCallbackSplit0(void);

// CD-stream DMA transfer-complete callback (split/stereo case). Twin of
// AkaoStreamTransferCallbackMono above, using a different IRQ callback.
static void AkaoStreamTransferCallbackSplit(void) {
    SpuSetTransferCallback(0);
    if (D_80063004 != 0) {
        SpuSetIRQ(0);
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        SpuSetIRQ(1);
    }
    SpuSetKey(1, g_AkaoStreamMask);
    g_Channel3OffMask &= ~g_AkaoStreamMask;
}

static void AkaoStreamIrqCallbackMono1(void);

static void AkaoStreamIrqCallbackMono0(void) {
    if (D_80063004 == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x77000);
    SpuWrite(D_80062FE0, 0x1000);
    SpuSetIRQ(0);
    if (D_80063004 > 0x1000) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono1);
        SpuSetIRQ(1);
        D_80063004 -= 0x1000;
        D_80062FE0 += 0x1000;
        return;
    }
    if (D_80063000 != 0) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono1);
        SpuSetIRQ(1);
        D_80062FE0 = D_80063000;
        D_80063004 = D_80062F08;
        return;
    }
    D_80063004 = 0;
    SpuSetIRQAddr(0x77000);
    SpuSetIRQCallback(func_80029A50);
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackMono1(void) {
    if (D_80063004 == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x78000);
    SpuWrite(D_80062FE0, 0x1000);
    SpuSetIRQ(0);
    if (D_80063004 > 0x1000) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
        D_80063004 -= 0x1000;
        D_80062FE0 += 0x1000;
        return;
    }
    if (D_80063000 != 0) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
        D_80062FE0 = D_80063000;
        D_80063004 = D_80062F08;
        return;
    }
    D_80063004 = 0;
    SpuSetIRQAddr(0x78000);
    SpuSetIRQCallback(func_80029A50);
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackSplit1(void);

static void AkaoStreamIrqCallbackSplit0(void) {
    if (D_80063004 == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x77000);
    SpuWrite(D_80062FE0, 0x1000);
    SpuSetIRQ(0);
    SpuSetVoiceLoopStartAddr(0x10, 0x77000);
    SpuSetVoiceLoopStartAddr(0x11, 0x77800);
    if (D_80063004 > 0x1000) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit1);
        D_80063004 -= 0x1000;
        D_80062FE0 += 0x1000;
    } else if (D_80063000 != 0) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit1);
        D_80062FE0 = D_80063000;
        D_80063004 = D_80062F08;
    } else {
        D_80063004 = 0;
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(func_80029A50);
    }
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackSplit1(void) {
    if (D_80063004 == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x78000);
    SpuWrite(D_80062FE0, 0x1000);
    SpuSetIRQ(0);
    SpuSetVoiceLoopStartAddr(0x10, 0x78000);
    SpuSetVoiceLoopStartAddr(0x11, 0x78800);
    if (D_80063004 > 0x1000) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        D_80063004 -= 0x1000;
        D_80062FE0 += 0x1000;
    } else if (D_80063000 != 0) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        D_80062FE0 = D_80063000;
        D_80063004 = D_80062F08;
    } else {
        D_80063004 = 0;
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(func_80029A50);
    }
    SpuSetIRQ(1);
}

static void AkaoGetCommandQueue(Unk8002B7E0** out_msg) {
    *out_msg = D_80081DC8;
    *out_msg = &D_80081DC8[g_AkaoCommandQueueId];
    g_AkaoCommandQueueId++;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoExec);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_8002DF88);

static void AkaoExecuteCommandsQueue(void) {
    Unk8002B7E0* msg;

    if (g_AkaoMutex == 0) {
        for (msg = D_80081DC8; g_AkaoCommandQueueId; g_AkaoCommandQueueId--, msg++) {
            D_80049548[msg->unk0](msg);
        }
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateChannelParamsToSpu);

// Applies the current CD volume (g_AkaoCdVol) to the SPU's CD-input channel.
// Confirmed against qgears' independent reverse-engineering (system_psyq_spu_
// set_common_attr call, mask = SPU_COMMON_CDVOLL|CDVOLR|CDREV): g_SpuCommonAttr's
// first field is a field-select mask, not a voice bitmask.
static void AkaoUpdateCdVolume(void) {
    g_SpuCommonAttr.mask = 0x1C0;
    g_SpuCommonAttr.cd.reverb = 0;
    g_SpuCommonAttr.cd.volume.right = g_AkaoCdVol.i.hi;
    g_SpuCommonAttr.cd.volume.left = g_AkaoCdVol.i.hi;
    SpuSetCommonAttr(&g_SpuCommonAttr);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicUpdateSlideAndDelay);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundUpdateSlideAndDelay);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicUpdatePitchAndVol);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundUpdatePitchAndVol);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateChannelAndOvlParamsToSpu);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateKeysOn);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCollectChannelsVoicesMask);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateKeysOff);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateNoiseVoices);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateReverbVoices);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdatePitchLfoVoices);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMain);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", func_80030380);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMainUpdate);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoExecuteSequence);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoInstrInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoGetNextNote);

static u8 func_80031A70(u8** arg0) {
    u8 expected;
    u8 len;
    u8 opcode;
    u8* data;

    data = *arg0;
    expected = 0xCA;
    do {
        opcode = *data;
        len = D_800499A8[opcode];
        data += len;
    } while (len);
    return opcode == expected ? 0xCA : 0xA0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_E8_Tempo);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_E9_TempoSlide);

static void AkaoOp_EA_ReverbDepth(u8** cursor, AkaoChannel* track) {
    u8* p = *cursor;
    u8 v0;
    u8 v1;
    u32 combined;

    /* cc1-psx writes the cursor back after each byte, not once at the end --
       tested; a single trailing writeback regresses the gate. */
    *cursor = p + 1;
    v0 = p[0];
    *cursor = p + 2;
    v1 = p[1];
    combined = (u32)v0 << 0x10;
    combined |= (u32)v1 << 0x18;
    *(u16*)&track->setToMinusOne = 0; // only the first half of this 4-byte unknown field
    track->updateMirror |= 0x80;
    track->unk40 = combined;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_EB_ReverbDepthSlide);

static void AkaoOp_A3_MasterVol(AkaoChannel* track) {
    track->volumeMultiplier = *track->akaoSequencePointer++;
    track->setFlags |= 3;
}

static void AkaoOp_A8_SetVol(AkaoChannel* track) {
    s32 val = (s8)*track->akaoSequencePointer++;

    track->initWith0_5C = 0;
    track->setFlags |= 3;
    track->volumeLevel = val << 0x17;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A9_SetVolSlide);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F4_OverlayVoiceOn);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F5_OverlayVoiceOff);

static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->unk5E = 0;
    track->volumeMultiplier2 = val << 8;
    if (track->updateMirror & 0x100) {
        track->setFlags |= 3;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F7_OverlayVolBalanceSlide);

static void AkaoOp_AA_SetPan(AkaoChannel* track) {
    track->baseVolumePan = *track->akaoSequencePointer++ << 8;
    track->setTo0_62 = 0;
    track->setFlags |= 3;
}

static void AkaoOp_AB_SetPanSlide(AkaoChannel* track) {
    u8 ch;
    u16 var_a0;

    track->setTo0_62 = *track->akaoSequencePointer++;
    if (track->setTo0_62 == 0) {
        track->setTo0_62 = 0x100;
    }
    ch = *track->akaoSequencePointer++;
    track->baseVolumePan &= 0xFF00;
    var_a0 = track->baseVolumePan;
    track->unkCA = ((ch << 8) - var_a0) / (u16)track->setTo0_62;
}

static void AkaoOp_A5_SetOctave(AkaoChannel* track) { track->pitchCorrection = *track->akaoSequencePointer++; }

static void AkaoOp_A6_IncOctave(AkaoChannel* track) { track->pitchCorrection = (track->pitchCorrection + 1) & 0xF; }

static void AkaoOp_A7_DecOctave(AkaoChannel* track) {
    track->pitchCorrection = (track->pitchCorrection + 0xFFFF) & 0xF;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A1_LoadInstrument);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F2_LoadInstrument);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B3_ResetAdsr);

static void AkaoOp_C0_TransposeAbsolute(AkaoChannel* track) {
    track->absoluteTransposition = (s8)*track->akaoSequencePointer++;
}

static void AkaoOp_C1_TransposeRelative(AkaoChannel* track) {
    track->absoluteTransposition = (s8)*track->akaoSequencePointer++ + track->absoluteTransposition;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A4_PitchBendSlide);

static void AkaoOp_DA_PortamentoOn(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->initWith0_6C = (s16)val;
    if (val == 0) {
        track->initWith0_6C = 0x100;
    }
    track->unkD4 = 0;
    track->unk6A = 0;
    track->initWith0_6E = 1;
}

static void AkaoOp_DB_PortamentoOff(AkaoChannel* track) { track->initWith0_6C = 0; }

static void AkaoOp_D8_FineTuningAbsolute(AkaoChannel* track) {
    track->frequencyMultiplier = (s8)*track->akaoSequencePointer++;
}

static void AkaoOp_D9_FineTuningRelative(AkaoChannel* track) {
    track->frequencyMultiplier = (s8)*track->akaoSequencePointer++ + track->frequencyMultiplier;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B4_Vibrato);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B5_VibratoDepth);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_DD_VibratoDepthSlide);

static void AkaoOp_B6_VibratoOff(AkaoChannel* track) {
    track->pitchLfoValue = 0;
    track->updateMirror &= ~1;
    track->setFlags |= 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B8_Tremolo);

static void AkaoOp_B9_TremoloDepth(AkaoChannel* track) {
    track->volumeLfoMultiplier = *track->akaoSequencePointer++ << 8;
}

static void AkaoDETremoloDepthSlideFromCurrent(AkaoChannel* track) {
    u16 rate;
    u8* addr;
    s32 delta;

    addr = track->akaoSequencePointer;
    track->akaoSequencePointer = addr + 1;
    rate = addr[0];
    if (rate == 0) {
        rate = 0x100;
    }
    track->akaoSequencePointer = addr + 2;
    delta = ((addr[1] << 8) - *(u16*)&track->volumeLfoMultiplier) / rate;
    track->initWith0_92 = rate;
    track->unk94 = delta;
}

static void AkaoOp_BA_TremoloOff(AkaoChannel* track) {
    track->volumeLfoValue = 0;
    track->updateMirror &= ~2;
    track->setFlags |= 3;
}

static void AkaoBCSetPanLfo(AkaoChannel* track) {
    u8* addr;
    u8* addr2;
    u8 rate;

    addr = track->akaoSequencePointer;
    track->updateMirror |= 4;
    track->akaoSequencePointer = addr + 1;
    rate = *addr;
    track->volumePanLfoRefreshInterval = rate;
    if (rate == 0) {
        track->volumePanLfoRefreshInterval = 0x100;
    }
    addr2 = track->akaoSequencePointer;
    track->akaoSequencePointer = addr2 + 1;
    track->volumePanLfoTableKeyNodeIndex = *addr2;
    track->volumePanLfoWaveTableAddr = g_AkaoWaveTableKey[*(u16*)&track->volumePanLfoTableKeyNodeIndex];
    track->volumePanLfoRefreshIntervalCounter = 1;
}

static void AkaoOp_BD_PanLfoDepth(AkaoChannel* track) { track->initWith0_9E = *track->akaoSequencePointer++ << 7; }

static void AkaoDFPanLfoDepthSlideFromCurrent(AkaoChannel* track) {
    u8* addr;
    s32 rate;
    s32 delta;

    addr = track->akaoSequencePointer;
    track->akaoSequencePointer = addr + 1;
    rate = *addr;
    if (rate == 0) {
        rate = 0x100;
    }
    track->akaoSequencePointer = addr + 2;
    delta = ((addr[1] << 7) - *(u16*)&track->initWith0_9E) / rate;
    track->initWith0_A0 = rate;
    track->unkA2 = delta;
}

static void AkaoOp_BE_PanLfoOff(AkaoChannel* track) {
    track->volumePanLfoValue = 0;
    track->updateMirror &= ~4;
    track->setFlags |= 3;
}

static void AkaoC4NoiseOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->noiseMusicVoicesMask = mask | config->noiseMusicVoicesMask;
    } else {
        g_AkaoNoiseMask |= mask;
    }
    D_8009A13C |= 0x10;
    AkaoUpdateNoiseVoices();
}

static void AkaoC5NoiseOff(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->noiseMusicVoicesMask &= ~mask;
    } else {
        g_AkaoNoiseMask &= ~mask;
    }
    D_8009A13C |= 0x10;
    AkaoUpdateNoiseVoices();
    track->noiseSwitchDelay = 0;
}

static void AkaoC6PitchLfoOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->pitchLfoMusicVoicesMask = mask | config->pitchLfoMusicVoicesMask;
    } else if (!(mask & 0x555555)) {
        g_AkaoPitchLfoMask |= mask;
    }
    AkaoUpdatePitchLfoVoices();
}

static void AkaoC7PitchLfoOff(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->pitchLfoMusicVoicesMask &= ~mask;
    } else {
        g_AkaoPitchLfoMask &= ~mask;
    }
    AkaoUpdatePitchLfoVoices();
    track->pitchLfoSwitchDelay = 0;
}

static void AkaoC2ReverbOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->reverbMusicVoicesMask = mask | config->reverbMusicVoicesMask;
    } else {
        g_AkaoReverbMask |= mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoC3ReverbOff(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->reverbMusicVoicesMask = ~mask & config->reverbMusicVoicesMask;
    } else {
        g_AkaoReverbMask &= ~mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_CC_LegatoOn(AkaoChannel* track) { track->initWith0_6E = 1; }

static void AkaoOp_CD_LegatoOff(void) {}

static void AkaoOp_D0_FullLengthOn(AkaoChannel* track) { track->initWith0_6E = 4; }

static void AkaoOp_D1_FullLengthOff(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_AC_NoiseClockFreq);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_AD_SetAr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_AE_SetDr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_AF_SetSl);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B1_SetSr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B2_SetRr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B7_AttackMode);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_BB_SustainMode);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_BF_ReleaseMode);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F8_AltVoiceOn);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F9_AltVoiceOff);

void AkaoC8LoopPoint(AkaoChannel* track) {
    track->lastSavedLoopPointIndex = (track->lastSavedLoopPointIndex + 1) & 3;
    track->loopPoint[track->lastSavedLoopPointIndex] = track->akaoSequencePointer;
    track->loopPointState[track->lastSavedLoopPointIndex] = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_C9_LoopReturnTimes);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F0_LoopJumpTimes);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F1_LoopBreakTimes);

void AkaoCALoopReturn(AkaoChannel* track) {
    track->loopPointState[track->lastSavedLoopPointIndex]++;
    track->akaoSequencePointer = track->loopPoint[track->lastSavedLoopPointIndex];
}

static void AkaoOp_A2_NextNoteLength(AkaoChannel* track) {
    u16 val = *track->akaoSequencePointer++;

    track->pauseMultiplier = 0;
    /* sets length_1 and length_2 to the same byte in one halfword store;
       writing them as two separate field assignments regresses the gate. */
    *(s16*)&track->pause_length1 = (val << 8) | val;
    track->pauseStorage = val;
}

static void AkaoOp_DC_FixNoteLength(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    short delta = READ_S8(track->akaoSequencePointer);
    if (delta != 0) {
        delta += track->pauseStorage;
        if (delta < 1) {
            delta = 1;
        } else if (delta > 255) {
            delta = 255;
        }
    }
    track->pauseMultiplier = delta;
}

static void AkaoOp_EC_DrumModeOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->drumOffset = track->akaoSequencePointer + READ_S16(track->akaoSequencePointer);
    track->updateMirror |= 0x8;
}

static void AkaoOp_ED_DrumModeOff(AkaoChannel* track, AkaoConfig* config, u32 mask) { track->updateMirror &= ~0x8; }

static void AkaoOp_FD_TimeSignature(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    config->lowerTimerEqualValue = *track->akaoSequencePointer++;
    config->upperTimerEqualValue = *track->akaoSequencePointer++;
    config->lowerTimerValue = 0;
    config->upperTimerValue = 0;
}

static void AkaoOp_FE_MeasureNumber(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    config->topTimer = *track->akaoSequencePointer++;
    config->topTimer |= *track->akaoSequencePointer++ << 8;
}

static void func_800335CC(AkaoChannel* track, AkaoConfig* config, u32 mask) { config->unk54 = 1; }

static void AkaoOp_B0_SetVoiceDrSl(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    AkaoOp_AE_SetDr(track, config, mask);
    AkaoOp_AF_SetSl(track, config, mask);
}

static void AkaoOp_CE_NoiseSwitch(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    int delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        track->noiseSwitchDelay = 257;
    } else {
        track->noiseSwitchDelay = delay + 1;
    }
    AkaoC4NoiseOn(track, config, mask);
}

static void AkaoOp_CF_NoiseSwitch(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    s16 var_v0 = *track->akaoSequencePointer++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->noiseSwitchDelay = var_v0;
}

static void AkaoOp_D2_FrequencyModulationSwitch(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    int delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        track->pitchLfoSwitchDelay = 257;
    } else {
        track->pitchLfoSwitchDelay = delay + 1;
    }
    AkaoC6PitchLfoOn(track, config, mask);
}

static void AkaoOp_D3_FrequencyModulationSwitch(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    s16 var_v0 = *track->akaoSequencePointer++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->pitchLfoSwitchDelay = var_v0;
}

static void AkaoOp_CB_SfxReset(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->updateMirror &= ~0x37;
    AkaoC5NoiseOff(track, config, mask);
    AkaoC7PitchLfoOff(track, config, mask);
    AkaoC3ReverbOff(track, config, mask);
    track->initWith0_6E &= ~0x5;
}

static void AkaoOp_D4_SideChainPlaybackOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->updateMirror |= 0x10;
}

static void AkaoOp_D5_SideChainPlaybackOff(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->updateMirror &= ~0x10;
}

static void AkaoOp_D6_SideChainPitchVolOn(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->updateMirror |= 0x20;
}

static void AkaoOp_D7_SideChainPitchVolOff(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->updateMirror &= ~0x20;
}

static void AkaoOp_EE_Jump(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
}

static void AkaoOp_EF_JumpConditional(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    int cond = *track->akaoSequencePointer++;
    if (config->jumpConditionStorage2 != 0 && cond <= config->jumpConditionStorage2) {
        track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
        config->jumpConditionStorage1 = cond;
    } else {
        track->akaoSequencePointer += 2;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A0_FinishChannel);

static void AkaoOp_Null(AkaoChannel* track, AkaoConfig* config, u32 mask) {
    AkaoOp_A0_FinishChannel(track, config, mask);
}

void SysSavemapReset(void) {
    s32 i;
    u8* bank;

    for (i = 1279, bank = &Savemap.memory_bank_5[255]; i >= 0; i--, bank--) {
        *bank = 0;
    }

    for (i = 0; i < 3; i++) {
        Savemap.partyID[i] = 0xFF;
        Savemap.memory_bank_2[i + 9] = 0xFF;
    }

    Savemap.phs_visibility_mask = 1; // Only Cloud is visible.
    g_FieldMusicLock = 0;
    D_800716CC = 0;
    D_80071E30 = 0;
    Savemap.partyID[0] = 0;
    Savemap.memory_bank_2[9] = 0;
    Savemap.memory_bank_4[0x68] = 0xFF; // Start of location name.
    Savemap.memory_bank_1[0x1C] = 0xFF; // Menu visibility, 2 bytes.
    Savemap.memory_bank_1[0x1D] = 0xFF;
    Savemap.time = 0;
    Savemap.countdown_timer_seconds = 0;
    g_FieldState.nFadeRedStart = 0;
    g_FieldState.nFadeGreenStart = 0;
    g_FieldState.nFadeBlueStart = 0;
    g_FieldState.movieCamDisabled = 0;
    g_PartyUpdatedByFieldScript = 0;
}
