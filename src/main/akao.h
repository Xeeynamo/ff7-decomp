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