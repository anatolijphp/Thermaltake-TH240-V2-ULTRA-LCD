// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00104000-0x00107fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_001040c4 @ 001040c4, 72 bytes
// called by: FUN_0010416c
void FUN_001040c4(undefined1 *param_1,int param_2)

{
  undefined1 *puVar1;
  
  do {
    puVar1 = param_1;
    if ((*(uint *)((int)&DAT_b8008017 + DAT_00182d9c + 1) & 0x800000) == 0) {
      puVar1 = param_1 + 1;
      (&DAT_b8008000)[DAT_00182d9c] = *param_1;
      param_2 = param_2 + -1;
    }
    param_1 = puVar1;
  } while (param_2 != 0);
  return;
}

// ==== FUN_00104120 @ 00104120, 28 bytes
void FUN_00104120(undefined4 param_1,undefined4 param_2)

{
  FUN_001030fc(param_1,param_2);
  return;
}

// ==== FUN_0010413c @ 0010413c, 28 bytes
undefined4 FUN_0010413c(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = FUN_00103514(param_1);
  return uVar1;
}

// ==== FUN_00104158 @ 00104158, 20 bytes
void FUN_00104158(undefined4 param_1)

{
  FUN_00103480(param_1);
  return;
}

// ==== FUN_0010416c @ 0010416c, 28 bytes
void FUN_0010416c(undefined4 param_1,undefined4 param_2)

{
  FUN_001040c4(param_1,param_2);
  return;
}

// ==== FUN_001041b0 @ 001041b0, 20 bytes
// called by: FUN_001041dc
void FUN_001041b0(undefined2 *param_1,undefined2 *param_2)

{
  undefined2 uVar1;
  
  uVar1 = *param_1;
  *param_1 = *param_2;
  *param_2 = uVar1;
  return;
}

// ==== FUN_001041c4 @ 001041c4, 24 bytes
// called by: FUN_001041dc
uint FUN_001041c4(int param_1,int param_2)

{
  return ((param_1 + param_2) - (param_1 + param_2 >> 0x1f) & 0x1ffffU) >> 1;
}

// ==== FUN_001041dc @ 001041dc, 220 bytes
// called by: FUN_001041dc, FUN_00104c60
void FUN_001041dc(int param_1,uint param_2,uint param_3)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  
  if ((int)param_2 < (int)param_3) {
    iVar2 = FUN_001041c4(param_2 & 0xffff,param_3 & 0xffff);
    FUN_001041b0(param_1 + param_2 * 2,param_1 + iVar2 * 2);
    uVar1 = *(ushort *)(param_1 + param_2 * 2);
    iVar2 = param_2 + 1;
    uVar3 = param_3;
    while (iVar2 <= (int)uVar3) {
      for (; (iVar2 <= (int)param_3 && (*(ushort *)(param_1 + iVar2 * 2) <= uVar1));
          iVar2 = iVar2 + 1) {
      }
      for (; ((int)param_2 <= (int)uVar3 && (uVar1 < *(ushort *)(param_1 + uVar3 * 2)));
          uVar3 = uVar3 - 1) {
      }
      if (iVar2 < (int)uVar3) {
        FUN_001041b0(param_1 + iVar2 * 2,param_1 + uVar3 * 2);
      }
    }
    FUN_001041b0(param_1 + param_2 * 2,param_1 + uVar3 * 2);
    FUN_001041dc(param_1,param_2,uVar3 - 1);
    FUN_001041dc(param_1,uVar3 + 1,param_3);
  }
  return;
}

// ==== FUN_001042b8 @ 001042b8, 176 bytes
// called by: FUN_00104c60, FUN_001049f4
undefined4 FUN_001042b8(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 | 0x200000;
  }
  else if (param_1 == 1) {
    uVar1 = DAT_b800e01c;
    DAT_b800e01c = uVar1 | 0x10000000;
  }
  else if (param_1 == 2) {
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 | 0x400000;
  }
  else {
    if (param_1 != 3) {
      return 0xffffb8e0;
    }
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 | 0x800000;
  }
  return 0;
}

// ==== FUN_00104368 @ 00104368, 176 bytes
// called by: FUN_00104524, FUN_00104c60, FUN_001049f4
undefined4 FUN_00104368(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 & 0xffdfffff;
  }
  else if (param_1 == 1) {
    uVar1 = DAT_b800e01c;
    DAT_b800e01c = uVar1 & 0xefffffff;
  }
  else if (param_1 == 2) {
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 & 0xffbfffff;
  }
  else {
    if (param_1 != 3) {
      return 0xffffb8e0;
    }
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 & 0xff7fffff;
  }
  return 0;
}

// ==== FUN_00104418 @ 00104418, 200 bytes
// called by: FUN_00104ad4, FUN_00104c60, FUN_001049f4
void FUN_00104418(int param_1,undefined4 param_2,uint param_3,uint param_4)

{
  uint uVar1;
  
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 | 0x4000;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 & 0xffffbfff;
  if (param_1 == 2) {
    DAT_00182dc9 = 1;
  }
  else if (param_1 == 3) {
    DAT_00182dc9 = 0;
  }
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 & 0xfffdffff;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 & 0xffff31ff | param_1 << 0xe;
  DAT_b800e008 = param_2;
  uVar1 = DAT_b800e004;
  DAT_b800e004 = (param_3 & 1) << 3 | (param_4 & 1) << 9 | uVar1 & 0xfffffdf1;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x20000;
  return;
}

// ==== FUN_001044e0 @ 001044e0, 24 bytes
// called by: FUN_00104c60, FUN_001049f4
void FUN_001044e0(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x2000;
  return;
}

// ==== FUN_001044f8 @ 001044f8, 16 bytes
// called by: FUN_00104524, FUN_001049f4
void FUN_001044f8(undefined2 *param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b800e00c;
  *param_1 = (short)uVar1;
  return;
}

// ==== FUN_00104508 @ 00104508, 28 bytes
// called by: FUN_00104c60
void FUN_00104508(undefined2 *param_1,undefined2 *param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b800e00c;
  *param_1 = (short)uVar1;
  uVar1 = DAT_b800e010;
  *param_2 = (short)uVar1;
  return;
}

// ==== FUN_00104524 @ 00104524, 412 bytes
uint FUN_00104524(void)

{
  uint uVar1;
  uint in_r3;
  uint local_10;
  
  uVar1 = DAT_b800e000;
  local_10 = in_r3;
  if ((uVar1 & 0x240000) == 0x240000) {
    if (DAT_00182dd8 == (code *)0x0) {
      if (DAT_007bce98 != (code *)0x0) {
        (*DAT_007bce98)();
      }
    }
    else {
      local_10 = 0;
      FUN_001044f8(&local_10);
      uVar1 = DAT_b800e000;
      (*DAT_00182dd8)(uVar1 >> 9 & 7,local_10 & 0xffff);
    }
    DAT_00182dd4 = 1;
    uVar1 = DAT_b800e000;
    uVar1 = uVar1 & 0xffe7ffff | 0x140000;
    DAT_b800e000 = uVar1;
  }
  else if ((uVar1 & 0x480000) == 0x480000) {
    if (DAT_007bcea0 != (code *)0x0) {
      (*DAT_007bcea0)();
    }
    uVar1 = DAT_b800e000;
    uVar1 = uVar1 & 0xffebffff | 0x80000;
    DAT_b800e000 = uVar1;
  }
  else if ((uVar1 & 0x900000) == 0x900000) {
    uVar1 = DAT_b800e000;
    if ((uVar1 & 0xc000) == 0xc000) {
      FUN_00104368(3);
    }
    if (DAT_007bcea4 != (code *)0x0) {
      (*DAT_007bcea4)();
    }
    DAT_00182dd5 = 1;
    uVar1 = DAT_b800e000;
    uVar1 = uVar1 & 0xfff3ffff | 0x140000;
    DAT_b800e000 = uVar1;
  }
  else {
    uVar1 = DAT_b800e01c;
    uVar1 = uVar1 & 0x30000000;
    if (uVar1 == 0x30000000) {
      if (DAT_007bce9c != (code *)0x0) {
        (*DAT_007bce9c)();
      }
      uVar1 = DAT_b800e01c;
      uVar1 = uVar1 | 0x20000000;
      DAT_b800e01c = uVar1;
    }
  }
  return uVar1;
}

// ==== FUN_00104770 @ 00104770, 344 bytes
// called by: FUN_001048c8, FUN_00104930
void FUN_00104770(undefined4 param_1)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  int extraout_r1;
  int extraout_r1_00;
  int unaff_r5;
  uint unaff_r6;
  uint unaff_r7;
  
  uVar1 = FUN_001014c8();
  uVar2 = FUN_00100e10(3,uVar1);
  uVar3 = FUN_00100e10(2,uVar1);
  if (uVar2 < 0x61a9) {
    if (25000 < uVar3) {
      unaff_r7 = 0x100000;
      unaff_r5 = FUN_0010d398(uVar3,param_1);
      FUN_0010d398(uVar3,param_1);
      if (extraout_r1_00 != 0) {
        unaff_r5 = unaff_r5 + 1;
      }
      FUN_0010d398(uVar3,unaff_r5);
    }
  }
  else {
    unaff_r7 = 0x180000;
    unaff_r5 = FUN_0010d398(uVar2,param_1);
    FUN_0010d398(uVar2,param_1);
    if (extraout_r1 != 0) {
      unaff_r5 = unaff_r5 + 1;
    }
    FUN_0010d398(uVar2,unaff_r5);
  }
  for (uVar2 = 1; uVar2 < 0x100; uVar2 = uVar2 + 1) {
    for (unaff_r6 = 1; (unaff_r6 < 8 && (unaff_r6 * uVar2 - unaff_r5 != 0)); unaff_r6 = unaff_r6 + 1
        ) {
    }
    if (unaff_r6 * uVar2 - unaff_r5 == 0) break;
  }
  if (unaff_r6 != 0) {
    unaff_r6 = unaff_r6 - 1;
  }
  if (uVar2 != 0) {
    uVar2 = uVar2 - 1;
  }
  uVar3 = DAT_b0000218;
  DAT_b0000218 = unaff_r6 << 0x10 | uVar2 << 0x18 | unaff_r7 | uVar3 & 0xe0ffff;
  return;
}

// ==== FUN_001048c8 @ 001048c8, 104 bytes
// called by: FUN_00104ad4
void FUN_001048c8(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000208;
  DAT_b0000208 = uVar1 | 1;
  FUN_00104770(&DAT_00001388);
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 | 0x4000;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 & 0xffffbfff;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x20000;
  DAT_00182dc8 = 1;
  return;
}

// ==== FUN_00104930 @ 00104930, 132 bytes
// called by: FUN_001284b8
void FUN_00104930(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000208;
  DAT_b0000208 = uVar1 | 1;
  FUN_00104770(&DAT_00001388);
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 | 0x4000;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 & 0xffffbfff;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x20000;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x200000;
  FUN_00100520(7,0x16,FUN_00104524);
  FUN_0010043c(0x16);
  return;
}

// ==== FUN_001049b4 @ 001049b4, 12 bytes
// called by: FUN_001284b8
void FUN_001049b4(undefined4 param_1)

{
  DAT_00182dd8 = param_1;
  return;
}

