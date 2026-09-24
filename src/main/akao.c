//! PSYQ=3.3 CC1=2.6.3 G=8 COMM=true

#include "common.h"
#include "game.h"
#include "libspu.h"
#include "akao.h"

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
    /* 0x18 */ s16* vibratoWave;
    /* 0x1C */ s16* tremoloWave;
    /* 0x20 */ s16* panLfoWave;
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
extern u16 g_AkaoLengthTable[14];
#define g_AkaoDummyStopSequence ((u8*)&g_AkaoLengthTable[12])
extern s16 g_AkaoLeftVolumeTable[0x100];
extern s16 g_AkaoRightVolumeTable[0x100];
extern s16 g_AkaoWaveTable[0x2C4];
extern s16* g_AkaoWaveTableKey[0x10];
extern u8 g_AkaoDefaultSound[0x20];

extern u32 g_AkaoSoundEvent;
extern s32 g_AkaoStreamMask;
extern u32 g_AkaoStreamLoopSize;
extern u16 g_AkaoStreamPitch;
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
extern s32 g_AkaoStreamVol;
extern s32 g_AkaoStreamPan;
extern s32 g_AkaoCdVolSlideStep;
extern u16 g_AkaoReverbMul;
extern u16 g_AkaoCdVolSlideSteps;

extern AkaoCdVol g_AkaoCdVol;
extern u32 g_AkaoMuteMusicMask;
extern u8* g_AkaoStreamSrc;
extern s32 g_AkaoPitchMulMusic;
extern s32 g_AkaoTempoMulMusic;
extern s32 g_AkaoControlFlags;
extern u8* g_AkaoStreamLoopSrc;
extern u32 g_AkaoStreamRemainingBytes;
extern s32 g_AkaoCommandQueueId; // sound message queue count
// Voice attribute work block (0x8007EBE4, size 0x2C).
// NOTE: SPU update calls pass '&g_AkaoVoiceAttr', but functions in akao.c assign to the
// individual 'g_AkaoVoiceAttrXxx' globals below. Do not replace individual global accesses
// with struct member accesses, or GCC 2.7.2 will hoist the base address and break byte matching.
extern u8 g_AkaoVoiceAttr[];
extern u16 g_AkaoMusicFadeSteps; // music fade/transition steps (default 0x10)
extern s32 g_AkaoVoiceAttrMask;
extern s32 g_AkaoVoiceAttrAddr;
extern s32 g_AkaoVoiceAttrLoopAddr;
extern s32 g_AkaoVoiceAttrAMode;
extern s32 g_AkaoVoiceAttrSMode;
extern s32 g_AkaoVoiceAttrRMode;
extern u16 g_AkaoVoiceAttrPitch;
extern u16 g_AkaoVoiceAttrAr;
extern u16 g_AkaoVoiceAttrDr;
extern u16 g_AkaoVoiceAttrSl;
extern u16 g_AkaoVoiceAttrSr;
extern u16 g_AkaoVoiceAttrRr;
extern s16 g_AkaoVoiceAttrVolL;
extern s16 g_AkaoVoiceAttrVolR;
extern AkaoChannel g_AkaoSavedChannels0[24];
extern AkaoChannel g_AkaoSavedChannels1[24];
extern AkaoQueuedCommand g_AkaoCommandQueue[32]; // sound messages queue
extern s32 g_AkaoSavedChannelConfig0;
extern u16 g_AkaoSavedMusicId0;
extern s32 g_AkaoSavedChannelConfig1;
extern u16 g_AkaoSavedMusicId1;
extern s32 g_AkaoMusicBuffer[];
extern AkaoChannel g_Channel1[];
extern AkaoChannel g_Channel2[];
extern s32 g_AkaoStreamVoice16UpdateMask;
extern s32 g_AkaoStreamVoice17UpdateMask;
extern AkaoSoundSlot g_AkaoSoundSlots[];
extern u16 g_Channel3NoiseClock;
extern u16 g_AkaoSoundChannelsMode;
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
extern s32 g_AkaoGlobalUpdateFlags;
extern u32 g_ReverbMode;
extern SpuReverbAttr g_ReverbAttr;
extern SpuCommonAttr g_SpuCommonAttr;

typedef struct {
    s32 pitchSlide;
    s32 volSlide;
    s16 currentKey;
    s16 padA;
} AkaoVoiceWork;
extern AkaoVoiceWork g_AkaoVoiceWork[24];

extern s32 g_AkaoSavedMusicActiveMask0;
extern s32 g_AkaoSavedMusicActiveMask1;
extern s32 g_Channel3OnMask;
extern s32 g_Channel3KeyedMask;
extern s32 g_Channel3Tempo;
extern s32 g_Channel3TempoUpdate;

#define READ_S8(addr) ((s8)(*(addr)++))
#define READ_S16(addr) ((s16)(*(addr)++ | (*(addr)++ << 8)))

volatile s16 g_AkaoTransfer;

void AkaoCmd_10_PlayMusic(AkaoQueuedCommand* cmd);
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoQueuedCommand* cmd);
void AkaoCmd_15_PlayMusicSwapSaved(AkaoQueuedCommand* cmd);
void AkaoCmd_18_FadePlayMusic(AkaoQueuedCommand* cmd);
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoQueuedCommand* cmd);
void AkaoCmd_20_PlaySound(AkaoQueuedCommand* cmd);
void AkaoCmd_21_PlayTwoSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_22_PlayThreeSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_23_PlayFourSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_29_PlaySoundSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_2A_PlaySoundSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_2B_PlaySoundSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_30_PlayMenuSound(AkaoQueuedCommand* cmd);
void AkaoCmd_34_PlaySoundDirect(AkaoQueuedCommand* cmd);
static void AkaoCmd_80_SetStereoMode(void);
static void AkaoCmd_81_SetMonoMode(void);
void AkaoCmd_82_ResetMusicAndSoundVol();
void AkaoCmd_90_SetMuteMusicMask();
void AkaoCmd_92_SetCondition();
void AkaoCmd_9A_FlushPendingMusicUpdates(void);
void AkaoCmd_9B_ApplyPendingMusicUpdates(void);
void AkaoCmd_9C_FlushPendingSoundUpdates(void);
void AkaoCmd_9D_ApplyPendingSoundUpdates(void);
void AkaoUpdateChannelParamsToSpu(s32 voiceIdx, void* attr);
void AkaoUpdateNoiseVoices(void);
void AkaoUpdateReverbVoices(void);
void AkaoUpdatePitchLfoVoices(void);

