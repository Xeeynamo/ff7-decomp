//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include "../magic/magic.h"
#include <libc.h>
#include <libetc.h>

void BattleTriggerActorFlashMode0(s32 arg0);
void BattleTriggerActorFlashMode1(s32 arg0);
void BattleSpawnActorRampEffect(s32 arg0, s16 arg1, s16 arg2);
void BattleSpawnPartEffect(s32 actor, s32 hitFlashType);

extern Yamada D_800EEBB8[]; // MAGIC/*.BIN overlay

s32 BattleMovementRegister(void (*f)(void));
s32 BattleDetachedRegister(void (*f)());
void func_800C2928();
void func_800C328C();
void func_800C3578();
void func_800C3950();
void func_800C3CA8();
void func_800C40F4();
void func_800C44B4();
void func_800C4814();
static void BattleModelMoveTick();
void func_800D1530();
s32 BattleModelReadAnimStream(BattleModelSub* arg0, s32 arg1, s16 nItems, u8* arg3);
void BattleSpawnFloatingIcon(s32 actor, s32 arg1);
void BattleQueueImpactEffect(s32 arg0, s16 arg1);
void BattleInitMagicCastEffect(void);
void BattleStartEffectWithModel(s32 targetMask, s32 callbackArg);

// MAGIC/ entrypoints that are not named yet
void func_801B037C(s16, u8);
void func_801B0000(s16, u8);
void func_801B0000_2(s16, u8);
void func_801B000C(s16, u8);
void func_801B0040(s16, u8);
void func_801B0054(s16, u8);
void func_801B0084(s16, u8);

// Effect dispatch, indexed by attackEffectId within one currentActionId.
// The id table gives a D_800EEBB8 file id; the entrypoint table, the call into it.
// clang-format off
// currentActionId 4 -- items: overlay file id per slot.
s16 D_800EF5B0[] = {
    0,   //   0 CAREL.BIN
    0,   //   1 CAREL.BIN
    0,   //   2 CAREL.BIN
    0,   //   3 CAREL.BIN
    0,   //   4 CAREL.BIN
    0,   //   5 CAREL.BIN
    0,   //   6 CAREL.BIN
    0,   //   7 CAREL.BIN
    0,   //   8 CAREL.BIN
    16,  //   9 THREAT.BIN
    65,  //  10 TODO.BIN
    64,  //  11 MINIMAM.BIN
    299, //  12 ECHOES.BIN
    139, //  13 HAKAI.BIN
    47,  //  14 TEARS.BIN
    45,  //  15 ALMIGHTY.BIN
    300, //  16 SMKBALL.BIN
    50,  //  17 HEIST.BIN
    81,  //  18 MYTYG.BIN
    36,  //  19 RESIST.BIN
    1,   //  20 FIRE.BIN
    155, //  21 RENBAKU.BIN
    154, //  22 ZENBAKU.BIN
    55,  //  23 STOP.BIN
    85,  //  24 DEATH.BIN
    10,  //  25 SLOW.BIN
    49,  //  26 SLIPL.BIN
    54,  //  27 SAILESS.BIN
    14,  //  28 BERSERK.BIN
    86,  //  29 CONF.BIN
    2,   //  30 FAIRA.BIN
    77,  //  31 FAIGA2.BIN
    8,   //  32 BRIZARA.BIN
    255, //  33 BRIZAG2.BIN
    4,   //  34 THUNDERA.BIN
    6,   //  35 THUNDG2.BIN
    51,  //  36 QUEIR.BIN
    52,  //  37 QUEIG.BIN
    29,  //  38 BIORA.BIN
    32,  //  39 BIOGA2.BIN
    30,  //  40 COMETEO.BIN
    104, //  41 Q_KETSU.BIN
    108, //  42 Q_SHU.BIN
    33,  //  43 BREAK.BIN
    11,  //  44 BARRIER.BIN
    12,  //  45 MABARIA.BIN
    13,  //  46 REFREC.BIN
    63,  //  47 DETHPER.BIN
    152, //  48 DAISENP.BIN
    68,  //  49 AQUA.BIN
    65,  //  50 TODO.BIN
    64,  //  51 MINIMAM.BIN
    35,  //  52 ESNA.BIN
    1,   //  53 FIRE.BIN
    168, //  54 HAKUGEK2.BIN
    173, //  55 NAPALM.BIN
    9,   //  56 GRAVIDE.BIN
    24,  //  57 GRAVIRA.BIN
    105, //  58 SUMI.BIN
    90,  //  59 HARI1000.BIN
    183, //  60 TBREATH.BIN
    71,  //  61 KUSAI.BIN
    180, //  62 VEGET.BIN
    180, //  63 VEGET.BIN
    180, //  64 VEGET.BIN
    180, //  65 VEGET.BIN
    180, //  66 VEGET.BIN
    180, //  67 VEGET.BIN
    180, //  68 VEGET.BIN
    180  //  69 VEGET.BIN
};

// currentActionId 2 -- magic spells: overlay file id per slot.
s16 D_800EF63C[] = {
    0,   //   0 CAREL.BIN
    0,   //   1 CAREL.BIN
    0,   //   2 CAREL.BIN
    0,   //   3 CAREL.BIN
    0,   //   4 CAREL.BIN
    35,  //   5 ESNA.BIN
    36,  //   6 RESIST.BIN
    84,  //   7 RAISE.BIN
    84,  //   8 RAISE.BIN
    64,  //   9 MINIMAM.BIN
    65,  //  10 TODO.BIN
    49,  //  11 SLIPL.BIN
    86,  //  12 CONF.BIN
    54,  //  13 SAILESS.BIN
    14,  //  14 BERSERK.BIN
    11,  //  15 BARRIER.BIN
    12,  //  16 MABARIA.BIN
    13,  //  17 REFREC.BIN
    308, //  18 WALL.BIN
    50,  //  19 HEIST.BIN
    10,  //  20 SLOW.BIN
    55,  //  21 STOP.BIN
    61,  //  22 DEBARIA.BIN
    63,  //  23 DETHPER.BIN
    85,  //  24 DEATH.BIN
    34,  //  25 ESCAPE.BIN
    22,  //  26 DEJON.BIN
    1,   //  27 FIRE.BIN
    2,   //  28 FAIRA.BIN
    57,  //  29 FAIGA.BIN
    7,   //  30 BRIZAD.BIN
    8,   //  31 BRIZARA.BIN
    58,  //  32 BRIZAG.BIN
    3,   //  33 THUNDER.BIN
    4,   //  34 THUNDERA.BIN
    5,   //  35 THUNDG1.BIN
    53,  //  36 QUEIK.BIN
    51,  //  37 QUEIR.BIN
    52,  //  38 QUEIG.BIN
    28,  //  39 BIO.BIN
    29,  //  40 BIORA.BIN
    31,  //  41 BIOGA1.BIN
    9,   //  42 GRAVIDE.BIN
    24,  //  43 GRAVIRA.BIN
    25,  //  44 GRAVIGA1.BIN
    27,  //  45 COMET.BIN
    30,  //  46 COMETEO.BIN
    43,  //  47 FREEZE.BIN
    33,  //  48 BREAK.BIN
    23,  //  49 TORNADO.BIN
    56,  //  50 FLEA.BIN
    215, //  51 FULLCARE.BIN
    60,  //  52 ALTEMA.BIN
    149  //  53 SEALD.BIN
};

// currentActionId 13 -- enemy skills: overlay file id per slot.
s16 D_800EF6A8[] = {
    74, //   0 KAERU.BIN
    80, //   1 LV4JIBAK.BIN
    72, //   2 HANMMER.BIN
    37, //   3 W_WIND.BIN
    81, //   4 MYTYG.BIN
    84, //   5 RAISE.BIN
    73, //   6 DRAGONN.BIN
    75, //   7 DETHFO.BIN
    44, //   8 FLAME.BIN
    78, //   9 REAZER.BIN
    79, //  10 MATRA.BIN
    71, //  11 KUSAI.BIN
    59, //  12 BEATA.BIN
    68, //  13 AQUA.BIN
    66, //  14 TRAINN.BIN
    67, //  15 MAGIKAL.BIN
    70, //  16 HATENA.BIN
    69, //  17 GOBLINP.BIN
    86, //  18 CONF.BIN
    76, //  19 LV5DETH.BIN
    83, //  20 SENNKOKU.BIN
    83, //  21 SENNKOKU.BIN
    62, //  22 SFLEA.BIN
    82  //  23 NANNTOKA.BIN
};

// currentActionId 3 -- summons: overlay file id per slot, indexed by Summon.
s16 D_800EF6D8[] = {
    189, //   0 CHOCO0.BIN
    42,  //   1 SIVA.BIN
    41,  //   2 IFLEET.BIN
    40,  //   3 LAMU.BIN
    39,  //   4 TITAN.BIN
    252, //   5 ODIN2.BIN
    15,  //   6 RIVA.BIN
    257, //   7 VAHAMUT.BIN
    193, //   8 KUJATA.BIN
    191, //   9 ALEX.BIN
    179, //  10 PHOENIX.BIN
    262, //  11 VAHAMUT2.BIN
    235, //  12 HADES.BIN
    213, //  13 TUPON.BIN
    265, //  14 VAHAMUT0.BIN
    271, //  15 KNIGHTS.BIN
    190, //  16 DEBUCHO.BIN
    253  //  17 ODIN1.BIN
};

