//! PSYQ=3.3 COMM=true

#include "highway_private.h"

HighwayRider g_HighwayRiders[6];
u8 g_HighwayArcadeMode;
s32 g_HighwayInputDisabled;
s32 g_HighwayEffectModels[4];
SVECTOR* g_HighwayPath;
s32 g_HighwayPathLen;
u8 g_HighwayPropPatternCount;
s32 g_HighwayOtOffset;
u8* g_HighwayPropData;
s32 g_HighwayPadKeys;
s32 g_HighwayNearestEnemy;
s32 g_HighwayNearestEnemyDist;
s16 g_HighwayPropRepeat[10];
s32 g_HighwayRidersTrackPos;
u8* g_HighwayPropPatterns[256];
SVECTOR* g_HighwayCameraPath;
s32 g_HighwayPadDir;
u8 g_HighwayEnemyCount;
s32 g_HighwayCameraPathEnd;
s32 g_HighwayPadAction;
s32 g_HighwayCameraPathStep;
u8 g_HighwayEnemySpawnDelay;
HighwayBuffer* g_HighwayRidersBuffer;
SVECTOR g_HighwayCameraFixedOffset;
u32 g_HighwayCameraPathPos;
VECTOR g_HighwayCameraOffset;
s32* D_800BE550;
HighwayPropScript* g_HighwayPropCurrent;
s32 g_HighwayCameraLift;
u16 g_HighwayPropYaw[10];
u8 g_HighwayPropPatternLen[10];
s32 g_HighwayNearestRiderDist;
u8 g_HighwayPropNeedNext[10];
HighwayPropScript* g_HighwayPropScripts[10];
s32 g_HighwayCameraYaw;
s16 g_HighwayPropRecord[10];
u16 g_HighwayPropOffset[10];
u8 g_HighwayCameraMode;
s32 D_80110ABC;
u16 g_HighwayPropFlags[10];
s32 g_HighwayEngineVolume;
s32 g_HighwayEnemyEngineVolume;
u8 g_HighwayPropPatternPos[10];
s32* D_801163F8;
u16 g_HighwayPropPattern[10];
u16 g_HighwayPropHeight[10];

inline void HighwayRiderReset(s32 index);
inline s32 HighwayRiderAngle(HighwayRider* a, HighwayRider* b);
inline s32 HighwayRiderDistance(HighwayRider* a, HighwayRider* b);

void HighwayRidersInit(void) {
    g_HighwayEnemySpawnDelay = 0x78;
    g_HighwayEffectModels[0] = 0x8E;
    g_HighwayEffectModels[1] = 0x95;
    g_HighwayEffectModels[2] = 0x94;
    g_HighwayEnemyCount = 0;
    g_HighwayEffectModels[3] = 0xB1;
    HighwayRiderReset(0);
    HighwayRiderReset(1);
    HighwayRiderReset(2);
    HighwayRiderReset(3);
    HighwayRiderReset(4);
    HighwayRiderReset(5);
    HighwayRidersSetup();
}

void HighwayRidersUpdate(s32 trackPos, HighwayBuffer* db) {
    g_HighwayRidersBuffer = db;
    g_HighwayRidersTrackPos = trackPos;
    HighwayEnemiesSpawn();
    HighwayRidersMove();
    HighwayRidersAction();
    HighwayRidersClamp();
    HighwayRidersDraw();
    HighwayRidersDrawEffects();
    HighwayKawaiModelsUpdate();
    HighwayRidersCollide();
}

// Clear rider record index; state 5 marks it inactive.
inline void HighwayRiderReset(s32 index) {
    HighwayRiderState* state;
    s32 i;

    state = &g_HighwayRiders[index].state;
    g_HighwayRiders[index].unk0 = 0;
    g_HighwayRiders[index].unk4 = 0;
    g_HighwayRiders[index].unk8 = 0;
    g_HighwayRiders[index].unk10.vx = 0;
    g_HighwayRiders[index].unk10.vy = 0;
    g_HighwayRiders[index].unk10.vz = 0;
    for (i = LEN(state->raw) - 1; i >= 0; i--) {
        state->raw[i] = 0;
    }
    state->common.status = 5;
}