void AkaoCmd_A0_SetSoundVolBalanceSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A1_SetSoundVolBalanceSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_A2_SetSoundVolBalanceSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_A3_SetSoundVolBalanceSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_A4_SlideSoundVolBalanceSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A5_SlideSoundVolBalanceSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_A6_SlideSoundVolBalanceSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_A7_SlideSoundVolBalanceSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_A8_SetSoundPanSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A9_SetSoundPanSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_AA_SetSoundPanSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_AB_SetSoundPanSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_AC_SlideSoundPanSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_AD_SlideSoundPanSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_AE_SlideSoundPanSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_AF_SlideSoundPanSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B0_SetSoundPitchSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_B1_SetSoundPitchSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_B2_SetSoundPitchSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_B3_SetSoundPitchSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B4_SlideSoundPitchSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_B5_SlideSoundPitchSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_B6_SlideSoundPitchSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_B7_SlideSoundPitchSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B8_SetAllSoundVolBalance(AkaoQueuedCommand* cmd);
void AkaoCmd_B9_SlideAllSoundVolBalance(AkaoQueuedCommand* cmd);
void AkaoCmd_BA_SetAllSoundPan(AkaoQueuedCommand* cmd);
void AkaoCmd_BB_SlideAllSoundPan(AkaoQueuedCommand* cmd);
void AkaoCmd_BC_SetAllSoundPitch(AkaoQueuedCommand* cmd);
void AkaoCmd_BD_SlideAllSoundPitch(AkaoQueuedCommand* cmd);
void AkaoCmd_C0_VolumeSet(AkaoQueuedCommand* cmd);
void AkaoCmd_C1_VolSlideFromCurr(AkaoVolSlideFromCurr* cmd);
void AkaoCmd_C2_VolSlideBetweenTargets(AkaoVolSlideBetweenTargets* cmd);
void AkaoCmd_C8_SetCdVol();
void AkaoCmd_C9_CdVolSlideFromCurr(AkaoCdVolSlideFromCurr* cmd);
void AkaoCmd_CA_CdVolSlideBetweenTargets(AkaoCdVolSlideBetweenTargets* cmd);
void AkaoCmd_D0_SetTempo(AkaoTempoPitchSlide* cmd);
void AkaoCmd_D1_TempoSlideFromCurr(AkaoSlideFromCurr* cmd);
void AkaoCmd_D2_TempoSlideBetweenTargets(AkaoTempoPitchSlide* cmd);
void AkaoCmd_D4_SetPitch(AkaoTempoPitchSlide* cmd);
void AkaoCmd_D5_PitchSlideFromCurr(AkaoSlideFromCurr* cmd);
void AkaoCmd_D6_PitchSlideBetweenTargets(AkaoTempoPitchSlide* cmd);
static void AkaoCmd_E0_SetReverbPan(AkaoSetReverbPan* cmd);
static void AkaoCmd_E4_SetReverbMul(AkaoSetReverbMul* cmd);
static void AkaoCmd_F0_StopMusic(void);
static void AkaoCmd_F1_StopAllSounds(void);
static void AkaoCmd_F2_ClearSavedMusic0(void);
static void AkaoCmd_F3_ClearSavedMusic1(void);
void AkaoCmd_F4_SaveSoundState();
void AkaoCmd_F5_RestoreSoundState();
static void AkaoCmd_F8_StreamReverbMaskClear(void);
static void AkaoCmd_F9_StreamReverbMaskRestore(void);
static void AkaoCmd_FA_StopStream(void);
void AkaoCmd_Null(AkaoQueuedCommand* cmd);
void AkaoOp_A0_FinishChannel(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
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
void AkaoOp_AE_SetDr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_AF_SetSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
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
static void AkaoOp_F3_MuteMusic(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);

s32 g_AkaoFrameTimeHistory[4] = {0, 0, 0, 0};

AkaoCommandHandler g_AkaoCommandHandler[0x100] = {
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
    AkaoCmd_20_PlaySound,
    AkaoCmd_21_PlayTwoSounds,
    AkaoCmd_22_PlayThreeSounds,
    AkaoCmd_23_PlayFourSounds,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_20_PlaySound,
    AkaoCmd_29_PlaySoundSlot1,
    AkaoCmd_2A_PlaySoundSlot0,
    AkaoCmd_2B_PlaySoundSlot3,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_30_PlayMenuSound,
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
    AkaoCmd_80_SetStereoMode,
    AkaoCmd_81_SetMonoMode,
    AkaoCmd_82_ResetMusicAndSoundVol,
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
    AkaoCmd_90_SetMuteMusicMask,
    AkaoCmd_Null,
    AkaoCmd_92_SetCondition,
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
    AkaoCmd_A0_SetSoundVolBalanceSlot2,
    AkaoCmd_A1_SetSoundVolBalanceSlot1,
    AkaoCmd_A2_SetSoundVolBalanceSlot0,
    AkaoCmd_A3_SetSoundVolBalanceSlot3,
    AkaoCmd_A4_SlideSoundVolBalanceSlot2,
    AkaoCmd_A5_SlideSoundVolBalanceSlot1,
    AkaoCmd_A6_SlideSoundVolBalanceSlot0,
    AkaoCmd_A7_SlideSoundVolBalanceSlot3,
    AkaoCmd_A8_SetSoundPanSlot2,
    AkaoCmd_A9_SetSoundPanSlot1,
    AkaoCmd_AA_SetSoundPanSlot0,
    AkaoCmd_AB_SetSoundPanSlot3,
    AkaoCmd_AC_SlideSoundPanSlot2,
    AkaoCmd_AD_SlideSoundPanSlot1,
    AkaoCmd_AE_SlideSoundPanSlot0,
    AkaoCmd_AF_SlideSoundPanSlot3,
    AkaoCmd_B0_SetSoundPitchSlot2,
    AkaoCmd_B1_SetSoundPitchSlot1,
    AkaoCmd_B2_SetSoundPitchSlot0,
    AkaoCmd_B3_SetSoundPitchSlot3,
    AkaoCmd_B4_SlideSoundPitchSlot2,
    AkaoCmd_B5_SlideSoundPitchSlot1,
    AkaoCmd_B6_SlideSoundPitchSlot0,
    AkaoCmd_B7_SlideSoundPitchSlot3,
    AkaoCmd_B8_SetAllSoundVolBalance,
    AkaoCmd_B9_SlideAllSoundVolBalance,
    AkaoCmd_BA_SetAllSoundPan,
    AkaoCmd_BB_SlideAllSoundPan,
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
    AkaoCmd_C8_SetCdVol,
    AkaoCmd_C9_CdVolSlideFromCurr,
    AkaoCmd_CA_CdVolSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_D0_SetTempo,
    AkaoCmd_D1_TempoSlideFromCurr,
    AkaoCmd_D2_TempoSlideBetweenTargets,
    AkaoCmd_Null,
    AkaoCmd_D4_SetPitch,
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
    AkaoCmd_F0_StopMusic,
    AkaoCmd_F1_StopAllSounds,
    AkaoCmd_F2_ClearSavedMusic0,
    AkaoCmd_F3_ClearSavedMusic1,
    AkaoCmd_F4_SaveSoundState,
    AkaoCmd_F5_RestoreSoundState,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_F8_StreamReverbMaskClear,
    AkaoCmd_F9_StreamReverbMaskRestore,
    AkaoCmd_FA_StopStream,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
};

u8 g_AkaoOpcodeParamLength[0x60] = {
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
    AkaoOp_F3_MuteMusic,
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

void AkaoSpuTransferComplete(void) {
    SpuSetTransferCallback(NULL);
    g_AkaoTransfer = 0;
}

void AkaoSpuTransferPrep(void) {
    g_AkaoTransfer = 1;
    SpuSetTransferCallback(AkaoSpuTransferComplete);
}

static void AkaoSpuWrite(u8* addr, s32 size) {
    AkaoSpuTransferPrep();
    SpuWrite(addr, size);
}

static void AkaoSpuRead(u8* addr, s32 size) {
    AkaoSpuTransferPrep();
    SpuRead(addr, size);
}

void AkaoSpuTransferSync(void) {
    while (g_AkaoTransfer != 0) {
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoInitData);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadInstr);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadInstr2);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoStart);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoLoadEffect);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoDeinit);

// Key off the voices in g_AkaoStreamMask and clear the SPU transfer/IRQ callbacks.
static void AkaoStreamStop(void) {
    SpuSetTransferCallback(0);
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    SpuSetKey(0, g_AkaoStreamMask);
    if (g_AkaoStreamMask & 0x10000) {
        g_AkaoStreamVoice16UpdateMask = AKAO_UPDATE_SPU_ALL;
    }
    if (g_AkaoStreamMask & 0x20000) {
        g_AkaoStreamVoice17UpdateMask = AKAO_UPDATE_SPU_ALL;
    }
    g_AkaoStreamMask = 0;
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
    AkaoUpdateNoiseVoices();
}

