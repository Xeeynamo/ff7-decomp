//! PSYQ=3.3

#include "highway_private.h"

const MATRIX g_HighwayIdentityMatrix = {{{0x1000, 0, 0}, {0, 0x1000, 0}, {0, 0, 0x1000}}, {0, 0, 0}};

void HighwayScratchpadInit(void) {
    g_HighwayCameraMatrices = &D_8010EBDC;
    g_HighwayScratchpad = (void*)0x1F800000;
    D_80116678 = (void*)0x1F8000A8;
    g_HighwayWorldMatrix = (MATRIX*)0x1F8000C8;
    D_8010EA30 = (void*)0x1F800000;
    g_HighwaySubdivVerts[0] = (SVECTOR*)0x1F800108;
    g_HighwaySubdivVerts[1] = (SVECTOR*)0x1F800110;
    g_HighwaySubdivVerts[2] = (SVECTOR*)0x1F800118;
    g_HighwaySubdivVerts[3] = (SVECTOR*)0x1F800120;
    g_HighwaySubdivVerts[4] = (SVECTOR*)0x1F800128;
    g_HighwaySubdivVerts[5] = (SVECTOR*)0x1F800130;
    g_HighwaySubdivVerts[6] = (SVECTOR*)0x1F800138;
    g_HighwaySubdivVerts[7] = (SVECTOR*)0x1F800140;
    g_HighwaySubdivVerts[8] = (SVECTOR*)0x1F800148;
    g_HighwaySubdivVerts[9] = (SVECTOR*)0x1F800150;
    g_HighwaySubdivVerts[10] = (SVECTOR*)0x1F800158;
    g_HighwaySubdivSxy[0] = (s32*)0x1F800160;
    g_HighwaySubdivSxy[1] = (s32*)0x1F800164;
    g_HighwaySubdivSxy[2] = (s32*)0x1F800168;
    g_HighwaySubdivSxy[3] = (s32*)0x1F80016C;
    g_HighwaySubdivSxy[4] = (s32*)0x1F800170;
    g_HighwaySubdivSxy[5] = (s32*)0x1F800174;
    g_HighwaySubdivSxy[6] = (s32*)0x1F800178;
    g_HighwaySubdivSxy[7] = (s32*)0x1F80017C;
    g_HighwaySubdivSxy[8] = (s32*)0x1F800180;
    g_HighwaySubdivSxy[9] = (s32*)0x1F800184;
    g_HighwaySubdivSxy[10] = (s32*)0x1F800188;
    RotMatrix(&D_800B4220, &g_HighwayCameraMatrices->m[0]);
    RotMatrix(&D_800B4220, &g_HighwayCameraMatrices->m[1]);
    RotMatrix(&D_800B4220, &g_HighwayCameraMatrices->m[2]);
    TransMatrix(&g_HighwayCameraMatrices->m[0], &D_800B4228);
    TransMatrix(&g_HighwayCameraMatrices->m[1], &D_800B4228);
    TransMatrix(&g_HighwayCameraMatrices->m[2], &D_800B4228);
    TransMatrix(&g_HighwayCameraMatrices->m[3], &D_800B4228);
    RotMatrix(&D_800B4220, &g_HighwayOverlayMatrix);
    TransMatrix(&g_HighwayOverlayMatrix, &D_800B4228);
}

