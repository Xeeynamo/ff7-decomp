//! PSYQ=3.3 CC1=2.6.3 G=8 COMM=true

#include "akao_private.h"
#include <kernel.h>
#include <libapi.h>
#include <libetc.h>

#define READ_S8(addr) ((s8)(*(addr)++))
#define READ_S16(addr) ((s16)(*(addr)++ | (*(addr)++ << 8)))

void AkaoCmd_10_PlayMusic(AkaoQueuedCommand* cmd);
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoQueuedCommand* cmd);
void AkaoCmd_15_PlayMusicSwapSaved(AkaoQueuedCommand* cmd);
void AkaoCmd_18_FadePlayMusic(AkaoQueuedCommand* cmd);
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoQueuedCommand* cmd);
void AkaoCmd_20_PlaySound(AkaoQueuedCommand* cmd);
void AkaoCmd_21_PlayTwoSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_22_PlayThreeSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_23_PlayFourSounds(AkaoQueuedCommand* cmd);
void AkaoCmd_29_PlaySlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_2A_PlaySlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_2B_PlaySlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_30_PlayMenuSound(AkaoQueuedCommand* cmd);
void AkaoCmd_34_PlayDirect(AkaoQueuedCommand* cmd);
static void AkaoCmd_80_SetStereoMode(AkaoQueuedCommand* cmd);
static void AkaoCmd_81_SetMonoMode(AkaoQueuedCommand* cmd);
void AkaoCmd_82_ResetVolume(AkaoQueuedCommand* cmd);
void AkaoCmd_90_SetMuteMusicMask(AkaoQueuedCommand* cmd);
void AkaoCmd_92_SetCondition(AkaoQueuedCommand* cmd);
void AkaoCmd_9A_FlushPendingMusicUpdates(void);
void AkaoCmd_9B_ApplyPendingMusicUpdates(AkaoQueuedCommand* cmd);
void AkaoCmd_9C_FlushPendingSfxUpdates(void);
void AkaoCmd_9D_ApplyPendingSfxUpdates(void);
void AkaoUpdateChannelParamsToSpu(s32 voiceIdx, AkaoVoiceAttr* attr);
void AkaoUpdateNoiseVoices(void);
void AkaoUpdateReverbVoices(void);
void AkaoUpdatePitchLfoVoices(void);