static void SetReverbMode(s32 in_ReverbMode) {
    AkaoStreamStop();
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

// Word-copies (size >> 2) words from src into music staging buffer g_AkaoMusicBuffer.
static void AkaoCopyMusic(s32* src, u32 size) {
    s32* dst;
    u32 nwords;

    nwords = size >> 2;
    dst = g_AkaoMusicBuffer;
    while (nwords != 0) {
        nwords -= 1;
        *dst = *src;
        src += 1;
        dst += 1;
    }
}

void AkaoInstrInit(AkaoChannel*, s32);

// Resets and initializes sound effect channel parameters, pointing to seqData with default volume and instrument 5.
static void SoundChannelInit(AkaoChannel* channel, u8* seqData) {
    channel->akaoSequencePointer = seqData;
    channel->volumeMultiplier = 0x78;
    AkaoInstrInit(channel, 5);
    channel->octave = 2;
    channel->fineTuning = 0;
    channel->transpose = 0;
    channel->portamentoSteps = 0;
    channel->pitchSlide = 0;
    channel->keyAdd = 0;
    channel->lengthFixed = 0;
    channel->lengthStored = 0;
    channel->pitchSlideStepsCur = 0;
    channel->volumeLevel = 0x32000000;
    channel->volSlideSteps = 0;
    channel->updateFlags = 0;
    channel->loopId = 0;
    channel->sfxMask = 0;
    channel->panLfoVol = 0;
    channel->panLfoDepth = 0;
    channel->tremoloDepth = 0;
    channel->vibratoDepth = 0;
    channel->panLfoDepthSlideSteps = 0;
    channel->tremoloDepthSlideSteps = 0;
    channel->vibratoDepthSlideSteps = 0;
    channel->pitchLfoSwitchDelay = 0;
    channel->noiseSwitchDelay = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicChannelsInit);

// Merges newly-requested bits (g_AkaoMusicOverMask/g_AkaoMusicAltMask) into the
// pending mask g_AkaoMusicActiveMask, then for each set bit points the matching
// g_Channel1 channel at the stop sequence g_AkaoDummyStopSequence and sets length1/2 to
// 0x204, clearing the request bits as it goes.
static void AkaoMusicStopChannels1(void) {
    s32 mask;
    s32 bit;
    AkaoChannel* channel;
    s32 overMask;
    s32 altMask;

    if (g_AkaoMusicActiveMask != 0) {
        channel = g_Channel1;
        bit = 1;
        overMask = g_AkaoMusicOverMask;
        altMask = g_AkaoMusicAltMask;
        g_AkaoMusicAltMask = 0;
        g_AkaoMusicOverMask = 0;
        g_AkaoMusicKeyedMask = 0;
        g_AkaoMusicOnMask = 0;
        overMask |= altMask;
        mask = g_AkaoMusicActiveMask;
        mask |= overMask;
        g_AkaoMusicActiveMask = mask;
        g_AkaoMusicOffMask |= mask;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u16*)&channel->length1 = 0x204;
                channel->akaoSequencePointer = g_AkaoDummyStopSequence;
            }
            bit *= 2;
            channel += 1;
        } while (mask != 0);
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicStopChannels12);

void AkaoSoundChannelsInit(u16 volPan, s32 channelId, s32 seq1, s32 seq2);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundMenuChannelsInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsStop);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelsClear);

// Resolves a 10-bit sound effect ID into a pair of sequence pointers: looks up
// g_AkaoEffectsAll[index] and g_AkaoEffectsAll[index+1] (u16 offsets), adding
// the sequence base g_AkaoEffectsAllSeq unless the entry is the 0xFFFF sentinel
// (in which case the sequence pointer is 0).
static void AkaoSoundGetSequence(s32* outSeq0, s32* outSeq1, u16 soundId) {
    u16 idx;
    s32 seq0;
    s32 seq1;
    u16 offset0;
    u16 offset1;

    idx = (soundId & 0x3FF) * 2;
    offset0 = *(u16*)((idx * 2) + g_AkaoEffectsAll);
    if (offset0 != 0xFFFF) {
        seq0 = offset0 + g_AkaoEffectsAllSeq;
    } else {
        seq0 = 0;
    }
    *outSeq0 = seq0;
    idx = idx + 1;
    offset1 = *(u16*)((idx * 2) + g_AkaoEffectsAll);
    if (offset1 != 0xFFFF) {
        seq1 = offset1 + g_AkaoEffectsAllSeq;
    } else {
        seq1 = 0;
    }
    *outSeq1 = seq1;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicVolReset);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundVolReset);

// Synchronizes g_AkaoMusicOnMask and g_AkaoMusicKeyedMask with the hardware
// SPU key status (SpuGetKeyStatus) for all active music channels not stolen
// by sound effects or stream audio, right before channel state backup/switching.
void AkaoMusicSyncKeyStatus(void);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicSyncKeyStatus);

// Synchronizes g_Channel3OnMask and g_Channel3KeyedMask with the hardware
// SPU key status (SpuGetKeyStatus) for all active sound effect channels.
void AkaoSoundSyncKeyStatus(void);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundSyncKeyStatus);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicRestoreChannelsAndConfig);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicCopyChannels1Into2);

// Copies 24 channels (0x18C0 bytes) and channel configuration (0x60 bytes)
// from source to destination buffers.
void AkaoMusicCopyChannelsAndConfig(
    AkaoChannel* srcChannels, AkaoChannel* dstChannels, AkaoChannelConfig* srcConfig, AkaoChannelConfig* dstConfig);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicCopyChannelsAndConfig);

/////////////////////////
// AKAO COMMANDS
/////////////////////////

// Copies the sequence to the staging buffer, restores channels and config from backup
// if musicId matches backup slot 0 or 1, otherwise initializes fresh music channels.
void AkaoCmd_10_PlayMusic(AkaoQueuedCommand* cmd) {
    AkaoCopyMusic((s32*)cmd->param0, cmd->param1);
    if (g_AkaoMusicId == BGM_TA) { // Final Fantasy VII Main Theme (World Map)
        AkaoMusicSyncKeyStatus();
        AkaoMusicCopyChannelsAndConfig(g_Channel1, g_AkaoSavedChannels1, (AkaoChannelConfig*)&g_Channel1Config,
                                       (AkaoChannelConfig*)&g_AkaoSavedChannelConfig1);
    }
    AkaoMusicStopChannels1();
    if (g_AkaoSavedMusicId0 && g_AkaoSavedMusicId0 == (u16)cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(0);
    } else if (g_AkaoSavedMusicId1 && g_AkaoSavedMusicId1 == (u16)cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(1);
    } else {
        AkaoMusicChannelsInit();
    }
    g_AkaoMusicId = cmd->param2;
}

// Copies the music sequence to the staging buffer, backs up the currently playing song,
// (to backup slot 1 if BGM_TA [World map Main Theme] or slot 0 for any other song)
// stops the channels, initializes new channels from the beginning, and sets g_AkaoMusicId.
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoQueuedCommand* cmd) {
    AkaoChannelConfig* channelConfig;

    AkaoCopyMusic((s32*)cmd->param0, cmd->param1);
    AkaoMusicSyncKeyStatus();
    channelConfig = (AkaoChannelConfig*)&g_Channel1Config;
    if (g_AkaoMusicId) {
        if (g_AkaoMusicId == BGM_TA) { // Final Fantasy VII Main Theme (World Map)
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1, g_AkaoSavedChannels1, channelConfig, (AkaoChannelConfig*)&g_AkaoSavedChannelConfig1);
        } else {
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1, g_AkaoSavedChannels0, channelConfig, (AkaoChannelConfig*)&g_AkaoSavedChannelConfig0);
        }
    }
    AkaoMusicStopChannels1();
    AkaoMusicChannelsInit();
    g_AkaoMusicId = cmd->param2;
}

// Copies the sequence to staging buffer, clears flag 0x100, and switches music with
// backup state swapping: if the requested music ID matches backup slot 0 or 1, active
// music (channel 1) is moved to channel 2 (transition) and saved back into the backup slot,
// while the target music is restored into active channel 1. If not saved in a slot, the
// current music is backed up and new channels are initialized fresh.
void AkaoCmd_15_PlayMusicSwapSaved(AkaoQueuedCommand* cmd);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_15_PlayMusicSwapSaved);

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_10_PlayMusic (resuming from backup if previously saved).
void AkaoCmd_18_FadePlayMusic(AkaoQueuedCommand* cmd) {
    if (g_AkaoMusicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_10_PlayMusic(cmd);
}

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_14_PlayMusicSaveCurrent (saving current music to backup).
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoQueuedCommand* cmd) {
    if (g_AkaoMusicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_14_PlayMusicSaveCurrent(cmd);
}

// Clears sound channel 4 (1 voice) and initializes it with center pan (0x40)
// using the provided raw sound sequence pointers directly (bypassing table lookup).
void AkaoCmd_34_PlaySoundDirect(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelsClear(4, 1);
    AkaoSoundChannelsInit(AKAO_PAN_CENTER, AKAO_SFX_SLOT_2, cmd->param0, cmd->param1);
}

// Clears sound channels for 2 voices starting at voice 4 (SFX slots 1 and 2),
// then resolves and initializes two sound effect sequences with the requested pan.
void AkaoCmd_21_PlayTwoSounds(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(4, 2);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param2);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_2, seq0, seq1);
}

void AkaoCmd_22_PlayThreeSounds(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(4, 3);
    AkaoStreamStop();
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_0, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param2);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param3);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_2, seq0, seq1);
}

// Clears sound channels for 4 voices starting at voice 6 (SFX slots 0 through 3),
// stops streaming audio, then resolves and initializes four sound effect sequences
// with the requested pan.
void AkaoCmd_23_PlayFourSounds(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(6, 4);
    AkaoStreamStop();
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_0, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param2);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param3);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_2, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param4);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_3, seq0, seq1);
}

