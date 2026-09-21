#include "common.h"
#include "game.h"
#include "libspu.h"

#define AKAO_PAN_LEFT 0x00
#define AKAO_PAN_CENTER 0x40
#define AKAO_PAN_RIGHT 0x7F

#define AKAO_SFX_SLOT_0 0x30
#define AKAO_SFX_SLOT_1 0x32
#define AKAO_SFX_SLOT_2 0x34
#define AKAO_SFX_SLOT_3 0x36


#define AKAO_MUSIC 0x0
#define AKAO_SOUND 0x1
#define AKAO_MENU 0x2

#define AKAO_STEREO 0x1
#define AKAO_MONO 0x2
#define AKAO_STEREO_CHANNELS 0x4

#define AKAO_SFX_LEGATO 0x1
#define AKAO_SFX_FULL_LENGTH 0x4

#define AKAO_UPDATE_SPU_VOICE (SPU_VOICE_VOLL | SPU_VOICE_VOLR)
#define AKAO_UPDATE_SPU_ADSR                                                                                           \
    (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_AR | SPU_VOICE_ADSR_DR |      \
     SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL)
#define AKAO_UPDATE_SPU_BASE_WOR                                                                                       \
    (SPU_VOICE_WDSA | SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_AR | SPU_VOICE_ADSR_DR |            \
     SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_SL | SPU_VOICE_LSAX)
#define AKAO_UPDATE_SPU_BASE (AKAO_UPDATE_SPU_BASE_WOR | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR)
#define AKAO_UPDATE_SPU_ALL (AKAO_UPDATE_SPU_BASE | AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH)

#define AKAO_UPDATE_VIBRATO 0x1
#define AKAO_UPDATE_TREMOLO 0x2
#define AKAO_UPDATE_PAN_LFO 0x4
#define AKAO_UPDATE_DRUM_MODE 0x8
#define AKAO_UPDATE_SIDE_CHAIN_PITCH 0x10
#define AKAO_UPDATE_SIDE_CHAIN_VOL 0x20
#define AKAO_UPDATE_OVERLAY 0x100
#define AKAO_UPDATE_ALTERNATIVE 0x200

#define AKAO_UPDATE_NOISE_CLOCK 0x10
#define AKAO_UPDATE_REVERB 0x80

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

// Each sound effect slot occupies a stereo voice pair (2 channels, 0x210 bytes).
typedef struct {
    AkaoChannel voices[2];
} AkaoSoundSlot; // size: 0x210

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

extern void (*D_80049548[0x100])();
extern u8 D_80049948[0x60];
extern u8 g_AkaoOpcodeSize[0x100]; // opcode lengths
extern void (*g_AkaoOpcodeHandler[96])();
extern u16 g_AkaoLengthTable[14];
extern u8 D_80049C40[];
extern s16 g_AkaoLeftVolumeTable[0x100];
extern s16 g_AkaoRightVolumeTable[0x100];
extern s16 g_AkaoWaveTable[0x2C4];
extern s16* g_AkaoWaveTableKey[0x10];
extern u8 g_AkaoDefaultSound[0x20];

extern u32 g_AkaoSoundEvent;
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
extern u32 g_AkaoMuteMusicMask;
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
extern u16 g_AkaoMusicFadeSteps; // music fade/transition steps (default 0x10)
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
extern u16 g_AkaoVoiceAttrSr;
extern u16 D_8007EC0A;
extern s16 g_AkaoVoiceAttrVolL;
extern s16 g_AkaoVoiceAttrVolR;
extern s32 g_AkaoSavedChannels0;
extern s32 g_AkaoSavedChannels1;
extern AkaoCommand D_80081DC8[32]; // sound messages queue
extern s32 g_AkaoSavedChannelConfig0;
extern u16 g_AkaoSavedMusicId0;
extern s32 g_AkaoSavedChannelConfig1;
extern u16 g_AkaoSavedMusicId1;
extern s32 g_AkaoMusicBuffer[];
extern AkaoChannel g_Channel1[];
extern AkaoChannel g_Channel2[];
extern s32 D_80097768;
extern s32 D_80097870;
extern AkaoSoundSlot g_AkaoSoundSlots[];
extern u16 g_Channel3NoiseClock;
extern u16 D_80099E0C;
extern s32 g_Channel3ActiveMask[];
extern s32 g_Channel3OffMask;
extern s32 g_AkaoSoundActiveMaskStored;
extern s32 g_AkaoNoiseMask;
extern s32 g_AkaoReverbMask;
extern s32 g_AkaoPitchLfoMask;
extern u16 g_AkaoMusicId;
extern s32 g_Channel1Config;
extern s32 g_Channel2Config;
extern s32 g_AkaoMusicActiveMask;
extern s32 g_AkaoMusicOnMask;
extern s32 g_AkaoMusicKeyedMask;
extern s32 g_AkaoMusicOffMask;
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

volatile s16 g_AkaoTransfer;

