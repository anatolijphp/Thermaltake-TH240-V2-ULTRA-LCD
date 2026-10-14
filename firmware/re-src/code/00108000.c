// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00108000-0x0010bfff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_001082e8 @ 001082e8, 232 bytes
// called by: FUN_0010d3f8
void FUN_001082e8(uint param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    if ((param_1 & 0x10) == 0) {
      uVar1 = DAT_b800707c;
      DAT_b800707c = uVar1 & ~(1 << (param_1 & 7));
    }
    else {
      uVar1 = DAT_b8007078;
      DAT_b8007078 = uVar1 & ~(1 << (param_1 & 7));
    }
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 & ~(3 << ((param_1 & 7) << 1));
  }
  else {
    if ((param_1 & 0x10) == 0) {
      uVar1 = DAT_b800707c;
      DAT_b800707c = uVar1 | 1 << (param_1 & 7);
    }
    else {
      uVar1 = DAT_b8007078;
      DAT_b8007078 = uVar1 | 1 << (param_1 & 7);
    }
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 & ~(3 << ((param_1 & 7) << 1)) | 2 << ((param_1 & 7) << 1);
  }
  return;
}

// ==== FUN_001083d0 @ 001083d0, 28 bytes
// called by: screen_create_proc
void FUN_001083d0(undefined4 param_1,undefined4 param_2)

{
  FUN_0010c414(param_1,param_2);
  return;
}

// ==== FUN_001083ec @ 001083ec, 12 bytes
void FUN_001083ec(void)

{
  FUN_0010c5a8();
  return;
}

// ==== FUN_001083f8 @ 001083f8, 12 bytes
// called by: usb_command
undefined4 FUN_001083f8(void)

{
  return DAT_00182ea0;
}

// ==== FUN_00108404 @ 00108404, 36 bytes
// called by: FUN_001173b0, FUN_0012a460
void FUN_00108404(uint param_1)

{
  DAT_00182ea0 = param_1 | 0x80000000;
  DAT_b1002050 = param_1;
  return;
}

// ==== FUN_00108428 @ 00108428, 60 bytes
void FUN_00108428(int param_1)

{
  undefined4 local_8;
  
  for (; param_1 != 0; param_1 = param_1 + -1) {
    for (local_8 = 0; local_8 < 10; local_8 = local_8 + 1) {
    }
  }
  return;
}

// ==== FUN_0010846c @ 0010846c, 24 bytes
// called by: FUN_0010c414
void FUN_0010846c(void)

{
  uint uVar1;
  
  uVar1 = DAT_b1002000;
  DAT_b1002000 = uVar1 | 1;
  return;
}

// ==== FUN_00108484 @ 00108484, 24 bytes
// called by: FUN_0010c414, FUN_0010c5a8
void FUN_00108484(void)

{
  uint uVar1;
  
  uVar1 = DAT_b1002000;
  DAT_b1002000 = uVar1 & 0xfffffffe;
  return;
}

// ==== FUN_00108564 @ 00108564, 72 bytes
// called by: FUN_001085e8
int FUN_00108564(int param_1,int param_2)

{
  int extraout_r1;
  int extraout_r1_00;
  
  FUN_0010d398(param_1,param_2);
  if (extraout_r1 != 0) {
    FUN_0010d398(param_1,param_2);
    param_1 = (param_2 - extraout_r1_00) + param_1;
  }
  return param_1;
}

// ==== FUN_001085ac @ 001085ac, 60 bytes
// called by: FUN_001085e8
bool FUN_001085ac(void)

{
  bool bVar1;
  
  bVar1 = DAT_00182ea4 != 0;
  if (bVar1) {
    FUN_0010cbf0(DAT_00182ea4 | 0x80000000,DAT_00182ea8);
  }
  return bVar1;
}

// ==== FUN_001085e8 @ 001085e8, 168 bytes
// called by: FUN_0010c414
bool FUN_001085e8(int param_1)

{
  int iVar1;
  bool bVar2;
  
  DAT_00182ea8 = *(int *)(param_1 + 0xc) + 0x100;
  DAT_00182ea4 = mem_alloc(DAT_00182ea8);
  DAT_00182ea0 = FUN_00108564(DAT_00182ea4,0x100);
  bVar2 = DAT_00182ea0 != 0;
  if (bVar2) {
    FUN_001085ac();
    DAT_b1002050 = DAT_00182ea0;
    iVar1 = DAT_b1002050;
    DAT_b100200c = iVar1 + *(int *)(param_1 + 0xc);
    DAT_00182ea0 = DAT_00182ea0 | 0x80000000;
  }
  else {
    DAT_00182ea0 = 0;
  }
  return bVar2;
}

// ==== FUN_00108690 @ 00108690, 44 bytes
// called by: FUN_0010c414
undefined4 FUN_00108690(uint param_1)

{
  DAT_00182ea0 = param_1 | 0x80000000;
  DAT_b1002050 = param_1;
  return 1;
}

// ==== FUN_001086bc @ 001086bc, 60 bytes
// called by: FUN_0010c5a8
undefined4 FUN_001086bc(void)

{
  if (DAT_00182ea4 != 0) {
    mem_free(DAT_00182ea4);
  }
  DAT_00182ea4 = 0;
  DAT_00182ea0 = 0;
  return 1;
}

// ==== FUN_00108784 @ 00108784, 56 bytes
// called by: FUN_0010c414
void FUN_00108784(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = DAT_b1002004;
    DAT_b1002004 = uVar1 | 0x100;
  }
  else {
    uVar1 = DAT_b1002004;
    DAT_b1002004 = uVar1 & 0xfffffeff;
  }
  return;
}

// ==== FUN_001087bc @ 001087bc, 32 bytes
// called by: FUN_0010c414
void FUN_001087bc(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002004;
  DAT_b1002004 = uVar1 & 0xfffffffc | param_1 & 3;
  return;
}

// ==== FUN_00108854 @ 00108854, 32 bytes
// called by: FUN_0010c414
void FUN_00108854(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002000;
  DAT_b1002000 = uVar1 & 0xffcfffff | (param_1 & 3) << 0x14;
  return;
}

// ==== FUN_00108874 @ 00108874, 32 bytes
// called by: FUN_0010c414
void FUN_00108874(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002000;
  DAT_b1002000 = uVar1 & 0xfffffff1 | (param_1 & 7) << 1;
  return;
}

// ==== FUN_001088a4 @ 001088a4, 32 bytes
// called by: FUN_0010c414
void FUN_001088a4(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002000;
  DAT_b1002000 = uVar1 & 0xfffeffff | (param_1 & 1) << 0x10;
  return;
}

// ==== FUN_001088c4 @ 001088c4, 416 bytes
// called by: FUN_0010c414
void FUN_001088c4(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 | 0xc0000000;
    uVar1 = DAT_b0000088;
    DAT_b0000088 = uVar1 | 0xffff;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 | 0xfc0000;
  }
  else if (param_1 == 1) {
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 | 0xc0000000;
    uVar1 = DAT_b0000088;
    DAT_b0000088 = uVar1 | 0x3ffff;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 | 0xfc0000;
  }
  else if (param_1 == 2) {
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 | 0xc0000000;
    DAT_b0000088 = 0xffffffff;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 | 0xfc0000;
  }
  else if (param_1 == 3) {
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 | 0xc0000000;
    DAT_b0000088 = 0xffffffff;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 | 0xfc0000;
    uVar1 = DAT_b0000090;
    DAT_b0000090 = uVar1 | 0xf;
  }
  else if (param_1 == 4) {
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 | 0xc0000000;
    DAT_b0000088 = 0xffffffff;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 | 0xfc0000;
    uVar1 = DAT_b0000090;
    DAT_b0000090 = uVar1 | 0xf;
    uVar1 = DAT_b0000084;
    DAT_b0000084 = uVar1 & 0xfc003fff | 0x2aa8000;
  }
  return;
}