void AkaoCmd_30_PlayMenuSound(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param0);
    AkaoSoundMenuChannelsInit(seq0, seq1);
}

void AkaoCmd_20_PlaySound(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(4, 1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_2, seq0, seq1);
}

void AkaoCmd_29_PlaySoundSlot1(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(2, 1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
}

void AkaoCmd_2A_PlaySoundSlot0(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(0, 1);
    AkaoStreamStop();
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_0, seq0, seq1);
}

void AkaoCmd_2B_PlaySoundSlot3(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_3, seq0, seq1);
}

void AkaoCmd_C0_VolumeSet(AkaoQueuedCommand* cmd) {
    g_AkaoVolMulMusicSlideSteps = 0;
    g_AkaoVolMulMusic = (cmd->param0 & AKAO_VOL_MAX) << 0x10;
    AkaoMusicVolReset();
}

// Starts a volume slide from the current g_AkaoVolMulMusic toward a target
// derived from cmd, over cmd's tick count.
void AkaoCmd_C1_VolSlideFromCurr(AkaoVolSlideFromCurr* cmd) {
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    g_AkaoVolMulMusicSlideSteps = effectiveSteps;
    g_AkaoVolMulMusicSlideStep = (((cmd->targetVol & AKAO_VOL_MAX) << 0x10) - g_AkaoVolMulMusic) / effectiveSteps;
    AkaoMusicVolReset();
}

// Starts a volume slide between two explicit targets from cmd (rather than
// from the current g_AkaoVolMulMusic), over cmd's tick count.
void AkaoCmd_C2_VolSlideBetweenTargets(AkaoVolSlideBetweenTargets* cmd) {
    s32 startVol;
    s32 effectiveSteps;
    s32 targetVol;

    targetVol = cmd->steps;
    effectiveSteps = 1;
    if (targetVol != 0) {
        effectiveSteps = targetVol;
    }
    targetVol = (cmd->targetVol & AKAO_VOL_MAX) << 0x10;
    startVol = (cmd->startVol & AKAO_VOL_MAX) << 0x10;
    g_AkaoVolMulMusicSlideSteps = effectiveSteps;
    g_AkaoVolMulMusic = startVol;
    g_AkaoVolMulMusicSlideStep = (targetVol - startVol) / effectiveSteps;
    AkaoMusicVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_C8_SetCdVol);

// Starts a CD-audio volume slide from the current g_AkaoCdVol toward a
// target derived from cmd, over cmd's tick count.
void AkaoCmd_C9_CdVolSlideFromCurr(AkaoCdVolSlideFromCurr* cmd) {
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    g_AkaoCdVolSlideSteps = effectiveSteps;
    g_AkaoCdVolSlideStep = ((cmd->targetVol << 0x10) - g_AkaoCdVol.val) / effectiveSteps;
}

// Starts a CD-audio volume slide between two explicit targets from cmd
// (rather than from the current g_AkaoCdVol), over cmd's tick count.
void AkaoCmd_CA_CdVolSlideBetweenTargets(AkaoCdVolSlideBetweenTargets* cmd) {
    s32 steps;
    s32 startVol;
    s32 effectiveSteps;
    s32 targetVolShifted;
    s32 startVolShifted;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    targetVolShifted = cmd->targetVol << 0x10;
    startVolShifted = cmd->startVol << 0x10;
    g_AkaoCdVolSlideSteps = effectiveSteps;
    g_AkaoCdVol.val = startVolShifted;
    g_AkaoCdVolSlideStep = (targetVolShifted - startVolShifted) / effectiveSteps;
}

// Sets the volume balance for a 2-voice sound effect channel pair (voice[0]
// and voice[1]). Clears any active balance slide and flags the hardware voices
// (SPU_VOICE_VOLL | SPU_VOICE_VOLR) for volume recalculation.
static void AkaoSoundChannelSetVolBalance(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    u16 balance;
    s32 mask1;
    s32 mask0;
    AkaoChannel* voice = slot->voices;
    // The do{}while(0) affects register allocation and is required for the
    // match.
    do {
        balance = *(u16*)&cmd->param0;
        mask1 = voice[1].voiceAttr.mask;
        voice[1].volBalanceSlideSteps = 0;
        voice[0].volBalanceSlideSteps = 0;
        voice[1].volBalance = (s16)((balance & AKAO_VOL_MAX) << 8);
    } while (0);
    voice[0].volBalance = (s16)((balance & AKAO_VOL_MAX) << 8);
    mask0 = voice[0].voiceAttr.mask;
    voice[1].voiceAttr.mask = mask1 | AKAO_UPDATE_SPU_VOICE;
    voice[0].voiceAttr.mask = mask0 | AKAO_UPDATE_SPU_VOICE;
}

// Starts a volume balance slide from current balance toward target in cmd over
// the specified step count for a 2-voice sound effect channel pair.
static void AkaoSoundChannelSlideVolBalance(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s16 steps;
    s32 rawSteps;
    AkaoChannel* voice = slot->voices;

    rawSteps = cmd->param0;
    steps = 1;
    if (rawSteps != 0) {
        steps = *(u16*)&cmd->param0;
    }
    voice[0].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_VOL_MAX) << 8) - voice[0].volBalance) / steps;
    voice[1].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_VOL_MAX) << 8) - voice[1].volBalance) / steps;
    voice[1].volBalanceSlideSteps = steps;
    voice[0].volBalanceSlideSteps = steps;
}

// Sets the volume balance across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_B8_SetAllSoundVolBalance(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the volume balance across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target balance in cmd.
void AkaoCmd_B9_SlideAllSoundVolBalance(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A0_SetSoundVolBalanceSlot2(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[2]);
}

void AkaoCmd_A4_SlideSoundVolBalanceSlot2(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[2]);
}

void AkaoCmd_A1_SetSoundVolBalanceSlot1(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[1]);
}

void AkaoCmd_A5_SlideSoundVolBalanceSlot1(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[1]);
}

void AkaoCmd_A2_SetSoundVolBalanceSlot0(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A6_SlideSoundVolBalanceSlot0(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A3_SetSoundVolBalanceSlot3(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[3]);
}

void AkaoCmd_A7_SlideSoundVolBalanceSlot3(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[3]);
}

// Sets the stereo pan for a 2-voice sound effect channel pair (voice[0]
// and voice[1]). Clears any active pan slide and flags the hardware voices
// (SPU_VOICE_VOLL | SPU_VOICE_VOLR) for volume recalculation.
static void AkaoSoundChannelSetPan(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s16 pan;
    s32 mask1;
    AkaoChannel* voice = slot->voices;

    pan = (*(u16*)&cmd->param0 & AKAO_PAN_MAX) << 8;
    mask1 = voice[1].voiceAttr.mask;
    voice[1].volPanSlideSteps = 0;
    voice[0].volPanSlideSteps = 0;
    voice[1].volPan = pan;
    voice[0].volPan = pan;
    voice[0].voiceAttr.mask = voice[0].voiceAttr.mask | AKAO_UPDATE_SPU_VOICE;
    voice[1].voiceAttr.mask = (mask1 | AKAO_UPDATE_SPU_VOICE);
}

// Starts a pan slide from current pan toward target in cmd over
// the specified step count for a 2-voice sound effect channel pair.
static void AkaoSoundChannelSlidePan(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s16 steps;
    s32 rawSteps;
    AkaoChannel* voice = slot->voices;

    rawSteps = cmd->param0;
    steps = 1;
    if (rawSteps != 0) {
        steps = *(u16*)&cmd->param0;
    }
    voice[0].volPanSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_PAN_MAX) << 8) - voice[0].volPan) / steps;
    voice[1].volPanSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_PAN_MAX) << 8) - voice[1].volPan) / steps;
    voice[1].volPanSlideSteps = steps;
    voice[0].volPanSlideSteps = steps;
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BA_SetAllSoundPan(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BB_SlideAllSoundPan(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A8_SetSoundPanSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_AC_SlideSoundPanSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A9_SetSoundPanSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AD_SlideSoundPanSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AA_SetSoundPanSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AE_SlideSoundPanSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AB_SetSoundPanSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_AF_SlideSoundPanSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[3]); }