// ==== FUN_001049c0 @ 001049c0, 52 bytes
// called by: FUN_001284b8
void FUN_001049c0(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 & 0xfffff1ff | (param_1 & 7) << 9;
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 | 0x2000;
  return;
}

// ==== FUN_001049f4 @ 001049f4, 224 bytes
undefined4 FUN_001049f4(uint param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  if ((param_1 < 5) || (1 < param_1)) {
    if (DAT_00182dc9 == '\0') {
      FUN_00104368(3);
      FUN_00104418(2,0x150,1);
      FUN_001042b8(0);
      DAT_b800e000 = param_1 << 9 | 0x62000;
      DAT_00182dd4 = '\0';
      FUN_001044e0();
      do {
      } while (DAT_00182dd4 == '\0');
      FUN_001044f8(param_2);
      uVar1 = DAT_b800e000;
      DAT_b800e000 = uVar1 & 0xffff31ff | 0x140000;
      FUN_00104418(3,0x150,1);
      FUN_001042b8(0);
      FUN_001042b8(3);
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffffb8e2;
    }
  }
  else {
    uVar2 = 0xffffb8e1;
  }
  return uVar2;
}

// ==== FUN_00104ad4 @ 00104ad4, 212 bytes
undefined4 FUN_00104ad4(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  if (DAT_00182dc8 == '\0') {
    FUN_001048c8();
  }
  if (DAT_00182dc8 == '\x01') {
    DAT_00182dc8 = '\x02';
    if (param_1 == 0) {
      uVar1 = DAT_b0000208;
      DAT_b0000208 = uVar1 | 1;
      FUN_00104418(3,0x180,1,0);
      uVar1 = DAT_b0000218;
      DAT_b0000218 = uVar1 & 0xe0ffff | 0x1000000;
      FUN_00100520(7,0x16,FUN_00104524);
      FUN_0010043c(0x16);
      DAT_00182dc8 = '\x02';
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffffffff;
    }
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

// ==== FUN_00104ba8 @ 00104ba8, 124 bytes
char FUN_00104ba8(void)

{
  uint uVar1;
  
  if (DAT_00182dc8 == '\x02') {
    FUN_00100408(0x16);
    uVar1 = DAT_b800e000;
    DAT_b800e000 = uVar1 & 0xfffdffff;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 | 0x4000;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 & 0xffffbfff;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 & 0xfffffffe;
    DAT_00182dc8 = '\0';
  }
  return DAT_00182dc8;
}

// ==== FUN_00104c24 @ 00104c24, 60 bytes
void FUN_00104c24(int param_1,int param_2)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < param_2; iVar1 = iVar1 + 1) {
    dbg_printf(&DAT_00104f2c,*(undefined2 *)(param_1 + iVar1 * 2));
  }
  dbg_printf("\n");
  return;
}

// ==== FUN_00104c60 @ 00104c60, 828 bytes
undefined4 FUN_00104c60(int param_1,ushort *param_2,ushort *param_3)

{
  bool bVar1;
  uint uVar2;
  ushort uVar3;
  undefined4 uVar4;
  int iVar5;
  bool bVar6;
  bool bVar7;
  undefined1 auStack_34 [4];
  ushort local_30;
  ushort local_2e;
  ushort local_2c;
  undefined1 auStack_28 [4];
  ushort local_24;
  ushort local_22;
  ushort local_20;
  uint local_1c;
  
  local_1c = 0;
  if (DAT_00182dc8 == '\x02') {
    if ((param_1 == 0) || (param_1 == 1)) {
      uVar2 = DAT_b800e000;
      DAT_b800e000 = uVar2 & 0xffff3fff | 0x8000;
      FUN_00104368(0);
      FUN_00104418(3,0x180,0);
      DAT_00182dd5 = '\0';
      FUN_001042b8(3);
      local_1c = 0x10;
      do {
        iVar5 = local_1c + -1;
        bVar6 = local_1c != 0;
        local_1c = iVar5;
      } while (bVar6);
      if (param_1 == 1) {
        do {
        } while (DAT_00182dd5 == '\0');
      }
      else if (DAT_00182dd5 == '\0') {
        return 0;
      }
      FUN_00104368(3);
      FUN_00104418(2,0x180,0);
      FUN_001042b8(0);
      DAT_00182dd4 = '\0';
      FUN_001044e0();
      do {
      } while (DAT_00182dd4 == '\0');
      local_1c = 0x2000;
      do {
        bVar6 = local_1c != 0;
        local_1c = local_1c + -1;
      } while (bVar6);
      for (local_1c = 0; local_1c < 6; local_1c = local_1c + 1) {
        DAT_00182dd4 = '\0';
        FUN_001044e0();
        do {
        } while (DAT_00182dd4 == '\0');
        FUN_00104508(auStack_28 + local_1c * 2,auStack_34 + local_1c * 2);
      }
      FUN_001041dc(auStack_28,0,5);
      FUN_001041dc(auStack_34,0,5);
      uVar3 = divide((uint)local_24 + (uint)local_22 + (uint)local_20,3);
      *param_2 = uVar3;
      uVar3 = divide((uint)local_30 + (uint)local_2e + (uint)local_2c,3);
      *param_3 = uVar3;
      if (((int)(DAT_00182dcc - 0x10) < (int)(uint)*param_2) &&
         ((int)(uint)*param_2 < (int)(DAT_00182dcc + 0x10))) {
        bVar6 = true;
      }
      else {
        bVar6 = false;
      }
      if (((int)(DAT_00182dd0 - 0x10) < (int)(uint)*param_3) &&
         ((int)(uint)*param_3 < (int)(DAT_00182dd0 + 0x10))) {
        bVar1 = true;
      }
      else {
        bVar1 = false;
      }
      FUN_00104418(3,0x20,0);
      DAT_00182dcc = (uint)*param_2;
      DAT_00182dd0 = (uint)*param_3;
      local_1c = 0x40000;
      do {
        iVar5 = local_1c + -1;
        bVar7 = local_1c != 0;
        local_1c = iVar5;
      } while (bVar7);
      if (param_1 == 0) {
        if (DAT_00182dd5 == '\0') {
          dbg_printf("Library Pen up\n");
          uVar4 = 0;
        }
        else {
          uVar4 = 1;
        }
      }
      else if ((bVar6) && (bVar1)) {
        uVar4 = 1;
      }
      else {
        uVar4 = 0;
      }
    }
    else {
      FUN_00100408(0x16);
      uVar4 = 0xffffffff;
    }
  }
  else {
    uVar4 = 0xffffffff;
  }
  return uVar4;
}

// ==== FUN_00104fe0 @ 00104fe0, 112 bytes
// called by: FUN_001052cc
void FUN_00104fe0(int param_1,int param_2,int param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  if (param_1 == 0) {
    uVar4 = 0;
  }
  else {
    uVar4 = 0x80000000;
  }
  if (param_2 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0x40000000;
  }
  if (param_3 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = 0x20000000;
  }
  uVar1 = DAT_b800e038;
  DAT_b800e038 = uVar4 | uVar2 | uVar3 | (param_4 & 0x1f) << 0x18 | uVar1 & 0xffffff;
  return;
}

// ==== FUN_00105050 @ 00105050, 64 bytes
void FUN_00105050(byte *param_1,byte *param_2,byte *param_3,uint *param_4)

{
  byte bVar1;
  uint uVar2;
  
  uVar2 = DAT_b800e038;
  bVar1 = (byte)(uVar2 >> 0x18);
  *param_1 = bVar1 >> 7;
  *param_2 = (bVar1 & 0x40) >> 6;
  *param_3 = (bVar1 & 0x20) >> 5;
  *param_4 = (uVar2 & 0x1f000000) >> 0x18;
  return;
}

// ==== FUN_00105090 @ 00105090, 48 bytes
// called by: FUN_001052cc
void FUN_00105090(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_b800e038;
  DAT_b800e038 = (param_1 & 3) << 0x10 | param_2 & 0xff | uVar1 & 0xfffcff00;
  return;
}

// ==== FUN_001050dc @ 001050dc, 52 bytes
// called by: FUN_001052cc
void FUN_001050dc(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_b800e034;
  DAT_b800e034 = param_1 << 0x1f | (param_2 & 3) << 0xc | uVar1 & 0x7fffcfff;
  return;
}

// ==== FUN_00105134 @ 00105134, 76 bytes
// called by: FUN_001052cc
void FUN_00105134(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  
  uVar1 = DAT_b800e034;
  DAT_b800e034 = (param_1 & 0x3ff) << 0x10 | (param_2 & 0xf) << 8 | uVar1 & 0xfc00f000 |
                 (param_3 & 0xf) << 4 | param_4 & 0xf;
  return;
}

// ==== FUN_00105180 @ 00105180, 68 bytes
void FUN_00105180(uint *param_1,uint *param_2,uint *param_3,uint *param_4)

{
  uint uVar1;
  
  uVar1 = DAT_b800e034;
  *param_1 = (uVar1 & 0x3ff0000) >> 0x10;
  *param_2 = (uVar1 & 0xf00) >> 8;
  *param_3 = (uVar1 & 0xf0) >> 4;
  *param_4 = uVar1 & 0xf;
  return;
}

// ==== FUN_001051dc @ 001051dc, 36 bytes
// called by: FUN_001052cc
void FUN_001051dc(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_b800e030;
  DAT_b800e030 = uVar1 & 0xfffffcc0 | param_2 | param_1 << 8;
  return;
}

// ==== FUN_00105220 @ 00105220, 96 bytes
// called by: FUN_001052cc
void FUN_00105220(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  
  uVar1 = DAT_b800e034;
  DAT_b800e034 = uVar1 & 0xbfffffff | (param_1 & 1) << 0x1e;
  uVar1 = DAT_b800e030;
  DAT_b800e030 = uVar1 & 0xffff03ff | (param_2 & 0xf) << 0xc |
                 (param_3 & 1) << 0xb | (param_4 & 1) << 10;
  return;
}

// ==== FUN_00105280 @ 00105280, 76 bytes
void FUN_00105280(byte *param_1,uint *param_2,undefined1 *param_3,undefined1 *param_4)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar2 = DAT_b800e034;
  *param_1 = ((byte)((uint)uVar2 >> 0x18) & 0x40) >> 6;
  uVar1 = DAT_b800e030;
  *param_2 = (uVar1 & 0xf000) >> 0xc;
  *param_3 = (char)((uVar1 & 0x800) >> 0xb);
  *param_4 = (char)((uVar1 & 0x400) >> 10);
  return;
}