void AkaoCmd_A0_SetVolBalanceSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A1_SetVolBalanceSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_A2_SetVolBalanceSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_A3_SetVolBalanceSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_A4_SlideVolBalanceSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A5_SlideVolBalanceSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_A6_SlideVolBalanceSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_A7_SlideVolBalanceSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_A8_SetPanSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_A9_SetPanSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_AA_SetPanSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_AB_SetPanSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_AC_SlidePanSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_AD_SlidePanSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_AE_SlidePanSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_AF_SlidePanSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B0_SetPitchSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_B1_SetPitchSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_B2_SetPitchSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_B3_SetPitchSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B4_SlidePitchSlot2(AkaoQueuedCommand* cmd);
void AkaoCmd_B5_SlidePitchSlot1(AkaoQueuedCommand* cmd);
void AkaoCmd_B6_SlidePitchSlot0(AkaoQueuedCommand* cmd);
void AkaoCmd_B7_SlidePitchSlot3(AkaoQueuedCommand* cmd);
void AkaoCmd_B8_SetAllVolBalance(AkaoQueuedCommand* cmd);
void AkaoCmd_B9_SlideAllVolBalance(AkaoQueuedCommand* cmd);
void AkaoCmd_BA_SetAllPan(AkaoQueuedCommand* cmd);
void AkaoCmd_BB_SlideAllPan(AkaoQueuedCommand* cmd);
void AkaoCmd_BC_SetAllPitch(AkaoQueuedCommand* cmd);
void AkaoCmd_BD_SlideAllPitch(AkaoQueuedCommand* cmd);
void AkaoCmd_C0_VolumeSet(AkaoQueuedCommand* cmd);
void AkaoCmd_C1_VolSlideFromCurr(AkaoVolSlideFromCurr* cmd);
void AkaoCmd_C2_VolSlideBetweenTargets(AkaoVolSlideBetweenTargets* cmd);
void AkaoCmd_C8_SetCdVol(AkaoSetCdVol* cmd);
static void AkaoUpdateCdVolume(void);
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
void AkaoCmd_F4_SaveState(AkaoQueuedCommand* cmd);
void AkaoCmd_F5_RestoreState(AkaoQueuedCommand* cmd);
static void AkaoCmd_F8_StreamReverbMaskClear(AkaoQueuedCommand* cmd);
static void AkaoCmd_F9_StreamReverbMaskRestore(AkaoQueuedCommand* cmd);
static void AkaoCmd_FA_StopStream(void);
void AkaoCmd_Null(AkaoQueuedCommand* cmd);
void AkaoStreamInit(AkaoQueuedCommand* cmd);
void AkaoOp_A0_FinishChannel(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_A1_LoadInstrument(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_A2_NextNoteLength(AkaoChannel* track);
static void AkaoOp_A3_MasterVol(AkaoChannel* track);
void AkaoOp_A4_PitchBendSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_A5_SetOctave(AkaoChannel* track);
static void AkaoOp_A6_IncOctave(AkaoChannel* track);
static void AkaoOp_A7_DecOctave(AkaoChannel* track);
static void AkaoOp_A8_SetVol(AkaoChannel* track);
void AkaoOp_A9_SetVolSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_AA_SetPan(AkaoChannel* track);
static void AkaoOp_AB_SetPanSlide(AkaoChannel* track);
void AkaoOp_AC_NoiseClockFreq(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_AD_SetAr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_AE_SetDr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_AF_SetSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_B0_SetVoiceDrSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B1_SetSr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B2_SetRr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B3_ResetAdsr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B4_Vibrato(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B5_VibratoDepth(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_B6_VibratoOff(AkaoChannel* track);
void AkaoOp_B7_AttackMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_B8_Tremolo(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_B9_TremoloDepth(AkaoChannel* track);
static void AkaoOp_BA_TremoloOff(AkaoChannel* track);
void AkaoOp_BB_SustainMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_BC_SetPanLfo(AkaoChannel* track);
static void AkaoOp_BD_PanLfoDepth(AkaoChannel* track);
static void AkaoOp_BE_PanLfoOff(AkaoChannel* track);
void AkaoOp_BF_ReleaseMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C0_TransposeAbsolute(AkaoChannel* track);
static void AkaoOp_C1_TransposeRelative(AkaoChannel* track);
static void AkaoOp_C2_ReverbOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C3_ReverbOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C4_NoiseOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C5_NoiseOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C6_PitchLfoOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_C7_PitchLfoOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_C8_LoopPoint(AkaoChannel* track);
void AkaoOp_C9_LoopReturnTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
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
void AkaoOp_DD_VibratoDepthSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_DE_TremoloDepthSlideFromCurr(AkaoChannel* track);
static void AkaoOp_DF_PanLfoDepthSlideFromCurr(AkaoChannel* track);
void AkaoOp_E8_Tempo(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_E9_TempoSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EA_ReverbDepth(u8** cursor, AkaoChannel* track);
void AkaoOp_EB_ReverbDepthSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EC_DrumModeOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_ED_DrumModeOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EE_Jump(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_EF_JumpConditional(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F0_LoopJumpTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F1_LoopBreakTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F2_LoadInstrument(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F4_OverlayVoiceOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F5_OverlayVoiceOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track);
void AkaoOp_F7_OverlayVolBalanceSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F8_AltVoiceOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
void AkaoOp_F9_AltVoiceOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask);
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
    AkaoCmd_29_PlaySlot1,
    AkaoCmd_2A_PlaySlot0,
    AkaoCmd_2B_PlaySlot3,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_30_PlayMenuSound,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_34_PlayDirect,
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
    AkaoCmd_82_ResetVolume,
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
    AkaoCmd_9C_FlushPendingSfxUpdates,
    AkaoCmd_9D_ApplyPendingSfxUpdates,
    AkaoCmd_Null,
    AkaoCmd_Null,
    AkaoCmd_A0_SetVolBalanceSlot2,
    AkaoCmd_A1_SetVolBalanceSlot1,
    AkaoCmd_A2_SetVolBalanceSlot0,
    AkaoCmd_A3_SetVolBalanceSlot3,
    AkaoCmd_A4_SlideVolBalanceSlot2,
    AkaoCmd_A5_SlideVolBalanceSlot1,
    AkaoCmd_A6_SlideVolBalanceSlot0,
    AkaoCmd_A7_SlideVolBalanceSlot3,
    AkaoCmd_A8_SetPanSlot2,
    AkaoCmd_A9_SetPanSlot1,
    AkaoCmd_AA_SetPanSlot0,
    AkaoCmd_AB_SetPanSlot3,
    AkaoCmd_AC_SlidePanSlot2,
    AkaoCmd_AD_SlidePanSlot1,
    AkaoCmd_AE_SlidePanSlot0,
    AkaoCmd_AF_SlidePanSlot3,
    AkaoCmd_B0_SetPitchSlot2,
    AkaoCmd_B1_SetPitchSlot1,
    AkaoCmd_B2_SetPitchSlot0,
    AkaoCmd_B3_SetPitchSlot3,
    AkaoCmd_B4_SlidePitchSlot2,
    AkaoCmd_B5_SlidePitchSlot1,
    AkaoCmd_B6_SlidePitchSlot0,
    AkaoCmd_B7_SlidePitchSlot3,
    AkaoCmd_B8_SetAllVolBalance,
    AkaoCmd_B9_SlideAllVolBalance,
    AkaoCmd_BA_SetAllPan,
    AkaoCmd_BB_SlideAllPan,
    AkaoCmd_BC_SetAllPitch,
    AkaoCmd_BD_SlideAllPitch,
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
    AkaoCmd_F4_SaveState,
    AkaoCmd_F5_RestoreState,
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

u16 g_AkaoLengthTable[11] = {
    0xC0C0, 0x6060, 0x3030, 0x1818, 0x0C0C, 0x0606, 0x0303, 0x2020, 0x1010, 0x0808, 0x0404,
};

u8 g_AkaoDummyStopSequence[] = {0xA0};

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
        g_AkaoBgmLanes[0].reverbMode = in_ReverbMode;
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
    while (nwords) {
        nwords -= 1;
        *dst++ = *src++;
    }
}

void AkaoInstrInit(AkaoChannel*, u16);

// Resets and initializes SFX audio channel parameters, pointing to seqData with default volume and instrument 5.
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

void AkaoMusicChannelsInit(void) {
    AkaoChannel* channel;
    s32 active;
    s32 bit;
    u16* offsets;
    u16 offset;
    s32 stored;

    offsets = (u16*)g_AkaoMusicBuffer;
#ifndef PLATFORM_PSYZ
    active = *((s32*)offsets)++ & 0xFFFFFF;
#else
    active = *(s32*)offsets & 0xFFFFFF;
    offsets += 2;
#endif
    channel = g_Channel1;
    g_AkaoBgmLanes->offMask |= 0xFFFFFF;
    g_AkaoBgmLanes->activeMask = active;
    bit = 1;
    while (active) {
        if (active & bit) {
            offset = *offsets++;
            active ^= bit;
            channel->akaoSequencePointer = (u8*)offsets + offset;
            channel->length = 0x103;
            channel->volumeMultiplier = 0x7F;
            AkaoInstrInit(channel, 0x14);
            channel->volumeLevel = 0x3FFF0000;
            channel->volBalance = 0x4000;
            channel->volPan = 0x4000;
            channel->drumOffset = (u8*)g_AkaoMusicBuffer;
            channel->fineTuning = 0;
            channel->transpose = 0;
            channel->pitchSlide = 0;
            channel->keyAdd = 0;
            channel->pitchSlideStepsCur = 0;
            channel->portamentoSteps = 0;
            channel->lengthFixed = 0;
            channel->lengthStored = 0;
            channel->volBalanceSlideSteps = 0;
            channel->volSlideSteps = 0;
            channel->volPanSlideSteps = 0;
            channel->portamentoSteps = 0;
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
        channel++;
        bit <<= 1;
    }
    g_AkaoBgmLanes->tempo = 0xFFFF0000;
    g_AkaoBgmLanes->tempoUpdate = 1;
    g_AkaoBgmLanes->tempoSlideSteps = 0;
    g_AkaoBgmLanes->reverbDepth = 0;
    g_AkaoBgmLanes->reverbDepthSlideSteps = 0;
    g_AkaoBgmLanes[0].reverbDepthSlideStep = 0;
    g_AkaoBgmLanes->updateFlags = 0;
    g_AkaoBgmLanes->timerLowerCur = 0;
    g_AkaoBgmLanes->timerLower = 0;
    g_AkaoBgmLanes->timerUpperCur = 0;
    g_AkaoBgmLanes->timerTopCur = 0;
    g_AkaoBgmLanes[0].noiseMask = 0;
    g_AkaoBgmLanes[0].reverbMask = 0;
    g_AkaoBgmLanes[0].pitchLfoMask = 0;
    g_AkaoBgmLanes[0].condition = 0;
    g_AkaoBgmLanes[0].conditionStored = 0;
    g_AkaoBgmLanes->muteMusic = 0;
    g_AkaoBgmLanes->keyedMask = 0;
    g_AkaoBgmLanes->onMask = 0;
    g_AkaoBgmLanes->altMask = 0;
    g_AkaoBgmLanes->overMask = 0;
    if (g_AkaoControlFlags & AKAO_CONTROL_PAUSE_MUSIC_UPDATE) {
        stored = g_AkaoBgmLanes->activeMask;
        g_AkaoBgmLanes->activeMask = 0;
        g_AkaoBgmLanes->activeMaskStored = stored;
    }
    AkaoUpdateNoiseVoices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
}

static void AkaoMusicStopChannels1(void) {
    s32 mask;
    s32 bit;
    AkaoChannel* channel;
    s32 overMask;
    s32 altMask;

    if (g_AkaoBgmLanes->activeMask) {
        channel = g_Channel1;
        bit = 1;
        overMask = g_AkaoBgmLanes->overMask;
        altMask = g_AkaoBgmLanes->altMask;
        g_AkaoBgmLanes->altMask = 0;
        g_AkaoBgmLanes->overMask = 0;
        g_AkaoBgmLanes->keyedMask = 0;
        g_AkaoBgmLanes->onMask = 0;
        overMask |= altMask;
        mask = g_AkaoBgmLanes->activeMask;
        mask |= overMask;
        g_AkaoBgmLanes->activeMask = mask;
        g_AkaoBgmLanes->offMask |= mask;
        do {
            if (mask & bit) {
                mask ^= bit;
                channel->length = 0x204;
                channel->akaoSequencePointer = g_AkaoDummyStopSequence;
            }
            bit <<= 1;
            channel++;
        } while (mask);
    }
}

void AkaoMusicStopChannels12(void) {
    s32 mask;

    mask = g_AkaoBgmLanes->activeMask;
    if (mask) {
        AkaoChannel* channel;
        s32 bit;
        s32 overMask;
        s32 altMask;

        channel = g_Channel1;
        bit = 1;
        overMask = g_AkaoBgmLanes->overMask;
        altMask = g_AkaoBgmLanes->altMask;
        g_AkaoBgmLanes->altMask = 0;
        g_AkaoBgmLanes->overMask = 0;
        g_AkaoBgmLanes->keyedMask = 0;
        g_AkaoBgmLanes->onMask = 0;
        overMask |= altMask;
        mask |= overMask;
        g_AkaoBgmLanes->activeMask = mask;
        g_AkaoBgmLanes->offMask |= mask;
        do {
            if (mask & bit) {
                mask ^= bit;
                channel->length = 0x204;
                channel->akaoSequencePointer = g_AkaoDummyStopSequence;
            }
            bit <<= 1;
            channel++;
        } while (mask);
    }
    mask = g_AkaoBgmLanes[1].activeMask;
    if (mask) {
        AkaoChannel* channel;
        s32 bit;
        s32 overMask;
        s32 altMask;

        channel = g_Channel2;
        bit = 1;
        overMask = g_AkaoBgmLanes[1].overMask;
        altMask = g_AkaoBgmLanes[1].altMask;
        g_AkaoBgmLanes[1].altMask = 0;
        g_AkaoBgmLanes[1].overMask = 0;
        g_AkaoBgmLanes[1].keyedMask = 0;
        g_AkaoBgmLanes[1].onMask = 0;
        overMask |= altMask;
        mask |= overMask;
        g_AkaoBgmLanes[1].activeMask = mask;
        g_AkaoBgmLanes[1].offMask |= mask;
        do {
            if (mask & bit) {
                mask ^= bit;
                channel->length = 0x204;
                channel->akaoSequencePointer = g_AkaoDummyStopSequence;
            }
            bit <<= 1;
            channel++;
        } while (mask);
    }
}

void AkaoSoundChannelsInit(u16 volPan, s32 channelId, s32 seq1, s32 seq2) {
    AkaoChannel* channel;
    u32 active;
    u32 all;
    u16 id;

    active = 0;
    volPan = (volPan & 0x7F) << 8;
    id = channelId;
    channel = &g_Channel1[id];
    channel[0].length = 0x101;
    channel[1].length = 0x101;
    channel[0].akaoSequencePointer = g_AkaoDummyStopSequence;
    channel[1].akaoSequencePointer = g_AkaoDummyStopSequence;
    channel[0].playingType = AKAO_SOUND;
    channel[1].playingType = AKAO_SOUND;
    channel[0].setToMinusOne = -1;
    channel[1].setToMinusOne = -1;
    if (seq1) {
        active = 1;
        SoundChannelInit(channel, (u8*)seq1);
        channel->volPan = volPan;
        channel->volPanSlideSteps = 0;
    }
    channel++;
    if (seq2) {
        active |= 2;
        SoundChannelInit(channel, (u8*)seq2);
        channel->volPan = volPan;
        channel->volPanSlideSteps = 0;
    }
    active <<= id - 0x20;
    all = active | g_AkaoSfxLanes[0].activeMask;
    active = (3 << (id - 0x20)) & all;
    g_AkaoSfxLanes[0].activeMask = all;
    g_AkaoSfxLanes->offMask |= active;
    active = ~active;
    g_AkaoSfxLanes->onMask &= active;
    g_AkaoSfxLanes->keyedMask &= active;
    g_AkaoSfxLanes->noiseMask &= active;
    g_AkaoSfxLanes->reverbMask &= active;
    g_AkaoSfxLanes->pitchLfoMask &= active;
    if (g_AkaoControlFlags & AKAO_CONTROL_PAUSE_UPDATE) {
        active = all;
        if (g_AkaoSoundSlots[3].voices[0].playingType == AKAO_MENU) {
            g_AkaoSfxLanes[0].activeMask = active & 0xC00000;
            active &= ~0xC00000;
        } else {
            g_AkaoSfxLanes[0].activeMask = 0;
        }
        g_AkaoSfxLanes->activeMaskStored |= active;
    }
    AkaoUpdateNoiseVoices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
}

void AkaoSoundMenuChannelsInit(s32 seq0, s32 seq1) {
    AkaoChannel* channel;
    u32 active;

    active = 0;
    channel = g_AkaoSoundSlots[3].voices;
    channel[1].length = channel[active].length = 0x101;
    channel[active].akaoSequencePointer = g_AkaoDummyStopSequence;
    channel[1].akaoSequencePointer = g_AkaoDummyStopSequence;
    channel[0].playingType = AKAO_MENU;
    channel[1].playingType = AKAO_MENU;
    channel[0].setToMinusOne = -1;
    channel[1].setToMinusOne = -1;
    active = 0;
    if (seq0) {
        active = 1;
        SoundChannelInit(channel, (u8*)seq0);
        channel->volPan = AKAO_PAN_CENTER << 8;
        channel->volPanSlideSteps = 0;
    }
    if (seq1) {
        channel = &channel[1];
        active |= 2;
        SoundChannelInit(channel, (u8*)seq1);
        channel->volPan = AKAO_PAN_CENTER << 8;
        channel->volPanSlideSteps = 0;
    }
    active <<= 22;
    g_AkaoSfxLanes[0].activeMask |= active;
    active = 0xC00000;
    g_AkaoSfxLanes->offMask |= active;
    active = ~active;
    g_AkaoSfxLanes->onMask &= active;
    g_AkaoSfxLanes->keyedMask &= active;
    g_AkaoSfxLanes->noiseMask &= active;
    g_AkaoSfxLanes->reverbMask &= active;
    g_AkaoSfxLanes->pitchLfoMask &= active;
    AkaoUpdateNoiseVoices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
    g_AkaoSfxLanes->activeMaskStored &= active;
}

void AkaoSoundChannelsStop(void) {
    AkaoChannel* channel;
    u16 i;

    for (channel = g_AkaoSoundSlots[0].voices, i = 0x30; i < 0x38; i++, channel++) {
        if (channel->playingType != AKAO_MENU) {
            channel->length = 0x204;
            channel->akaoSequencePointer = g_AkaoDummyStopSequence;
        }
    }
    if (g_AkaoSoundSlots[3].voices[0].playingType == AKAO_MENU) {
        g_AkaoSfxLanes->onMask &= 0xC00000;
        g_AkaoSfxLanes->keyedMask &= 0xC00000;
        g_AkaoSfxLanes->offMask = g_AkaoSfxLanes->offMask & (~0xC00000 & g_AkaoSfxLanes->activeMask);
    } else {
        g_AkaoSfxLanes->offMask = g_AkaoSfxLanes->activeMask;
        g_AkaoSfxLanes->onMask = 0;
        g_AkaoSfxLanes->keyedMask = 0;
    }
}

void AkaoSoundChannelsClear(u16 voice, s32 slots) {
    AkaoChannel* channel;
    u32 mask;
    u16 i;

    channel = &g_AkaoSoundSlots[0].voices[voice + 1];
    i = slots * 2;
    switch (slots & 0xFFFF) {
    case 1:
        mask = 3 << (voice + 0x10);
        g_AkaoSfxLanes->onMask &= ~mask;
        g_AkaoSfxLanes->keyedMask &= ~mask;
        g_AkaoSfxLanes->offMask |= mask;
        break;
    case 2:
        g_AkaoSfxLanes->onMask &= ~0x3C0000;
        g_AkaoSfxLanes->keyedMask &= ~0x3C0000;
        g_AkaoSfxLanes->offMask |= 0x3C0000;
        break;
    case 3:
        g_AkaoSfxLanes->onMask &= ~0x3F0000;
        g_AkaoSfxLanes->keyedMask &= ~0x3F0000;
        g_AkaoSfxLanes->offMask |= 0x3F0000;
        break;
    case 4:
        g_AkaoSfxLanes->onMask &= ~0xFF0000;
        g_AkaoSfxLanes->keyedMask &= ~0xFF0000;
        g_AkaoSfxLanes->offMask |= 0xFF0000;
        break;
    }
    while (i) {
        channel->length = 0x204;
        channel->akaoSequencePointer = g_AkaoDummyStopSequence;
        i--;
        channel--;
    }
}

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

void AkaoMusicVolReset(void) {
    AkaoChannel* channel;
    s32 active;
    s32 mask;

    active = g_AkaoBgmLanes[0].activeMask;
    channel = g_Channel1;
    if (active) {
        mask = 1;
        while (active) {
            if (active & mask) {
                active ^= mask;
                channel->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
            }
            channel++;
            mask <<= 1;
        }
    }
}

void AkaoSoundVolReset(void) {
    AkaoChannel* channel;
    u32 active;
    s32 mask;

    active = g_AkaoSfxLanes->activeMask;
    channel = g_AkaoSoundSlots[0].voices;
    if (active) {
        mask = 0x10000;
        while (active) {
            if (active & mask) {
                active ^= mask;
                channel->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
            }
            channel++;
            mask <<= 1;
        }
    }
}

// by sound effects or stream audio, right before channel state backup/switching.
void AkaoMusicSyncKeyStatus(void) {
    AkaoChannel* channel;
    s32 active;
    s32 bit;
    s32 alt;

    active = (g_AkaoBgmLanes->activeMask | g_AkaoBgmLanes->overMask) & ~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask);
    if (active) {
        bit = 1;
        channel = g_Channel1;
        g_AkaoBgmLanes->onMask = g_AkaoBgmLanes->keyedMask;
        while (active) {
            if (active & bit) {
                if (SpuGetKeyStatus(bit) == SPU_ON) {
                    if (channel->updateFlags & AKAO_UPDATE_ALTERNATIVE) {
                        channel->updateFlags |= 0x400;
                    }
                    g_AkaoBgmLanes->onMask |= bit;
                } else if (
                    (channel->updateFlags & 0x600) == 0x600 &&
                    (~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask) & (alt = 1 << channel->alternativeChannelId)) &&
                    SpuGetKeyStatus(alt) == SPU_ON) {
                    g_AkaoBgmLanes->onMask |= bit;
                } else {
                    g_AkaoBgmLanes->onMask &= ~bit;
                    g_AkaoBgmLanes->keyedMask &= ~bit;
                }
                active ^= bit;
            }
            channel++;
            bit <<= 1;
        }
    }
}

// Synchronizes g_AkaoSfxLanes->onMask and g_AkaoSfxLanes->keyedMask with the hardware
// SPU key status (SpuGetKeyStatus) for all active SFX audio channels.
static u8 AkaoScanSequenceTerminator(u8** seqPtr);
void AkaoSoundSyncKeyStatus(void) {
    AkaoChannel* channel;
    u32 active;
    s32 bit;

    active = g_AkaoSfxLanes->activeMask;
    bit = 0x10000;
    if (active) {
        channel = g_AkaoSoundSlots[0].voices;
        g_AkaoSfxLanes->onMask = g_AkaoSfxLanes->keyedMask;
        while (active) {
            if (active & bit) {
                if (AkaoScanSequenceTerminator(&channel->akaoSequencePointer) == AKAO_OP_LOOP_RETURN) {
                    if (SpuGetKeyStatus(bit) == SPU_ON) {
                        g_AkaoSfxLanes->onMask |= bit;
                    } else {
                        g_AkaoSfxLanes->onMask &= ~bit;
                        g_AkaoSfxLanes->keyedMask &= ~bit;
                    }
                    channel->voiceAttr.mask = 0x1FF93;
                } else if (channel->playingType != AKAO_MENU) {
                    g_AkaoSfxLanes->activeMask &= ~bit;
                    g_AkaoSfxLanes->onMask &= ~bit;
                    g_AkaoSfxLanes->keyedMask &= ~bit;
                }
                active ^= bit;
            }
            channel++;
            bit <<= 1;
        }
    }
}

void AkaoMusicRestoreChannelsAndConfig(u16 slot) {
    u32* src;
    u32* dst;
    AkaoChannel* channel;
    u32 active;
    s32 bit;
    u16 i;
    u16 j;
    s32 stored;

    g_AkaoBgmLanes->activeMask = g_AkaoPrevBgmLanes[slot].activeMask;
    g_AkaoBgmLanes->onMask = g_AkaoPrevBgmLanes[slot].onMask;
    g_AkaoBgmLanes->keyedMask = g_AkaoPrevBgmLanes[slot].keyedMask;
    g_AkaoBgmLanes->tempo = g_AkaoPrevBgmLanes[slot].tempo;
    g_AkaoBgmLanes->tempoSlideStep = g_AkaoPrevBgmLanes[slot].tempoSlideStep;
    g_AkaoBgmLanes->tempoSlideSteps = g_AkaoPrevBgmLanes[slot].tempoSlideSteps;
    g_AkaoBgmLanes->tempoUpdate = g_AkaoPrevBgmLanes[slot].tempoUpdate;
    g_AkaoBgmLanes->overMask = g_AkaoPrevBgmLanes[slot].overMask;
    g_AkaoBgmLanes->altMask = g_AkaoPrevBgmLanes[slot].altMask;
    g_AkaoBgmLanes->musicId = g_AkaoPrevBgmLanes[slot].musicId;
    g_AkaoBgmLanes->conditionStored = g_AkaoPrevBgmLanes[slot].conditionStored;
    g_AkaoBgmLanes->condition = g_AkaoPrevBgmLanes[slot].condition;
    g_AkaoBgmLanes->reverbDepth = g_AkaoPrevBgmLanes[slot].reverbDepth;
    g_AkaoBgmLanes->reverbDepthSlideStep = g_AkaoPrevBgmLanes[slot].reverbDepthSlideStep;
    g_AkaoBgmLanes->reverbDepthSlideSteps = g_AkaoPrevBgmLanes[slot].reverbDepthSlideSteps;
    g_AkaoBgmLanes->noiseClock = g_AkaoPrevBgmLanes[slot].noiseClock;
    g_AkaoBgmLanes->noiseMask = g_AkaoPrevBgmLanes[slot].noiseMask;
    g_AkaoBgmLanes->reverbMask = g_AkaoPrevBgmLanes[slot].reverbMask;
    g_AkaoBgmLanes->pitchLfoMask = g_AkaoPrevBgmLanes[slot].pitchLfoMask;
    g_AkaoBgmLanes->muteMusic = g_AkaoPrevBgmLanes[slot].muteMusic;
    g_AkaoBgmLanes->updateFlags = g_AkaoPrevBgmLanes[slot].updateFlags;
    g_AkaoBgmLanes->timerUpper = g_AkaoPrevBgmLanes[slot].timerUpper;
    g_AkaoBgmLanes->timerUpperCur = g_AkaoPrevBgmLanes[slot].timerUpperCur;
    g_AkaoBgmLanes->timerLower = g_AkaoPrevBgmLanes[slot].timerLower;
    g_AkaoBgmLanes->timerLowerCur = g_AkaoPrevBgmLanes[slot].timerLowerCur - 2;
    g_AkaoBgmLanes->timerTopCur = g_AkaoPrevBgmLanes[slot].timerTopCur;
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_REVERB;
    src = (u32*)g_AkaoSavedChannels1;
    if (!slot) {
        src = (u32*)(g_AkaoSavedChannels1 - AKAO_NUM_VOICES);
    }
    for (dst = (u32*)g_Channel1, i = sizeof(AkaoChannel) * AKAO_NUM_VOICES / 4; i; i--, src++, dst++) {
        *dst = *src;
    }
    active = g_AkaoBgmLanes->activeMask;
    if (active) {
        for (j = AKAO_NUM_VOICES, channel = g_Channel1, bit = 1; j; j--, channel++, bit <<= 1) {
            if (!(active & bit)) {
                channel->length = 0x204;
                channel->akaoSequencePointer = g_AkaoDummyStopSequence;
            }
        }
    }
    active |= g_AkaoBgmLanes->overMask;
    g_AkaoBgmLanes->offMask = ~g_AkaoBgmLanes->onMask & 0xFFFFFF;
    channel = g_Channel1;
    bit = 1;
    while (active) {
        if (active & bit) {
            active ^= bit;
            channel->length += 0x202;
            channel->voiceAttr.mask |= AKAO_UPDATE_SPU_ALL;
        }
        channel++;
        bit <<= 1;
    }
    AkaoUpdateNoiseVoices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
    if (g_AkaoVolMulMusicSlideSteps == 0 && g_AkaoVoiceWork->currentKey == 0) {
        g_AkaoVolMulMusicSlideSteps = 60;
        g_AkaoVolMulMusicSlideStep = (g_AkaoVolMulMusic - 0xA0000) / 60;
        g_AkaoVolMulMusic = 0xA0000;
    }
    g_AkaoPrevBgmLanes[slot].musicId = 0;
    if (g_AkaoControlFlags & AKAO_CONTROL_PAUSE_MUSIC_UPDATE) {
        stored = g_AkaoBgmLanes->activeMask;
        g_AkaoBgmLanes->activeMask = 0;
        g_AkaoBgmLanes->activeMaskStored = stored;
    }
}

void AkaoMusicCopyChannels1Into2(void) {
    u32* src;
    u32* dst;
    u16 i;
    AkaoChannel* channel;
    u16 remaining;

    for (i = sizeof(AkaoChannel) * AKAO_NUM_VOICES / 4, src = (u32*)g_Channel1,
        dst = (u32*)(g_Channel1 + AKAO_NUM_VOICES);
         i; i--, src++, dst++) {
        *dst = *src;
    }
    for (i = sizeof(AkaoChannelConfig) / 4, src = (u32*)&g_AkaoBgmLanes[0], dst = (u32*)&g_AkaoBgmLanes[1]; i; i--,
        src++, dst++) {
        *dst = *src;
    }
    for (src = (u32*)g_AkaoMusicBuffer, dst = (u32*)g_AkaoMusicBuffer + 0xC00, i = 0xC00; i; i--, src++, dst++) {
        *dst = *src;
    }
    for (channel = g_Channel2, i = AKAO_NUM_VOICES; i; i--, channel++) {
        channel->akaoSequencePointer += 0x3000;
        channel->drumOffset += 0x3000;
        channel->loopPoint[0] += 0x3000;
        channel->loopPoint[1] += 0x3000;
        channel->loopPoint[2] += 0x3000;
        channel->loopPoint[3] += 0x3000;
        channel->overlayChannelId += AKAO_NUM_VOICES;
    }
    for (i = 0, remaining = AKAO_NUM_VOICES; i < AKAO_NUM_VOICES; i++, remaining--) {
        g_AkaoVoiceWork[i].pitchSlide = 0x7F8000;
        g_AkaoVoiceWork[i].volSlide = -(0x7F8000 / (g_AkaoMusicFadeSteps * remaining));
        g_AkaoVoiceWork[i].currentKey = remaining * g_AkaoMusicFadeSteps;
    }
    g_Channel2VoiceMask = 0xFFFFFF;
    g_AkaoControlFlags &= ~AKAO_CONTROL_STATE_SAVED;
}

// Copies 24 audio channels (0x18C0 bytes) and channel configuration (0x60 bytes)
// from source to destination buffers.
void AkaoMusicCopyChannelsAndConfig(
    AkaoChannel* srcChannels, AkaoChannel* dstChannels, AkaoChannelConfig* srcConfig, AkaoChannelConfig* dstConfig) {
    u16 i;

    for (i = sizeof(AkaoChannel) * AKAO_NUM_VOICES / 4; i; i--, srcChannels = (AkaoChannel*)((u32*)srcChannels + 1),
        dstChannels = (AkaoChannel*)((u32*)dstChannels + 1)) {
        *(u32*)dstChannels = *(u32*)srcChannels;
    }
    for (i = sizeof(AkaoChannelConfig) / 4; i; i--, srcConfig = (AkaoChannelConfig*)((u32*)srcConfig + 1),
        dstConfig = (AkaoChannelConfig*)((u32*)dstConfig + 1)) {
        *(u32*)dstConfig = *(u32*)srcConfig;
    }
}

/////////////////////////
// AKAO COMMANDS
/////////////////////////

// Copies the sequence to the staging buffer, restores audio channels and config from backup
// if musicId matches backup slot 0 or 1, otherwise initializes fresh music audio channels.
void AkaoCmd_10_PlayMusic(AkaoQueuedCommand* cmd) {
    AkaoCopyMusic((s32*)(u_long)(u32)cmd->param0, cmd->param1);
    if (g_AkaoBgmLanes->musicId == BGM_TA) { // Final Fantasy VII Main Theme (World Map)
        AkaoMusicSyncKeyStatus();
        AkaoMusicCopyChannelsAndConfig(g_Channel1, g_AkaoSavedChannels1, g_AkaoBgmLanes, &g_AkaoPrevBgmLanes[1]);
    }
    AkaoMusicStopChannels1();
    if (g_AkaoPrevBgmLanes[0].musicId && g_AkaoPrevBgmLanes[0].musicId == (u16)cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(0);
    } else if (g_AkaoPrevBgmLanes[1].musicId && g_AkaoPrevBgmLanes[1].musicId == (u16)cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(1);
    } else {
        AkaoMusicChannelsInit();
    }
    g_AkaoBgmLanes->musicId = cmd->param2;
}

// Copies the music sequence to the staging buffer, backs up the currently playing song,
// (to backup slot 1 if BGM_TA [World map Main Theme] or slot 0 for any other song)
// stops the channels, initializes new channels from the beginning, and sets g_AkaoBgmLanes[0].musicId.
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoQueuedCommand* cmd) {
    AkaoChannelConfig* channelConfig;

    AkaoCopyMusic((s32*)(u_long)(u32)cmd->param0, cmd->param1);
    AkaoMusicSyncKeyStatus();
    channelConfig = g_AkaoBgmLanes;
    if (g_AkaoBgmLanes[0].musicId) {
        if (g_AkaoBgmLanes[0].musicId == BGM_TA) { // Final Fantasy VII Main Theme (World Map)
            AkaoMusicCopyChannelsAndConfig(g_Channel1, g_AkaoSavedChannels1, channelConfig, &g_AkaoPrevBgmLanes[1]);
        } else {
            AkaoMusicCopyChannelsAndConfig(g_Channel1, g_AkaoSavedChannels0, channelConfig, g_AkaoPrevBgmLanes);
        }
    }
    AkaoMusicStopChannels1();
    AkaoMusicChannelsInit();
    g_AkaoBgmLanes[0].musicId = cmd->param2;
}

// Copies the sequence to staging buffer, clears flag 0x100, and switches music with
// backup state swapping: if the requested music ID matches backup slot 0 or 1, active
// music (channel 1) is moved to channel 2 (transition) and saved back into the backup slot,
// while the target music is restored into active channel 1. If not saved in a slot, the
// current music is backed up and new channels are initialized fresh.
void AkaoCmd_15_PlayMusicSwapSaved(AkaoQueuedCommand* cmd) {
    g_AkaoControlFlags &= ~AKAO_CONTROL_STATE_SAVED;
    AkaoCopyMusic((s32*)(u_long)(u32)cmd->param0, cmd->param1);
    AkaoMusicSyncKeyStatus();
    if (g_AkaoPrevBgmLanes[0].musicId == (u16)cmd->param2) {
        AkaoMusicCopyChannelsAndConfig(g_Channel1, g_Channel1 + AKAO_NUM_VOICES, g_AkaoBgmLanes, &g_AkaoBgmLanes[1]);
        AkaoMusicStopChannels1();
        AkaoMusicRestoreChannelsAndConfig(0);
        if (g_AkaoBgmLanes[1].musicId == BGM_TA) {
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1 + AKAO_NUM_VOICES, g_AkaoSavedChannels1, &g_AkaoBgmLanes[1], &g_AkaoPrevBgmLanes[1]);
        } else {
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1 + AKAO_NUM_VOICES, g_AkaoSavedChannels0, &g_AkaoBgmLanes[1], g_AkaoPrevBgmLanes);
        }
    } else if (g_AkaoPrevBgmLanes[1].musicId == (u16)cmd->param2) {
        AkaoMusicCopyChannelsAndConfig(g_Channel1, g_Channel1 + AKAO_NUM_VOICES, g_AkaoBgmLanes, &g_AkaoBgmLanes[1]);
        AkaoMusicStopChannels1();
        AkaoMusicRestoreChannelsAndConfig(1);
        if (g_AkaoBgmLanes[1].musicId == BGM_TA) {
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1 + AKAO_NUM_VOICES, g_AkaoSavedChannels1, &g_AkaoBgmLanes[1], &g_AkaoPrevBgmLanes[1]);
        } else {
            AkaoMusicCopyChannelsAndConfig(
                g_Channel1 + AKAO_NUM_VOICES, g_AkaoSavedChannels0, &g_AkaoBgmLanes[1], g_AkaoPrevBgmLanes);
        }
    } else {
        if (g_AkaoBgmLanes->musicId) {
            if (g_AkaoBgmLanes->musicId == BGM_TA) {
                AkaoMusicCopyChannelsAndConfig(
                    g_Channel1, g_AkaoSavedChannels1, g_AkaoBgmLanes, &g_AkaoPrevBgmLanes[1]);
            } else {
                AkaoMusicCopyChannelsAndConfig(g_Channel1, g_AkaoSavedChannels0, g_AkaoBgmLanes, g_AkaoPrevBgmLanes);
            }
        }
        AkaoMusicStopChannels1();
        AkaoMusicChannelsInit();
    }
    g_AkaoBgmLanes[1].altMask = 0;
    g_AkaoBgmLanes[1].overMask = 0;
    g_AkaoBgmLanes[1].activeMask = 0;
    g_AkaoBgmLanes->musicId = cmd->param2;
}

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_10_PlayMusic (resuming from backup if previously saved).
void AkaoCmd_18_FadePlayMusic(AkaoQueuedCommand* cmd) {
    if (g_AkaoBgmLanes[0].musicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_10_PlayMusic(cmd);
}

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_14_PlayMusicSaveCurrent (saving current music to backup).
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoQueuedCommand* cmd) {
    if (g_AkaoBgmLanes[0].musicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_14_PlayMusicSaveCurrent(cmd);
}

// Clears audio channel 4 (1 voice) and initializes it with center pan (0x40)
// using the provided raw sound sequence pointers directly (bypassing table lookup).
void AkaoCmd_34_PlayDirect(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelsClear(4, 1);
    AkaoSoundChannelsInit(AKAO_PAN_CENTER, AKAO_SFX_SLOT_2, cmd->param0, cmd->param1);
}

// Clears audio channels for 2 voices starting at voice 4 (SFX slots 1 and 2),
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

// Clears audio channels for 4 voices starting at voice 6 (SFX slots 0 through 3),
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

void AkaoCmd_29_PlaySlot1(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(2, 1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
}

void AkaoCmd_2A_PlaySlot0(AkaoQueuedCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(0, 1);
    AkaoStreamStop();
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_0, seq0, seq1);
}

void AkaoCmd_2B_PlaySlot3(AkaoQueuedCommand* cmd) {
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
    if (steps) {
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
    if (targetVol) {
        effectiveSteps = targetVol;
    }
    targetVol = (cmd->targetVol & AKAO_VOL_MAX) << 0x10;
    startVol = (cmd->startVol & AKAO_VOL_MAX) << 0x10;
    g_AkaoVolMulMusicSlideSteps = effectiveSteps;
    g_AkaoVolMulMusic = startVol;
    g_AkaoVolMulMusicSlideStep = (targetVol - startVol) / effectiveSteps;
    AkaoMusicVolReset();
}

void AkaoCmd_C8_SetCdVol(AkaoSetCdVol* cmd) {
    g_AkaoCdVolSlideSteps = 0;
    g_AkaoCdVol.val = cmd->vol << 0x10;
    AkaoUpdateCdVolume();
}

// Starts a CD-audio volume slide from the current g_AkaoCdVol toward a
// target derived from cmd, over cmd's tick count.
void AkaoCmd_C9_CdVolSlideFromCurr(AkaoCdVolSlideFromCurr* cmd) {
    s32 steps;
    s32 effectiveSteps;

    steps = cmd->steps;
    effectiveSteps = 1;
    if (steps) {
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
    if (steps) {
        effectiveSteps = steps;
    }
    targetVolShifted = cmd->targetVol << 0x10;
    startVolShifted = cmd->startVol << 0x10;
    g_AkaoCdVolSlideSteps = effectiveSteps;
    g_AkaoCdVol.val = startVolShifted;
    g_AkaoCdVolSlideStep = (targetVolShifted - startVolShifted) / effectiveSteps;
}

// Sets the volume balance for a 2-voice SFX audio channel pair (voice[0]
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
// the specified step count for a 2-voice SFX audio channel pair.
static void AkaoSoundChannelSlideVolBalance(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s16 steps;
    s32 rawSteps;
    AkaoChannel* voice = slot->voices;

    rawSteps = cmd->param0;
    steps = 1;
    if (rawSteps) {
        steps = *(u16*)&cmd->param0;
    }
    voice[0].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_VOL_MAX) << 8) - voice[0].volBalance) / steps;
    voice[1].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_VOL_MAX) << 8) - voice[1].volBalance) / steps;
    voice[1].volBalanceSlideSteps = steps;
    voice[0].volBalanceSlideSteps = steps;
}