void HighwayRidersSetup(void) {
    s32* params;

    HighwayRiderReset(0);
    params = g_HighwayRiders[0].state.raw;
    params[0] = 0;
    params[1] = 0;
    params[5] = 4;
    params[4] = 2;
    params[2] = 1;
    params[3] = 0;
    params[6] = 0;
    params[8] = 0x3E800;
    params[11] = 0xDAC00;
    params[14] = 0x1F400;
    params[18] = 0;
    params[19] = 70;
    params[20] = 70;
    params[21] = 70;
    params[24] = 20;
    params[25] = 100;
    params[26] = 36;
    params[27] = 36;
    params[31] = 48;
    params[32] = 48;
    params[40] = 1;
    g_HighwayRiders[0].nodes[0] = HighwayNodeAlloc(0x61, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayRiders[0].nodes[1] = HighwayNodeAlloc(0x9B, 0, 0, 1, g_HighwayRiders[0].nodes[0], 0, -10, 85, 0, 0, 0);
    g_HighwayRiders[0].nodes[2] = HighwayNodeAlloc(0x95, 0, 0, 1, g_HighwayRiders[0].nodes[0], -80, 0, -40, 0, 0, 0);
    g_HighwayRiders[0].nodes[3] = HighwayNodeAlloc(0x61, 0, 0, 1, g_HighwayRiders[0].nodes[0], 0, 0, 0, 0, 0, 0);

    HighwayRiderReset(1);
    params = g_HighwayRiders[1].state.raw;
    params[0] = 0;
    params[1] = 1;
    params[5] = 3;
    params[4] = 3;
    if (!g_HighwayArcadeMode) {
        params[2] = 2;
        params[3] = 13;
    } else {
        params[2] = 17;
        params[3] = 19;
    }
    params[6] = 0;
    params[8] = 0x9C400;
    params[11] = 0x9C400;
    params[14] = 0x5DC00;
    params[18] = 0;
    params[19] = 70;
    params[20] = 75;
    params[40] = 1;
    params[30] = 0x800;
    params[31] = 500;
    params[24] = -30;
    params[25] = 100;
    params[26] = 75;
    params[27] = 75;
    g_HighwayRiders[1].nodes[0] = HighwayNodeAlloc(0x61, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayRiders[1].nodes[1] = HighwayNodeAlloc(0x99, 0, 0, 1, g_HighwayRiders[1].nodes[0], 0, 0, 0, 0, 0, 0);
    g_HighwayRiders[1].nodes[2] = HighwayNodeAlloc(0x9A, 0, 0, 1, g_HighwayRiders[1].nodes[0], 0, 3, -6, 0, 0, 0);

    HighwayRiderReset(5);
    params = g_HighwayRiders[5].state.raw;
    params[0] = 5;
    params[1] = 10;
    params[5] = 2;
    params[4] = 1;
    params[2] = 12;
    params[3] = 19;
    params[6] = 0x3E800;
    params[8] = -7000;
    params[11] = 0x5DC00;
    params[14] = 0x1F400;
    params[18] = 0;
    params[19] = 70;
    params[20] = 70;
    params[24] = -80;
    params[25] = 75;
    params[26] = 132;
    params[27] = 240;
    params[30] = 0x100;
    params[32] = 0x1000;
    params[31] = 250;
    params[33] = 30;
    params[34] = 9999;
    params[35] = 9999;
    params[40] = 1;
    g_HighwayRiders[5].nodes[0] = HighwayNodeAlloc(0x61, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayRiders[5].nodes[1] = HighwayNodeAlloc(0x61, 0, 0, 1, g_HighwayRiders[5].nodes[0], 0, 0, 0, 0, 0, 0);
}

void HighwayEnemyInit(s32 index) {
    s32* params;

    params = g_HighwayRiders[index].state.raw;
    params[1] = 2;
    params[11] = 0xBB800;
    params[14] = 0x1F400;
    params[5] = 3;
    g_HighwayEnemyCount++;
    params[19] = 70;
    params[4] = 2;
    params[20] = 70;
    params[18] = 0;
    params[40] = 1;
    params[24] = 20;
    params[25] = 100;
    params[26] = 36;
    params[27] = 36;
    params[30] = 0x400;
    params[32] = 0x400;
    params[31] = 450;
    params[33] = 20;
    params[0] = 5;
    g_HighwayRiders[index].nodes[0] = HighwayNodeAlloc(0x61, 0, 0, 1, &g_HighwayRootNode, 0, 0, 0, 0, 0, 0);
    g_HighwayRiders[index].nodes[1] =
        HighwayNodeAlloc(0x9B, 0, 0, 1, g_HighwayRiders[index].nodes[0], 0, 0, 120, 0, 0, 0);
    g_HighwayRiders[index].nodes[2] =
        HighwayNodeAlloc(0x94, 0, 0, 1, g_HighwayRiders[index].nodes[0], 0, 0, 0, 0, 0, 0);
    switch (index) {
    case 2:
        params[6] = -0xC800;
        params[8] = -0xBB800;
        break;
    case 3:
        params[6] = 0;
        params[8] = -0xBB800;
        break;
    case 4:
        params[6] = 0xC800;
        params[8] = -0xBB800;
        break;
    }
}

void func_800A7A48(void) {}

void HighwayRidersClamp(void) {
    HighwayRiderClamp(0);
    HighwayRiderClamp(1);
    if (g_HighwayRiders[2].state.common.status != 5) {
        HighwayRiderClamp(2);
    }
    if (g_HighwayRiders[3].state.common.status != 5) {
        HighwayRiderClamp(3);
    }
    if (g_HighwayRiders[4].state.common.status != 5) {
        HighwayRiderClamp(4);
    }
    if (g_HighwayRiders[5].state.common.status != 5) {
        HighwayRiderClamp(5);
    }
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRiderClamp);

void HighwayEnemiesSpawn(void) {
    if (!g_HighwayEnemiesDisabled && !g_HighwayEnemySpawnDelay) {
        if (rand() % 60 == 0 && g_HighwayRiders[2].state.common.status == 5) {
            HighwayEnemySpawn(2);
        }
        if (rand() % 60 == 0 && g_HighwayRiders[3].state.common.status == 5) {
            HighwayEnemySpawn(3);
        }
        if (rand() % 60 == 0 && g_HighwayRiders[4].state.common.status == 5) {
            HighwayEnemySpawn(4);
        }
    }
    if (g_HighwayEnemySpawnDelay) {
        g_HighwayEnemySpawnDelay--;
    }
}

void HighwayEnemySpawn(s32 index) {
    HighwayRiderState* reset;
    HighwayRiderState* state;
    s32 i;
    s32 r;
    s32 unk8;

    reset = &g_HighwayRiders[index].state;
    g_HighwayRiders[index].unk0 = 0;
    g_HighwayRiders[index].unk4 = 0;
    g_HighwayRiders[index].unk8 = 0;
    g_HighwayRiders[index].unk10.vx = 0;
    g_HighwayRiders[index].unk10.vy = 0;
    g_HighwayRiders[index].unk10.vz = 0;
    for (i = LEN(reset->raw) - 1; i >= 0; i--) {
        reset->raw[i] = 0;
    }
    reset->common.status = 5;
    HighwayEnemyInit(index);
    state = &g_HighwayRiders[index].state;
    r = rand() % 3;
    switch (r) {
    case 0:
        state->common.hp = 5;
        state->common.maxHp = 5;
        unk8 = index + 1;
        break;
    case 1:
        unk8 = index + 7;
        state->common.hp = 60;
        state->common.maxHp = 60;
        break;
    case 2:
        unk8 = index + 12;
        state->common.hp = 30;
        state->common.maxHp = 30;
        break;
    default:
        unk8 = index + 1;
        state->common.hp = 40;
        state->common.maxHp = 40;
        break;
    }
    state = &g_HighwayRiders[index].state;
    state->common.unkC = index + 4;
    state->common.status = 0;
    state->common.unk8 = unk8;
    state->common.unk90 = r + 10;
}

void func_800A7F48(s32 index) {
    HighwayRiderState* state;
    s32 i;

    state = &g_HighwayRiders[index].state;
    g_HighwayKawaiModels[state->common.unk8].flags = 0;
    g_HighwayKawaiModels[state->common.unkC].flags = 0;
    state->common.status = 5;
    for (i = 0; i < state->common.unk14; i++) {
        HighwayNodeFree(g_HighwayRiders[index].nodes[i]);
    }
}

void HighwayRidersMove(void) {
    if (g_HighwayRiders[0].state.common.status != 5) {
        HighwayRiderMove(0);
    }
    if (g_HighwayRiders[1].state.common.status != 5) {
        HighwayRiderMove(1);
    }
    if (g_HighwayRiders[5].state.common.status != 5) {
        HighwayRiderMove(5);
    }
    if (g_HighwayRiders[2].state.common.status != 5) {
        HighwayEnemyMove(2);
    }
    if (g_HighwayRiders[3].state.common.status != 5) {
        HighwayEnemyMove(3);
    }
    if (g_HighwayRiders[4].state.common.status != 5) {
        HighwayEnemyMove(4);
    }
}

void HighwayRiderMove(s32 index) {
    VECTOR pos;
    VECTOR ahead;
    HighwayRiderState* state;
    s32 angle;

    state = &g_HighwayRiders[index].state;
    switch (state->common.type) {
    case 0:
        if (!state->common.onPath) {
            if (!state->common.attackTimer) {
                if (g_HighwayPadAction == 1) {
                    state->common.attackTimer = 29;
                }
                if (g_HighwayPadAction == 2) {
                    state->common.attackTimer = -29;
                }
            }
            if (g_HighwayPadDir == 1) {
                angle = 0xA00;
            }
            if (g_HighwayPadDir == 2) {
                angle = 0xC00;
            }
            if (g_HighwayPadDir == 3) {
                angle = 0xE00;
            }
            if (g_HighwayPadDir == 4) {
                angle = 0x800;
            }
            if (g_HighwayPadDir == 6) {
                angle = 0;
            }
            if (g_HighwayPadDir == 7) {
                angle = 0x600;
            }
            if (g_HighwayPadDir == 8) {
                angle = 0x400;
            }
            if (g_HighwayPadDir == 9) {
                angle = 0x200;
            }
            if (state->common.status == 6) {
                g_HighwayInputDisabled = 1;
                if (state->common.x > 0xB400) {
                    g_HighwayPadDir = 4;
                    angle = 0x800;
                }
                if (state->common.x < 0xA500) {
                    g_HighwayPadDir = 6;
                    angle = 0;
                }
            }
            if (g_HighwayPadDir) {
                state->common.velX += (rcos(angle) * 0x658) >> 12;
                state->common.velZ += (rsin(angle) * 0x658) >> 11;
            }
            state->common.velX = state->common.velX * 8 / 10;
            state->common.velZ = state->common.velZ * 8 / 10;
            state->common.x += state->common.velX;
            state->common.z += state->common.velZ;
            if (!g_HighwayPadDir) {
                if (state->common.unk10C >= 0x400) {
                    state->common.unk10C -= 0xC00;
                }
                if (state->common.unk10C < -0x3FF) {
                    state->common.unk10C += 0xC00;
                }
                if (state->common.unk10C > -0x400 && state->common.unk10C < 0x400) {
                    state->common.unk10C = 0;
                }
            } else {
                state->common.unk10C += rcos(angle) >> 1;
                if (state->common.unk10C > 0x4400) {
                    state->common.unk10C = 0x4400;
                }
                if (state->common.unk10C < -0x4400) {
                    state->common.unk10C = -0x4400;
                }
                state->common.unkC8 = ABS(state->common.unk10C >> 9);
            }
        } else {
            HighwayPathSample(state->common.pathPos, state->common.path, &pos, 1);
            HighwayPathSample(state->common.pathPos + 0x80000, state->common.path, &ahead, 1);
            if (state->common.unk44 < pos.vx) {
                state->common.unk110 += state->common.unk7C * 2;
            }
            if (state->common.unk44 > pos.vx) {
                state->common.unk110 -= state->common.unk7C * 2;
            }
            if (state->common.unk110 > 0x200) {
                state->common.unk110 = 0x200;
            }
            if (state->common.unk110 < -0x200) {
                state->common.unk110 = -0x200;
            }
            state->common.x = (pos.vx << 7) + state->common.unk3C;
            state->common.z = (pos.vz << 7) + state->common.unk40;
            state->common.unk44 = pos.vx;
            if ((state->common.pathPos += 0x10000) > state->common.pathEnd) {
                state->common.onPath = 0;
                state->common.status = 0;
                g_HighwayRiders[0].state.common.unk108 = 0x49;
                g_HighwayRiders[1].state.common.unkFC = 0x31;
                g_HighwayRiders[0].state.common.nodeCount--;
            }
        }
        break;
    case 1:
        if (!state->common.onPath) {
            if (state->common.unkFC) {
                state->common.unk7C = 0;
            }
            if (state->common.z > state->common.maxZ) {
                state->common.unkF4 = 1;
                state->common.unkF8 = 0xC00;
            }
            if (state->common.z < state->common.minZ) {
                state->common.unkF4 = 1;
                state->common.unkF8 = 0x400;
            }
            if (!state->common.unkF4) {
                if (rand() % 4096 < state->common.unk78) {
                    state->common.unkF4 = 10;
                    state->common.unkF8 = rand() % 4096;
                    state->common.velX = state->common.velX * 8 / 10;
                    state->common.velZ = state->common.velZ * 8 / 10;
                }
            } else {
                state->common.unkF4--;
                state->common.velX += (rcos(state->common.unkF8) * state->common.unk7C) >> 12;
                state->common.velZ += (rsin(state->common.unkF8) * state->common.unk7C) >> 11;
                state->common.x += state->common.velX;
                state->common.z += state->common.velZ;
                state->common.unk3C = state->common.x;
                state->common.unk40 = state->common.z;
                state->common.velX = state->common.velX * 8 / 10;
                state->common.velZ = state->common.velZ * 8 / 10;
            }
        } else if (state->common.onPath == 1) {
            HighwayPathSample(state->common.pathPos, state->common.path, &pos, 1);
            state->common.x = (pos.vx << 7) + state->common.unk3C;
            state->common.z = (pos.vz << 7) + state->common.unk40;
            if ((state->common.pathPos += 0x10000) > state->common.pathEnd) {
                state->common.onPath = 2;
            }
        }
        break;
    case 10:
        if (!state->common.onPath) {
            if (rand() % 30 == 0) {
                if (!state->common.unk10C) {
                    HighwayPlaySfx(SFX_HIGHWAY_SMACK, 2, 0);
                }
                state->common.unk10C = 3;
            }
            if (state->common.z > state->common.maxZ) {
                state->common.unkF8 = 1;
                state->common.unkFC = 0xC00;
            }
            if (state->common.z < state->common.minZ) {
                state->common.unkF8 = 1;
                state->common.unkFC = 0x400;
            }
            if (!state->common.unkF8) {
                if (rand() % 4096 < state->common.unk78) {
                    if (rand() % 4096 < state->common.unk80) {
                        state->common.unkFC = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[1]);
                    } else {
                        state->common.unkFC = rand() % 4096;
                    }
                    state->common.unkF8 = rand() % (state->common.unk84 + 1);
                }
            } else {
                state->common.unkF8--;
                state->common.velX += (rcos(state->common.unkFC) * state->common.unk7C) >> 12;
                state->common.velZ += (rsin(state->common.unkFC) * state->common.unk7C) >> 11;
            }
            state->common.velX = state->common.velX * 8 / 10;
            state->common.velZ = state->common.velZ * 8 / 10;
            state->common.x += state->common.velX;
            state->common.z += state->common.velZ;
            state->common.unk3C = state->common.x;
            state->common.unk40 = state->common.z;
        } else {
            if (state->common.onPath == 1) {
                HighwayPathSample(state->common.pathPos, state->common.path, &pos, 1);
                state->common.x = (pos.vx << 7) + state->common.unk3C;
                state->common.z = (pos.vz << 7) + state->common.unk40;
                if ((state->common.pathPos += 0x10000) > state->common.pathEnd) {
                    state->common.onPath = 2;
                }
            }
            if (state->common.unk12C) {
                state->common.unkC0 = 0;
                state->common.unkBC = 1;
                state->common.unkC4 = (state->common.unk12C >> 2) % 11;
            }
        }
        break;
    }
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayEnemyMove);

void HighwayRidersAction(void) {
    if (g_HighwayRiders[0].state.common.status != 5) {
        HighwayRiderAction(0);
    }
    if (g_HighwayRiders[1].state.common.status != 5) {
        HighwayRiderAction(1);
    }
    if (g_HighwayRiders[2].state.common.status != 5) {
        HighwayRiderAction(2);
    }
    if (g_HighwayRiders[3].state.common.status != 5) {
        HighwayRiderAction(3);
    }
    if (g_HighwayRiders[4].state.common.status != 5) {
        HighwayRiderAction(4);
    }
    if (g_HighwayRiders[5].state.common.status != 5) {
        HighwayRiderAction(5);
    }
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRiderAction);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRidersDraw);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRiderDraw);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRidersDrawEffects);

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRiderDrawEffects);