// ==== FUN_001052cc @ 001052cc, 868 bytes
undefined4 FUN_001052cc(int param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_r8;
  uint uVar6;
  uint unaff_r9;
  uint uVar7;
  bool bVar8;
  int local_3c;
  int local_34;
  
  local_34 = 0x100;
  uVar1 = DAT_b0000208;
  DAT_b0000208 = uVar1 | 1;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 | 0x4000;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 & 0xffffbfff;
  uVar1 = DAT_b0000218;
  uVar1 = uVar1 & 0xffe7ffff | param_1 << 0x13;
  DAT_b0000218 = uVar1;
  if (param_1 == 0) {
    uVar2 = FUN_001014c8(uVar1);
    uVar1 = FUN_0010d398(uVar2,param_2);
    if (0x100 < uVar1) {
      return 0xffffffff;
    }
    uVar4 = DAT_b0000218;
    DAT_b0000218 = uVar4 & 0xf8ffff | (uVar1 - 1) * 0x1000000;
  }
  else {
    if (param_1 == 1) {
      return 0xffffffff;
    }
    if ((param_1 == 2) || (param_1 == 3)) {
      uVar2 = FUN_001014c8(uVar1);
      uVar2 = FUN_00100e10(param_1,uVar2);
      uVar1 = FUN_0010d398(uVar2,param_2 * 0x500);
      if (0x800 < uVar1) {
        return 0xffffffff;
      }
      for (uVar4 = 1; uVar6 = unaff_r8, uVar7 = unaff_r9, uVar4 < 0x101; uVar4 = uVar4 + 1) {
        for (uVar5 = 2;
            (uVar6 = unaff_r8, uVar7 = unaff_r9, uVar5 < 9 &&
            (uVar6 = uVar5, uVar7 = uVar4, uVar5 * uVar4 - uVar1 != 0)); uVar5 = uVar5 + 1) {
        }
        if (uVar6 * uVar7 - uVar1 == 0) break;
        unaff_r8 = uVar6;
        unaff_r9 = uVar7;
      }
      uVar1 = DAT_b0000218;
      DAT_b0000218 = (uVar7 - 1) * 0x1000000 | (uVar6 - 1) * 0x10000 | param_1 << 0x13 |
                     uVar1 & 0xe0ffff;
    }
  }
  DAT_b800e000 = 0x20000;
  uVar1 = DAT_b800e01c;
  DAT_b800e01c = uVar1 | 0x1000000;
  do {
    bVar8 = local_34 != 0;
    local_34 = local_34 + -1;
  } while (bVar8);
  uVar1 = DAT_b800e01c;
  DAT_b800e01c = uVar1 & 0xfeffffff;
  DAT_b800e01c = 0x3d000000;
  uVar1 = DAT_b800e01c;
  DAT_b800e01c = uVar1 & 0xfe00ffff | 0x500000;
  if (DAT_00182ddc == '\0') {
    DAT_00182ddc = '\x01';
    uVar1 = DAT_b800e01c;
    DAT_b800e01c = uVar1 | 0x22000000;
    FUN_00104fe0(0,0,1,0x10);
    FUN_00105090(1,0x100);
    iVar3 = FUN_00102420(0);
    local_3c = iVar3;
    while ((uint)(local_3c - iVar3) < 0xb) {
      local_3c = FUN_00102420(0);
    }
    uVar1 = DAT_b800e01c;
    DAT_b800e01c = uVar1 & 0xfdffffff;
    FUN_00104fe0(0,0,0,0x10);
  }
  uVar1 = DAT_b800e01c;
  DAT_b800e01c = uVar1 & 0x3fffffff;
  FUN_001050dc(0,1);
  FUN_00105134(4,4,4);
  FUN_001051dc(2,0x10);
  FUN_00105220(1,0xd,0);
  uVar1 = DAT_b800e000;
  DAT_b800e000 = uVar1 & 0xfffffeff;
  FUN_00100520(7,0x16,FUN_00104524);
  FUN_0010043c(0x16);
  return 0;
}

// ==== FUN_001056b4 @ 001056b4, 136 bytes
// called by: FUN_0012a298, FUN_00105a20, lcd_boot_proc, FUN_0011b2e8, usb_command
void FUN_001056b4(void)

{
  DAT_00182de2 = 0;
  DAT_00182df8 = 0;
  DAT_00182e08 = 0;
  DAT_00182dea = 0;
  DAT_00182de4 = 0;
  DAT_00182e14 = 0;
  DAT_00182e18 = 0;
  DAT_00182de3 = 0;
  DAT_00182de5 = 0;
  DAT_00182de6 = 0;
  DAT_00182dec = 0;
  DAT_00182df0 = 0;
  DAT_00182df4 = 0;
  DAT_b100a010 = 0xf4;
  DAT_b100a014 = 0xf4;
  DAT_b100a020 = 4;
  DAT_b100a024 = 4;
  DAT_b100a008 = 0;
  DAT_b100a054 = 0;
  DAT_b100a058 = 0;
  DAT_b100a0b0 = 0;
  DAT_b100a0b8 = 0xfff;
  DAT_b100a000 = 2;
  DAT_b100a000 = 0;
  DAT_b100a050 = 0x400000;
  return;
}

// ==== FUN_0010573c @ 0010573c, 32 bytes
// called by: FUN_00105a20, FUN_00106170
void FUN_0010573c(undefined2 *param_1,ushort *param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b100a018;
  *param_1 = (short)((uint)uVar1 >> 0x10);
  uVar1 = DAT_b100a018;
  *param_2 = (ushort)uVar1 & 0x1fff;
  return;
}

// ==== FUN_0010575c @ 0010575c, 24 bytes
// called by: FUN_00105a20, FUN_001065d4, FUN_00106170
void FUN_0010575c(uint param_1,uint param_2)

{
  DAT_b100a018 = param_2 & 0xffff1fff | (param_1 & 0xffff1fff) << 0x10;
  return;
}

// ==== FUN_00105774 @ 00105774, 216 bytes
// called by: FUN_00105a20, FUN_00106170
undefined4 FUN_00105774(int param_1,uint param_2,uint param_3)

{
  uint uVar1;
  
  if (param_1 == 0) {
    DAT_b100a058 = 0xffff7fff;
    uVar1 = DAT_b100a054;
    DAT_b100a054 = uVar1 | 0x40;
  }
  else if ((param_1 == 1 || param_1 == 2) || param_1 == 3) {
    DAT_b100a058 = 0x8000;
    uVar1 = DAT_b100a054;
    DAT_b100a054 = uVar1 & 0xffffffbf;
    if (param_1 == 2 || param_1 == 3) {
      uVar1 = DAT_b100a058;
      DAT_b100a058 = uVar1 & 0xffffe0c0 | (param_3 & 0x1f) << 8 | param_2 & 0x1f;
      return 1;
    }
    if (param_1 == 4) goto LAB_00105814;
  }
  else if (param_1 == 4) {
    DAT_b100a05c = 0x8000;
LAB_00105814:
    uVar1 = DAT_b100a05c;
    DAT_b100a05c = uVar1 & 0xffff8000 | (param_3 & 0x1f) << 8 | param_2 & 0x1f;
    return 1;
  }
  uVar1 = DAT_b100a058;
  DAT_b100a058 = uVar1 & 0xffffe0c0;
  DAT_b100a0b0 = param_3 & 0xffff3fff | (param_2 & 0xffff3fff) << 0x10;
  return 1;
}

// ==== FUN_0010584c @ 0010584c, 440 bytes
// called by: FUN_00105a20, FUN_00106170
undefined4
FUN_0010584c(int param_1,uint param_2,uint param_3,uint param_4,uint param_5,ushort *param_6,
            ushort *param_7)

{
  ushort uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  bool bVar7;
  bool bVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  
  if (param_1 == 0) {
    if (param_4 < param_2 || param_5 < param_3) {
      return 2;
    }
    uVar2 = FUN_0012edc0(param_3 - 2);
    uVar3 = FUN_0012edc0(param_5 - 1);
    uVar2 = FUN_0012eb14(uVar3,uVar2);
    FUN_0012f23c(uVar2,10);
    uVar1 = FUN_0012ed6c();
    *param_7 = uVar1;
    uVar2 = FUN_0012edc0(param_2 - 2);
    uVar3 = FUN_0012edc0(param_4 - 1);
    uVar2 = FUN_0012eb14(uVar3,uVar2);
    FUN_0012f23c(uVar2,10);
    uVar1 = FUN_0012ed6c();
  }
  else {
    if (param_1 == 1) {
      bVar8 = param_2 <= param_4;
      bVar7 = param_4 == param_2;
      if (!bVar8 || bVar7) {
        bVar8 = param_3 <= param_5;
        bVar7 = param_5 == param_3;
      }
      if (bVar8 && !bVar7) {
        return 2;
      }
      uVar2 = FUN_0012edc0(param_3 - 1);
      uVar3 = FUN_0012edf0(param_5);
      uVar2 = FUN_0012eb14(uVar3,uVar2);
      FUN_0012f23c(uVar2,0xd);
      uVar1 = FUN_0012ed6c();
      *param_7 = uVar1;
      if (0x2000 < uVar1) {
        *param_7 = 0x2000;
      }
      uVar2 = FUN_0012edc0(param_2 - 1);
      uVar3 = FUN_0012edf0(param_4);
      uVar2 = FUN_0012eb14(uVar3,uVar2);
      FUN_0012f23c(uVar2,0xd);
      uVar1 = FUN_0012ed6c();
      *param_6 = uVar1;
      if (uVar1 < 0x2001) {
        return 1;
      }
      *param_6 = 0x2000;
      return 1;
    }
    if ((param_1 != 2 && param_1 != 3) && param_1 != 4) {
      return 2;
    }
    bVar8 = param_2 <= param_4;
    bVar7 = param_4 == param_2;
    if (!bVar8 || bVar7) {
      bVar8 = param_3 <= param_5;
      bVar7 = param_5 == param_3;
    }
    if (bVar8 && !bVar7) {
      return 2;
    }
    uVar9 = FUN_0010d398(param_2,param_4);
    if ((int)((ulonglong)uVar9 >> 0x20) != 0) {
      return 2;
    }
    uVar10 = FUN_0010d398(param_3,param_5);
    if ((int)((ulonglong)uVar10 >> 0x20) != 0) {
      return 2;
    }
    uVar4 = ((uint)uVar10 >> 1) - 1;
    uVar6 = uVar4 & 0xffff;
    if (1 < uVar6 && uVar6 != 3) {
      return 2;
    }
    uVar5 = (int)uVar9 - 1;
    uVar6 = uVar5 & 0xffff;
    uVar1 = (ushort)uVar5;
    if ((1 < uVar6) && (uVar6 != 3 && uVar6 != 7)) {
      return 2;
    }
    *param_7 = (ushort)uVar4;
  }
  *param_6 = uVar1;
  return 1;
}

// ==== FUN_00105a04 @ 00105a04, 28 bytes
// called by: FUN_00105a20
void FUN_00105a04(undefined2 *param_1,undefined2 *param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b100a028;
  *param_2 = (short)uVar1;
  uVar1 = DAT_b100a028;
  *param_1 = (short)((uint)uVar1 >> 0x10);
  return;
}

// ==== FUN_00105a20 @ 00105a20, 1068 bytes
void FUN_00105a20(void)