// Sets the volume balance across all 4 SFX audio channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_B8_SetAllVolBalance(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the volume balance across all 4 SFX audio channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target balance in cmd.
void AkaoCmd_B9_SlideAllVolBalance(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A0_SetVolBalanceSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A4_SlideVolBalanceSlot2(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[2]);
}

void AkaoCmd_A1_SetVolBalanceSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_A5_SlideVolBalanceSlot1(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[1]);
}

void AkaoCmd_A2_SetVolBalanceSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_A6_SlideVolBalanceSlot0(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A3_SetVolBalanceSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_A7_SlideVolBalanceSlot3(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[3]);
}

// Sets the stereo pan for a 2-voice SFX audio channel pair (voice[0]
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
// the specified step count for a 2-voice SFX audio channel pair.
static void AkaoSoundChannelSlidePan(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s16 steps;
    s32 rawSteps;
    AkaoChannel* voice = slot->voices;

    rawSteps = cmd->param0;
    steps = 1;
    if (rawSteps) {
        steps = *(u16*)&cmd->param0;
    }
    voice[0].volPanSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_PAN_MAX) << 8) - voice[0].volPan) / steps;
    voice[1].volPanSlideStep = (s16)(((*(u16*)&cmd->param1 & AKAO_PAN_MAX) << 8) - voice[1].volPan) / steps;
    voice[1].volPanSlideSteps = steps;
    voice[0].volPanSlideSteps = steps;
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BA_SetAllPan(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BB_SlideAllPan(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A8_SetPanSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_AC_SlidePanSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A9_SetPanSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AD_SlidePanSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AA_SetPanSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AE_SlidePanSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AB_SetPanSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPan(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_AF_SlidePanSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[3]); }

// Sets the pitch multiplier for a 2-voice SFX audio channel pair
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

static void AkaoSoundChannelSlidePitch(AkaoQueuedCommand* cmd, AkaoSoundSlot* slot) {
    s8* cmdBytes = (s8*)cmd;
    s32 temp;
    AkaoChannel* voice = slot->voices;
    s32 steps;

    steps = 1;
    temp = cmd->param0 != 0; // FAKE MATCH
    if (temp) {
        steps = cmd->param0;
    }
    voice[0].pitchMulSoundSlideStep = (s32)((cmdBytes[8] << 8) - voice[0].pitchMulSound) / steps;
    temp = (s32)((cmdBytes[8] << 8) - voice[1].pitchMulSound) / steps;
    voice[1].pitchMulSoundSlideStep = temp;
    voice[0].pitchMulSoundSlideSteps = voice[1].pitchMulSoundSlideSteps = steps;
}

// Sets the pitch multiplier across all 4 SFX audio channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_BC_SetAllPitch(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the pitch multiplier across all 4 SFX audio channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target pitch multiplier in cmd.
void AkaoCmd_BD_SlideAllPitch(AkaoQueuedCommand* cmd) {
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_B0_SetPitchSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B4_SlidePitchSlot2(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B1_SetPitchSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B5_SlidePitchSlot1(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B2_SetPitchSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B6_SlidePitchSlot0(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B3_SetPitchSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_B7_SlidePitchSlot3(AkaoQueuedCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]); }

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
    if (steps) {
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
    if (steps) {
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
    if (steps) {
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
    if (steps) {
        effectiveSteps = steps;
    }
    delta = (cmd->target << 0x10) - startVal;
    g_AkaoPitchMulMusicSlideSteps = effectiveSteps;
    g_AkaoPitchMulMusicSlideStep = delta / effectiveSteps;
}

static void AkaoCmd_F0_StopMusic(void) { AkaoMusicStopChannels12(); }

static void AkaoCmd_F1_StopAllSounds(void) { AkaoSoundChannelsStop(); }

static void AkaoCmd_80_SetStereoMode(AkaoQueuedCommand* cmd) {
    g_AkaoBgmLanes[0].stereoMono = AKAO_STEREO;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

void AkaoCmd_82_ResetVolume(AkaoQueuedCommand* cmd) {
    g_AkaoBgmLanes[0].stereoMono = AKAO_STEREO_CHANNELS;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

static void AkaoCmd_81_SetMonoMode(AkaoQueuedCommand* cmd) {
    g_AkaoBgmLanes[0].stereoMono = AKAO_MONO;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

void AkaoCmd_90_SetMuteMusicMask(AkaoQueuedCommand* cmd) {
    AkaoChannel* channel;
    u16 i;

    g_AkaoMuteMusicMask = cmd->param0;
    for (i = 0, channel = g_Channel1; i < AKAO_NUM_VOICES; i++, channel++) {
        channel->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
    }
}

void AkaoCmd_92_SetCondition(AkaoQueuedCommand* cmd) { g_AkaoBgmLanes[0].condition = cmd->param0; }

// Moves newly-requested channels_1 voices into the active mask, resetting
// each one's SPU attributes.
void AkaoCmd_9B_ApplyPendingMusicUpdates(AkaoQueuedCommand* cmd) {
    s32 savedMask;
    s32 bit;
    s32 pendingBits;
    s32 voiceIdx;

    if (g_AkaoBgmLanes->activeMask) {
        pendingBits = (g_AkaoBgmLanes->activeMask | g_AkaoBgmLanes->overMask | g_AkaoBgmLanes->altMask) &
                      ~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask);
        if (pendingBits) {
            bit = 1;
            voiceIdx = 0;
            g_AkaoVoiceAttr->vol_r = 0;
            g_AkaoVoiceAttr->vol_l = 0;
            g_AkaoVoiceAttr->sr = AKAO_VOL_MAX;
            for (; pendingBits != 0; bit <<= 1, voiceIdx += 1) {
                if (pendingBits & bit) {
                    g_AkaoVoiceAttr->mask = AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, g_AkaoVoiceAttr);
                    pendingBits ^= bit;
                }
            }
        }
        savedMask = g_AkaoBgmLanes->activeMask;
        g_AkaoBgmLanes->activeMask = 0;
        g_AkaoBgmLanes->activeMaskStored = savedMask;
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

    pendingBits = g_AkaoBgmLanes->activeMaskStored;
    if (pendingBits) {
        bit = 1;
        voice = g_Channel1;
        do {
            if (pendingBits & bit) {
                pendingBits ^= bit;
                voice->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
            bit <<= 1;
            voice++;
            stillPending = pendingBits;
        } while (stillPending);
        savedMask = g_AkaoBgmLanes->activeMaskStored;
        g_AkaoBgmLanes->activeMaskStored = 0;
        g_AkaoBgmLanes->activeMask = savedMask;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~AKAO_CONTROL_PAUSE_MUSIC_UPDATE;
}

void AkaoCmd_9D_ApplyPendingSfxUpdates(void) {
    s32 savedMask;
    short cleared;
    s32 newMask;
    s32 bit;
    s32 voiceIdx;

    newMask = g_AkaoSfxLanes->activeMask;
    savedMask = newMask;
    if (newMask) {
        bit = 0x10000;
        if (g_AkaoSoundSlots[3].voices[0].playingType == AKAO_MENU) {
            newMask &= ~((1 << 22) | (1 << 23));
        }
        g_AkaoSfxLanes->activeMaskStored = newMask;
        // FAKE MATCH
        *(&g_AkaoSfxLanes->activeMask + (cleared = 0)) = newMask ^ savedMask;
        g_AkaoVoiceAttr->vol_r = cleared;
        g_AkaoVoiceAttr->vol_l = cleared;
        g_AkaoVoiceAttr->sr = AKAO_VOL_MAX;
        voiceIdx = 0x10;
        if (newMask != cleared) {
            for (; newMask != 0; bit <<= 1, voiceIdx += 1) {
                if (newMask & bit) {
                    g_AkaoVoiceAttr->mask = AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
                    AkaoUpdateChannelParamsToSpu(voiceIdx & 0xFFFF, g_AkaoVoiceAttr);
                    newMask ^= bit;
                }
            }
        }
    }
    g_AkaoControlFlags |= AKAO_CONTROL_PAUSE_UPDATE;
}

// channels_3 counterpart to AkaoCmd_9A_FlushPendingMusicUpdates.
void AkaoCmd_9C_FlushPendingSfxUpdates(void) {
    AkaoChannel* half;
    s32 savedMask;
    s32 bit;
    s32 pendingBits;

    pendingBits = g_AkaoSfxLanes->activeMaskStored;
    if (pendingBits) {
        for (bit = 0x10000, half = &g_AkaoSoundSlots[0].voices[0]; pendingBits != 0; bit <<= 1, half++) {
            if (pendingBits & bit) {
                pendingBits ^= bit;
                half->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
        }
        savedMask = g_AkaoSfxLanes->activeMaskStored;
        g_AkaoSfxLanes->activeMaskStored = 0;
        g_AkaoSfxLanes->activeMask = savedMask;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~AKAO_CONTROL_PAUSE_UPDATE;
}

static void AkaoCmd_E0_SetReverbPan(AkaoSetReverbPan* cmd) {
    g_AkaoReverbPan = cmd->pan & AKAO_PAN_MAX;
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_REVERB;
}

static void AkaoCmd_E4_SetReverbMul(AkaoSetReverbMul* cmd) {
    u8 mul;
    s32 flags;
    s32 mask;

    mul = cmd->mul;
    g_AkaoReverbMul = (s16)mul;
    mask = ~AKAO_CONTROL_REVERB_ENABLE;
    if (mul) {
        flags = g_AkaoControlFlags | AKAO_CONTROL_REVERB_ENABLE;
    } else {
        flags = g_AkaoControlFlags & mask;
    }
    g_AkaoControlFlags = flags;
    AkaoUpdateReverbVoices();
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_REVERB;
}

static void AkaoCmd_F2_ClearSavedMusic0(void) { g_AkaoPrevBgmLanes[0].musicId = 0; }

static void AkaoCmd_F3_ClearSavedMusic1(void) { g_AkaoPrevBgmLanes[1].musicId = 0; }

void AkaoCmd_F4_SaveState(AkaoQueuedCommand* cmd) {
    u32* src;
    u32* dst;
    AkaoSoundConfig* savedLane;
    u16 i;

    AkaoSoundSyncKeyStatus();
    src = (u32*)g_AkaoSoundSlots;
    dst = (u32*)((AkaoChannel*)src - AKAO_NUM_VOICES);
    i = sizeof(AkaoSoundSlot) * 4 / sizeof(u32);
    do {
        i--;
        *dst++ = *src++;
    } while (i);
    i = sizeof(AkaoSoundConfig) / sizeof(u32);
    src = (u32*)g_AkaoSfxLanes;
    savedLane = (AkaoSoundConfig*)dst;
    do {
        i--;
        *dst++ = *src++;
    } while (i);
    if (g_AkaoSoundSlots[3].voices[0].playingType == AKAO_MENU) {
        savedLane->activeMask &= ~0xC00000;
        savedLane->onMask &= ~0xC00000;
    }
    g_AkaoControlFlags |= AKAO_CONTROL_STATE_SAVED;
    AkaoSoundChannelsStop();
    cmd->param0 = AKAO_VOL_MAX;
    AkaoCmd_B8_SetAllVolBalance(cmd);
    cmd->param0 = 0;
    AkaoCmd_BC_SetAllPitch(cmd);
}

void AkaoCmd_F5_RestoreState(AkaoQueuedCommand* cmd) {
    u32* src;
    u32* dst;
    u32 active;
    u16 i;

    if (g_AkaoControlFlags & AKAO_CONTROL_STATE_SAVED) {
        src = (u32*)g_Channel2;
        dst = (u32*)((AkaoChannel*)src + AKAO_NUM_VOICES);
        i = sizeof(AkaoSoundSlot) * 4 / sizeof(u32);
        do {
            i--;
            *dst++ = *src++;
        } while (i);
        active = g_AkaoSfxLanes[0].activeMask;
        i = sizeof(AkaoSoundConfig) / sizeof(u32);
        dst = (u32*)g_AkaoSfxLanes;
        do {
            i--;
            *dst++ = *src++;
        } while (i);
        g_AkaoSfxLanes->offMask = active & ~g_AkaoSfxLanes->activeMask;
        g_AkaoControlFlags &= ~AKAO_CONTROL_STATE_SAVED;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
        g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    }
}

static void AkaoCmd_F8_StreamReverbMaskClear(AkaoQueuedCommand* cmd) {
    u32* addr;
    s32 reverbMask;
    s32 invStreamMask;

    AkaoStreamInit(cmd);
    addr = &g_AkaoSfxLanes->activeMask;
    reverbMask = g_AkaoSfxLanes->reverbMask;
    invStreamMask = ~g_AkaoStreamMask;
    *addr &= invStreamMask;
    g_AkaoSfxLanes->reverbMask = invStreamMask & reverbMask;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_F9_StreamReverbMaskRestore(AkaoQueuedCommand* cmd) {
    s32 activeMask;

    AkaoStreamInit(cmd);
    activeMask = g_AkaoSfxLanes->activeMask;
    g_AkaoSfxLanes->activeMask = ~g_AkaoStreamMask & activeMask;
    g_AkaoSfxLanes->reverbMask |= g_AkaoStreamMask;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_FA_StopStream(void) { AkaoStreamStop(); }

void AkaoCmd_Null(AkaoQueuedCommand* cmd) {}

static void AkaoClearTransferCallback(void) { SpuSetTransferCallback(0); }

static void AkaoStreamVoiceAttrMono(void);
static void AkaoStreamVoiceAttrSplit(void);
static void AkaoStreamTransferCallbackMono(void);
static void AkaoStreamTransferCallbackSplit(void);

void AkaoStreamInit(AkaoQueuedCommand* cmd) {
    u32 flags;
    u32 loopOffset;

    g_AkaoSfxLanes->offMask |= g_AkaoStreamMask;
    SpuSetTransferCallback(NULL);
    SpuSetIRQ(SPU_OFF);
    SpuSetIRQCallback(NULL);
    g_AkaoStreamSrc = (u8*)cmd->param0;
    g_AkaoStreamPan = cmd->param1;
    g_AkaoStreamVol = cmd->param2 << 7;
    g_AkaoStreamRemainingBytes = *(u32*)g_AkaoStreamSrc;
    if (g_AkaoStreamRemainingBytes) {
        g_AkaoStreamSrc += 4;
        flags = *(u32*)g_AkaoStreamSrc;
        g_AkaoStreamSrc += 4;
        *(u32*)&g_AkaoStreamFormat = flags;
        loopOffset = *(u32*)g_AkaoStreamSrc;
        g_AkaoStreamSrc += 8;
        if (flags & 2) {
            g_AkaoStreamLoopSrc = g_AkaoStreamSrc + loopOffset;
        } else {
            g_AkaoStreamLoopSrc = NULL;
        }
        if (flags & 2) {
            g_AkaoStreamLoopSize = g_AkaoStreamRemainingBytes - loopOffset;
        } else {
            g_AkaoStreamLoopSize = 0;
        }
        if (flags & 1) {
            AkaoStreamVoiceAttrSplit();
            SpuSetTransferCallback(AkaoStreamTransferCallbackSplit);
            g_AkaoStreamMask = 0x30000;
        } else {
            AkaoStreamVoiceAttrMono();
            SpuSetTransferCallback(AkaoStreamTransferCallbackMono);
            g_AkaoStreamMask = 0x10000;
        }
        SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
        SpuSetTransferStartAddr(0x77000);
        SpuWrite(g_AkaoStreamSrc, 0x2000);
        if (g_AkaoStreamRemainingBytes > 0x2000) {
            g_AkaoStreamRemainingBytes -= 0x2000;
            g_AkaoStreamSrc += 0x2000;
        } else {
            g_AkaoStreamRemainingBytes = 0;
        }
    }
    g_AkaoSfxLanes->pitchLfoMask &= ~g_AkaoStreamMask;
    g_AkaoSfxLanes->noiseMask &= ~g_AkaoStreamMask;
    AkaoUpdatePitchLfoVoices();
    AkaoUpdateNoiseVoices();
    AkaoSoundChannelsClear(0, 1);
}

// Configures the voice-attribute block for a mono CD-stream voice (ADSR
// envelope, pan, reverb-echo work area) and applies it via AkaoUpdateChannelParamsToSpu.
//
// NOTE: g_AkaoVoiceAttr is an array so fields can be reached with `->`, which gives one lui/sw per access.
// `g_AkaoVoiceAttr[0].field` keeps the address in a register instead and does not match here.
static void AkaoStreamVoiceAttrMono(void) {
    g_AkaoVoiceAttr->mask = 0x1FF93;
    g_AkaoVoiceAttr->ar = 0;
    g_AkaoVoiceAttr->addr = 0x77000;
    g_AkaoVoiceAttr->loop_addr = 0x77000;
    g_AkaoVoiceAttr->dr = 0xF;
    g_AkaoVoiceAttr->sl = 0xF;
    g_AkaoVoiceAttr->sr = 0x7F;
    g_AkaoVoiceAttr->rr = 6;
    g_AkaoVoiceAttr->a_mode = 1;
    g_AkaoVoiceAttr->s_mode = 3;
    g_AkaoVoiceAttr->r_mode = 3;
    g_AkaoVoiceAttr->vol_l = (g_AkaoStreamPan ^ AKAO_PAN_MAX) * g_AkaoStreamVol >> 7;
    g_AkaoVoiceAttr->pitch = g_AkaoStreamFormat.pitch;
    g_AkaoVoiceAttr->vol_r = g_AkaoStreamVol * g_AkaoStreamPan >> 7;
    AkaoUpdateChannelParamsToSpu(0x10, g_AkaoVoiceAttr);
}

static void AkaoStreamVoiceAttrSplit(void) {
    g_AkaoVoiceAttr->mask = 0x1FF93;
    g_AkaoVoiceAttr->addr = 0x77000;
    g_AkaoVoiceAttr->loop_addr = 0x78000;
    g_AkaoVoiceAttr->dr = 0xF;
    g_AkaoVoiceAttr->sl = 0xF;
    g_AkaoVoiceAttr->sr = 0x7F;
    g_AkaoVoiceAttr->rr = 6;
    g_AkaoVoiceAttr->a_mode = 1;
    g_AkaoVoiceAttr->s_mode = 3;
    g_AkaoVoiceAttr->r_mode = 3;
    g_AkaoVoiceAttr->vol_r = 0;
    g_AkaoVoiceAttr->ar = 0;
    g_AkaoVoiceAttr->vol_l = g_AkaoStreamVol >> 1;
    g_AkaoVoiceAttr->pitch = g_AkaoStreamFormat.pitch;
    AkaoUpdateChannelParamsToSpu(0x10, g_AkaoVoiceAttr);
    g_AkaoVoiceAttr->mask = 0x1FF93;
    g_AkaoVoiceAttr->vol_l = 0;
    g_AkaoVoiceAttr->addr = 0x77800;
    g_AkaoVoiceAttr->loop_addr = 0x78800;
    g_AkaoVoiceAttr->vol_r = g_AkaoStreamVol >> 1;
    AkaoUpdateChannelParamsToSpu(0x11, g_AkaoVoiceAttr);
}

static void AkaoStreamIrqCallbackMono0(void);

// CD-stream DMA transfer-complete callback (mono case). Keys on the stream
// voice(s) in g_AkaoStreamMask; when g_AkaoStreamRemainingBytes (bytes remaining) is nonzero, first
// re-arms the SPU transfer IRQ with AkaoStreamIrqCallbackMono0 to continue streaming.
static void AkaoStreamTransferCallbackMono(void) {
    SpuSetTransferCallback(0);
    if (g_AkaoStreamRemainingBytes) {
        SpuSetIRQ(0);
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackMono0);
        SpuSetIRQ(1);
    }
    SpuSetKey(1, g_AkaoStreamMask);
    g_AkaoSfxLanes->offMask &= ~g_AkaoStreamMask;
}

static void AkaoStreamIrqCallbackSplit0(void);

// CD-stream DMA transfer-complete callback (split/stereo case). Twin of
// AkaoStreamTransferCallbackMono above, using a different IRQ callback.
static void AkaoStreamTransferCallbackSplit(void) {
    SpuSetTransferCallback(0);
    if (g_AkaoStreamRemainingBytes) {
        SpuSetIRQ(0);
        SpuSetIRQAddr(0x78000);
        SpuSetIRQCallback(AkaoStreamIrqCallbackSplit0);
        SpuSetIRQ(1);
    }
    SpuSetKey(1, g_AkaoStreamMask);
    g_AkaoSfxLanes->offMask &= ~g_AkaoStreamMask;
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
    if (g_AkaoStreamLoopSrc) {
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
    if (g_AkaoStreamLoopSrc) {
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
    } else if (g_AkaoStreamLoopSrc) {
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
    } else if (g_AkaoStreamLoopSrc) {
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
        data = (u8*)(u_long)(u32)g_AkaoCmd.params[0];
        if (data[0] == 'A' && data[1] == 'K' && data[2] == 'A' && data[3] == 'O') {
            data += 4;
            musicId = *(u16*)data;
            data += 2;
            dataSize = *(u16*)data;
            data += 2;
            reverbMode = *(u16*)data;
            data += 8;
            if (g_AkaoBgmLanes[0].musicId != musicId) {
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
        command->opcode = AKAO_APPLY_PENDING_SFX_UPDATES;
        break;
    case AKAO_FLUSH_ALL_PENDING_UPDATES:
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_FLUSH_PENDING_MUSIC_UPDATES;
        AkaoGetCommandQueue(&command);
        command->opcode = AKAO_FLUSH_PENDING_SFX_UPDATES;
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

s32 AkaoDispatchCommand(AkaoQueuedCommand* cmd) {
    switch (cmd->opcode) {
    case AKAO_PLAY_MUSIC:
    case AKAO_PLAY_MUSIC_SAVE_CURR:
    case AKAO_PLAY_MUSIC_SWAP_SAVED:
    case AKAO_FADE_PLAY_MUSIC:
    case AKAO_FADE_PLAY_MUSIC_SAVE_CURR:
        cmd->opcode = 0;
        break;
    case AKAO_PLAY_ONE_CONSECUTIVE_SOUND:
        cmd->opcode = AKAO_PLAY_SOUND;
        break;
    case AKAO_PLAY_TWO_CONSECUTIVE_SOUNDS:
        cmd->opcode = AKAO_PLAY_TWO_SOUNDS;
        cmd->param2 = cmd->param1 + 1;
        break;
    case AKAO_PLAY_THREE_CONSECUTIVE_SOUNDS:
        cmd->opcode = AKAO_PLAY_THREE_SOUNDS;
        cmd->param2 = cmd->param1 + 1;
        cmd->param3 = cmd->param1 + 2;
        break;
    case AKAO_PLAY_FOUR_CONSECUTIVE_SOUNDS:
        cmd->opcode = AKAO_PLAY_FOUR_SOUNDS;
        cmd->param2 = cmd->param1 + 1;
        cmd->param3 = cmd->param1 + 2;
        cmd->param4 = cmd->param1 + 3;
        break;
    case AKAO_SET_TEMPO_AND_PITCH:
        g_AkaoCommandHandler[AKAO_SET_TEMPO](cmd);
        cmd->opcode = AKAO_SET_PITCH;
        break;
    case AKAO_TEMPO_AND_PITCH_SLIDE_FROM_CURR:
        g_AkaoCommandHandler[AKAO_TEMPO_SLIDE_FROM_CURR](cmd);
        cmd->opcode = AKAO_PITCH_SLIDE_FROM_CURR;
        break;
    case AKAO_TEMPO_AND_PITCH_SLIDE_BETWEEN_TARGETS:
        g_AkaoCommandHandler[AKAO_TEMPO_SLIDE_BETWEEN_TARGETS](cmd);
        cmd->opcode = AKAO_PITCH_SLIDE_BETWEEN_TARGETS;
        break;
    case AKAO_APPLY_ALL_PENDING_UPDATES:
        g_AkaoCommandHandler[AKAO_APPLY_PENDING_MUSIC_UPDATES](cmd);
        cmd->opcode = AKAO_APPLY_PENDING_SFX_UPDATES;
        break;
    case AKAO_FLUSH_ALL_PENDING_UPDATES:
        g_AkaoCommandHandler[AKAO_FLUSH_PENDING_MUSIC_UPDATES](cmd);
        cmd->opcode = AKAO_FLUSH_PENDING_SFX_UPDATES;
        break;
    }
    g_AkaoCommandHandler[(u8)cmd->opcode](cmd);
    return 0;
}

static void AkaoExecuteCommandsQueue(void) {
    AkaoQueuedCommand* cmd;

    if (g_AkaoMutex == 0) {
        for (cmd = g_AkaoCommandQueue; g_AkaoCommandQueueId; g_AkaoCommandQueueId--, cmd++) {
            ((void (*)(AkaoQueuedCommand*))g_AkaoCommandHandler[(u8)cmd->opcode])(cmd);
        }
    }
}

void AkaoUpdateChannelParamsToSpu(s32 voiceIdx, AkaoVoiceAttr* attr) {
    if (attr->mask & SPU_VOICE_PITCH) {
        SpuSetVoicePitch(voiceIdx, attr->pitch);
        attr->mask &= ~SPU_VOICE_PITCH;
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & AKAO_UPDATE_SPU_VOICE) {
        SpuSetVoiceVolume(voiceIdx, attr->vol_l, attr->vol_r);
        attr->mask &= ~AKAO_UPDATE_SPU_VOICE;
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & SPU_VOICE_WDSA) {
        SpuSetVoiceStartAddr(voiceIdx, attr->addr);
        attr->mask &= ~SPU_VOICE_WDSA;
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & SPU_VOICE_LSAX) {
        SpuSetVoiceLoopStartAddr(voiceIdx, attr->loop_addr);
        attr->mask &= ~SPU_VOICE_LSAX;
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & (SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR)) {
        SpuSetVoiceSRAttr(voiceIdx, attr->sr, attr->s_mode);
        attr->mask &= ~(SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR);
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_AR)) {
        SpuSetVoiceARAttr(voiceIdx, attr->ar, attr->a_mode);
        attr->mask &= ~(SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_AR);
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & (SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR)) {
        SpuSetVoiceRRAttr(voiceIdx, attr->rr, attr->r_mode);
        attr->mask &= ~(SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR);
        if (attr->mask == 0) {
            return;
        }
    }
    if (attr->mask & (SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SL)) {
        SpuSetVoiceDR(voiceIdx, attr->dr);
        SpuSetVoiceSL(voiceIdx, attr->sl);
        attr->mask &= ~(SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SL);
    }
}

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

void AkaoMusicUpdatePitchAndVol(AkaoChannel* channel, u32 mask, u32 voice);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicUpdatePitchAndVol);

void AkaoSoundUpdatePitchAndVol(AkaoChannel* channel, u32 mask);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundUpdatePitchAndVol);

void AkaoUpdateChannelAndOvlParamsToSpu(AkaoChannel* channel, u32 mask, u32 voice) {
    AkaoChannel* overlay;
    s16 vol;
    u16 balance;

    vol = channel->voiceAttr.vol_l;
    balance = 0x7F - (channel->volBalance >> 8);
    channel->voiceAttr.vol_l = (vol * balance) >> 8;
    overlay = &g_Channel1[channel->overlayChannelId];
    overlay->voiceAttr.vol_l = (vol * channel->volBalance) >> 16;
    vol = channel->voiceAttr.vol_r;
    channel->voiceAttr.vol_r = (vol * balance) >> 8;
    overlay->voiceAttr.vol_r = (vol * channel->volBalance) >> 16;
    overlay->voiceAttr.pitch = channel->voiceAttr.pitch;
    overlay->voiceAttr.mask |= channel->voiceAttr.mask;
    AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
    if (mask & (1 << voice)) {
        AkaoUpdateChannelParamsToSpu(voice, &overlay->voiceAttr);
    }
}

void AkaoUpdateKeysOn(void) {
    AkaoChannel* channel;
    s32 bit;
    u16 voice;
    u32 free;
    u32 active;
    u32 keyOn;
    s32 depth;
    u32 flags;
    u32 alt;

    keyOn = 0;
    if (g_AkaoBgmLanes->updateFlags & AKAO_UPDATE_REVERB) {
        g_ReverbAttr.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        depth = (s16)FIXED_HI(g_AkaoBgmLanes->reverbDepth);
        if (g_AkaoReverbMul < 0x80) {
            depth += (depth * g_AkaoReverbMul) >> 7;
        } else {
            depth = (depth * g_AkaoReverbMul) >> 8;
        }
        if (g_AkaoReverbPan < 0x40) {
            g_ReverbAttr.depth.left = depth;
            g_ReverbAttr.depth.right = depth - ((depth * (g_AkaoReverbPan ^ 0x3F)) >> 6);
        } else {
            g_ReverbAttr.depth.right = depth;
            g_ReverbAttr.depth.left = depth - ((depth * (g_AkaoReverbPan & 0x3F)) >> 6);
        }
        SpuSetReverbDepth(&g_ReverbAttr);
        g_AkaoBgmLanes->updateFlags ^= AKAO_UPDATE_REVERB;
    }
    if (g_AkaoBgmLanes->updateFlags & AKAO_UPDATE_NOISE_CLOCK) {
        SpuSetNoiseClock(g_AkaoSfxLanes->activeMask != 0 ? g_AkaoSfxLanes->noiseClock : g_AkaoBgmLanes[0].noiseClock);
        g_AkaoBgmLanes->updateFlags ^= AKAO_UPDATE_NOISE_CLOCK;
    }
    active = g_AkaoBgmLanes[1].activeMask;
    if (active) {
        bit = 1;
        voice = 0;
        channel = g_Channel2;
        free = ~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask) & g_Channel2VoiceMask;
        active &= free & g_AkaoBgmLanes[1].keyedMask;
        keyOn = free & g_AkaoBgmLanes[1].onMask;
        do {
            if (active & bit) {
                AkaoMusicUpdatePitchAndVol(channel, bit, voice);
                if (channel->voiceAttr.mask) {
                    flags = channel->updateFlags;
                    if (flags & AKAO_UPDATE_OVERLAY) {
                        AkaoUpdateChannelAndOvlParamsToSpu(channel, free, channel->overlayChannelId - AKAO_NUM_VOICES);
                    } else if (flags & AKAO_UPDATE_ALTERNATIVE) {
                        if (bit & g_AkaoBgmLanes[1].onMask) {
                            channel->updateFlags = flags ^ 0x400;
                            channel->voiceAttr.mask |= 0x1FF93;
                        }
                        if (channel->updateFlags & 0x400) {
                            alt = channel->alternativeChannelId;
                            if (free & (1 << alt)) {
                                AkaoUpdateChannelParamsToSpu(alt, &channel->voiceAttr);
                                if (keyOn & bit) {
                                    keyOn |= 1 << channel->alternativeChannelId;
                                    keyOn &= ~bit;
                                }
                            }
                        } else {
                            AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
                        }
                    } else {
                        AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
                    }
                }
                active ^= bit;
            }
            bit <<= 1;
            channel++;
            voice++;
        } while (active);
        g_AkaoBgmLanes[1].onMask = 0;
    }
    active = g_AkaoBgmLanes->activeMask;
    if (active) {
        bit = 1;
        voice = 0;
        channel = g_Channel1;
        free = ~(g_Channel2VoiceMask | g_AkaoSfxLanes->activeMask | g_AkaoStreamMask);
        active &= free & g_AkaoBgmLanes->keyedMask;
        keyOn |= free & g_AkaoBgmLanes->onMask;
        do {
            if (active & bit) {
                AkaoMusicUpdatePitchAndVol(channel, bit, voice);
                if (channel->voiceAttr.mask) {
                    if (bit & g_AkaoMuteMusicMask) {
                        channel->voiceAttr.vol_r = 0;
                        channel->voiceAttr.vol_l = 0;
                    }
                    flags = channel->updateFlags;
                    if (flags & AKAO_UPDATE_OVERLAY) {
                        AkaoUpdateChannelAndOvlParamsToSpu(channel, free, channel->overlayChannelId);
                    } else if (flags & AKAO_UPDATE_ALTERNATIVE) {
                        if (bit & g_AkaoBgmLanes->onMask) {
                            channel->updateFlags = flags ^ 0x400;
                            channel->voiceAttr.mask |= 0x1FF93;
                        }
                        if ((channel->updateFlags & 0x400) && (free & (1 << (alt = channel->alternativeChannelId)))) {
                            AkaoUpdateChannelParamsToSpu(alt, &channel->voiceAttr);
                            if (keyOn & bit) {
                                keyOn |= 1 << channel->alternativeChannelId;
                                keyOn &= ~bit;
                            }
                        } else {
                            AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
                        }
                    } else {
                        AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
                    }
                }
                active ^= bit;
            }
            bit <<= 1;
            channel++;
            voice++;
        } while (active);
        g_AkaoBgmLanes->onMask = 0;
    }
    active = g_AkaoSfxLanes->activeMask;
    if (active) {
        bit = 0x10000;
        channel = g_AkaoSoundSlots[0].voices;
        keyOn |= g_AkaoSfxLanes->onMask;
        active &= g_AkaoSfxLanes->keyedMask;
        do {
            if (active & bit) {
                AkaoSoundUpdatePitchAndVol(channel, bit);
                active ^= bit;
                if (channel->voiceAttr.mask) {
                    AkaoUpdateChannelParamsToSpu(channel->voiceAttr.voice_id, &channel->voiceAttr);
                }
            }
            bit <<= 1;
            channel++;
        } while (active);
        g_AkaoSfxLanes->onMask = 0;
    }
    if (keyOn) {
        SpuSetKey(SPU_ON, keyOn);
    }
}

void AkaoCollectChannelsVoicesMask(AkaoChannel* channel, u32* outMask, u32 active, u32 filter) {
    u32 bit;
    u32 voice;
    u16 id;

    bit = 1;
    *outMask |= active;
    while (active) {
        if (active & bit) {
            if (channel->updateFlags & AKAO_UPDATE_OVERLAY) {
                id = channel->overlayChannelId;
                if (channel->overlayChannelId >= AKAO_NUM_VOICES) {
                    id -= AKAO_NUM_VOICES;
                }
                voice = 1 << id;
                if (filter & voice) {
                    *outMask |= voice;
                }
            } else if (channel->updateFlags & AKAO_UPDATE_ALTERNATIVE) {
                voice = 1 << channel->alternativeChannelId;
                if (filter & voice) {
                    *outMask |= voice;
                }
            }
            active ^= bit;
        }
        bit <<= 1;
        channel++;
    }
}

void AkaoUpdateKeysOff(void) {
    u32 voices;
    u32 free;
    u32 active;

    voices = 0;
    if (g_AkaoBgmLanes[1].activeMask) {
        active = g_AkaoBgmLanes[1].offMask & g_Channel2VoiceMask & ~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask);
        if (active) {
            AkaoCollectChannelsVoicesMask(
                g_Channel2, &voices, active, ~(g_AkaoSfxLanes->activeMask | g_AkaoStreamMask));
        }
        g_AkaoBgmLanes[1].offMask = 0;
    }
    if (g_AkaoBgmLanes->activeMask) {
        free = ~(g_Channel2VoiceMask | g_AkaoSfxLanes->activeMask | g_AkaoStreamMask);
        active = free & g_AkaoBgmLanes->offMask;
        if (active) {
            AkaoCollectChannelsVoicesMask(g_Channel1, &voices, active, free);
        }
        g_AkaoBgmLanes->offMask = 0;
    }
    voices |= g_AkaoSfxLanes->offMask;
    g_AkaoSfxLanes->offMask = 0;
    if (voices) {
        SpuSetKey(SPU_OFF, voices);
    }
}

void AkaoUpdateNoiseVoices(void) {
    u32 voices;
    u32 free;
    u32 active;

    voices = 0;
    active = g_AkaoBgmLanes[1].noiseMask & g_Channel2VoiceMask & ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
    if (active) {
        AkaoCollectChannelsVoicesMask(g_Channel2, &voices, active, ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask));
    }
    free = ~(g_Channel2VoiceMask | g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
    active = free & g_AkaoBgmLanes->noiseMask;
    if (active) {
        AkaoCollectChannelsVoicesMask(g_Channel1, &voices, active, free);
    }
    voices |= g_AkaoSfxLanes->noiseMask;
    SpuSetNoiseVoice(SPU_ON, voices);
    voices ^= 0xFFFFFF;
    SpuSetNoiseVoice(SPU_OFF, voices);
}

void AkaoUpdateReverbVoices(void) {
    u32 voices;
    u32 free;
    u32 active;

    voices = 0;
    active = g_AkaoBgmLanes[1].reverbMask & g_Channel2VoiceMask & ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
    if (active) {
        AkaoCollectChannelsVoicesMask(g_Channel2, &voices, active, ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask));
    }
    if (g_AkaoControlFlags & AKAO_CONTROL_REVERB_ENABLE) {
        voices = 0xFFFFFF;
    } else {
        free = ~(g_Channel2VoiceMask | g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
        active = free & g_AkaoBgmLanes->reverbMask;
        if (active) {
            AkaoCollectChannelsVoicesMask(g_Channel1, &voices, active, free);
        }
    }
    voices |= g_AkaoSfxLanes->reverbMask;
    SpuSetReverbVoice(SPU_ON, voices);
    voices ^= 0xFFFFFF;
    SpuSetReverbVoice(SPU_OFF, voices);
}

void AkaoUpdatePitchLfoVoices(void) {
    u32 voices;
    u32 free;
    u32 active;

    voices = 0;
    active = g_AkaoBgmLanes[1].pitchLfoMask & g_Channel2VoiceMask & ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
    if (active) {
        AkaoCollectChannelsVoicesMask(g_Channel2, &voices, active, ~(g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask));
    }
    free = ~(g_Channel2VoiceMask | g_AkaoSfxLanes[0].activeMask | g_AkaoStreamMask);
    active = free & g_AkaoBgmLanes->pitchLfoMask;
    if (active) {
        AkaoCollectChannelsVoicesMask(g_Channel1, &voices, active, free);
    }
    voices |= g_AkaoSfxLanes->pitchLfoMask;
    SpuSetPitchLFOVoice(SPU_ON, voices);
    voices ^= 0xFFFFFF;
    SpuSetPitchLFOVoice(SPU_OFF, voices);
}

void AkaoMainUpdate(void);
extern s32 g_AkaoFrameTimeTotal;

// Root counter 2 interrupt handler: runs one sequencer update per elapsed tick (twice as many in double speed mode).
long AkaoMain(void) {
    s32 rcnt;
    u32 vsync;
    u16 ticks;

    rcnt = GetRCnt(RCntCNT2);
    vsync = VSync(1);
    if (vsync < g_AkaoLastHcount) {
        g_AkaoLastHcount = 0;
    }
    ticks = (vsync - g_AkaoLastHcount) / 66;
    if (ticks == 0 || ticks > 8) {
        ticks = 1;
    }
    g_AkaoLastHcount = vsync;
    vsync = ticks;
    if (g_AkaoControlFlags & AKAO_CONTROL_DOUBLE_SPEED) {
        ticks <<= 1;
    }
    while (ticks) {
        ticks--;
        AkaoMainUpdate();
    }
    rcnt = GetRCnt(RCntCNT2) - rcnt;
    if (rcnt <= 0) {
        rcnt += 0x43D1;
    }
    g_AkaoFrameTimeHistory[0] = g_AkaoFrameTimeHistory[1];
    g_AkaoFrameTimeHistory[1] = g_AkaoFrameTimeHistory[2];
    g_AkaoFrameTimeHistory[2] = g_AkaoFrameTimeHistory[3];
    g_AkaoFrameTimeHistory[3] = rcnt;
    g_AkaoFrameTimeTotal = g_AkaoFrameTimeHistory[0] + g_AkaoFrameTimeHistory[1] + g_AkaoFrameTimeHistory[2] + rcnt;
    return vsync;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoUpdateGlobalSlides);

void AkaoExecuteSequence(AkaoChannel* channel, AkaoChannelConfig* config, u32 mask);
void AkaoMusicUpdateSlideAndDelay(AkaoChannel* channel, AkaoChannelConfig* config, u32 mask);
void AkaoSoundUpdateSlideAndDelay(AkaoChannel* channel, u32 mask);
void AkaoUpdateGlobalSlides(void);
void AkaoUpdateKeysOn(void);
void AkaoUpdateKeysOff(void);

// One sequencer tick: advances the music slots and the sound effect slots, then flushes queued commands and keys.
void AkaoMainUpdate(void) {
    AkaoChannel* channel;
    u32 active;
    u32 mask;
    u32 tempo;
    u32 mul;
    u16 length;

    AkaoUpdateKeysOn();

    active = g_AkaoBgmLanes->activeMask;
    if (active) {
        mul = FIXED_U8(g_AkaoTempoMulMusic);
        tempo = FIXED_HI(g_AkaoBgmLanes->tempo);
        if (mul) {
            if (mul < 0x80) {
                tempo += (tempo * mul) >> 7;
            } else {
                tempo = (tempo * mul) >> 8;
            }
        }
        g_AkaoBgmLanes->tempoUpdate += tempo;
        if ((g_AkaoBgmLanes->tempoUpdate & 0xFFFF0000) || (g_AkaoControlFlags & AKAO_CONTROL_DOUBLE_SPEED)) {
            mask = 1;
            channel = g_Channel1;
            g_AkaoBgmLanes->tempoUpdate &= 0xFFFF;
            g_AkaoMusicSlot = 0;
            do {
                if (active & mask) {
                    length = channel->length - 0x101;
                    channel->length = length;
                    if (!(length & 0xFF)) {
                        AkaoExecuteSequence(channel, g_AkaoBgmLanes, mask);
                    } else if (!(length & 0xFF00)) {
                        channel->length = length | 0x100;
                        g_AkaoBgmLanes->offMask |= mask;
                        g_AkaoBgmLanes->keyedMask &= ~mask;
                    }
                    AkaoMusicUpdateSlideAndDelay(channel, g_AkaoBgmLanes, mask);
                    active ^= mask;
                }
                channel++;
                mask <<= 1;
            } while (active);
            if (g_AkaoBgmLanes->tempoSlideSteps) {
                g_AkaoBgmLanes->tempoSlideSteps--;
                g_AkaoBgmLanes->tempo += g_AkaoBgmLanes[0].tempoSlideStep;
            }
            if (g_AkaoBgmLanes->reverbDepthSlideSteps) {
                g_AkaoBgmLanes->reverbDepthSlideSteps--;
                g_AkaoBgmLanes->reverbDepth += g_AkaoBgmLanes[0].reverbDepthSlideStep;
                g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_REVERB;
            }
            if (g_AkaoBgmLanes->timerLower) {
                if (++g_AkaoBgmLanes->timerLowerCur == g_AkaoBgmLanes->timerLower) {
                    g_AkaoBgmLanes->timerLowerCur = 0;
                    if (++g_AkaoBgmLanes->timerUpperCur == g_AkaoBgmLanes[0].timerUpper) {
                        g_AkaoBgmLanes->timerUpperCur = 0;
                        g_AkaoBgmLanes->timerTopCur++;
                    }
                }
            }
        }
    }

    active = g_AkaoBgmLanes[1].activeMask;
    if (active) {
        mul = FIXED_U8(g_AkaoTempoMulMusic);
        tempo = FIXED_HI(g_AkaoBgmLanes[1].tempo);
        if (mul) {
            if (mul < 0x80) {
                tempo += (tempo * mul) >> 7;
            } else {
                tempo = (tempo * mul) >> 8;
            }
        }
        g_AkaoBgmLanes[1].tempoUpdate += tempo;
        if ((g_AkaoBgmLanes[1].tempoUpdate & 0xFFFF0000) || (g_AkaoControlFlags & AKAO_CONTROL_DOUBLE_SPEED)) {
            g_AkaoBgmLanes[1].tempoUpdate &= 0xFFFF;
            g_AkaoMusicSlot = 1;
            mask = 1;
            channel = g_Channel2;
            do {
                if (active & mask) {
                    length = channel->length - 0x101;
                    channel->length = length;
                    if ((length & 0xFF) == 0) {
                        AkaoExecuteSequence(channel, &g_AkaoBgmLanes[1], mask);
                    } else if ((length & 0xFF00) == 0) {
                        channel->length = length | 0x100;
                        g_AkaoBgmLanes[1].offMask |= mask;
                        g_AkaoBgmLanes[1].keyedMask &= ~mask;
                    }
                    AkaoMusicUpdateSlideAndDelay(channel, &g_AkaoBgmLanes[1], mask);
                    active ^= mask;
                }
                channel++;
                mask <<= 1;
            } while (active);
            if (g_AkaoBgmLanes[1].tempoSlideSteps) {
                g_AkaoBgmLanes[1].tempoSlideSteps--;
                g_AkaoBgmLanes[1].tempo += g_AkaoBgmLanes[1].tempoSlideStep;
            }
        }
    }

    active = g_AkaoSfxLanes->activeMask;
    if (active) {
        g_AkaoSfxLanes->tempoUpdate += FIXED_HI(g_AkaoSfxLanes->tempo);
        if ((g_AkaoSfxLanes->tempoUpdate & 0xFFFF0000) || (g_AkaoControlFlags & AKAO_CONTROL_DOUBLE_SPEED)) {
            g_AkaoSfxLanes->tempoUpdate &= 0xFFFF;
            mask = 0x10000;
            channel = g_AkaoSoundSlots[0].voices;
            while (active) {
                if (active & mask) {
                    if (!(g_AkaoControlFlags & AKAO_CONTROL_PAUSE_UPDATE) || channel->playingType == AKAO_MENU) {
                        channel->setToMinusOne++;
                        length = channel->length - 0x101;
                        channel->length = length;
                        if ((length & 0xFF) == 0) {
                            AkaoExecuteSequence(channel, g_AkaoBgmLanes, mask);
                        } else if ((length & 0xFF00) == 0) {
                            channel->length = length | 0x100;
                            g_AkaoSfxLanes->offMask |= mask;
                            g_AkaoSfxLanes->keyedMask &= ~mask;
                        }
                        AkaoSoundUpdateSlideAndDelay(channel, mask);
                    }
                    active ^= mask;
                }
                channel++;
                mask <<= 1;
            }
        }
    }

    if (g_AkaoBgmLanes->muteMusic) {
        AkaoCmd_9B_ApplyPendingMusicUpdates((AkaoQueuedCommand*)&g_AkaoCmd);
        g_AkaoBgmLanes->muteMusic = 0;
    }
    AkaoExecuteCommandsQueue();
    AkaoUpdateGlobalSlides();
    AkaoUpdateKeysOff();
}

u8 AkaoGetNextNote(AkaoChannel* channel);
void AkaoExecuteSequence(AkaoChannel* channel, AkaoChannelConfig* config, u32 mask) {
    AkaoDrumKey* drum;
    u32 opcode;
    u32 value;
    u32 amplitude;
    u32 base;
    u32 target;
    u16 length;
    u16 octave;
    u16 id;
    u8 key;

    do {
        key = *channel->akaoSequencePointer++;
        opcode = key;
        if (opcode >= AKAO_OP_FINISH_CHANNEL) {
            g_AkaoOpcodeHandler[opcode - AKAO_OP_FINISH_CHANNEL](channel, config, mask);
        } else {
            break;
        }
    } while (opcode != AKAO_OP_FINISH_CHANNEL);
    if (opcode == AKAO_OP_FINISH_CHANNEL) {
        return;
    }

    value = AkaoGetNextNote(channel);
    if (channel->lengthFixed) {
        channel->length = (channel->lengthFixed << 8) + channel->lengthFixed;
    }
    length = channel->length;
    if (length & 0xFF) {
        if (value >= AKAO_OP_REST ||
            (value < AKAO_OP_TIE && !(channel->sfxMask & (AKAO_SFX_LEGATO | AKAO_SFX_FULL_LENGTH)))) {
            channel->length = length - 0x200;
        }
    } else {
        length = g_AkaoLengthTable[(u8)(opcode % 11)];
        if ((value < AKAO_OP_TIE || value >= AKAO_OP_REST) &&
            !(channel->sfxMask & (AKAO_SFX_LEGATO | AKAO_SFX_FULL_LENGTH))) {
            length -= 0x200;
        }
        channel->length = length;
    }
    channel->lengthStored = channel->length & 0xFF;

    if (key >= AKAO_OP_REST) {
        channel->portamentoSteps = 0;
        channel->vibratoPitch = 0;
        channel->tremoloVol = 0;
        channel->sfxMask &= ~AKAO_SFX_LEGATO_PREV;
        return;
    }
    if (key < AKAO_OP_TIE) {
        key /= 11;
        if (channel->updateFlags & AKAO_UPDATE_DRUM_MODE) {
            if (channel->playingType == AKAO_MUSIC) {
                config->onMask |= mask;
            } else {
                g_AkaoSfxLanes->onMask |= mask;
            }
            drum = (AkaoDrumKey*)channel->drumOffset;
            drum += (u8)(key % 12);
            if (drum->instrument != channel->currentInstrument) {
                channel->currentInstrument = drum->instrument;
                channel->voiceAttr.addr = g_AkaoInstrument[drum->instrument].addr;
                channel->voiceAttr.loop_addr = g_AkaoInstrument[drum->instrument].loopAddr;
                channel->voiceAttr.ar = g_AkaoInstrument[drum->instrument].ar;
                channel->voiceAttr.dr = g_AkaoInstrument[drum->instrument].dr;
                channel->voiceAttr.sl = g_AkaoInstrument[drum->instrument].sl;
                channel->voiceAttr.sr = g_AkaoInstrument[drum->instrument].sr;
                channel->voiceAttr.a_mode = g_AkaoInstrument[drum->instrument].aMode;
                channel->voiceAttr.s_mode = g_AkaoInstrument[drum->instrument].sMode;
                if (!(channel->updateFlags & AKAO_UPDATE_ALTERNATIVE)) {
                    channel->voiceAttr.rr = g_AkaoInstrument[drum->instrument].rr;
                    channel->voiceAttr.r_mode = g_AkaoInstrument[drum->instrument].rMode;
                    channel->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE;
                } else {
                    channel->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE_WOR;
                }
            }
            value = g_AkaoInstrument[drum->instrument].pitch[(u8)(drum->key % 12)];
            octave = (u8)(drum->key / 12);
            if (octave >= 7) {
                value <<= octave - 6;
            } else if (octave < 6) {
                value >>= 6 - octave;
            }
            channel->volumeLevel = (drum->volume[0] + (drum->volume[1] << 8)) << 16;
            channel->volPan = drum->pan << 8;
        } else {
            key += channel->octave * 12;
            if (channel->portamentoSteps && channel->keyStored) {
                target = key + channel->transpose;
                key = channel->keyStored + channel->transposeStored;
                channel->pitchSlideSteps = channel->portamentoSteps;
                channel->keyAdd = target - channel->keyStored - channel->transposeStored;
                channel->key = channel->keyStored - (channel->transpose - channel->transposeStored);
            } else {
                channel->key = key;
                key += channel->transpose;
            }
            octave = (u8)(key / 12);
            key %= 12;
            if (!(channel->sfxMask & AKAO_SFX_LEGATO_PREV)) {
                if (channel->playingType == AKAO_MUSIC) {
                    config->onMask |= mask;
                    if (channel->updateFlags & AKAO_UPDATE_OVERLAY) {
                        id = channel->overlayChannelId;
                        if (channel->overlayChannelId >= AKAO_NUM_VOICES) {
                            id -= AKAO_NUM_VOICES;
                        }
                        config->onMask |= 1 << id;
                    }
                } else {
                    g_AkaoSfxLanes->onMask |= mask;
                }
                channel->pitchSlideStepsCur = 0;
            }
            value = g_AkaoInstrument[channel->currentInstrument].pitch[key];
            if (octave >= 7) {
                value <<= octave - 6;
            } else if (octave < 6) {
                value >>= 6 - octave;
            }
        }
        if (channel->playingType == AKAO_MUSIC) {
            config->keyedMask |= mask;
        } else {
            g_AkaoSfxLanes->keyedMask |= mask;
        }
        channel->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE | SPU_VOICE_PITCH;
        if (channel->fineTuning) {
            if (channel->fineTuning > 0) {
                value += (value * channel->fineTuning) >> 7;
            } else {
                value += (value * channel->fineTuning) >> 8;
            }
            value &= 0xFFFF;
        }
        channel->basePitch = value;
        if (channel->updateFlags & AKAO_UPDATE_VIBRATO) {
            amplitude = (channel->vibratoDepth & 0x7F00) >> 8;
            if (channel->vibratoDepth & 0x8000) {
                base = (amplitude * value) >> 7;
            } else {
                base = (amplitude * ((value * 15) >> 8)) >> 7;
            }
            channel->vibratoBase = base;
            channel->vibratoWave = g_AkaoWaveTableKey[channel->vibratoType];
            channel->vibratoDelayCur = channel->vibratoDelay;
            channel->vibratoRateCur = 1;
        }
        if (channel->updateFlags & AKAO_UPDATE_TREMOLO) {
            channel->tremoloWave = g_AkaoWaveTableKey[channel->tremoloType];
            channel->tremoloDelayCur = channel->tremoloDelay;
            channel->tremoloRateCur = 1;
        }
        if (channel->updateFlags & AKAO_UPDATE_PAN_LFO) {
            channel->panLfoWave = g_AkaoWaveTableKey[channel->panLfoType];
            channel->panLfoRateCur = 1;
        }
        channel->vibratoPitch = 0;
        channel->tremoloVol = 0;
        channel->pitchSlide = 0;
    }

    channel->sfxMask = (channel->sfxMask & ~AKAO_SFX_LEGATO_PREV) | ((channel->sfxMask & AKAO_SFX_LEGATO) << 1);
    if (channel->keyAdd) {
        channel->key += channel->keyAdd;
        key = channel->key + channel->transpose;
        if (channel->playingType == AKAO_MUSIC) {
            value = g_AkaoInstrument[channel->currentInstrument].pitch[(u8)(key % 12)];
            if (channel->fineTuning) {
                if (channel->fineTuning > 0) {
                    value += (value * channel->fineTuning) >> 7;
                } else {
                    value += (value * channel->fineTuning) >> 8;
                }
                value &= 0xFFFF;
            }
            value <<= 16;
        } else {
            value = g_AkaoInstrument[channel->currentInstrument].pitch[(u8)(key % 12)] << 16;
        }
        key /= 12;
        if (key > 6) {
            value <<= key - 6;
        } else if (key < 6) {
            value >>= 6 - key;
        }
        channel->pitchSlideStepsCur = channel->pitchSlideSteps;
        channel->pitchSlideStep =
            (s32)(value - ((channel->basePitch << 16) + channel->pitchSlide)) / channel->pitchSlideStepsCur;
        channel->keyAdd = 0;
    }
    channel->keyStored = channel->key;
    channel->transposeStored = channel->transpose;
}

void AkaoInstrInit(AkaoChannel* channel, u16 instrument) {
    AkaoInstrument* instr;

    channel->currentInstrument = instrument;
    instr = &g_AkaoInstrument[instrument];
    channel->voiceAttr.addr = instr->addr;
    channel->voiceAttr.loop_addr = instr->loopAddr;
    channel->voiceAttr.a_mode = instr->aMode;
    channel->voiceAttr.s_mode = instr->sMode;
    channel->voiceAttr.r_mode = instr->rMode;
    channel->voiceAttr.ar = instr->ar;
    channel->voiceAttr.dr = instr->dr;
    channel->voiceAttr.sl = instr->sl;
    channel->voiceAttr.sr = instr->sr;
    channel->voiceAttr.rr = instr->rr;
    channel->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE;
}

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

void AkaoOp_E8_Tempo(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    config->tempo = *track->akaoSequencePointer++ << 0x10;
    config->tempo |= *track->akaoSequencePointer++ << 0x18;
    config->tempoSlideSteps = 0;
}

void AkaoOp_E9_TempoSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 steps;
    u8 lo;
    u8 hi;

    steps = *track->akaoSequencePointer++;
    config->tempoSlideSteps = steps;
    if (steps == 0) {
        config->tempoSlideSteps = 0x100;
    }
    lo = *track->akaoSequencePointer++;
    hi = *track->akaoSequencePointer++;
    config->tempoSlideStep = (((lo << 0x10) | (hi << 0x18)) - (config->tempo &= 0xFFFF0000)) / config->tempoSlideSteps;
}

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

void AkaoOp_EB_ReverbDepthSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 steps;
    u8 lo;
    u8 hi;

    steps = *track->akaoSequencePointer++;
    config->reverbDepthSlideSteps = steps;
    if (steps == 0) {
        config->reverbDepthSlideSteps = 0x100;
    }
    lo = *track->akaoSequencePointer++;
    hi = *track->akaoSequencePointer++;
    config->reverbDepthSlideStep =
        (((lo << 0x10) | (hi << 0x18)) - (config->reverbDepth &= 0xFFFF0000)) / config->reverbDepthSlideSteps;
}

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

void AkaoOp_A9_SetVolSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 steps;

    steps = *track->akaoSequencePointer++;
    track->volSlideSteps = steps;
    if (steps == 0) {
        track->volSlideSteps = 0x100;
    }
    track->volSlideStep =
        ((*(s8*)track->akaoSequencePointer++ << 0x17) - (track->volumeLevel &= 0xFFFF0000)) / track->volSlideSteps;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F4_OverlayVoiceOn);

void AkaoOp_F5_OverlayVoiceOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u16 channelId;

    channelId = track->overlayChannelId;
    if (g_AkaoMusicSlot) {
        channelId -= AKAO_NUM_VOICES;
    }
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        track->updateFlags &= ~AKAO_UPDATE_OVERLAY;
        config->overMask &= ~(1 << channelId);
    }
}

static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->volBalanceSlideSteps = 0;
    track->volBalance = val << 8;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        track->voiceAttr.mask |= AKAO_UPDATE_SPU_VOICE;
    }
}

void AkaoOp_F7_OverlayVolBalanceSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 steps;
    u8 target;
    s16 curr;

    steps = *track->akaoSequencePointer++;
    track->volBalanceSlideSteps = steps;
    if (steps == 0) {
        track->volBalanceSlideSteps = 0x100;
    }
    target = *track->akaoSequencePointer++;
    curr = track->volBalance & 0xFF00;
    track->volBalance = curr;
    track->volBalanceSlideStep = ((target << 8) - curr) / track->volBalanceSlideSteps;
}

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

void AkaoOp_A1_LoadInstrument(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoInstrument* instr;
    u16 channelId;
    u8 id;

    channelId = track->overlayChannelId;
    id = *track->akaoSequencePointer++;
    if (g_AkaoMusicSlot) {
        channelId -= AKAO_NUM_VOICES;
    }
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        config->overMask &= ~(1 << channelId);
        track->updateFlags &= ~AKAO_UPDATE_OVERLAY;
    }
    if (track->playingType != AKAO_MUSIC || !(mask & config->keyedMask & g_AkaoSfxLanes->activeMask)) {
        track->voiceAttr.mask |= SPU_VOICE_PITCH;
        track->basePitch = (u32)(track->basePitch * g_AkaoInstrument[id].pitch[0]) /
                           g_AkaoInstrument[track->currentInstrument].pitch[0];
    }
    if (track->updateFlags & AKAO_UPDATE_ALTERNATIVE) {
        instr = &g_AkaoInstrument[id];
        track->currentInstrument = id;
        track->voiceAttr.addr = instr->addr;
        track->voiceAttr.loop_addr = instr->loopAddr;
        track->voiceAttr.ar = instr->ar;
        track->voiceAttr.dr = instr->dr;
        track->voiceAttr.sl = instr->sl;
        track->voiceAttr.sr = instr->sr;
        track->voiceAttr.a_mode = instr->aMode;
        track->voiceAttr.s_mode = instr->sMode;
        track->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE_WOR;
    } else {
        AkaoInstrInit(track, id);
    }
}

void AkaoOp_F2_LoadInstrument(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoInstrument* instr;
    u8 id;

    id = *track->akaoSequencePointer++;
    instr = &g_AkaoInstrument[id];
    if (track->playingType != AKAO_MUSIC || !(mask & config->keyedMask & g_AkaoSfxLanes->activeMask)) {
        track->voiceAttr.mask |= SPU_VOICE_PITCH;
        track->basePitch =
            (u32)(track->basePitch * instr->pitch[0]) / g_AkaoInstrument[track->currentInstrument].pitch[0];
    }
    track->currentInstrument = id;
    track->voiceAttr.addr = 0x76FE0;
    track->voiceAttr.loop_addr = instr->loopAddr;
    track->voiceAttr.ar = instr->ar;
    track->voiceAttr.dr = instr->dr;
    track->voiceAttr.sl = instr->sl;
    track->voiceAttr.sr = instr->sr;
    track->voiceAttr.a_mode = instr->aMode;
    track->voiceAttr.s_mode = instr->sMode;
    if (track->updateFlags & AKAO_UPDATE_ALTERNATIVE) {
        track->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE_WOR;
    } else {
        track->voiceAttr.rr = instr->rr;
        track->voiceAttr.r_mode = instr->rMode;
        track->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE;
    }
}

void AkaoOp_B3_ResetAdsr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoInstrument* instr;
    AkaoChannel* overlay;

    instr = &g_AkaoInstrument[track->currentInstrument];
    track->voiceAttr.ar = instr->ar;
    track->voiceAttr.dr = instr->dr;
    track->voiceAttr.sl = instr->sl;
    track->voiceAttr.sr = instr->sr;
    track->voiceAttr.rr = instr->rr;
    track->voiceAttr.a_mode = instr->aMode;
    track->voiceAttr.s_mode = instr->sMode;
    track->voiceAttr.r_mode = instr->rMode;
    track->voiceAttr.mask |= AKAO_UPDATE_SPU_ADSR;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        overlay = &g_Channel1[track->overlayChannelId];
        overlay->voiceAttr.ar = track->voiceAttr.ar;
        overlay->voiceAttr.dr = track->voiceAttr.dr;
        overlay->voiceAttr.sl = track->voiceAttr.sl;
        overlay->voiceAttr.sr = track->voiceAttr.sr;
        overlay->voiceAttr.rr = track->voiceAttr.rr;
        overlay->voiceAttr.a_mode = track->voiceAttr.a_mode;
        overlay->voiceAttr.s_mode = track->voiceAttr.s_mode;
        overlay->voiceAttr.r_mode = track->voiceAttr.r_mode;
    }
}

static void AkaoOp_C0_TransposeAbsolute(AkaoChannel* track) { track->transpose = (s8)*track->akaoSequencePointer++; }

static void AkaoOp_C1_TransposeRelative(AkaoChannel* track) {
    track->transpose = (s8)*track->akaoSequencePointer++ + track->transpose;
}

void AkaoOp_A4_PitchBendSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 steps;

    steps = *track->akaoSequencePointer++;
    track->pitchSlideSteps = steps;
    if (steps == 0) {
        track->pitchSlideSteps = 0x100;
    }
    track->keyAdd = (s8)*track->akaoSequencePointer++;
}

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