// Sets the sound effect pitch multiplier for a 2-voice sound effect channel pair
// (voice[0] and voice[1]). Clears any active pitch slide and flags the hardware voices
// (SPU_VOICE_PITCH) for pitch recalculation.
static void AkaoSoundChannelSetPitch(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s32 pitch;
    s32 mask1;
    s8* cmdBytes = (s8*)cmd;
    AkaoChannel* voice = slot->voices;

    pitch = cmdBytes[4] << 8;
    mask1 = voice[1].voiceAttr.mask;
    voice[1].pitchMulSoundSlideSteps = 0;
    voice[0].pitchMulSoundSlideSteps = 0;
    voice[1].pitchMulSound = pitch;
    voice[0].pitchMulSound = pitch;
    voice[0].voiceAttr.mask = voice[0].voiceAttr.mask | SPU_VOICE_PITCH;
    voice[1].voiceAttr.mask = mask1 | SPU_VOICE_PITCH;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelSlidePitch);

// Sets the pitch multiplier across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_BC_SetAllSoundPitch(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the pitch multiplier across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target pitch multiplier in cmd.
void AkaoCmd_BD_SlideAllSoundPitch(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_B0_SetSoundPitchSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B4_SlideSoundPitchSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B1_SetSoundPitchSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B5_SlideSoundPitchSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B2_SetSoundPitchSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B6_SlideSoundPitchSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B3_SetSoundPitchSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_B7_SlideSoundPitchSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_D0_SetTempo(AkaoTempoPitchSlide* cmd) {
    s32 tempo = cmd->start;
    g_AkaoTempoMulMusicSlideSteps = 0;
    g_AkaoTempoMulMusic = tempo << 0x10;
}

// Starts a tempo slide toward a target derived from cmd, over cmd's tick
// count.
void AkaoCmd_D1_TempoSlideFromCurr(AkaoSlideFromCurr* cmd) {
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    g_AkaoTempoMulMusicSlideStep = ((cmd->target << 0x10) - g_AkaoTempoMulMusic) / effectiveSteps;
    g_AkaoTempoMulMusicSlideSteps = effectiveSteps;
}

// Starts a tempo slide between two explicit targets from cmd, over cmd's
// tick count.
void AkaoCmd_D2_TempoSlideBetweenTargets(AkaoTempoPitchSlide* cmd) {
    long delta;
    s32 startVal;
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    startVal = cmd->start << 0x10;
    g_AkaoTempoMulMusic = startVal;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    delta = (cmd->target << 0x10) - startVal;
    g_AkaoTempoMulMusicSlideSteps = effectiveSteps;
    g_AkaoTempoMulMusicSlideStep = delta / effectiveSteps;
}

void AkaoCmd_D4_SetPitch(AkaoTempoPitchSlide* cmd) {
    s32 pitch = cmd->start;
    g_AkaoPitchMulMusicSlideSteps = 0;
    g_AkaoPitchMulMusic = pitch << 0x10;
}

// Starts a pitch slide from the current g_AkaoPitchMulMusic toward a
// target derived from cmd, over cmd's tick count.
void AkaoCmd_D5_PitchSlideFromCurr(AkaoSlideFromCurr* cmd) {
    s32 steps;
    s32 effectiveSteps;
    s32 step;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    step = ((cmd->target << 0x10) - g_AkaoPitchMulMusic) / effectiveSteps;
    g_AkaoPitchMulMusicSlideSteps = effectiveSteps;
    g_AkaoPitchMulMusicSlideStep = step;
}

// Starts a pitch slide between two explicit targets from cmd, over cmd's
// tick count.
void AkaoCmd_D6_PitchSlideBetweenTargets(AkaoTempoPitchSlide* cmd) {
    s32 delta;
    s32 startVal;
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    startVal = cmd->start << 0x10;
    g_AkaoPitchMulMusic = startVal;
    effectiveSteps = 1;
    if (steps != 0) {
        effectiveSteps = steps;
    }
    delta = (cmd->target << 0x10) - startVal;
    g_AkaoPitchMulMusicSlideSteps = effectiveSteps;
    g_AkaoPitchMulMusicSlideStep = delta / effectiveSteps;
}

static void AkaoCmd_F0_StopMusic(void) { AkaoMusicStopChannels12(); }

static void AkaoCmd_F1_StopAllSounds(void) { AkaoSoundChannelsStop(); }

static void AkaoCmd_80_SetStereoMode(void) {
    g_Channel1Config = AKAO_STEREO;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_82_ResetMusicAndSoundVol);

static void AkaoCmd_81_SetMonoMode(void) {
    g_Channel1Config = AKAO_MONO;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_90_SetMuteMusicMask);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_92_SetCondition);

// Moves newly-requested channels_1 voices into the active mask, resetting
// each one's SPU attributes.
void AkaoCmd_9B_ApplyPendingMusicUpdates(void) {
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
            g_AkaoVoiceAttrVolR = 0;
            g_AkaoVoiceAttrVolL = 0;
            g_AkaoVoiceAttrSr = AKAO_VOL_MAX;
            for (; pendingBits != 0; bit *= 2, voiceIdx += 1) {
                if (pendingBits & bit) {
                    g_AkaoVoiceAttrMask = AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, &g_AkaoVoiceAttr);
                    pendingBits ^= bit;
                }
            }
        }
        savedMask = g_AkaoMusicActiveMask;
        g_AkaoMusicActiveMask = 0;
        g_AkaoMusicActiveMaskStored = savedMask;
    }
    g_AkaoControlFlags |= AKAO_CONTROL_PAUSE_MUSIC_UPDATE;
}

// Restore counterpart: moves the stored channels_1 mask back to active,
// resetting SPU attributes along the way.
void AkaoCmd_9A_FlushPendingMusicUpdates(void) {
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
                voice->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
            bit *= 2;
            voice++;
        } while (stillPending = pendingBits != 0);
        savedMask = g_AkaoMusicActiveMaskStored;
        g_AkaoMusicActiveMaskStored = 0;
        g_AkaoMusicActiveMask = savedMask;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~AKAO_CONTROL_PAUSE_MUSIC_UPDATE;
}

// channels_3 counterpart to AkaoCmd_9B_ApplyPendingMusicUpdates; also masks off
// the top two voices in mono mode.
void AkaoCmd_9D_ApplyPendingSoundUpdates(void) {
    s32 savedMask;
    short cleared;
    s32 newMask;
    s32 bit;
    s32 voiceIdx;

    newMask = g_Channel3ActiveMask[0];
    savedMask = newMask;
    if (newMask != 0) {
        bit = 0x10000;
        if (g_AkaoSoundChannelsMode == AKAO_MONO) {
            newMask &= ~((1 << 22) | (1 << 23));
        }
        g_AkaoSoundActiveMaskStored = newMask;
        g_Channel3ActiveMask[cleared = 0] = newMask ^ savedMask;
        g_AkaoVoiceAttrVolR = cleared;
        g_AkaoVoiceAttrVolL = cleared;
        g_AkaoVoiceAttrSr = AKAO_VOL_MAX;
        voiceIdx = 0x10;
        if (newMask != cleared) {
            for (; newMask != 0; bit *= 2, voiceIdx += 1) {
                if (newMask & bit) {
                    g_AkaoVoiceAttrMask = AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, &g_AkaoVoiceAttr);
                    newMask ^= bit;
                }
            }
        }
    }
    g_AkaoControlFlags |= AKAO_CONTROL_PAUSE_SOUND_UPDATE;
}