// ==== FUN_00108da4 @ 00108da4, 84 bytes
// called by: FUN_0010c414
void FUN_00108da4(byte *param_1)

{
  uint uVar1;
  
  *param_1 = *param_1 - 1;
  param_1[1] = param_1[1] - 1;
  param_1[2] = param_1[2] - 1;
  uVar1 = DAT_b1002010;
  DAT_b1002010 = (uint)param_1[2] |
                 uVar1 & 0xff000000 | (uint)*param_1 << 0x10 | (uint)param_1[1] << 8;
  return;
}

// ==== FUN_00108df8 @ 00108df8, 48 bytes
// called by: FUN_0010c414
void FUN_00108df8(byte *param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002014;
  DAT_b1002014 = (uint)param_1[2] |
                 uVar1 & 0xff000000 | (uint)*param_1 << 0x10 | (uint)param_1[1] << 8;
  return;
}

// ==== FUN_00108e28 @ 00108e28, 100 bytes
// called by: FUN_0010c414
void FUN_00108e28(short *param_1)

{
  uint uVar1;
  
  if (*param_1 != 0) {
    *param_1 = *param_1 + -1;
  }
  DAT_b1002018 = CONCAT22(*param_1,param_1[1]);
  uVar1 = DAT_b100201c;
  DAT_b100201c = uVar1 & 0xfc00ffff | ((ushort)param_1[2] & 0xffff07ff) << 0x10;
  return;
}

// ==== FUN_00108e8c @ 00108e8c, 152 bytes
// called by: FUN_0010c414
void FUN_00108e8c(char *param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100201c;
  DAT_b100201c = uVar1 & 0xfffffff0 | (uint)(*param_1 == '\0') << 3 |
                 (uint)(param_1[1] == '\0') << 2 | (uint)(param_1[2] == '\0') << 1 |
                 (uint)(param_1[3] == '\0');
  return;
}

// ==== FUN_00108f24 @ 00108f24, 176 bytes
void FUN_00108f24(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,
                 int param_7,int param_8)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar1 = DAT_b1002040;
  if (param_4 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0x20;
  }
  if (param_5 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = 0x10;
  }
  if (param_6 == 0) {
    uVar4 = 0;
  }
  else {
    uVar4 = 8;
  }
  if (param_7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = 4;
  }
  DAT_b1002040 = uVar1 & 0xffff30c2 | param_1 << 0xe | param_2 << 10 | param_3 << 8 | uVar2 | uVar3
                 | uVar4 | uVar5 | (uint)(param_8 != 0);
  return;
}

// ==== FUN_00108fd4 @ 00108fd4, 28 bytes
// called by: FUN_0010c414
void FUN_00108fd4(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002040;
  DAT_b1002040 = uVar1 & 0xfffff3ff | param_1 << 10;
  return;
}

// ==== FUN_001091f4 @ 001091f4, 36 bytes
// called by: FUN_00116e40
void FUN_001091f4(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002008;
  DAT_b1002008 = uVar1 & 0xffe8ffff | (param_1 & 0x17) << 0x10;
  return;
}

// ==== FUN_0010923c @ 0010923c, 24 bytes
// called by: FUN_00109290
void FUN_0010923c(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002008;
  DAT_b1002008 = uVar1 & ~param_1;
  return;
}

// ==== FUN_00109268 @ 00109268, 40 bytes
// called by: FUN_00109290
bool FUN_00109268(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b1002008;
  return (uVar1 & param_1 << 0x10) != 0;
}

// ==== FUN_00109290 @ 00109290, 304 bytes
void FUN_00109290(void)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = DAT_b1002008;
  if ((uVar1 & 1) != 0) {
    iVar2 = FUN_00109268(1);
    if ((iVar2 == 1) && (DAT_007bcf8c != (code *)0x0)) {
      (*DAT_007bcf8c)(0,1);
    }
    FUN_0010923c(1);
  }
  if ((uVar1 & 2) != 0) {
    iVar2 = FUN_00109268(2);
    if ((iVar2 == 1) && (DAT_007bcf90 != (code *)0x0)) {
      (*DAT_007bcf90)(0,2);
    }
    FUN_0010923c(2);
  }
  if ((uVar1 & 4) != 0) {
    iVar2 = FUN_00109268(4);
    if ((iVar2 == 1) && (DAT_007bcf94 != (code *)0x0)) {
      (*DAT_007bcf94)(0,4);
    }
    FUN_0010923c(4);
  }
  if ((uVar1 & 0x10) != 0) {
    iVar2 = FUN_00109268(0x10);
    if ((iVar2 == 1) && (DAT_007bcf98 != (code *)0x0)) {
      (*DAT_007bcf98)(0,0x10);
    }
    FUN_0010923c(0x10);
  }
  return;
}

// ==== FUN_001093c0 @ 001093c0, 212 bytes
// called by: FUN_00116e40
undefined4 FUN_001093c0(int param_1,undefined4 param_2,undefined4 *param_3)

{
  if (param_1 == 1) {
    *param_3 = DAT_007bcf8c;
    DAT_007bcf8c = param_2;
  }
  else if (param_1 == 2) {
    *param_3 = DAT_007bcf90;
    DAT_007bcf90 = param_2;
  }
  else if (param_1 == 4) {
    *param_3 = DAT_007bcf94;
    DAT_007bcf94 = param_2;
  }
  else {
    if (param_1 != 0x10) {
      return 1;
    }
    *param_3 = DAT_007bcf98;
    DAT_007bcf98 = param_2;
  }
  FUN_00100520(1,8,FUN_00109290);
  FUN_0010043c(8);
  return 0;
}

// ==== FUN_001096e0 @ 001096e0, 168 bytes
// called by: FUN_00100aa4
undefined4 FUN_001096e0(void)

{
  int iVar1;
  
  if (DAT_00182fec == '\0') {
    iVar1 = 0;
    do {
      *(uint *)(iVar1 * 4 + 0x7c0000) = iVar1 << 0x14 | 0xdfe;
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x40);
    iVar1 = 0x40;
    do {
      *(uint *)(iVar1 * 4 + 0x7c0000) = iVar1 << 0x14 | 0xdf2;
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x800);
    iVar1 = 0x800;
    do {
      *(uint *)(iVar1 * 4 + 0x7c0000) = iVar1 * 0x100000 + 0x80000000U | 0xdf2;
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x840);
    iVar1 = 0x840;
    do {
      *(uint *)(iVar1 * 4 + 0x7c0000) = iVar1 << 0x14 | 0xdf2;
      iVar1 = iVar1 + 1;
    } while (iVar1 < 0x1000);
    DAT_00182fec = '\x01';
  }
  FUN_0010d3d4(0x7c0000);
  return 0;
}

// ==== FUN_00109798 @ 00109798, 72 bytes
void FUN_00109798(void)

{
  uint uVar1;
  
  uVar1 = DAT_b100d00c;
  if ((uVar1 & 1) != 0) {
    DAT_b100d00c = uVar1 & 2 | 1;
    if (DAT_00182ff0 != (code *)0x0) {
      (*DAT_00182ff0)();
    }
  }
  return;
}

// ==== FUN_001097e0 @ 001097e0, 92 bytes
// called by: FUN_00115160
undefined4 FUN_001097e0(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 | 0x1000020;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 | 0x10000;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 & 0xfffeffff;
  FUN_00100520(1,0xb,FUN_00109798);
  FUN_0010043c(0xb);
  return 0;
}

