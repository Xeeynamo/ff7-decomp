//! PSYQ=3.3 CC1=2.6.3 G=8 COMM=true

#include "akao.h"


void AkaoSpuTransferComplete(void) {
    SpuSetTransferCallback(NULL);
    g_AkaoTransfer = 0;
}

void AkaoSpuTransferPrep(void) {
    g_AkaoTransfer = 1;
    SpuSetTransferCallback(AkaoSpuTransferComplete);
}

static void AkaoSpuWrite(s32 addr, s32 size) {
    AkaoSpuTransferPrep();
    SpuWrite(addr, size);
}

static void AkaoSpuRead(s32 addr, s32 size) {
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
        D_80097768 = AKAO_UPDATE_SPU_ALL;
    }
    if (g_AkaoStreamMask & 0x20000) {
        D_80097870 = AKAO_UPDATE_SPU_ALL;
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
// g_Channel1 channel at the stop sequence D_80049C40 and sets length1/2 to
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
                channel->akaoSequencePointer = D_80049C40;
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
void AkaoMusicCopyChannelsAndConfig(void* srcChannels, void* dstChannels, void* srcConfig, void* dstConfig);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoMusicCopyChannelsAndConfig);



/////////////////////////
// AKAO COMMANDS
/////////////////////////


// Copies the sequence to the staging buffer, restores channels and config from backup
// if musicId matches backup slot 0 or 1, otherwise initializes fresh music channels.
void AkaoCmd_10_PlayMusic(AkaoCommand* cmd) {
    AkaoCopyMusic((s32*)cmd->param0, cmd->param1);
    if (g_AkaoMusicId == MUSIC_TA) {
        AkaoMusicSyncKeyStatus();
        AkaoMusicCopyChannelsAndConfig(&g_Channel1, &g_AkaoSavedChannels1, &g_Channel1Config, &g_AkaoSavedChannelConfig1);
    }
    AkaoMusicStopChannels1();
    if (g_AkaoSavedMusicId0 && g_AkaoSavedMusicId0 == cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(0);
    } else if (g_AkaoSavedMusicId1 && g_AkaoSavedMusicId1 == cmd->param2) {
        AkaoMusicRestoreChannelsAndConfig(1);
    } else {
        AkaoMusicChannelsInit();
    }
    g_AkaoMusicId = cmd->param2;
}