{
  ushort uVar1;
  int iVar2;
  undefined *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  ushort uVar8;
  ushort uVar9;
  uint local_40;
  undefined2 *local_3c;
  undefined2 *local_38;
  ushort local_34 [2];
  ushort local_30 [2];
  undefined2 local_2c [2];
  undefined2 local_28 [2];
  
  uVar7 = DAT_b100a02c;
  if ((uVar7 & 0x40) == 0) {
    if ((uVar7 & 8) == 0) {
      if ((uVar7 & 4) == 0) {
        if ((uVar7 & 2) != 0) {
          uVar7 = DAT_b100a02c;
          DAT_b100a02c = uVar7 & 0xffffff00 | 2;
          DAT_00182de0 = 0;
          DAT_00182de1 = 1;
          return;
        }
        if ((uVar7 & 0x20) == 0) {
          return;
        }
        uVar7 = DAT_b100a02c;
        DAT_b100a02c = uVar7 & 0xffffff00 | 0x20;
        uVar7 = DAT_b100a000;
        DAT_b100a000 = uVar7 | 0x200;
        if (DAT_00182df4 == (code *)0x0) {
          do {
                    /* WARNING: Do nothing block with infinite loop */
          } while( true );
        }
        iVar2 = (*DAT_00182df4)((uint)DAT_00182de8 * (DAT_00182e08 & 1) + DAT_00182e0c,DAT_00182de8)
        ;
        if (iVar2 != 0) {
          DAT_00182e08 = DAT_00182e08 + 1;
          return;
        }
        goto LAB_00105af0;
      }
      FUN_0010573c(&local_3c,&local_40);
      if (DAT_00182df8 != 0) {
        local_40 = (uint)(ushort)((short)local_40 - (short)DAT_00182df8);
      }
      DAT_007bceac = local_40 & 0xffff;
      DAT_007bceb0 = (uint)local_3c & 0xffff;
      uVar7 = DAT_b100a02c;
      uVar7 = uVar7 & 0xffffff00 | 4;
    }
    else {
      DAT_007bcec4 = DAT_b100a0a8;
      DAT_007bcec8 = DAT_b100a0ac;
      uVar7 = DAT_b100a02c;
      uVar7 = uVar7 & 0xffffff00 | 8;
    }
    DAT_b100a02c = uVar7;
    DAT_00182de0 = 0;
    return;
  }
  uVar7 = DAT_b100a008;
  uVar7 = uVar7 & 0x700;
  FUN_00105a04(local_30,local_34);
  DAT_007bceb4 = (uint)local_34[0];
  DAT_007bceb8 = (uint)local_30[0];
  DAT_007bcea8 = uVar7;
  if ((DAT_00182df0 != (code *)0x0) && (iVar2 = (*DAT_00182df0)(), iVar2 == 0)) {
LAB_00105af0:
    FUN_001056b4();
    DAT_00182de0 = 0;
    return;
  }
  if (DAT_00182de2 == '\0') {
    if (uVar7 == 0x300) {
      if ((local_34[0] & 0x1f) != 0) {
        local_34[0] = (local_34[0] & 0xffe0) + 0x20;
      }
    }
    else if (uVar7 == 0x200 || uVar7 == 0x500) {
      if ((local_34[0] & 7) != 0) {
        local_34[0] = (local_34[0] & 0xfff8) + 8;
      }
    }
    else if ((local_34[0] & 0xf) != 0) {
      local_34[0] = (local_34[0] & 0xfff0) + 0x10;
    }
  }
  else {
    local_38 = local_2c;
    local_3c = local_28;
    local_40 = DAT_00182dfc & 0xffff;
    iVar2 = FUN_0010584c(DAT_00182e04 & 0xff,local_30[0],local_34[0],DAT_00182e00 & 0xffff);
    if (iVar2 == 1) {
      FUN_00105774(DAT_00182e04 & 0xff,local_28[0],local_2c[0]);
      local_34[0] = (ushort)DAT_00182dfc;
      local_30[0] = (ushort)DAT_00182e00;
    }
    else {
      DAT_00182de0 = 0;
      DAT_00182de1 = 1;
    }
  }
  if (DAT_00182df8 < local_34[0]) {
    DAT_00182df8 = 0;
    DAT_b100a094 = 0;
    DAT_007bcebc = 0;
  }
  else {
    DAT_007bcebc = DAT_00182df8;
    iVar2 = DAT_00182df8 - local_34[0];
    DAT_b100a094 = iVar2;
    local_34[0] = (ushort)DAT_00182df8;
    DAT_00182df8 = iVar2;
  }
  puVar3 = (undefined *)0x0;
  if (DAT_00182e10 != (undefined *)0x8021) {
    puVar3 = DAT_00182e10 + -0x8000;
  }
  uVar9 = 0;
  uVar8 = 0;
  if (DAT_00182e10 != (undefined *)0x8021 && puVar3 != (undefined *)0x31) {
    if (DAT_00182de4 != '\0') {
      uVar9 = local_34[0];
      uVar8 = local_30[0];
      local_34[0] = (ushort)DAT_00182e14;
      local_30[0] = (ushort)DAT_00182e18;
    }
    uVar7 = DAT_007bcebc;
    if (DAT_007bcebc == 0) {
      uVar7 = (uint)local_34[0];
    }
    if (DAT_00182e10 == &DAT_00014021) {
      DAT_007bcec4 = local_30[0] * uVar7 * 4;
    }
    else {
      DAT_007bcec4 = local_30[0] * uVar7 * 2;
    }
    goto LAB_00105bf8;
  }
  if (DAT_00182de4 != '\0') {
    uVar9 = local_34[0];
    uVar8 = local_30[0];
    local_34[0] = (ushort)DAT_00182e14;
    local_30[0] = (ushort)DAT_00182e18;
  }
  if (uVar7 == 0x300) {
    uVar4 = (uint)(local_34[0] >> 2);
LAB_00105ccc:
    uVar1 = local_30[0];
    if ((local_30[0] & 7) != 0) {
      uVar1 = (local_30[0] & 0xfff8) + 8;
      local_30[0] = uVar1;
    }
  }
  else {
    if (uVar7 == 0x200 || uVar7 == 0x500) {
      uVar4 = (uint)local_34[0];
    }
    else if ((local_34[0] & 1) == 0) {
      uVar4 = (uint)(local_34[0] >> 1);
    }
    else {
      uVar4 = (local_34[0] >> 1) + 1;
    }
    if ((uVar7 != 0) && (((uVar7 == 0x100 || uVar7 == 0x200) || uVar7 == 0x300 || (uVar7 != 0x500)))
       ) goto LAB_00105ccc;
    if ((local_30[0] & 0xf) != 0) {
      local_30[0] = (local_30[0] & 0xfff0) + 0x10;
    }
    uVar1 = local_30[0] >> 1;
  }
  iVar2 = DAT_b100a07c;
  uVar6 = (uint)local_34[0];
  uVar5 = (uint)local_30[0];
  DAT_b100a080 = uVar6 * uVar5 + iVar2;
  iVar2 = DAT_b100a080;
  DAT_b100a084 = uVar4 * uVar1 + iVar2;
  if (uVar7 == 0x400) {
    DAT_007bcec4 = uVar6 * uVar5;
  }
  else {
    DAT_007bcec4 = uVar4 * 2 * (uint)uVar1 + uVar6 * uVar5;
  }
LAB_00105bf8:
  if (DAT_00182de4 != '\0') {
    local_34[0] = uVar9;
    local_30[0] = uVar8;
  }
  FUN_0010575c(local_30[0],local_34[0]);
  uVar7 = DAT_b100a02c;
  DAT_b100a02c = uVar7 & 0xffffff00 | 0x40;
  return;
}

// ==== FUN_00105e4c @ 00105e4c, 36 bytes
// called by: FUN_0011b2e8
bool FUN_00105e4c(void)

{
  do {
  } while (DAT_00182de0 != '\0');
  return DAT_00182de1 == '\0';
}

// ==== FUN_00105e70 @ 00105e70, 24 bytes
// called by: FUN_0012a298, lcd_boot_proc
bool FUN_00105e70(void)

{
  return DAT_00182de0 == '\0';
}

// ==== FUN_00105eb8 @ 00105eb8, 196 bytes
// called by: lcd_boot_proc, FUN_0011b2e8, idle_screen_proc
undefined4 FUN_00105eb8(void)

{
  uint uVar1;
  undefined *puVar2;
  uint uVar3;
  undefined8 uVar4;
  undefined4 local_20;
  undefined1 auStack_1c [4];
  undefined1 auStack_18 [4];
  undefined1 auStack_14 [4];
  undefined1 auStack_10 [4];
  undefined1 auStack_c [4];
  
  uVar3 = DAT_b0000204;
  DAT_b0000204 = uVar3 | 0x80;
  DAT_b0000014 = 0x20000;
  DAT_b0000014 = 0;
  FUN_001019c0(auStack_1c,auStack_18,auStack_14,auStack_10,&local_20,auStack_c);
  uVar3 = DAT_b000021c;
  puVar2 = (undefined *)FUN_0010d398(local_20,((uVar3 & 0xf0) >> 4) + 1);
  uVar3 = 0;
  if ((&DAT_000186a0 < puVar2) &&
     (uVar4 = FUN_0010d398(puVar2), uVar3 = (uint)uVar4, (int)((ulonglong)uVar4 >> 0x20) == 0)) {
    uVar3 = uVar3 - 1;
  }
  uVar1 = DAT_b000021c;
  DAT_b000021c = uVar1 & 0xf8ffffff | (uVar3 & 7) << 0x18;
  FUN_00100520(1,0x1a,FUN_00105a20);
  FUN_00100830(0x7f);
  FUN_0010043c(0x1a);
  return 1;
}

// ==== FUN_00105f7c @ 00105f7c, 236 bytes
// called by: FUN_001065d4
undefined4 FUN_00105f7c(int param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  
  uVar3 = 0;
  if (param_2 == 0xa1) {
LAB_00106028:
    if (param_1 == 1) {
      return 2;
    }
    if (param_2 == 0xb1) {
      uVar2 = 0xb0;
    }
    else {
      uVar2 = 0xa0;
    }
    DAT_b100a000 = uVar2;
    uVar3 = 0x40;
  }
  else {
    if ((int)param_2 < 0xa2) {
      if (param_2 != 0x90) {
        if (param_2 == 0x91) goto LAB_00106028;
        if (param_2 != 0x98 && param_2 != 0xa0) {
          return 2;
        }
      }
    }
    else if (param_2 != 0xa8 && param_2 != 0xb0) {
      if (param_2 == 0xb1) goto LAB_00106028;
      if (param_2 != 0xb8) {
        return 2;
      }
    }
    uVar1 = DAT_b100a000;
    DAT_b100a000 = param_2 | uVar1 & 0x40;
  }
  DAT_00182e04 = 0;
  if (DAT_00182de6 != '\0') {
    DAT_00182e04 = 3;
  }
  if (DAT_00182de5 != '\0') {
    DAT_00182e04 = 4;
  }
  if (param_1 == 0) {
    uVar1 = DAT_b100a008;
    uVar3 = uVar1 & 0x1808 | uVar3 | 0x8000;
  }
  else {
    if (param_1 != 1) {
      return 2;
    }
    uVar3 = DAT_b100a008;
    uVar3 = uVar3 & 0xffff67ff;
  }
  DAT_b100a008 = uVar3;
  return 1;
}

// ==== FUN_00106068 @ 00106068, 148 bytes
// called by: FUN_001065d4
undefined4 FUN_00106068(undefined *param_1)