// channels_3 counterpart to AkaoCmd_9A_FlushPendingMusicUpdates.
void AkaoCmd_9C_FlushPendingSoundUpdates(void) {
    AkaoChannel* half;
    s32 savedMask;
    s32 bit;
    s32 pendingBits;

    pendingBits = g_AkaoSoundActiveMaskStored;
    if (pendingBits != 0) {
        for (bit = 0x10000, half = &g_AkaoSoundSlots[0].voices[0]; pendingBits != 0; bit *= 2, half++) {
            if (pendingBits & bit) {
                pendingBits ^= bit;
                half->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
        }
        savedMask = g_AkaoSoundActiveMaskStored;
        g_AkaoSoundActiveMaskStored = 0;
        g_Channel3ActiveMask[0] = savedMask;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~AKAO_CONTROL_PAUSE_SOUND_UPDATE;
}

static void AkaoCmd_E0_SetReverbPan(AkaoSetReverbPan* cmd) {
    g_AkaoReverbPan = cmd->pan & AKAO_PAN_MAX;
    g_AkaoGlobalUpdateFlags |= AKAO_UPDATE_REVERB;
}

static void AkaoCmd_E4_SetReverbMul(AkaoSetReverbMul* cmd) {
    u8 mul;
    s32 flags;
    s32 mask;

    mul = cmd->mul;
    g_AkaoReverbMul = (s16)mul;
    mask = ~AKAO_CONTROL_REVERB_ENABLE;
    if (mul != 0) {
        flags = g_AkaoControlFlags | AKAO_CONTROL_REVERB_ENABLE;
    } else {
        flags = g_AkaoControlFlags & mask;
    }
    g_AkaoControlFlags = flags;
    AkaoUpdateReverbVoices();
    g_AkaoGlobalUpdateFlags |= AKAO_UPDATE_REVERB;
}

static void AkaoCmd_F2_ClearSavedMusic0(void) { g_AkaoSavedMusicId0 = 0; }

static void AkaoCmd_F3_ClearSavedMusic1(void) { g_AkaoSavedMusicId1 = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F4_SaveSoundState);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F5_RestoreSoundState);

static void AkaoCmd_F8_StreamReverbMaskClear(void) {
    s32* addr;
    s32 reverbMask;
    s32 invStreamMask;

    AkaoStreamInit();
    addr = g_Channel3ActiveMask;
    reverbMask = g_AkaoReverbMask;
    invStreamMask = ~g_AkaoStreamMask;
    *addr &= invStreamMask;
    g_AkaoReverbMask = invStreamMask & reverbMask;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_F9_StreamReverbMaskRestore(void) {
    s32 activeMask;

    AkaoStreamInit();
    activeMask = g_Channel3ActiveMask[0];
    g_Channel3ActiveMask[0] = ~g_AkaoStreamMask & activeMask;
    g_AkaoReverbMask |= g_AkaoStreamMask;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_FA_StopStream(void) { AkaoStreamStop(); }

void AkaoCmd_Null(AkaoQueuedCommand* cmd) {}

static void AkaoClearTransferCallback(void) { SpuSetTransferCallback(0); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoStreamInit);

// Configures the voice-attribute block for a mono CD-stream voice (ADSR
// envelope, pan, reverb-echo work area) and applies it via AkaoUpdateChannelParamsToSpu.
//
// NOTE: Although these fields form the exact memory layout of an AkaoVoiceAttr
// struct starting at &g_AkaoVoiceAttr (0x8007EBE4), accessing them as struct members
// (e.g. g_AkaoVoiceAttr.mask = ...) causes GCC 2.7.2 to hoist the base address into
// a register and use offset-based stores. The original binary was written using
// separate global variables, generating individual lui/sw instructions for each field.
// They must remain accessed as individual globals here to preserve 100% byte-exact matching.
static void AkaoStreamVoiceAttrMono(void) {
    g_AkaoVoiceAttrMask = 0x1FF93;
    g_AkaoVoiceAttrAr = 0;
    g_AkaoVoiceAttrAddr = 0x77000;
    g_AkaoVoiceAttrLoopAddr = 0x77000;
    g_AkaoVoiceAttrDr = 0xF;
    g_AkaoVoiceAttrSl = 0xF;
    g_AkaoVoiceAttrSr = 0x7F;
    g_AkaoVoiceAttrRr = 6;
    g_AkaoVoiceAttrAMode = 1;
    g_AkaoVoiceAttrSMode = 3;
    g_AkaoVoiceAttrRMode = 3;
    g_AkaoVoiceAttrVolL = (g_AkaoStreamPan ^ AKAO_PAN_MAX) * g_AkaoStreamVol >> 7;
    g_AkaoVoiceAttrPitch = g_AkaoStreamPitch;
    g_AkaoVoiceAttrVolR = g_AkaoStreamVol * g_AkaoStreamPan >> 7;
    AkaoUpdateChannelParamsToSpu(0x10, &g_AkaoVoiceAttr);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoStreamVoiceAttrSplit);

static void AkaoStreamIrqCallbackMono0(void);

// CD-stream DMA transfer-complete callback (mono case). Keys on the stream
// voice(s) in g_AkaoStreamMask; when g_AkaoStreamRemainingBytes (bytes remaining) is nonzero, first
// re-arms the SPU transfer IRQ with AkaoStreamIrqCallbackMono0 to continue streaming.
static void AkaoStreamTransferCallbackMono(void) {
    SpuSetTransferCallback(0);
    if (g_AkaoStreamRemainingBytes != 0) {
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
    if (g_AkaoStreamRemainingBytes != 0) {
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
    if (g_AkaoStreamRemainingBytes == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x77000);
    SpuWrite(g_AkaoStreamSrc, 0x1000);
    SpuSetIRQ(0);
    if (g_AkaoStreamRemainingBytes > 0x1000) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono1);
        SpuSetIRQ(1);
        g_AkaoStreamRemainingBytes -= 0x1000;
        g_AkaoStreamSrc += 0x1000;
        return;
    }
    if (g_AkaoStreamLoopSrc != 0) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono1);
        SpuSetIRQ(1);
        g_AkaoStreamSrc = g_AkaoStreamLoopSrc;
        g_AkaoStreamRemainingBytes = g_AkaoStreamLoopSize;
        return;
    }
    g_AkaoStreamRemainingBytes = 0;
    SpuSetIRQAddr(0x77000);
    SpuSetIRQCallback(AkaoStreamStop);
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackMono1(void) {
    if (g_AkaoStreamRemainingBytes == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x78000);
    SpuWrite(g_AkaoStreamSrc, 0x1000);
    SpuSetIRQ(0);
    if (g_AkaoStreamRemainingBytes > 0x1000) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
        g_AkaoStreamRemainingBytes -= 0x1000;
        g_AkaoStreamSrc += 0x1000;
        return;
    }
    if (g_AkaoStreamLoopSrc != 0) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
        g_AkaoStreamSrc = g_AkaoStreamLoopSrc;
        g_AkaoStreamRemainingBytes = g_AkaoStreamLoopSize;
        return;
    }
    g_AkaoStreamRemainingBytes = 0;
    SpuSetIRQAddr(0x78000);
    SpuSetIRQCallback(AkaoStreamStop);
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackSplit1(void);

static void AkaoStreamIrqCallbackSplit0(void) {
    if (g_AkaoStreamRemainingBytes == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x77000);
    SpuWrite(g_AkaoStreamSrc, 0x1000);
    SpuSetIRQ(0);
    SpuSetVoiceLoopStartAddr(0x10, 0x77000);
    SpuSetVoiceLoopStartAddr(0x11, 0x77800);
    if (g_AkaoStreamRemainingBytes > 0x1000) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit1);
        g_AkaoStreamRemainingBytes -= 0x1000;
        g_AkaoStreamSrc += 0x1000;
    } else if (g_AkaoStreamLoopSrc != 0) {
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit1);
        g_AkaoStreamSrc = g_AkaoStreamLoopSrc;
        g_AkaoStreamRemainingBytes = g_AkaoStreamLoopSize;
    } else {
        g_AkaoStreamRemainingBytes = 0;
        SpuSetIRQAddr(0x77000);
        SpuSetIRQCallback(AkaoStreamStop);
    }
    SpuSetIRQ(1);
}

static void AkaoStreamIrqCallbackSplit1(void) {
    if (g_AkaoStreamRemainingBytes == 0) {
        return;
    }
    SpuSetTransferStartAddr(0x78000);
    SpuWrite(g_AkaoStreamSrc, 0x1000);
    SpuSetIRQ(0);
    SpuSetVoiceLoopStartAddr(0x10, 0x78000);
    SpuSetVoiceLoopStartAddr(0x11, 0x78800);
    if (g_AkaoStreamRemainingBytes > 0x1000) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        g_AkaoStreamRemainingBytes -= 0x1000;
        g_AkaoStreamSrc += 0x1000;
    } else if (g_AkaoStreamLoopSrc != 0) {
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        g_AkaoStreamSrc = g_AkaoStreamLoopSrc;
        g_AkaoStreamRemainingBytes = g_AkaoStreamLoopSize;
    } else {
        g_AkaoStreamRemainingBytes = 0;
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamStop);
    }
    SpuSetIRQ(1);
}

static void AkaoGetCommandQueue(AkaoQueuedCommand** out_cmd) {
    *out_cmd = g_AkaoCommandQueue;
    *out_cmd = &g_AkaoCommandQueue[g_AkaoCommandQueueId];
    g_AkaoCommandQueueId++;
}