// Copies the music sequence to the staging buffer, backs up the currently playing
// song (to backup slot 1 if MUSIC_TA, or slot 0 for any other song), stops channels,
// initializes new channels from the beginning, and sets g_AkaoMusicId.
void AkaoCmd_14_PlayMusicSaveCurrent(AkaoCommand* cmd) {
    s32* channelConfig;

    AkaoCopyMusic((s32*)cmd->param0, cmd->param1);
    AkaoMusicSyncKeyStatus();
    channelConfig = &g_Channel1Config;
    if (g_AkaoMusicId) {
        if (g_AkaoMusicId == MUSIC_TA) {
            AkaoMusicCopyChannelsAndConfig(&g_Channel1, &g_AkaoSavedChannels1, channelConfig, &g_AkaoSavedChannelConfig1);
        } else {
            AkaoMusicCopyChannelsAndConfig(&g_Channel1, &g_AkaoSavedChannels0, channelConfig, &g_AkaoSavedChannelConfig0);
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
void AkaoCmd_15_PlayMusicSwapSaved(AkaoCommand* cmd);
INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_15_PlayMusicSwapSaved);

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_10_PlayMusic (resuming from backup if previously saved).
void AkaoCmd_18_FadePlayMusic(AkaoCommand* cmd) {
    if (g_AkaoMusicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_10_PlayMusic(cmd);
}

// Fades out the currently playing music (if any) over cmd->param3 ticks (default 0x10)
// and plays new music via AkaoCmd_14_PlayMusicSaveCurrent (saving current music to backup).
void AkaoCmd_19_FadePlayMusicSaveCurrent(AkaoCommand* cmd) {
    if (g_AkaoMusicId) {
        g_AkaoMusicFadeSteps = cmd->param3 ? cmd->param3 : 0x10;
        AkaoMusicCopyChannels1Into2();
    }
    AkaoCmd_14_PlayMusicSaveCurrent(cmd);
}

// Clears sound channel 4 (1 voice) and initializes it with center pan (0x40)
// using the provided raw sound sequence pointers directly (bypassing table lookup).
void AkaoCmd_34_PlaySoundDirect(AkaoCommand* cmd) {
    AkaoSoundChannelsClear(4, 1);
    AkaoSoundChannelsInit(AKAO_PAN_CENTER, AKAO_SFX_SLOT_2, cmd->param0, cmd->param1);
}

// Clears sound channels for 2 voices starting at voice 4 (SFX slots 1 and 2),
// then resolves and initializes two sound effect sequences with the requested pan.
void AkaoCmd_21_PlayTwoSounds(AkaoCommand* cmd) {
    s32 seq0, seq1;

    AkaoSoundChannelsClear(4, 2);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param1);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_1, seq0, seq1);
    AkaoSoundGetSequence(&seq0, &seq1, cmd->param2);
    AkaoSoundChannelsInit(cmd->param0, AKAO_SFX_SLOT_2, seq0, seq1);
}

void AkaoCmd_22_PlayThreeSounds(AkaoCommand* cmd) {
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
void AkaoCmd_23_PlayFourSounds(AkaoCommand* cmd) {
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

void AkaoCmd_30(AkaoCommand* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->param0);
    AkaoSoundMenuChannelsInit(sp10, sp14);
}

void AkaoCmd_20(AkaoCommand* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(4, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->param1);
    AkaoSoundChannelsInit(arg0->param0, 0x34, sp10, sp14);
}

void AkaoCmd_29(AkaoCommand* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(2, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->param1);
    AkaoSoundChannelsInit(arg0->param0, 0x32, sp10, sp14);
}

void AkaoCmd_2A(AkaoCommand* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(0, 1);
    AkaoStreamStop();
    AkaoSoundGetSequence(&sp10, &sp14, arg0->param1);
    AkaoSoundChannelsInit(arg0->param0, 0x30, sp10, sp14);
}

void AkaoCmd_2B(AkaoCommand* arg0) {
    s32 sp10, sp14;

    AkaoSoundChannelsClear(6, 1);
    AkaoSoundGetSequence(&sp10, &sp14, arg0->param1);
    AkaoSoundChannelsInit(arg0->param0, 0x36, sp10, sp14);
}

void AkaoCmd_C0_VolumeSet(AkaoCommand* arg0) {
    g_AkaoVolMulMusicSlideSteps = 0;
    g_AkaoVolMulMusic = (arg0->param0 & 0x7F) << 0x10;
    AkaoMusicVolReset();
}

// Starts a volume slide from the current g_AkaoVolMulMusic toward a target
// derived from arg0, over arg0's tick count.
void AkaoCmd_C1_VolSlideFromCurr(AkaoVolSlideFromCurr* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoVolMulMusicSlideSteps = var_a1;
    g_AkaoVolMulMusicSlideStep = (((arg0->targetVol & 0x7F) << 0x10) - g_AkaoVolMulMusic) / var_a1;
    AkaoMusicVolReset();
}

// Starts a volume slide between two explicit targets from arg0 (rather than
// from the current g_AkaoVolMulMusic), over arg0's tick count.
void AkaoCmd_C2_VolSlideBetweenTargets(AkaoVolSlideBetweenTargets* arg0) {
    s32 temp_v1;
    s32 var_a1;
    s32 temp_v0;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v0 = (arg0->targetVol & 0x7F) << 0x10;
    temp_v1 = (arg0->startVol & 0x7F) << 0x10;
    g_AkaoVolMulMusicSlideSteps = var_a1;
    g_AkaoVolMulMusic = temp_v1;
    g_AkaoVolMulMusicSlideStep = (temp_v0 - temp_v1) / var_a1;
    AkaoMusicVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_C8);

// Starts a CD-audio volume slide from the current g_AkaoCdVol toward a
// target derived from arg0, over arg0's tick count.
void AkaoCmd_C9_CdVolSlideFromCurr(AkaoCdVolSlideFromCurr* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoCdVolSlideSteps = var_a1;
    g_AkaoCdVolSlideStep = ((arg0->targetVol << 0x10) - g_AkaoCdVol.val) / var_a1;
}

// Starts a CD-audio volume slide between two explicit targets from arg0
// (rather than from the current g_AkaoCdVol), over arg0's tick count.
void AkaoCmd_CA_CdVolSlideBetweenTargets(AkaoCdVolSlideBetweenTargets* arg0) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a1;
    s32 temp_v0_shifted;
    s32 temp_v1_shifted;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v0_shifted = arg0->targetVol << 0x10;
    temp_v1_shifted = arg0->startVol << 0x10;
    g_AkaoCdVolSlideSteps = var_a1;
    g_AkaoCdVol.val = temp_v1_shifted;
    g_AkaoCdVolSlideStep = (temp_v0_shifted - temp_v1_shifted) / var_a1;
}

// Sets the volume balance for a 2-voice sound effect channel pair (voice[0]
// and voice[1]). Clears any active balance slide and flags the hardware voices
// (SPU_VOICE_VOLL | SPU_VOICE_VOLR) for volume recalculation.
static void AkaoSoundChannelSetVolBalance(AkaoCommand* cmd, AkaoSoundSlot* slot) {
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
        voice[1].volBalance = (s16)((balance & 0x7F) << 8);
    } while (0);
    voice[0].volBalance = (s16)((balance & 0x7F) << 8);
    mask0 = voice[0].voiceAttr.mask;
    voice[1].voiceAttr.mask = mask1 | 3;
    voice[0].voiceAttr.mask = mask0 | 3;
}