{
  uint uVar1;
  undefined *puVar2;
  bool bVar3;
  
  puVar2 = param_1 + -0x6021;
  if (param_1 != &DAT_00006021) {
    if ((int)param_1 < 0x6021) {
      if ((param_1 != (undefined *)0x21 && param_1 != (undefined *)0x31) &&
         (param_1 != (undefined *)0x4021 && param_1 != (undefined *)0x4031)) {
        return 2;
      }
    }
    else {
      bVar3 = puVar2 == (undefined *)0x2000;
      if (!bVar3) {
        puVar2 = param_1 + -0x8031;
        bVar3 = puVar2 == (undefined *)0x0;
      }
      if (!bVar3) {
        bVar3 = puVar2 == (undefined *)0xbff0;
      }
      if (!bVar3) {
        return 2;
      }
    }
  }
  DAT_b100a008 = param_1;
  uVar1 = DAT_b100a000;
  DAT_b100a000 = uVar1 & 0xffffff7f;
  DAT_00182e04 = 1;
  DAT_00182e10 = param_1;
  puVar2 = (undefined *)0x0;
  if (param_1 != (undefined *)0x8021) {
    puVar2 = param_1 + -0x8000;
  }
  if (param_1 == (undefined *)0x8021 || puVar2 == (undefined *)0x31) {
    DAT_00182e04 = 2;
  }
  return 1;
}

// ==== FUN_001060fc @ 001060fc, 116 bytes
// called by: FUN_001065d4
void FUN_001060fc(void)

{
  uint uVar1;
  undefined *puVar2;
  
  DAT_00182de0 = 1;
  DAT_00182de1 = 0;
  FUN_0010cc30(&DAT_007bcea8,0x24);
  if (DAT_00182de3 == '\0') {
    puVar2 = &DAT_00004600;
  }
  else {
    DAT_00182e08 = 0;
    DAT_00182e0c = DAT_b100a0a0;
    puVar2 = &DAT_00006600;
  }
  DAT_b100a02c = puVar2;
  uVar1 = DAT_b100a02c;
  DAT_b100a02c = uVar1 & 0xffffff00 | 0x66;
  uVar1 = DAT_b100a000;
  DAT_b100a000 = uVar1 | 1;
  uVar1 = DAT_b100a000;
  DAT_b100a000 = uVar1 & 0xfffffffe;
  return;
}

// ==== FUN_00106170 @ 00106170, 556 bytes
// called by: FUN_001065d4
void FUN_00106170(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  bool bVar7;
  uint local_30;
  undefined2 *local_2c;
  undefined2 *local_28;
  undefined2 local_24 [2];
  undefined2 local_20 [2];
  undefined2 local_1c [2];
  undefined2 local_18 [2];
  
  DAT_00182de0 = 1;
  DAT_00182de1 = 0;
  DAT_00182e04 = 0;
  if (DAT_00182de6 != '\0') {
    DAT_00182e04 = 3;
  }
  if (DAT_00182de5 != '\0') {
    DAT_00182e04 = 4;
  }
  FUN_0010cc30(&DAT_007bcea8,0x24);
  if (DAT_00182dec == 0) {
LAB_0010624c:
    if (DAT_00182de2 != '\0') {
      if (DAT_00182dec == 0) {
        puVar6 = local_24;
        puVar5 = local_20;
      }
      else {
        puVar6 = local_20;
        puVar5 = local_24;
      }
      FUN_0010573c(puVar5,puVar6);
      local_28 = local_1c;
      local_2c = local_18;
      local_30 = DAT_00182dfc & 0xffff;
      iVar4 = FUN_0010584c(DAT_00182e04 & 0xff,local_20[0],local_24[0],DAT_00182e00 & 0xffff);
      if (iVar4 != 1) goto LAB_001062b8;
      FUN_00105774(DAT_00182e04 & 0xff,local_18[0],local_1c[0]);
      if (DAT_00182de5 == '\0') {
        uVar3 = DAT_00182e00;
        uVar2 = DAT_00182dfc;
        if (DAT_00182dec != 0) {
          uVar3 = DAT_00182dfc;
          uVar2 = DAT_00182e00;
        }
        FUN_0010575c(uVar3 & 0xffff,uVar2 & 0xffff);
      }
      else {
        DAT_b100a01c = DAT_00182dfc & 0x1fff | (DAT_00182e00 & 0xffff1fff) << 0x10;
      }
    }
    DAT_b100a02c = 0x800;
    uVar3 = DAT_b100a02c;
    DAT_b100a02c = uVar3 & 0xffffff00 | 8;
    uVar3 = DAT_b100a000;
    DAT_b100a000 = uVar3 | 1;
    uVar3 = DAT_b100a000;
    DAT_b100a000 = uVar3 & 0xfffffffe;
  }
  else {
    uVar3 = DAT_b100a008;
    uVar3 = uVar3 & 0x8040;
    bVar7 = uVar3 != 0x8000;
    if (bVar7) {
      uVar3 = DAT_b100a000;
    }
    if (!bVar7 || (uVar3 & 8) == 0) {
      FUN_0010573c(&local_30,&local_2c);
      iVar4 = DAT_b100a07c;
      if (DAT_00182dec == 0x22) {
        DAT_b100a07c = iVar4 + (local_30 & 0xffff) + -1;
        iVar4 = DAT_b100a080;
        DAT_b100a080 = iVar4 + (uint)((ushort)local_30 >> 1) + -1;
        iVar4 = DAT_b100a084;
        iVar4 = iVar4 + (uint)((ushort)local_30 >> 1) + -1;
      }
      else {
        DAT_b100a07c = (((uint)local_2c & 0xffff) - 1) * (local_30 & 0xffff) + iVar4;
        uVar3 = DAT_b100a098;
        local_30 = CONCAT22(local_30._2_2_,(short)uVar3);
        iVar4 = DAT_b100a080;
        iVar1 = (int)((uVar3 & 0xffff) * (((uint)local_2c & 0xffff) - 2)) / 2;
        DAT_b100a080 = iVar4 + iVar1;
        iVar4 = DAT_b100a084;
        iVar4 = iVar4 + iVar1;
      }
      DAT_b100a084 = iVar4;
      goto LAB_0010624c;
    }
LAB_001062b8:
    DAT_00182de1 = 1;
    DAT_00182de0 = 0;
  }
  return;
}

// ==== FUN_001063e4 @ 001063e4, 60 bytes
// called by: FUN_001065d4
undefined4 FUN_001063e4(uint param_1,uint param_2,uint param_3,uint param_4,undefined4 param_5)

{
  if (param_1 < param_3 && param_2 < param_4) {
    DAT_b100a044 = param_1 | param_2 << 0x10;
    DAT_b100a048 = param_3 | param_4 << 0x10;
    DAT_b100a04c = param_5;
    DAT_b100a000 = 0x40;
    return 1;
  }
  return 2;
}

// ==== FUN_00106420 @ 00106420, 52 bytes
// called by: FUN_001065d4
undefined4 FUN_00106420(int param_1,uint param_2,uint param_3)

{
  uint *puVar1;
  
  if (param_1 == 0) {
    puVar1 = &DAT_b100a010;
  }
  else {
    if (param_1 != 1) {
      return 2;
    }
    puVar1 = &DAT_b100a014;
  }
  *puVar1 = param_3 & 0xf | (param_2 & 0xf) << 4;
  return 1;
}

// ==== FUN_00106454 @ 00106454, 340 bytes
// called by: FUN_001065d4
undefined4 FUN_00106454(int param_1,int param_2,int param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  uVar2 = 0xffffff;
  iVar4 = 0;
  do {
    for (; uVar1 = DAT_b100a000, (uVar1 & uVar2 & 4) != 0; uVar2 = uVar2 - 1) {
    }
    if (uVar2 == 0) {
      return 3;
    }
    iVar3 = param_1 + iVar4;
    *(uint *)((int)&DAT_b100a100 + iVar4) =
         CONCAT13(*(undefined1 *)(iVar3 + 3),
                  CONCAT12(*(undefined1 *)(iVar3 + 2),
                           CONCAT11(*(undefined1 *)(iVar3 + 1),*(undefined1 *)(param_1 + iVar4))));
    iVar4 = iVar4 + 4;
  } while (iVar4 < 0x40);
  uVar2 = 0xffffff;
  iVar4 = 0;
  do {
    for (; uVar1 = DAT_b100a000, (uVar1 & uVar2 & 4) != 0; uVar2 = uVar2 - 1) {
    }
    if (uVar2 == 0) {
      return 3;
    }
    iVar3 = param_2 + iVar4;
    *(uint *)(&DAT_b100a140 + iVar4) =
         CONCAT13(*(undefined1 *)(iVar3 + 3),
                  CONCAT12(*(undefined1 *)(iVar3 + 2),
                           CONCAT11(*(undefined1 *)(iVar3 + 1),*(undefined1 *)(param_2 + iVar4))));
    iVar4 = iVar4 + 4;
  } while (iVar4 < 0x40);
  if (2 < param_4) {
    uVar2 = DAT_b100a008;
    DAT_b100a008 = uVar2 | 8;
    uVar2 = 0xffffff;
    iVar4 = 0;
    do {
      for (; uVar1 = DAT_b100a000, (uVar1 & uVar2 & 4) != 0; uVar2 = uVar2 - 1) {
      }
      if (uVar2 == 0) {
        return 3;
      }
      iVar3 = param_3 + iVar4;
      *(uint *)((int)&DAT_b100a180 + iVar4) =
           CONCAT13(*(undefined1 *)(iVar3 + 3),
                    CONCAT12(*(undefined1 *)(iVar3 + 2),
                             CONCAT11(*(undefined1 *)(iVar3 + 1),*(undefined1 *)(param_3 + iVar4))))
      ;
      iVar4 = iVar4 + 4;
    } while (iVar4 < 0x40);
    for (uVar2 = 0xffffff; uVar1 = DAT_b100a000, (uVar1 & uVar2 & 4) != 0; uVar2 = uVar2 - 1) {
    }
    if (uVar2 == 0) {
      return 3;
    }
  }
  return 1;
}