// Angle on the XZ plane from rider a to rider b.
inline s32 HighwayRiderAngle(HighwayRider* a, HighwayRider* b) {
    s32 dx;
    s32 dz;

    dx = b->state.common.x - a->state.common.x;
    dz = b->state.common.z - a->state.common.z;
    return ratan2(dz, dx);
}

// Distance between the nodes of rider a and rider b.
inline s32 HighwayRiderDistance(HighwayRider* a, HighwayRider* b) {
    JetNode* na;
    JetNode* nb;
    s32 dx;
    s32 dy;
    s32 dz;

    nb = b->nodes[0];
    na = a->nodes[0];
    dx = nb->m.t[0] - na->m.t[0];
    dy = nb->m.t[1] - na->m.t[1];
    dz = nb->m.t[2] - na->m.t[2];
    return SquareRoot0(dx * dx + dy * dy + dz * dz);
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayRiderEndpoints);

void HighwayRidersCollide(void) {
    VECTOR pos;
    SVECTOR rot;
    SVECTOR a0;
    SVECTOR a1;
    SVECTOR b0;
    SVECTOR b1;
    s16 ha;
    s16 hb;
    HighwayRiderState* sa;
    HighwayRiderState* sb;
    s32 radius;
    s32 angle;
    s16 i;
    s16 j;
    s32 count;

    for (i = 0, count = LEN(g_HighwayRiders); i < count; i++) {
        sa = &g_HighwayRiders[i].state;
        if (sa->common.status == 5) {
            continue;
        }
        HighwayRiderEndpoints(i, &a0, &a1, &ha);
        for (j = i + 1; j < count; j++) {
            sb = &g_HighwayRiders[j].state;
            if (sb->common.status == 5) {
                continue;
            }
            HighwayRiderEndpoints(j, &b0, &b1, &hb);
            HighwayTrackSample(g_HighwayRidersTrackPos + sb->common.z, sb->common.x, &pos, &rot);
            angle = 0xFFFF;
            radius = sa->common.unk68 + sb->common.unk68 + sa->common.unkC8 + sb->common.unkC8;
            if (SquareRoot0((b0.vx - a0.vx) * (b0.vx - a0.vx) + (b0.vy - a0.vy) * (b0.vy - a0.vy) +
                            (b0.vz - a0.vz) * (b0.vz - a0.vz)) < radius) {
                angle = ratan2(b0.vz - a0.vz, b0.vx - a0.vx) - 0x800 + rot.vy;
            }
            if (angle == 0xFFFF) {
                radius = sa->common.unk6C + sb->common.unk68 + sa->common.unkC8 + sb->common.unkC8;
                if (SquareRoot0((b0.vx - a1.vx) * (b0.vx - a1.vx) + (b0.vy - a1.vy) * (b0.vy - a1.vy) +
                                (b0.vz - a1.vz) * (b0.vz - a1.vz)) < radius) {
                    angle = ratan2(b0.vz - a1.vz, b0.vx - a1.vx) - 0x800 + rot.vy;
                }
            }
            if (angle == 0xFFFF) {
                radius = sa->common.unk68 + sb->common.unk6C + sa->common.unkC8 + sb->common.unkC8;
                if (SquareRoot0((b1.vx - a0.vx) * (b1.vx - a0.vx) + (b1.vy - a0.vy) * (b1.vy - a0.vy) +
                                (b1.vz - a0.vz) * (b1.vz - a0.vz)) < radius) {
                    angle = ratan2(b1.vz - a0.vz, b1.vx - a0.vx) - 0x800 + rot.vy;
                }
            }
            if (angle == 0xFFFF) {
                radius = sa->common.unk6C + sb->common.unk6C + sa->common.unkC8 + sb->common.unkC8;
                if (SquareRoot0((b1.vx - a1.vx) * (b1.vx - a1.vx) + (b1.vy - a1.vy) * (b1.vy - a1.vy) +
                                (b1.vz - a1.vz) * (b1.vz - a1.vz)) < radius) {
                    angle = ratan2(b1.vz - a1.vz, b1.vx - a1.vx) - 0x800 + rot.vy;
                }
            }
            if (angle != 0xFFFF) {
                HighwayRiderCollision(i, j, angle);
            }
        }
    }
}