// currentActionId 32 -- enemy attacks: overlay file id per slot.
s16 D_800EF6FC[] = {
    158, //   0 KOUDAN1.BIN
    143, //   1 KAEN.BIN
    97,  //   2 ITO.BIN
    93,  //   3 TAN.BIN
    21,  //   4 LASER.BIN
    99,  //   5 LASER2.BIN
    111, //   6 MIZU.BIN
    91,  //   7 SEED.BIN
    114, //   8 CBREATH.BIN
    118, //   9 DOKUFUKI.BIN
    116, //  10 NONOSHI.BIN
    87,  //  11 ONPA.BIN
    96,  //  12 SUNA.BIN
    132, //  13 THUNDER2.BIN
    94,  //  14 KIEN.BIN
    117, //  15 OIL.BIN
    98,  //  16 HIKARI.BIN
    89,  //  17 SEARCH.BIN
    135, //  18 NETSU.BIN
    102, //  19 MISSILE.BIN
    109, //  20 OSEN.BIN
    105, //  21 SUMI.BIN
    92,  //  22 MGBEAM.BIN
    104, //  23 Q_KETSU.BIN
    136, //  24 GEKI1.BIN
    137, //  25 GEKI2.BIN
    127, //  26 OTSUNAMI.BIN
    88,  //  27 BACUME.BIN
    107, //  28 JIBAKU1.BIN
    122, //  29 SOUTHERN.BIN
    1,   //  30 FIRE.BIN
    90,  //  31 HARI1000.BIN
    121, //  32 QUASAR.BIN
    161, //  33 NADARE.BIN
    103, //  34 KAMA.BIN
    95,  //  35 KONOHA.BIN
    113, //  36 RAKUSEKI.BIN
    100, //  37 YU_WAKU.BIN
    101, //  38 BLASTER.BIN
    110, //  39 TRIANGLE.BIN
    115, //  40 ATOMIC.BIN
    1,   //  41 FIRE.BIN
    188, //  42 HADO.BIN
    184, //  43 HO_DEN.BIN
    112, //  44 HARRIER.BIN
    1,   //  45 FIRE.BIN
    195, //  46 SPADE1.BIN
    197, //  47 HEART1.BIN
    199, //  48 CLUB1.BIN
    201, //  49 DIA1.BIN
    203, //  50 JOKER1.BIN
    196, //  51 SPADE2.BIN
    198, //  52 HEART2.BIN
    200, //  53 CLUB2.BIN
    202, //  54 DIA2.BIN
    204, //  55 JOKER2.BIN
    106, //  56 KAMA2.BIN
    159, //  57 KOUDAN2.BIN
    144, //  58 KAEN2.BIN
    120, //  59 FUBUKI.BIN
    119, //  60 DOKUEKI.BIN
    125, //  61 TOBOE.BIN
    126, //  62 KAFUN.BIN
    160, //  63 ROCKET.BIN
    16,  //  64 THREAT.BIN
    108, //  65 Q_SHU.BIN
    164, //  66 JIBASIRI.BIN
    142, //  67 JIBAKU2.BIN
    123, //  68 NORTHERN.BIN
    209, //  69 HADOHO.BIN
    216, //  70 ROLLFIRE.BIN
    44,  //  71 FLAME.BIN
    182, //  72 GESUI.BIN
    182, //  73 GESUI.BIN
    181, //  74 MIZUFUKI.BIN
    124, //  75 SCOPE.BIN
    163, //  76 TAIL.BIN
    151, //  77 AYASII.BIN
    141, //  78 TSURARA.BIN
    131, //  79 AWA.BIN
    165, //  80 SHOWER.BIN
    130, //  81 CHIKASUI.BIN
    133, //  82 ONSEN.BIN
    134, //  83 MAGMA.BIN
    129, //  84 KINZAN.BIN
    152, //  85 DAISENP.BIN
    152, //  86 DAISENP.BIN
    174, //  87 SEKIKA.BIN
    167, //  88 HAKUGEK1.BIN
    168, //  89 HAKUGEK2.BIN
    139, //  90 HAKAI.BIN
    140, //  91 WLASER.BIN
    138, //  92 AEROGA.BIN
    154, //  93 ZENBAKU.BIN
    0,   //  94 CAREL.BIN
    157, //  95 REITO.BIN
    44,  //  96 FLAME.BIN
    44,  //  97 FLAME.BIN
    145, //  98 REIKI.BIN
    147, //  99 HIKARI3.BIN
    146, // 100 HIKARI2.BIN
    187, // 101 ENERGY.BIN
    150, // 102 HEATBU.BIN
    46,  // 103 WHIRL.BIN
    156, // 104 YAMI.BIN
    175, // 105 RMISSILE.BIN
    183, // 106 TBREATH.BIN
    173, // 107 NAPALM.BIN
    166, // 108 GAS.BIN
    211, // 109 SHAKU.BIN
    210, // 110 RE_DO.BIN
    172, // 111 KONA.BIN
    171, // 112 PINK.BIN
    169, // 113 JAMMER.BIN
    47,  // 114 TEARS.BIN
    162, // 115 ELECBALL.BIN
    153, // 116 UTAGOE.BIN
    185, // 117 SAIKYO.BIN
    148, // 118 MAGMA2.BIN
    48,  // 119 TIMEDMG.BIN
    178, // 120 KOROSI.BIN
    155, // 121 RENBAKU.BIN
    118, // 122 DOKUFUKI.BIN
    126, // 123 KAFUN.BIN
    128, // 124 SMOG2.BIN
    38,  // 125 ELEVATOR.BIN
    177, // 126 FROG.BIN
    208, // 127 SHIELD.BIN
    208, // 128 SHIELD.BIN
    304, // 129 PUNCH.BIN
    304, // 130 PUNCH.BIN
    44,  // 131 FLAME.BIN
    176, // 132 DAIONPA.BIN
    53,  // 133 QUEIK.BIN
    51,  // 134 QUEIR.BIN
    52,  // 135 QUEIG.BIN
    4,   // 136 THUNDERA.BIN
    170, // 137 JAMMER2.BIN
    44,  // 138 FLAME.BIN
    44,  // 139 FLAME.BIN
    186, // 140 KAKUSAN.BIN
    212, // 141 CHO_KYU.BIN
    307, // 142 LASBOSS.BIN
    318, // 143 file id 318
    84,  // 144 RAISE.BIN
    309, // 145 MAGEND.BIN
    310, // 146 TO_SEKI.BIN
    311, // 147 DEADSEF.BIN
    0,   // 148 CAREL.BIN
    312, // 149 ANTLION.BIN
    313, // 150 STING.BIN
    313, // 151 STING.BIN
    314, // 152 WEP3.BIN
    315, // 153 BEAM.BIN
    316, // 154 WEP4.BIN
    317, // 155 DIA.BIN
    315, // 156 BEAM.BIN
    0    // 157 CAREL.BIN
};

// currentActionId 20 -- limit breaks: overlay file id per slot.
s16 D_800EF838[] = {
    18,  //   0 BLAVER.BIN
    19,  //   1 KYOGIRI.BIN
    291, //   2 HAKOU.BIN
    297, //   3 CLIMB.BIN
    233, //   4 MRAIN.BIN
    234, //   5 GARYU.BIN
    292, //   6 BUSHIN2.BIN
    20,  //   7 HVSHOT.BIN
    289, //   8 GRENADE.BIN
    249, //   9 M_BREAK.BIN
    296, //  10 H_BLOW.BIN
    290, //  11 S_BEAM.BIN
    298, //  12 ANGERMAX.BIN
    295, //  13 CATASTRO.BIN
    17,  //  14 IYASHI.BIN
    288, //  15 JAKI.BIN
    305, //  16 DAICHI.BIN
    259, //  17 BRAND.BIN
    306, //  18 S_GUARD.BIN
    250, //  19 BEAT.BIN
    287, //  20 GOSPEL.BIN
    206, //  21 TIFA.BIN
    206, //  22 TIFA.BIN
    206, //  23 TIFA.BIN
    206, //  24 TIFA.BIN
    206, //  25 TIFA.BIN
    206, //  26 TIFA.BIN
    206, //  27 TIFA.BIN
    240, //  28 BOOST.BIN
    241, //  29 MOD.BIN
    242, //  30 HYPER.BIN
    236, //  31 DYNAMITE.BIN
    243, //  32 DIVE.BIN
    244, //  33 RANTO.BIN
    238, //  34 HIWIND.BIN
    205, //  35 SLDFANG.BIN
    225, //  36 HMOON.BIN
    205, //  37 SLDFANG.BIN
    226, //  38 SDUST.BIN
    227, //  39 LUNATIC.BIN
    228, //  40 ERAVE.BIN
    229, //  41 COSMO.BIN
    254, //  42 DISE.BIN
    1,   //  43 FIRE.BIN
    1,   //  44 FIRE.BIN
    1,   //  45 FIRE.BIN
    1,   //  46 FIRE.BIN
    1,   //  47 FIRE.BIN
    1,   //  48 FIRE.BIN
    245, //  49 JINRAI.BIN
    237, //  50 MEIKYO.BIN
    260, //  51 GAISEI.BIN
    246, //  52 CHI.BIN
    247, //  53 GAISHU.BIN
    248, //  54 SEIJA.BIN
    239, //  55 SHINRA.BIN
    261, //  56 GATTAI.BIN
    230, //  57 MDANCE.BIN
    231, //  58 TSOL.BIN
    232, //  59 LGIRL.BIN
    76,  //  60 LV5DETH.BIN
    85,  //  61 DEATH.BIN
    251, //  62 BEAST.BIN
    251, //  63 BEAST.BIN
    251, //  64 BEAST.BIN
    301, //  65 SPARK.BIN
    251, //  66 BEAST.BIN
    302, //  67 NIGHT.BIN
    251, //  68 BEAST.BIN
    303, //  69 SATAN.BIN
    113, //  70 RAKUSEKI.BIN
    141, //  71 TSURARA.BIN
    224, //  72 TOYBOX.BIN
    224, //  73 TOYBOX.BIN
    224, //  74 TOYBOX.BIN
    224, //  75 TOYBOX.BIN
    30,  //  76 COMETEO.BIN
    234, //  77 GARYU.BIN
    303, //  78 SATAN.BIN
    291  //  79 HAKOU.BIN
};

// currentActionId 8 -- throw: overlay file id per slot.
s16 D_800EF8D8[] = {
    0,   //   0 CAREL.BIN
    217, //   1 WPCU.BIN
    217, //   2 WPCU.BIN
    217, //   3 WPCU.BIN
    217, //   4 WPCU.BIN
    217, //   5 WPCU.BIN
    217, //   6 WPCU.BIN
    217, //   7 WPCU.BIN
    217, //   8 WPCU.BIN
    217, //   9 WPCU.BIN
    217, //  10 WPCU.BIN
    217, //  11 WPCU.BIN
    217, //  12 WPCU.BIN
    217, //  13 WPCU.BIN
    217, //  14 WPCU.BIN
    217, //  15 WPCU.BIN
    0,   //  16 CAREL.BIN
    0,   //  17 CAREL.BIN
    0,   //  18 CAREL.BIN
    0,   //  19 CAREL.BIN
    0,   //  20 CAREL.BIN
    0,   //  21 CAREL.BIN
    0,   //  22 CAREL.BIN
    0,   //  23 CAREL.BIN
    0,   //  24 CAREL.BIN
    0,   //  25 CAREL.BIN
    0,   //  26 CAREL.BIN
    0,   //  27 CAREL.BIN
    0,   //  28 CAREL.BIN
    0,   //  29 CAREL.BIN
    0,   //  30 CAREL.BIN
    0,   //  31 CAREL.BIN
    218, //  32 WPBA.BIN
    218, //  33 WPBA.BIN
    218, //  34 WPBA.BIN
    218, //  35 WPBA.BIN
    218, //  36 WPBA.BIN
    218, //  37 WPBA.BIN
    218, //  38 WPBA.BIN
    218, //  39 WPBA.BIN
    218, //  40 WPBA.BIN
    218, //  41 WPBA.BIN
    218, //  42 WPBA.BIN
    218, //  43 WPBA.BIN
    218, //  44 WPBA.BIN
    218, //  45 WPBA.BIN
    218, //  46 WPBA.BIN
    218, //  47 WPBA.BIN
    0,   //  48 CAREL.BIN
    0,   //  49 CAREL.BIN
    0,   //  50 CAREL.BIN
    0,   //  51 CAREL.BIN
    0,   //  52 CAREL.BIN
    0,   //  53 CAREL.BIN
    0,   //  54 CAREL.BIN
    0,   //  55 CAREL.BIN
    0,   //  56 CAREL.BIN
    0,   //  57 CAREL.BIN
    0,   //  58 CAREL.BIN
    0,   //  59 CAREL.BIN
    0,   //  60 CAREL.BIN
    0,   //  61 CAREL.BIN
    219, //  62 WPEA.BIN
    219, //  63 WPEA.BIN
    219, //  64 WPEA.BIN
    219, //  65 WPEA.BIN
    219, //  66 WPEA.BIN
    219, //  67 WPEA.BIN
    219, //  68 WPEA.BIN
    219, //  69 WPEA.BIN
    219, //  70 WPEA.BIN
    219, //  71 WPEA.BIN
    219, //  72 WPEA.BIN
    220, //  73 WPSI.BIN
    220, //  74 WPSI.BIN
    220, //  75 WPSI.BIN
    220, //  76 WPSI.BIN
    220, //  77 WPSI.BIN
    220, //  78 WPSI.BIN
    220, //  79 WPSI.BIN
    220, //  80 WPSI.BIN
    220, //  81 WPSI.BIN
    220, //  82 WPSI.BIN
    220, //  83 WPSI.BIN
    220, //  84 WPSI.BIN
    220, //  85 WPSI.BIN
    220, //  86 WPSI.BIN
    221, //  87 WPYU.BIN
    221, //  88 WPYU.BIN
    221, //  89 WPYU.BIN
    221, //  90 WPYU.BIN
    221, //  91 WPYU.BIN
    221, //  92 WPYU.BIN
    221, //  93 WPYU.BIN
    221, //  94 WPYU.BIN
    221, //  95 WPYU.BIN
    221, //  96 WPYU.BIN
    221, //  97 WPYU.BIN
    221, //  98 WPYU.BIN
    221, //  99 WPYU.BIN
    221, // 100 WPYU.BIN
    222, // 101 WPKE.BIN
    222, // 102 WPKE.BIN
    222, // 103 WPKE.BIN
    222, // 104 WPKE.BIN
    222, // 105 WPKE.BIN
    222, // 106 WPKE.BIN
    222, // 107 WPKE.BIN
    222, // 108 WPKE.BIN
    222, // 109 WPKE.BIN
    222, // 110 WPKE.BIN
    222, // 111 WPKE.BIN
    222, // 112 WPKE.BIN
    222, // 113 WPKE.BIN
    223, // 114 WPBI.BIN
    223, // 115 WPBI.BIN
    223, // 116 WPBI.BIN
    223, // 117 WPBI.BIN
    223, // 118 WPBI.BIN
    223, // 119 WPBI.BIN
    223, // 120 WPBI.BIN
    223, // 121 WPBI.BIN
    223, // 122 WPBI.BIN
    223, // 123 WPBI.BIN
    223, // 124 WPBI.BIN
    223, // 125 WPBI.BIN
    223, // 126 WPBI.BIN
    0    // 127 CAREL.BIN
};