// ==== FUN_001065d4 @ 001065d4, 848 bytes
// called by: FUN_0012a298, lcd_boot_proc, FUN_0011b2e8, usb_command
void FUN_001065d4(undefined4 param_1,uint *param_2,uint *param_3,undefined4 param_4)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  
  switch(param_1) {
  case 0:
    DAT_b100a07c = param_2;
    return;
  case 1:
    DAT_b100a094 = param_2;
    return;
  case 2:
    DAT_b100a098 = param_2;
    return;
  case 3:
    DAT_b100a09c = param_2;
    return;
  case 4:
    DAT_b100a0a0 = param_2;
    return;
  case 5:
    DAT_b100a0b8 = param_2;
    return;
  case 6:
    DAT_b100a004 = param_2;
    return;
  case 7:
    FUN_00106454(&DAT_00182e1c,&DAT_00182e5c,0,2);
    return;
  case 8:
    FUN_00106068(param_2);
    return;
  case 9:
    FUN_00105f7c((uint)param_2 & 0xff,(uint)param_3 & 0xffff);
    return;
  case 10:
    FUN_0010575c((uint)param_2 & 0xffff,(uint)param_3 & 0xffff);
    return;
  case 0xb:
    FUN_00106170();
    if (DAT_00182dea == 0) {
      return;
    }
    iVar1 = DAT_b100a0a0;
    *(undefined1 *)(iVar1 + 2) = 0xff;
    *(undefined1 *)(iVar1 + 3) = 0xe0;
    *(char *)(iVar1 + 4) = (char)(DAT_00182dea - 4 >> 8);
    *(char *)(iVar1 + 5) = (char)DAT_00182dea + -4;
    return;
  case 0xc:
    FUN_001060fc();
    return;
  case 0xd:
    FUN_001063e4((short)*param_2,*(undefined2 *)((int)param_2 + 2),(short)param_2[1],
                 *(undefined2 *)((int)param_2 + 6),param_2[2]);
    DAT_00182de4 = 1;
    DAT_00182e14 = param_2[2];
    DAT_00182e18 = ((uint)*(ushort *)((int)param_2 + 6) - (uint)*(ushort *)((int)param_2 + 2)) *
                   0x10 + 0x10;
    return;
  case 0xe:
    DAT_00182df8 = param_2;
    return;
  case 0xf:
    break;
  case 0x10:
    break;
  case 0x11:
    DAT_00182df0 = param_2;
    return;
  case 0x12:
    DAT_00182de3 = 1;
    DAT_00182df4 = param_2;
    DAT_b100a050 = (uint)param_3 >> 3 & 0x1f00 | 0x400008;
    DAT_00182de8 = (short)((uint)((int)param_3 << 0xf) >> 0x10);
    return;
  case 0x13:
    uVar3 = (uint)param_3 >> 4 & 0xf0;
    FUN_00106420((uint)param_2 & 0xff,uVar3,(uint)param_3 & 0xff,uVar3,param_4);
    return;
  case 0x14:
    if (param_2 == (uint *)0x0) {
      return;
    }
    puVar2 = param_2 + 1;
    if (((uint)puVar2 & 1) != 0) {
      puVar2 = (uint *)((int)param_2 + 5);
    }
    if (((uint)puVar2 & 3) == 0) {
      puVar2 = (uint *)((int)puVar2 + 2);
    }
    if (&DAT_0000fffe < puVar2) {
      puVar2 = (uint *)&DAT_0000fffe;
    }
    uVar3 = DAT_b100a054;
    DAT_b100a054 = uVar3 | 4;
    DAT_b100a070 = puVar2;
    DAT_00182dea = (short)puVar2;
    return;
  case 0x15:
    DAT_b100a080 = param_2;
    return;
  case 0x16:
    DAT_b100a084 = param_2;
    return;
  case 0x17:
    DAT_b100a020 = param_2;
    return;
  case 0x18:
    DAT_b100a024 = param_2;
    return;
  case 0x19:
    uVar3 = DAT_b100a020;
    goto LAB_001068b8;
  case 0x1a:
    uVar3 = DAT_b100a024;
    goto LAB_001068b8;
  case 0x1b:
    DAT_b100a01c = (uint)param_3 & 0x1fff | ((uint)param_2 & 0xffff1fff) << 0x10;
    return;
  case 0x1c:
    DAT_b100a074 = param_2;
    return;
  case 0x1d:
    uVar3 = DAT_b100a01c;
    *param_2 = uVar3 >> 0x10;
    uVar3 = DAT_b100a01c;
    *param_3 = uVar3 & 0x1fff;
    return;
  case 0x1e:
    uVar3 = DAT_b100a074;
LAB_001068b8:
    *param_2 = uVar3;
    return;
  case 0x1f:
    DAT_00182de6 = 1;
    break;
  case 0x20:
    DAT_00182de5 = 1;
    break;
  case 0x21:
    DAT_00182dec = 0x21;
    uVar3 = DAT_b100a008;
    uVar3 = uVar3 | 0x1800;
    goto LAB_0010691c;
  case 0x22:
    DAT_00182dec = 0x22;
    uVar3 = DAT_b100a008;
    uVar3 = uVar3 & 0xffffe7ff | 0x1000;
    goto LAB_0010691c;
  case 0x23:
    DAT_00182dec = 0;
    uVar3 = DAT_b100a008;
    uVar3 = uVar3 & 0xffffe7ff;
LAB_0010691c:
    DAT_b100a008 = uVar3;
    return;
  default:
    return;
  }
  DAT_00182de2 = 1;
  DAT_00182dfc = param_3;
  DAT_00182e00 = param_2;
  return;
}

// ==== FUN_00106924 @ 00106924, 44 bytes
// called by: lcd_boot_proc, FUN_0011b2e8
/* WARNING: Removing unreachable block (ram,0x0010041c) */

undefined4 FUN_00106924(void)

{
  uint uVar1;
  
  DAT_b100a000 = 2;
  DAT_b100a000 = 0;
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 & 0xffffff7f;
  DAT_b8000124 = 0x4000000;
  return 0;
}

// ==== FUN_00106970 @ 00106970, 88 bytes
undefined4 FUN_00106970(uint param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    FUN_0010a938(param_1 & 0xff,param_2,param_3);
    FUN_0010a9c4(param_1 & 0xff,1,0);
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff0403;
  }
  return uVar1;
}

// ==== FUN_001069c8 @ 001069c8, 32 bytes
undefined4 FUN_001069c8(undefined1 param_1)

{
  FUN_0010a9c4(param_1,0);
  return 0;
}

// ==== FUN_001069e8 @ 001069e8, 224 bytes
// called by: FUN_0010d4c8
undefined4 FUN_001069e8(uint param_1,uint param_2,int param_3,uint param_4)

{
  undefined4 uVar1;
  int local_28;
  uint local_24;
  uint local_20;
  uint local_1c;
  
  (&DAT_007bced0)[param_1 * 6] = 0;
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    if ((param_2 == 0) || (param_3 == 0)) {
      uVar1 = 0xffff0402;
    }
    else if (param_4 == 0) {
      uVar1 = 0xffff0402;
    }
    else {
      local_1c = param_4 & 0xffffff00;
      local_24 = param_2 & 0xffffff00;
      if (param_1 != 0) {
        if ((&DAT_007bcf44)[param_1 * 2] != -1) {
          local_1c = CONCAT31((int3)(param_4 >> 8),*(undefined1 *)(&DAT_007bcf44 + param_1 * 2));
        }
        if ((&DAT_007bcf48)[param_1 * 2] != -1) {
          local_24 = CONCAT31((int3)(param_2 >> 8),*(undefined1 *)(&DAT_007bcf48 + param_1 * 2));
        }
      }
      local_28 = param_3;
      local_20 = param_2;
      FUN_0010a2d8(param_1 & 0xff,&local_20,&local_28,param_4);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 0xffff0403;
  }
  return uVar1;
}

// ==== FUN_00106ac8 @ 00106ac8, 364 bytes
undefined4 FUN_00106ac8(uint param_1,uint param_2,uint param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint *puVar4;
  uint local_40;
  uint local_3c;
  
  iVar1 = param_1 * 0x18;
  if (*(int *)(&DAT_007bcee0 + iVar1) == 0) {
    if (param_4 == 0) {
      uVar2 = 0xffff0402;
    }
    else if (((param_2 & 0xfff) == 0) && ((param_2 & 0xfff) == 0)) {
      uVar3 = mem_alloc((param_4 + 0xfffU >> 0xc) * 0x14);
      (&DAT_007bced0)[param_1 * 6] = uVar3 | 0x80000000;
      *(int *)(&DAT_007bced4 + iVar1) = param_4;
      *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
           *(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xffffff0f;
      FUN_0010b418(param_1 & 0xff);
      FUN_0010b408(param_1 & 0xff,(&DAT_007bced0)[param_1 * 6]);
      puVar4 = (uint *)(&DAT_007bced0)[param_1 * 6];
      local_40 = param_3;
      local_3c = param_2;
      do {
        if (*(uint *)(&DAT_007bced4 + iVar1) < 0x1000) {
          uVar3 = *(uint *)(&DAT_007bced4 + iVar1);
        }
        else {
          uVar3 = 0x1000;
        }
        *(uint *)(&DAT_007bced4 + iVar1) = *(int *)(&DAT_007bced4 + iVar1) - uVar3;
        *puVar4 = local_3c;
        puVar4[1] = local_40;
        puVar4[2] = uVar3;
        puVar4[3] = 0;
        if (*(int *)(&DAT_007bced4 + iVar1) == 0) {
          puVar4[3] = puVar4[3] | 0x80000000;
        }
        puVar4 = puVar4 + 5;
        local_3c = local_3c + uVar3;
        local_40 = local_40 + uVar3;
      } while (*(int *)(&DAT_007bced4 + iVar1) != 0);
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffff0402;
    }
  }
  else {
    uVar2 = 0xffff0403;
  }
  return uVar2;
}

// ==== FUN_00106c34 @ 00106c34, 100 bytes
// called by: FUN_00106de4
int FUN_00106c34(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = (&DAT_007bcecc)[param_1 * 6];
  if (iVar1 != 0) {
    iVar3 = (&DAT_007bced0)[param_1 * 6];
    while (iVar3 != 0) {
      if ((*(ushort *)(iVar3 + 0xc) & 0x8000) == 0) {
        iVar2 = 0;
      }
      else {
        iVar2 = *(int *)(iVar3 + 0x10);
      }
      iVar1 = mem_free(iVar3);
      iVar3 = iVar2;
    }
  }
  return iVar1;
}

// ==== FUN_00106c98 @ 00106c98, 84 bytes
// called by: FUN_0010d4c8
undefined4 FUN_00106c98(uint param_1,undefined1 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  if ((&DAT_007bcecc)[param_1 * 6] == 0) {
    uVar1 = 0xffff0401;
  }
  else {
    uVar1 = FUN_0010b32c(param_1 & 0xff,param_2,param_3,param_4);
  }
  return uVar1;
}

// ==== FUN_00106cec @ 00106cec, 68 bytes
undefined4 FUN_00106cec(uint param_1)

{
  undefined4 uVar1;
  
  uVar1 = 0;
  if ((&DAT_007bcecc)[param_1 * 6] != 0) {
    FUN_0010a230(param_1 & 0xff,1);
    uVar1 = FUN_0010a83c(param_1 & 0xff,0xb);
  }
  return uVar1;
}

// ==== FUN_00106d30 @ 00106d30, 36 bytes
void FUN_00106d30(undefined1 param_1)

{
  FUN_0010a874(param_1,0xb);
  FUN_0010a230(param_1,0);
  return;
}

// ==== FUN_00106d54 @ 00106d54, 144 bytes
// called by: FUN_00106eb8, FUN_00106e94
undefined4 FUN_00106d54(uint param_1)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = &DAT_007bcecc + param_1 * 6;
  uVar2 = 0;
  FUN_00100830(0x80);
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    if ((int)param_1 < 5) {
      if (*piVar1 == 0) {
        *piVar1 = 0;
        *piVar1 = 1;
        FUN_0010a230(param_1 & 0xff,1);
        FUN_0010a83c(param_1 & 0xff,0xb);
        FUN_00100830(0x7f);
      }
      else {
        uVar2 = 0xffff0403;
      }
    }
    else {
      uVar2 = 0xffff0402;
    }
  }
  else {
    uVar2 = 0xffff0403;
  }
  return uVar2;
}

// ==== FUN_00106de4 @ 00106de4, 176 bytes
// called by: FUN_0010d75c
undefined4 FUN_00106de4(uint param_1)