void HighwayRiderCollision(s16 a, s16 b, s32 angle) {
    s32 playSfx;
    s32 sn;
    s32 cs;
    s32 x;
    s32 z;

    playSfx = 0;
    if (!a) {
        playSfx = b != 1;
    }
    if (a == 1 && b != a) {
        playSfx = 1;
    }
    if (playSfx == 1) {
        HighwayPlaySfx(SFX_HOLLOW_METAL, 2, 5);
        HighwaySetSlotVolume(0x6F, 2);
        HighwaySetSlotPitch(0, 2);
    }
    angle %= 0x1000;
    if (angle < 0) {
        angle += 0x1000;
    }
    sn = rsin(angle) * 5000;
    z = sn >> 11;
    cs = rcos(angle) * 5000;
    x = cs >> 12;
    if (a) {
        g_HighwayRiders[a].state.common.velX = x;
        g_HighwayRiders[a].state.common.velZ = z;
    } else {
        g_HighwayRiders[0].state.common.velX = cs >> 13;
        g_HighwayRiders[0].state.common.velZ = sn >> 12;
    }
    g_HighwayRiders[b].state.common.velX = -x;
    g_HighwayRiders[b].state.common.velZ = -z;
    if (a >= 2 && b == 5) {
        g_HighwayRiders[a].state.common.velX = x * 2;
        g_HighwayRiders[a].state.common.velZ = z * 2;
        g_HighwayRiders[5].state.common.velX = 0;
        g_HighwayRiders[5].state.common.velZ = 0;
    }
    if (g_HighwayRiders[b].state.common.status == 2 && g_HighwayRiders[a].state.common.status != 2 && a >= 2 &&
        b >= 2) {
        HighwayRiderDamage(a, 15);
    }
    if (g_HighwayRiders[a].state.common.status == 2 && g_HighwayRiders[b].state.common.status != 2 && a >= 2 &&
        b >= 2) {
        HighwayRiderDamage(b, 15);
    }
    if (a == 1 && b) {
        HighwayTruckHit(b, angle);
    }
    if (!a) {
        if (b != 1) {
            HighwayGaugeDamage(0, 0x40);
        }
        if (g_HighwayRiders[b].state.common.status != 2 && b != 1) {
            HighwayRiderDamage(b, 1);
        }
    }
    if (b == 5) {
        HighwayRiderDamage(a, 20);
    }
}