// currentActionId 4 -- items: entrypoint per slot.
MagicEntry D_800EF9D8[] = {
    (MagicEntry)0x801B08D4,              //   0 CAREL.BIN
    (MagicEntry)0x801B109C,              //   1 CAREL.BIN
    (MagicEntry)0x801B09B8,              //   2 CAREL.BIN
    (MagicEntry)0x801B0D24,              //   3 CAREL.BIN
    (MagicEntry)0x801B0E08,              //   4 CAREL.BIN
    (MagicEntry)0x801B0A8C,              //   5 CAREL.BIN
    (MagicEntry)0x801B0A8C,              //   6 CAREL.BIN
    (MagicEntry)0x801B0EEC,              //   7 CAREL.BIN
    (MagicEntry)0x801B0B60,              //   8 CAREL.BIN
    (MagicEntry)0x801B0364,              //   9 THREAT.BIN
    (MagicEntry)0x801B0054,              //  10 TODO.BIN
    (MagicEntry)0x801B0054,              //  11 MINIMAM.BIN
    (MagicEntry)0x801B0000,              //  12 ECHOES.BIN
    (MagicEntry)0x801B0000,              //  13 HAKAI.BIN
    (MagicEntry)0x801B0000,              //  14 TEARS.BIN
    (MagicEntry)0x801B0000,              //  15 ALMIGHTY.BIN
    (MagicEntry)0x801B0000,              //  16 SMKBALL.BIN
    (MagicEntry)0x801B0054,              //  17 HEIST.BIN
    (MagicEntry)0x801B0054,              //  18 MYTYG.BIN
    (MagicEntry)0x801B0564,              //  19 RESIST.BIN
    (MagicEntry)BattleQueueImpactEffect, //  20 FIRE.BIN
    (MagicEntry)0x801B0054,              //  21 RENBAKU.BIN
    (MagicEntry)0x801B0054,              //  22 ZENBAKU.BIN
    (MagicEntry)0x801B0054,              //  23 STOP.BIN
    BattleStartEffectWithModel,           //  24 DEATH.BIN
    (MagicEntry)0x801B0000,              //  25 SLOW.BIN
    (MagicEntry)0x801B0054,              //  26 SLIPL.BIN
    (MagicEntry)0x801B0054,              //  27 SAILESS.BIN
    (MagicEntry)0x801B07D0,              //  28 BERSERK.BIN
    BattleStartEffectWithModel,           //  29 CONF.BIN
    MAGIC_Faira,                         //  30 FAIRA.BIN
    (MagicEntry)0x801B0084,              //  31 FAIGA2.BIN
    MAGIC_Brizara,                       //  32 BRIZARA.BIN
    (MagicEntry)0x801B0054,              //  33 BRIZAG2.BIN
    MAGIC_Thundera,                      //  34 THUNDERA.BIN
    (MagicEntry)0x801B000C,              //  35 THUNDG2.BIN
    (MagicEntry)0x801B0074,              //  36 QUEIR.BIN
    (MagicEntry)0x801B0054,              //  37 QUEIG.BIN
    (MagicEntry)0x801B0000,              //  38 BIORA.BIN
    (MagicEntry)0x801B0000,              //  39 BIOGA2.BIN
    (MagicEntry)0x801B0000,              //  40 COMETEO.BIN
    (MagicEntry)0x801B1028,              //  41 Q_KETSU.BIN
    (MagicEntry)0x801B1028,              //  42 Q_SHU.BIN
    (MagicEntry)0x801B0854,              //  43 BREAK.BIN
    MAGIC_Barrier,                       //  44 BARRIER.BIN
    MAGIC_MBarrier,                      //  45 MABARIA.BIN
    MAGIC_Refrec,                        //  46 REFREC.BIN
    (MagicEntry)0x801B0054,              //  47 DETHPER.BIN
    (MagicEntry)0x801B0054,              //  48 DAISENP.BIN
    (MagicEntry)0x801B0054,              //  49 AQUA.BIN
    (MagicEntry)0x801B0054,              //  50 TODO.BIN
    (MagicEntry)0x801B0054,              //  51 MINIMAM.BIN
    (MagicEntry)0x801B0000,              //  52 ESNA.BIN
    MAGIC_Fire,                          //  53 FIRE.BIN
    (MagicEntry)0x801B0000,              //  54 HAKUGEK2.BIN
    (MagicEntry)0x801B0000,              //  55 NAPALM.BIN
    (MagicEntry)0x801B0730,              //  56 GRAVIDE.BIN
    (MagicEntry)0x801B0C10,              //  57 GRAVIRA.BIN
    (MagicEntry)0x801B0B38,              //  58 SUMI.BIN
    (MagicEntry)0x801B0FC4,              //  59 HARI1000.BIN
    (MagicEntry)0x801B0000,              //  60 TBREATH.BIN
    (MagicEntry)0x801B0054,              //  61 KUSAI.BIN
    (MagicEntry)0x801B0000,              //  62 VEGET.BIN
    (MagicEntry)0x801B0030,              //  63 VEGET.BIN
    (MagicEntry)0x801B0060,              //  64 VEGET.BIN
    (MagicEntry)0x801B0090,              //  65 VEGET.BIN
    (MagicEntry)0x801B00C0,              //  66 VEGET.BIN
    (MagicEntry)0x801B00F0,              //  67 VEGET.BIN
    (MagicEntry)0x801B0120,              //  68 VEGET.BIN
    (MagicEntry)0x801B0150               //  69 VEGET.BIN
};

// currentActionId 2 -- magic spells: entrypoint per slot.
MagicEntry D_800EFAF0[] = {
    (MagicEntry)0x801B08D4,    //   0 CAREL.BIN
    (MagicEntry)0x801B09B8,    //   1 CAREL.BIN
    (MagicEntry)0x801B0A8C,    //   2 CAREL.BIN
    (MagicEntry)0x801B0C44,    //   3 CAREL.BIN
    (MagicEntry)0x801B0B60,    //   4 CAREL.BIN
    (MagicEntry)0x801B0000,    //   5 ESNA.BIN
    (MagicEntry)0x801B0564,    //   6 RESIST.BIN
    BattleStartEffectWithModel, //   7 RAISE.BIN
    BattleStartEffectWithModel, //   8 RAISE.BIN
    (MagicEntry)0x801B0054,    //   9 MINIMAM.BIN
    (MagicEntry)0x801B0054,    //  10 TODO.BIN
    (MagicEntry)0x801B0054,    //  11 SLIPL.BIN
    BattleStartEffectWithModel, //  12 CONF.BIN
    (MagicEntry)0x801B0054,    //  13 SAILESS.BIN
    (MagicEntry)0x801B07D0,    //  14 BERSERK.BIN
    MAGIC_Barrier,             //  15 BARRIER.BIN
    MAGIC_MBarrier,            //  16 MABARIA.BIN
    MAGIC_Refrec,              //  17 REFREC.BIN
    (MagicEntry)0x801B0000,    //  18 WALL.BIN
    (MagicEntry)0x801B0054,    //  19 HEIST.BIN
    (MagicEntry)0x801B0000,    //  20 SLOW.BIN
    (MagicEntry)0x801B0054,    //  21 STOP.BIN
    (MagicEntry)0x801B0D4C,    //  22 DEBARIA.BIN
    (MagicEntry)0x801B0054,    //  23 DETHPER.BIN
    BattleStartEffectWithModel, //  24 DEATH.BIN
    (MagicEntry)0x801B0000,    //  25 ESCAPE.BIN
    (MagicEntry)0x801B0000,    //  26 DEJON.BIN
    MAGIC_Fire,                //  27 FIRE.BIN
    MAGIC_Faira,               //  28 FAIRA.BIN
    (MagicEntry)0x801B0084,    //  29 FAIGA.BIN
    MAGIC_Brizad,              //  30 BRIZAD.BIN
    MAGIC_Brizara,             //  31 BRIZARA.BIN
    (MagicEntry)0x801B0054,    //  32 BRIZAG.BIN
    MAGIC_Thunder,             //  33 THUNDER.BIN
    MAGIC_Thundera,            //  34 THUNDERA.BIN
    (MagicEntry)0x801B000C,    //  35 THUNDG1.BIN
    (MagicEntry)0x801B0054,    //  36 QUEIK.BIN
    (MagicEntry)0x801B0074,    //  37 QUEIR.BIN
    (MagicEntry)0x801B0054,    //  38 QUEIG.BIN
    (MagicEntry)0x801B0640,    //  39 BIO.BIN
    (MagicEntry)0x801B0000,    //  40 BIORA.BIN
    (MagicEntry)0x801B0000,    //  41 BIOGA1.BIN
    (MagicEntry)0x801B0730,    //  42 GRAVIDE.BIN
    (MagicEntry)0x801B0C10,    //  43 GRAVIRA.BIN
    (MagicEntry)0x801B1464,    //  44 GRAVIGA1.BIN
    (MagicEntry)0x801B0000,    //  45 COMET.BIN
    (MagicEntry)0x801B0000,    //  46 COMETEO.BIN
    (MagicEntry)0x801B0000,    //  47 FREEZE.BIN
    (MagicEntry)0x801B0854,    //  48 BREAK.BIN
    (MagicEntry)0x801B0000,    //  49 TORNADO.BIN
    (MagicEntry)0x801B0054,    //  50 FLEA.BIN
    (MagicEntry)0x801B0000,    //  51 FULLCARE.BIN
    (MagicEntry)0x801B0054,    //  52 ALTEMA.BIN
    (MagicEntry)0x801B0054     //  53 SEALD.BIN
};

