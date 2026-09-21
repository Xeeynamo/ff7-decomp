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

typedef struct {
    /* 0x00 */ u32 addr;
    /* 0x04 */ u32 loopAddr;
    /* 0x08 */ u8 ar;
    /* 0x09 */ u8 dr;
    /* 0x0A */ u8 sl;
    /* 0x0B */ s8 sr;
    /* 0x0C */ u8 rr;
    /* 0x0D */ u8 aMode;
    /* 0x0E */ u8 sMode;
    /* 0x0F */ u8 rMode;
    /* 0x10 */ s32 pitch[12];
} AkaoInstrument; // size: 0x40

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
    /* 0x00 */ u32 stereoMono;
    /* 0x04 */ u32 activeMask;
    /* 0x08 */ u32 onMask;
    /* 0x0C */ u32 keyedMask;
    /* 0x10 */ u32 offMask;
    /* 0x14 */ u32 activeMaskStored;
    /* 0x18 */ u32 tempo;
    /* 0x1C */ s32 tempoSlideStep;
    /* 0x20 */ u32 tempoUpdate;
    /* 0x24 */ u32 overMask;
    /* 0x28 */ u32 altMask;
    /* 0x2C */ u32 noiseMask;
    /* 0x30 */ u32 reverbMask;
    /* 0x34 */ u32 pitchLfoMask;
    /* 0x38 */ u32 updateFlags;
    /* 0x3C */ s32 reverbMode;
    /* 0x40 */ s32 reverbDepth;
    /* 0x44 */ s32 reverbDepthSlideStep;
    /* 0x48 */ u16 tempoSlideSteps;
    /* 0x4A */ u16 musicId;
    /* 0x4C */ u16 conditionStored;
    /* 0x4E */ u16 condition;
    /* 0x50 */ u16 reverbDepthSlideSteps;
    /* 0x52 */ u16 noiseClock;
    /* 0x54 */ u16 muteMusic;
    /* 0x56 */ u16 timerUpper;
    /* 0x58 */ u16 timerUpperCur;
    /* 0x5A */ u16 timerLower;
    /* 0x5C */ u16 timerLowerCur;
    /* 0x5E */ u16 timerTopCur;
} AkaoChannelConfig;

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
extern s32 g_AkaoStreamMask;
extern s32 D_80062F08;
extern u16 D_80062F1E;
// Music-driver slide state: each MulMusic value is a fixed-point scalar for
// pitch/volume/tempo (current value in the upper 16 bits, lower 16 bits are
// fractional precision the driver accumulates every tick for a smooth
// ramp); *SlideStep is the per-tick delta added to it, *SlideSteps is the
// remaining tick count. Names/meaning confirmed one-off against the
// independent qgears reverse-engineering project (not part of this repo):
// https://github.com/q-gears/q-gears, src/main/SCUS_941_akao.cpp.
extern s32 g_AkaoPitchMulMusicSlideStep;
extern s32 g_AkaoVolMulMusicSlideStep;
extern s32 g_AkaoTempoMulMusicSlideStep;
extern s16 g_AkaoPitchMulMusicSlideSteps;
extern s16 g_AkaoVolMulMusicSlideSteps;
extern s16 g_AkaoTempoMulMusicSlideSteps;
extern s32 g_AkaoVolMulMusic;
extern u16 g_AkaoReverbPan;
extern s32 g_AkaoEffectsAll;
extern s32 g_AkaoEffectsAllSeq;
extern s32 g_AkaoMutex;
extern s32 D_80062FAC;
extern s32 D_80062FB0;
extern s32 g_AkaoCdVolSlideStep;
extern u16 g_AkaoReverbMul;
extern u16 g_AkaoCdVolSlideSteps;

extern AkaoCdVol g_AkaoCdVol;
extern s32 D_80062FE0;
extern s32 g_AkaoPitchMulMusic;
extern s32 g_AkaoTempoMulMusic;
extern s32 g_AkaoControlFlags;
extern s32 D_80063000;
extern u32 D_80063004;
extern s32 g_AkaoCommandQueueId; // sound message queue count
extern u8 D_800716CC;
extern AkaoInstrument g_AkaoInstrument[];
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

typedef struct {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 padA;
} AkaoVoiceWork;
extern AkaoVoiceWork D_8009C5A0[24];

extern s32 D_80083338;
extern s32 D_80083398;
extern s32 D_8009A130;
extern s32 D_8009A134;
extern s32 D_8009A138;
extern s32 D_8009A144;
extern s16 D_8009A154;
extern s16 D_8009A15A;
extern s16 D_8009A15C;
extern s16 D_8009A15E;
extern s16 D_8009A162;
extern s32 D_8009A168;
extern s16 D_8009A1AE;
extern s32 g_Channel3OnMask;
extern s32 g_Channel3KeyedMask;
extern s32 g_Channel3Tempo;
extern s32 g_Channel3TempoUpdate;

#define READ_S8(addr) ((s8)(*(addr)++))
#define READ_S16(addr) ((s16)(*(addr)++ | (*(addr)++ << 8)))