void HighwayRiderDamage(s32 index, s32 damage) {
    HighwayRiderState* state;

    state = &g_HighwayRiders[index].state;
    switch (state->common.type) {
    case 1:
        HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[1]);
        state->common.status = 1;
        state->common.unk100 = 18;
        break;
    case 2:
        if (state->common.hp <= 0) {
            g_HighwayScore += 500;
            HighwayPlaySfx(SFX_HIGHWAY_ROAD_SCRAPE, 1, 10);
            state->common.status = 2;
            state->common.unk118 = rand() % 3;
            state->common.unk108 = 59;
            state->common.unk110 = 0;
            state->common.nodeCount--;
            if (g_HighwayEnemyCount != 1 && g_HighwayCameraMode == 1) {
                HighwayCameraSetPath(rand() % 2 + 3, 4, 40000);
            }
            g_HighwayEnemyCount--;
        } else {
            state->common.hp -= damage;
            state->common.status = 1;
            state->common.attackTimer = 28;
            if (g_HighwayRiders[index].state.common.unkC != 19) {
                g_HighwayKawaiStates[g_HighwayRiders[index].state.common.unkC].flash = 0xFF;
            }
        }
        break;
    case 0:
        break;
    }
}

void HighwayTruckHit(s32 index, s32 angle) {
    if (g_HighwayScore) {
        g_HighwayScore -= 50;
    }
    angle += 0x800;
    angle %= 0x1000;
    if (angle >= 0 && angle < 0x400) {
        HighwayGaugeDamage(2, 400);
    }
    if (angle >= 0x400 && angle < 0x800) {
        HighwayGaugeDamage(3, 400);
    }
    if (angle >= 0x800 && angle < 0xC00) {
        HighwayGaugeDamage(1, 400);
    }
    if (angle >= 0xC00 && angle < 0x1000) {
        HighwayGaugeDamage(4, 400);
    }
    g_HighwayKawaiStates[g_HighwayRiders[1].state.common.unk8].flash = 0xFF;
}