// currentActionId 13 -- enemy skills: entrypoint per slot.
MagicEntry D_800EFBC8[] = {
    (MagicEntry)0x801B0054,    //   0 KAERU.BIN
    (MagicEntry)0x801B0070,    //   1 LV4JIBAK.BIN
    (MagicEntry)0x801B0054,    //   2 HANMMER.BIN
    (MagicEntry)0x801B04C0,    //   3 W_WIND.BIN
    (MagicEntry)0x801B0054,    //   4 MYTYG.BIN
    BattleStartEffectWithModel, //   5 RAISE.BIN
    (MagicEntry)0x801B0054,    //   6 DRAGONN.BIN
    (MagicEntry)0x801B0054,    //   7 DETHFO.BIN
    (MagicEntry)0x801B0000,    //   8 FLAME.BIN
    (MagicEntry)0x801B0088,    //   9 REAZER.BIN
    (MagicEntry)0x801B0070,    //  10 MATRA.BIN
    (MagicEntry)0x801B0054,    //  11 KUSAI.BIN
    (MagicEntry)0x801B0054,    //  12 BEATA.BIN
    (MagicEntry)0x801B0054,    //  13 AQUA.BIN
    (MagicEntry)0x801B0054,    //  14 TRAINN.BIN
    (MagicEntry)0x801B0054,    //  15 MAGIKAL.BIN
    (MagicEntry)0x801B0054,    //  16 HATENA.BIN
    (MagicEntry)0x801B0054,    //  17 GOBLINP.BIN
    BattleStartEffectWithModel, //  18 CONF.BIN
    MAGIC_Lv5Death,            //  19 LV5DETH.BIN
    BattleStartEffectWithModel, //  20 SENNKOKU.BIN
    BattleStartEffectWithModel, //  21 SENNKOKU.BIN
    (MagicEntry)0x801B0054,    //  22 SFLEA.BIN
    (MagicEntry)0x801B006C     //  23 NANNTOKA.BIN
};

// currentActionId 32 -- enemy attacks: entrypoint per slot.
MagicEntry D_800EFC28[] = {
    (MagicEntry)0x801B0030,                //   0 KOUDAN1.BIN
    (MagicEntry)0x801B0010,                //   1 KAEN.BIN
    (MagicEntry)0x801B0000,                //   2 ITO.BIN
    (MagicEntry)0x801B0634,                //   3 TAN.BIN
    (MagicEntry)0x801B06F4,                //   4 LASER.BIN
    (MagicEntry)0x801B0010,                //   5 LASER2.BIN
    (MagicEntry)0x801B0000,                //   6 MIZU.BIN
    (MagicEntry)0x801B08D0,                //   7 SEED.BIN
    (MagicEntry)0x801B0F14,                //   8 CBREATH.BIN
    (MagicEntry)0x801B08C4,                //   9 DOKUFUKI.BIN
    (MagicEntry)0x801B0000,                //  10 NONOSHI.BIN
    (MagicEntry)0x801B0000,                //  11 ONPA.BIN
    (MagicEntry)0x801B0CDC,                //  12 SUNA.BIN
    (MagicEntry)0x801B0000,                //  13 THUNDER2.BIN
    (MagicEntry)0x801B0948,                //  14 KIEN.BIN
    (MagicEntry)0x801B0000,                //  15 OIL.BIN
    (MagicEntry)0x801B0698,                //  16 HIKARI.BIN
    (MagicEntry)0x801B08B4,                //  17 SEARCH.BIN
    (MagicEntry)0x801B0000,                //  18 NETSU.BIN
    (MagicEntry)0x801B0000,                //  19 MISSILE.BIN
    (MagicEntry)0x801B0000,                //  20 OSEN.BIN
    (MagicEntry)0x801B0B38,                //  21 SUMI.BIN
    (MagicEntry)0x801B0000,                //  22 MGBEAM.BIN
    (MagicEntry)0x801B1028,                //  23 Q_KETSU.BIN
    (MagicEntry)0x801B0000,                //  24 GEKI1.BIN
    (MagicEntry)0x801B0000,                //  25 GEKI2.BIN
    (MagicEntry)0x801B0000,                //  26 OTSUNAMI.BIN
    (MagicEntry)0x801B0000,                //  27 BACUME.BIN
    (MagicEntry)0x801B0338,                //  28 JIBAKU1.BIN
    (MagicEntry)0x801B0000,                //  29 SOUTHERN.BIN
    MAGIC_Fire,                            //  30 FIRE.BIN
    (MagicEntry)0x801B0FC4,                //  31 HARI1000.BIN
    (MagicEntry)0x801B0000,                //  32 QUASAR.BIN
    (MagicEntry)0x801B0000,                //  33 NADARE.BIN
    (MagicEntry)0x801B0010,                //  34 KAMA.BIN
    (MagicEntry)0x801B0000,                //  35 KONOHA.BIN
    (MagicEntry)0x801B0020,                //  36 RAKUSEKI.BIN
    (MagicEntry)0x801B0000,                //  37 YU_WAKU.BIN
    (MagicEntry)0x801B0000,                //  38 BLASTER.BIN
    (MagicEntry)0x801B0000,                //  39 TRIANGLE.BIN
    (MagicEntry)0x801B1978,                //  40 ATOMIC.BIN
    MAGIC_Fire,                            //  41 FIRE.BIN
    (MagicEntry)0x801B0000,                //  42 HADO.BIN
    (MagicEntry)0x801B0000,                //  43 HO_DEN.BIN
    (MagicEntry)0x801B0000,                //  44 HARRIER.BIN
    MAGIC_Fire,                            //  45 FIRE.BIN
    (MagicEntry)0x801B0054,                //  46 SPADE1.BIN
    (MagicEntry)0x801B0054,                //  47 HEART1.BIN
    (MagicEntry)0x801B0054,                //  48 CLUB1.BIN
    (MagicEntry)0x801B0054,                //  49 DIA1.BIN
    BattleStartEffectWithModel,             //  50 JOKER1.BIN
    (MagicEntry)0x801B0054,                //  51 SPADE2.BIN
    (MagicEntry)0x801B0054,                //  52 HEART2.BIN
    (MagicEntry)0x801B0054,                //  53 CLUB2.BIN
    (MagicEntry)0x801B0054,                //  54 DIA2.BIN
    (MagicEntry)0x801B0054,                //  55 JOKER2.BIN
    (MagicEntry)0x801B09D4,                //  56 KAMA2.BIN
    (MagicEntry)0x801B0030,                //  57 KOUDAN2.BIN
    (MagicEntry)0x801B0000,                //  58 KAEN2.BIN
    (MagicEntry)0x801B0020,                //  59 FUBUKI.BIN
    (MagicEntry)0x801B0000,                //  60 DOKUEKI.BIN
    (MagicEntry)0x801B0000,                //  61 TOBOE.BIN
    (MagicEntry)0x801B0F18,                //  62 KAFUN.BIN
    (MagicEntry)0x801B0000,                //  63 ROCKET.BIN
    (MagicEntry)0x801B0364,                //  64 THREAT.BIN
    (MagicEntry)0x801B1028,                //  65 Q_SHU.BIN
    (MagicEntry)0x801B0000,                //  66 JIBASIRI.BIN
    (MagicEntry)0x801B0000,                //  67 JIBAKU2.BIN
    (MagicEntry)0x801B0000,                //  68 NORTHERN.BIN
    (MagicEntry)0x801B0000,                //  69 HADOHO.BIN
    (MagicEntry)0x801B0000,                //  70 ROLLFIRE.BIN
    (MagicEntry)0x801B01B4,                //  71 FLAME.BIN
    (MagicEntry)0x801B0000,                //  72 GESUI.BIN
    (MagicEntry)0x801B0020,                //  73 GESUI.BIN
    (MagicEntry)0x801B0000,                //  74 MIZUFUKI.BIN
    (MagicEntry)0x801B0C94,                //  75 SCOPE.BIN
    (MagicEntry)0x801B0000,                //  76 TAIL.BIN
    (MagicEntry)0x801B0054,                //  77 AYASII.BIN
    (MagicEntry)0x801B0000,                //  78 TSURARA.BIN
    (MagicEntry)0x801B0000,                //  79 AWA.BIN
    (MagicEntry)0x801B0000,                //  80 SHOWER.BIN
    (MagicEntry)0x801B0000,                //  81 CHIKASUI.BIN
    (MagicEntry)0x801B0000,                //  82 ONSEN.BIN
    (MagicEntry)0x801B0000,                //  83 MAGMA.BIN
    (MagicEntry)0x801B0000,                //  84 KINZAN.BIN
    (MagicEntry)0x801B007C,                //  85 DAISENP.BIN
    (MagicEntry)0x801B0054,                //  86 DAISENP.BIN
    (MagicEntry)0x801B0008,                //  87 SEKIKA.BIN
    (MagicEntry)0x801B0000,                //  88 HAKUGEK1.BIN
    (MagicEntry)0x801B0000,                //  89 HAKUGEK2.BIN
    (MagicEntry)0x801B0000,                //  90 HAKAI.BIN
    (MagicEntry)0x801B0000,                //  91 WLASER.BIN
    (MagicEntry)0x801B0000,                //  92 AEROGA.BIN
    (MagicEntry)0x801B0054,                //  93 ZENBAKU.BIN
    (MagicEntry)0x801B0FCC,                //  94 CAREL.BIN
    (MagicEntry)0x801B0000,                //  95 REITO.BIN
    (MagicEntry)0x801B0048,                //  96 FLAME.BIN
    (MagicEntry)0x801B0164,                //  97 FLAME.BIN
    (MagicEntry)0x801B0000,                //  98 REIKI.BIN
    (MagicEntry)0x801B0000,                //  99 HIKARI3.BIN
    (MagicEntry)0x801B0000,                // 100 HIKARI2.BIN
    (MagicEntry)0x801B0000,                // 101 ENERGY.BIN
    (MagicEntry)0x801B0054,                // 102 HEATBU.BIN
    (MagicEntry)0x801B0000,                // 103 WHIRL.BIN
    (MagicEntry)0x801B0000,                // 104 YAMI.BIN
    (MagicEntry)0x801B0000,                // 105 RMISSILE.BIN
    (MagicEntry)0x801B0000,                // 106 TBREATH.BIN
    (MagicEntry)0x801B0000,                // 107 NAPALM.BIN
    (MagicEntry)0x801B0010,                // 108 GAS.BIN
    (MagicEntry)0x801B0000,                // 109 SHAKU.BIN
    (MagicEntry)0x801B0000,                // 110 RE_DO.BIN
    (MagicEntry)0x801B0000,                // 111 KONA.BIN
    (MagicEntry)0x801B0000,                // 112 PINK.BIN
    (MagicEntry)0x801B0000,                // 113 JAMMER.BIN
    (MagicEntry)0x801B0000,                // 114 TEARS.BIN
    (MagicEntry)0x801B0E98,                // 115 ELECBALL.BIN
    (MagicEntry)0x801B0054,                // 116 UTAGOE.BIN
    (MagicEntry)0x801B0000,                // 117 SAIKYO.BIN
    (MagicEntry)0x801B0000,                // 118 MAGMA2.BIN
    (MagicEntry)0x801B0000,                // 119 TIMEDMG.BIN
    (MagicEntry)0x801B0054,                // 120 KOROSI.BIN
    (MagicEntry)0x801B0054,                // 121 RENBAKU.BIN
    (MagicEntry)0x801B096C,                // 122 DOKUFUKI.BIN
    (MagicEntry)0x801B0F94,                // 123 KAFUN.BIN
    (MagicEntry)0x801B0000,                // 124 SMOG2.BIN
    (MagicEntry)0x801B0000,                // 125 ELEVATOR.BIN
    (MagicEntry)0x801B0008,                // 126 FROG.BIN
    (MagicEntry)0x801B0048,                // 127 SHIELD.BIN
    (MagicEntry)0x801B0010,                // 128 SHIELD.BIN
    (MagicEntry)0x801B0048,                // 129 PUNCH.BIN
    (MagicEntry)0x801B0010,                // 130 PUNCH.BIN
    (MagicEntry)0x801B009C,                // 131 FLAME.BIN
    (MagicEntry)0x801B0000,                // 132 DAIONPA.BIN
    (MagicEntry)0x801B0054,                // 133 QUEIK.BIN
    (MagicEntry)0x801B0074,                // 134 QUEIR.BIN
    (MagicEntry)0x801B0054,                // 135 QUEIG.BIN
    MAGIC_Thundera,                        // 136 THUNDERA.BIN
    (MagicEntry)0x801B0000,                // 137 JAMMER2.BIN
    (MagicEntry)0x801B0000,                // 138 FLAME.BIN
    (MagicEntry)0x801B00F0,                // 139 FLAME.BIN
    (MagicEntry)0x801B0000,                // 140 KAKUSAN.BIN
    (MagicEntry)0x801B0000,                // 141 CHO_KYU.BIN
    (MagicEntry)0x801B0040,                // 142 LASBOSS.BIN
    (MagicEntry)0x801B0040,                // 143 file id 318
    BattleStartEffectWithModel,             // 144 RAISE.BIN
    (MagicEntry)0x801B0000,                // 145 MAGEND.BIN
    (MagicEntry)0x801B0000,                // 146 TO_SEKI.BIN
    (MagicEntry)BattleInitMagicCastEffect, // 147 DEADSEF.BIN
    (MagicEntry)0x801B0A8C,                // 148 CAREL.BIN
    (MagicEntry)0x801B0040,                // 149 ANTLION.BIN
    (MagicEntry)0x801B0040,                // 150 STING.BIN
    (MagicEntry)0x801B0060,                // 151 STING.BIN
    (MagicEntry)0x801B0054,                // 152 WEP3.BIN
    (MagicEntry)0x801B0054,                // 153 BEAM.BIN
    (MagicEntry)0x801B0054,                // 154 WEP4.BIN
    (MagicEntry)0x801B0000,                // 155 DIA.BIN
    (MagicEntry)0x801B007C,                // 156 BEAM.BIN
    (MagicEntry)0x801B0000                 // 157 CAREL.BIN
};

