//! PSYQ=3.3 CC1=2.7.2 G=8

#include "main_private.h"

// Likely plays a sound effect: writes a sound command (0x30) and the masked
// 16-bit sound id (arg0, duplicated into both parameter words) into the
// sound-request globals, then dispatches via AkaoExec.
// NOTE: AkaoExec's own body computes a value in $v0 before
// returning, so its game.h prototype has been corrected to `int`. Its other
// callers across the codebase still discard the result via a bare statement;
// propagating this same int-return pattern to those sibling wrappers may be a
// good change.
static void func_80026408(u16 arg0) {
    g_AkaoCmd.opcode = AKAO_PLAY_MENU_SOUND;
    g_AkaoCmd.params[0] = arg0;
    g_AkaoCmd.params[1] = arg0;
    AkaoExec();
}

void SysMenuSetCursorMovement(MenuTable* table, s32 column, s32 row, s32 numColumns, s32 numRowsPerPage, s32 colOffset,
                              s32 rowOffset, s32 numTotalColumns, s32 numTotalRows, s32 scrollAnimX, s32 scrollAnimY,
                              s32 wrapModeX, s32 wrapModeY, u16 scrolling) {
    table->column = column;
    table->row = row;
    table->numColumns = numColumns;
    table->numRowsPerPage = numRowsPerPage;
    table->colOffset = colOffset;
    table->rowOffset = rowOffset;
    table->numTotalColumns = numTotalColumns;
    table->numTotalRows = numTotalRows;
    table->scrollAnimX = scrollAnimX;
    table->scrollAnimY = scrollAnimY;
    table->wrapModeX = wrapModeX;
    table->wrapModeY = wrapModeY;
    table->scrolling = scrolling;
}

INCLUDE_ASM("asm/us/main/nonmatchings/menutable", SysMenuHandleButtons);