void HighwayRiderSetPath(s32 index, u8 pathIndex) {
    HighwayRiderState* state;

    state = &g_HighwayRiders[index].state;
    HighwayPathLoad(pathIndex);
    state->common.onPath = 1;
    state->common.pathPos = 0;
    state->common.path = g_HighwayPath;
    state->common.pathEnd = ((g_HighwayPathLen - 1) << 16) - 1;
}

void HighwayEnemyAi(s32 index) {
    HighwayRiderState* state;
    s32 speed;
    s32 toTarget;
    s32 toLeader;
    s32 dist;
    s32 i;
    HighwayRiderState* other;

    state = &g_HighwayRiders[index].state;
    if (state->common.unk114) {
        speed = 4000;
        state->common.unkFC = 0xC00;
        state->common.velX = ((rcos(state->common.unkFC) * speed) >> 12) + state->common.velX;
        state->common.velZ = ((rsin(state->common.unkFC) * speed) >> 11) + state->common.velZ;
        state->common.velX = state->common.velX * 8 / 10;
        state->common.velZ = state->common.velZ * 8 / 10;
        state->common.x += state->common.velX;
        state->common.z += state->common.velZ;
        if (state->common.z < -5000) {
            g_HighwayKawaiModels[state->common.unk8].flags = 0;
            g_HighwayKawaiModels[state->common.unkC].flags = 0;
            state->common.status = 5;
            other = state;
            for (i = 0; i < other->common.unk14; i++) {
                HighwayNodeFree(g_HighwayRiders[index].nodes[i]);
            }
        }
        return;
    }
    switch (state->common.unk90) {
    case 10:
        speed = state->common.status != 1 ? 1000 : 0;
        toTarget = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[1]);
        toLeader = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[0]);
        HighwayRiderDistance(&g_HighwayRiders[index], &g_HighwayRiders[1]);
        dist = HighwayRiderDistance(&g_HighwayRiders[index], &g_HighwayRiders[0]);
        switch (g_HighwayEnemyCount) {
        case 1:
            state->common.unkFC = toTarget;
        case 2:
            state->common.unkFC = toTarget;
        case 3:
            speed = 1000;
            state->common.unkFC = toTarget;
            break;
        }
        if (dist < 500 && g_HighwayRiders[0].state.common.attackTimer) {
            speed = 1300;
            state->common.unkFC = toLeader + 0x800;
        }
        if (state->common.minZ > state->common.z) {
            speed = 2000;
            state->common.unkFC = 0x400;
        }
        state->common.velX = ((rcos(state->common.unkFC) * speed) >> 12) + state->common.velX;
        state->common.velZ = ((rsin(state->common.unkFC) * speed) >> 11) + state->common.velZ;
        state->common.velX = state->common.velX * 8 / 10;
        state->common.velZ = state->common.velZ * 8 / 10;
        state->common.x += state->common.velX;
        state->common.z += state->common.velZ;
        break;
    case 11:
        speed = state->common.status != 1 ? 800 : 0;
        state->common.unkFC = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[1]);
        if (state->common.minZ > state->common.z) {
            speed = 2000;
            state->common.unkFC = 0x400;
        }
        state->common.velX = ((rcos(state->common.unkFC) * speed) >> 12) + state->common.velX;
        state->common.velZ = ((rsin(state->common.unkFC) * speed) >> 11) + state->common.velZ;
        state->common.velX = state->common.velX * 8 / 10;
        state->common.velZ = state->common.velZ * 8 / 10;
        state->common.x += state->common.velX;
        state->common.z += state->common.velZ;
        break;
    case 12:
        speed = state->common.status != 1 ? 800 : 0;
        toTarget = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[1]);
        toLeader = HighwayRiderAngle(&g_HighwayRiders[index], &g_HighwayRiders[0]);
        if (!state->common.unk11C && !(rand() % 100)) {
            state->common.unk11C = rand() % 90;
        }
        if (!state->common.unk11C) {
            state->common.unkFC = toTarget;
        } else {
            state->common.unkFC = toLeader + 0x200;
            state->common.unk11C--;
        }
        if (state->common.minZ > state->common.z) {
            speed = 2000;
            state->common.unkFC = 0x400;
        }
        state->common.velX = ((rcos(state->common.unkFC) * speed) >> 12) + state->common.velX;
        state->common.velZ = ((rsin(state->common.unkFC) * speed) >> 11) + state->common.velZ;
        state->common.velX = state->common.velX * 8 / 10;
        state->common.velZ = state->common.velZ * 8 / 10;
        state->common.x += state->common.velX;
        state->common.z += state->common.velZ;
        break;
    }
    if (state->common.z > state->common.maxZ) {
        state->common.z = state->common.maxZ;
    }
}

