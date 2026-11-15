//! PSYQ=3.6 G=8
#include "main_private.h"

static const u8 magic_sort_order[6][4] = {
    {0, 1, 2, 3}, {0, 2, 1, 3}, {2, 0, 1, 3}, {1, 0, 2, 3}, {1, 2, 0, 3}, {2, 1, 0, 3},
};

void SysSortMagicInUnitStructure(s32 partyId) {
    u8 grid[4][24]; // menu layout: 4 magic categories of 8 rows of 3 slots, reordered by the config option
    u8 owned[NUM_MAGICS];
    u8 slotOf[NUM_MAGICS];
    MagicRecord copy[NUM_MAGICS];
    MagicRecord* magic;
    s32 id;
    u8 magicId;
    s32 i;
    s32 j;
    s32 k;
    u8* row;
    u8* cell;
    s32 col;
    s32 order;

    magic = g_ActiveCharacters[partyId].enabledMagic;
    order = (Savemap.config >> 10) & 7; // magic order option of the config menu
    for (i = 0; i < NUM_MAGICS; i++) {
        copy[i] = magic[i];
        if (magic[i].id != 0xFF) {
            slotOf[magic[i].id] = i;
        }
        magic[i].id = 0xFF;
    }
    for (i = 0; i < LEN(grid); i++) {
        for (j = 0; j < LEN(grid[0]); j++) {
            grid[i][j] = 0xFF;
        }
    }
    for (i = 0; i < LEN(D_800831C4); i++) {
        grid[magic_sort_order[order][D_800831C4[i] >> 5]][D_800831C4[i] & 0x1F] = i;
    }
    for (i = 0; i < NUM_MAGICS; i++) {
        owned[i] = 0xFF;
    }
    for (i = 0; i < NUM_MAGICS; i++) {
        magicId = copy[i].id;
        if (magicId != 0xFF) {
            owned[magicId] = magicId;
        }
    }
    for (i = 0; i < LEN(grid); i++) {
        for (j = 0; j < LEN(grid[0]); j++) {
            if (grid[i][j] != 0xFF) {
                grid[i][j] = owned[grid[i][j]];
            }
        }
    }
    k = 0;
    for (i = 0; i < LEN(grid); i++) {
        row = grid[i];
        for (j = 0; j < LEN(grid[0]); j += 3) {
            cell = row + j;
            if ((cell[0] & cell[1] & cell[2]) != 0xFF) {
                for (col = 0; col < 3; col++) {
                    if (cell[col] != 0xFF) {
                        magic[k] = copy[slotOf[cell[col]]];
                    }
                    k++;
                }
            }
        }
    }
    // enabledMagic holds the magics, then the summons; id is the attack id (summons follow the magics)
    for (i = 0; i < NUM_MAGICS + NUM_SUMMONS; i++) {
        id = magic[i].id;
        if (id != 0xFF) {
            if (i >= NUM_MAGICS) {
                id += NUM_MAGICS;
            }
            if (id == 0x19 || id == 0x47) { // Escape, Knights of Round
                magic[i].allAttacksLeft = 0;
            }
        }
    }
}

// called from the FIRE overlay, so it cannot be static
void func_8001C3C4(void) {}

INCLUDE_ASM("asm/us/main/nonmatchings/1C0EC", func_8001C3CC);