void AkaoOp_B4_Vibrato(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 depth;
    u8 rate;
    s32 pitch;
    u32 amplitude;
    s32 base;

    track->updateFlags |= AKAO_UPDATE_VIBRATO;
    if (track->playingType != AKAO_MUSIC) {
        track->vibratoDelay = 0;
        depth = *track->akaoSequencePointer++;
        if (depth) {
            track->vibratoDepth = depth << 8;
        }
    } else {
        track->vibratoDelay = *track->akaoSequencePointer++;
    }
    rate = *track->akaoSequencePointer++;
    track->vibratoRate = rate;
    if (rate == 0) {
        track->vibratoRate = 0x100;
    }
    pitch = (u16)track->basePitch;
    track->vibratoType = *track->akaoSequencePointer++;
    amplitude = (track->vibratoDepth & 0x7F00) >> 8;
    if (track->vibratoDepth & 0x8000) {
        base = (s32)(amplitude * pitch) >> 7;
    } else {
        base = (s32)(amplitude * ((pitch * 15) >> 8)) >> 7;
    }
    track->vibratoBase = base;
    track->vibratoWave = g_AkaoWaveTableKey[track->vibratoType];
    track->vibratoDelayCur = track->vibratoDelay;
    track->vibratoRateCur = 1;
}

