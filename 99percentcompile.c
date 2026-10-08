
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;
void ArrangeItems(s32);
const char *func_80015248(s32, s32, s32);
void func_8001786C(u8);
void func_8001CF3C(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void func_8001DE0C(s32 *, s32, s32, s32, s32);
void func_8001E040(s32 *);
void func_8001EB2C(s32, s32);
void func_8001F6C0(s32 *, s32);
void func_80020058(s8);
void func_80020B68(s32, s32, s32);
void func_8002120C(s32);
s32 func_80023050();
void func_8002305C(s32, s32);
void func_800230C4(s32);
void func_80023AC4();
void func_80025288(s32);
s32 func_80025310(u32);
void func_800258BC(s32, s32);
void func_80025A44(s32, s32);
typedef struct 
{
  u8 pad[0x12];
} Unk80026448;
void func_80026448(Unk80026448 *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, u16);
void func_800264A8(void *);
void func_80026A34(s32, s32, s32, s16 *);
void func_80026A94(void *, s16 *);
void func_80026B5C(s32);
void func_80026F44(s32, s32, const char *, s32);
void func_8002708C(s32, s32, s32, s32);
void func_801D0228();
void func_80028484();
void func_80028E00(s32, s32, u32, s32, s32);
void func_801D01E8(s32);
s32 func_801D0CAC(s32);
s32 func_801D0CE8(s32);
s32 func_801D0D24(u8);
s32 func_801D0DCC(s32);
void func_801D0E4C(s32 *);
extern u16 D_80062D7C;
extern u16 D_80062D7E;
extern s32 D_80062F58;
typedef struct 
{
  u8 pad[0x5C];
} DRAWENV_t;
extern DRAWENV_t D_800706A4[2];
extern u8 D_8009C740[];
extern u8 D_8009C744[];
extern u8 D_8009C757[];
extern u16 D_8009C75A[];
extern s8 D_8009CA50;
extern s8 D_8009CA51;
extern s8 D_8009CA5E;
extern u8 D_8009CA5F;
extern s32 D_8009CA8C;
extern s8 D_8009CAD4;
extern s8 D_8009CAD5;
extern s8 D_8009CAE2;
extern u8 D_8009CAE3;
extern s32 D_8009CB10;
extern u8 D_8009CBCF[];
extern u8 D_8009CBDC[];
extern u16 D_8009CBE0[];
extern u8 D_8009D5E8;
extern s16 D_8009D85C[];
extern s16 D_8009D85E[];
extern s32 D_801D3282;
extern s32 D_801D3590;
extern s32 D_801D3CD4;
extern s32 D_801D3CF8;
extern unsigned char D_801D3D25[];
extern s32 D_801D3D5C;
extern s32 D_801D3D74;
extern s16 D_801D3D76;
extern s32 D_801D3D84;
extern s32 D_801D3D88;
extern s32 D_801D3D8C;
extern u8 D_801D3D90[];
extern u8 D_801D3DDC[];
extern unsigned char D_801D3DE4[];
extern s8 D_801D3DE6;
extern s8 D_801D3DEB[];
extern s16 D_801D3DF0;
extern s16 D_801D3DF6;
extern s8 D_801D3DF9[];
extern s8 D_801D3E0B[];
extern s16 D_801D3E14;
extern s8 D_801D3E1C[];
extern s8 D_801D3E1D;
extern s8 D_801D3E21;
extern s8 D_801D3E2F[];
extern s16 D_801D3E38;
extern s8 D_801D3E40;
extern s8 D_801D3E41[];
extern s8 D_801D3E45;
extern u32 D_801D3E48;
extern s16 D_801D3E4C[];
extern s16 D_801D3E4E;
extern u16 D_801D3E50;
extern s16 D_801D3E52;
extern s16 D_801D3E54;
extern s16 D_801D3E56;
extern s16 D_801D3E58;
extern s32 D_801D3E5C;
extern u8 D_801D3E60[];
void func_801D3260();



char func_801D0E80(s32 arg0)
{
  unsigned char temp_s3;
  s16 *new_var3;
  s32 sp38[2];
  s16 sp40[12];
  s32 *temp_a2;
  s32 *var_a0_2;
  s8 *var_s1_3;
  s8 *var_s1_4;
  s32 temp_a0_10;
  s8 *var_s2;
  int new_var6;
  unsigned char new_var11;
  short new_var13;
  s32 var_a0;
  s32 temp_a0_11;
  s32 temp_a0_4;
  s32 temp_a0_5;
  s32 temp_a0_6;
  s32 temp_a0_7;
  s32 temp_a0_8;
  s32 temp_a0_9;
  s8 *new_var16;
  s32 var_s0_2;
  s32 temp_s1_2;
  s32 temp_s3_2;
  s32 temp_v1;
  s32 temp_v1_2;
  s32 temp_v1_6;
  s8 *var_s0;
  //s32 temp_e5c;
  Unk80026448 *new_var5;
  s32 var_s1;
  s32 var_s1_2;
  s32 var_s1_5;
  s32 var_s1_7;
  int new_var4;
  s32 var_s2_4;
  int new_var17;
  s32 var_s2_5;
  int new_var8;
  s32 var_s2_6;
  s32 var_s3;
  s32 var_s4;
  s32 *new_var10;
  s32 temp_loopval;
  int new_var19;
  s32 var_loopc;
  s32 var_s5;
  s32 temp_bool2;
  s32 var_s6;
  s32 var_v0;
  int new_var9;
  int region_e_cond;
  s32 var_v0_3;
  s32 var_v0_4;
  s32 var_v0_7;
  s32 var_v0_8;
  s32 temp_s1_b;
  int new_var2;
  s32 temp_bool;
  Unk80026448 *new_var18;
  s32 temp_s2_b;
  int new_var15;
  s32 var_v0_2;
  s8 var_v0_5;
  s8 var_v0_6;
  u16 *temp_v1_13;
  u16 temp_a0;
  u16 temp_a0_12;
  u16 temp_a0_2;
  u16 temp_a1;
  u32 temp_a1_2;
  u16 temp_v1_3;
  int new_var14;
  u32 temp_s1;
  unsigned int temp_a0_3;
  u8 temp_a1_3;
  u8 temp_v1_10;
  int new_var12;
  u8 temp_v1_11;
  u8 temp_v1_12;
  u8 temp_v1_4;
  int new_var7;
  u8 temp_v1_5;
  u8 temp_v1_7;
  u8 temp_v1_8;
  u8 temp_v1_9;
  u16 *new_var;
  unsigned short var_a1;
  func_800230C4(D_80062F58);
  if (D_801D3E48 == 2)
  {
    //temp_e5c = D_801D3E5C;
    if (D_801D3E5C == 0)
    {
      temp_v1 = D_8009CBE0[D_801D3DF9[0] + D_801D3DF0] & 0x1FF;
      if (((temp_v1 == 6) || (temp_v1 == 0x46)) != 0)
      {
        var_v0_2 = arg0;
        var_v0_2 = var_v0_2 % 3;
        func_8001EB2C(0, (var_v0_2 * 0x38) + 0x4B);
      }
      else
      {
        var_v0_2 = D_801D3E0B[0];
        func_8001EB2C(0, (var_v0_2 * 0x38) + 0x4B);
      }
    }
    if ((arg0 & 2) != 0)
    {
      func_8001EB2C(0xA9, (D_801D3DF9[0] * 0x10) + 0x3C);
    }
    if (D_801D3E5C)
    {
      D_801D3E5C -= 1;
    }
  }
  func_80026B5C(0x80);
  switch (D_801D3E48)
  {
    case 0:
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
      break;

    case 1:
      if (arg0 & 2)
    {
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
    }
      func_8001EB2C(0xA9, (D_801D3DF9[0] * 0x10) + 0x3C);
      var_s4 = D_801D3DF9[0] + D_801D3DF0;
      goto block_33;

    case 2:
      if (arg0 & 2)
    {
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
    }
      var_s4 = D_801D3DF9[0] + D_801D3DF0;
      goto block_33;

    case 3:
      if (arg0 & 2)
    {
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
    }
      func_8001EB2C((D_801D3E1C[0] * 0xA6) + 3, (D_801D3E1D * 0x10) + 0x3C);
      new_var14 = 0xFF;
      var_s4 = ((D_801D3E1D + D_801D3E14) * 2) + D_801D3E1C[0];
      var_a1 = D_801D3D90[var_s4];
      var_a0 = 0xE;
      if (var_a1 != new_var14)
    {
      do
      {
      }
      while (0);
      goto block_35;
    }
      break;

    case 4:
      new_var2 = 8;
      if (arg0 & 2)
    {
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
      //var_s0 = 0;
    }
      var_s0 = 0;
    {
      s16 *w = (s16 *) (&D_801D3D74);
      var_s2 = (s8 *) (&D_801D3CF8);
      var_s1 = 6;
      func_8001EB2C(w[0] - 0x12, D_801D3D76 + ((D_801D3E2F[0] * 0xC) + new_var2));
      do
      {
        new_var3 = w;
        temp_a2 = (s32 *) var_s2;
        func_80026F44(w[0] + 8, new_var3[1] + var_s1, temp_a2, 7);
        var_s2 += 0xC;
        var_s0 += 1;
        var_s1 += 0xC;
      }
      while (((s32) var_s0) < 8);
      sp40[0] = 0;
      sp40[1] = 0;
      sp40[2] = 0x100;
      sp40[3] = 0x100;
      func_80026A34(0, 1, 0x7F, sp40);
      func_8001E040((s32 *) (&D_801D3D74));
    }

        break;

    case 5:
      if (arg0 & 2)
    {
      func_8001EB2C((D_801D3DE6 * 0x38) + 8, 0xC);
    }
      var_s4 = D_801D3E41[0] + D_801D3E38;
      block_33:
    var_a1 = D_8009CBE0[var_s4];

      var_a0 = 4;
      if ((var_a1 & 0xFFFF) != 0xFFFF)
    {
      var_a1 = var_a1 & 0x1FF;
      block_35:
      func_80026F44(0x10, 0x23, func_80015248(var_a0, var_a1, 0), 7);

    }

  }

  func_80026B5C(8);
  sp40[0] = 0;
  sp40[1] = 0;
  sp40[2] = 0x100;
  sp40[3] = 0x100;
  func_80026A34(0, 1, 0x7F, sp40);
  var_s1_2 = 0xD;
  if (D_801D3DE6 != 2)
  {
    var_s0 = 0;
    var_s6 = 0x30;
    var_s5 = 0x100;
    var_s4 = 0x38;
    var_s3 = 0x36;
    var_s2 = (s8 *) 0x3B;
    do
    {
      if (D_8009CBCF[var_s1_2] != 0xFF)
      {
        func_80020B68(0x50, (s32) var_s2, (s32) var_s0);
        func_8001CF3C(0x16, var_s3, 0x30, 0x30, 0, var_s4, var_s6, var_s6, var_s1_2, 0);
        sp40[0] = 0;
        sp40[1] = 0;
        sp40[2] = var_s5;
        sp40[3] = var_s5;
        func_80026A34(0, 1, 0x7F, sp40);
      }
      var_s1_2 = var_s1_2 + 1;
      var_s4 += 0x30;
      var_s3 += 0x38;
      var_s0 += 1;
      var_s2 += 0x38;
    }
    while (((s32) var_s0) < 3);
    func_8001DE0C(sp38, 0, 0x32, 0xAA, 0xAB);
    func_8001E040(sp38);
  }
  var_s2 = (s8 *) 0;
  var_s1_3 = (s8 *) (&D_801D3CD4);
  var_s0 = (s8 *) 0x22;
  do
  {
    func_80026F44((s32) var_s0, 0xD, var_s1_3, 7);
    var_s1_3 += 0xC;
    var_s2 += 1;
    var_s0 += 0x38;
  }
  while (((s32) var_s2) < 3);
  sp40[2] = 0x16C;
  sp40[3] = 0xE0;
  sp40[0] = 0;
  sp40[1] = 0;
  func_80026A94((void *) (((u8 *) D_800706A4) + (D_80062F58 * 0x5C)), sp40);
  if (D_801D3DE6 != 2)
  {
    if (D_801D3E48 == 5)
    {
      if ((D_801D3D84 != 0) && (arg0 & 2))
      {
        temp_v1_2 = ((D_801D3D8C - D_801D3E38) * 0x10) + (D_801D3E45 * 4);
        if (((u32) (temp_v1_2 + 0xB)) < 0x10FU)
        {
          func_8001EB2C(0xA5, temp_v1_2 + 0x38);
        }
      }
      func_8001EB2C(0xA9, (D_801D3E41[0] * 0x10) + 0x3C);
      var_s5 = 5;
    }
    else
    {
      var_s5 = 1;
    }
    do
    {
      D_801D3E4C[0] = 0xA;
      D_801D3E4E = 0x140;
    }
    while (0);
    var_s0 = (s8 *) (var_s5 * 0x12);
    temp_a1_2 = *((u16 *) ((D_801D3DDC + 2) + ((s32) var_s0)));
    D_801D3E52 = 0x160;
    D_801D3E54 = 0x35;
    D_801D3E56 = 0xA;
    D_801D3E58 = 0xA5;
    D_801D3E50 = temp_a1_2;
    var_s6 = 0xA;
    func_80028484(D_801D3E4C, temp_a1_2);
    temp_bool2 = *((s16 *) (((u8 *) D_801D3DE4) + ((s32) var_s0)));
    if (temp_bool2 != 0)
    {
      var_s6 = 0xB;
    }
    func_80026B5C(9);
    var_s2_4 = 0;
    if (var_s6 != 0)
    {
      do
      {
        new_var6 = 0x3A;
        var_s1 = (*((s16 *) ((D_801D3DDC + 2) + ((s32) var_s0)))) + var_s2_4;
        temp_a0 = *(D_8009CBE0 - (-var_s1));
        if ((temp_a0 & 0xFFFF) != 0xFFFF)
        {
          var_s4 = temp_a0 & 0x1FF;
          var_s3 = (-((func_801D0DCC(var_s4) & 4) == 0)) & 7;
          func_80026F44(0xD6, (var_s2_4 * 0x10) + ((D_801D3DEB[(s32) var_s0] * 4) + new_var6), func_80015248(4, var_s4, 8), var_s3);
        }
        var_s2_4 += 1;
      }
      while (var_s2_4 < var_s6);
    }
    var_s2_4 = 0;
    if (var_s6 != 0)
    {
      var_s5 *= 0x12;
      do
      {
        var_s1_2 = (*((s16 *) ((D_801D3DDC + 2) + var_s5))) + var_s2_4;
        temp_v1_3 = *(D_8009CBE0 + var_s1_2);
        var_s1_2 = temp_v1_3 & 0xFFFF;
        if (var_s1_2 != 0xFFFF)
        {
          var_s4 = temp_v1_3 & 0x1FF;
          new_var19 = func_801D0DCC(var_s4) & 4;
          var_s1_2 = (s32) (((u32) var_s1_2) >> 9);
          var_s3 = (-(new_var19 == 0)) & 7;
          ((void (*)()) func_801D0228)(0xC4, ((s8 *) (var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x38), var_s4, 0);
          func_8002708C(0x13F, (s32) (((s8 *) (var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x3C)), 0xD5, var_s3);
          func_80028E00(0x140, (s32) (((s8 *) (var_s2_4 * 0x10)) + ((D_801D3DEB[var_s5] * 4) + 0x3B)), var_s1_2, 3, var_s3);
        }
        var_s2_4 += 1;
      }
      while (var_s2_4 < var_s6);
    }
  }
  else
  {
    var_s6 = 0xA;
    D_801D3E4C[0] = 0xA;
    D_801D3E4E = 0x20;
    D_801D3E56 = 0xA;
    D_801D3E52 = 0x160;
    D_801D3E54 = 0x35;
    D_801D3E58 = 0xA5;
    D_801D3E50 = (u16) D_801D3E14;
    var_s2_6 = 0;
    var_s4 = 0x38;
    do
    {
    }
    while (0);
    func_80028484(D_801D3E4C);
    func_80026B5C(9);
    var_s0 = 0;
    do
    {
      var_s5 = var_s2_6 * 0x10;
      var_s3 = 0x20;
      var_s1 = (D_801D3E14 + var_s2_6) * 2;
      loop_66:
      var_s4 = var_s1 + ((s32) var_s0);

      temp_a1_3 = D_801D3D90[var_s4];
      if (temp_a1_3 != 0xFF)
      {
        func_80026F44(var_s3, var_s5 + (new_var17 = (D_801D3E21 * 4) + 0x3A), func_80015248(0xE, temp_a1_3, 8), 7);
      }
      var_s0 += 1;
      var_s3 += 0xA6;
      if (((s32) var_s0) < 2)
      {
        goto loop_66;
      }
      var_s2_6 += 1;
      var_s0 = 0;
    }
    while (((s32) var_s2_6) < 0xC);
  }
  temp_s3 = 0x35;
  sp40[1] = temp_s3;
  sp40[2] = 0x16C;
  sp40[3] = 0xA5;
  sp40[0] = 0;
  func_80026A94((void *) (((u8 *) D_800706A4) + (D_80062F58 * 0x5C)), sp40);
  var_s0 = 0;
  var_s1_4 = (s8 *) (&D_801D3D5C);
  do
  {
    func_8001E040(var_s1_4);
    var_s0 += 1;
    var_s1_4 = var_s1_4 + 8;
  }
  while (((s32) var_s0) < 3);
  if (func_80023050() == 0)
  {
    func_800264A8(D_801D3DDC + (D_801D3E48 * 0x12));
    switch (D_801D3E48)
    {
      case 0:
        if (D_80062D7C & 0x20)
      {
        func_801D01E8(1);
        new_var5 = (Unk80026448 *) (&D_801D3DE6);
        switch (*((s8 *) new_var5))
        {
          case 0:
            D_801D3E48 = 1;
            return;

          case 1:
            func_80026448((Unk80026448 *) (((s8 *) new_var5) + 0x3E), 0, 0, 1, 8, 0, 0, (s32) (*((s8 *) new_var5)), 8, 0, 0, 0, (s32) (*((s8 *) new_var5)), 0);
            D_801D3E48 = 4;
            return;

          case 2:
            func_80026448((Unk80026448 *) (((s8 *) new_var5) + 0x2C), 0, 0, 2, 0xA, 0, 0, (s32) (*((s8 *) new_var5)), 0x20, 0, 0, (s32) (*((s8 *) new_var5)), 0, 0);
            D_801D3E48 = 3;
            return;

        }

      }
      else
        if (D_80062D7E & 0x40)
      {
        func_801D01E8(4);
        func_8002305C(5, 0);
        func_8002120C(0);
        return;
      }
        break;

      case 1:
        if (D_801D3DF6 == 0)
      {
        if (D_80062D7C & 0x20)
        {
          var_s4 = D_801D3DF9[0] + D_801D3DF0;
          temp_a0_2 = D_8009CBE0[var_s4];
          if (((temp_a0_2 & 0xFFFF) != 0xFFFF) && (!(func_801D0DCC(var_s4 = temp_a0_2 & 0x1FF) & 4)))
          {
            if (var_s4 != 0x62)
            {
              if (var_s4 == 0x67)
              {
                func_801D01E8(0x107);
                D_8009CA51 = 1;
                D_8009CA5E = 1;
                D_8009CA50 = 6;
                D_8009CA5F = 0xFF;
                D_8009CA8C = 0xFFFFFF;
                D_8009CAD5 = 1;
                D_8009CAD4 = 7;
                D_8009CAE2 = 1;
                D_8009CAE3 = 0xFF;
                D_8009CB10 = 0xFFFFFF;
                return;
              }
              goto block_e48;
            }
            func_801D01E8(0x107);
            D_8009D5E8 |= 1;
            func_8002305C(5, 0);
            func_8002120C(0);
            func_80023AC4();
            return;
            block_e48:
            func_801D01E8(1);

            D_801D3E5C = 0;
            D_801D3E48 = 2;
            return;
          }
          func_801D01E8(3);
          return;
        }
        var_v0_3 = D_80062D7C & 0x40;
        goto block_217;
      }
        break;

      case 2:
        if (D_801D3E5C == 0)
      {
        if (D_80062D7C & 0x20)
        {
          temp_a0_4 = D_8009CBDC[D_801D3E0B[0]];
          var_s4 = D_8009CBE0[D_801D3DF9[0] + D_801D3DF0] & 0x1FF;
          new_var4 = var_s4 < 0x5FU;
          temp_a0_3 = temp_a0_4;
          var_v0_4 = new_var4;
          if (temp_a0_3 == 0xFF)
          {
            if ((var_s4 != 6) && (var_s4 != 0x46))
            {
              func_801D01E8(3);
              return;
            }
          }
          {
            switch (var_s4)
            {
              case 0xD:
                temp_a0_4 = temp_a0_3 * 0x84;
                temp_v1_4 = D_8009C757[temp_a0_4];
                if (!(temp_v1_4 & 0x20))
              {
                if (!(temp_v1_4 & 0x10))
                {
                  var_v0_5 = temp_v1_4 | 0x20;
                }
                else
                {
                  var_v0_5 = temp_v1_4 & 0xEF;
                }
                D_8009C757[temp_a0_4] = var_v0_5;
                func_801D01E8(0x107);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0xE:
                temp_a0_5 = temp_a0_3;
                temp_a0_5 *= 0x84;
                temp_v1_5 = D_8009C757[temp_a0_5];
                region_e_cond = 0x20;
                region_e_cond = (temp_v1_5 & region_e_cond) != 0;
                if (region_e_cond || ((temp_v1_5 & 0x10) == 0))
              {
                var_v0_6 = (region_e_cond) ? (temp_v1_5 & 0xDF) : (temp_v1_5 | 0x10);
                D_8009C757[temp_a0_5] = var_v0_6;
                func_801D01E8(0x107);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x57:

              case 0x58:

              case 0x59:

              case 0x5A:

              case 0x5B:

              case 0x5C:

              case 0x5D:

              case 0x5E:
                if (temp_a0_3 == D_801D3D25[var_s4])
              {
                if (func_801D0D24(temp_a0_3) != 0)
                {
                  func_801D01E8(0x180);
                  temp_v1_6 = D_801D3D25[var_s4] * 0x84;
                  *((u16 *) (((u8 *) D_8009C75A) + temp_v1_6)) = (*((u16 *) (((u8 *) D_8009C75A) + temp_v1_6))) | 0x200;
                  func_80025288(var_s4 | 0x200);
                  ;
                  if ((func_80025310(var_s4) & 0xFFFF) == 0xFFFF)
                  {
                    D_801D3E48 = 1;
                  }
                  func_801D0E4C(((var_s4 - 0x57) * 0x66) + (&func_801D3260));
                  func_8001F6C0((s32 *) D_801D3E60, 7);
                  return;
                }
                var_a0_2 = (s32 *) (((var_s4 - 0x57) * 0x66) + ((s8 *) (&D_801D3282)));
                func_801D0E4C(var_a0_2);
                func_8001F6C0((s32 *) D_801D3E60, 7);
                func_801D01E8(3);
                return;
              }
                if (temp_a0_3 == 6)
              {
                var_a0_2 = &D_801D3590;
              }
              else
              {
                new_var6 = 6;
                if (((s32) temp_a0_3) >= new_var6)
                {
                  var_v0_8 = (temp_a0_3 - 1) * 3;
                }
                else
                {
                  var_v0_8 = temp_a0_3 * 3;
                }
                var_a0_2 = (s32 *) (((var_v0_8 + 2) * 0x22) + ((s8 *) (&func_801D3260)));
              }
                func_801D0E4C(var_a0_2);
                func_8001F6C0((s32 *) D_801D3E60, 7);
                func_801D01E8(3);
                return;

              case 0x47:

              case 0x48:

              case 0x49:

              case 0x4A:

              case 0x4B:

              case 0x4C:
                switch (var_s4)
              {
                case 0x47:
                  temp_a0_6 = temp_a0_3 * 0x84;
                  temp_v1_7 = D_8009C740[temp_a0_6];
                  if (temp_v1_7 < 0xFFU)
                {
                  D_8009C740[temp_a0_6] = temp_v1_7 - (-1);
                  default:
                    goto src_tail;

                }
                else
                {
                  func_801D01E8(3);
                  return;
                }
                  break;

                case 0x48:
                  temp_a0_7 = 33 * (4 * temp_a0_3);
                  temp_v1_8 = D_8009C740[1 + temp_a0_7];
                  if (temp_v1_8 < 0xFFU)
                {
                  D_8009C740[1 + temp_a0_7] = temp_v1_8 + 1;
                  goto src_tail;
                  func_801D01E8(3);
                }
                  func_801D01E8(3);
                  return;

                case 0x49:
                  temp_a0_8 = temp_a0_3 * 0x84;
                  temp_v1_9 = D_8009C740[2 + temp_a0_8];
                  if (temp_v1_9 < 0xFFU)
                {
                  D_8009C740[2 + temp_a0_8] = temp_v1_9 + 1;
                  goto src_tail;
                }
                  func_801D01E8(3);
                  return;

                case 0x4A:
                  temp_a0_9 = temp_a0_3 * 0x84;
                  temp_v1_10 = D_8009C740[3 + temp_a0_9];
                  if (temp_v1_10 < 0xFFU)
                {
                  D_8009C740[3 + temp_a0_9] = temp_v1_10 + 1;
                  goto src_tail;
                }
                  func_801D01E8(3);
                  return;

                case 0x4B:
                  temp_a0_10 = temp_a0_3 * 0x84;
                  temp_v1_11 = D_8009C744[temp_a0_10];
                  if (temp_v1_11 < 0xFFU)
                {
                  D_8009C744[temp_a0_10] = temp_v1_11 + 1;
                  goto src_tail;
                }
                  func_801D01E8(3);
                  return;

                case 0x4C:
                  temp_a0_11 = temp_a0_3 * 0x84;
                  temp_v1_12 = D_8009C744[1 + temp_a0_11];
                  if (temp_v1_12 < 0xFFU)
                {
                  D_8009C744[1 + temp_a0_11] = temp_v1_12 + 1;
                  src_tail:
                  func_801D01E8(0x107);

                  func_80020058(D_801D3E0B[0]);
                  func_8001786C(*((u8 *) D_801D3E0B));
                  func_80025288(var_s4 | 0x200);
                  if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                  {
                    return;
                  }
                  D_801D3E48 = 1;
                  return;
                }
                  func_801D01E8(3);
                  return;

              }

                break;

              case 0x0:
                if ((func_801D0CAC(D_801D3E0B[0]) == 0) && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_800258BC(D_801D3E0B[0], 0x64);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x1:
                if ((func_801D0CAC(D_801D3E0B[0]) == 0) && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_800258BC(D_801D3E0B[0], 0x1F4);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x3:
                temp_s3 = func_801D0CE8(D_801D3E0B[0]) == 0;
                var_s2_4 = (new_var7 = 0);
                if (temp_s3 && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[var_s2_4] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_80025A44(D_801D3E0B[0], 0x64);
                func_80025288(var_s4 | 0x200);
                if (0xFFFF != (func_80025310(var_s4) & 0xFFFF))
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x4:
                if ((func_801D0CE8(D_801D3E0B[0]) == 0) && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_80025A44(D_801D3E0B[0], 0x2710);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x7:
                if ((*((s16 *) (((u8 *) D_8009D85C) + (0x440 * D_801D3E0B[0])))) == 0)
              {
                func_801D01E8(0x107);
                func_800258BC(D_801D3E0B[0], (*((s16 *) (((u8 *) D_8009D85E) + (D_801D3E0B[0] * 0x440)))) / 4);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x46:
                var_s0 = 0;
                var_s1_5 = 0;
                if (var_s4)
              {
              }
                do
              {
                if ((D_8009CBDC[(s32) var_s0] != 0xFF) && ((func_801D0CAC((s32) var_s0) == 0) || (func_801D0CE8((s32) var_s0) == 0)))
                {
                  var_s1_5 = 1;
                }
                var_s0 += 1;
              }
              while (((s32) var_s0) < 3);
                var_s0 = 0;
                if (var_s1_5 != 0)
              {
                new_var15 = 0xFF;
                var_s1_5 = 0;
                do
                {
                  if (((*((s16 *) (((u8 *) D_8009D85C) + var_s1_5))) != 0) && (D_8009CBDC[(s32) var_s0] != new_var15))
                  {
                    func_800258BC((s32) var_s0, 0x2710);
                    func_80025A44((s32) var_s0, 0x2710);
                  }
                  var_s0 += 1;
                  var_s1_5 += 0x440;
                }
                while (((s32) var_s0) < 3);
                func_801D01E8(0x107);
                func_80025288(var_s4 | 0x200);
                temp_s3_2 = func_80025310(var_s4);
                if ((temp_s3_2 & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x2:
                if ((func_801D0CAC(D_801D3E0B[0]) == 0) && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_800258BC(D_801D3E0B[0], 0x2710);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x5:
                if (((func_801D0CAC(D_801D3E0B[0]) == 0) || (func_801D0CE8(D_801D3E0B[0]) == 0)) && ((*((s16 *) (((u8 *) D_8009D85C) + (D_801D3E0B[0] * 0x440)))) != 0))
              {
                func_801D01E8(0x107);
                func_800258BC(D_801D3E0B[0], 0x2710);
                func_80025A44(D_801D3E0B[0], 0x2710);
                func_80025288(var_s4 | 0x200);
                temp_v1_5 = D_8009C757[temp_a0_5];
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                if (!D_801D3DE6)
                {
                }
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

              case 0x6:
                var_s0 = 0;
                var_s1_7 = 0;
                do
              {
                if ((D_8009CBDC[(s32) var_s0] != 0xFF) && ((func_801D0CAC((s32) var_s0) == 0) || (func_801D0CE8((s32) var_s0) == 0)))
                {
                  var_s1_7 = 1;
                }
                var_s0 += 1;
              }
              while (((s32) var_s0) < 3);
                var_s0 = 0;
                if (var_s1_7 != 0)
              {
                new_var12 = 0xFF;
                var_loopc = 0;
                do
                {
                  if (((*((s16 *) (((u8 *) D_8009D85C) + var_loopc))) != 0) && (D_8009CBDC[(s32) var_s0] != new_var12))
                  {
                    func_800258BC((s32) var_s0, 0x2710);
                    func_80025A44((s32) var_s0, 0x2710);
                  }
                  var_s0 += 1;
                  var_loopc += 0x440;
                }
                while (((s32) var_s0) < 3);
                func_801D01E8(0x107);
                func_80025288(var_s4 | 0x200);
                if ((func_80025310(var_s4) & 0xFFFF) != 0xFFFF)
                {
                  return;
                }
                D_801D3E48 = 1;
                return;
              }
              else
              {
                func_801D01E8(3);
                return;
              }
                break;

            }

          }
        }
        else
          if (D_80062D7C & 0x40)
        {
          func_801D01E8(4);
          D_801D3E48 = 1;
          return;
        }
      }
        break;

      case 3:
        var_v0_3 = D_80062D7C & 0x40;
        goto block_217;

      case 4:
        if (D_80062D7C & 0x20)
      {
        func_801D01E8(1);
        temp_bool = D_801D3E2F[0] == 0;
        if (temp_bool)
        {
          new_var18 = (Unk80026448 *) ((&D_801D3E2F[0]) + 7);
          func_80026448(new_var18, 0, 0, 1, 0xA, 0, 0, 1, 0x140, 0, 0, 0, 0, 0);
          D_801D3D84 = 0;
          D_801D3D88 = 0;
          D_801D3D8C = 0;
          D_801D3E48 = 5;
          return;
        }
        ArrangeItems(D_801D3E2F[0]);
        goto block_219;
      }
        var_v0_3 = D_80062D7C & 0x40;
        goto block_217;

      case 5:
        if (D_80062D7C & 0x20)
      {
        switch (D_801D3D84)
        {
          case 0:
            func_801D01E8(1);
            D_801D3D88 = (s32) D_801D3E40;
            D_801D3D8C = D_801D3E41[0] + D_801D3E38;
            D_801D3D84 += 1;
            return;

          case 1:
            func_801D01E8(1);
            temp_v1_13 = &D_8009CBE0[D_801D3D8C];
            new_var16 = D_801D3E41;
            new_var = temp_v1_13;
            temp_a0_12 = *new_var;
            *temp_v1_13 = D_8009CBE0[new_var16[0] + D_801D3E38];
            D_801D3D84 = 0;
            D_8009CBE0[new_var16[0] + D_801D3E38] = temp_a0_12;
            return;

        }

      }
      else
      {
        var_v0_3 = D_80062D7C & 0x40;
        block_217:
        if (var_v0_3 != 0)
        {
          func_801D01E8(4);
          block_219:
          D_801D3E48 = 0;

        }

      }
        break;

    }

  }
}