void HighwayOverlaysInit(void) {
    g_HighwayOverlayNode0 = HighwayNodeAlloc(0xAE, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayOverlayNode1 = HighwayNodeAlloc(0xA5, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayOverlayNode2 = HighwayNodeAlloc(0xA3, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayOverlayNode3 = HighwayNodeAlloc(0xA7, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
}

void HighwayOverlaysDraw(HighwayBuffer* db) {
    if (g_HighwayOverlayOn0 == 1) {
        HighwayDrawOverlayQuads(db, g_HighwayOverlayNode0, -500);
    }
    if (g_HighwayOverlayOn1 == 1) {
        HighwayDrawOverlayQuads(db, g_HighwayOverlayNode1, -300);
    }
    if (g_HighwayOverlayOn2 == 1) {
        HighwayDrawOverlayQuads(db, g_HighwayOverlayNode2, 0);
    }
    if (g_HighwayOverlayOn3 == 1) {
        HighwayDrawOverlayTris(db, g_HighwayOverlayNode3, -500);
    }
}

void HighwayRaceInit(void) {
    MATRIX light;
    s32 i;

    light = g_HighwayIdentityMatrix;
    SetLightMatrix(&light);
    SetFogNearFar(g_HighwayFogNear, g_HighwayFogFar, 300);
    g_HighwayFogNear = 6500;
    g_HighwayFogFar = 11500;
    D_800BD640 = 16800;
    D_80116428 = 20300;
    g_HighwaySpeed = 0x7A;
    D_800BD638 = 0x80;
    g_HighwayExit = 0;
    D_8010EBCC = 0;
    g_HighwayEnemiesDisabled = 0;
    g_HighwayDistance = 0;
    g_HighwayTrackSegment = 0;
    g_HighwaySegmentFrac = 0;
    g_HighwaySegmentsCrossed = 0;
    D_801163A4 = 0;
    g_HighwayEngineVolume = 0;
    D_80110BC8 = 0;
    g_HighwayEnemyEngineVolume = 0;
    D_80110BD8 = 0;
    g_HighwaySfxCooldown1 = 0;
    g_HighwaySfxCooldown2 = 0;
    g_HighwayGauges[0].maxHp = 0xFFFF;
    g_HighwayGauges[2].maxHp = 0xFFFF;
    g_HighwayGauges[4].maxHp = 0xFFFF;
    g_HighwayGauges[1].maxHp = 0xFFFF;
    g_HighwayGauges[3].maxHp = 0xFFFF;
    for (i = 0; i < LEN(g_HighwayGauges); i++) {
        g_HighwayGauges[i].unk4 = 0;
        g_HighwayGauges[i].unk6 = 0;
        g_HighwayGauges[i].unk8 = 0;
        g_HighwayGauges[i].unk9 = 1;
        g_HighwayGauges[i].hp = g_HighwayGauges[i].maxHp;
    }
    g_HighwayStoryEnding = 0;
    g_HighwayArcadeFinish = 0;
    g_HighwayOverlayOn0 = 0;
    g_HighwayOverlayOn1 = 0;
    g_HighwayOverlayOn2 = 0;
    g_HighwayOverlayOn3 = 0;
    g_HighwayCameraRolled = 0;
    g_HighwayFadeLevel = 0;
    g_HighwayFadeMode = 1;
    g_HighwayOtOffset = 0;
    if (!g_HighwayArcadeMode) {
        g_HighwayEvents = g_HighwayCourses[0].events;
    }
    if (g_HighwayArcadeMode == 1) {
        g_HighwayEvents = g_HighwayCourses[1].events;
    }
}

// Load the overlay's files from disc and wait for each read.
void HighwayLoadAssets(void) {
    SystemLoadFileBySector(g_HighwayAssetFiles[0].loc, g_HighwayAssetFiles[0].len, (u_long*)0x80120000, NULL);
    while (SystemCdromReadChain())
        ;
    HighwayTexturesInit();
    HighwayRoadUvInit();
    SystemLoadFileBySector(g_HighwayAssetFiles[1].loc, g_HighwayAssetFiles[1].len, (u_long*)0x801A0000, NULL);
    while (SystemCdromReadChain())
        ;
    SystemLoadFileBySector(g_HighwayAssetFiles[2].loc, g_HighwayAssetFiles[2].len, (u_long*)0x80120000, NULL);
    while (SystemCdromReadChain())
        ;
}

// Same as JetAudioFadeOut.
void HighwayAudioFadeOut(void) {
    g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
    g_AkaoCmd.params[0] = 0xF0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SLIDE_ALL_VOL_BALANCE;
    g_AkaoCmd.params[0] = 0xF0;
    g_AkaoCmd.params[1] = 0;
    AkaoExec();
}

// Reset the volumes and start the music.
void HighwayAudioInit(void) {
    g_AkaoCmd.opcode = AKAO_VOLUME_SET;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_SET_ALL_VOL_BALANCE;
    g_AkaoCmd.params[0] = AKAO_VOL_MAX;
    AkaoExec();
    g_AkaoCmd.opcode = AKAO_PLAY_MUSIC;
    g_AkaoCmd.params[0] = g_HighwayMusicAddr;
    AkaoExec();
}

void HighwayPlaySfx(s32 soundId, s32 slot, s32 timer) {
    s32 busy;

    busy = 0;
    if (slot == 1 && g_HighwaySfxCooldown1 > 0) {
        busy = 1;
    }
    if (slot == 2 && g_HighwaySfxCooldown2 > 0) {
        busy = 1;
    }
    if (slot == 1 && timer < 0) {
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT2;
        g_AkaoCmd.params[0] = 0;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
        busy = 0;
    }
    if (slot == 2 && timer < 0) {
        g_AkaoCmd.opcode = AKAO_PLAY_SLOT1;
        g_AkaoCmd.params[0] = 0;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
        busy = 0;
    }
    if (busy != 1) {
        if (slot == 1) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT2;
        }
        if (slot == 2) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT1;
        }
        if (slot == 3) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
        }
        if (slot == 4) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
        }
        g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
        g_AkaoCmd.params[1] = soundId;
        AkaoExec();
    }
    if (!busy) {
        if (slot == 1) {
            g_HighwaySfxCooldown1 = timer;
        }
        if (slot == 2) {
            g_HighwaySfxCooldown2 = timer;
        }
    }
    if (slot == 1 && timer < 0) {
        g_HighwaySfxCooldown1 = -timer;
    }
    if (slot == 2 && timer < 0) {
        g_HighwaySfxCooldown2 = -timer;
    }
}