void AkaoOp_B5_VibratoDepth(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    s32 pitch;
    u16 depth;
    u32 amplitude;
    s32 base;

    depth = *track->akaoSequencePointer++ << 8;
    pitch = track->basePitch;
    track->vibratoDepth = depth;
    amplitude = (depth & 0x7F00) >> 8;
    if (depth & 0x8000) {
        base = amplitude * pitch;
    } else {
        base = amplitude * ((pitch * 15) >> 8);
    }
    track->vibratoBase = base >> 7;
}

void AkaoOp_DD_VibratoDepthSlide(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u16 rate;
    u8* addr;

    addr = track->akaoSequencePointer;
    track->akaoSequencePointer = addr + 1;
    rate = addr[0];
    if (rate == 0) {
        rate = 0x100;
    }
    track->akaoSequencePointer = addr + 2;
    track->vibratoDepthSlideStep = ((addr[1] << 8) - track->vibratoDepth) / rate;
    track->vibratoDepthSlideSteps = rate;
}

static void AkaoOp_B6_VibratoOff(AkaoChannel* track) {
    track->vibratoPitch = 0;
    track->updateFlags &= ~AKAO_UPDATE_VIBRATO;
    track->voiceAttr.mask |= SPU_VOICE_PITCH;
}

void AkaoOp_B8_Tremolo(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 depth;
    u8 rate;

    track->updateFlags |= AKAO_UPDATE_TREMOLO;
    if (track->playingType != AKAO_MUSIC) {
        track->tremoloDelay = 0;
        depth = *track->akaoSequencePointer++;
        if (depth) {
            track->tremoloDepth = depth << 8;
        }
    } else {
        track->tremoloDelay = *track->akaoSequencePointer++;
    }
    rate = *track->akaoSequencePointer++;
    track->tremoloRate = rate;
    if (rate == 0) {
        track->tremoloRate = 0x100;
    }
    track->tremoloType = *track->akaoSequencePointer++;
    track->tremoloWave = g_AkaoWaveTableKey[track->tremoloType];
    track->tremoloDelayCur = track->tremoloDelay;
    track->tremoloRateCur = 1;
}

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
        g_AkaoSfxLanes->noiseMask |= mask;
    }
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    AkaoUpdateNoiseVoices();
}