// currentActionId 20 -- limit breaks: entrypoint per slot.
MagicEntryResult D_800EFEA0[] = {
    (MagicEntryResult)0x801B0000,    //   0 BLAVER.BIN
    (MagicEntryResult)0x801B069C,    //   1 KYOGIRI.BIN
    (MagicEntryResult)0x801B0010,    //   2 HAKOU.BIN
    (MagicEntryResult)0x801B0000,    //   3 CLIMB.BIN
    (MagicEntryResult)0x801B0054,    //   4 MRAIN.BIN
    (MagicEntryResult)0x801B0060,    //   5 GARYU.BIN
    (MagicEntryResult)0x801B0008,    //   6 BUSHIN2.BIN
    (MagicEntryResult)0x801B0000,    //   7 HVSHOT.BIN
    (MagicEntryResult)0x801B0000,    //   8 GRENADE.BIN
    (MagicEntryResult)0x801B0000,    //   9 M_BREAK.BIN
    (MagicEntryResult)0x801B0000,    //  10 H_BLOW.BIN
    (MagicEntryResult)0x801B0000,    //  11 S_BEAM.BIN
    (MagicEntryResult)0x801B0000,    //  12 ANGERMAX.BIN
    (MagicEntryResult)0x801B0058,    //  13 CATASTRO.BIN
    (MagicEntryResult)0x801B0000,    //  14 IYASHI.BIN
    (MagicEntryResult)0x801B0000,    //  15 JAKI.BIN
    (MagicEntryResult)0x801B0000,    //  16 DAICHI.BIN
    (MagicEntryResult)0x801B0020,    //  17 BRAND.BIN
    (MagicEntryResult)0x801B0000,    //  18 S_GUARD.BIN
    (MagicEntryResult)0x801B0000,    //  19 BEAT.BIN
    (MagicEntryResult)0x801B0048,    //  20 GOSPEL.BIN
    (MagicEntryResult)0x801B0088,    //  21 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  22 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  23 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  24 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  25 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  26 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  27 TIFA.BIN
    (MagicEntryResult)0x801B0000,    //  28 BOOST.BIN
    (MagicEntryResult)0x801B0000,    //  29 MOD.BIN
    (MagicEntryResult)0x801B0000,    //  30 HYPER.BIN
    (MagicEntryResult)0x801B0000,    //  31 DYNAMITE.BIN
    (MagicEntryResult)0x801B0000,    //  32 DIVE.BIN
    (MagicEntryResult)0x801B0000,    //  33 RANTO.BIN
    (MagicEntryResult)0x801B0000,    //  34 HIWIND.BIN
    (MagicEntryResult)0x801B0054,    //  35 SLDFANG.BIN
    (MagicEntryResult)0x801B0054,    //  36 HMOON.BIN
    (MagicEntryResult)0x801B007C,    //  37 SLDFANG.BIN
    (MagicEntryResult)0x801B0054,    //  38 SDUST.BIN
    (MagicEntryResult)0x801B0054,    //  39 LUNATIC.BIN
    (MagicEntryResult)0x801B0054,    //  40 ERAVE.BIN
    (MagicEntryResult)0x801B0000,    //  41 COSMO.BIN
    (MagicEntryResult)0x801B0054,    //  42 DISE.BIN
    (MagicEntryResult)0x801B0000,    //  43 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  44 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  45 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  46 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  47 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  48 FIRE.BIN
    (MagicEntryResult)0x801B0000,    //  49 JINRAI.BIN
    (MagicEntryResult)0x801B0000,    //  50 MEIKYO.BIN
    (MagicEntryResult)0x801B0000,    //  51 GAISEI.BIN
    (MagicEntryResult)0x801B0000,    //  52 CHI.BIN
    (MagicEntryResult)0x801B0000,    //  53 GAISHU.BIN
    (MagicEntryResult)0x801B0000,    //  54 SEIJA.BIN
    (MagicEntryResult)0x801B0000,    //  55 SHINRA.BIN
    (MagicEntryResult)0x801B007C,    //  56 GATTAI.BIN
    (MagicEntryResult)BattleStartEffectWithModel, //  57 MDANCE.BIN
    (MagicEntryResult)BattleStartEffectWithModel, //  58 TSOL.BIN
    (MagicEntryResult)0x801B0054,    //  59 LGIRL.BIN
    (MagicEntryResult)MAGIC_Lv5Death,//  60 LV5DETH.BIN
    (MagicEntryResult)BattleStartEffectWithModel, //  61 DEATH.BIN
    (MagicEntryResult)0x801B0054,    //  62 BEAST.BIN
    (MagicEntryResult)0x801B0054,    //  63 BEAST.BIN
    (MagicEntryResult)0x801B0054,    //  64 BEAST.BIN
    (MagicEntryResult)0x801B0054,    //  65 SPARK.BIN
    (MagicEntryResult)0x801B0054,    //  66 BEAST.BIN
    (MagicEntryResult)0x801B0054,    //  67 NIGHT.BIN
    (MagicEntryResult)0x801B0054,    //  68 BEAST.BIN
    (MagicEntryResult)0x801B00E0,    //  69 SATAN.BIN
    (MagicEntryResult)0x801B0020,    //  70 RAKUSEKI.BIN
    (MagicEntryResult)0x801B0000,    //  71 TSURARA.BIN
    (MagicEntryResult)0x801B0074,    //  72 TOYBOX.BIN
    (MagicEntryResult)0x801B0054,    //  73 TOYBOX.BIN
    (MagicEntryResult)0x801B0094,    //  74 TOYBOX.BIN
    (MagicEntryResult)0x801B00B4,    //  75 TOYBOX.BIN
    (MagicEntryResult)0x801B0000,    //  76 COMETEO.BIN
    (MagicEntryResult)0x801B0088,    //  77 GARYU.BIN
    (MagicEntryResult)0x801B0104,    //  78 SATAN.BIN
    (MagicEntryResult)0x801B0038     //  79 HAKOU.BIN
};

