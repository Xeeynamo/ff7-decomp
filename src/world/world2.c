#include "world.h"

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmLoadModelPacketAndScale);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCreatePacketForModelPart);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", func_800C02F4);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmLoadTexturesToVram);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelPacket2);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePacketForModelPartWithoutMatrixes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculateBoneMatrixes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmScaleModelAll);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmScaleModelVertexes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmScaleModelAnimations);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmApplyModelLightingToPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmApplyPolyLightingToPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculateModelLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmCalculatePartLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePartTransparency);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelPacket);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePacketForModelPartWithMatrixes);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdateModelLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmUpdatePartLighting);

INCLUDE_ASM("asm/us/world/nonmatchings/world2", WmGetModelTotalRenderPacketSize);