void HighwaySetSlotPitch(s32 pitch, s32 slot) {
    if (slot == 1) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT2;
    }
    if (slot == 2) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT1;
    }
    if (slot == 3) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT0;
    }
    if (slot == 4) {
        g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT3;
    }
    g_AkaoCmd.params[0] = pitch;
    AkaoExec();
}

void HighwaySetSlotVolume(s32 volume, u8 slot) {
    if (slot == 1) {
        g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT2;
    }
    if (slot == 2) {
        g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT1;
    }
    if (slot == 3) {
        g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
    }
    if (slot == 4) {
        g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
    }
    g_AkaoCmd.params[0] = volume;
    AkaoExec();
}

void HighwaySetSlotPan(s32 pan, u8 slot) {
    if (slot == 1) {
        g_AkaoCmd.opcode = AKAO_SET_PAN_SLOT2;
    }
    if (slot == 2) {
        g_AkaoCmd.opcode = AKAO_SET_PAN_SLOT1;
    }
    if (slot == 3) {
        g_AkaoCmd.opcode = AKAO_SET_PAN_SLOT0;
    }
    if (slot == 4) {
        g_AkaoCmd.opcode = AKAO_SET_PAN_SLOT3;
    }
    g_AkaoCmd.params[0] = (s16)pan;
    AkaoExec();
}

void HighwayTrackAdvance(void) {
    s32 i;

    g_HighwayDistance += g_HighwaySpeed;
    g_HighwaySegmentFrac += g_HighwaySpeed;
    g_HighwaySegmentsCrossed = g_HighwaySegmentFrac >> 8;
    g_HighwaySegmentFrac &= 0xFF;
    g_HighwayTrackSegment += g_HighwaySegmentsCrossed;
    g_HighwayTrackPos = g_HighwayDistance + 0x2300;
    for (i = 0; i < g_HighwaySegmentsCrossed; i++) {
        HighwayTrackFreeSegment();
        HighwayTrackGenerateSegment();
    }
}