void HighwayPropsInit(void) {
    s32 offset;
    s32 i;

    if (!g_HighwayArcadeMode) {
        g_HighwayPropData = g_HighwayCourses[0].propData;
        g_HighwayPropScripts[0] = g_HighwayCourses[0].propScripts[0];
        g_HighwayPropScripts[1] = g_HighwayCourses[0].propScripts[1];
        g_HighwayPropScripts[2] = g_HighwayCourses[0].propScripts[2];
        g_HighwayPropScripts[3] = g_HighwayCourses[0].propScripts[3];
        g_HighwayPropScripts[4] = g_HighwayCourses[0].propScripts[4];
        g_HighwayPropScripts[5] = g_HighwayCourses[0].propScripts[5];
        g_HighwayPropScripts[6] = g_HighwayCourses[0].propScripts[6];
        g_HighwayPropScripts[7] = g_HighwayCourses[0].propScripts[7];
        g_HighwayPropScripts[8] = g_HighwayCourses[0].propScripts[8];
        g_HighwayPropScripts[9] = g_HighwayCourses[0].propScripts[9];
    } else {
        g_HighwayPropData = g_HighwayCourses[1].propData;
        g_HighwayPropScripts[0] = g_HighwayCourses[1].propScripts[0];
        g_HighwayPropScripts[1] = g_HighwayCourses[1].propScripts[1];
        g_HighwayPropScripts[2] = g_HighwayCourses[1].propScripts[2];
        g_HighwayPropScripts[3] = g_HighwayCourses[1].propScripts[3];
        g_HighwayPropScripts[4] = g_HighwayCourses[1].propScripts[4];
        g_HighwayPropScripts[5] = g_HighwayCourses[1].propScripts[5];
        g_HighwayPropScripts[6] = g_HighwayCourses[1].propScripts[6];
        g_HighwayPropScripts[7] = g_HighwayCourses[1].propScripts[7];
        g_HighwayPropScripts[8] = g_HighwayCourses[1].propScripts[8];
        g_HighwayPropScripts[9] = g_HighwayCourses[1].propScripts[9];
    }
    g_HighwayPropPatternCount = *g_HighwayPropData;
    offset = g_HighwayPropPatternCount + 1;
    for (i = 0; i < g_HighwayPropPatternCount; i++) {
        if (g_HighwayArcadeMode == 0) {
            g_HighwayPropPatterns[i] =
                g_HighwayCourses[0].propData + g_HighwayPropData[offset] + (g_HighwayPropData[offset + 1] << 8);
        }
        if (g_HighwayArcadeMode == 1) {
            g_HighwayPropPatterns[i] =
                g_HighwayCourses[1].propData + g_HighwayPropData[offset] + (g_HighwayPropData[offset + 1] << 8);
        }
        offset += 2;
    }
    for (i = 0; i < LEN(g_HighwayPropRecord); i++) {
        g_HighwayPropRecord[i] = 0;
        g_HighwayPropNeedNext[i] = 1;
    }
}

