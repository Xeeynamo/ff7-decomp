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

void SysMenuHandleButtons(MenuTable* table) {
    if (table->scrolling == 0) {
        if (g_Pad0KeysRepeat & PAD_UP) {
            table->row--;
            switch (table->wrapModeY) {
            case 0:
                if (table->row < 0) {
                    table->row = 0;
                    if (table->rowOffset > 0) {
                        table->rowOffset--;
                        table->scrollAnimY = -3;
                        table->scrolling = 1;
                        func_80026408(1);
                    }
                } else {
                    func_80026408(1);
                }
                break;
            case 1:
            case 2:
                if (table->row < 0) {
                    table->row = table->numRowsPerPage - 1;
                }
                func_80026408(1);
                break;
            }
        } else if (g_Pad0KeysRepeat & PAD_DOWN) {
            table->row++;
            switch (table->wrapModeY) {
            case 0:
                if (table->row >= table->numRowsPerPage) {
                    table->row = table->numRowsPerPage - 1;
                    if (table->rowOffset < table->numTotalRows - table->numRowsPerPage) {
                        table->scrollAnimY = -1;
                        table->scrolling = 2;
                        func_80026408(1);
                    }
                } else {
                    func_80026408(1);
                }
                break;
            case 1:
            case 2:
                if (table->row >= table->numRowsPerPage) {
                    table->row = 0;
                }
                func_80026408(1);
                break;
            }
        } else if (g_Pad0KeysRepeat & PAD_LEFT) {
            switch (table->wrapModeX) {
            case 0:
                table->column--;
                if (table->column < 0) {
                    table->column = 0;
                } else {
                    func_80026408(1);
                }
                break;
            case 1:
                table->column--;
                if (table->column < 0) {
                    table->column = table->numColumns - 1;
                }
                func_80026408(1);
                break;
            case 2:
                if (table->column != 0 || table->row != 0 || table->rowOffset != 0) {
                    table->column--;
                    if (table->column < 0) {
                        table->column = table->numColumns - 1;
                        table->row--;
                        if (table->row < 0) {
                            table->row = 0;
                            if (table->rowOffset > 0) {
                                table->rowOffset--;
                                table->scrollAnimY = -3;
                                table->scrolling = 1;
                            }
                        }
                    }
                    func_80026408(1);
                }
                break;
            }
        } else if (g_Pad0KeysRepeat & PAD_RIGHT) {
            switch (table->wrapModeX) {
            case 0:
                table->column++;
                if (table->column >= table->numColumns) {
                    table->column = table->numColumns - 1;
                } else {
                    func_80026408(1);
                }
                break;
            case 1:
                table->column++;
                if (table->column >= table->numColumns) {
                    table->column = 0;
                }
                func_80026408(1);
                break;
            case 2:
                if (table->column != table->numColumns - 1 || table->row != table->numRowsPerPage - 1 ||
                    table->rowOffset != table->numTotalRows - table->numRowsPerPage) {
                    table->column++;
                    if (table->column >= table->numColumns) {
                        table->column = 0;
                        if (table->row >= table->numColumns) {
                            table->column = 0;
                        }
                        table->row++;
                        if (table->row >= table->numRowsPerPage) {
                            table->row = table->numRowsPerPage - 1;
                            if (table->rowOffset < table->numTotalRows - table->numRowsPerPage) {
                                table->scrollAnimY = -1;
                                table->scrolling = 2;
                            }
                        }
                    }
                    func_80026408(1);
                }
                break;
            }
        } else if (g_Pad0KeysRepeat & PAD_R1) {
            table->rowOffset += table->numRowsPerPage;
            if (table->rowOffset > table->numTotalRows - table->numRowsPerPage) {
                table->rowOffset = table->numTotalRows - table->numRowsPerPage;
            } else {
                func_80026408(1);
            }
        } else if (g_Pad0KeysRepeat & PAD_L1) {
            table->rowOffset -= table->numRowsPerPage;
            if (table->rowOffset < 0) {
                table->rowOffset = 0;
            } else {
                func_80026408(1);
            }
        }
    } else {
        switch (table->scrolling) {
        case 1:
            table->scrollAnimY++;
            if (table->scrollAnimY == 0) {
                table->scrolling = 0;
                table->scrollAnimY = 0;
            }
            break;
        case 2:
            table->scrollAnimY--;
            if (table->scrollAnimY == -4) {
                table->scrolling = 0;
                table->scrollAnimY = 0;
                table->rowOffset++;
            }
            break;
        }
    }
}