// currentActionId 8 -- throw: entrypoint per slot.
MagicEntry D_800EFFE0[] = {
    (MagicEntry)0x801B0054, //   0 CAREL.BIN
    (MagicEntry)0x801B0054, //   1 WPCU.BIN
    (MagicEntry)0x801B008C, //   2 WPCU.BIN
    (MagicEntry)0x801B00C4, //   3 WPCU.BIN
    (MagicEntry)0x801B00FC, //   4 WPCU.BIN
    (MagicEntry)0x801B0134, //   5 WPCU.BIN
    (MagicEntry)0x801B016C, //   6 WPCU.BIN
    (MagicEntry)0x801B01A4, //   7 WPCU.BIN
    (MagicEntry)0x801B01DC, //   8 WPCU.BIN
    (MagicEntry)0x801B0214, //   9 WPCU.BIN
    (MagicEntry)0x801B024C, //  10 WPCU.BIN
    (MagicEntry)0x801B0284, //  11 WPCU.BIN
    (MagicEntry)0x801B02BC, //  12 WPCU.BIN
    (MagicEntry)0x801B02F4, //  13 WPCU.BIN
    (MagicEntry)0x801B032C, //  14 WPCU.BIN
    (MagicEntry)0x801B0364, //  15 WPCU.BIN
    (MagicEntry)0x801B0364, //  16 CAREL.BIN
    (MagicEntry)0x801B0364, //  17 CAREL.BIN
    (MagicEntry)0x801B0364, //  18 CAREL.BIN
    (MagicEntry)0x801B0364, //  19 CAREL.BIN
    (MagicEntry)0x801B0364, //  20 CAREL.BIN
    (MagicEntry)0x801B0364, //  21 CAREL.BIN
    (MagicEntry)0x801B0364, //  22 CAREL.BIN
    (MagicEntry)0x801B0364, //  23 CAREL.BIN
    (MagicEntry)0x801B0364, //  24 CAREL.BIN
    (MagicEntry)0x801B0364, //  25 CAREL.BIN
    (MagicEntry)0x801B0364, //  26 CAREL.BIN
    (MagicEntry)0x801B0364, //  27 CAREL.BIN
    (MagicEntry)0x801B0364, //  28 CAREL.BIN
    (MagicEntry)0x801B0364, //  29 CAREL.BIN
    (MagicEntry)0x801B0364, //  30 CAREL.BIN
    (MagicEntry)0x801B0364, //  31 CAREL.BIN
    (MagicEntry)0x801B0054, //  32 WPBA.BIN
    (MagicEntry)0x801B008C, //  33 WPBA.BIN
    (MagicEntry)0x801B0054, //  34 WPBA.BIN
    (MagicEntry)0x801B00C8, //  35 WPBA.BIN
    (MagicEntry)0x801B0100, //  36 WPBA.BIN
    (MagicEntry)0x801B013C, //  37 WPBA.BIN
    (MagicEntry)0x801B0174, //  38 WPBA.BIN
    (MagicEntry)0x801B01B0, //  39 WPBA.BIN
    (MagicEntry)0x801B01EC, //  40 WPBA.BIN
    (MagicEntry)0x801B0228, //  41 WPBA.BIN
    (MagicEntry)0x801B0260, //  42 WPBA.BIN
    (MagicEntry)0x801B029C, //  43 WPBA.BIN
    (MagicEntry)0x801B02D4, //  44 WPBA.BIN
    (MagicEntry)0x801B0310, //  45 WPBA.BIN
    (MagicEntry)0x801B0348, //  46 WPBA.BIN
    (MagicEntry)0x801B0384, //  47 WPBA.BIN
    (MagicEntry)0x801B0054, //  48 CAREL.BIN
    (MagicEntry)0x801B0054, //  49 CAREL.BIN
    (MagicEntry)0x801B0054, //  50 CAREL.BIN
    (MagicEntry)0x801B0054, //  51 CAREL.BIN
    (MagicEntry)0x801B0054, //  52 CAREL.BIN
    (MagicEntry)0x801B0054, //  53 CAREL.BIN
    (MagicEntry)0x801B0054, //  54 CAREL.BIN
    (MagicEntry)0x801B0054, //  55 CAREL.BIN
    (MagicEntry)0x801B0054, //  56 CAREL.BIN
    (MagicEntry)0x801B0054, //  57 CAREL.BIN
    (MagicEntry)0x801B0054, //  58 CAREL.BIN
    (MagicEntry)0x801B0054, //  59 CAREL.BIN
    (MagicEntry)0x801B0054, //  60 CAREL.BIN
    (MagicEntry)0x801B0054, //  61 CAREL.BIN
    (MagicEntry)0x801B0054, //  62 WPEA.BIN
    (MagicEntry)0x801B00C0, //  63 WPEA.BIN
    (MagicEntry)0x801B012C, //  64 WPEA.BIN
    (MagicEntry)0x801B0198, //  65 WPEA.BIN
    (MagicEntry)0x801B0204, //  66 WPEA.BIN
    (MagicEntry)0x801B0270, //  67 WPEA.BIN
    (MagicEntry)0x801B02DC, //  68 WPEA.BIN
    (MagicEntry)0x801B0348, //  69 WPEA.BIN
    (MagicEntry)0x801B03B4, //  70 WPEA.BIN
    (MagicEntry)0x801B0420, //  71 WPEA.BIN
    (MagicEntry)0x801B048C, //  72 WPEA.BIN
    (MagicEntry)0x801B0054, //  73 WPSI.BIN
    (MagicEntry)0x801B008C, //  74 WPSI.BIN
    (MagicEntry)0x801B00C4, //  75 WPSI.BIN
    (MagicEntry)0x801B00FC, //  76 WPSI.BIN
    (MagicEntry)0x801B0138, //  77 WPSI.BIN
    (MagicEntry)0x801B0170, //  78 WPSI.BIN
    (MagicEntry)0x801B01AC, //  79 WPSI.BIN
    (MagicEntry)0x801B01E4, //  80 WPSI.BIN
    (MagicEntry)0x801B021C, //  81 WPSI.BIN
    (MagicEntry)0x801B0254, //  82 WPSI.BIN
    (MagicEntry)0x801B028C, //  83 WPSI.BIN
    (MagicEntry)0x801B02C8, //  84 WPSI.BIN
    (MagicEntry)0x801B0300, //  85 WPSI.BIN
    (MagicEntry)0x801B033C, //  86 WPSI.BIN
    (MagicEntry)0x801B0054, //  87 WPYU.BIN
    (MagicEntry)0x801B008C, //  88 WPYU.BIN
    (MagicEntry)0x801B00C8, //  89 WPYU.BIN
    (MagicEntry)0x801B0100, //  90 WPYU.BIN
    (MagicEntry)0x801B013C, //  91 WPYU.BIN
    (MagicEntry)0x801B0174, //  92 WPYU.BIN
    (MagicEntry)0x801B01AC, //  93 WPYU.BIN
    (MagicEntry)0x801B01E4, //  94 WPYU.BIN
    (MagicEntry)0x801B021C, //  95 WPYU.BIN
    (MagicEntry)0x801B0258, //  96 WPYU.BIN
    (MagicEntry)0x801B0294, //  97 WPYU.BIN
    (MagicEntry)0x801B02D0, //  98 WPYU.BIN
    (MagicEntry)0x801B030C, //  99 WPYU.BIN
    (MagicEntry)0x801B0344, // 100 WPYU.BIN
    (MagicEntry)0x801B0054, // 101 WPKE.BIN
    (MagicEntry)0x801B008C, // 102 WPKE.BIN
    (MagicEntry)0x801B00C4, // 103 WPKE.BIN
    (MagicEntry)0x801B00FC, // 104 WPKE.BIN
    (MagicEntry)0x801B0134, // 105 WPKE.BIN
    (MagicEntry)0x801B016C, // 106 WPKE.BIN
    (MagicEntry)0x801B01A4, // 107 WPKE.BIN
    (MagicEntry)0x801B01DC, // 108 WPKE.BIN
    (MagicEntry)0x801B0214, // 109 WPKE.BIN
    (MagicEntry)0x801B024C, // 110 WPKE.BIN
    (MagicEntry)0x801B0284, // 111 WPKE.BIN
    (MagicEntry)0x801B02BC, // 112 WPKE.BIN
    (MagicEntry)0x801B02F4, // 113 WPKE.BIN
    (MagicEntry)0x801B0054, // 114 WPBI.BIN
    (MagicEntry)0x801B008C, // 115 WPBI.BIN
    (MagicEntry)0x801B00C4, // 116 WPBI.BIN
    (MagicEntry)0x801B00FC, // 117 WPBI.BIN
    (MagicEntry)0x801B0134, // 118 WPBI.BIN
    (MagicEntry)0x801B016C, // 119 WPBI.BIN
    (MagicEntry)0x801B01A4, // 120 WPBI.BIN
    (MagicEntry)0x801B01DC, // 121 WPBI.BIN
    (MagicEntry)0x801B0214, // 122 WPBI.BIN
    (MagicEntry)0x801B024C, // 123 WPBI.BIN
    (MagicEntry)0x801B0284, // 124 WPBI.BIN
    (MagicEntry)0x801B02BC, // 125 WPBI.BIN
    (MagicEntry)0x801B02F4  // 126 WPBI.BIN
};
// clang-format on

static s32 BattleModelReadAnimIntoMatrix(s16 arg0, s16 nItems, u8* arg2) {
    BattleModelSub* var_a0;
    s32 var_a1;
    s32 temp_s0;
    s32 temp_v0;

    var_a0 = g_BattleModels[arg0].boneTransforms;
    var_a1 = g_BattleModels[arg0].animInProgress;
    g_BattleModels[arg0].animInProgress = BattleModelReadAnimStream(var_a0, var_a1, nItems, arg2);
    return g_BattleModels[arg0].animInProgress == 0;
}

static void BattleWeaponReadAnimIntoMatrix(s16 arg0, s16 arg1, u8* arg2) {
    D_800FA6D8[arg0].unk0 = BattleModelReadAnimStream(D_800FA6D8[arg0].unk8, D_800FA6D8[arg0].unk0, arg1, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleExecuteUnitAnimScript);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD400);

static s16 BattleUnitAnimScriptReadU16(s16 arg0, u8* arg1) {
    u32 val;
    val = arg1[g_BattleModels[arg0].scriptPc++];
    return (arg1[g_BattleModels[arg0].scriptPc++] << 8) + val;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD5E4);

static void BattleModelRestoreVelocity(u8 arg0) {
    g_BattleModels[arg0].rootTrans.vx = D_80163C80[arg0].vx;
    g_BattleModels[arg0].rootTrans.vy = D_80163C80[arg0].vy;
    g_BattleModels[arg0].rootTrans.vz = D_80163C80[arg0].vz;
}

static void BattleClearEffectAndFlag(void) {
    D_801590DC = 1;
    g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD860);

static void BattleResetUnitAnimScript(s16 arg0) {
    g_BattleModels[arg0].scriptEnabled = 1;
    g_BattleModels[arg0].scriptPc = 0;
    g_BattleModels[arg0].scriptWaitFrames = 0;
}

void BattleApplyToggleFlag(void) {
    if (D_80166F68) {
        D_800F9DA4 = D_800F9DA4 | 1;
    } else {
        D_800F9DA4 = D_800F9DA4 & ~1;
    }
}

static void BattleSpawnPartEffectTick(void) {
    func_800BBA84(g_BattleDetachedSlots[g_BattleDetachedCursor].partEffect.unk4, D_801590CC, 0);
    BattleSpawnPartEffect(D_801590CC, g_BattleDetachedSlots[g_BattleDetachedCursor].partEffect.hitFlashType);
    g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
}

static void BattleApplyFacingFlipTick(void) {
    if (g_modelScreenPos[D_801590CC].prevX <
        g_modelScreenPos[g_BattleDetachedSlots[g_BattleDetachedCursor].facingFlip.actor].prevX) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].facingFlip.unk14 |= 0x100;
    }
    BattleSpawnFloatingIcon(g_BattleDetachedSlots[g_BattleDetachedCursor].facingFlip.actor,
                            g_BattleDetachedSlots[g_BattleDetachedCursor].facingFlip.unk14);
    g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
}

void BattleQueueImpactEffect(s32 arg0, s16 arg1) {
    func_800BBA84(0x14, arg1, 0);
    BattleSpawnActorRampEffect(arg0, 1, 1);
    BattleSpawnFloatingIcon(D_800FA6D4, 0x2C);
}