// Starts a volume balance slide from current balance toward target in cmd over
// the specified step count for a 2-voice sound effect channel pair.
static void AkaoSoundChannelSlideVolBalance(AkaoCommand* cmd, AkaoSoundSlot* slot) {
    s16 steps;
    s32 rawSteps;
    AkaoChannel* voice = slot->voices;

    rawSteps = cmd->param0;
    steps = 1;
    if (rawSteps != 0) {
        steps = *(u16*)&cmd->param0;
    }
    voice[0].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & 0x7F) << 8) - voice[0].volBalance) / steps;
    voice[1].volBalanceSlideStep = (s16)(((*(u16*)&cmd->param1 & 0x7F) << 8) - voice[1].volBalance) / steps;
    voice[1].volBalanceSlideSteps = steps;
    voice[0].volBalanceSlideSteps = steps;
}

// Sets the volume balance across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_B8_SetAllSoundVolBalance(AkaoCommand* cmd) {
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the volume balance across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target balance in cmd.
void AkaoCmd_B9_SlideAllSoundVolBalance(AkaoCommand* cmd) {
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlideVolBalance(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A0(void* arg0) { AkaoSoundChannelSetVolBalance(arg0, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A4(void* arg0) { AkaoSoundChannelSlideVolBalance(arg0, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A1(void* arg0) { AkaoSoundChannelSetVolBalance(arg0, &g_AkaoSoundSlots[1]); }

void AkaoCmd_A5(void* arg0) { AkaoSoundChannelSlideVolBalance(arg0, &g_AkaoSoundSlots[1]); }

void AkaoCmd_A2(void* arg0) { AkaoSoundChannelSetVolBalance(arg0, &g_AkaoSoundSlots[0]); }

void AkaoCmd_A6(void* arg0) { AkaoSoundChannelSlideVolBalance(arg0, &g_AkaoSoundSlots[0]); }

void AkaoCmd_A3(void* arg0) { AkaoSoundChannelSetVolBalance(arg0, &g_AkaoSoundSlots[3]); }

void AkaoCmd_A7(void* arg0) { AkaoSoundChannelSlideVolBalance(arg0, &g_AkaoSoundSlots[3]); }

// Sets the stereo pan for a 2-voice sound effect channel pair (voice[0]
// and voice[1]). Clears any active pan slide and flags the hardware voices
// (SPU_VOICE_VOLL | SPU_VOICE_VOLR) for volume recalculation.
static void AkaoSoundChannelSetPan(AkaoCommand* cmd, AkaoSoundSlot* slot) {
    s16 pan;
    s32 mask1;
    AkaoChannel* voice = slot->voices;

    pan = (*(u16*)&cmd->param0 & 0x7F) << 8;
    mask1 = voice[1].voiceAttr.mask;
    voice[1].volPanSlideSteps = 0;
    voice[0].volPanSlideSteps = 0;
    voice[1].volPan = pan;
    voice[0].volPan = pan;
    voice[0].voiceAttr.mask = voice[0].voiceAttr.mask | 3;
    voice[1].voiceAttr.mask = (mask1 | 3);
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelSlidePan);

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BA(void* arg0) {
    AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[0]);
}

// Apply the paired handler to 4 blocks spaced 0x210 bytes apart.
void AkaoCmd_BB(void* arg0) {
    AkaoSoundChannelSlidePan(arg0, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePan(arg0, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePan(arg0, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePan(arg0, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_A8(void* arg0) { AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[2]); }

void AkaoCmd_AC(void* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_A9(void* arg0) { AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AD(void* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_AA(void* arg0) { AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AE(void* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_AB(void* arg0) { AkaoSoundChannelSetPan(arg0, &g_AkaoSoundSlots[3]); }

void AkaoCmd_AF(void* cmd) { AkaoSoundChannelSlidePan(cmd, &g_AkaoSoundSlots[3]); }

// Sets the sound effect pitch multiplier for a 2-voice sound effect channel pair
// (voice[0] and voice[1]). Clears any active pitch slide and flags the hardware voices
// (SPU_VOICE_PITCH) for pitch recalculation.
static void AkaoSoundChannelSetPitch(AkaoCommand* cmd, AkaoSoundSlot* slot) {
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
    voice[0].voiceAttr.mask = voice[0].voiceAttr.mask | 0x10;
    voice[1].voiceAttr.mask = mask1 | 0x10;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoSoundChannelSlidePitch);

// Sets the pitch multiplier across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots.
void AkaoCmd_BC_SetAllSoundPitch(AkaoCommand* cmd) {
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]);
}

// Slides the pitch multiplier across all 4 sound effect channel slots (slots 3, 2, 1, 0)
// in g_AkaoSoundSlots toward the target pitch multiplier in cmd.
void AkaoCmd_BD_SlideAllSoundPitch(AkaoCommand* cmd) {
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]);
    AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]);
}

void AkaoCmd_B0(AkaoCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B4(AkaoCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[2]); }

void AkaoCmd_B1(AkaoCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B5(AkaoCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[1]); }

void AkaoCmd_B2(AkaoCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B6(AkaoCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[0]); }

void AkaoCmd_B3(AkaoCommand* cmd) { AkaoSoundChannelSetPitch(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_B7(AkaoCommand* cmd) { AkaoSoundChannelSlidePitch(cmd, &g_AkaoSoundSlots[3]); }

void AkaoCmd_D0(AkaoTempoPitchSlide* arg0) {
    s32 n = arg0->start;
    g_AkaoTempoMulMusicSlideSteps = 0;
    g_AkaoTempoMulMusic = n << 0x10;
}

// Starts a tempo slide toward a target derived from arg0, over arg0's tick
// count.
void AkaoCmd_D1_TempoSlideFromCurr(AkaoSlideFromCurr* arg0) {
    s32 temp_v0;
    s32 var_a1;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    g_AkaoTempoMulMusicSlideStep = ((arg0->target << 0x10) - g_AkaoTempoMulMusic) / var_a1;
    g_AkaoTempoMulMusicSlideSteps = var_a1;
}

// Starts a tempo slide between two explicit targets from arg0, over arg0's
// tick count.
void AkaoCmd_D2_TempoSlideBetweenTargets(AkaoTempoPitchSlide* arg0) {
    long new_var;
    s32 temp_a2;
    s32 temp_v1;
    s32 var_a1;

    temp_v1 = arg0->steps;
    temp_a2 = arg0->start << 0x10;
    g_AkaoTempoMulMusic = temp_a2;
    var_a1 = 1;
    if (temp_v1 != 0) {
        var_a1 = temp_v1;
    }
    new_var = (arg0->target << 0x10) - temp_a2;
    g_AkaoTempoMulMusicSlideSteps = var_a1;
    g_AkaoTempoMulMusicSlideStep = new_var / var_a1;
}

void AkaoCmd_D4(AkaoTempoPitchSlide* arg0) {
    s32 n = arg0->start;
    g_AkaoPitchMulMusicSlideSteps = 0;
    g_AkaoPitchMulMusic = n << 0x10;
}

// Starts a pitch slide from the current g_AkaoPitchMulMusic toward a
// target derived from arg0, over arg0's tick count.
void AkaoCmd_D5_PitchSlideFromCurr(AkaoSlideFromCurr* arg0) {
    s32 temp_v0;
    s32 var_a1;
    s32 temp_v1;

    temp_v0 = arg0->steps;
    var_a1 = 1;
    if (temp_v0 != 0) {
        var_a1 = temp_v0;
    }
    temp_v1 = ((arg0->target << 0x10) - g_AkaoPitchMulMusic) / var_a1;
    g_AkaoPitchMulMusicSlideSteps = var_a1;
    g_AkaoPitchMulMusicSlideStep = temp_v1;
}

// Starts a pitch slide between two explicit targets from arg0, over arg0's
// tick count.
void AkaoCmd_D6_PitchSlideBetweenTargets(AkaoTempoPitchSlide* arg0) {
    s32 new_var;
    s32 temp_a2;
    s32 temp_v1;
    s32 var_a1;

    temp_v1 = arg0->steps;
    temp_a2 = arg0->start << 0x10;
    g_AkaoPitchMulMusic = temp_a2;
    var_a1 = 1;
    if (temp_v1 != 0) {
        var_a1 = temp_v1;
    }
    new_var = (arg0->target << 0x10) - temp_a2;
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

static void AkaoCmd_81_SetMonoMode(void) {
    g_Channel1Config = 2;
    AkaoMusicVolReset();
    AkaoSoundVolReset();
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_90);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_92);

void AkaoUpdateChannelParamsToSpu(s32, void*);

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
            g_AkaoVoiceAttrSr = 0x7F;
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
                voice->voiceAttr.mask |= SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
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
    g_AkaoControlFlags &= ~1;
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
        if (D_80099E0C == 2) {
            newMask &= ~((1 << 22) | (1 << 23));
        }
        g_AkaoSoundActiveMaskStored = newMask;
        g_Channel3ActiveMask[cleared = 0] = newMask ^ savedMask;
        g_AkaoVoiceAttrVolR = cleared;
        g_AkaoVoiceAttrVolL = cleared;
        g_AkaoVoiceAttrSr = 0x7F;
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
                half->voiceAttr.mask |= SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_SR;
            }
        }
        savedMask = g_AkaoSoundActiveMaskStored;
        g_AkaoSoundActiveMaskStored = 0;
        g_Channel3ActiveMask[0] = savedMask;
        AkaoUpdateNoiseVoices();
        AkaoUpdateReverbVoices();
        AkaoUpdatePitchLfoVoices();
    }
    g_AkaoControlFlags &= ~2;
}

static void AkaoCmd_E0_SetReverbPan(AkaoSetReverbPan* arg0) {
    g_AkaoReverbPan = arg0->pan & 0x7F;
    D_8009A13C |= 0x80;
}

static void AkaoCmd_E4_SetReverbMul(AkaoSetReverbMul* arg0) {
    u8 temp_v0;
    s32 var_v0;
    s32 mask;

    temp_v0 = arg0->mul;
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

static void AkaoCmd_F2(void) { g_AkaoSavedMusicId0 = 0; }

static void AkaoCmd_F3(void) { g_AkaoSavedMusicId1 = 0; }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F4);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoCmd_F5);

static void AkaoCmd_F8_StreamReverbMaskClear(void) {
    s32* addr;
    s32 temp_a0;
    s32 temp_v1;

    AkaoStreamInit();
    addr = g_Channel3ActiveMask;
    temp_a0 = g_AkaoReverbMask;
    temp_v1 = ~g_AkaoStreamMask;
    *addr &= temp_v1;
    g_AkaoReverbMask = temp_v1 & temp_a0;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_F9_StreamReverbMaskRestore(void) {
    s32 temp_a0;

    AkaoStreamInit();
    temp_a0 = g_Channel3ActiveMask[0];
    g_Channel3ActiveMask[0] = ~g_AkaoStreamMask & temp_a0;
    g_AkaoReverbMask |= g_AkaoStreamMask;
    AkaoUpdateReverbVoices();
}

static void AkaoCmd_FA(void) { AkaoStreamStop(); }

void AkaoCmd_Null(AkaoCommand* arg0) {}

static void AkaoClearTransferCallback(void) { SpuSetTransferCallback(0); }

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoStreamInit);

// Configures the voice-attribute block for a mono CD-stream voice (ADSR
// envelope, pan, reverb-echo work area) and applies it via AkaoUpdateChannelParamsToSpu.
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
    g_AkaoVoiceAttrVolL = (g_AkaoStreamPan ^ 0x7F) * g_AkaoStreamVol >> 7;
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

static void AkaoGetCommandQueue(AkaoCommand** out_cmd) {
    *out_cmd = g_AkaoCommandQueue;
    *out_cmd = &g_AkaoCommandQueue[g_AkaoCommandQueueId];
    g_AkaoCommandQueueId++;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoExec);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoDispatchCommand);

static void AkaoExecuteCommandsQueue(void) {
    AkaoCommand* cmd;

    if (g_AkaoMutex == 0) {
        for (cmd = g_AkaoCommandQueue; g_AkaoCommandQueueId; g_AkaoCommandQueueId--, cmd++) {
            ((void (*)(AkaoCommand*))g_AkaoCommandHandler[cmd->opcode])(cmd);
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
    expected = 0xCA;
    do {
        opcode = *data;
        len = g_AkaoOpcodeSize[opcode];
        data += len;
    } while (len);
    return opcode == expected ? 0xCA : 0xA0;
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
    track->updateFlags |= 0x80;
    track->pitchMulSoundSlideStep = combined;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_EB_ReverbDepthSlide);

static void AkaoOp_A3_MasterVol(AkaoChannel* track) {
    track->volumeMultiplier = *track->akaoSequencePointer++;
    track->voiceAttr.mask |= 3;
}

static void AkaoOp_A8_SetVol(AkaoChannel* track) {
    s32 val = (s8)*track->akaoSequencePointer++;

    track->volSlideSteps = 0;
    track->voiceAttr.mask |= 3;
    track->volumeLevel = val << 0x17;
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_A9_SetVolSlide);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F4_OverlayVoiceOn);

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F5_OverlayVoiceOff);

static void AkaoOp_F6_OverlayVolBalance(AkaoChannel* track) {
    u8 val = *track->akaoSequencePointer++;

    track->volBalanceSlideSteps = 0;
    track->volBalance = val << 8;
    if (track->updateFlags & 0x100) {
        track->voiceAttr.mask |= 3;
    }
}

INCLUDE_ASM("asm/us/main/nonmatchings/akao", AkaoOp_F7_OverlayVolBalanceSlide);

static void AkaoOp_AA_SetPan(AkaoChannel* track) {
    track->volPan = *track->akaoSequencePointer++ << 8;
    track->volPanSlideSteps = 0;
    track->voiceAttr.mask |= 3;
}

static void AkaoOp_AB_SetPanSlide(AkaoChannel* track) {
    u8 ch;
    u16 var_a0;

    track->volPanSlideSteps = *track->akaoSequencePointer++;
    if (track->volPanSlideSteps == 0) {
        track->volPanSlideSteps = 0x100;
    }
    ch = *track->akaoSequencePointer++;
    track->volPan &= 0xFF00;
    var_a0 = track->volPan;
    track->volPanSlideStep = ((ch << 8) - var_a0) / (u16)track->volPanSlideSteps;
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
    track->updateFlags &= ~1;
    track->voiceAttr.mask |= 0x10;
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
    track->updateFlags &= ~2;
    track->voiceAttr.mask |= 3;
}

static void AkaoOp_BC_SetPanLfo(AkaoChannel* track) {
    u8* addr;
    u8* addr2;
    u8 rate;

    addr = track->akaoSequencePointer;
    track->updateFlags |= 4;
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
    track->updateFlags &= ~4;
    track->voiceAttr.mask |= 3;
}

static void AkaoOp_C4_NoiseOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->noiseMask = mask | config->noiseMask;
    } else {
        g_AkaoNoiseMask |= mask;
    }
    D_8009A13C |= 0x10;
    AkaoUpdateNoiseVoices();
}

static void AkaoOp_C5_NoiseOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->noiseMask &= ~mask;
    } else {
        g_AkaoNoiseMask &= ~mask;
    }
    D_8009A13C |= 0x10;
    AkaoUpdateNoiseVoices();
    track->noiseSwitchDelay = 0;
}