static void AkaoOp_C5_NoiseOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->noiseMask &= ~mask;
    } else {
        g_AkaoSfxLanes->noiseMask &= ~mask;
    }
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    AkaoUpdateNoiseVoices();
    track->noiseSwitchDelay = 0;
}

static void AkaoOp_C6_PitchLfoOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->pitchLfoMask = mask | config->pitchLfoMask;
    } else if (!(mask & 0x555555)) {
        g_AkaoSfxLanes->pitchLfoMask |= mask;
    }
    AkaoUpdatePitchLfoVoices();
}

static void AkaoOp_C7_PitchLfoOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->pitchLfoMask &= ~mask;
    } else {
        g_AkaoSfxLanes->pitchLfoMask &= ~mask;
    }
    AkaoUpdatePitchLfoVoices();
    track->pitchLfoSwitchDelay = 0;
}

static void AkaoOp_C2_ReverbOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->reverbMask = mask | config->reverbMask;
    } else {
        g_AkaoSfxLanes->reverbMask |= mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_C3_ReverbOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == AKAO_MUSIC) {
        config->reverbMask = ~mask & config->reverbMask;
    } else {
        g_AkaoSfxLanes->reverbMask &= ~mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_CC_LegatoOn(AkaoChannel* track) { track->sfxMask = AKAO_SFX_LEGATO; }

static void AkaoOp_CD_LegatoOff(void) {}

static void AkaoOp_D0_FullLengthOn(AkaoChannel* track) { track->sfxMask = AKAO_SFX_FULL_LENGTH; }

static void AkaoOp_D1_FullLengthOff(void) {}

void AkaoOp_AC_NoiseClockFreq(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u8 freq;

    freq = *track->akaoSequencePointer++;
    if (track->playingType == AKAO_MUSIC) {
        if (freq & 0xC0) {
            config->noiseClock = (config->noiseClock + (freq & 0x3F)) & 0x3F;
        } else {
            config->noiseClock = freq;
        }
    } else if (freq & 0xC0) {
        g_AkaoSfxLanes->noiseClock = (g_AkaoSfxLanes->noiseClock + (freq & 0x3F)) & 0x3F;
    } else {
        g_AkaoSfxLanes->noiseClock = freq;
    }
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_NOISE_CLOCK;
}

void AkaoOp_AD_SetAr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.ar = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_AR;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.ar = track->voiceAttr.ar;
    }
}