s32 AkaoExec(void) {
    AkaoQueuedCommand* command;
    u8* data;
    u16 musicId;
    u16 dataSize;
    u16 reverbMode;
    s32 result;

    result = 0;
    g_AkaoMutex = 1;

    switch (g_AkaoCmd.opcode) {
    case AKAO_PLAY_MUSIC:
    case AKAO_PLAY_MUSIC_SAVE_CURR:
    case AKAO_PLAY_MUSIC_SWAP_SAVED:
    case AKAO_FADE_PLAY_MUSIC:
    case AKAO_FADE_PLAY_MUSIC_SAVE_CURR:
        data = (u8*)g_AkaoCmd.params[0];
        if (data[0] == 'A' && data[1] == 'K' && data[2] == 'A' && data[3] == 'O') {
            data += 4;
            musicId = *(u16*)data;
            data += 2;
            dataSize = *(u16*)data;
            data += 2;
            reverbMode = *(u16*)data;
            data += 8;
            if (g_AkaoMusicId != musicId) {
                SetReverbMode(reverbMode);
                AkaoGetCommandQueue(&command);
                command->param0 = (s32)data;
                command->param1 = dataSize;
                command->param2 = musicId;
                command->param3 = g_AkaoCmd.params[1];
                command->opcode = g_AkaoCmd.opcode;
            } else {
                result = 1;
            }
        } else {
            result = -1;
        }
        break;
    case AKAO_PLAY_ONE_CONSECUTIVE_SOUND:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->opcode = AKAO_PLAY_SOUND;
        break;
    case AKAO_PLAY_TWO_CONSECUTIVE_SOUNDS:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[1] + 1;
        command->opcode = AKAO_PLAY_TWO_SOUNDS;
        break;
    case AKAO_PLAY_THREE_CONSECUTIVE_SOUNDS:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[1] + 1;
        command->param3 = g_AkaoCmd.params[1] + 2;
        command->opcode = AKAO_PLAY_THREE_SOUNDS;
        break;
    case AKAO_PLAY_FOUR_CONSECUTIVE_SOUNDS:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[1] + 1;
        command->param3 = g_AkaoCmd.params[1] + 2;
        command->param4 = g_AkaoCmd.params[1] + 3;
        command->opcode = AKAO_PLAY_FOUR_SOUNDS;
        break;
    case AKAO_SET_TEMPO_AND_PITCH:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->opcode = AKAO_SET_TEMPO;
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->opcode = AKAO_SET_PITCH;
        break;
    case AKAO_TEMPO_AND_PITCH_SLIDE_FROM_CURR:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->opcode = AKAO_TEMPO_SLIDE_FROM_CURR;
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->opcode = AKAO_PITCH_SLIDE_FROM_CURR;
        break;
    case AKAO_TEMPO_AND_PITCH_SLIDE_BETWEEN_TARGETS:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[2];
        command->opcode = AKAO_TEMPO_SLIDE_BETWEEN_TARGETS;
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[2];
        command->opcode = AKAO_PITCH_SLIDE_BETWEEN_TARGETS;
        break;
    case AKAO_APPLY_ALL_PENDING_UPDATES:
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_APPLY_PENDING_MUSIC_UPDATES;
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_APPLY_PENDING_SOUND_UPDATES;
        break;
    case AKAO_FLUSH_ALL_PENDING_UPDATES:
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_FLUSH_PENDING_MUSIC_UPDATES;
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_FLUSH_PENDING_SOUND_UPDATES;
        break;
    default:
        AkaoGetCommandQueue(&command);
        command->param0 = g_AkaoCmd.params[0];
        command->param1 = g_AkaoCmd.params[1];
        command->param2 = g_AkaoCmd.params[2];
        command->param3 = g_AkaoCmd.params[3];
        command->param4 = g_AkaoCmd.params[4];
        command->opcode = g_AkaoCmd.opcode;
        break;
    }

    g_AkaoMutex = 0;
    return result;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoDispatchCommand);

static void AkaoExecuteCommandsQueue(void) {
    AkaoQueuedCommand* cmd;

    if (g_AkaoMutex == 0) {
        for (cmd = g_AkaoCommandQueue; g_AkaoCommandQueueId; g_AkaoCommandQueueId--, cmd++) {
            ((void (*)(AkaoQueuedCommand*))g_AkaoCommandHandler[(u8)cmd->opcode])(cmd);
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

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateGlobalSlides);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMainUpdate);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoExecuteSequence);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoInstrInit);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoGetNextNote);

static u8 AkaoScanSequenceTerminator(u8** seqPtr) {
    u8 expected;
    u8 len;
    u8 opcode;
    u8* data;

    data = *seqPtr;
    expected = AKAO_OP_LOOP_RETURN;
    do {
        opcode = *data;
        len = g_AkaoOpcodeSize[opcode];
        data += len;
    } while (len);
    return opcode == expected ? AKAO_OP_LOOP_RETURN : AKAO_OP_FINISH_CHANNEL;
}

/////////////////////////
// AKAO OPCODES
/////////////////////////

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
    track->updateFlags |= AKAO_UPDATE_REVERB_DEPTH;
    track->pitchMulSoundSlideStep = combined;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_EB_ReverbDepthSlide);

static void AkaoOp_A3_MasterVol(AkaoChannel* track) {
    track->volumeMultiplier = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
}

static void AkaoOp_A8_SetVol(AkaoChannel* track) {
    s32 val = (s8)*track->akaoSequencePointer++;

    track->volSlideSteps = 0;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
    track->volumeLevel = val << 0x17;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A9_SetVolSlide);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F4_OverlayVoiceOn);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F5_OverlayVoiceOff);

static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->volBalanceSlideSteps = 0;
    track->volBalance = val << 8;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F7_OverlayVolBalanceSlide);

static void AkaoOp_AA_SetPan(AkaoChannel* track) {
    track->volPan = *track->akaoSequencePointer++ << 8;
    track->volPanSlideSteps = 0;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
}

static void AkaoOp_AB_SetPanSlide(AkaoChannel* track) {
    u8 targetPan;
    u16 currPan;

    track->volPanSlideSteps = *track->akaoSequencePointer++;
    if (track->volPanSlideSteps == 0) {
        track->volPanSlideSteps = 0x100;
    }
    targetPan = *track->akaoSequencePointer++;
    track->volPan &= 0xFF00;
    currPan = track->volPan;
    track->volPanSlideStep = ((targetPan << 8) - currPan) / (u16)track->volPanSlideSteps;
}

static void AkaoOp_A5_SetOctave(AkaoChannel* track) { track->octave = *track->akaoSequencePointer++; }

static void AkaoOp_A6_IncOctave(AkaoChannel* track) { track->octave = (track->octave + 1) & 0xF; }

static void AkaoOp_A7_DecOctave(AkaoChannel* track) { track->octave = (track->octave + 0xFFFF) & 0xF; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A1_LoadInstrument);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F2_LoadInstrument);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B3_ResetAdsr);

static void AkaoOp_C0_TransposeAbsolute(AkaoChannel* track) { track->transpose = (s8)*track->akaoSequencePointer++; }

static void AkaoOp_C1_TransposeRelative(AkaoChannel* track) {
    track->transpose = (s8)*track->akaoSequencePointer++ + track->transpose;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A4_PitchBendSlide);

static void AkaoOp_DA_PortamentoOn(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->portamentoSteps = (s16)val;
    if (val == 0) {
        track->portamentoSteps = 0x100;
    }
    track->transposeStored = 0;
    track->keyStored = 0;
    track->sfxMask = 1;
}

static void AkaoOp_DB_PortamentoOff(AkaoChannel* track) { track->portamentoSteps = 0; }

static void AkaoOp_D8_FineTuningAbsolute(AkaoChannel* track) { track->fineTuning = (s8)*track->akaoSequencePointer++; }

static void AkaoOp_D9_FineTuningRelative(AkaoChannel* track) {
    track->fineTuning = (s8)*track->akaoSequencePointer++ + track->fineTuning;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B4_Vibrato);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B5_VibratoDepth);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_DD_VibratoDepthSlide);

static void AkaoOp_B6_VibratoOff(AkaoChannel* track) {
    track->vibratoPitch = 0;
    track->updateFlags &= ~AKAO_UPDATE_VIBRATO;
    track->voiceAttr.mask |= SPU_VOICE_PITCH;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_B8_Tremolo);

