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
    /* 0x0 */ s32 opcode;
    /* 0x4 */ s8 start;
    /* 0x5 */ s8 pad5[3];
    /* 0x8 */ s32 steps;
    /* 0xC */ s8 target;
} AkaoTempoPitchSlide;


typedef struct {
    /* 0x00 */ u32 voice_id;
    /* 0x04 */ u32 mask;
    /* 0x08 */ u32 addr;
    /* 0x0C */ u32 loop_addr;
    /* 0x10 */ s32 a_mode;
    /* 0x14 */ s32 s_mode;
    /* 0x18 */ s32 r_mode;
    /* 0x1C */ u16 pitch;
    /* 0x1E */ u16 ar;
    /* 0x20 */ u16 dr;
    /* 0x22 */ u16 sl;
    /* 0x24 */ s16 sr;
    /* 0x26 */ u16 rr;
    /* 0x28 */ s16 vol_l;
    /* 0x2A */ s16 vol_r;
} AkaoVoiceAttr; /* size = 0x2C */

// Field names cross-checked against the independent qgears reverse-engineering
// project's AkaoChannel struct (same source as the g_Akao*SlideStep/Steps
// naming above): https://github.com/Akari1982/q-gears_reverse,
// ffvii/DISC/SCUS_941_akao.h. Spans neither this repo nor qgears resolves
// (LFO delay/rate sub-fields, the 0xA8-0xB8 gap) are left as unkNN.
typedef struct {
    /* 0x00 */ u8* akaoSequencePointer;
    /* 0x04 */ u8* loopPoint[4];
    /* 0x14 */ u8* drumOffset;
    /* 0x18 */ u32 vibratoWave;
    /* 0x1C */ u32 tremoloWave;
    /* 0x20 */ u32 panLfoWave;
    /* 0x24 */ u32 overlayChannelId;
    /* 0x28 */ s32 alternativeChannelId;
    /* 0x2C */ s32 volumeMultiplier;
    /* 0x30 */ s32 basePitch;
    /* 0x34 */ s32 pitchSlide;
    /* 0x38 */ u32 updateFlags;
    /* 0x3C */ u32 pitchMulSound;
    /* 0x40 */ s32 pitchMulSoundSlideStep;
    /* 0x44 */ s32 volumeLevel;
    /* 0x48 */ s32 volSlideStep;
    /* 0x4C */ s32 pitchSlideStep;
    /* 0x50 */ u32 setToMinusOne;
    /* 0x54 */ u16 playingType;
    /* 0x56 */ u8 length1;
    /* 0x57 */ u8 length2;
    /* 0x58 */ u16 currentInstrument;
    /* 0x5A */ u16 pitchMulSoundSlideSteps;
    /* 0x5C */ u16 volSlideSteps;
    /* 0x5E */ u16 volBalanceSlideSteps;
    /* 0x60 */ u16 volPan;
    /* 0x62 */ s16 volPanSlideSteps;
    /* 0x64 */ u16 pitchSlideStepsCur;
    /* 0x66 */ u16 octave;
    /* 0x68 */ u16 pitchSlideSteps;
    /* 0x6A */ u16 keyStored;
    /* 0x6C */ u16 portamentoSteps;
    /* 0x6E */ u16 sfxMask;
    /* 0x70 */ u16 pad70;
    /* 0x72 */ u16 vibratoDelay;
    /* 0x74 */ u16 vibratoDelayCur;
    /* 0x76 */ u16 vibratoRate;
    /* 0x78 */ u16 vibratoRateCur;
    /* 0x7A */ u16 vibratoType;
    /* 0x7C */ u16 vibratoBase;
    /* 0x7E */ u16 vibratoDepth;
    /* 0x80 */ u16 vibratoDepthSlideSteps;
    /* 0x82 */ s16 vibratoDepthSlideStep;
    /* 0x84 */ u16 pad84;
    /* 0x86 */ u16 tremoloDelay;
    /* 0x88 */ u16 tremoloDelayCur;
    /* 0x8A */ u16 tremoloRate;
    /* 0x8C */ u16 tremoloRateCur;
    /* 0x8E */ u16 tremoloType;
    /* 0x90 */ u16 tremoloDepth;
    /* 0x92 */ u16 tremoloDepthSlideSteps;
    /* 0x94 */ s16 tremoloDepthSlideStep;
    /* 0x96 */ u16 pad96;
    /* 0x98 */ u16 panLfoRate;
    /* 0x9A */ u16 panLfoRateCur;
    /* 0x9C */ u16 panLfoType;
    /* 0x9E */ u16 panLfoDepth;
    /* 0xA0 */ u16 panLfoDepthSlideSteps;
    /* 0xA2 */ s16 panLfoDepthSlideStep;
    /* 0xA4 */ u16 noiseSwitchDelay;
    /* 0xA6 */ u16 pitchLfoSwitchDelay;
    /* 0xA8 */ u8 padA8[0x10];
    /* 0xB8 */ u16 loopId;
    /* 0xBA */ u16 loopTimes[4];
    /* 0xC2 */ s16 lengthStored;
    /* 0xC4 */ s16 lengthFixed;
    /* 0xC6 */ s16 volBalance;
    /* 0xC8 */ s16 volBalanceSlideStep;
    /* 0xCA */ s16 volPanSlideStep;
    /* 0xCC */ u16 transpose;
    /* 0xCE */ s16 fineTuning;
    /* 0xD0 */ u16 key;
    /* 0xD2 */ s16 keyAdd;
    /* 0xD4 */ u16 transposeStored;
    /* 0xD6 */ s16 vibratoPitch;
    /* 0xD8 */ s16 tremoloVol;
    /* 0xDA */ s16 panLfoVol;
    /* 0xDC */ AkaoVoiceAttr voiceAttr;
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
    /* 0x0 */ u8 opcode;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ s16 unk2;
    /* 0x4 */ s32 param0;
    /* 0x8 */ s32 param1;
    /* 0xC */ u16 param2;
    /* 0xE */ u16 padE;
    /* 0x10 */ s32 param3;
    /* 0x14 */ s32 param4;
    /* 0x18 */ s32 param5;
    /* 0x1C */ s32 param6;
    /* 0x20 */ s32 param7;
} AkaoCommand; // size:0x24

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ s32 steps;
    /* 0x8 */ s32 targetVol;
} AkaoVolSlideFromCurr;

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ s32 steps;
    /* 0x8 */ s32 startVol;
    /* 0xC */ s32 targetVol;
} AkaoVolSlideBetweenTargets;

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ s32 steps;
    /* 0x8 */ u16 targetVol;
} AkaoCdVolSlideFromCurr;

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ s32 steps;
    /* 0x8 */ u16 startVol;
    /* 0xA */ u16 padA;
    /* 0xC */ u16 targetVol;
    /* 0xE */ u16 padE;
} AkaoCdVolSlideBetweenTargets;

typedef struct {
    /* 0x0 */ s32 opcode;
    /* 0x4 */ s32 steps;
    /* 0x8 */ s8 target;
} AkaoSlideFromCurr;

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ u16 pan;
} AkaoSetReverbPan;

typedef struct {
    /* 0x0 */ u32 opcode;
    /* 0x4 */ u8 mul;
} AkaoSetReverbMul;

extern void (*D_80049548[])(AkaoCommand*);
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
extern AkaoCommand D_80081DC8[32]; // sound messages queue
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