// ==== FUN_00109854 @ 00109854, 44 bytes
// called by: FUN_00115160, FUN_0011b9d4, FUN_00109e94
undefined8 FUN_00109854(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  DAT_b100d030 = param_1;
  DAT_b100d034 = param_2;
  DAT_b100d038 = param_3;
  DAT_b100d03c = param_4;
  return CONCAT44(param_2,param_1);
}

// ==== FUN_001098b4 @ 001098b4, 40 bytes
// called by: FUN_00115160, FUN_0011b9d4, FUN_00109e94
undefined4 FUN_001098b4(uint param_1)

{
  undefined4 uVar1;
  
  if ((param_1 & 0xffffffc0) == 0) {
    DAT_b100d004 = param_1;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff1502;
  }
  return uVar1;
}

// ==== FUN_001098ec @ 001098ec, 40 bytes
// called by: FUN_0010a0bc, FUN_00115160, FUN_0011b9d4, FUN_00109e94
undefined4 FUN_001098ec(uint param_1)

{
  undefined4 uVar1;
  
  if ((param_1 & 0xfffffff8) == 0) {
    DAT_b100d008 = param_1;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffff1503;
  }
  return uVar1;
}

// ==== FUN_001099f0 @ 001099f0, 168 bytes
// called by: FUN_00115160, FUN_00109e94
void FUN_001099f0(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_b100d01c;
  DAT_b100d01c = uVar1 & 0xffff0000 | param_1 & 0xffff;
  uVar1 = DAT_b100d018;
  DAT_b100d018 = uVar1 & 0xffff0000 | param_1 >> 0x10;
  uVar1 = DAT_b100d014;
  DAT_b100d014 = uVar1 & 0xffff0000 | param_2 & 0xffff;
  uVar1 = DAT_b100d010;
  DAT_b100d010 = uVar1 & 0xffff0000 | param_2 >> 0x10;
  return;
}

// ==== FUN_00109aec @ 00109aec, 136 bytes
// called by: FUN_00115160, FUN_00109e94
void FUN_00109aec(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = DAT_b100d01c;
  DAT_b100d01c = uVar1 & 0xffff | param_1 << 0x10;
  uVar1 = DAT_b100d018;
  DAT_b100d018 = uVar1 & 0xffff | param_1 & 0xffff0000;
  uVar1 = DAT_b100d014;
  DAT_b100d014 = uVar1 & 0xffff | param_2 << 0x10;
  uVar1 = DAT_b100d010;
  DAT_b100d010 = uVar1 & 0xffff | param_2 & 0xffff0000;
  return;
}

// ==== FUN_00109bb8 @ 00109bb8, 64 bytes
// called by: FUN_00115160, FUN_0011b9d4, FUN_00109e94
void FUN_00109bb8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  DAT_b100d040 = param_1;
  DAT_b100d048 = param_2;
  DAT_b100d050 = param_3;
  DAT_b100d054 = param_4;
  DAT_b100d020 = (int)(short)param_5;
  DAT_b100d024 = (int)param_5._2_2_;
  return;
}

// ==== FUN_00109bf8 @ 00109bf8, 48 bytes
// called by: FUN_0010a0bc, FUN_00115160, FUN_0011b9d4, FUN_00109e94
void FUN_00109bf8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  DAT_b100d044 = param_1;
  DAT_b100d04c = param_4;
  DAT_b100d028 = (int)(short)param_5;
  DAT_b100d02c = (int)param_5._2_2_;
  return;
}

// ==== FUN_00109c28 @ 00109c28, 48 bytes
// called by: FUN_0010a0bc
void FUN_00109c28(uint param_1)

{
  DAT_b100d060 = param_1 & 0xff000000 | (param_1 >> 0x10 & 0xff) << 0x10 |
                 (param_1 >> 8 & 0xff) << 8 | param_1 & 0xff;
  return;
}

// ==== FUN_00109c88 @ 00109c88, 16 bytes
// called by: FUN_00109e78
uint FUN_00109c88(void)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  return uVar1 & 1;
}

// ==== FUN_00109c98 @ 00109c98, 28 bytes
// called by: FUN_0010a0bc
void FUN_00109c98(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 & 0xfffffffb | param_1 << 2;
  return;
}

// ==== FUN_00109cc8 @ 00109cc8, 28 bytes
// called by: FUN_00115160, FUN_0011b9d4, FUN_00109e94
void FUN_00109cc8(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 & 0xffffff8f | param_1 << 4;
  return;
}

// ==== FUN_00109d28 @ 00109d28, 80 bytes
// called by: FUN_00109e94
void FUN_00109d28(int param_1,uint param_2,byte *param_3)

{
  uint uVar1;
  
  for (uVar1 = 0; uVar1 < param_2; uVar1 = uVar1 + 1) {
    *(uint *)(&DAT_b100d400 + uVar1 * 4 + param_1) =
         (uint)param_3[3] << 0x18 | (uint)param_3[2] << 0x10 | (uint)param_3[1] << 8 |
         (uint)*param_3;
    param_3 = param_3 + 4;
  }
  return;
}

// ==== FUN_00109d78 @ 00109d78, 28 bytes
// called by: FUN_0010a0bc, FUN_00115160, FUN_00109e94
void FUN_00109d78(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 & 0xfffff7ff | param_1 << 0xb;
  return;
}

// ==== FUN_00109da8 @ 00109da8, 28 bytes
// called by: FUN_00115160, FUN_0011b9d4, FUN_00109e94
void FUN_00109da8(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 & 0xfffff8ff | param_1 << 8;
  return;
}

// ==== FUN_00109dd8 @ 00109dd8, 28 bytes
// called by: FUN_00115160
void FUN_00109dd8(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 & 0xfffffff7 | param_1 << 3;
  return;
}

// ==== FUN_00109e08 @ 00109e08, 24 bytes
// called by: FUN_0010a0bc, FUN_00115160, FUN_0011b9d4, FUN_00109e94
void FUN_00109e08(void)

{
  uint uVar1;
  
  uVar1 = DAT_b100d000;
  DAT_b100d000 = uVar1 | 1;
  return;
}

// ==== FUN_00109e78 @ 00109e78, 24 bytes
// called by: FUN_0010a0bc, FUN_00115160, FUN_00109e94
void FUN_00109e78(int param_1)

{
  do {
    param_1 = FUN_00109c88(param_1);
  } while (param_1 != 0);
  return;
}

// ==== FUN_00109e94 @ 00109e94, 548 bytes
void FUN_00109e94(void)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int in_stack_00000000;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  int in_stack_0000000c;
  int in_stack_00000010;
  undefined4 in_stack_00000014;
  int in_stack_00000018;
  int in_stack_0000001c;
  int in_stack_00000020;
  int in_stack_00000024;
  undefined4 in_stack_00000028;
  undefined4 *in_stack_0000002c;
  undefined4 local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  
  FUN_00109e78();
  FUN_00109d78(0);
  if (in_stack_00000000 != 0) {
    FUN_00109d28(0,0x100,in_stack_00000000);
  }
  FUN_00109bb8(in_stack_00000004,in_stack_00000008,in_stack_0000000c << 0x10,
               in_stack_00000010 << 0x10,in_stack_00000014);
  uVar1 = DAT_b100d008;
  if ((uVar1 & 1) == 0) {
    iVar3 = 2;
  }
  else {
    iVar3 = 4;
  }
  local_18 = in_stack_00000024;
  local_14 = in_stack_00000028;
  local_24 = in_stack_0000001c * iVar3 + in_stack_00000020 * in_stack_00000024 + in_stack_00000018;
  local_20 = 0;
  local_1c = 0;
  FUN_00109bf8(local_24,0,0,in_stack_00000024,in_stack_00000028);
  if (in_stack_0000002c == (undefined4 *)0x0) {
    local_2c = DAT_00181e3c;
    local_28 = DAT_00181e40;
    FUN_00109854(DAT_00181e34,DAT_00181e38,DAT_00181e3c,DAT_00181e40);
    FUN_00109cc8(0);
    uVar2 = FUN_00109da8(0);
  }
  else {
    FUN_00109854(*in_stack_0000002c,in_stack_0000002c[1],in_stack_0000002c[2],in_stack_0000002c[3]);
    FUN_001098b4(*(undefined1 *)(in_stack_0000002c + 4));
    FUN_001098ec(*(undefined1 *)((int)in_stack_0000002c + 0x11));
    FUN_00109cc8(in_stack_0000002c[5]);
    FUN_0010c9b8(&local_2c,in_stack_0000002c + 6,8);
    FUN_001099f0(local_2c,local_28);
    FUN_0010c9b8(&local_2c,in_stack_0000002c + 8,8);
    FUN_00109aec(local_2c,local_28);
    uVar2 = FUN_00109da8(*(undefined1 *)(in_stack_0000002c + 10));
  }
  FUN_00109e08(uVar2);
  FUN_00109e78();
  return;
}

