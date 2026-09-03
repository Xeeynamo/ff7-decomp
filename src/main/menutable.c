//! PSYQ=3.3 CC1=2.7.2 G=8

#include "main_private.h"

// Likely plays a sound effect: writes a sound command (0x30) and the masked
// 16-bit sound id (arg0, duplicated into both parameter words) into the
// sound-request globals, then dispatches via SystemAkaoExecute.
// NOTE: SystemAkaoExecute's own body computes a value in $v0 before
// returning, so its game.h prototype has been corrected to `int`. Its other
// callers across the codebase still discard the result via a bare statement;
// propagating this same int-return pattern to those sibling wrappers may be a
// good change.
static int func_80026408(u16 arg0) {
    *D_8009A000 = 0x30;
    *D_8009A004 = arg0;
    *D_8009A008 = arg0;
    return SystemAkaoExecute();
}

void SysMenuSetCursorMovement(
    MenuTable* table, s32 column, s32 row, s32 numColumns, s32 numRowsPerPage, s32 unk0, s32 rowOffset, s32 unk4,
    s32 numTotalRows, s32 unkE, s32 unkF, s32 unk10, s32 unk11, u16 scrolling) {
    table->column = column;
    table->row = row;
    table->numColumns = numColumns;
    table->numRowsPerPage = numRowsPerPage;
    table->unk0 = unk0;
    table->rowOffset = rowOffset;
    table->unk4 = unk4;
    table->numTotalRows = numTotalRows;
    table->unkE = unkE;
    table->unkF = unkF;
    table->unk10 = unk10;
    table->unk11 = unk11;
    table->scrolling = scrolling;
}

INCLUDE_ASM("asm/us/main/nonmatchings/menutable", SysMenuHandleButtons);
