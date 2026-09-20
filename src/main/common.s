# Shared .bss block for 0x80062ED0..0x80070800. See SBSS.md for the full story.
#
# One line per COMMON variable: the /* ADDR */ comment is the absolute address,
# glabel emits the symbol, and .space reserves its size. Addresses are fixed by
# the original image, so do not reorder, resize or rename anything here without
# re-checking against splat's disassembly (SBSS.md has the compare snippet).
#
# This file is built, despite what the map looks like. add_s_as() in
# tools/ninja/gen.py assembles it to build/us/asm/us/main/data/common.bss.s.o,
# the output path the generated linker script expects, from this source.
#
# Trailing comments name the candidate owning C file, inferred from
# %gp_rel(SYMBOL) references. "owner unknown" means no reference was found.

.include "macro.inc"

.section .bss, "wa"
.set push
.set noreorder
.align 2

/* 80062ED0 */ glabel D_80062ED0;                           .space 0x10
/* 80062EE0 */ glabel D_80062EE0;                           .space 4   
/* 80062EE4 */ glabel D_80062EE4;                           .space 0xC
/* 80062EF0 */ glabel D_80062EF0;                           .space 0x10
/* 80062F00 */ glabel g_AkaoStreamMask;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F04 */ glabel D_80062F04;                           .space 4   
/* 80062F08 */ glabel D_80062F08;                           .space 4   
/* 80062F0C */ glabel g_PartyMenuPreviousMenuId;            .space 4     # !!GP!! 1F6B4.c
/* 80062F10 */ glabel D_80062F10;                           .space 4     # !!GP!! 17238.c (SysGetMateriaActivatedStars)
/* 80062F14 */ glabel D_80062F14;                           .space 4     # !!GP!! 14C70.c (func_800155A4, func_800155B0)
/* 80062F18 */ glabel D_80062F18;                           .space 4   
/* 80062F1C */ glabel D_80062F1C;                           .space 2   
/* 80062F1E */ glabel D_80062F1E;                           .space 2   
/* 80062F20 */ glabel g_PartyMenuListTransitionFactor;      .space 4     # !!GP!! 1F6B4.c
/* 80062F24 */ glabel D_80062F24;                           .space 4   
/* 80062F28 */ glabel g_AkaoPitchMulMusicSlideStep;         .space 4   
/* 80062F2C */ glabel g_AkaoVolMulMusicSlideStep;           .space 4   
/* 80062F30 */ glabel g_AkaoTempoMulMusicSlideStep;         .space 4   
/* 80062F34 */ glabel D_80062F34;                           .space 0xC 
/* 80062F40 */ glabel g_AkaoPitchMulMusicSlideSteps;        .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F44 */ glabel g_AkaoVolMulMusicSlideSteps;          .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F48 */ glabel g_AkaoTempoMulMusicSlideSteps;        .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F4C */ glabel D_80062F4C;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F50 */ glabel D_80062F50;                           .space 8     # !!GP!! 1F6B4.c
/* 80062F58 */ glabel g_MenuRenderBufferIndex;              .space 4     # !!GP!! 1F6B4.c
/* 80062F5C */ glabel g_AkaoVolMulMusic;                    .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F60 */ glabel D_80062F60;                           .space 4     # !!GP!! 14C70.c (func_800155B0)
/* 80062F64 */ glabel D_80062F64;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F68 */ glabel D_80062F68;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F6C */ glabel D_80062F6C;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F70 */ glabel g_AkaoReverbPan;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F74 */ glabel g_AkaoEffectsAll;                           .space 4     # !!GP!! akao.c (func_8002988C, func_80029998)
/* 80062F78 */ glabel D_80062F78;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F7C */ glabel D_80062F7C;                           .space 4     # !!GP!! 17238.c
/* 80062F80 */ glabel D_80062F80;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F84 */ glabel g_AkaoEffectsAllSeq;                           .space 4     # !!GP!! akao.c (func_8002988C)
/* 80062F88 */ glabel D_80062F88;                           .space 4     # !!GP!! btlinit.c (func_800148A0)
/* 80062F8C */ glabel g_AkaoMutex;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062F90 */ glabel D_80062F90;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F94 */ glabel g_PartyMenuSelectedMenuId;            .space 4     # !!GP!! 1F6B4.c
/* 80062F98 */ glabel D_80062F98;                           .space 4     # !!GP!! 1F6B4.c
/* 80062F9C */ glabel D_80062F9C;                           .space 4     # !!GP!! 1CDA4.c
/* 80062FA0 */ glabel g_TutorialActive;                     .space 4   
/* 80062FA4 */ glabel D_80062FA4;                           .space 8
/* 80062FAC */ glabel D_80062FAC;                           .space 4   
/* 80062FB0 */ glabel D_80062FB0;                           .space 4   
/* 80062FB4 */ glabel g_AkaoCdVolSlideStep;                 .space 4   
/* 80062FB8 */ glabel g_AkaoReverbMul;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062FBC */ glabel D_80062FBC;                           .space 4     # !!GP!! 17238.c
/* 80062FC0 */ glabel D_80062FC0;                           .space 4     # !!GP!! 1F6B4.c
/* 80062FC4 */ glabel D_80062FC4;                           .space 4     # !!GP!! 26B70.c (SysMenuSetOtag, SysMenuSetDrawenv, ...)
/* 80062FC8 */ glabel D_80062FC8;                           .space 4     # akao.c (func_8002B5A8, func_8002B608)
/* 80062FCC */ glabel g_AkaoCdVolSlideSteps;                .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062FD0 */ glabel D_80062FD0;                           .space 4
/* 80062FD4 */ glabel g_AkaoCdVol;                          .space 4     # !!GP!! akao.c (AkaoUpdateCdVolume, func_800294BC)
/* 80062FD8 */ glabel g_AkaoMuteMusicMask;                           .space 8     # !!GP!! akao.c (func_800294BC)
/* 80062FE0 */ glabel D_80062FE0;                           .space 4     # akao.c (AkaoStreamIrqCallback*)
/* 80062FE4 */ glabel g_AkaoPitchMulMusic;                  .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062FE8 */ glabel g_AkaoTempoMulMusic;                  .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062FEC */ glabel D_80062FEC;                           .space 4     # !!GP!! 1F6B4.c (SysMenuDrawBattleResult, func_800212A8)
/* 80062FF0 */ glabel D_80062FF0;                           .space 4     # !!GP!! 1CDA4.c (SysMenuDrawDialogString)
/* 80062FF4 */ glabel D_80062FF4;                           .space 4     # !!GP!! 1F6B4.c (SysMenuShow, SysMenuDrawMainMenu)
/* 80062FF8 */ glabel g_AkaoControlFlags;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80062FFC */ glabel D_80062FFC;                           .space 4     # !!GP!! 17238.c (SysAddMateria*)
/* 80063000 */ glabel D_80063000;                           .space 4     # akao.c (AkaoStreamIrqCallback*)
/* 80063004 */ glabel D_80063004;                           .space 4     # akao.c (AkaoStreamTransferCallback*)
/* 80063008 */ glabel D_80063008;                           .space 4     # !!GP!! 26B70.c (func_800269D0, func_800269E8)
/* 8006300C */ glabel D_8006300C;                           .space 4     # !!GP!! 26B70.c (func_80026A0C, func_80026A20)
/* 80063010 */ glabel g_AkaoCommandQueueId;                           .space 4     # !!GP!! akao.c (func_800294BC)
/* 80063014 */ glabel g_CurrentAction;                      .space 4     # !!GP!! btlinit.c (func_800148B4)
/* 80063018 */ glabel D_80063018;                           .space 4     # !!GP!! 1F6B4.c (func_800212A8, func_80021F58)
/* 8006301C */ glabel D_8006301C;                           .space 4     # !!GP!! 1F6B4.c (SysMenuDrawMainMenu)
/* 80063020 */ glabel D_80063020;                           .space 4     # !!GP!! 17238.c (SysAddMateria*, SysParseMateriaEquip)
/* 80063024 */ glabel D_80063024;                           .space 4     # !!GP!! 1F6B4.c (SysMenuShow)
/* 80063028 */ glabel transform_matrix;                     .space 0x20
/* 80063048 */ glabel D_80063048;                           .space 0x518
/* 80063560 */ glabel D_80063560;                           .space 0x100
/* 80063660 */ glabel D_80063660;                           .space 0x30
/* 80063690 */ glabel g_KernelTextBuffer;                   .space 0x5dec  # 14C70.c (SysGetPointerToTextInKernWithBlockAndTextId, func_80014C80)
/* 8006947C */ glabel D_8006947C;                           .space 0x10
/* 8006948C */ glabel D_8006948C;                           .space 0x4
/* 80069490 */ glabel g_KernelTextBlockOffsets;             .space 0xc  # 14C70.c (SysGetPointerToTextInKernWithBlockAndTextId, func_80014C80)
/* 8006949C */ glabel D_8006949C;                           .space 0x18
/* 800694B4 */ glabel D_800694B4;                           .space 0xf
/* 800694C3 */ glabel D_800694C3;                           .space 0x1
/* 800694C4 */ glabel D_800694C4;                           .space 0x10
/* 800694D4 */ glabel D_800694D4;                           .space 0x10
/* 800694E4 */ glabel D_800694E4;                           .space 0x2
/* 800694E6 */ glabel D_800694E6;                           .space 0x2
/* 800694E8 */ glabel D_800694E8;                           .space 0x2
/* 800694EA */ glabel D_800694EA;                           .space 0x2
/* 800694EC */ glabel D_800694EC;                           .space 0x2
/* 800694EE */ glabel D_800694EE;                           .space 0x2
/* 800694F0 */ glabel D_800694F0;                           .space 0x2
/* 800694F2 */ glabel D_800694F2;                           .space 0x2
/* 800694F4 */ glabel D_800694F4;                           .space 0x2
/* 800694F6 */ glabel D_800694F6;                           .space 0x2
/* 800694F8 */ glabel D_800694F8;                           .space 0x2
/* 800694FA */ glabel D_800694FA;                           .space 0x2
/* 800694FC */ glabel D_800694FC;                           .space 0x2
/* 800694FE */ glabel D_800694FE;                           .space 0x2
/* 80069500 */ glabel D_80069500;                           .space 0x2
/* 80069502 */ glabel D_80069502;                           .space 0x2
/* 80069504 */ glabel D_80069504;                           .space 0x2
/* 80069506 */ glabel D_80069506;                           .space 0x2
/* 80069508 */ glabel D_80069508;                           .space 0x1
/* 80069509 */ glabel D_80069509;                           .space 0x1
/* 8006950A */ glabel D_8006950A;                           .space 0x2e
/* 80069538 */ glabel D_80069538;                           .space 0x2
/* 8006953A */ glabel D_8006953A;                           .space 0x2
/* 8006953C */ glabel D_8006953C;                           .space 0x2
/* 8006953E */ glabel D_8006953E;                           .space 0x2
/* 80069540 */ glabel D_80069540;                           .space 0x2
/* 80069542 */ glabel D_80069542;                           .space 0x2
/* 80069544 */ glabel D_80069544;                           .space 0x2
/* 80069546 */ glabel D_80069546;                           .space 0x2
/* 80069548 */ glabel D_80069548;                           .space 0x2
/* 8006954A */ glabel D_8006954A;                           .space 0x2
/* 8006954C */ glabel D_8006954C;                           .space 0x2
/* 8006954E */ glabel D_8006954E;                           .space 0x2
/* 80069550 */ glabel D_80069550;                           .space 0x2
/* 80069552 */ glabel D_80069552;                           .space 0x2
/* 80069554 */ glabel D_80069554;                           .space 0x1
/* 80069555 */ glabel D_80069555;                           .space 0x1
/* 80069556 */ glabel D_80069556;                           .space 0x1
/* 80069557 */ glabel D_80069557;                           .space 0x1
/* 80069558 */ glabel D_80069558;                           .space 0x114
/* 8006966C */ glabel D_8006966C;                           .space 0x20
/* 8006968C */ glabel D_8006968C;                           .space 0x20
/* 800696AC */ glabel s_PadBuffers;                         .space 0x44  # input.c (InputInit, InputReadPad1Raw, InputReadPadsRaw)
/* 800696F0 */ glabel D_800696F0;                           .space 0xc   # 1CDA4.c (SysMenuStoreWindowColor, SysMenuRestoreWindowColor)
/* 800696FC */ glabel D_800696FC;                           .space 0x50
/* 8006974C */ glabel g_MenuOrderingTables;                 .space 0xa0
/* 800697EC */ glabel g_RewardMenuTable;                    .space 0x2
/* 800697EE */ glabel D_800697EE;                           .space 0x2
/* 800697F0 */ glabel D_800697F0;                           .space 0x2
/* 800697F2 */ glabel D_800697F2;                           .space 0x2
/* 800697F4 */ glabel D_800697F4;                           .space 0x2
/* 800697F6 */ glabel D_800697F6;                           .space 0x1
/* 800697F7 */ glabel D_800697F7;                           .space 0x1
/* 800697F8 */ glabel D_800697F8;                           .space 0x1
/* 800697F9 */ glabel D_800697F9;                           .space 0x1
/* 800697FA */ glabel D_800697FA;                           .space 0x1
/* 800697FB */ glabel D_800697FB;                           .space 0x1
/* 800697FC */ glabel D_800697FC;                           .space 0x1
/* 800697FD */ glabel D_800697FD;                           .space 0x3
/* 80069800 */ glabel D_80069800;                           .space 0x30
/* 80069830 */ glabel D_80069830;                           .space 0x1
/* 80069831 */ glabel D_80069831;                           .space 0x1
/* 80069832 */ glabel D_80069832;                           .space 0x1
/* 80069833 */ glabel D_80069833;                           .space 0x1
/* 80069834 */ glabel D_80069834;                           .space 0x1
/* 80069835 */ glabel D_80069835;                           .space 0xf
/* 80069844 */ glabel D_80069844;                           .space 0xa0
/* 800698E4 */ glabel D_800698E4;                           .space 0x1
/* 800698E5 */ glabel D_800698E5;                           .space 0x1
/* 800698E6 */ glabel D_800698E6;                           .space 0x1
/* 800698E7 */ glabel D_800698E7;                           .space 0x1
/* 800698E8 */ glabel D_800698E8;                           .space 0x4  # 33B70.c (SysCdromStartLoadLzs)
/* 800698EC */ glabel D_800698EC;                           .space 0x4
/* 800698F0 */ glabel D_800698F0;                           .space 0x7  # 33B70.c (ReadDiskNo, SysCdromStartLoadLzs)
/* 800698F7 */ glabel D_800698F7;                           .space 0x47f9  # 33B70.c (ReadDiskNo)
/* 8006E0F0 */ glabel D_8006E0F0;                           .space 0x4  # 33B70.c (func_80034A90)
/* 8006E0F4 */ glabel D_8006E0F4;                           .space 0x4  # 33B70.c (func_80034A58, func_80034A90)
/* 8006E0F8 */ glabel D_8006E0F8;                           .space 0x4
/* 8006E0FC */ glabel D_8006E0FC;                           .space 0x4
/* 8006E100 */ glabel D_8006E100;                           .space 0x4
/* 8006E104 */ glabel D_8006E104;                           .space 0x4
/* 8006E108 */ glabel D_8006E108;                           .space 0x4
/* 8006E10C */ glabel D_8006E10C;                           .space 0x4
/* 8006E110 */ glabel D_8006E110;                           .space 0x4
/* 8006E114 */ glabel D_8006E114;                           .space 0x4
/* 8006E118 */ glabel D_8006E118;                           .space 0x4
/* 8006E11C */ glabel D_8006E11C;                           .space 0x4
/* 8006E120 */ glabel D_8006E120;                           .space 0x8
/* 8006E128 */ glabel D_8006E128;                           .space 0x8
/* 8006E130 */ glabel D_8006E130;                           .space 0x8
/* 8006E138 */ glabel D_8006E138;                           .space 0x4
/* 8006E13C */ glabel D_8006E13C;                           .space 0x4
/* 8006E140 */ glabel D_8006E140;                           .space 0x4
/* 8006E144 */ glabel D_8006E144;                           .space 0x1
/* 8006E145 */ glabel D_8006E145;                           .space 0x1
/* 8006E146 */ glabel D_8006E146;                           .space 0x2
/* 8006E148 */ glabel D_8006E148;                           .space 0x4
/* 8006E14C */ glabel D_8006E14C;                           .space 0x5d4
/* 8006E720 */ glabel D_8006E720;                           .space 0x24
/* 8006E744 */ glabel D_8006E744;                           .space 0x4
/* 8006E748 */ glabel D_8006E748;                           .space 0x4
/* 8006E74C */ glabel D_8006E74C;                           .space 0x4
/* 8006E750 */ glabel D_8006E750;                           .space 0x15f4
/* 8006FD44 */ glabel D_8006FD44;                           .space 0x8c
/* 8006FDD0 */ glabel D_8006FDD0;                           .space 0x774
/* 80070544 */ glabel D_80070544;                           .space 0x4
/* 80070548 */ glabel D_80070548;                           .space 0x4
/* 8007054C */ glabel D_8007054C;                           .space 0x4
/* 80070550 */ glabel D_80070550;                           .space 0x4
/* 80070554 */ glabel D_80070554;                           .space 0x4
/* 80070558 */ glabel D_80070558;                           .space 0x4
/* 8007055C */ glabel D_8007055C;                           .space 0x4
/* 80070560 */ glabel D_80070560;                           .space 0x4
/* 80070564 */ glabel D_80070564;                           .space 0x4
/* 80070568 */ glabel D_80070568;                           .space 0x4
/* 8007056C */ glabel D_8007056C;                           .space 0x4
/* 80070570 */ glabel D_80070570;                           .space 0x4
/* 80070574 */ glabel D_80070574;                           .space 0x4
/* 80070578 */ glabel D_80070578;                           .space 0x4
/* 8007057C */ glabel D_8007057C;                           .space 0x14
/* 80070590 */ glabel D_80070590;                           .space 0x100
/* 80070690 */ glabel D_80070690;                           .space 0x4
/* 80070694 */ glabel D_80070694;                           .space 0x4
/* 80070698 */ glabel D_80070698;                           .space 0x4
/* 8007069C */ glabel D_8007069C;                           .space 0x4
/* 800706A0 */ glabel D_800706A0;                           .space 0x4
/* 800706A4 */ glabel D_800706A4;                           .space 0x5c  # 1F6B4.c (func_80024A04)
/* 80070700 */ glabel D_80070700;                           .space 0x5c  # 1F6B4.c (func_80024A04)
/* 8007075C */ glabel D_8007075C;                           .space 0x30  # 1F6B4.c (func_80024A04)
/* 8007078C */ glabel g_EntityToLine;                       .space 0x30
/* 800707BC */ glabel D_800707BC;                           .space 0x2  # 110B8.c (func_800111E4)
/* 800707BE */ glabel g_BattleMode;                         .space 0x2  # 110B8.c (func_800111E4)
/* 800707C0 */ glabel D_800707C0;                           .space 0x4  # 1CDA4.c (SysMenuDrawDialogString); 26B70.c (SysGetSingleStringWidth)
/* 800707C4 */ glabel D_800707C4;                           .space 0x1  # 144D8.c
/* 800707C5 */ glabel D_800707C5;                           .space 0x3b

.set pop