{
  uint uVar1;
  
  if ((&DAT_007bcecc)[param_1 * 6] != 0) {
    uVar1 = *(uint *)(&DAT_b0008000 + param_1 * 0x100);
    if (((uVar1 & 0xc0) == 0xc0) || ((uVar1 & 0x30) == 0x30)) {
      *(uint *)(&DAT_b0008000 + param_1 * 0x100) = uVar1 & 0xffffff0f | 2;
      (&DAT_b0008024)[param_1 * 0x40] = 0xf00;
    }
    FUN_0010a874(param_1 & 0xff,0xf);
    FUN_0010a230(param_1 & 0xff,0);
    if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) != 0) {
      *(undefined4 *)(&DAT_007bcee0 + param_1 * 0x18) = 0;
    }
    if ((&DAT_007bced0)[param_1 * 6] != 0) {
      FUN_00106c34(param_1);
    }
    (&DAT_007bcecc)[param_1 * 6] = 0;
  }
  return 0;
}

// ==== FUN_00106e94 @ 00106e94, 36 bytes
// called by: FUN_0010d4c8
undefined4 FUN_00106e94(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_00106d54(0);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0xffff0401;
  }
  return uVar2;
}

// ==== FUN_00106eb8 @ 00106eb8, 56 bytes
int FUN_00106eb8(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 1;
  while( true ) {
    if (4 < iVar2) {
      return -0xfbff;
    }
    iVar1 = FUN_00106d54(iVar2);
    if (iVar1 == 0) break;
    iVar2 = iVar2 + 1;
  }
  return iVar2;
}

// ==== FUN_00106ef0 @ 00106ef0, 60 bytes
// called by: FUN_0010d4c8
int FUN_00106ef0(uint param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(&DAT_007bcee0 + param_1 * 0x18);
  if (iVar1 == 0) {
    *(undefined4 *)(&DAT_007bcee0 + param_1 * 0x18) = 1;
    iVar1 = FUN_0010a7cc(param_1 & 0xff);
  }
  return iVar1;
}

// ==== FUN_00106f44 @ 00106f44, 20 bytes
void FUN_00106f44(undefined1 param_1)

{
  FUN_0010a1f4(param_1);
  return;
}

// ==== FUN_00106f58 @ 00106f58, 128 bytes
// called by: FUN_00116e40
undefined4 FUN_00106f58(void)

{
  int iVar1;
  
  if (DAT_00182e9c == '\0') {
    DAT_00182e9c = '\x01';
    FUN_0010a1d0();
    for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
      (&DAT_007bcecc)[iVar1 * 6] = 0;
      (&DAT_007bced0)[iVar1 * 6] = 0;
      (&DAT_007bcedc)[iVar1 * 6] = iVar1;
      (&DAT_007bcf44)[iVar1 * 2] = 0xffffffff;
      (&DAT_007bcf48)[iVar1 * 2] = 0xffffffff;
    }
  }
  return 0;
}

// ==== FUN_00106fd8 @ 00106fd8, 12 bytes
void FUN_00106fd8(void)

{
  FUN_0010a1d8();
  return;
}

// ==== FUN_00106fe4 @ 00106fe4, 88 bytes
undefined4 FUN_00106fe4(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    FUN_0010a41c(param_1 & 0xff,param_4);
    FUN_0010a490(param_1 & 0xff,param_2,param_3);
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff0403;
  }
  return uVar1;
}

// ==== FUN_0010703c @ 0010703c, 124 bytes
undefined4 FUN_0010703c(uint param_1,uint param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    FUN_0010a71c(param_1 & 0xff,param_2 & 0xff);
    if (param_2 == 0) {
      FUN_0010a874(param_1 & 0xff,4);
      FUN_0010a83c(param_1 & 0xff,0xb);
    }
    else {
      FUN_0010a874(param_1 & 0xff,0xb);
      FUN_0010a83c(param_1 & 0xff,4);
    }
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff0403;
  }
  return uVar1;
}

// ==== FUN_001070b8 @ 001070b8, 80 bytes
undefined4 FUN_001070b8(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (*(int *)(&DAT_007bcee0 + param_1 * 0x18) == 0) {
    if ((0 < param_1) && (param_1 < 5)) {
      (&DAT_007bcf44)[param_1 * 2] = param_2;
      (&DAT_007bcf48)[param_1 * 2] = param_3;
    }
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff0403;
  }
  return uVar1;
}

// ==== FUN_00107124 @ 00107124, 448 bytes
void FUN_00107124(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar1 = DAT_b8007044;
  uVar2 = DAT_b8007050;
  uVar3 = DAT_b8007054;
  if ((uVar1 & 0xf) != 0) {
    if (((uVar1 & 1) != 0) && (DAT_b8007044 = 1, DAT_007bcf6c != (code *)0x0)) {
      (*DAT_007bcf6c)();
    }
    if (((uVar1 & 2) != 0) && (DAT_b8007044 = 2, DAT_007bcf74 != (code *)0x0)) {
      (*DAT_007bcf74)();
    }
    if (((uVar1 & 4) != 0) && (DAT_b8007044 = 4, DAT_007bcf7c != (code *)0x0)) {
      (*DAT_007bcf7c)();
    }
    if (((uVar1 & 8) != 0) && (DAT_b8007044 = 8, DAT_007bcf84 != (code *)0x0)) {
      (*DAT_007bcf84)();
    }
  }
  if (((uVar2 & 0x10) != 0) &&
     (uVar1 = DAT_b8007050, DAT_b8007050 = uVar1 & 0xffffffef, DAT_007bcf70 != (code *)0x0)) {
    (*DAT_007bcf70)();
  }
  if (((uVar2 & 0x100000) != 0) &&
     (uVar1 = DAT_b8007050, DAT_b8007050 = uVar1 & 0xffefffff, DAT_007bcf78 != (code *)0x0)) {
    (*DAT_007bcf78)();
  }
  if (((uVar3 & 0x10) != 0) &&
     (uVar1 = DAT_b8007054, DAT_b8007054 = uVar1 & 0xffffffef, DAT_007bcf80 != (code *)0x0)) {
    (*DAT_007bcf80)();
  }
  if (((uVar3 & 0x100000) != 0) &&
     (uVar1 = DAT_b8007054, DAT_b8007054 = uVar1 & 0xffefffff, DAT_007bcf88 != (code *)0x0)) {
    (*DAT_007bcf88)();
  }
  return;
}

// ==== FUN_00107350 @ 00107350, 420 bytes
void FUN_00107350(uint param_1,int param_2,int *param_3)

{
  if (param_1 == 0x10) {
LAB_001073b4:
    if (param_2 != 0) {
      if ((param_1 & 0x10) == 0) {
        *param_3 = DAT_007bcf6c;
        DAT_007bcf6c = param_2;
      }
      else {
        *param_3 = DAT_007bcf70;
        DAT_007bcf70 = param_2;
      }
    }
  }
  else {
    if ((int)param_1 < 0x11) {
      if (param_1 == 0) goto LAB_001073b4;
      if (param_1 == 1) goto LAB_001073fc;
      if (param_1 == 2) goto LAB_00107444;
      if (param_1 != 3) goto LAB_001074cc;
    }
    else {
      if (param_1 == 0x11) {
LAB_001073fc:
        if (param_2 != 0) {
          if ((param_1 & 0x10) == 0) {
            *param_3 = DAT_007bcf74;
            DAT_007bcf74 = param_2;
          }
          else {
            *param_3 = DAT_007bcf78;
            DAT_007bcf78 = param_2;
          }
        }
        goto LAB_001074cc;
      }
      if (param_1 == 0x12) {
LAB_00107444:
        if (param_2 != 0) {
          if ((param_1 & 0x10) == 0) {
            *param_3 = DAT_007bcf7c;
            DAT_007bcf7c = param_2;
          }
          else {
            *param_3 = DAT_007bcf80;
            DAT_007bcf80 = param_2;
          }
        }
        goto LAB_001074cc;
      }
      if (param_1 != 0x13) goto LAB_001074cc;
    }
    if (param_2 != 0) {
      if ((param_1 & 0x10) == 0) {
        *param_3 = DAT_007bcf84;
        DAT_007bcf84 = param_2;
      }
      else {
        *param_3 = DAT_007bcf88;
        DAT_007bcf88 = param_2;
      }
    }
  }
LAB_001074cc:
  FUN_00100520(1,0x19,FUN_00107124);
  FUN_00100830(0x7f);
  FUN_0010043c(0x19);
  return;
}

// ==== FUN_001074f4 @ 001074f4, 184 bytes
void FUN_001074f4(uint param_1,uint param_2)

{
  uint uVar1;
  
  if ((param_1 & 0x10) == 0) {
    uVar1 = DAT_b8007040;
    DAT_b8007040 = uVar1 | 1 << (param_1 & 0xff);
  }
  else if ((param_1 & 2) == 0) {
    uVar1 = DAT_b8007050;
    DAT_b8007050 = uVar1 & ~(6 << ((param_1 & 1) << 4) | 0xd000d0U) |
                   (param_2 & 3) << (param_1 & 1) * 0x10 + 1;
  }
  else {
    uVar1 = DAT_b8007054;
    DAT_b8007054 = uVar1 & ~(6 << ((param_1 & 1) << 4) | 0xd000d0U) |
                   (param_2 & 3) << (param_1 & 1) * 0x10 + 1;
  }
  return;
}

// ==== FUN_001075ac @ 001075ac, 440 bytes
// called by: FUN_0010d3f8
void FUN_001075ac(uint param_1,uint param_2)

{
  uint uVar1;
  
  if ((param_1 & 0x10) == 0) {
    uVar1 = DAT_b8007040;
    DAT_b8007040 = uVar1 & ~(1 << (param_1 & 3));
    uVar1 = DAT_b8007044;
    DAT_b8007044 = uVar1 & ~(1 << (param_1 & 3));
  }
  else if ((param_1 & 2) == 0) {
    uVar1 = DAT_b8007050;
    DAT_b8007050 = uVar1 & ~((param_2 & 3) << (param_1 & 1) * 0x10 + 1 | 0xd000d0);
  }
  else {
    uVar1 = DAT_b8007054;
    DAT_b8007054 = uVar1 & ~((param_2 & 3) << (param_1 & 1) * 0x10 + 1 | 0xd000d0);
  }
  if (param_1 == 0x10) {
LAB_001076b8:
    DAT_007bcf70 = 0;
    DAT_007bcf6c = 0;
  }
  else {
    if ((int)param_1 < 0x11) {
      if (param_1 == 0) goto LAB_001076b8;
      if (param_1 == 1) goto LAB_001076d4;
      if (param_1 == 2) goto LAB_001076f0;
      if (param_1 != 3) goto LAB_00107720;
    }
    else {
      if (param_1 == 0x11) {
LAB_001076d4:
        DAT_007bcf78 = 0;
        DAT_007bcf74 = 0;
        goto LAB_00107720;
      }
      if (param_1 == 0x12) {
LAB_001076f0:
        DAT_007bcf80 = 0;
        DAT_007bcf7c = 0;
        goto LAB_00107720;
      }
      if (param_1 != 0x13) goto LAB_00107720;
    }
    DAT_007bcf88 = 0;
    DAT_007bcf84 = 0;
  }
LAB_00107720:
  uVar1 = DAT_b8007040;
  if ((((uVar1 & 0xf) == 0) && (uVar1 = DAT_b8007050, (uVar1 & 0x60006) == 0)) &&
     (uVar1 = DAT_b8007054, (uVar1 & 0x60006) == 0)) {
    FUN_00100408(0x19);
  }
  return;
}