// ==== FUN_0010a0bc @ 0010a0bc, 248 bytes
void FUN_0010a0bc(void)

{
  uint uVar1;
  int iVar2;
  short sStack00000000;
  short sStack00000002;
  short sStack00000004;
  short sStack00000006;
  undefined4 in_stack_00000008;
  int in_stack_0000000c;
  int in_stack_00000010;
  undefined1 in_stack_00000014;
  undefined1 in_stack_00000018;
  undefined4 local_c;
  
  FUN_00109e78();
  FUN_00109d78(1);
  FUN_00109c28(in_stack_00000008);
  uVar1 = DAT_b100d008;
  if ((uVar1 & 1) == 0) {
    iVar2 = 2;
  }
  else {
    iVar2 = 4;
  }
  local_c = CONCAT22(sStack00000006 - sStack00000004,sStack00000002 - sStack00000000);
  FUN_00109bf8(iVar2 * sStack00000000 + sStack00000004 * in_stack_00000010 + in_stack_0000000c,0,0,
               in_stack_00000010,local_c);
  FUN_001098ec(in_stack_00000014);
  FUN_00109c98(in_stack_00000018);
  FUN_00109e08();
  FUN_00109e78();
  return;
}

// ==== FUN_0010a1d0 @ 0010a1d0, 8 bytes
// called by: FUN_00106f58
undefined4 FUN_0010a1d0(void)

{
  return 0;
}

// ==== FUN_0010a1d8 @ 0010a1d8, 28 bytes
// called by: FUN_00106fd8
void FUN_0010a1d8(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 & 0xffff83ff;
  return;
}

// ==== FUN_0010a1f4 @ 0010a1f4, 60 bytes
// called by: FUN_00106f44
undefined4 FUN_0010a1f4(int param_1)