static void BattleFadeToGreyTick(void) {
    if (!g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft) {
        D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft--;
}

static void BattleResetModelScale(s16 arg0) {
    Unk80151200* dst = &D_80151200[arg0];
    dst->D_8015123C = 0x1000;
    dst->D_8015123A = 0x1000;
    D_80151200[arg0].D_80151238 = 0x1000;
    D_80151200[arg0].D_8015120C |= 0x20;
}

static void BattleSpawnEffectByType(s16 arg0, u8 arg1, u8 arg2) {
    s32 ret;

    BattleResetModelScale(arg0);
    switch (arg1) {
    case 0:
        ret = BattleEffectRegister(func_800C3578);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    case 8:
        ret = BattleEffectRegister(func_800C4814);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    case 1:
    case 10:
        ret = BattleMovementRegister(func_800C3950);
        g_BattleMovementSlots[ret].D_801620B2 = arg2;
        g_BattleMovementSlots[ret].D_801620B4 = arg0;
        g_BattleMovementSlots[ret].D_801620B6 = 0xF8;
        return;
    case 2:
        ret = BattleEffectRegister(func_800C3CA8);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    case 3:
        ret = BattleEffectRegister(func_800C328C);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    case 17:
    case 18:
        ret = BattleEffectRegister(func_800C40F4);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    case 7:
        ret = BattleEffectRegister(func_800C44B4);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg2;
        break;
    }
}

static void BattleReapplyEffectFromState(s16 arg0, u8 arg1) {
    s32 ret;

    BattleResetModelScale(arg0);
    switch (g_BattleData.actors[arg0].D_801636BC) {
    case 0:
        ret = BattleEffectRegister(func_800C3578);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    case 8:
        ret = BattleEffectRegister(func_800C4814);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    case 1:
    case 10:
        ret = BattleMovementRegister(func_800C3950);
        g_BattleMovementSlots[ret].D_801620B2 = arg1;
        g_BattleMovementSlots[ret].D_801620B4 = arg0;
        g_BattleMovementSlots[ret].D_801620B6 = 0xF8;
        break;
    case 2:
        ret = BattleEffectRegister(func_800C3CA8);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    case 3:
        ret = BattleEffectRegister(func_800C328C);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    case 17:
    case 18:
        ret = BattleEffectRegister(func_800C40F4);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    case 7:
        ret = BattleEffectRegister(func_800C44B4);
        g_BattleEffectSlots[ret].raw.D_80162980 = arg0;
        g_BattleEffectSlots[ret].raw.D_8016297E = arg1;
        break;
    }
}

static void BattleEffectScriptTick(void) {
    u8* ptr;
    u8 do_work;
    u32 param;
    u32 param_hi;

    if (D_80151200[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.actor].D_80151234 !=
        g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.unk2) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.framesLeft) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.framesLeft--;
        return;
    }
    do_work = 1;
    while (do_work) {
        ptr = g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.script;
        switch (ptr[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.scriptPos++]) {
        case 0xFD:
            param = ptr[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.scriptPos++];
            param_hi = ptr[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.scriptPos++];
            param_hi <<= 8;
            param |= param_hi;
            g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.framesLeft =
                ptr[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.scriptPos++];
            D_80151200[g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.actor].D_8015122E = param;
            do_work = 0;
            break;
        case 0xFE:
            g_BattleDetachedSlots[g_BattleDetachedCursor].effectScript.scriptPos = 0;
            break;
        case 0xFF:
            g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
            do_work = 0;
            break;
        default:
            do_work = 0;
            break;
        }
    }
}

static void BattleEffectDelayedCleanupTick(void) {
    switch (g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F2) {
    case 0:
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F4 = 3;
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F2++;
    case 1:
        if (g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F4 == 0) {
            g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
            func_800A3534(g_BattleDetachedSlots[g_BattleDetachedCursor].raw.unkA,
                          g_BattleDetachedSlots[g_BattleDetachedCursor].raw.unk8);
        }
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F4--;
        break;
    }
}

// Shell over a covered unit; MBarrier wins when both masks are set. Masks
// named by Akari's q-gears_reverse (ffvii/address_battle.txt): D_800FA69C is
// the magic barrier mask, D_80163608 the physical one.
static void BattleDispatchFlagEffect(void) {
    if ((D_800FA69C >> g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162980) & 1) {
        BattleTriggerActorFlashMode1(g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162980); // MBarrier
    } else if ((D_80163608 >> g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162980) & 1) {
        BattleTriggerActorFlashMode0(g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162980); // Barrier
    }
}

static void BattleFixedPointRampSpawnChildEffects(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.framesLeft) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk6 != -1) {
            BattleDispatchFlagEffect();
            dst = BattleDetachedRegister(func_800C2928);
            g_BattleDetachedSlots[dst].raw.unk14 = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unkE;
            g_BattleDetachedSlots[dst].raw.unkE = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unkA;
            g_BattleDetachedSlots[dst].raw.unk10.ptr =
                (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk8;
            dst = BattleDetachedRegister(BattleEffectDelayedCleanupTick);
            g_BattleDetachedSlots[dst].raw.unkA = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk19;
            g_BattleDetachedSlots[dst].raw.unk8 = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk6;
        }
        g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
        return;
    } else {
        g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.framesLeft--;
    }
}

void func_800CEB48(void);
void BattleFixedPointRampSpawnChildEffectsWithFade(void) {
    s32 dst;

    if (!g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.framesLeft) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unkE & 2) {
            D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        }
        if (g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk6 != -1 &&
            g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk18 != 1) {
            BattleDispatchFlagEffect();
            dst = BattleDetachedRegister(func_800C2928);
            g_BattleDetachedSlots[dst].raw.unk14 = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unkE;
            g_BattleDetachedSlots[dst].raw.unkE = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unkA;
            g_BattleDetachedSlots[dst].raw.unk10.ptr =
                (u8*)(u32)g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk8;
            dst = BattleDetachedRegister(BattleEffectDelayedCleanupTick);
            g_BattleDetachedSlots[dst].raw.unkA = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk19;
            g_BattleDetachedSlots[dst].raw.unk8 = g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.unk6;
        }
        func_800CEB48();
        return;
    } else {
        g_BattleEffectSlots[g_BattleEffectCursor].rampSpawn.framesLeft--;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CEB48);

static void BattleShakeModelYTick(void) {
    s16 index;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    g_BattleModels[index].rootTrans.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    *(s32*)0x1F80000C = index;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 - 1;
}

static void BattleShakeModelAltYTick(void) {
    s16 index;

    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    *(s32*)0x1F80000C = index;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    g_BattleModels[index].rootRot.vy += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

static void BattleShakeModelXZTick(void) {
    s16 index;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    index = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    g_BattleModels[index].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    g_BattleModels[index].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    *(s32*)0x1F80000C = index;
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 - 1;
}

static void BattleSyncModelPositionTick(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        if (IDX1 >= 4) {
            if (D_801031F0 == 0) {
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                    (MUL(g_BattleModels[IDX2].rootTrans.vy, g_BattleModels[IDX2].scale) -
                     MUL(g_BattleModels[IDX1].rootTrans.vy, g_BattleModels[IDX1].scale)) /
                    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0;
            } else {
                g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 = 0;
            }
        } else {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 = 0;
        }
    }
    if (!g_BattleMovementSlots[g_BattleMovementCursor].unk18) {
        D_80166F58 = 0;
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        g_BattleModels[IDX1].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        g_BattleModels[IDX1].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        g_BattleModels[IDX1].rootTrans.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        return;
    }
    g_BattleMovementSlots[g_BattleMovementCursor].unk18--;
#undef IDX2
#undef IDX1
#undef MUL
}

static void BattleInitModelMoveEffect(s16 arg0, s16 arg1, u8 arg2) {
    u8 dst;

    dst = BattleMovementRegister(BattleModelMoveTick);
    g_BattleMovementSlots[dst].D_801620B4 = arg0;
    g_BattleMovementSlots[dst].D_801620B0 = arg1;
    g_BattleMovementSlots[dst].D_801620B6 = D_800F99E8;
    g_BattleMovementSlots[dst].unk18 = arg2;
    if (D_800F99E8 == arg0) {
        g_BattleMovementSlots[dst].unkC = 0;
        g_BattleMovementSlots[dst].unkE = 0;
        g_BattleMovementSlots[dst].D_801620B2 = 0;
        return;
    }
    g_BattleMovementSlots[dst].unkC = (D_80163C80[arg0].vx - g_BattleModels[arg0].rootTrans.vx) / arg1;
    g_BattleMovementSlots[dst].unkE = (D_80163C80[arg0].vz - g_BattleModels[arg0].rootTrans.vz) / arg1;
    g_BattleMovementSlots[dst].D_801620B2 = (D_80163C80[arg0].vy - g_BattleModels[arg0].rootTrans.vy) / arg1;
}

static void BattleModelMoveTick(void) {
    s16 dst;

    dst = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    *(s32*)0x1F80000C = dst;
    *(s32*)0x1F800008 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    if (g_BattleMovementSlots[g_BattleMovementCursor].unk18 == 0) {
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        g_BattleModels[dst].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        g_BattleModels[dst].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        g_BattleModels[dst].rootTrans.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        return;
    }
    g_BattleMovementSlots[g_BattleMovementCursor].unk18--;
}

static void BattleModelCurveMoveTick(void) {
    s32 temp_a3;
    s16 temp_a2;
    s32 temp_a1;

    if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
        return;
    }
    temp_a2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    temp_a3 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    temp_a1 = g_BattleMovementSlots[g_BattleMovementCursor].unk10;
    *((s32*)0x1F80000C) = temp_a2;
    *((s32*)0x1F800008) = temp_a3;
    *((s32*)0x1F800010) = temp_a1;
    g_BattleModels[temp_a2].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
    g_BattleModels[temp_a2].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
    g_BattleModels[temp_a2].rootTrans.vy += D_800EEB28[temp_a1][g_BattleMovementSlots[g_BattleMovementCursor].unk18++];
    g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
}

