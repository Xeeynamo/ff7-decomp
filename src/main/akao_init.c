//! PSYQ=3.3 CC1=2.6.3 G=8 COMM=true

#include "akao_private.h"
#include <kernel.h>
#include <libapi.h>
#include <libetc.h>

// Small globals this file reaches through $gp. They must be defined here: akao.c only sees them as extern.
volatile s16 g_AkaoTransfer;
u32 g_AkaoSoundEvent;
s32 g_AkaoStreamMask;
s16 g_AkaoPitchMulMusicSlideSteps;
s16 g_AkaoVolMulMusicSlideSteps;
s16 g_AkaoTempoMulMusicSlideSteps;
s32 g_AkaoVolMulMusic;
u16 g_AkaoReverbPan;
u_long g_AkaoEffectsAll;
u_long g_AkaoEffectsAllSeq;
s32 g_AkaoMutex;
u16 g_AkaoReverbMul;
u16 g_AkaoCdVolSlideSteps;
AkaoCdVol g_AkaoCdVol;
u32 g_AkaoMuteMusicMask;
s32 g_AkaoPitchMulMusic;
s32 g_AkaoTempoMulMusic;
s32 g_AkaoControlFlags;
s32 g_AkaoCommandQueueId;
s32 g_Channel2VoiceMask;
u16 g_AkaoLastHcount;

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

void AkaoInitData(void) {
    AkaoVoiceWork* work;
    AkaoChannel* channel;
    u16 i;

    g_AkaoBgmLanes[0].stereoMono = 1;
    g_AkaoVolMulMusic = 0x7F0000;
    g_AkaoCdVol.val = 0x7FFF0000;
    g_AkaoPrevBgmLanes[1].musicId = 0;
    g_AkaoPrevBgmLanes[0].musicId = 0;
    g_AkaoBgmLanes[1].musicId = 0;
    g_AkaoBgmLanes[0].musicId = 0;
    g_AkaoPrevBgmLanes[1].activeMask = 0;
    g_AkaoPrevBgmLanes[0].activeMask = 0;
    g_AkaoSfxLanes->activeMask = 0;
    g_AkaoBgmLanes[1].activeMask = 0;
    g_AkaoBgmLanes[0].activeMask = 0;
    g_Channel2VoiceMask = 0;
    g_AkaoStreamMask = 0;
    g_AkaoSfxLanes->activeMaskStored = 0;
    g_AkaoBgmLanes[0].activeMaskStored = 0;
    g_AkaoCdVolSlideSteps = 0;
    g_AkaoVolMulMusicSlideSteps = 0;
    g_AkaoPitchMulMusicSlideSteps = 0;
    g_AkaoPitchMulMusic = 0;
    g_AkaoTempoMulMusicSlideSteps = 0;
    g_AkaoTempoMulMusic = 0;
    g_AkaoSfxLanes->pitchLfoMask = 0;
    g_AkaoBgmLanes[0].pitchLfoMask = 0;
    g_AkaoSfxLanes->reverbMask = 0;
    g_AkaoBgmLanes[0].reverbMask = 0;
    g_AkaoSfxLanes->noiseMask = 0;
    g_AkaoBgmLanes[0].noiseMask = 0;
    g_AkaoCommandQueue->opcode = 0;
    g_AkaoBgmLanes[0].timerLower = 0;
    g_AkaoBgmLanes[0].timerUpperCur = 0;
    g_AkaoBgmLanes[0].timerUpper = 0;
    g_AkaoBgmLanes[0].timerTopCur = 0;
    g_AkaoBgmLanes[0].reverbDepthSlideSteps = 0;
    g_AkaoBgmLanes->reverbDepth = 0;
    g_AkaoBgmLanes->reverbMode = 0;
    g_SpuCommonAttr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_MVOLMODEL | SPU_COMMON_MVOLMODER |
                           SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDREV | SPU_COMMON_CDMIX |
                           SPU_COMMON_EXTVOLL | SPU_COMMON_EXTVOLR | SPU_COMMON_EXTREV | SPU_COMMON_EXTMIX;
    g_SpuCommonAttr.mvol.right = 0x3FFF;
    g_SpuCommonAttr.mvol.left = 0x3FFF;
    g_SpuCommonAttr.mvolmode.left = 0;
    g_SpuCommonAttr.mvolmode.right = 0;
    g_SpuCommonAttr.cd.volume.right = 0x7FFF;
    g_SpuCommonAttr.cd.volume.left = 0x7FFF;
    g_SpuCommonAttr.cd.reverb = 0;
    g_SpuCommonAttr.cd.mix = 1;
    g_SpuCommonAttr.ext.volume.right = 0;
    g_SpuCommonAttr.ext.volume.left = 0;
    g_SpuCommonAttr.ext.reverb = 0;
    g_SpuCommonAttr.ext.mix = 0;
    SpuSetCommonAttr(&g_SpuCommonAttr);
    g_AkaoReverbMul = 0;
    g_AkaoLastHcount = 0;
    g_AkaoMutex = 0;
    g_AkaoCommandQueueId = 0;
    g_AkaoMuteMusicMask = 0;
    g_Channel2VoiceMask = 0;
    g_AkaoControlFlags = 0;
    g_AkaoReverbPan = AKAO_PAN_CENTER;
    work = g_AkaoVoiceWork;
    channel = g_Channel1;
    for (i = 0; i < AKAO_NUM_VOICES; i++, channel++, work++) {
        work->currentKey = 0;
        work->volSlide = 0;
        channel->setToMinusOne = 0;
        channel->updateFlags = 0;
        channel->voiceAttr.voice_id = i;
        channel->playingType = 0;
        SpuSetVoiceVolumeAttr(i, 0, 0, 0, 0);
        work->pitchSlide = 0x7F0000;
    }
    channel = g_AkaoSoundSlots[0].voices;
    for (i = 16; i < AKAO_NUM_VOICES; i++, channel++) {
        channel->pitchMulSoundSlideSteps = 0;
        channel->pitchMulSound = 0;
        channel->volBalanceSlideSteps = 0;
        channel->setToMinusOne = 0;
        channel->updateFlags = 0;
        channel->alternativeChannelId = i;
        channel->voiceAttr.voice_id = i;
        channel->playingType = 1;
        channel->volBalance = 0x7F00;
    }
    g_AkaoSfxLanes->tempoUpdate = 1;
    g_AkaoSfxLanes->offMask = 0;
    g_AkaoSfxLanes->keyedMask = 0;
    g_AkaoSfxLanes->onMask = 0;
    g_AkaoBgmLanes[0].offMask = 0;
    g_AkaoBgmLanes[0].keyedMask = 0;
    g_AkaoBgmLanes[0].onMask = 0;
    g_AkaoSfxLanes->tempo = 0x66A80000;
}

