#ifndef AKAO_PRIVATE_H
#define AKAO_PRIVATE_H

#include "common.h"
#include "game.h"
#include "libspu.h"
#include "akao.h"

#define AKAO_INSTR_COUNT 0x80
#define AKAO_INSTR2_FIRST 0x35 // first instrument owned by INSTR2
#define AKAO_NUM_VOICES 24

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
    /* 0x0B */ u8 sr;
    /* 0x0C */ u8 rr;
    /* 0x0D */ u8 aMode;
    /* 0x0E */ u8 sMode;
    /* 0x0F */ u8 rMode;
    /* 0x10 */ s32 pitch[12];
} AkaoInstrument; // size: 0x40

typedef struct {
    /* 0x0 */ u8 instrument;
    /* 0x1 */ u8 key;
    /* 0x2 */ u8 volume[2]; // little endian, unaligned
    /* 0x4 */ u8 pan;
} AkaoDrumKey; // size: 0x5

// Field names cross-checked against the independent qgears reverse-engineering
// project's AkaoChannel struct (same source as the g_Akao*SlideStep/Steps
// naming above): https://github.com/Akari1982/q-gears_reverse,
// ffvii/DISC/SCUS_941_akao.h. Spans neither this repo nor qgears resolves
// (LFO delay/rate sub-fields, the 0xA8-0xB8 gap) are left as unkNN.
typedef struct {
    /* 0x00 */ u8* akaoSequencePointer;
    /* 0x04 */ u8* loopPoint[4];
    /* 0x14 */ u8* drumOffset;
    /* 0x18 */ s16* vibratoWave;
    /* 0x1C */ s16* tremoloWave;
    /* 0x20 */ s16* panLfoWave;
    /* 0x24 */ u32 overlayChannelId;
    /* 0x28 */ s32 alternativeChannelId;
    /* 0x2C */ u32 volumeMultiplier;
    /* 0x30 */ u32 basePitch;
    /* 0x34 */ s32 pitchSlide;
    /* 0x38 */ u32 updateFlags;
    /* 0x3C */ u32 pitchMulSound;
    /* 0x40 */ s32 pitchMulSoundSlideStep;
    /* 0x44 */ s32 volumeLevel;
    /* 0x48 */ s32 volSlideStep;
    /* 0x4C */ s32 pitchSlideStep;
    /* 0x50 */ u32 setToMinusOne;
    /* 0x54 */ u16 playingType;
    /* 0x56 */ u16 length; // low 8bit: ticks to next note, high: ticks to key off
    /* 0x58 */ u16 currentInstrument;
    /* 0x5A */ u16 pitchMulSoundSlideSteps;
    /* 0x5C */ u16 volSlideSteps;
    /* 0x5E */ u16 volBalanceSlideSteps;
    /* 0x60 */ u16 volPan;
    /* 0x62 */ s16 volPanSlideSteps; // signed in opcode handlers, unsigned in slide updates
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

// Each sound effect slot occupies a stereo voice pair (2 audio channels, 0x210 bytes).
typedef struct {
    AkaoChannel voices[2];
} AkaoSoundSlot; // size: 0x210

typedef struct {
    /* 0x00 */ u16 opcode;
    /* 0x02 */ u16 pad;
    /* 0x04 */ s32 param0;
    /* 0x08 */ s32 param1;
    /* 0x0C */ s32 param2;
    /* 0x10 */ s32 param3;
    /* 0x14 */ s32 param4;
    /* 0x18 */ s32 param5;
    /* 0x1C */ s32 param6;
    /* 0x20 */ s32 param7;
} AkaoQueuedCommand; // size:0x24

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
    /* 0x4 */ u16 vol;
} AkaoSetCdVol;

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

typedef void (*AkaoCommandHandler)();
extern AkaoCommandHandler g_AkaoCommandHandler[0x100];
extern u8 g_AkaoOpcodeParamLength[0x60];
extern u8 g_AkaoOpcodeSize[0x100]; // opcode lengths
extern void (*g_AkaoOpcodeHandler[96])();
extern u16 g_AkaoLengthTable[11];
extern u8 g_AkaoDummyStopSequence[];
extern s16 g_AkaoLeftVolumeTable[0x100];
extern s16 g_AkaoRightVolumeTable[0x100];
extern s16 g_AkaoWaveTable[0x2C4];
extern s16* g_AkaoWaveTableKey[0x10];
extern u8 g_AkaoDefaultSound[0x20];

extern u32 g_AkaoSoundEvent;
extern s32 g_AkaoStreamMask;
extern u32 g_AkaoStreamLoopSize;
typedef struct {
    u16 flags;
    u16 pitch;
} AkaoStreamFormat;
extern AkaoStreamFormat g_AkaoStreamFormat;
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
extern u_long g_AkaoEffectsAll;
extern u_long g_AkaoEffectsAllSeq;
extern s32 g_AkaoMutex;
extern s32 g_AkaoStreamVol;
extern s32 g_AkaoStreamPan;
extern s32 g_AkaoCdVolSlideStep;
extern u16 g_AkaoReverbMul;
extern s16 g_AkaoCdVolSlideSteps;

extern AkaoCdVol g_AkaoCdVol;
extern u32 g_AkaoMuteMusicMask;
extern u8* g_AkaoStreamSrc;
extern s32 g_AkaoPitchMulMusic;
extern s32 g_AkaoTempoMulMusic;
extern s32 g_AkaoControlFlags;
extern u8* g_AkaoStreamLoopSrc;
extern u32 g_AkaoStreamRemainingBytes;
extern s32 g_AkaoCommandQueueId; // sound message queue count
extern AkaoVoiceAttr g_AkaoVoiceAttr[1];
extern u16 g_AkaoMusicFadeSteps; // music fade/transition steps (default 0x10)
extern AkaoChannel g_AkaoSavedChannels0[AKAO_NUM_VOICES];
extern AkaoChannel g_AkaoSavedChannels1[AKAO_NUM_VOICES];
extern AkaoQueuedCommand g_AkaoCommandQueue[32]; // sound messages queue
extern s32 g_AkaoMusicBuffer[];
extern AkaoChannel g_Channel1[];
extern AkaoChannel g_Channel2[];
extern s32 g_AkaoStreamVoice16UpdateMask;
extern s32 g_AkaoStreamVoice17UpdateMask;
extern AkaoSoundSlot g_AkaoSoundSlots[];
extern s32 g_AkaoMusicSlot; // 0 while g_Channel1 is being sequenced, 1 for g_Channel2

// Integer part of a 16.16 fixed point global, and the low byte of it.
#define FIXED_HI(x) (*((u16*)&(x) + 1))
#define FIXED_U8(x) (*((u8*)&(x) + 2))
extern SpuReverbAttr g_ReverbAttr;
extern SpuCommonAttr g_SpuCommonAttr;

typedef struct {
    s32 pitchSlide;
    s32 volSlide;
    u16 currentKey;
    s16 padA;
} AkaoVoiceWork;
extern AkaoVoiceWork g_AkaoVoiceWork[AKAO_NUM_VOICES];

extern s32 g_Channel2VoiceMask; // hardware voices lent to the second music slot
extern u16 g_AkaoLastHcount;    // VSync(1) horizontal count at the previous AkaoMain

extern AkaoInstrument g_AkaoInstrument[AKAO_INSTR_COUNT];
extern u8 g_AkaoSpuMallocRec[SPU_MALLOC_RECSIZ * (4 + 1)];
extern u8 g_AkaoEffectsBuffer[0xC800];

long AkaoMain(void);

#endif