// ==== FUN_0010799c @ 0010799c, 48 bytes
// called by: FUN_0010d3f8
void FUN_0010799c(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000208;
  DAT_b0000208 = uVar1 | 0x20;
  DAT_b0000018 = 0x400;
  uVar1 = DAT_b0000018;
  DAT_b0000018 = uVar1 & 0xfffffbff;
  return;
}

// ==== FUN_001079cc @ 001079cc, 52 bytes
// called by: FUN_0010d3f8
void FUN_001079cc(void)

{
  uint uVar1;
  
  DAT_b800707c = 0;
  DAT_b8007078 = 0;
  FUN_00100408(0x19);
  uVar1 = DAT_b0000208;
  DAT_b0000208 = uVar1 & 0xffffffdf;
  return;
}

// ==== FUN_00107a98 @ 00107a98, 256 bytes
// called by: FUN_0010d3f8
void FUN_00107a98(uint param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = DAT_b8007008;
  if (param_2 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1 << ((param_1 & 7) << 3);
  }
  DAT_b8007008 = uVar2 | uVar1 & ~(1 << ((param_1 & 7) << 3));
  if ((param_1 & 0x10) != 0) {
    if ((param_1 & 2) == 0) {
      uVar1 = DAT_b8007050;
      if (param_2 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = 1 << (param_1 & 1) * 0x10 + 3;
      }
      DAT_b8007050 = uVar2 | uVar1 & ~(1 << (param_1 & 1) * 0x10 + 3) | 0xd000d0;
    }
    else {
      uVar1 = DAT_b8007054;
      if (param_2 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = 1 << (param_1 & 1) * 0x10 + 3;
      }
      DAT_b8007054 = uVar2 | uVar1 & ~(1 << (param_1 & 1) * 0x10 + 3) | 0xd000d0;
    }
  }
  return;
}

// ==== FUN_00107b98 @ 00107b98, 1848 bytes
// called by: FUN_0010d3f8
undefined4 FUN_00107b98(uint param_1,char *param_2)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  byte bVar11;
  undefined1 uVar12;
  bool bVar13;
  undefined1 auStack_3c [4];
  int local_38;
  undefined1 auStack_34 [4];
  undefined1 auStack_30 [4];
  undefined1 auStack_2c [4];
  undefined1 auStack_28 [4];
  
  FUN_001019c0(auStack_3c,auStack_28,auStack_2c,auStack_30,auStack_34,&local_38);
  local_38 = local_38 * 1000;
  if ((param_1 & 0x10) == 0) {
    uVar9 = DAT_b8007008;
    if (param_2[9] == '\0') {
      uVar10 = 0;
    }
    else {
      uVar10 = 1 << (param_1 & 7) * 8 + 2;
    }
    DAT_b8007008 = uVar10 | uVar9 & ~(1 << (param_1 & 7) * 8 + 2);
  }
  else if ((param_1 & 2) == 0) {
    uVar9 = DAT_b8007050;
    if (param_2[9] == '\0') {
      uVar10 = 0;
    }
    else {
      uVar10 = 1 << ((param_1 & 1) << 4);
    }
    DAT_b8007050 = uVar10 | uVar9 & ~(1 << ((param_1 & 1) << 4)) | 0xd000d0;
  }
  else {
    uVar9 = DAT_b8007054;
    if (param_2[9] == '\0') {
      uVar10 = 0;
    }
    else {
      uVar10 = 1 << ((param_1 & 1) << 4);
    }
    DAT_b8007054 = uVar10 | uVar9 & ~(1 << ((param_1 & 1) << 4)) | 0xd000d0;
  }
  uVar9 = DAT_b8007008;
  bVar13 = *param_2 == '\0';
  uVar12 = 1;
  if (bVar13) {
    uVar10 = 0;
  }
  else {
    uVar10 = 1 << (param_1 & 7) * 8 + 3;
  }
  DAT_b8007008 = uVar10 | uVar9 & ~(1 << (param_1 & 7) * 8 + 3);
  FUN_0012eca4(*(undefined4 *)(param_2 + 4),0);
  if (bVar13) {
    if ((param_1 & 2) == 0) {
      uVar9 = DAT_b8007000;
      DAT_b8007000 = uVar9 & 0xffffff00 | (byte)param_2[0xb] - 1 & 0xff;
    }
    else {
      uVar9 = DAT_b8007000;
      DAT_b8007000 = uVar9 & 0xffff00ff | ((byte)param_2[0xb] - 1 & 0xff) << 8;
    }
    iVar8 = 1;
    bVar1 = param_2[10];
    if (bVar1 == 4) {
      iVar8 = 1;
    }
    else if (bVar1 < 5) {
      if (bVar1 == 1) {
        iVar8 = 4;
      }
      else if (bVar1 == 2) {
        iVar8 = 0;
      }
    }
    else if (bVar1 == 8) {
      iVar8 = 2;
    }
    else if (bVar1 == 0x10) {
      iVar8 = 3;
    }
    uVar9 = DAT_b8007004;
    DAT_b8007004 = uVar9 & ~(7 << ((param_1 & 7) << 2)) | iVar8 << ((param_1 & 7) << 2);
    *(int *)(&DAT_b800700c + (param_1 & 7) * 0xc) = *(int *)(param_2 + 0xc) + -1;
    iVar8 = FUN_0010d398(*(int *)(param_2 + 0xc) * (uint)(byte)param_2[8],100);
    *(int *)(&DAT_b8007010 + (param_1 & 7) * 0xc) = iVar8 + -1;
    uVar3 = FUN_0012edf0(*(undefined4 *)(param_2 + 0xc));
    uVar4 = FUN_0012edf0(param_2[10]);
    uVar5 = FUN_0012edf0(param_2[0xb]);
    uVar6 = FUN_0012edf0(local_38);
    uVar5 = FUN_0012eb14(uVar6,uVar5);
    uVar4 = FUN_0012eb14(uVar5,uVar4);
    uVar3 = FUN_0012eb14(uVar4,uVar3);
  }
  else {
    uVar3 = FUN_0012edf0(local_38);
    uVar3 = FUN_0012eb14(uVar3,*(undefined4 *)(param_2 + 4));
    FUN_0012f04c(uVar3,0x4d800000);
    if ((bool)uVar12) {
      uVar9 = 1;
      FUN_0012ee18(uVar3,0x48000000);
      if ((bool)uVar12) {
        FUN_0012eb14(uVar3,0x47800000);
        uVar10 = FUN_0012ed6c();
        uVar10 = uVar10 & 0xffff;
        uVar4 = FUN_0012edf0(uVar10);
        uVar4 = FUN_0012eb14(uVar3,uVar4);
        FUN_0012f04c(uVar4,0x47800000);
        if (!(bool)uVar12) {
          uVar10 = uVar10 + 1 & 0xfffeffff;
        }
        uVar12 = 0xff < uVar10;
        if (0x100 < uVar10) {
          uVar10 = 0x100;
          uVar4 = FUN_0012edf0(0x100);
          uVar3 = FUN_0012eb14(uVar3,uVar4);
          FUN_0012eb14(uVar3,0x47800000);
          uVar9 = FUN_0012ed6c();
          uVar9 = uVar9 & 0xff;
          uVar4 = FUN_0012edf0(uVar9);
          uVar4 = FUN_0012eb14(uVar3,uVar4);
          FUN_0012f04c(uVar4,0x47800000);
          if (!(bool)uVar12) {
            uVar9 = uVar9 + 1 & 0xff;
          }
          bVar1 = 0;
          do {
            bVar11 = bVar1;
            bVar1 = bVar11 + 1;
          } while (1 << bVar11 <= (int)uVar9);
          uVar9 = 1 << (uint)bVar11 & 0xff;
          if (0x10 < uVar9) {
            return 0;
          }
          uVar4 = FUN_0012edf0(0x100);
          uVar3 = FUN_0012ee9c(uVar4,uVar3);
        }
      }
      else {
        uVar10 = 2;
      }
      uVar4 = FUN_0012edf0(uVar9);
      uVar5 = FUN_0012edf0(uVar10);
      uVar3 = FUN_0012eb14(uVar3,uVar5);
      FUN_0012eb14(uVar3,uVar4);
      uVar7 = FUN_0012ed6c();
      uVar7 = uVar7 & 0xffff;
      uVar3 = FUN_0012edf0(uVar7);
      uVar4 = FUN_0012edf0(uVar9);
      uVar5 = FUN_0012edf0(uVar10);
      uVar6 = FUN_0012edf0(local_38);
      uVar5 = FUN_0012eb14(uVar6,uVar5);
      uVar4 = FUN_0012eb14(uVar5,uVar4);
      uVar3 = FUN_0012eb14(uVar4,uVar3);
      if ((param_1 & 2) == 0) {
        uVar2 = DAT_b8007000;
        DAT_b8007000 = uVar2 & 0xffffff00 | uVar10 - 1 & 0xff;
      }
      else {
        uVar2 = DAT_b8007000;
        DAT_b8007000 = uVar2 & 0xffff00ff | (uVar10 - 1 & 0xff) << 8;
      }
      param_2[10] = (char)uVar9;
      if (uVar9 == 4) {
        uVar9 = 1;
      }
      else if (uVar9 < 5) {
        if (uVar9 == 1) {
          uVar9 = 4;
        }
        else if (uVar9 == 2) {
          uVar9 = 0;
        }
      }
      else if (uVar9 == 8) {
        uVar9 = 2;
      }
      else if (uVar9 == 0x10) {
        uVar9 = 3;
      }
      param_2[0xb] = (char)uVar10;
      uVar10 = DAT_b8007004;
      DAT_b8007004 = uVar10 & ~(7 << ((param_1 & 7) << 2)) | (uVar9 & 7) << ((param_1 & 7) << 2);
      if ((param_1 & 0x10) == 0) {
        *(uint *)(&DAT_b800700c + (param_1 & 7) * 0xc) = uVar7 - 1;
        iVar8 = divide((byte)param_2[8] * uVar7,100);
        *(int *)(&DAT_b8007010 + (param_1 & 7) * 0xc) = iVar8 + -1;
      }
      else if (*(int *)(param_2 + 0xc) == 0) {
        *(uint *)(&DAT_b800700c + (param_1 & 7) * 0xc) = uVar7 - 1;
        iVar8 = divide((byte)param_2[8] * uVar7,100);
        *(int *)(&DAT_b8007010 + (param_1 & 7) * 0xc) = iVar8 + -1;
      }
      else {
        *(int *)(&DAT_b800700c + (param_1 & 7) * 0xc) = *(int *)(param_2 + 0xc) + -1;
        iVar8 = FUN_0010d398(*(int *)(param_2 + 0xc) * (uint)(byte)param_2[8],100);
        *(int *)(&DAT_b8007010 + (param_1 & 7) * 0xc) = iVar8 + -1;
      }
    }
    else {
      uVar3 = 0;
    }
  }
  return uVar3;
}