// Uploads the samples of an INSTR*.ALL file to the SPU and copies the instrument table of the matching INSTR*.DAT file.
void AkaoLoadInstr(u32* instrAll, u32* instrDat) {
    u32* dst;
    u32 count;
    SpuSetTransferStartAddr(*instrAll++);
    count = *instrAll++;
    instrAll += 2;
    AkaoSpuWrite((u8*)instrAll, count);
    dst = (u32*)&g_AkaoInstrument[0];
    count = sizeof(g_AkaoInstrument) / sizeof(u32);
    do {
        count--;
        *dst++ = *instrDat++;
    } while (count);
    AkaoSpuTransferSync();
}

// Same as AkaoLoadInstr, but only replaces the instruments from AKAO_INSTR2_FIRST onwards.
void AkaoLoadInstr2(u32* instrAll, u32* instrDat) {
    u32* dst;
    u32 count;
    SpuSetTransferStartAddr(*instrAll++);
    count = *instrAll++;
    instrAll += 2;
    AkaoSpuWrite((u8*)instrAll, count);
    dst = (u32*)&g_AkaoInstrument[AKAO_INSTR2_FIRST];
    count = (AKAO_INSTR_COUNT - AKAO_INSTR2_FIRST) * sizeof(AkaoInstrument) / sizeof(u32);
    do {
        count--;
        *dst++ = *instrDat++;
    } while (count);
    AkaoSpuTransferSync();
}

void AkaoStart(u32* instrAll, u32* instrDat) {
    g_AkaoEffectsAll = (u_long)g_AkaoEffectsBuffer;
    g_AkaoEffectsAllSeq = (u_long)g_AkaoEffectsBuffer + 0x1000;
    SpuInitMalloc(4, g_AkaoSpuMallocRec);
    SpuMallocWithStartAddr(0x77000, 0x2000);
    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
    AkaoLoadInstr(instrAll, instrDat);
    SpuSetTransferStartAddr(0x76FE0);
    AkaoSpuWrite(g_AkaoDefaultSound, sizeof(g_AkaoDefaultSound));
    AkaoSpuTransferSync();
    AkaoInitData();
    do {
        g_AkaoSoundEvent = OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, AkaoMain);
    } while (g_AkaoSoundEvent == -1);
    while (EnableEvent(g_AkaoSoundEvent) == 0) {
    }
    while (SetRCnt(RCntCNT2, 0x43D1, RCntMdINTR) == 0) {
    }
    while (StartRCnt(RCntCNT2) == 0) {
    }
}

void AkaoLoadEffect(u32* effectAll) {
    u32* dst;
    u16 i;

    dst = (u32*)g_AkaoEffectsAll;
    i = sizeof(g_AkaoEffectsBuffer) / 4;
    do {
        i--;
        *dst++ = *effectAll++;
    } while (i != 0);
}

void AkaoDeinit(void) {
    while (StopRCnt(RCntCNT2) == 0) {
    }
    UnDeliverEvent(RCntCNT2, EvSpINT);
    while (DisableEvent(g_AkaoSoundEvent) == 0) {
    }
    while (CloseEvent(g_AkaoSoundEvent) == 0) {
    }
    SpuSetTransferCallback(NULL);
    SpuSetIRQ(SPU_OFF);
    SpuSetIRQCallback(NULL);
    SpuSetKey(SPU_OFF, 0xFFFFFF);
}