// Run the next scripted race event once the track has passed its segment; the list ends with segment 0xFFFF.
void HighwayEventsUpdate(void) {
    HighwayEvent* event;
    HighwayRiderState* state;

    if (g_HighwaySfxCooldown1 > 0) {
        g_HighwaySfxCooldown1--;
    }
    if (g_HighwaySfxCooldown2 > 0) {
        g_HighwaySfxCooldown2--;
    }
    event = &g_HighwayEvents[g_HighwayEventIndex];
    if (event->segment < g_HighwayTrackSegment - 45) {
        switch (event->code) {
        case 18:
            HighwayCameraSetPath(5, 4, 0x27100);
            break;
        case 5:
            g_HighwayCameraFixedOffset.vx = 90;
            g_HighwayCameraFixedOffset.vy = 180;
            g_HighwayCameraMode = 5;
            g_HighwayCameraFixedPos = g_HighwayTrackPos + 0x29E0;
            break;
        case 19:
            g_HighwayCameraFixedOffset.vx = 90;
            g_HighwayCameraFixedOffset.vy = 180;
            g_HighwayCameraMode = 5;
            g_HighwayCameraFixedPos = g_HighwayTrackPos + 0x14F0;
            break;
        case 6:
            g_HighwayCameraMode = 1;
            break;
        case 11:
            g_HighwayOverlayOn0 = 0;
            break;
        case 10:
            g_HighwayOverlayOn0 = 1;
            break;
        case 13:
            g_HighwayOverlayOn1 = 0;
            break;
        case 12:
            g_HighwayOverlayOn1 = 1;
            break;
        case 17:
            g_HighwayOverlayOn2 = 0;
            break;
        case 16:
            g_HighwayOverlayOn2 = 1;
            break;
        case 26:
            g_HighwayOverlayOn3 = 0;
            break;
        case 25:
            g_HighwayOverlayOn3 = 1;
            break;
        case 8:
            g_HighwayRiders[2].state.common.unk114 = 1;
            g_HighwayRiders[3].state.common.unk114 = 1;
            g_HighwayRiders[4].state.common.unk114 = 1;
            g_HighwayEnemiesDisabled = 1;
            break;
        case 14:
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0x126;
            AkaoExec();
            g_HighwayRiders[5].state.common.status = 0;
            break;
        case 20:
            state = &g_HighwayRiders[0].state;
            state->common.unk108 = 0;
            state->common.unk3C = -0xC800;
            state->common.unk40 = 0x38400;
            HighwayRiderSetPath(0, 7);
            state = &g_HighwayRiders[5].state;
            state->common.unk3C = 0xAA00;
            state->common.unk40 = 0x1F400;
            state->common.status = 6;
            HighwayRiderSetPath(5, 6);
            state = &g_HighwayRiders[1].state;
            state->common.unk3C = -0xB400;
            state->common.unk40 = 0x5DC00;
            state->common.status = 6;
            HighwayRiderSetPath(1, 8);
            HighwayCameraSetPath(2, 4, 0x10000);
            g_HighwayInputDisabled = 1;
            g_HighwayStoryEnding = 1;
            g_HighwayEndingTimer = 0;
            break;
        case 21:
            if (!g_HighwayArcadeFinish) {
                g_HighwayRiders[0].state.common.nodeCount--;
                g_HighwayRiders[0].state.common.unk108 = 0x4A;
                g_HighwayRiders[1].state.common.unkFC = 0x31;
                HighwayCameraSetPath(9, 4, 0x10000);
                g_HighwayInputDisabled = 1;
                g_HighwayArcadeFinish = 1;
                g_HighwayEndingTimer = 0;
                g_HighwayBanner = 2;
            }
            break;
        case 22:
            g_HighwayBanner = 1;
            break;
        case 23:
            g_HighwayBanner = 3;
            break;
        case 24:
            g_HighwayBanner = 0;
            break;
        case 28:
            g_HighwayRiders[0].state.common.status = 6;
            break;
        }
        g_HighwayEventIndex++;
    }
    if (g_HighwayArcadeFinish) {
        if (g_HighwayEndingTimer >= 6) {
            if (g_HighwaySpeed > 0) {
                g_HighwaySpeed -= 10;
            }
            if (g_HighwaySpeed < 0) {
                g_HighwaySpeed = 0;
            }
        }
        if (g_HighwayEndingTimer == 265) {
            g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
            g_AkaoCmd.params[0] = 0xF0;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SLIDE_ALL_VOL_BALANCE;
            g_AkaoCmd.params[0] = 0xF0;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
        }
        if (g_HighwayEndingTimer == 5) {
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
            g_AkaoCmd.params[0] = 0x7F;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
            g_AkaoCmd.params[0] = 0x7F;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT0;
            g_AkaoCmd.params[0] = 1;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT3;
            g_AkaoCmd.params[0] = 1;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0x241;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0x241;
            AkaoExec();
            g_HighwayRiders[0].state.common.z -= 0x2800;
            g_HighwayRiders[1].state.common.z += 0x6400;
        }
        if (g_HighwayEndingTimer == 25) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
            g_AkaoCmd.params[0] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
            g_AkaoCmd.params[0] = 0;
            AkaoExec();
        }
        if (g_HighwayEndingTimer == 330) {
            g_HighwayFadeMode = 2;
        }
        if (g_HighwayEndingTimer == 400) {
            g_HighwayExit = 1;
        }
        g_HighwayEndingTimer++;
    }
    if (g_HighwayStoryEnding) {
        if (g_HighwayEndingTimer >= 221) {
            g_HighwayRiders[5].state.common.unk12C++;
        }
        if (g_HighwayEndingTimer >= 181) {
            if (g_HighwaySpeed > 0) {
                g_HighwaySpeed -= 3;
            }
            if (g_HighwaySpeed < 0) {
                g_HighwaySpeed = 0;
            }
        }
        if (g_HighwayEndingTimer == 315) {
            g_AkaoCmd.opcode = AKAO_VOL_SLIDE_FROM_CURR;
            g_AkaoCmd.params[0] = 0xF0;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SLIDE_ALL_VOL_BALANCE;
            g_AkaoCmd.params[0] = 0xF0;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
        }
        if (g_HighwayEndingTimer == 184) {
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
            g_AkaoCmd.params[0] = 0x7F;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
            g_AkaoCmd.params[0] = 0x7F;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT0;
            g_AkaoCmd.params[0] = 1;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_PITCH_SLOT3;
            g_AkaoCmd.params[0] = 1;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0x241;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0x241;
            AkaoExec();
        }
        if (g_HighwayEndingTimer == 203) {
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT0;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_PLAY_SLOT3;
            g_AkaoCmd.params[0] = AKAO_PAN_CENTER;
            g_AkaoCmd.params[1] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT0;
            g_AkaoCmd.params[0] = 0;
            AkaoExec();
            g_AkaoCmd.opcode = AKAO_SET_VOL_BALANCE_SLOT3;
            g_AkaoCmd.params[0] = 0;
            AkaoExec();
        }
        if (g_HighwayEndingTimer == 380) {
            g_HighwayFadeMode = 2;
        }
        if (g_HighwayEndingTimer == 450) {
            g_HighwayExit = 1;
        }
        g_HighwayEndingTimer++;
    }
}