void AkaoOp_AE_SetDr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.dr = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_DR;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.dr = track->voiceAttr.dr;
    }
}

void AkaoOp_AF_SetSl(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.sl = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_SL;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.sl = track->voiceAttr.sl;
    }
}

void AkaoOp_B1_SetSr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.sr = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.sr = track->voiceAttr.sr;
    }
}

void AkaoOp_B2_SetRr(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.rr = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.rr = track->voiceAttr.rr;
    }
}

void AkaoOp_B7_AttackMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.a_mode = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_AMODE;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.a_mode = track->voiceAttr.a_mode;
    }
}

void AkaoOp_BB_SustainMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.s_mode = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_SMODE;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.s_mode = track->voiceAttr.s_mode;
    }
}

void AkaoOp_BF_ReleaseMode(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->voiceAttr.r_mode = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_RMODE;
    if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
        g_Channel1[track->overlayChannelId].voiceAttr.r_mode = track->voiceAttr.r_mode;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F8_AltVoiceOn);

void AkaoOp_F9_AltVoiceOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    config->altMask &= ~(1 << track->alternativeChannelId);
    track->updateFlags &= ~AKAO_UPDATE_ALTERNATIVE;
    track->voiceAttr.rr = g_AkaoInstrument[track->currentInstrument].rr;
    track->voiceAttr.mask |= SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR;
}