void AkaoCmd_10_PlayMusic(AkaoCommand* cmd);
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoCommand* cmd);
void AkaoCmd_15_PlayMusicSwapSaved(AkaoCommand* cmd);
void AkaoCmd_18_FadePlayMusic(AkaoCommand* cmd);
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoCommand* cmd);
void AkaoCmd_20(AkaoCommand* arg0);
void AkaoCmd_21_PlayTwoSounds(AkaoCommand* cmd);
void AkaoCmd_22_PlayThreeSounds(AkaoCommand* cmd);
void AkaoCmd_23_PlayFourSounds(AkaoCommand* cmd);
void AkaoCmd_29(AkaoCommand* arg0);
void AkaoCmd_2A(AkaoCommand* arg0);
void AkaoCmd_2B(AkaoCommand* arg0);
void AkaoCmd_30(AkaoCommand* arg0);
void AkaoCmd_34_PlaySoundDirect(AkaoCommand* cmd);
static void AkaoCmd_80(void);
static void AkaoCmd_81_SetMonoMode(void);
void AkaoCmd_82();
void AkaoCmd_90();
void AkaoCmd_92();
void AkaoCmd_9A_FlushPendingMusicUpdates(void);
void AkaoCmd_9B_ApplyPendingMusicUpdates(void);
void AkaoCmd_9C_FlushPendingSoundUpdates(void);
void AkaoCmd_9D_ApplyPendingSoundUpdates(void);
void AkaoCmd_A0(void* arg0);
void AkaoCmd_A1(void* arg0);
void AkaoCmd_A2(void* arg0);
void AkaoCmd_A3(void* arg0);
void AkaoCmd_A4(void* arg0);
void AkaoCmd_A5(void* arg0);
void AkaoCmd_A6(void* arg0);
void AkaoCmd_A7(void* arg0);
void AkaoCmd_A8(void* arg0);
void AkaoCmd_A9(void* arg0);
void AkaoCmd_AA(void* arg0);
void AkaoCmd_AB(void* arg0);
void AkaoCmd_AC(s32 arg0);
void AkaoCmd_AD(s32 arg0);
void AkaoCmd_AE(s32 arg0);
void AkaoCmd_AF(s32 arg0);
void AkaoCmd_B0(AkaoCommand* cmd);
void AkaoCmd_B1(AkaoCommand* cmd);
void AkaoCmd_B2(AkaoCommand* cmd);
void AkaoCmd_B3(AkaoCommand* cmd);
void AkaoCmd_B4(AkaoCommand* cmd);
void AkaoCmd_B5(AkaoCommand* cmd);
void AkaoCmd_B6(AkaoCommand* cmd);
void AkaoCmd_B7(AkaoCommand* cmd);
void AkaoCmd_B8_SetAllSoundVolBalance(AkaoCommand* cmd);
void AkaoCmd_B9_SlideAllSoundVolBalance(AkaoCommand* cmd);
void AkaoCmd_BA(void* arg0);
void AkaoCmd_BB(void* arg0);
void AkaoCmd_BC_SetAllSoundPitch(AkaoCommand* cmd);
void AkaoCmd_BD_SlideAllSoundPitch(AkaoCommand* cmd);
void AkaoCmd_C0_VolumeSet(AkaoCommand* arg0);
void AkaoCmd_C1_VolSlideFromCurr(AkaoVolSlideFromCurr* arg0);
void AkaoCmd_C2_VolSlideBetweenTargets(AkaoVolSlideBetweenTargets* arg0);
void AkaoCmd_C8();
void AkaoCmd_C9_CdVolSlideFromCurr(AkaoCdVolSlideFromCurr* arg0);
void AkaoCmd_CA_CdVolSlideBetweenTargets(AkaoCdVolSlideBetweenTargets* arg0);
void AkaoCmd_D0(AkaoTempoPitchSlide* arg0);
void AkaoCmd_D1_TempoSlideFromCurr(AkaoSlideFromCurr* arg0);
void AkaoCmd_D2_TempoSlideBetweenTargets(AkaoTempoPitchSlide* arg0);
void AkaoCmd_D4(AkaoTempoPitchSlide* arg0);
void AkaoCmd_D5_PitchSlideFromCurr(AkaoSlideFromCurr* arg0);
void AkaoCmd_D6_PitchSlideBetweenTargets(AkaoTempoPitchSlide* arg0);
static void AkaoCmd_E0_SetReverbPan(AkaoSetReverbPan* arg0);
static void AkaoCmd_E4_SetReverbMul(AkaoSetReverbMul* arg0);
static void AkaoCmd_F0(void);
static void AkaoCmd_F1(void);
static void AkaoCmd_F2(void);
static void AkaoCmd_F3(void);
void AkaoCmd_F4();
void AkaoCmd_F5();
static void AkaoCmd_F8_StreamReverbMaskClear(void);
static void AkaoCmd_F9_StreamReverbMaskRestore(void);
static void AkaoCmd_FA(void);
void AkaoCmd_Null(AkaoCommand* arg0);
void AkaoOp_A0_FinishChannel();
void AkaoOp_A1_LoadInstrument();
static void AkaoOp_A2_NextNoteLength(AkaoChannel* track);
static void AkaoOp_A3_MasterVol(AkaoChannel* track);
void AkaoOp_A4_PitchBendSlide();
static void AkaoOp_A5_SetOctave(AkaoChannel* track);
static void AkaoOp_A6_IncOctave(AkaoChannel* track);
static void AkaoOp_A7_DecOctave(AkaoChannel* track);
static void AkaoOp_A8_SetVol(AkaoChannel* track);
void AkaoOp_A9_SetVolSlide();
static void AkaoOp_AA_SetPan(AkaoChannel* track);
static void AkaoOp_AB_SetPanSlide(AkaoChannel* track);
void AkaoOp_AC_NoiseClockFreq();
void AkaoOp_AD_SetAr();
void AkaoOp_AE_SetDr();
void AkaoOp_AF_SetSl();
static void AkaoOp_B0_SetVoiceDrSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B1_SetSr();
void AkaoOp_B2_SetRr();
void AkaoOp_B3_ResetAdsr();
void AkaoOp_B4_Vibrato();
void AkaoOp_B5_VibratoDepth();
static void AkaoOp_B6_VibratoOff(AkaoChannel* track);
void AkaoOp_B7_AttackMode();
void AkaoOp_B8_Tremolo();
static void AkaoOp_B9_TremoloDepth(AkaoChannel* track);
static void AkaoOp_BA_TremoloOff(AkaoChannel* track);
void AkaoOp_BB_SustainMode();
static void AkaoOp_BC_SetPanLfo(AkaoChannel* track);
static void AkaoOp_BD_PanLfoDepth(AkaoChannel* track);
static void AkaoOp_BE_PanLfoOff(AkaoChannel* track);
void AkaoOp_BF_ReleaseMode();
static void AkaoOp_C0_TransposeAbsolute(AkaoChannel* track);
static void AkaoOp_C1_TransposeRelative(AkaoChannel* track);
static void AkaoOp_C2_ReverbOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C3_ReverbOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C4_NoiseOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C5_NoiseOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C6_PitchLfoOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C7_PitchLfoOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_C8_LoopPoint(AkaoChannel* track);
void AkaoOp_C9_LoopReturnTimes();
void AkaoOp_CA_LoopReturn(AkaoChannel* track);
static void AkaoOp_CB_SfxReset(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_CC_LegatoOn(AkaoChannel* track);
static void AkaoOp_CD_LegatoOff(void);
static void AkaoOp_CE_NoiseSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_CF_NoiseSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D0_FullLengthOn(AkaoChannel* track);
static void AkaoOp_D1_FullLengthOff(void);
static void AkaoOp_D2_FrequencyModulationSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D3_FrequencyModulationSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D4_SideChainPlaybackOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D5_SideChainPlaybackOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D6_SideChainPitchVolOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D7_SideChainPitchVolOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_D8_FineTuningAbsolute(AkaoChannel* track);
static void AkaoOp_D9_FineTuningRelative(AkaoChannel* track);
static void AkaoOp_DA_PortamentoOn(AkaoChannel* track);
static void AkaoOp_DB_PortamentoOff(AkaoChannel* track);
static void AkaoOp_DC_FixNoteLength(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_DD_VibratoDepthSlide();
static void AkaoOp_DE_TremoloDepthSlideFromCurr(AkaoChannel* track);
static void AkaoOp_DF_PanLfoDepthSlideFromCurr(AkaoChannel* track);
void AkaoOp_E8_Tempo();
void AkaoOp_E9_TempoSlide();
static void AkaoOp_EA_ReverbDepth(u8** cursor, AkaoChannel* track);
void AkaoOp_EB_ReverbDepthSlide();
static void AkaoOp_EC_DrumModeOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_ED_DrumModeOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EE_Jump(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EF_JumpConditional(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F0_LoopJumpTimes();
void AkaoOp_F1_LoopBreakTimes();
void AkaoOp_F2_LoadInstrument();
void AkaoOp_F4_OverlayVoiceOn();
void AkaoOp_F5_OverlayVoiceOff();
static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track);
void AkaoOp_F7_OverlayVolBalanceSlide();
void AkaoOp_F8_AltVoiceOn();
void AkaoOp_F9_AltVoiceOff();
static void AkaoOp_FD_TimeSignature(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_FE_MeasureNumber(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_Null(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void func_800335CC(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);

s32 D_80049538[4] = {0, 0, 0, 0};

void (*D_80049548[0x100])() = {
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_10_PlayMusic,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_14_PlayMusicSaveCurrent,
    AkaoCmd_15_PlayMusicSwapSaved,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_18_FadePlayMusic,
    AkaoCmd_19_FadePlayMusicSaveCurrent,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_20,
    AkaoCmd_21_PlayTwoSounds,
    AkaoCmd_22_PlayThreeSounds,
    AkaoCmd_23_PlayFourSounds,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_20,
    AkaoCmd_29,
    AkaoCmd_2A,
    AkaoCmd_2B,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_30,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_34_PlaySoundDirect,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_80,
    AkaoCmd_81_SetMonoMode,
    AkaoCmd_82,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_90,
    AkaoCmd_Null,
    AkaoCmd_92,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_9A_FlushPendingMusicUpdates,
    AkaoCmd_9B_ApplyPendingMusicUpdates,
    AkaoCmd_9C_FlushPendingSoundUpdates,
    AkaoCmd_9D_ApplyPendingSoundUpdates,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_A0,
    AkaoCmd_A1,
    AkaoCmd_A2,
    AkaoCmd_A3,
    AkaoCmd_A4,
    AkaoCmd_A5,
    AkaoCmd_A6,
    AkaoCmd_A7,
    AkaoCmd_A8,
    AkaoCmd_A9,
    AkaoCmd_AA,
    AkaoCmd_AB,
    AkaoCmd_AC,
    AkaoCmd_AD,
    AkaoCmd_AE,
    AkaoCmd_AF,
    AkaoCmd_B0,
    AkaoCmd_B1,
    AkaoCmd_B2,
    AkaoCmd_B3,
    AkaoCmd_B4,
    AkaoCmd_B5,
    AkaoCmd_B6,
    AkaoCmd_B7,
    AkaoCmd_B8_SetAllSoundVolBalance,
    AkaoCmd_B9_SlideAllSoundVolBalance,
    AkaoCmd_BA,
    AkaoCmd_BB,
    AkaoCmd_BC_SetAllSoundPitch,
    AkaoCmd_BD_SlideAllSoundPitch,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_C0_VolumeSet,
    AkaoCmd_C1_VolSlideFromCurr,
    AkaoCmd_C2_VolSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_C8,
    AkaoCmd_C9_CdVolSlideFromCurr,
    AkaoCmd_CA_CdVolSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_D0,
    AkaoCmd_D1_TempoSlideFromCurr,
    AkaoCmd_D2_TempoSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_D4,
    AkaoCmd_D5_PitchSlideFromCurr,
    AkaoCmd_D6_PitchSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_E0_SetReverbPan,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_E4_SetReverbMul,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_F0,
    AkaoCmd_F1,
    AkaoCmd_F2,
    AkaoCmd_F3,
    AkaoCmd_F4,
    AkaoCmd_F5,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_F8_StreamReverbMaskClear,
    AkaoCmd_F9_StreamReverbMaskRestore,
    AkaoCmd_FA,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
};

u8 D_80049948[0x60] = {
    0x00, 0x02, 0x02, 0x02, 0x03, 0x02, 0x01, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x02, 0x02, 0x01, 0x04, 0x02, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02, 0x03, 0x02, 0x01, 0x02,
    0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02,
    0x01, 0x00, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x00, 0x02, 0x03, 0x03, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x04, 0x03, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x01, 0x03, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00,
};

u8 g_AkaoOpcodeSize[0x100] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x03, 0x02, 0x01, 0x01, 0x02, 0x03, 0x02,
    0x03, 0x02, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x01, 0x04, 0x02, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02, 0x03, 0x02,
    0x01, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01,
    0x01, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x01, 0x02, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void (*g_AkaoOpcodeHandler[96])() = {
    AkaoOp_A0_FinishChannel,
    AkaoOp_A1_LoadInstrument,
    AkaoOp_A2_NextNoteLength,
    AkaoOp_A3_MasterVol,
    AkaoOp_A4_PitchBendSlide,
    AkaoOp_A5_SetOctave,
    AkaoOp_A6_IncOctave,
    AkaoOp_A7_DecOctave,
    AkaoOp_A8_SetVol,
    AkaoOp_A9_SetVolSlide,
    AkaoOp_AA_SetPan,
    AkaoOp_AB_SetPanSlide,
    AkaoOp_AC_NoiseClockFreq,
    AkaoOp_AD_SetAr,
    AkaoOp_AE_SetDr,
    AkaoOp_AF_SetSl,
    AkaoOp_B0_SetVoiceDrSl,
    AkaoOp_B1_SetSr,
    AkaoOp_B2_SetRr,
    AkaoOp_B3_ResetAdsr,
    AkaoOp_B4_Vibrato,
    AkaoOp_B5_VibratoDepth,
    AkaoOp_B6_VibratoOff,
    AkaoOp_B7_AttackMode,
    AkaoOp_B8_Tremolo,
    AkaoOp_B9_TremoloDepth,
    AkaoOp_BA_TremoloOff,
    AkaoOp_BB_SustainMode,
    AkaoOp_BC_SetPanLfo,
    AkaoOp_BD_PanLfoDepth,
    AkaoOp_BE_PanLfoOff,
    AkaoOp_BF_ReleaseMode,
    AkaoOp_C0_TransposeAbsolute,
    AkaoOp_C1_TransposeRelative,
    AkaoOp_C2_ReverbOn,
    AkaoOp_C3_ReverbOff,
    AkaoOp_C4_NoiseOn,
    AkaoOp_C5_NoiseOff,
    AkaoOp_C6_PitchLfoOn,
    AkaoOp_C7_PitchLfoOff,
    AkaoOp_C8_LoopPoint,
    AkaoOp_C9_LoopReturnTimes,
    AkaoOp_CA_LoopReturn,
    AkaoOp_CB_SfxReset,
    AkaoOp_CC_LegatoOn,
    AkaoOp_CD_LegatoOff,
    AkaoOp_CE_NoiseSwitch,
    AkaoOp_CF_NoiseSwitch,
    AkaoOp_D0_FullLengthOn,
    AkaoOp_D1_FullLengthOff,
    AkaoOp_D2_FrequencyModulationSwitch,
    AkaoOp_D3_FrequencyModulationSwitch,
    AkaoOp_D4_SideChainPlaybackOn,
    AkaoOp_D5_SideChainPlaybackOff,
    AkaoOp_D6_SideChainPitchVolOn,
    AkaoOp_D7_SideChainPitchVolOff,
    AkaoOp_D8_FineTuningAbsolute,
    AkaoOp_D9_FineTuningRelative,
    AkaoOp_DA_PortamentoOn,
    AkaoOp_DB_PortamentoOff,
    AkaoOp_DC_FixNoteLength,
    AkaoOp_DD_VibratoDepthSlide,
    AkaoOp_DE_TremoloDepthSlideFromCurr,
    AkaoOp_DF_PanLfoDepthSlideFromCurr,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_E8_Tempo,
    AkaoOp_E9_TempoSlide,
    AkaoOp_EA_ReverbDepth,
    AkaoOp_EB_ReverbDepthSlide,
    AkaoOp_EC_DrumModeOn,
    AkaoOp_ED_DrumModeOff,
    AkaoOp_EE_Jump,
    AkaoOp_EF_JumpConditional,
    AkaoOp_F0_LoopJumpTimes,
    AkaoOp_F1_LoopBreakTimes,
    AkaoOp_F2_LoadInstrument,
    func_800335CC,
    AkaoOp_F4_OverlayVoiceOn,
    AkaoOp_F5_OverlayVoiceOff,
    AkaoOp_F6_OverlayVolBalance,
    AkaoOp_F7_OverlayVolBalanceSlide,
    AkaoOp_F8_AltVoiceOn,
    AkaoOp_F9_AltVoiceOff,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_Null,
    AkaoOp_FD_TimeSignature,
    AkaoOp_FE_MeasureNumber,
    AkaoOp_Null,
};

u16 g_AkaoLengthTable[14] = {
    0xC0C0, 0x6060, 0x3030, 0x1818, 0x0C0C, 0x0606, 0x0303, 0x2020, 0x1010, 0x0808, 0x0404, 0x0000, 0x00A0, 0x0000,
};

s16 g_AkaoLeftVolumeTable[0x100] = {
    0x7F80, 0x7E80, 0x7D80, 0x7C80, 0x7B80, 0x7A80, 0x7980, 0x7880, 0x7780, 0x7680, 0x7580, 0x7480, 0x7380, 0x7280,
    0x7180, 0x7080, 0x6F80, 0x6E80, 0x6D80, 0x6C80, 0x6B80, 0x6A80, 0x6980, 0x6880, 0x6780, 0x6680, 0x6580, 0x6480,
    0x6380, 0x6280, 0x6180, 0x6080, 0x5F80, 0x5E80, 0x5D80, 0x5C80, 0x5B80, 0x5A80, 0x5980, 0x5880, 0x5780, 0x5680,
    0x5580, 0x5480, 0x5380, 0x5280, 0x5180, 0x5080, 0x4F80, 0x4E80, 0x4D80, 0x4C80, 0x4B80, 0x4A80, 0x4980, 0x4880,
    0x4780, 0x4680, 0x4580, 0x4480, 0x4380, 0x4280, 0x4180, 0x4080, 0x3F80, 0x3E80, 0x3D80, 0x3C80, 0x3B80, 0x3A80,
    0x3980, 0x3880, 0x3780, 0x3680, 0x3580, 0x3480, 0x3380, 0x3280, 0x3180, 0x3080, 0x2F80, 0x2E80, 0x2D80, 0x2C80,
    0x2B80, 0x2A80, 0x2980, 0x2880, 0x2780, 0x2680, 0x2580, 0x2480, 0x2380, 0x2280, 0x2180, 0x2080, 0x1F80, 0x1E80,
    0x1D80, 0x1C80, 0x1B80, 0x1A80, 0x1980, 0x1880, 0x1780, 0x1680, 0x1580, 0x1480, 0x1380, 0x1280, 0x1180, 0x1080,
    0x0F80, 0x0E80, 0x0D80, 0x0C80, 0x0B80, 0x0A80, 0x0980, 0x0880, 0x0780, 0x0680, 0x0580, 0x0480, 0x0380, 0x0280,
    0x0180, 0x0080, 0x0000, 0xFFFF, 0xFFFC, 0xFFF7, 0xFFF0, 0xFFE7, 0xFFDC, 0xFFCF, 0xFFC0, 0xFFAF, 0xFF9C, 0xFF87,
    0xFF70, 0xFF57, 0xFF3C, 0xFF1F, 0xFF00, 0xFEDF, 0xFEBC, 0xFE97, 0xFE70, 0xFE47, 0xFE1C, 0xFDEF, 0xFDC0, 0xFD8F,
    0xFD5C, 0xFD27, 0xFCF0, 0xFCB7, 0xFC7C, 0xFC3F, 0xFC00, 0xFBBF, 0xFB7C, 0xFB37, 0xFAF0, 0xFAA7, 0xFA5C, 0xFA0F,
    0xF9C0, 0xF96F, 0xF91C, 0xF8C7, 0xF870, 0xF817, 0xF7BC, 0xF75F, 0xF700, 0xF69F, 0xF63C, 0xF5D7, 0xF570, 0xF507,
    0xF49C, 0xF42F, 0xF3C0, 0xF34F, 0xF2DC, 0xF267, 0xF1F0, 0xF177, 0xF0FC, 0xF07F, 0x1000, 0x1081, 0x1104, 0x1189,
    0x1210, 0x1299, 0x1324, 0x13B1, 0x1440, 0x14D1, 0x1564, 0x15F9, 0x1690, 0x1729, 0x17C4, 0x1861, 0x1900, 0x19A1,
    0x1A44, 0x1AE9, 0x1B90, 0x1C39, 0x1CE4, 0x1D91, 0x1E40, 0x1EF1, 0x1FA4, 0x2059, 0x2110, 0x21C9, 0x2284, 0x2341,
    0x2400, 0x24C1, 0x2584, 0x2649, 0x2710, 0x27D9, 0x28A4, 0x2971, 0x2A40, 0x2B11, 0x2BE4, 0x2CB9, 0x2D90, 0x2E69,
    0x2F44, 0x3021, 0x3100, 0x31E1, 0x32C4, 0x33A9, 0x3490, 0x3579, 0x3664, 0x3751, 0x3840, 0x3931, 0x3A24, 0x3B19,
    0x3C10, 0x3D09, 0x3E04, 0x3F01,
};

s16 g_AkaoRightVolumeTable[0x100] = {
    0x0080, 0x0180, 0x0280, 0x0380, 0x0480, 0x0580, 0x0680, 0x0780, 0x0880, 0x0980, 0x0A80, 0x0B80, 0x0C80, 0x0D80,
    0x0E80, 0x0F80, 0x1080, 0x1180, 0x1280, 0x1380, 0x1480, 0x1580, 0x1680, 0x1780, 0x1880, 0x1980, 0x1A80, 0x1B80,
    0x1C80, 0x1D80, 0x1E80, 0x1F80, 0x2080, 0x2180, 0x2280, 0x2380, 0x2480, 0x2580, 0x2680, 0x2780, 0x2880, 0x2980,
    0x2A80, 0x2B80, 0x2C80, 0x2D80, 0x2E80, 0x2F80, 0x3080, 0x3180, 0x3280, 0x3380, 0x3480, 0x3580, 0x3680, 0x3780,
    0x3880, 0x3980, 0x3A80, 0x3B80, 0x3C80, 0x3D80, 0x3E80, 0x3F80, 0x4080, 0x4180, 0x4280, 0x4380, 0x4480, 0x4580,
    0x4680, 0x4780, 0x4880, 0x4980, 0x4A80, 0x4B80, 0x4C80, 0x4D80, 0x4E80, 0x4F80, 0x5080, 0x5180, 0x5280, 0x5380,
    0x5480, 0x5580, 0x5680, 0x5780, 0x5880, 0x5980, 0x5A80, 0x5B80, 0x5C80, 0x5D80, 0x5E80, 0x5F80, 0x6080, 0x6180,
    0x6280, 0x6380, 0x6480, 0x6580, 0x6680, 0x6780, 0x6880, 0x6980, 0x6A80, 0x6B80, 0x6C80, 0x6D80, 0x6E80, 0x6F80,
    0x7080, 0x7180, 0x7280, 0x7380, 0x7480, 0x7580, 0x7680, 0x7780, 0x7880, 0x7980, 0x7A80, 0x7B80, 0x7C80, 0x7D80,
    0x7E80, 0x7F80, 0x3F01, 0x3E04, 0x3D09, 0x3C10, 0x3B19, 0x3A24, 0x3931, 0x3840, 0x3751, 0x3664, 0x3579, 0x3490,
    0x33A9, 0x32C4, 0x31E1, 0x3100, 0x3021, 0x2F44, 0x2E69, 0x2D90, 0x2CB9, 0x2BE4, 0x2B11, 0x2A40, 0x2971, 0x28A4,
    0x27D9, 0x2710, 0x2649, 0x2584, 0x24C1, 0x2400, 0x2341, 0x2284, 0x21C9, 0x2110, 0x2059, 0x1FA4, 0x1EF1, 0x1E40,
    0x1D91, 0x1CE4, 0x1C39, 0x1B90, 0x1AE9, 0x1A44, 0x19A1, 0x1900, 0x1861, 0x17C4, 0x1729, 0x1690, 0x15F9, 0x1564,
    0x14D1, 0x1440, 0x13DC, 0x1324, 0x1299, 0x1210, 0x1189, 0x1104, 0x1081, 0x1000, 0xF07F, 0xF0FC, 0xF177, 0xF1F0,
    0xF267, 0xF2DC, 0xF34F, 0xF3C0, 0xF42F, 0xF49C, 0xF507, 0xF570, 0xF5D7, 0xF63C, 0xF69F, 0xF700, 0xF75F, 0xF7BC,
    0xF817, 0xF870, 0xF8C7, 0xF91C, 0xF843, 0xF9C0, 0xFA0F, 0xFA5C, 0xFAA7, 0xFAF0, 0xFB37, 0xFB7C, 0xFBBF, 0xFC00,
    0xFC3F, 0xFC7C, 0xFCB7, 0xFCF0, 0xFD27, 0xFD5C, 0xFD8F, 0xFDC0, 0xFDEF, 0xFE1C, 0xFE47, 0xFE70, 0xFE97, 0xFEBC,
    0xFEDF, 0xFF00, 0xFF1F, 0xFF3C, 0xFF57, 0xFF70, 0xFF87, 0xFF9C, 0xFFAF, 0xFFC0, 0xFFCF, 0xFFDC, 0xFFE7, 0xFFF0,
    0xFFF7, 0xFFFC, 0xFFFF, 0x0000,
};

s16 g_AkaoWaveTable[0x2C4] = {
    0x1FFF, 0xE001, 0x3FFF, 0xC001, 0x5FFF, 0xA001, 0x7FFF, 0x8001, 0x0000, 0x0000, 0xFFFE, 0x0000, 0x7FFF, 0x8001,
    0x0000, 0x0000, 0xFFFE, 0x0000, 0x1FFF, 0x0000, 0x3FFF, 0x0000, 0x5FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0xFFFE,
    0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0xFFFE, 0xE001, 0x0000, 0xC001, 0x0000, 0xA001, 0x0000, 0x8001, 0x0000,
    0x0000, 0xFFFE, 0x8001, 0x0000, 0x8001, 0x0000, 0x0000, 0xFFFE, 0x0000, 0x0500, 0x09E2, 0x12CD, 0x169E, 0x19E1,
    0x1C81, 0x1E6D, 0x1F9A, 0x1FFF, 0x1F9C, 0x1E71, 0x1C86, 0x19E8, 0x16A7, 0x12D7, 0x0E90, 0x09EE, 0x050D, 0x0000,
    0xFB0C, 0xF62A, 0xF186, 0xED3D, 0xE96A, 0xE626, 0xE384, 0xE196, 0xE067, 0xE000, 0xE061, 0xE18A, 0xE373, 0xE60F,
    0xE94F, 0xED1D, 0xF163, 0xF604, 0xFAE5, 0x0000, 0x0A01, 0x13C4, 0x259A, 0x2D3C, 0x33C2, 0x3902, 0x3CDA, 0x3F34,
    0x3FFF, 0x3F38, 0x3CE2, 0x390D, 0x33D1, 0x2D4F, 0x25AF, 0x1D21, 0x13DD, 0x0A1B, 0x0000, 0xF618, 0xEC54, 0xE30C,
    0xDA7B, 0xD2D5, 0xCC4D, 0xC709, 0xC32D, 0xC0CF, 0xC000, 0xC0C3, 0xC315, 0xC6E6, 0xCC1F, 0xD29E, 0xDA3B, 0xE2C6,
    0xEC09, 0xF5CA, 0x0000, 0x0F02, 0x1DA6, 0x3867, 0x43DA, 0x4DA3, 0x5583, 0x5B47, 0x5ECE, 0x5FFF, 0x5ED4, 0x5B53,
    0x5594, 0x4DBA, 0x43F6, 0x3886, 0x2BB2, 0x1DCB, 0x0F29, 0x0000, 0xF124, 0xE27E, 0xD492, 0xC7B8, 0xBC40, 0xB273,
    0xAA8E, 0xA4C3, 0xA137, 0xA000, 0xA124, 0xA49F, 0xAA59, 0xB22E, 0xBBED, 0xC759, 0xD429, 0xE20E, 0xF0AF, 0x0000,
    0x1403, 0x2788, 0x4B34, 0x5A79, 0x6784, 0x7204, 0x79B5, 0x7E68, 0x7FFF, 0x7E71, 0x79C5, 0x721B, 0x67A3, 0x5A9E,
    0x4B5E, 0x3A43, 0x27BA, 0x1437, 0x0000, 0xEC30, 0xD8A8, 0xC619, 0xB4F6, 0xA5AB, 0x989A, 0x8E13, 0x865A, 0x819F,
    0x8000, 0x8186, 0x862A, 0x8DCC, 0x983E, 0xA53D, 0xB477, 0xC58D, 0xD813, 0xEB95, 0x0000, 0x0000, 0xFFD9, 0x0000,
    0x0000, 0x1403, 0x2788, 0x4B34, 0x5A79, 0x6784, 0x7204, 0x79B5, 0x7E68, 0x7FFF, 0x7E71, 0x79C5, 0x721B, 0x67A3,
    0x5A9E, 0x4B5E, 0x3A43, 0x27BA, 0x1437, 0x0000, 0xEC30, 0xD8A8, 0xC619, 0xB4F6, 0xA5AB, 0x989A, 0x8E13, 0x865A,
    0x819F, 0x8000, 0x8186, 0x862A, 0x8DCC, 0x983E, 0xA53D, 0xB477, 0xC58D, 0xD813, 0xEB95, 0x0000, 0x0000, 0xFFD9,
    0x0000, 0x09E2, 0x12CD, 0x19E1, 0x1E6D, 0x1FFF, 0x1E71, 0x19E8, 0x12D7, 0x09EE, 0x000D, 0xF62A, 0xED3D, 0xE626,
    0xE196, 0xE000, 0xE18A, 0xE60F, 0xED1D, 0xF604, 0x0000, 0x13C4, 0x259A, 0x33C2, 0x3CDA, 0x3FFF, 0x3CE2, 0x33D1,
    0x25AF, 0x13DD, 0x001A, 0xEC54, 0xDA7B, 0xCC4D, 0xC32D, 0xC000, 0xC315, 0xCC1F, 0xDA3B, 0xEC09, 0x0000, 0x1DA6,
    0x3867, 0x4DA3, 0x5B47, 0x5FFF, 0x5B53, 0x4DBA, 0x3886, 0x1DCB, 0x0027, 0xE27E, 0xC7B8, 0xB273, 0xA4C3, 0xA000,
    0xA49F, 0xB22E, 0xC759, 0xE20E, 0x0000, 0x2788, 0x4B34, 0x6784, 0x79B5, 0x7FFF, 0x79C5, 0x67A3, 0x4B5E, 0x27BA,
    0x0034, 0xD8A8, 0xB4F6, 0x989A, 0x865A, 0x8000, 0x862A, 0x983E, 0xB477, 0xD813, 0x0000, 0x0000, 0xFFEC, 0x0000,
    0x0000, 0x2788, 0x4B34, 0x6784, 0x79B5, 0x7FFF, 0x79C5, 0x67A3, 0x4B5E, 0x27BA, 0x0034, 0xD8A8, 0xB4F6, 0x989A,
    0x865A, 0x8000, 0x862A, 0x983E, 0xB477, 0xD813, 0x0000, 0x0000, 0xFFEC, 0x0000, 0x0000, 0x07FF, 0x0FFF, 0x17FF,
    0x1FFF, 0x17FF, 0x0FFF, 0x07FF, 0x0000, 0xF801, 0xF001, 0xE801, 0xE001, 0xE801, 0xF001, 0xF801, 0x0000, 0x0FFF,
    0x1FFF, 0x2FFF, 0x3FFF, 0x2FFF, 0x1FFF, 0x0FFF, 0x0000, 0xF001, 0xE001, 0xD001, 0xC001, 0xD001, 0xE001, 0xF001,
    0x0000, 0x17FF, 0x2FFF, 0x47FF, 0x5FFF, 0x47FF, 0x2FFF, 0x17FF, 0x0000, 0xE801, 0xD001, 0xB801, 0xA001, 0xB801,
    0xD001, 0xE801, 0x0000, 0x1FFF, 0x3FFF, 0x5FFF, 0x7FFF, 0x5FFF, 0x3FFF, 0x1FFF, 0x0000, 0xE001, 0xC001, 0xA001,
    0x8001, 0xA001, 0xC001, 0xE001, 0x0000, 0x0000, 0xFFF0, 0x0000, 0x0000, 0x1FFF, 0x3FFF, 0x5FFF, 0x7FFF, 0x5FFF,
    0x3FFF, 0x1FFF, 0x0000, 0xE001, 0xC001, 0xA001, 0x8001, 0xA001, 0xC001, 0xE001, 0x0000, 0x0000, 0xFFF0, 0x0000,
    0x0000, 0x31FD, 0x7D05, 0xF5FD, 0xECEE, 0x793C, 0x75D0, 0x07FC, 0xF411, 0xC2F5, 0x660F, 0x330E, 0x3FEA, 0xBEE1,
    0x8C12, 0x1821, 0xAFFB, 0x4A00, 0xD3E7, 0xF4F5, 0xE137, 0xE7D3, 0x520D, 0x1103, 0xAEDF, 0x4AF8, 0x0746, 0x70F4,
    0x8EC1, 0x9719, 0x3512, 0xF600, 0x1B00, 0xB4E9, 0x3B1E, 0xAE15, 0x9CC8, 0x65F9, 0xC323, 0x71F8, 0x2CF1, 0xA604,
    0x92FC, 0xC90E, 0xB009, 0x10E5, 0x0804, 0x8001, 0x12FA, 0x1118, 0xE709, 0x47F8, 0x95D9, 0xDD1C, 0x1B09, 0x0BF4,
    0x7BFC, 0x20FF, 0x9404, 0x7DFC, 0x8AF5, 0xB717, 0x74F9, 0x2AFC, 0xDD06, 0xD7F3, 0xF4FD, 0x0608, 0xF218, 0x69DF,
    0xBC07, 0x4AF7, 0x89EB, 0xBB27, 0x1109, 0x7FEF, 0x3615, 0xECCD, 0x7621, 0x9815, 0x56CF, 0xD406, 0x3322, 0xDFE6,
    0xAF0A, 0xB9F5, 0xCE08, 0x5D05, 0xBFEE, 0xF01A, 0x20E8, 0x9F16, 0x8CF2, 0xC2E7, 0xD22E, 0xC601, 0xB9D0, 0x810C,
    0x4323, 0x28DD, 0x96F7, 0x5C3C, 0x9FDE, 0xAEE8, 0xB61B, 0x3EF3, 0x1408, 0xDAE9, 0x2C0B, 0xB133, 0xA8E1, 0x05DA,
    0x0914, 0x4E22, 0xFFE8, 0x87F1, 0x21F0, 0x8D25, 0x7DFC, 0x1CF0, 0x5E11, 0x3DE8, 0xC70A, 0x8F17, 0xFFF0, 0x6A0D,
    0xFFE3, 0x12F3, 0x041F, 0x780C, 0xC4EF, 0x5B02, 0x3D02, 0x02F2, 0x1519, 0xDAD5, 0xBD09, 0x9219, 0xF503, 0x62DD,
    0x3806, 0x0F26, 0xA8FB, 0xE0FE, 0xF6E3, 0x0E02, 0x0210, 0x4406, 0x94D4, 0xD10A, 0xA846, 0x10F1, 0x1ACC, 0xBDFD,
    0x1A30, 0xA0EB, 0x5FEA, 0xBF10, 0xBBEF, 0xDF1B, 0xC20C, 0x2AE0, 0x3EF2, 0xAE27, 0x5001, 0xF9DD, 0x3611, 0xAD1A,
    0x07DF, 0x6D05, 0x1A0D, 0xB6DB, 0x6D2B, 0x3607, 0xFBCC, 0x4208, 0x0F21, 0x36F6, 0xC806, 0x0603, 0xFDD7, 0x7817,
    0x0605, 0xAF0B, 0x95FC, 0x10EC, 0xA70E, 0xADF3, 0x660F, 0xBDF2, 0xDEF8, 0xE421, 0xB1E7, 0xFCED, 0x5B1C, 0xA109,
    0xCCF2, 0xF5F3, 0x9AFB, 0xB417, 0x92E8, 0x7F08, 0x4BFA, 0x89FD, 0x3C1C, 0xFCEF, 0xF2DB, 0x4A1D, 0x671D, 0xD6FF,
    0x65DA, 0xF1FC, 0x6E0A, 0x0A0C, 0x11F8, 0x4A02, 0x5BFC, 0xEB06, 0xA5ED, 0x30E6, 0x8011, 0xD234, 0x08F8, 0x43DC,
    0xF609, 0xA6F2, 0x9A0D, 0x7006, 0xF5EA, 0x9CFC, 0x811E, 0xADE6, 0xE60E, 0x95F7, 0xB304, 0x9CFF, 0xEFEA, 0xF519,
    0xFA00, 0x3AF6, 0xC7D5, 0x2722, 0xBB38, 0x43D1, 0x77ED, 0x4908, 0xCE02, 0xAB0F, 0x3DEB, 0xDB0A, 0x6401, 0x3FF9,
    0x76F2, 0x9309, 0xD20F, 0x74E5, 0x0000, 0x0000, 0xFF00, 0x0000,
};

s16* g_AkaoWaveTableKey[0x10] = {
    g_AkaoWaveTable,         &g_AkaoWaveTable[0xC],   &g_AkaoWaveTable[0x12],  &g_AkaoWaveTable[0x1C],
    &g_AkaoWaveTable[0x22],  &g_AkaoWaveTable[0x2C],  &g_AkaoWaveTable[0x32],  &g_AkaoWaveTable[0xD2],
    &g_AkaoWaveTable[0x168], &g_AkaoWaveTable[0x1AC], &g_AkaoWaveTable[0xFC],  &g_AkaoWaveTable[0x150],
    &g_AkaoWaveTable[0x1C0], &g_AkaoWaveTable[0x1AC], &g_AkaoWaveTable[0x168], &g_AkaoWaveTable[0x1AC],
};

u8 g_AkaoDefaultSound[0x20] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
