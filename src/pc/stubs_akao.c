#include "../main/akao_private.h"

// The driver indexes g_Channel1 past its end to reach the second music slot and the SFX slots,
// so the three arrays must stay adjacent and in this order.
#define AKAO_CHANNELS __attribute__((section(".bss.akao_channels")))
AkaoChannel g_Channel1[AKAO_NUM_VOICES] AKAO_CHANNELS;
AkaoChannel g_Channel2[AKAO_NUM_VOICES] AKAO_CHANNELS;
AkaoSoundSlot g_AkaoSoundSlots[4] AKAO_CHANNELS;

AkaoChannelConfig g_AkaoBgmLanes[2];
AkaoChannelConfig g_AkaoPrevBgmLanes[2];
AkaoSoundConfig g_AkaoSfxLanes[1];
AkaoChannel g_AkaoSavedChannels0[AKAO_NUM_VOICES];
AkaoChannel g_AkaoSavedChannels1[AKAO_NUM_VOICES];
AkaoQueuedCommand g_AkaoCommandQueue[32];
AkaoVoiceAttr g_AkaoVoiceAttr[1];
AkaoVoiceWork g_AkaoVoiceWork[AKAO_NUM_VOICES];
AkaoInstrument g_AkaoInstrument[AKAO_INSTR_COUNT];
s32 g_AkaoMusicBuffer[0x6000 / 4];
u8 g_AkaoEffectsBuffer[0xC800];
u8 g_AkaoSpuMallocRec[SPU_MALLOC_RECSIZ * (4 + 1)];
SpuReverbAttr g_ReverbAttr;
SpuCommonAttr g_SpuCommonAttr;
u16 g_AkaoMusicFadeSteps;
s32 g_AkaoMusicSlot;
s32 g_AkaoPitchMulMusicSlideStep;
s32 g_AkaoVolMulMusicSlideStep;
s32 g_AkaoTempoMulMusicSlideStep;
s32 g_AkaoCdVolSlideStep;
u8* g_AkaoStreamSrc;
u8* g_AkaoStreamLoopSrc;
u32 g_AkaoStreamLoopSize;
u32 g_AkaoStreamRemainingBytes;
AkaoStreamFormat g_AkaoStreamFormat;
s32 g_AkaoStreamVol;
s32 g_AkaoStreamPan;
s32 g_AkaoStreamVoice16UpdateMask;
s32 g_AkaoStreamVoice17UpdateMask;

void SpuSetVoiceVolumeAttr(int vNum, short volL, short volR, short volModeL, short volModeR) {
    SpuVoiceAttr attr;

    attr.voice = 1 << vNum;
    attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    attr.volume.left = volL;
    attr.volume.right = volR;
    attr.volmode.left = volModeL;
    attr.volmode.right = volModeR;
    SpuSetVoiceAttr(&attr);
}

// psyz finishes SPU transfers inside SpuWrite and never raises the DMA interrupt, so report the transfer as done
// right away or AkaoSpuTransferSync spins forever.
SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc func) {
    if (func) {
        func();
    }
    return 0;
}