static void AkaoOp_B9_TremoloDepth(AkaoChannel* track) { track->tremoloDepth = *track->akaoSequencePointer++ << 8; }

static void AkaoOp_DE_TremoloDepthSlideFromCurr(AkaoChannel* track) {
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
    delta = ((addr[1] << 8) - *(u16*)&track->tremoloDepth) / rate;
    track->tremoloDepthSlideSteps = rate;
    track->tremoloDepthSlideStep = delta;
}

static void AkaoOp_BA_TremoloOff(AkaoChannel* track) {
    track->tremoloVol = 0;
    track->updateFlags &= ~AKAO_UPDATE_TREMOLO;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
}

static void AkaoOp_BC_SetPanLfo(AkaoChannel* track) {
    u8* addr;
    u8* addr2;
    u8 rate;

    addr = track->akaoSequencePointer;
    track->updateFlags |= AKAO_UPDATE_PAN_LFO;
    track->akaoSequencePointer = addr + 1;
    rate = *addr;
    track->panLfoRate = rate;
    if (rate == 0) {
        track->panLfoRate = 0x100;
    }
    addr2 = track->akaoSequencePointer;
    track->akaoSequencePointer = addr2 + 1;
    track->panLfoType = *addr2;
    track->panLfoWave = g_AkaoWaveTableKey[*(u16*)&track->panLfoType];
    track->panLfoRateCur = 1;
}

static void AkaoOp_BD_PanLfoDepth(AkaoChannel* track) { track->panLfoDepth = *track->akaoSequencePointer++ << 7; }

static void AkaoOp_DF_PanLfoDepthSlideFromCurr(AkaoChannel* track) {
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
    delta = ((addr[1] << 7) - *(u16*)&track->panLfoDepth) / rate;
    track->panLfoDepthSlideSteps = rate;
    track->panLfoDepthSlideStep = delta;
}

static void AkaoOp_BE_PanLfoOff(AkaoChannel* track) {
    track->panLfoVol = 0;
    track->updateFlags &= ~AKAO_UPDATE_PAN_LFO;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
}

static void AkaoOp_C4_NoiseOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->noiseMask = mask | config->noiseMask;
    } else {
        g_AkaoNoiseMask |= mask;
    }
    g_AkaoGlobalUpdateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    AkaoUpdateNoiseVoices();
}

static void AkaoOp_C5_NoiseOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->noiseMask &= ~mask;
    } else {
        g_AkaoNoiseMask &= ~mask;
    }
    g_AkaoGlobalUpdateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    AkaoUpdateNoiseVoices();
    track->noiseSwitchDelay = 0;
}

static void AkaoOp_C6_PitchLfoOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->pitchLfoMask = mask | config->pitchLfoMask;
    } else if (!(mask & 0x555555)) {
        g_AkaoPitchLfoMask |= mask;
    }
    AkaoUpdatePitchLfoVoices();
}

static void AkaoOp_C7_PitchLfoOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->pitchLfoMask &= ~mask;
    } else {
        g_AkaoPitchLfoMask &= ~mask;
    }
    AkaoUpdatePitchLfoVoices();
    track->pitchLfoSwitchDelay = 0;
}

static void AkaoOp_C2_ReverbOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->reverbMask = mask | config->reverbMask;
    } else {
        g_AkaoReverbMask |= mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_C3_ReverbOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->reverbMask = ~mask & config->reverbMask;
    } else {
        g_AkaoReverbMask &= ~mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_CC_LegatoOn(AkaoChannel* track) { track->sfxMask = AKAO_SFX_LEGATO; }

static void AkaoOp_CD_LegatoOff(void) {}

static void AkaoOp_D0_FullLengthOn(AkaoChannel* track) { track->sfxMask = AKAO_SFX_FULL_LENGTH; }

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

void AkaoOp_C8_LoopPoint(AkaoChannel* track) {
    track->loopId = (track->loopId + 1) & 3;
    track->loopPoint[track->loopId] = track->akaoSequencePointer;
    track->loopTimes[track->loopId] = 0;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_C9_LoopReturnTimes);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F0_LoopJumpTimes);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F1_LoopBreakTimes);

void AkaoOp_CA_LoopReturn(AkaoChannel* track) {
    track->loopTimes[track->loopId]++;
    track->akaoSequencePointer = track->loopPoint[track->loopId];
}

static void AkaoOp_A2_NextNoteLength(AkaoChannel* track) {
    u16 val = *track->akaoSequencePointer++;

    track->lengthFixed = 0;
    /* sets length_1 and length_2 to the same byte in one halfword store;
       writing them as two separate field assignments regresses the gate. */
    *(s16*)&track->length1 = (val << 8) | val;
    track->lengthStored = val;
}

static void AkaoOp_DC_FixNoteLength(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    short delta = READ_S8(track->akaoSequencePointer);
    if (delta != 0) {
        delta += track->lengthStored;
        if (delta < 1) {
            delta = 1;
        } else if (delta > 255) {
            delta = 255;
        }
    }
    track->lengthFixed = delta;
}

static void AkaoOp_EC_DrumModeOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->drumOffset = track->akaoSequencePointer + READ_S16(track->akaoSequencePointer);
    track->updateFlags |= AKAO_UPDATE_DRUM_MODE;
}

static void AkaoOp_ED_DrumModeOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~AKAO_UPDATE_DRUM_MODE;
}

static void AkaoOp_FD_TimeSignature(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    config->timerLower = *track->akaoSequencePointer++;
    config->timerUpper = *track->akaoSequencePointer++;
    config->timerLowerCur = 0;
    config->timerUpperCur = 0;
}

static void AkaoOp_FE_MeasureNumber(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    config->timerTopCur = *track->akaoSequencePointer++;
    config->timerTopCur |= *track->akaoSequencePointer++ << 8;
}

static void AkaoOp_F3_MuteMusic(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) { config->muteMusic = 1; }

static void AkaoOp_B0_SetVoiceDrSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoOp_AE_SetDr(track, config, mask);
    AkaoOp_AF_SetSl(track, config, mask);
}

static void AkaoOp_CE_NoiseSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    int delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        track->noiseSwitchDelay = 257;
    } else {
        track->noiseSwitchDelay = delay + 1;
    }
    AkaoOp_C4_NoiseOn(track, config, mask);
}

static void AkaoOp_CF_NoiseSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    s16 delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        delay = 257;
    } else {
        delay++;
    }
    track->noiseSwitchDelay = delay;
}

static void AkaoOp_D2_FrequencyModulationSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    int delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        track->pitchLfoSwitchDelay = 257;
    } else {
        track->pitchLfoSwitchDelay = delay + 1;
    }
    AkaoOp_C6_PitchLfoOn(track, config, mask);
}

static void AkaoOp_D3_FrequencyModulationSwitch(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    s16 delay = *track->akaoSequencePointer++;
    if (delay == 0) {
        delay = 257;
    } else {
        delay++;
    }
    track->pitchLfoSwitchDelay = delay;
}

static void AkaoOp_CB_SfxReset(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~AKAO_UPDATE_LFO_MASK;
    AkaoOp_C5_NoiseOff(track, config, mask);
    AkaoOp_C7_PitchLfoOff(track, config, mask);
    AkaoOp_C3_ReverbOff(track, config, mask);
    track->sfxMask &= ~(AKAO_SFX_LEGATO | AKAO_SFX_FULL_LENGTH);
}

static void AkaoOp_D4_SideChainPlaybackOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags |= AKAO_UPDATE_SIDE_CHAIN_PITCH;
}

static void AkaoOp_D5_SideChainPlaybackOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~AKAO_UPDATE_SIDE_CHAIN_PITCH;
}

static void AkaoOp_D6_SideChainPitchVolOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags |= AKAO_UPDATE_SIDE_CHAIN_VOL;
}

static void AkaoOp_D7_SideChainPitchVolOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~AKAO_UPDATE_SIDE_CHAIN_VOL;
}

static void AkaoOp_EE_Jump(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
}

static void AkaoOp_EF_JumpConditional(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    int cond = *track->akaoSequencePointer++;
    if (config->condition != 0 && cond <= config->condition) {
        track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
        config->conditionStored = cond;
    } else {
        track->akaoSequencePointer += 2;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A0_FinishChannel);

static void AkaoOp_Null(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoOp_A0_FinishChannel(track, config, mask);
}