void AkaoOp_C8_LoopPoint(AkaoChannel* track) {
    track->loopId = (track->loopId + 1) & 3;
    track->loopPoint[track->loopId] = track->akaoSequencePointer;
    track->loopTimes[track->loopId] = 0;
}

void AkaoOp_C9_LoopReturnTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u16 times;

    times = *track->akaoSequencePointer++;
    if (times == 0) {
        times = 0x100;
    }
    if (++track->loopTimes[track->loopId] != times) {
        track->akaoSequencePointer = track->loopPoint[track->loopId];
    } else {
        track->loopId = (track->loopId - 1) & 3;
    }
}

void AkaoOp_F0_LoopJumpTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u16 times;

    times = *track->akaoSequencePointer++;
    if (times == 0) {
        times = 0x100;
    }
    if (track->loopTimes[track->loopId] + 1 != times) {
        track->akaoSequencePointer += 2;
    } else {
        track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
    }
}

void AkaoOp_F1_LoopBreakTimes(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    u16 times;

    times = *track->akaoSequencePointer++;
    if (times == 0) {
        times = 0x100;
    }
    if (track->loopTimes[track->loopId] + 1 != times) {
        track->akaoSequencePointer += 2;
    } else {
        track->akaoSequencePointer += READ_S16(track->akaoSequencePointer);
        track->loopId = (track->loopId - 1) & 3;
    }
}

void AkaoOp_CA_LoopReturn(AkaoChannel* track) {
    track->loopTimes[track->loopId]++;
    track->akaoSequencePointer = track->loopPoint[track->loopId];
}

static void AkaoOp_A2_NextNoteLength(AkaoChannel* track) {
    u16 val = *track->akaoSequencePointer++;

    track->lengthFixed = 0;
    track->length = (val << 8) | val;
    track->lengthStored = val;
}

static void AkaoOp_DC_FixNoteLength(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    short delta = READ_S8(track->akaoSequencePointer);
    if (delta) {
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

void AkaoOp_A0_FinishChannel(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoChannel* channel;
    u32 keep;
    u16 id;

    if (track->playingType == AKAO_MUSIC) {
        keep = mask ^ 0xFFFFFF;
        config->activeMask &= keep;
        if (config->activeMask == 0) {
            config->musicId = 0;
        }
        config->noiseMask &= keep;
        config->reverbMask &= keep;
        config->pitchLfoMask &= keep;
        if (track->updateFlags & AKAO_UPDATE_OVERLAY) {
            id = track->overlayChannelId;
            if (g_AkaoMusicSlot) {
                id -= AKAO_NUM_VOICES;
            }
            config->overMask &= ~(1 << id);
        }
        if (track->updateFlags & AKAO_UPDATE_ALTERNATIVE) {
            config->altMask &= ~(1 << track->alternativeChannelId);
        }
    } else {
        keep = mask ^ 0xFF0000;
        g_AkaoSfxLanes[0].activeMask &= keep;
        g_AkaoSfxLanes->noiseMask &= keep;
        g_AkaoSfxLanes->reverbMask &= keep;
        g_AkaoSfxLanes->pitchLfoMask &= keep;
        g_AkaoBgmLanes->onMask &= ~mask;
        g_AkaoBgmLanes->keyedMask &= ~mask;
        g_AkaoBgmLanes->offMask &= ~mask;
        channel = &g_Channel1[track->alternativeChannelId];
        channel->voiceAttr.mask |= AKAO_UPDATE_SPU_BASE;
    }
    track->updateFlags = 0;
    g_AkaoBgmLanes->updateFlags |= AKAO_UPDATE_NOISE_CLOCK;
    AkaoUpdateNoiseVoices();
    AkaoUpdateReverbVoices();
    AkaoUpdatePitchLfoVoices();
}

static void AkaoOp_Null(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    AkaoOp_A0_FinishChannel(track, config, mask);
}