void HighwayPropScriptStep(u8 index, u8* modelId, s16* offset, s16* height, u16* flags, u16* yaw) {
    u8 a;
    u8 b;
    u8 c;
    HighwayPropScript* rec;

    if (g_HighwayPropNeedNext[index]) {
        rec = &g_HighwayPropScripts[index][g_HighwayPropRecord[index]];
        g_HighwayPropCurrent = rec;
        g_HighwayPropRecord[index]++;
        g_HighwayPropPattern[index] = rec->pattern;
        g_HighwayPropRepeat[index] = rec->repeat;
        g_HighwayPropOffset[index] = rec->offset;
        g_HighwayPropHeight[index] = rec->height;
        g_HighwayPropFlags[index] = rec->flags;
        g_HighwayPropYaw[index] = rec->yaw;
        g_HighwayPropPatternLen[index] = g_HighwayPropData[rec->pattern + 1];
        g_HighwayPropPatternPos[index] = 0;
        g_HighwayPropNeedNext[index] = 0;
    }
    HighwayPropPatternGet(g_HighwayPropPattern[index], g_HighwayPropPatternPos[index], &a, &b, &c);
    g_HighwayPropPatternPos[index]++;
    if (g_HighwayPropPatternPos[index] == g_HighwayPropPatternLen[index]) {
        g_HighwayPropPatternPos[index] = 0;
        if (!--g_HighwayPropRepeat[index]) {
            g_HighwayPropNeedNext[index] = 1;
        }
    }
    *modelId = a;
    *offset = g_HighwayPropOffset[index];
    *height = g_HighwayPropHeight[index];
    *flags = g_HighwayPropFlags[index];
    *yaw = g_HighwayPropYaw[index];
}

void HighwayPropPatternGet(u8 table, u8 index, u8* first, u8* second, u8* unused) {
    u8* entry;

    entry = &g_HighwayPropPatterns[table][index * 3];
    *first = entry[0];
    *second = entry[1];
}

void HighwayCameraInit(void) {
    g_HighwayCameraMode = 1;
    g_HighwayCameraEye.vx = 0;
    g_HighwayCameraEye.vy = 0;
    g_HighwayCameraEye.vz = -2000;
    g_HighwayCameraTarget.vx = 0;
    g_HighwayCameraTarget.vy = 0;
    g_HighwayCameraTarget.vz = 0;
    g_HighwayCameraTargetOffset.vx = 0;
    g_HighwayCameraTargetOffset.vy = 0;
    g_HighwayCameraTargetOffset.vz = 0;
    g_HighwayCameraEyeOffset.vx = 0;
    g_HighwayCameraEyeOffset.vy = 0;
    g_HighwayCameraEyeOffset.vz = -500;
    g_HighwayCameraLift = 0;
    g_HighwayCameraFixedPos = 0;
    g_HighwayCameraFixedOffset.vx = 0;
    g_HighwayCameraFixedOffset.vy = 0;
    g_HighwayCameraFixedOffset.vz = 0;
    D_801163F8 = g_HighwayPathLengths;
    D_800BE550 = g_HighwayPathOffsets;
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayCameraUpdate);

void func_800AC2FC(void) {}

void HighwayCameraSetPath(s32 pathIndex, u8 mode, s32 position) {
    g_HighwayCameraMode = mode;
    HighwayPathLoad(pathIndex);
    g_HighwayCameraPathPos = 0;
    g_HighwayCameraPathStep = position;
    g_HighwayCameraPath = g_HighwayPath;
    g_HighwayCameraPathEnd = ((g_HighwayPathLen - 1) << 16) - 1;
}

// Load path pathIndex: like the path part of JetObjectPathLoad.
void HighwayPathLoad(u8 pathIndex) {
    s32* lengths;
    s32* offsets;
    s32 offset;

    lengths = g_HighwayPathLengths;
    offsets = g_HighwayPathOffsets;
    g_HighwayPathLen = lengths[pathIndex];
    offset = offsets[pathIndex];
    g_HighwayPath = (SVECTOR*)(g_HighwayPathData + offset);
}

// Sample a path at a 16.16 position, like JetPathSample.
void HighwayPathSample(u32 pathPosition, SVECTOR* path, VECTOR* position, u8 flag) {
    SVECTOR seg[2];
    VECTOR delta;
    s32 idx;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 frac;

    idx = pathPosition >> 16;
    frac = pathPosition & 0xFFFF;
    seg[0].vx = path[idx].vx;
    seg[0].vy = path[idx].vy;
    seg[0].vz = path[idx].vz;
    seg[1].vx = path[idx + 1].vx;
    seg[1].vy = path[idx + 1].vy;
    seg[1].vz = path[idx + 1].vz;
    dx = seg[1].vx - seg[0].vx;
    dy = seg[1].vy - seg[0].vy;
    dz = seg[1].vz - seg[0].vz;
    delta.vx = dx;
    delta.vy = dy;
    delta.vz = dz;
    delta.vx = frac * dx;
    delta.vy = frac * dy;
    delta.vz = frac * dz;
    delta.vx = delta.vx >> 16;
    delta.vy = delta.vy >> 16;
    delta.vz = delta.vz >> 16;
    if (!flag) {
        position->vx = -path[idx].vx - delta.vx;
        position->vy = -path[idx].vy - delta.vy;
        position->vz = -path[idx].vz - delta.vz;
    } else {
        position->vx = -path[idx].vx - delta.vx;
        position->vy = path[idx].vy + delta.vy;
        position->vz = path[idx].vz + delta.vz;
    }
}

void HighwayInputReset(void) {
    D_80110ABC = 0;
    g_HighwayPadDir = 0;
    g_HighwayPadAction = 0;
    g_HighwayInputDisabled = 0;
}

INCLUDE_ASM("asm/us/mini/highway/nonmatchings/highway_rider", HighwayInputUpdate);