{
  undefined4 uVar1;
  
  if (param_1 < 5) {
    if ((*(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0x800000) == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a230 @ 0010a230, 104 bytes
// called by: FUN_00106de4, FUN_00106d30, FUN_00106cec, FUN_00106d54
void FUN_0010a230(uint param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  uVar1 = 0x400 << (param_1 & 0xff);
  puVar3 = (uint *)(&DAT_b0008000 + param_1 * 0x100);
  if (param_2 == 0) {
    *puVar3 = *puVar3 & 0xfffffffe;
    uVar2 = DAT_b0000204;
    DAT_b0000204 = uVar2 & ~uVar1;
  }
  else {
    uVar2 = DAT_b0000204;
    DAT_b0000204 = uVar2 | uVar1;
    *puVar3 = *puVar3 | 1;
  }
  return;
}

// ==== FUN_0010a2c0 @ 0010a2c0, 24 bytes
// called by: FUN_0010a2d8
void FUN_0010a2c0(int param_1)

{
  *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
       *(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xfffffdff;
  return;
}

// ==== FUN_0010a2d8 @ 0010a2d8, 144 bytes
// called by: FUN_001069e8
undefined4 FUN_0010a2d8(int param_1,undefined4 *param_2,undefined4 *param_3,uint param_4)

{
  byte bVar1;
  undefined4 uVar2;
  
  if (param_1 < 5) {
    FUN_0010a2c0(param_1);
    *(undefined4 *)(&DAT_b0008004 + param_1 * 0x100) = *param_2;
    bVar1 = *(byte *)(param_2 + 1);
    *(undefined4 *)(&DAT_b0008008 + param_1 * 0x100) = *param_3;
    *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
         (*(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xffffffcf | (uint)bVar1 << 4) & 0xffffff3f |
         (uint)*(byte *)(param_3 + 1) << 6;
    if (param_4 < 0x1000000) {
      *(uint *)(&DAT_b000800c + param_1 * 0x100) = param_4;
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffffb080;
    }
  }
  else {
    uVar2 = 0xffffb080;
  }
  return uVar2;
}

// ==== FUN_0010a368 @ 0010a368, 136 bytes
undefined4 FUN_0010a368(int param_1,int param_2,undefined4 *param_3,undefined1 *param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  if (param_1 < 5) {
    if ((param_2 == 0) || (param_2 == 1)) {
      uVar2 = *(uint *)(&DAT_b0008000 + param_1 * 0x100);
      if (param_2 == 0) {
        *param_3 = *(undefined4 *)(&DAT_b0008004 + param_1 * 0x100);
        *param_4 = (char)((uVar2 & 0x30) >> 4);
      }
      else {
        *param_3 = *(undefined4 *)(&DAT_b0008008 + param_1 * 0x100);
        *param_4 = (char)((uVar2 & 0xc0) >> 6);
      }
      uVar1 = 0;
    }
    else {
      uVar1 = 0xffffb080;
    }
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a41c @ 0010a41c, 64 bytes
// called by: FUN_00106fe4
undefined4 FUN_0010a41c(int param_1,int param_2)

{
  undefined4 uVar1;
  
  if ((param_1 < 5) && (param_1 != 0)) {
    *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
         *(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xffe7ffff | param_2 << 0x13;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a490 @ 0010a490, 528 bytes
// called by: FUN_00106fe4
undefined4 FUN_0010a490(uint param_1,uint param_2,int param_3)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  
  if (((int)param_1 < 5) && (param_1 != 0)) {
    if ((param_3 == 1) && (param_2 == 4)) {
      uVar1 = 0xffffb080;
    }
    else {
      puVar4 = (uint *)(&DAT_b0008000 + param_1 * 0x100);
      if (param_3 == 1) {
        *puVar4 = *puVar4 & 0xfffffff3 | 8;
        uVar3 = 0xf0000 << ((param_1 - 1) * 4 & 0xff);
        uVar2 = 7 << ((param_1 - 1) * 4 & 0xff);
      }
      else {
        *puVar4 = *puVar4 & 0xfffffff3 | 4;
        uVar3 = 0xf << ((param_1 - 1) * 4 & 0xff);
        uVar2 = 0x70000 << ((param_1 - 1) * 4 & 0xff);
      }
      uVar5 = DAT_b000002c;
      if ((param_2 == 0) || (param_2 == 1)) {
        uVar2 = uVar5 & ~uVar3 & ~uVar2;
      }
      else {
        uVar2 = uVar5 & ~uVar3 | uVar2;
      }
      if (param_3 == 1) {
        uVar3 = 0x70000;
        for (uVar5 = 1; uVar5 < 5; uVar5 = uVar5 + 1) {
          if (((uVar5 != param_1) && ((uVar2 & uVar3) >> ((uVar5 - 1) * 4 + 0x10 & 0xff) == param_2)
              ) && ((uVar2 = uVar2 | uVar3, param_2 == 0 || (param_2 == 1)))) {
            uVar2 = uVar2 | 7 << ((uVar5 - 1) * 4 & 0xff);
          }
          uVar3 = uVar3 << 4;
        }
        uVar2 = uVar2 | param_2 << ((param_1 - 1) * 4 + 0x10 & 0xff);
        if ((param_2 == 0) || (param_2 == 1)) {
          uVar2 = uVar2 | param_2 << ((param_1 - 1) * 4 & 0xff);
        }
      }
      else {
        uVar3 = 7;
        for (uVar5 = 1; uVar5 < 5; uVar5 = uVar5 + 1) {
          if (((uVar5 != param_1) && ((uVar2 & uVar3) >> ((uVar5 - 1) * 4 & 0xff) == param_2)) &&
             ((uVar2 = uVar2 | uVar3, param_2 == 0 || (param_2 == 1)))) {
            uVar2 = uVar2 | 0x70000 << ((uVar5 - 1) * 4 & 0xff);
          }
          uVar3 = uVar3 << 4;
        }
        uVar2 = uVar2 | param_2 << ((param_1 - 1) * 4 & 0xff);
        if ((param_2 == 0) || (param_2 == 1)) {
          uVar2 = uVar2 | param_2 << ((param_1 - 1) * 4 + 0x10 & 0xff);
        }
      }
      DAT_b000002c = uVar2;
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a71c @ 0010a71c, 76 bytes
// called by: FUN_0010703c
undefined4 FUN_0010a71c(int param_1,int param_2)

{
  undefined4 uVar1;
  
  if ((param_1 < 5) && (param_1 != 0)) {
    if (param_2 < 0x10) {
      *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
           *(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xffff0fff | param_2 << 0xc;
      uVar1 = 0;
    }
    else {
      uVar1 = 0xffffb080;
    }
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a7cc @ 0010a7cc, 52 bytes
// called by: FUN_00106ef0
undefined4 FUN_0010a7cc(int param_1)

{
  undefined4 uVar1;
  
  if (param_1 < 5) {
    *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
         *(uint *)(&DAT_b0008000 + param_1 * 0x100) | 0x800001;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a83c @ 0010a83c, 56 bytes
// called by: FUN_00106cec, FUN_00106d54, FUN_0010703c
undefined4 FUN_0010a83c(int param_1,uint param_2)

{
  undefined4 uVar1;
  
  if (((int)param_2 < 1) || (0xf < (int)param_2)) {
    uVar1 = 0xffffb080;
  }
  else {
    (&DAT_b0008020)[param_1 * 0x40] = (&DAT_b0008020)[param_1 * 0x40] | param_2;
    uVar1 = 0;
  }
  return uVar1;
}

// ==== FUN_0010a874 @ 0010a874, 24 bytes
// called by: FUN_00106de4, FUN_00106d30, FUN_0010703c
void FUN_0010a874(int param_1,uint param_2)

{
  (&DAT_b0008020)[param_1 * 0x40] = (&DAT_b0008020)[param_1 * 0x40] & ~param_2;
  return;
}

// ==== FUN_0010a938 @ 0010a938, 96 bytes
// called by: FUN_00106970
undefined4 FUN_0010a938(int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  
  if ((((param_1 < 5) && (param_1 != 1)) && (param_1 != 2)) && ((param_1 != 3 && (param_1 != 4)))) {
    *(uint *)(&DAT_b0008028 + param_1 * 0x100) =
         *(uint *)(&DAT_b0008028 + param_1 * 0x100) & 0xf0f0ffff | param_2 << 0x18 | param_3 << 0x10
    ;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010a9c4 @ 0010a9c4, 92 bytes
// called by: FUN_001069c8, FUN_00106970
undefined4 FUN_0010a9c4(int param_1,int param_2,uint param_3)

{
  undefined4 uVar1;
  
  if ((((param_1 < 5) && (param_1 != 1)) && (param_1 != 2)) && ((param_1 != 3 && (param_1 != 4)))) {
    *(uint *)(&DAT_b0008028 + param_1 * 0x100) =
         *(uint *)(&DAT_b0008028 + param_1 * 0x100) & 0xfffffffc | param_2 << 1 | param_3;
    uVar1 = 0;
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010abe4 @ 0010abe4, 1808 bytes
void FUN_0010abe4(void)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  
  uVar1 = DAT_b0008024;
  if ((uVar1 & 0x80000000) != 0) {
    uVar1 = DAT_b0008024;
    if ((uVar1 & 0x1000000) == 0) {
      uVar1 = DAT_b0008024;
      if ((uVar1 & 0x2000000) == 0) {
        uVar1 = DAT_b0008024;
        if ((uVar1 & 0x4000000) == 0) {
          uVar1 = DAT_b0008024;
          if ((uVar1 & 0x8000000) == 0) {
            uVar1 = DAT_b0008420;
            if ((uVar1 & 4) == 0) {
              uVar3 = DAT_b0008424;
              uVar1 = DAT_b0008420;
              uVar3 = uVar3 & uVar1;
            }
            else {
              uVar3 = DAT_b0008424;
              uVar1 = DAT_b0008420;
              uVar3 = uVar3 & uVar1;
            }
            if ((uVar3 & 1) == 0) {
              if ((uVar3 & 2) == 0) {
                if ((uVar3 & 8) == 0) {
                  uVar1 = DAT_b0008424;
                  if ((uVar1 & 0xf00) != 0) {
                    if ((uVar1 & 0x200) == 0) {
                      if ((uVar1 & 0x400) == 0) {
                        if ((uVar1 & 0x800) == 0) {
                          uVar2 = 0x100;
                        }
                        else {
                          uVar2 = 0x800;
                        }
                      }
                      else {
                        uVar2 = 0x400;
                      }
                    }
                    else {
                      uVar2 = 0x200;
                    }
                    DAT_b0008424 = uVar2;
                    if (DAT_007c6048 != (code *)0x0) {
                      (*DAT_007c6048)(uVar2);
                    }
                  }
                }
                else {
                  DAT_b0008424 = 8;
                  if (DAT_007c604c != (code *)0x0) {
                    (*DAT_007c604c)(0);
                  }
                }
              }
              else {
                DAT_b0008424 = 2;
                if (DAT_007c6044 != (code *)0x0) {
                  (*DAT_007c6044)(0);
                }
              }
            }
            else {
              DAT_b0008424 = 1;
              if (DAT_007c6040 != (code *)0x0) {
                (*DAT_007c6040)(0);
              }
            }
          }
          else {
            uVar1 = DAT_b0008320;
            if ((uVar1 & 4) == 0) {
              uVar3 = DAT_b0008324;
              uVar1 = DAT_b0008320;
              uVar3 = uVar3 & uVar1;
            }
            else {
              uVar3 = DAT_b0008324;
              uVar1 = DAT_b0008320;
              uVar3 = uVar3 & uVar1;
            }
            if ((uVar3 & 1) == 0) {
              if ((uVar3 & 2) == 0) {
                if ((uVar3 & 8) == 0) {
                  uVar1 = DAT_b0008324;
                  if ((uVar1 & 0xf00) != 0) {
                    if ((uVar1 & 0x200) == 0) {
                      if ((uVar1 & 0x400) == 0) {
                        if ((uVar1 & 0x800) == 0) {
                          uVar2 = 0x100;
                        }
                        else {
                          uVar2 = 0x800;
                        }
                      }
                      else {
                        uVar2 = 0x400;
                      }
                    }
                    else {
                      uVar2 = 0x200;
                    }
                    DAT_b0008324 = uVar2;
                    if (DAT_007c6038 != (code *)0x0) {
                      (*DAT_007c6038)(uVar2);
                    }
                  }
                }
                else {
                  DAT_b0008324 = 8;
                  if (DAT_007c603c != (code *)0x0) {
                    (*DAT_007c603c)(0);
                  }
                }
              }
              else {
                DAT_b0008324 = 2;
                if (DAT_007c6034 != (code *)0x0) {
                  (*DAT_007c6034)(0);
                }
              }
            }
            else {
              DAT_b0008324 = 1;
              if (DAT_007c6030 != (code *)0x0) {
                (*DAT_007c6030)(0);
              }
            }
          }
        }
        else {
          uVar1 = DAT_b0008220;
          if ((uVar1 & 4) == 0) {
            uVar3 = DAT_b0008224;
            uVar1 = DAT_b0008220;
            uVar3 = uVar3 & uVar1;
          }
          else {
            uVar3 = DAT_b0008224;
            uVar1 = DAT_b0008220;
            uVar3 = uVar3 & uVar1;
          }
          if ((uVar3 & 1) == 0) {
            if ((uVar3 & 2) == 0) {
              if ((uVar3 & 8) == 0) {
                uVar1 = DAT_b0008224;
                if ((uVar1 & 0xf00) != 0) {
                  if ((uVar1 & 0x200) == 0) {
                    if ((uVar1 & 0x400) == 0) {
                      if ((uVar1 & 0x800) == 0) {
                        uVar2 = 0x100;
                      }
                      else {
                        uVar2 = 0x800;
                      }
                    }
                    else {
                      uVar2 = 0x400;
                    }
                  }
                  else {
                    uVar2 = 0x200;
                  }
                  DAT_b0008224 = uVar2;
                  if (DAT_007c6028 != (code *)0x0) {
                    (*DAT_007c6028)(uVar2);
                  }
                }
              }
              else {
                DAT_b0008224 = 8;
                if (DAT_007c602c != (code *)0x0) {
                  (*DAT_007c602c)(0);
                }
              }
            }
            else {
              DAT_b0008224 = 2;
              if (DAT_007c6024 != (code *)0x0) {
                (*DAT_007c6024)(0);
              }
            }
          }
          else {
            DAT_b0008224 = 1;
            if (DAT_007c6020 != (code *)0x0) {
              (*DAT_007c6020)(0);
            }
          }
        }
      }
      else {
        uVar1 = DAT_b0008120;
        if ((uVar1 & 4) == 0) {
          uVar3 = DAT_b0008124;
          uVar1 = DAT_b0008120;
          uVar3 = uVar3 & uVar1;
        }
        else {
          uVar3 = DAT_b0008124;
          uVar1 = DAT_b0008120;
          uVar3 = uVar3 & uVar1;
        }
        if ((uVar3 & 1) == 0) {
          if ((uVar3 & 2) == 0) {
            if ((uVar3 & 8) == 0) {
              uVar1 = DAT_b0008124;
              if ((uVar1 & 0xf00) != 0) {
                if ((uVar1 & 0x200) == 0) {
                  if ((uVar1 & 0x400) == 0) {
                    if ((uVar1 & 0x800) == 0) {
                      uVar2 = 0x100;
                    }
                    else {
                      uVar2 = 0x800;
                    }
                  }
                  else {
                    uVar2 = 0x400;
                  }
                }
                else {
                  uVar2 = 0x200;
                }
                DAT_b0008124 = uVar2;
                if (DAT_007c6018 != (code *)0x0) {
                  (*DAT_007c6018)(uVar2);
                }
              }
            }
            else {
              DAT_b0008124 = 8;
              if (DAT_007c601c != (code *)0x0) {
                (*DAT_007c601c)(0);
              }
            }
          }
          else {
            DAT_b0008124 = 2;
            if (DAT_007c6014 != (code *)0x0) {
              (*DAT_007c6014)(0);
            }
          }
        }
        else {
          DAT_b0008124 = 1;
          if (DAT_007c6010 != (code *)0x0) {
            (*DAT_007c6010)(0);
          }
        }
      }
    }
    else {
      uVar3 = DAT_b0008024;
      uVar1 = DAT_b0008020;
      uVar3 = uVar3 & uVar1;
      if ((uVar3 & 1) == 0) {
        if ((uVar3 & 2) == 0) {
          if (((uVar3 & 8) != 0) && (DAT_b0008024 = 8, DAT_007c600c != (code *)0x0)) {
            (*DAT_007c600c)(0);
          }
        }
        else {
          DAT_b0008024 = 2;
          if (DAT_007c6004 != (code *)0x0) {
            (*DAT_007c6004)(0);
          }
        }
      }
      else {
        DAT_b0008024 = 1;
        if (DAT_007c6000 != (code *)0x0) {
          (*DAT_007c6000)(0);
        }
      }
    }
  }
  return;
}

// ==== FUN_0010b32c @ 0010b32c, 200 bytes
// called by: FUN_00106c98
undefined4 FUN_0010b32c(int param_1,int param_2,undefined4 param_3,undefined4 *param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  if (param_1 < 5) {
    if ((((param_2 == 1) || (param_2 == 2)) || (param_2 == 4)) || (param_2 == 8)) {
      for (uVar2 = 0; (uVar2 < 4 && (1 << (uVar2 & 0xff) != param_2)); uVar2 = uVar2 + 1) {
      }
      if (param_4 != (undefined4 *)0x0) {
        *param_4 = (&DAT_007c6000)[param_1 * 4 + uVar2];
      }
      (&DAT_007c6000)[param_1 * 4 + uVar2] = param_3;
      FUN_00100520(7,0x13,FUN_0010abe4);
      FUN_00100790(0x13,1);
      FUN_0010043c(0x13);
      uVar1 = 0;
    }
    else {
      uVar1 = 0xffffb080;
    }
  }
  else {
    uVar1 = 0xffffb080;
  }
  return uVar1;
}

// ==== FUN_0010b408 @ 0010b408, 16 bytes
// called by: FUN_00106ac8, FUN_0010b430
void FUN_0010b408(int param_1,undefined4 param_2)

{
  *(undefined4 *)(&DAT_b0008010 + param_1 * 0x100) = param_2;
  return;
}

// ==== FUN_0010b418 @ 0010b418, 24 bytes
// called by: FUN_00106ac8, FUN_0010b430
void FUN_0010b418(int param_1)

{
  *(uint *)(&DAT_b0008000 + param_1 * 0x100) = *(uint *)(&DAT_b0008000 + param_1 * 0x100) | 0x200;
  return;
}

// ==== FUN_0010b430 @ 0010b430, 1008 bytes
undefined4 FUN_0010b430(int param_1,int *param_2,int *param_3)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  int extraout_r1;
  int extraout_r1_00;
  int iVar5;
  uint extraout_r1_01;
  int extraout_r1_02;
  int extraout_r1_03;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint local_4c;
  uint local_40;
  uint local_3c;
  int local_38;
  
  if (param_3[4] < 1) {
    uVar3 = 0xffffb080;
  }
  else {
    uVar1 = (*(uint *)(&DAT_b0008028 + param_1 * 0x100) & 0xf000000) >> 0x18;
    uVar2 = (*(uint *)(&DAT_b0008028 + param_1 * 0x100) & 0xf0000) >> 0x10;
    *(uint *)(&DAT_b0008000 + param_1 * 0x100) =
         (*(uint *)(&DAT_b0008000 + param_1 * 0x100) & 0xffffffcf |
         (uint)*(byte *)(param_3 + 1) << 4) & 0xffffff3f | (uint)*(byte *)(param_3 + 3) << 6;
    FUN_0010b418(param_1);
    FUN_0010b408(param_1,param_2);
    uVar8 = param_3[4];
    iVar10 = *param_3;
    local_38 = param_3[2];
    local_3c = 0;
    local_40 = 0;
    if ((param_3[5] == 0) || ((*(uint *)(&DAT_b0008028 + param_1 * 0x100) & 1) == 0)) {
      local_4c = 0x8000;
    }
    else {
      iVar4 = FUN_0010d398(0x8000,param_3[5]);
      local_4c = param_3[5] * iVar4;
    }
    do {
      if ((char)param_3[1] == '\0') {
        *param_2 = local_3c + iVar10;
        iVar10 = iVar10 + local_3c;
      }
      else if ((char)param_3[1] == '\x01') {
        *param_2 = iVar10 - local_3c;
        iVar10 = iVar10 - local_3c;
      }
      else {
        *param_2 = iVar10;
      }
      if ((char)param_3[3] == '\0') {
        param_2[1] = local_38 + local_40;
        local_38 = local_38 + local_40;
      }
      else if ((char)param_3[3] == '\x01') {
        param_2[1] = local_38 - local_40;
        local_38 = local_38 - local_40;
      }
      else {
        param_2[1] = local_38;
      }
      if (uVar8 < local_4c) {
        uVar9 = 0;
      }
      else {
        uVar9 = uVar8 - local_4c;
        uVar8 = local_4c;
      }
      if (((param_3[5] == 0) || ((*(uint *)(&DAT_b0008028 + param_1 * 0x100) & 1) == 0)) ||
         (param_1 != 0)) {
        local_40 = uVar8;
        local_3c = uVar8;
        if (param_1 == 0) {
          if (uVar1 == 1) {
            if (uVar2 != 1) {
              local_40 = uVar8 >> 1;
            }
          }
          else if (uVar2 == 1) {
            local_40 = uVar8 << 1;
          }
        }
      }
      else {
        FUN_0010d398(uVar8,param_3[5]);
        iVar4 = FUN_0010d398(uVar8,param_3[5]);
        local_3c = (param_3[6] + param_3[5]) * iVar4 + extraout_r1;
        if (uVar1 == 1) {
          if (uVar2 == 1) {
            FUN_0010d398(uVar8,param_3[5]);
            iVar4 = FUN_0010d398(uVar8,param_3[5]);
            local_40 = (param_3[7] + param_3[5]) * iVar4 + extraout_r1_00;
          }
          else {
            iVar4 = FUN_0010d398(uVar8,param_3[5]);
            iVar5 = param_3[7];
            uVar6 = param_3[5];
            FUN_0010d398(uVar8,uVar6);
            local_40 = iVar4 * (iVar5 + (uVar6 >> 1)) + (extraout_r1_01 >> 1);
          }
        }
        else if (uVar2 == 1) {
          iVar4 = FUN_0010d398(uVar8,param_3[5]);
          iVar5 = param_3[5];
          iVar7 = param_3[7];
          FUN_0010d398(uVar8,param_3[5]);
          local_40 = iVar4 * (iVar7 + iVar5 * 2) + extraout_r1_02 * 2;
        }
        else {
          FUN_0010d398(uVar8,param_3[5]);
          iVar4 = FUN_0010d398(uVar8,param_3[5]);
          local_40 = (param_3[7] + param_3[5]) * iVar4 + extraout_r1_03;
        }
      }
      param_2[2] = uVar8 | (uint)*(ushort *)(param_3 + 5) << 0x11;
      param_2[3] = *(ushort *)(param_3 + 6) & 0x7fff |
                   (*(ushort *)(param_3 + 7) & 0xffff7fff) << 0x10;
      if (uVar9 == 0) {
        param_2[3] = param_2[3] | 0x80000000;
      }
      param_2[4] = (int)(param_2 + 5);
      param_2 = param_2 + 5;
      uVar8 = uVar9;
    } while (uVar9 != 0);
    uVar3 = 0;
  }
  return uVar3;
}

// ==== FUN_0010b830 @ 0010b830, 128 bytes
// called by: FUN_0010c414
undefined4 FUN_0010b830(int param_1,int param_2)

{
  undefined1 uVar1;
  undefined4 uVar2;
  uint unaff_r4;
  undefined4 local_28;
  
  local_28 = 0x4020201;
  if (param_1 == 2) {
    unaff_r4 = DAT_b0000220;
  }
  else if (param_1 == 3) {
    unaff_r4 = DAT_b0000224;
  }
  uVar1 = *(undefined1 *)((int)&local_28 + ((unaff_r4 & 0xc000) >> 0xe));
  uVar2 = FUN_0010d398(param_2 * ((unaff_r4 & 0x1ff) + 2),((unaff_r4 & 0x3e00) >> 9) + 2);
  uVar2 = FUN_0010d398(uVar2,uVar1);
  return uVar2;
}

// ==== FUN_0010b934 @ 0010b934, 80 bytes
// called by: lcm_init, FUN_0010b9b8
void FUN_0010b934(uint param_1)

{
  undefined4 local_c;
  undefined4 local_8;
  
  for (local_c = 0; local_c < param_1; local_c = local_c + 1) {
    for (local_8 = 0; local_8 < 1000; local_8 = local_8 + 1) {
    }
  }
  return;
}

// ==== FUN_0010b984 @ 0010b984, 52 bytes
// called by: FUN_0010bbec, FUN_0010bad8, FUN_0010ba7c
void FUN_0010b984(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  for (uVar2 = 0; uVar2 < param_1; uVar2 = uVar2 + 1) {
    for (uVar1 = 0; uVar1 < 100; uVar1 = uVar1 + 2) {
    }
  }
  return;
}

// ==== FUN_0010b9b8 @ 0010b9b8, 144 bytes
// called by: lcm_init
void FUN_0010b9b8(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000080;
  DAT_b0000080 = uVar1 & 0xfffffcff;
  uVar1 = DAT_b8001008;
  DAT_b8001008 = uVar1 | 0x10;
  uVar1 = DAT_b8001004;
  DAT_b8001004 = uVar1 | 0x10;
  uVar1 = DAT_b8001000;
  DAT_b8001000 = uVar1 | 0x10;
  FUN_0010b934(&DAT_00001388);
  uVar1 = DAT_b8001008;
  DAT_b8001008 = uVar1 & 0xffffffef;
  FUN_0010b934(&DAT_00001388);
  uVar1 = DAT_b8001008;
  DAT_b8001008 = uVar1 | 0x10;
  FUN_0010b934(&DAT_00001388);
  return;
}

// ==== FUN_0010ba54 @ 0010ba54, 40 bytes
// called by: FUN_0010ba7c
void FUN_0010ba54(void)

{
  uint uVar1;
  
  uVar1 = DAT_b8001040;
  DAT_b8001040 = uVar1 | 0xe00;
  uVar1 = DAT_b8001044;
  DAT_b8001044 = uVar1 | 0xe00;
  return;
}

// ==== FUN_0010ba7c @ 0010ba7c, 92 bytes
// called by: lcm_init
void FUN_0010ba7c(void)

{
  uint uVar1;
  
  uVar1 = DAT_b0000090;
  DAT_b0000090 = uVar1 & 0xff03ffff;
  FUN_0010ba54();
  uVar1 = DAT_b8001048;
  DAT_b8001048 = uVar1 | 0x200;
  uVar1 = DAT_b8001048;
  DAT_b8001048 = uVar1 | 0x400;
  uVar1 = DAT_b8001048;
  DAT_b8001048 = uVar1 | 0x800;
  FUN_0010b984(5);
  return;
}

// ==== FUN_0010bad8 @ 0010bad8, 276 bytes
// called by: lcm_init
void FUN_0010bad8(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 & 0xfffff7ff;
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 & 0xfffffbff;
  FUN_0010b984(2);
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 & 0xfffffdff;
  FUN_0010b984(2);
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 | 0x400;
  FUN_0010b984(2);
  for (uVar2 = 0; uVar2 < 8; uVar2 = uVar2 + 1) {
    uVar1 = DAT_b8001048;
    DAT_b8001048 = uVar1 & 0xfffffbff;
    if ((param_1 & 0x80) == 0) {
      uVar1 = DAT_b8001048;
      DAT_b8001048 = uVar1 & 0xfffffdff;
    }
    else {
      uVar1 = DAT_b8001048;
      DAT_b8001048 = uVar1 | 0x200;
    }
    FUN_0010b984(3);
    uVar1 = DAT_b8001048;
    DAT_b8001048 = uVar1 | 0x400;
    param_1 = (param_1 & 0x7f) << 1;
    FUN_0010b984(2);
  }
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 | 0x800;
  FUN_0010b984(1);
  return;
}

// ==== FUN_0010bbec @ 0010bbec, 276 bytes
// called by: lcm_init
void FUN_0010bbec(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 & 0xfffff7ff;
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 & 0xfffffbff;
  FUN_0010b984(2);
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 | 0x200;
  FUN_0010b984(2);
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 | 0x400;
  FUN_0010b984(2);
  for (uVar2 = 0; uVar2 < 8; uVar2 = uVar2 + 1) {
    uVar1 = DAT_b8001048;
    DAT_b8001048 = uVar1 & 0xfffffbff;
    if ((param_1 & 0x80) == 0) {
      uVar1 = DAT_b8001048;
      DAT_b8001048 = uVar1 & 0xfffffdff;
    }
    else {
      uVar1 = DAT_b8001048;
      DAT_b8001048 = uVar1 | 0x200;
    }
    FUN_0010b984(3);
    uVar1 = DAT_b8001048;
    DAT_b8001048 = uVar1 | 0x400;
    param_1 = (param_1 & 0x7f) << 1;
    FUN_0010b984(2);
  }
  uVar2 = DAT_b8001048;
  DAT_b8001048 = uVar2 | 0x800;
  FUN_0010b984(1);
  return;
}

// ==== lcm_init @ 0010bd00, 1792 bytes
// called by: FUN_0010c414
// refs "LCM_Init test !!"; the panel init PatchPanel replaces
/* refs "LCM_Init test !!"; the panel init PatchPanel replaces */

undefined4 lcm_init(void)

{
  uint uVar1;
  undefined4 uVar2;
  
  dbg_printf("LCM_Init test !!\n");
  FUN_0010ba7c();
  uVar1 = DAT_b8001048;
  DAT_b8001048 = uVar1 & 0xfffff7ff;
  FUN_0010b9b8();
  FUN_0010bad8(0x11);
  FUN_0010b934(0x78);
  FUN_0010bad8(0xff);
  FUN_0010bbec(0x77);
  FUN_0010bbec(1);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x10);
  FUN_0010bad8(0xc0);
  FUN_0010bbec(0x3b);
  FUN_0010bbec(0);
  FUN_0010bad8(0xc1);
  FUN_0010bbec(0xb);
  FUN_0010bbec(2);
  FUN_0010bad8(0xc2);
  FUN_0010bbec(7);
  FUN_0010bbec(2);
  FUN_0010bad8(0xcc);
  FUN_0010bbec(0x10);
  FUN_0010bad8(0xcd);
  FUN_0010bbec(8);
  FUN_0010bad8(0xb0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x11);
  FUN_0010bbec(0x16);
  FUN_0010bbec(0xe);
  FUN_0010bbec(0x11);
  FUN_0010bbec(6);
  FUN_0010bbec(5);
  FUN_0010bbec(9);
  FUN_0010bbec(8);
  FUN_0010bbec(0x21);
  FUN_0010bbec(6);
  FUN_0010bbec(0x13);
  FUN_0010bbec(0x10);
  FUN_0010bbec(0x29);
  FUN_0010bbec(0x31);
  FUN_0010bbec(0x18);
  FUN_0010bad8(0xb1);
  FUN_0010bbec(0);
  FUN_0010bbec(0x11);
  FUN_0010bbec(0x16);
  FUN_0010bbec(0xe);
  FUN_0010bbec(0x11);
  FUN_0010bbec(7);
  FUN_0010bbec(5);
  FUN_0010bbec(9);
  FUN_0010bbec(9);
  FUN_0010bbec(0x21);
  FUN_0010bbec(5);
  FUN_0010bbec(0x13);
  FUN_0010bbec(0x11);
  FUN_0010bbec(0x2a);
  FUN_0010bbec(0x31);
  FUN_0010bbec(0x18);
  FUN_0010bad8(0xff);
  FUN_0010bbec(0x77);
  FUN_0010bbec(1);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x11);
  FUN_0010bad8(0xb0);
  FUN_0010bbec(0x6d);
  FUN_0010bad8(0xb1);
  FUN_0010bbec(0x37);
  FUN_0010bad8(0xb2);
  FUN_0010bbec(0x81);
  FUN_0010bad8(0xb3);
  FUN_0010bbec(0x80);
  FUN_0010bad8(0xb5);
  FUN_0010bbec(0x43);
  FUN_0010bad8(0xb7);
  FUN_0010bbec(0x85);
  FUN_0010bad8(0xb8);
  FUN_0010bbec(0x20);
  FUN_0010bad8(0xc1);
  FUN_0010bbec(0x78);
  FUN_0010bad8(0xc2);
  FUN_0010bbec(0x78);
  FUN_0010bad8(0xd0);
  FUN_0010bbec(0x88);
  FUN_0010bad8(0xe0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(2);
  FUN_0010bad8(0xe1);
  FUN_0010bbec(3);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(4);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x20);
  FUN_0010bbec(0x20);
  FUN_0010bad8(0xe2);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe3);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x11);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe4);
  FUN_0010bbec(0x22);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe5);
  FUN_0010bbec(5);
  FUN_0010bbec(0xec);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(7);
  FUN_0010bbec(0xee);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe6);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x11);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe7);
  FUN_0010bbec(0x22);
  FUN_0010bbec(0);
  FUN_0010bad8(0xe8);
  FUN_0010bbec(6);
  FUN_0010bbec(0xed);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(8);
  FUN_0010bbec(0xef);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bad8(0xeb);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x40);
  FUN_0010bbec(0x40);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bad8(0xed);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xba);
  FUN_0010bbec(10);
  FUN_0010bbec(0xbf);
  FUN_0010bbec(0x45);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0x54);
  FUN_0010bbec(0xfb);
  FUN_0010bbec(0xa0);
  FUN_0010bbec(0xab);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xff);
  FUN_0010bbec(0xff);
  FUN_0010bad8(0xef);
  FUN_0010bbec(0x10);
  FUN_0010bbec(0xd);
  FUN_0010bbec(4);
  FUN_0010bbec(8);
  FUN_0010bbec(0x3f);
  FUN_0010bbec(0x1f);
  FUN_0010bad8(0xff);
  FUN_0010bbec(0x77);
  FUN_0010bbec(1);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0x13);
  FUN_0010bad8(0xef);
  FUN_0010bbec(8);
  FUN_0010bad8(0xff);
  FUN_0010bbec(0x77);
  FUN_0010bbec(1);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bbec(0);
  FUN_0010bad8(0x11);
  FUN_0010b934(0x78);
  FUN_0010bad8(0x29);
  FUN_0010b934(0x32);
  FUN_0010bad8(0x36);
  FUN_0010bbec(0);
  FUN_0010bad8(0x3a);
  uVar2 = FUN_0010bbec(0x60);
  return uVar2;
}