static void AkaoOp_C6_PitchLfoOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->pitchLfoMask = mask | config->pitchLfoMask;
    } else if (!(mask & 0x555555)) {
        g_AkaoPitchLfoMask |= mask;
    }
    AkaoUpdatePitchLfoVoices();
}

static void AkaoOp_C7_PitchLfoOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->pitchLfoMask &= ~mask;
    } else {
        g_AkaoPitchLfoMask &= ~mask;
    }
    AkaoUpdatePitchLfoVoices();
    track->pitchLfoSwitchDelay = 0;
}

static void AkaoOp_C2_ReverbOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->reverbMask = mask | config->reverbMask;
    } else {
        g_AkaoReverbMask |= mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_C3_ReverbOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    if (track->playingType == 0) {
        config->reverbMask = ~mask & config->reverbMask;
    } else {
        g_AkaoReverbMask &= ~mask;
    }
    AkaoUpdateReverbVoices();
}

static void AkaoOp_CC_LegatoOn(AkaoChannel* track) { track->sfxMask = 1; }

static void AkaoOp_CD_LegatoOff(void) {}

static void AkaoOp_D0_FullLengthOn(AkaoChannel* track) { track->sfxMask = 4; }

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
    track->updateFlags |= 0x8;
}

static void AkaoOp_ED_DrumModeOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~0x8;
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
    s16 var_v0 = *track->akaoSequencePointer++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->noiseSwitchDelay = var_v0;
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
    s16 var_v0 = *track->akaoSequencePointer++;
    if (var_v0 == 0) {
        var_v0 = 257;
    } else {
        var_v0++;
    }
    track->pitchLfoSwitchDelay = var_v0;
}

static void AkaoOp_CB_SfxReset(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~0x37;
    AkaoOp_C5_NoiseOff(track, config, mask);
    AkaoOp_C7_PitchLfoOff(track, config, mask);
    AkaoOp_C3_ReverbOff(track, config, mask);
    track->sfxMask &= ~0x5;
}

static void AkaoOp_D4_SideChainPlaybackOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags |= 0x10;
}

static void AkaoOp_D5_SideChainPlaybackOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~0x10;
}

static void AkaoOp_D6_SideChainPitchVolOn(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags |= 0x20;
}

static void AkaoOp_D7_SideChainPitchVolOff(AkaoChannel* track, AkaoChannelConfig* config, u32 mask) {
    track->updateFlags &= ~0x20;
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