static void BattleModelBounceTick(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    switch (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE) {
    case 0:
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 = g_BattleMovementSlots[g_BattleMovementCursor].unk19;
        break;
    case 1:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 2;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
                g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                MUL(g_BattleModels[IDX1].rootTrans.vy - g_BattleMovementSlots[g_BattleMovementCursor].unk10,
                    g_BattleModels[IDX2].scale) /
                g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
            return;
        }
        g_BattleModels[IDX1].rootTrans.vy += g_BattleMovementSlots[g_BattleMovementCursor].unk14;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    case 2:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        g_BattleModels[IDX1].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        g_BattleModels[IDX1].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        g_BattleModels[IDX1].rootTrans.vy -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

static void BattleModelSettleTick(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX2 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6;
    IDX1 = g_BattleMovementSlots[g_BattleMovementCursor].D_801620B4;
    switch (g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE) {
    case 0:
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 = g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
            (g_BattleMovementSlots[g_BattleMovementCursor].unk14 - g_BattleModels[IDX1].rootTrans.vy) /
            g_BattleMovementSlots[g_BattleMovementCursor].unk1A;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 1;
        break;
    case 1:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 =
                g_BattleMovementSlots[g_BattleMovementCursor].unk19;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2 =
                MUL(g_BattleMovementSlots[g_BattleMovementCursor].unk14 -
                        g_BattleMovementSlots[g_BattleMovementCursor].unk10,
                    g_BattleModels[g_BattleMovementSlots[g_BattleMovementCursor].D_801620B6].scale) /
                g_BattleMovementSlots[g_BattleMovementCursor].unk19;
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AE = 2;
            return;
        }
        g_BattleModels[IDX1].rootTrans.vx += g_BattleMovementSlots[g_BattleMovementCursor].unkC;
        g_BattleModels[IDX1].rootTrans.vz += g_BattleMovementSlots[g_BattleMovementCursor].unkE;
        g_BattleModels[IDX1].rootTrans.vy += g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    case 2:
        if (g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0 == 0) {
            g_BattleMovementSlots[g_BattleMovementCursor].D_801620AC = -1;
            return;
        }
        g_BattleModels[IDX1].rootTrans.vy -= g_BattleMovementSlots[g_BattleMovementCursor].D_801620B2;
        g_BattleMovementSlots[g_BattleMovementCursor].D_801620B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

static void BattleApplyDelayedFlagTick(void) {
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft == 0) {
        BattleSpawnFloatingIcon(g_BattleDetachedSlots[g_BattleDetachedCursor].delay.actor,
                                g_BattleDetachedSlots[g_BattleDetachedCursor].delay.unk6);
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft--;
}

static void BattleApplyDelayedActionTick(void) {
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft == 0) {
        func_800BBA84(g_BattleDetachedSlots[g_BattleDetachedCursor].delay.unk6, D_801590CC, 0);
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        return;
    }
    g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft--;
}

static void BattleApplyGatedDelayedTick(void) {
    if (g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft == 0) {
        if (D_801518DC == 0) {
            func_800D0C80(g_BattleDetachedSlots[g_BattleDetachedCursor].delay.unk6);
            g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        }
    } else {
        g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft--;
    }
}

static void BattleFixedPointRampRepeatTick(void) {
    if (g_BattleEffectSlots[g_BattleEffectCursor].rampRepeat.delayLeft == 0) {
        if (g_BattleEffectSlots[g_BattleEffectCursor].rampRepeat.repeatsLeft == 0) {
            g_BattleEffectSlots[g_BattleEffectCursor].raw.D_80162978 = -1;
            return;
        }
        BattleSetPendingMarkerPos(g_BattleModels[D_801590CC].currentActionId, D_80151200[D_801590CC].D_8015123E);
        g_BattleEffectSlots[g_BattleEffectCursor].rampRepeat.repeatsLeft--;
        return;
    }
    g_BattleEffectSlots[g_BattleEffectCursor].rampRepeat.delayLeft--;
}

void BattleLoadOverlaySector(s32 loc, s32 len) {
    SystemLoadFileBySector(loc, len, (u_long*)0x801B0000, NULL);
    BattleCdromReadChain();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleStorePlayerAnimScriptsPtr);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", BattleRestorePlayerAnimScriptsPtr);

static void BattleNoOpTick(void) {}

static void BattleEffectTimeoutTick(void);
void BattleInitMagicCastEffect(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < 3; i++) {
        g_BattleModels[i].specialFlags |= 1;
    }
    func_801B0040(g_BattleCurrentTargetMask, D_801590CC);
    ret = BattleDetachedRegister(BattleEffectTimeoutTick);
    *(s32*)0x1F800000 = ret;
    g_BattleDetachedSlots[ret].delay.framesLeft = 2;
}

static void BattleEffectTimeoutTick(void) {
    if (!g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft) {
        g_BattleDetachedSlots[g_BattleDetachedCursor].raw.D_801621F0 = -1;
        func_800BB978();
        return;
    }
    g_BattleDetachedSlots[g_BattleDetachedCursor].delay.framesLeft--;
}

static void BattleDispatchModelRunScript(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (g_BattleModels[arg0].currentActionId) {
    case CMD_ITEM:
        D_800EF9D8[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
        break;
    case CMD_COIN:
        // Coin: always WPYU.BIN, via the loader's fixed D_800EEBB8[221].
        func_801B037C(g_BattleCurrentTargetMask, D_801590CC);
        break;
    case CMD_THROW:
        D_800EFFE0[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
        break;
    }
}

void func_800D0C80(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (g_BattleModels[arg0].currentActionId) {
    case CMD_MAGIC:
        if (D_801031F0 == 0) {
            if (g_BattleModels[arg0].attackEffectId == 25) {
                g_BattleModels[0].ready = 1;
                g_BattleModels[1].ready = 1;
                g_BattleModels[2].ready = 1;
            }
            D_800EFAF0[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
            return;
        }
        switch (g_BattleModels[arg0].attackEffectId) {
        case 41:
            // BIOGA2.BIN
            func_801B0000(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 44:
            // GRAVIGA2.BIN
            func_801B0000_2(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 35:
            // THUNDG2.BIN
            func_801B000C(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 32:
            // BRIZAG2.BIN
            func_801B0054(g_BattleCurrentTargetMask, D_801590CC);
            break;
        case 29:
            // FAIGA2.BIN
            func_801B0084(g_BattleCurrentTargetMask, D_801590CC);
            break;
        default:
            D_800EFAF0[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
            break;
        }
        break;
    case CMD_ENEMY_SKILL:
        D_800EFBC8[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
        break;
    case CMD_LIMIT:
        if (g_BattleModels[arg0].attackEffectId == 2) {
            if (D_801590CC == g_BattleQueueTargets[2].targetId) {
                D_80163A98 = 0;
            } else {
                D_80163A98 = 1;
            }
        }
        *(s32*)0x1F800000 = D_800EFEA0[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
        switch (g_BattleModels[arg0].attackEffectId) {
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4F:
            *(s32*)0x1F800000 = 0;
            break;
        }
        func_800D08B8(arg0, *(s32*)0x1F800000);
        break;
    case CMD_ENEMY_ATTACK:
        D_800EFC28[g_BattleModels[arg0].attackEffectId](g_BattleCurrentTargetMask, D_801590CC);
        break;
    case CMD_SUMMON:
        func_800C64AC();
        break;
    }
}

static void BattleDispatchModelLoadImage(u8 arg0) {
    s32 lba;
    s32 var_a1;
    s32 id;

    switch (g_BattleModels[arg0].currentActionId) {
    case CMD_MAGIC:
        if (D_801031F0 == 0) {
            id = D_800EF63C[g_BattleModels[arg0].attackEffectId];
            BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        } else {
            switch (g_BattleModels[arg0].attackEffectId) {
            case 29:
                BattleLoadOverlaySector(D_800EEBB8[77].loc, D_800EEBB8[77].len);
                break;
            case 41:
                BattleLoadOverlaySector(D_800EEBB8[32].loc, D_800EEBB8[32].len);
                break;
            case 44:
                BattleLoadOverlaySector(D_800EEBB8[26].loc, D_800EEBB8[26].len);
                break;
            case 32:
                BattleLoadOverlaySector(D_800EEBB8[255].loc, D_800EEBB8[255].len);
                break;
            case 35:
                BattleLoadOverlaySector(D_800EEBB8[6].loc, D_800EEBB8[6].len);
                break;
            default:
                id = D_800EF63C[g_BattleModels[arg0].attackEffectId];
                lba = D_800EEBB8[id].loc;
                var_a1 = D_800EEBB8[id].len;
                BattleLoadOverlaySector(lba, var_a1);
                break;
            }
        }
        break;
    case CMD_COIN:
        BattleLoadOverlaySector(D_800EEBB8[221].loc, D_800EEBB8[221].len);
        break;
    case CMD_THROW:
        id = D_800EF8D8[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case CMD_ENEMY_SKILL:
        id = D_800EF6A8[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case CMD_LIMIT:
        id = D_800EF838[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case CMD_ITEM:
        id = D_800EF5B0[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case CMD_ENEMY_ATTACK:
        id = D_800EF6FC[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case CMD_SUMMON:
        id = D_800EF6D8[g_BattleModels[arg0].attackEffectId];
        BattleLoadOverlaySector(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D1530);

void BattleEnqueueLoadImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_LOAD_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

static void BattleEnqueueStoreImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_STORE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

static void BattleEnqueueMoveImage(RECT* rect, s32 x, s32 y) {
    D_800F01DC->method = QUEUE_MOVE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->x = x;
    D_800F01DC->y = y;
    D_800F01DC++;
}

void BattleEnqueueClearImage(RECT* rect, s32 arg1, s32 arg2, s32 arg3) {
    D_800F01DC->method = QUEUE_CLEAR_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC++;
}

void BattleFlushImageQueue(void) {
    Unk800F01DC* item;

    for (item = D_800F4BAC; item < D_800F01DC; item++) {
        switch (item->method) {
        case QUEUE_LOAD_IMAGE:
            LoadImage(item->rect, item->ptr);
            break;
        case QUEUE_STORE_IMAGE:
            StoreImage(item->rect, item->ptr);
            break;
        case QUEUE_MOVE_IMAGE:
            MoveImage(item->rect, item->x, item->y);
            break;
        case QUEUE_CLEAR_IMAGE:
            ClearImage(item->rect, 0, 0, 0);
            break;
        }
    }
    D_800F01DC = D_800F4BAC;
}

static void BattleResetImageQueue(void) { D_800F01DC = D_800F4BAC; }

static void BattleSetLoadClutToVram(u_long* addr, s16 x, s16 y) {
    TIM_IMAGE tim;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.crect && tim.caddr) {
        D_800F4B2C[D_800F01E0] = *tim.crect;
        D_800F4B2C[D_800F01E0].x += x & ~15;
        D_800F4B2C[D_800F01E0].y = y + D_800F4B2C[D_800F01E0].y; // requires GCC 2.6.3
        BattleEnqueueLoadImage(&D_800F4B2C[D_800F01E0], tim.caddr);
        D_800F01E0 = (D_800F01E0 + 1) & 7;
    }
}

static void BattleSetLoadTextureToVram(u_long* addr, s32 xy) {
    TIM_IMAGE tim;
    s32 temp_a1;
    s32 temp_a3;
    s32 temp_a2;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.prect && tim.paddr) {
        D_800F4B6C[D_800F01E4] = *tim.prect;
        temp_a1 = (tim.prect->y & 0x300) >> 4 | (tim.prect->x & 0x3FF) >> 6;
        temp_a2 = temp_a1 + xy;
        temp_a3 = (temp_a1 & 0x0F) * 0x40;
        D_800F4B6C[D_800F01E4].x = ((temp_a2 & 0x0F) * 0x40 + (D_800F4B6C[D_800F01E4].x - temp_a3)) & 0x3FF;
        temp_a3 = (temp_a1 & 0x30) * 0x10;
        D_800F4B6C[D_800F01E4].y = ((temp_a2 & 0x30) * 0x10 + (D_800F4B6C[D_800F01E4].y - temp_a3)) & 0x1FF;
        BattleEnqueueLoadImage(&D_800F4B6C[D_800F01E4], tim.paddr);
        D_800F01E4 = (D_800F01E4 + 1) & 7;
    }
}

void BattleSetLoadTimToVram(u_long* addr, s16 imgXY, s16 clutX, s16 clutY) {
    BattleSetLoadClutToVram(addr, clutX, clutY);
    BattleSetLoadTextureToVram(addr, imgXY);
}
