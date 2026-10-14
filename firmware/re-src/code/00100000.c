// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00100000-0x00103fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== reset @ 00100000, 4 bytes
// the image's entry (file offset 0x20)
/* WARNING: Removing unreachable block (ram,0x001000dc) */
/* the image's entry (file offset 0x20) */

void reset(void)

{
  uint uVar1;
  
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 & 0xffffdfff);
  FUN_001000c0(uVar1 & 0xffffdfff);
  FUN_001001cc();
                    /* WARNING: Could not recover jumptable at 0x001000ec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  if (((uint)DAT_001823c4 & 1) == 0) {
                    /* WARNING: Could not recover jumptable at 0x001000f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_001823c4)(DAT_001823b8,DAT_001823bc,DAT_001823c0);
    return;
  }
  (*(code *)(0x1823b7 - (int)DAT_001823c4))();
  return;
}

// ==== FUN_00100064 @ 00100064, 72 bytes
/* WARNING: Removing unreachable block (ram,0x001000dc) */

void FUN_00100064(void)

{
  uint uVar1;
  
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 & 0xffffdfff);
  FUN_001000c0(uVar1 & 0xffffdfff);
  FUN_001001cc();
                    /* WARNING: Could not recover jumptable at 0x001000ec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  if (((uint)DAT_001823c4 & 1) == 0) {
                    /* WARNING: Could not recover jumptable at 0x001000f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_001823c4)(DAT_001823b8,DAT_001823bc,DAT_001823c0);
    return;
  }
  (*(code *)(0x1823b7 - (int)DAT_001823c4))();
  return;
}

// ==== FUN_001000c0 @ 001000c0, 52 bytes
// called by: FUN_00100064
/* WARNING: Removing unreachable block (ram,0x001000dc) */

void FUN_001000c0(void)

{
                    /* WARNING: Could not recover jumptable at 0x001000ec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  if (((uint)DAT_001823c4 & 1) == 0) {
                    /* WARNING: Could not recover jumptable at 0x001000f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_001823c4)(DAT_001823b8,DAT_001823bc,DAT_001823c0);
    return;
  }
  (*(code *)(0x1823b7 - (int)DAT_001823c4))();
  return;
}

// ==== FUN_001001ac @ 001001ac, 24 bytes
// called by: FUN_0010c768
/* WARNING: Removing unreachable block (ram,0x0010c72e) */

longlong FUN_001001ac(int *param_1,uint param_2,int *param_3)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  char *pcVar5;
  undefined8 uVar6;
  int *piVar7;
  undefined4 uVar8;
  
  if (param_2 != 100) {
    return (ulonglong)param_2 << 0x20;
  }
  uVar4 = 0;
  iVar1 = *param_3;
  pcVar5 = "";
  if (iVar1 < 0) {
    iVar1 = -iVar1;
    pcVar5 = "-";
    piVar2 = param_1;
  }
  else {
    piVar2 = (int *)*param_1;
    if ((int)piVar2 << 0x1e < 0) {
      pcVar5 = "+";
    }
    else {
      piVar2 = (int *)((int)piVar2 << 0x1d);
      if (-1 < (int)piVar2) goto LAB_0010c732;
      pcVar5 = " ";
    }
  }
  uVar4 = 1;
LAB_0010c732:
  iVar3 = 0;
  uVar8 = 100;
  piVar7 = param_1;
  while (iVar1 != 0) {
    uVar6 = FUN_0010d1d0(iVar1,piVar2);
    piVar2 = (int *)((int)((ulonglong)uVar6 >> 0x20) + 0x30);
    *(char *)((int)param_1 + iVar3 + 0x24) = (char)piVar2;
    iVar3 = iVar3 + 1;
    iVar1 = (int)uVar6;
  }
  uVar4 = FUN_0010d062(piVar7,iVar3,pcVar5,uVar4,piVar7,uVar8,param_3);
  return CONCAT44(piVar7,uVar4);
}

// ==== FUN_001001bc @ 001001bc, 12 bytes
// called by: FUN_001001cc
undefined8 FUN_001001bc(undefined4 param_1,undefined4 param_2)

{
  FUN_0010cffa(param_1,param_2);
  return CONCAT44(param_2,param_1);
}

// ==== FUN_001001c8 @ 001001c8, 4 bytes
// called by: FUN_00100200
void FUN_001001c8(void)

{
  return;
}

// ==== FUN_001001cc @ 001001cc, 44 bytes
// called by: FUN_00100064, FUN_001000c0
/* WARNING: Removing unreachable block (ram,0x001001e4) */

void FUN_001001cc(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined8 uVar5;
  uint uStack_20;
  
  uVar3 = FUN_0010d298();
  FUN_001001bc(uVar3,param_3);
  FUN_0012a4f0();
  uVar5 = FUN_0010d2f8();
  FUN_001001c8();
  iVar4 = 0x100210;
  uVar5 = FUN_00126f6c((int)uVar5,(int)((ulonglong)uVar5 >> 0x20));
  if (DAT_00182bd9 == '\0') {
    uVar1 = DAT_b8000108;
    for (uStack_20 = 1; uStack_20 < 0x20; uStack_20 = uStack_20 + 1) {
      if ((1 << (uStack_20 & 0xff) & uVar1) != 0) {
        (**(code **)(&DAT_00182bdc + uStack_20 * 4))();
      }
    }
  }
  else {
    uVar1 = DAT_b800010c;
    uVar2 = DAT_b8000110;
    if ((uVar2 != 0) && (uVar1 >> 2 == uVar2)) {
      (**(code **)(&DAT_00182bdc + (uVar1 >> 2) * 4))();
    }
    DAT_b8000130 = 1;
  }
                    /* WARNING: Could not recover jumptable at 0x001002e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(iVar4 + -4))((int)uVar5,(int)((ulonglong)uVar5 >> 0x20),param_3,param_4);
  return;
}

// ==== FUN_00100200 @ 00100200, 236 bytes
// called by: FUN_0010d2f8
void FUN_00100200(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined8 uVar4;
  uint local_20;
  
  FUN_001001c8();
  iVar3 = 0x100210;
  uVar4 = FUN_00126f6c(param_1,param_2);
  if (DAT_00182bd9 == '\0') {
    uVar1 = DAT_b8000108;
    for (local_20 = 1; local_20 < 0x20; local_20 = local_20 + 1) {
      if ((1 << (local_20 & 0xff) & uVar1) != 0) {
        (**(code **)(&DAT_00182bdc + local_20 * 4))();
      }
    }
  }
  else {
    uVar1 = DAT_b800010c;
    uVar2 = DAT_b8000110;
    if ((uVar2 != 0) && (uVar1 >> 2 == uVar2)) {
      (**(code **)(&DAT_00182bdc + (uVar1 >> 2) * 4))();
    }
    DAT_b8000130 = 1;
  }
                    /* WARNING: Could not recover jumptable at 0x001002e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(iVar3 + -4))((int)uVar4,(int)((ulonglong)uVar4 >> 0x20),param_3,param_4);
  return;
}

// ==== FUN_001002ec @ 001002ec, 220 bytes
void FUN_001002ec(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  int in_lr;
  uint local_20;
  
  if (DAT_00182bd9 == '\0') {
    uVar1 = DAT_b8000108;
    for (local_20 = 1; local_20 < 0x20; local_20 = local_20 + 1) {
      if ((1 << (local_20 & 0xff) & uVar1) != 0) {
        (**(code **)(&DAT_00182c5c + local_20 * 4))();
      }
    }
  }
  else {
    uVar1 = DAT_b800010c;
    uVar2 = DAT_b8000110;
    if ((uVar2 != 0) && (uVar1 >> 2 == uVar2)) {
      (**(code **)(&DAT_00182c5c + (uVar1 >> 2) * 4))();
    }
    DAT_b8000130 = 1;
  }
                    /* WARNING: Could not recover jumptable at 0x001003c4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(in_lr + -4))(param_1,param_2,param_3,param_4);
  return;
}

// ==== FUN_001003cc @ 001003cc, 60 bytes
// called by: FUN_00100520
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_001003cc(void)

{
  int iVar1;
  
  _IRQ = 0xe59ff018;
  ram0x0000001c = 0xe59ff018;
  ram0x00000038 = 0x100210;
  ram0x0000003c = FUN_001002ec;
  iVar1 = FUN_00100ba4();
  if (iVar1 == 1) {
    FUN_00100b28(6);
  }
  return;
}

// ==== FUN_00100408 @ 00100408, 52 bytes
// called by: FUN_001079cc, FUN_00104ba8, FUN_00104c60, FUN_00102780, FUN_001075ac
undefined4 FUN_00100408(uint param_1)

{
  undefined4 uVar1;
  
  if (((int)param_1 < 0x20) && (0 < (int)param_1)) {
    DAT_b8000124 = 1 << (param_1 & 0xff);
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

// ==== FUN_0010043c @ 0010043c, 52 bytes
// called by: FUN_001097e0, FUN_0010b32c, FUN_00107350, FUN_00102990, FUN_001052cc, FUN_00104ad4, FUN_00103514, FUN_0012c560, FUN_00102558, FUN_001093c0, FUN_00104930, FUN_00105eb8
undefined4 FUN_0010043c(uint param_1)

{
  undefined4 uVar1;
  
  if (((int)param_1 < 0x20) && (0 < (int)param_1)) {
    DAT_b8000120 = 1 << (param_1 & 0xff);
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

// ==== FUN_00100520 @ 00100520, 336 bytes
// called by: FUN_001052cc, FUN_00103514, FUN_0012c560, FUN_00102558, FUN_001093c0, FUN_00102780, FUN_001097e0, FUN_0010b32c, FUN_00107350, FUN_00102990, FUN_00104ad4, FUN_00104930, FUN_00105eb8
undefined4 FUN_00100520(int param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  undefined4 unaff_r7;
  undefined4 uVar2;
  
  if (DAT_00182bd8 == '\0') {
    FUN_001003cc();
    DAT_00182bd8 = '\x01';
  }
  uVar1 = param_2 >> 0x1f;
  *(uint *)(&DAT_b8000000 + ((int)(param_2 + (uVar1 >> 0x1e)) >> 2) * 4) =
       *(uint *)(&DAT_b8000000 + ((int)(param_2 + (uVar1 >> 0x1e)) >> 2) * 4) &
       (-8 << ((param_2 + ((int)(param_2 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU) |
       (1 << ((param_2 + ((int)(param_2 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU)) - 1U) |
       param_1 << ((param_2 + ((int)(param_2 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU);
  switch(param_1) {
  case 0:
    uVar2 = *(undefined4 *)(&DAT_00182c5c + param_2 * 4);
    *(undefined4 *)(&DAT_00182c5c + param_2 * 4) = param_3;
    return uVar2;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    break;
  case 7:
    break;
  default:
    goto switchD_001005f4_default;
  }
  unaff_r7 = *(undefined4 *)(&DAT_00182bdc + param_2 * 4);
  *(undefined4 *)(&DAT_00182bdc + param_2 * 4) = param_3;
switchD_001005f4_default:
  return unaff_r7;
}

// ==== FUN_001006c4 @ 001006c4, 204 bytes
// called by: FUN_001286c8, FUN_001284b8
undefined4 FUN_001006c4(int param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  if ((param_1 < 0x20) && (0 < param_1)) {
    uVar1 = param_1 >> 0x1f;
    *(uint *)(&DAT_b8000000 + ((int)(param_1 + (uVar1 >> 0x1e)) >> 2) * 4) =
         *(uint *)(&DAT_b8000000 + ((int)(param_1 + (uVar1 >> 0x1e)) >> 2) * 4) &
         (-8 << ((param_1 + ((int)(param_1 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU) |
         (1 << ((param_1 + ((int)(param_1 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU)) - 1U) |
         param_2 << ((param_1 + ((int)(param_1 + (uVar1 >> 0x1e)) >> 2) * -4) * 8 & 0xffU);
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

// ==== FUN_00100790 @ 00100790, 160 bytes
// called by: FUN_0010b32c, FUN_00102558
undefined4 FUN_00100790(uint param_1,int param_2)

{
  undefined4 uVar1;
  
  if (((int)param_1 < 0x20) && (0 < (int)param_1)) {
    *(uint *)(&DAT_b8000000 + ((int)param_1 >> 2) * 4) =
         *(uint *)(&DAT_b8000000 +
                  ((int)(param_1 + ((uint)((int)param_1 >> 0x1f) >> 0x1e)) >> 2) * 4) &
         ((1 << ((param_1 + ((int)(param_1 + ((uint)((int)param_1 >> 0x1f) >> 0x1e)) >> 2) * -4) * 8
                & 0xff)) - 1U | -0xc1 << ((param_1 & 3) << 3)) | param_2 << (param_1 & 3) * 8 + 6;
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

// ==== FUN_00100830 @ 00100830, 140 bytes
// called by: FUN_00107350, FUN_00102990, FUN_00115160, FUN_0012c560, FUN_00102558, FUN_00106d54, FUN_00105eb8
undefined4 FUN_00100830(void)

{
  return 0;
}

// ==== FUN_0010090c @ 0010090c, 408 bytes
int FUN_0010090c(void)

{
  uint uVar1;
  int local_c;
  
  local_c = 0;
  uVar1 = DAT_b0003010;
  switch(uVar1 & 7) {
  case 0:
    break;
  case 1:
    local_c = 2;
    break;
  case 2:
    local_c = 4;
    break;
  case 3:
    local_c = 8;
    break;
  case 4:
    local_c = 0x10;
    break;
  case 5:
    local_c = 0x20;
    break;
  case 6:
    local_c = 0x40;
  }
  uVar1 = DAT_b0003014;
  switch(uVar1 & 7) {
  case 0:
    break;
  case 1:
    local_c = local_c + 2;
    break;
  case 2:
    local_c = local_c + 4;
    break;
  case 3:
    local_c = local_c + 8;
    break;
  case 4:
    local_c = local_c + 0x10;
    break;
  case 5:
    local_c = local_c + 0x20;
    break;
  case 6:
    local_c = local_c + 0x40;
  }
  if (local_c == 0) {
    local_c = 1;
  }
  return local_c;
}

// ==== FUN_00100aa4 @ 00100aa4, 44 bytes
// called by: FUN_0012c4d4
undefined4 FUN_00100aa4(undefined4 param_1)

{
  FUN_001096e0(param_1);
  DAT_00182cdc = 1;
  DAT_00182ce0 = param_1;
  return 0;
}

// ==== FUN_00100ad0 @ 00100ad0, 88 bytes
void FUN_00100ad0(void)

{
  uint uVar1;
  
  FUN_0010d3c8();
  coproc_moveto_Invalidate_Entire_Instruction(0);
  coproc_moveto_Invalidate_Entire_Data_cache(0);
  coproc_moveto_Data_Synchronization(0);
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 & 0xfffffffe);
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 & 0xffffefff);
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 & 0xfffffffb);
  DAT_00182cdc = 0;
  DAT_00182ce0 = 0xffffffff;
  return;
}

// ==== FUN_00100b28 @ 00100b28, 112 bytes
// called by: FUN_00115160, FUN_001003cc
int FUN_00100b28(int param_1)

{
  if (param_1 == 6) {
    coproc_moveto_Invalidate_Entire_Instruction(0);
  }
  else if (param_1 == 7) {
    param_1 = FUN_0010d3c8(7);
    coproc_moveto_Data_Synchronization(0);
  }
  else if (param_1 == 8) {
    param_1 = FUN_0010d3c8(8);
    coproc_moveto_Invalidate_Entire_Instruction(0);
    coproc_moveto_Data_Synchronization(0);
  }
  return param_1;
}

// ==== FUN_00100b98 @ 00100b98, 12 bytes
// called by: FUN_00115160
undefined4 FUN_00100b98(void)

{
  undefined4 in_cr7;
  
  coprocessor_moveto(0xf,0,0,0,in_cr7,in_cr7);
  return 0;
}

// ==== FUN_00100ba4 @ 00100ba4, 12 bytes
// called by: FUN_00115160, FUN_001003cc
undefined1 FUN_00100ba4(void)

{
  return DAT_00182cdc;
}

// ==== FUN_00100bbc @ 00100bbc, 136 bytes
undefined4 FUN_00100bbc(int param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 in_cr0;
  undefined4 in_cr7;
  undefined4 in_cr9;
  undefined4 in_cr13;
  
  uVar1 = coprocessor_movefromRt(0xf,0,1,in_cr9,in_cr0);
  coprocessor_moveto(0xf,0,1,uVar1 | 7,in_cr9,in_cr0);
  uVar1 = param_2 >> 0x1f;
  if (param_2 + ((int)(param_2 + (uVar1 >> 0x1c)) >> 4) * -0x10 == 0) {
    iVar2 = (int)(param_2 + (uVar1 >> 0x1c)) >> 4;
  }
  else {
    iVar2 = ((int)(param_2 + (uVar1 >> 0x1c)) >> 4) + 1;
  }
  for (iVar3 = 0; iVar3 < iVar2; iVar3 = iVar3 + 1) {
    coprocessor_moveto(0xf,0,1,param_1,in_cr7,in_cr13);
    param_1 = param_1 + 0x10;
  }
  uVar1 = coprocessor_movefromRt(0xf,0,1,in_cr9,in_cr0);
  coprocessor_moveto(0xf,0,1,uVar1 & 0xfffffff8 | 8,in_cr9,in_cr0);
  return 0;
}

// ==== FUN_00100d90 @ 00100d90, 112 bytes
// called by: FUN_00100e00
void FUN_00100d90(void)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b8000114;
  DAT_b8000124 = 0xffffffff;
  FUN_0010c9b8(&DAT_007c4000,0xff000000,0x2000);
  FUN_0010c9b8(0xff000000,&LAB_00100c68,0x2000);
  (*(code *)&SUB_ff000000)();
  FUN_0010c9b8(0xff000000,&DAT_007c4000,0x2000);
  DAT_b8000124 = 0xffffffff;
  DAT_b8000120 = uVar1;
  return;
}

// ==== FUN_00100e00 @ 00100e00, 16 bytes
undefined4 FUN_00100e00(void)

{
  FUN_00100d90();
  return 0;
}

// ==== FUN_00100e10 @ 00100e10, 148 bytes
// called by: FUN_0012c6d0, FUN_00101ea8, FUN_001052cc, FUN_00101a48, FUN_00104770, FUN_00101fec, sys_set_clock_divider
undefined4 FUN_00100e10(int param_1,int param_2)

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
  if ((unaff_r4 & 0x10000) == 0) {
    uVar1 = *(undefined1 *)((int)&local_28 + ((unaff_r4 & 0xc000) >> 0xe));
    uVar2 = FUN_0010d398(param_2 * ((unaff_r4 & 0x1ff) + 2),((unaff_r4 & 0x3e00) >> 9) + 2);
    uVar2 = FUN_0010d398(uVar2,uVar1);
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00100eb0 @ 00100eb0, 340 bytes
// called by: FUN_00101524, FUN_00101a48
/* WARNING: Removing unreachable block (ram,0x00100ffc) */

uint FUN_00100eb0(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined4 local_38 [4];
  undefined4 local_28;
  
  local_38[0] = DAT_00180228;
  local_38[1] = DAT_0018022c;
  local_38[2] = DAT_00180230;
  local_38[3] = DAT_00180234;
  do {
    for (uVar6 = 0; uVar6 < 4; uVar6 = uVar6 + 1) {
      for (uVar7 = 0; uVar7 < 0x20; uVar7 = uVar7 + 1) {
        for (uVar8 = 0; uVar8 < 0x200; uVar8 = uVar8 + 1) {
          iVar5 = (uVar7 + 2) * 2;
          iVar1 = (uVar8 + 2) * 2;
          local_28 = local_38[uVar6];
          uVar2 = FUN_0010d398(param_1,iVar5);
          if ((((999 < uVar2) && (uVar2 = FUN_0010d398(param_1,iVar5), uVar2 < 0x3a99)) &&
              (iVar3 = FUN_0010d398(param_1,iVar5), 99999 < (uint)(iVar1 * iVar3))) &&
             (iVar3 = FUN_0010d398(param_1,iVar5), (uint)(iVar1 * iVar3) < 0x7a121)) {
            uVar4 = FUN_0010d398(param_1 * iVar1,iVar5);
            iVar5 = FUN_0010d398(uVar4,local_28);
            if (param_2 == iVar5) {
              return uVar6 << 0xe | uVar7 << 9 | uVar8;
            }
          }
        }
      }
    }
    param_2 = param_2 + -4000;
  } while( true );
}

// ==== FUN_001010a0 @ 001010a0, 884 bytes
void FUN_001010a0(int param_1,uint param_2,uint param_3,uint param_4,uint param_5,int param_6)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  
  uVar2 = DAT_b8000114;
  DAT_b8000124 = 0xfffffffe;
  uVar1 = DAT_b0000004;
  if ((uVar1 & 0x30) == 0x20) {
    uVar1 = DAT_b000301c;
    DAT_b000301c = uVar1 | 1;
    if (param_3 < 96000) {
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 | 0x40000;
    }
    else {
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 & 0xfffbffff;
    }
  }
  iVar3 = 0;
  do {
    iVar3 = iVar3 + 1;
  } while (iVar3 != 100);
  uVar1 = DAT_b0003004;
  DAT_b0003004 = uVar1 | 0x28;
  if (param_1 == 0) {
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xfffffff8 | param_4;
    uVar1 = DAT_b0000004;
    if ((uVar1 & 0x30) != 0) {
      uVar1 = DAT_b000301c;
      DAT_b000301c = uVar1 | 1;
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 | 0x40000;
    }
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xffffff00;
  }
  else if (param_1 == 3) {
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 | 2;
    DAT_b0000224 = param_2;
    iVar3 = 0;
    do {
      iVar3 = iVar3 + 1;
    } while (iVar3 != 1000);
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xfffff0e0 | param_4 | 0x18;
    uVar1 = DAT_b000021c;
    DAT_b000021c = uVar1 & 0xfffff0f0 | param_5 | param_6 << 8;
    iVar3 = 0;
    do {
      iVar3 = iVar3 + 1;
    } while (iVar3 != 1000);
  }
  else if (param_1 == 2) {
    uVar1 = DAT_b000020c;
    if ((uVar1 & 0x18) == 0x18) {
      uVar1 = DAT_b000020c;
      DAT_b000020c = uVar1 & 0xffffff00;
      DAT_b000021c = 0x100;
      DAT_b0000220 = 0xf8801c;
      iVar3 = 0;
      do {
        iVar3 = iVar3 + 1;
      } while (iVar3 != 500);
      iVar3 = 5;
      do {
        DAT_b0000220 = param_2 | 0xf80000;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      iVar3 = 0;
      do {
        iVar3 = iVar3 + 1;
      } while (iVar3 != 500);
      iVar3 = 0;
      do {
        iVar3 = iVar3 + 1;
      } while (iVar3 != 1000);
    }
    else {
      uVar1 = DAT_b000020c;
      DAT_b000020c = uVar1 & 0xffffff00;
      DAT_b0000220 = param_2 | 0xf80000;
      iVar3 = 0;
      do {
        iVar3 = iVar3 + 1;
      } while (iVar3 != 1000);
    }
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xfffff0e0 | param_4 | 0x10;
    uVar1 = DAT_b000021c;
    DAT_b000021c = uVar1 & 0xfffff0f0 | param_5 | param_6 << 8;
  }
  iVar3 = 0;
  do {
    iVar3 = iVar3 + 1;
  } while (iVar3 != 1000);
  uVar1 = DAT_b0003004;
  DAT_b0003004 = uVar1 & 0xfffffff7;
  iVar3 = 0;
  do {
    iVar3 = iVar3 + 1;
  } while (iVar3 != 1000);
  DAT_b8000120 = uVar2;
  return;
}

// ==== FUN_00101414 @ 00101414, 180 bytes
// called by: FUN_00101524
void FUN_00101414(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  
  uVar1 = DAT_b8000114;
  DAT_b8000124 = 0xffffffff;
  FUN_0010c9b8(0x807c4000,0xff000000,0x2000);
  FUN_0010c9b8(0xff000000,0x801000a0,0x2000);
  (*(code *)&SUB_ff001000)(param_1,param_2,param_3,param_4,param_5,param_6);
  FUN_0010c9b8(0xff000000,0x807c4000,0x2000);
  DAT_b8000124 = 0xffffffff;
  DAT_b8000120 = uVar1;
  return;
}

// ==== FUN_001014c8 @ 001014c8, 92 bytes
// called by: FUN_001286c8, FUN_0012c6d0, dbg_printf, FUN_00101ea8, FUN_001052cc, FUN_00101524, FUN_00101a48, FUN_00104770, FUN_00103ecc, FUN_00101fec, sys_set_clock_divider, FUN_0012c4d4
undefined * FUN_001014c8(void)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = FUN_00103d00();
  if (iVar2 == 0x47) {
    DAT_00182d18 = &DAT_00002ee0;
  }
  else {
    uVar1 = DAT_b0000004;
    if ((uVar1 & 0xc) == 8) {
      DAT_00182d18 = &DAT_00002ee0;
    }
    else {
      DAT_00182d18 = &DAT_00006978;
    }
  }
  return DAT_00182d18;
}

// ==== FUN_00101524 @ 00101524, 1180 bytes
// called by: FUN_0012c4d4
undefined8
FUN_00101524(int param_1,uint param_2,uint param_3,uint param_4,uint param_5,uint param_6)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  int extraout_r1;
  int extraout_r1_00;
  int extraout_r1_01;
  uint uVar4;
  undefined4 unaff_r11;
  uint uVar5;
  
  uVar5 = param_2;
  uVar3 = param_4;
  DAT_00182d18 = FUN_001014c8();
  if (param_1 != 0) {
    FUN_0010d398(param_2,param_3);
    FUN_0010d398(param_2,param_3);
    if (param_2 < param_3) {
      uVar1 = 0xb0000001;
      goto LAB_00101598;
    }
    if (param_3 < param_4) {
      uVar1 = 0xb0000001;
      goto LAB_00101598;
    }
    if (param_3 < param_5) {
      uVar1 = 0xb0000001;
      goto LAB_00101598;
    }
    if (param_5 < param_6) {
      uVar1 = 0xb0000001;
      goto LAB_00101598;
    }
    DAT_00182d14 = param_5 * 2 < param_4;
    FUN_0010d398(param_2,param_3);
    iVar2 = FUN_0010d398(param_2,param_3);
    DAT_00182d08 = iVar2 + -1 + (uint)(extraout_r1 != 0);
    DAT_00182cf0 = FUN_0010d398(param_2,DAT_00182d08 + 1);
    if (DAT_00182d14 == '\x01') {
      DAT_00182cf8 = (undefined *)FUN_0010d398(DAT_00182cf0,6);
      FUN_0010d398(DAT_00182cf0,param_4);
      iVar2 = FUN_0010d398(DAT_00182cf0,param_4);
      DAT_00182d0c = iVar2 + -1 + (uint)(extraout_r1_00 != 0);
      if (DAT_00182d0c == 0) {
        DAT_00182d0c = 1;
      }
      if (5 < (int)DAT_00182d0c) {
        DAT_00182d0c = 5;
      }
      DAT_00182cf4 = FUN_0010d398(DAT_00182cf0,DAT_00182d0c + 1);
      uVar3 = FUN_0010d398(DAT_00182cf0,param_6);
      DAT_00182d10 = divide(uVar3,6);
      if (DAT_00182d10 != 0) {
        DAT_00182d10 = DAT_00182d10 + -1;
      }
      DAT_00182cfc = FUN_0010d398(DAT_00182cf0,(DAT_00182d10 + 1) * 6);
    }
    else {
      DAT_00182cf8 = (undefined *)(DAT_00182cf0 >> 1);
      FUN_0010d398(DAT_00182cf0,param_4);
      iVar2 = FUN_0010d398(DAT_00182cf0,param_4);
      uVar4 = iVar2 + -1 + (uint)(extraout_r1_01 != 0);
      DAT_00182d0c = uVar4;
      if (1 < uVar4) {
        uVar1 = 0xb0000001;
        goto LAB_00101598;
      }
      DAT_00182cf4 = FUN_0010d398(DAT_00182cf0,uVar4 + 1);
      if ((int)uVar4 < 3) {
        uVar4 = 2;
      }
      uVar3 = FUN_0010d398(DAT_00182cf0,param_6);
      iVar2 = divide(uVar3,uVar4);
      DAT_00182d10 = iVar2 + -1;
      DAT_00182cfc = FUN_0010d398(DAT_00182cf0,iVar2 * uVar4);
    }
  }
  if (param_1 == 0) {
    DAT_00182ce4 = 0;
  }
  else if (param_1 == 1) {
    DAT_00182ce4 = 1;
  }
  else if (param_1 == 2) {
    DAT_00182ce4 = 2;
    DAT_00182cec = param_2;
    DAT_00182d00 = FUN_00100eb0(DAT_00182d18,param_2);
  }
  else {
    if (param_1 != 3) {
      uVar1 = 0xb0000001;
      goto LAB_00101598;
    }
    DAT_00182ce4 = 3;
    DAT_00182ce8 = param_2;
    DAT_00182d04 = FUN_00100eb0(DAT_00182d18,param_2);
  }
  if (param_1 == 0) {
    unaff_r11 = 0;
    DAT_00182cf8 = &DAT_00006978;
    DAT_00182d14 = '\0';
    DAT_00182d08 = 1;
    DAT_00182d0c = 1;
    DAT_00182d10 = 1;
  }
  if (param_1 == 3) {
    DAT_00182d1d = 0;
    unaff_r11 = DAT_00182d04;
  }
  else if (param_1 == 2) {
    unaff_r11 = DAT_00182d00;
  }
  uVar5 = DAT_00182d0c;
  FUN_00101414(param_1,unaff_r11,DAT_00182cf8,DAT_00182d08,DAT_00182d0c,DAT_00182d10,uVar3);
  uVar1 = 0;
LAB_00101598:
  return CONCAT44(uVar5,uVar1);
}

// ==== FUN_001019c0 @ 001019c0, 136 bytes
// called by: FUN_00107b98, FUN_00105eb8
void FUN_001019c0(undefined1 *param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4,
                 undefined4 *param_5,undefined4 *param_6)

{
  *param_1 = (undefined1)DAT_00182ce4;
  if (DAT_00182ce4 == 3) {
    *param_2 = DAT_00182ce8;
  }
  else if (DAT_00182ce4 == 2) {
    *param_2 = DAT_00182cec;
  }
  *param_3 = DAT_00182cf0;
  *param_4 = DAT_00182cf4;
  *param_5 = DAT_00182cf8;
  *param_6 = DAT_00182cfc;
  return;
}

// ==== FUN_00101a48 @ 00101a48, 224 bytes
undefined4 FUN_00101a48(int param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  uVar2 = FUN_001014c8();
  uVar1 = DAT_b000020c;
  if (((uVar1 & 0x18) == 0x18) && (param_1 == 3)) {
    uVar2 = FUN_00100e10(3,uVar2);
  }
  else {
    uVar1 = DAT_b000020c;
    if (((uVar1 & 0x18) == 0x10) && (param_1 == 2)) {
      uVar2 = FUN_00100e10(2,uVar2);
    }
    else {
      uVar3 = FUN_00100eb0(uVar2,param_2);
      if (param_1 == 2) {
        DAT_b0000220 = uVar3;
      }
      else if (param_1 == 3) {
        DAT_b0000224 = uVar3;
      }
      if ((param_1 == 2) || (param_1 == 3)) {
        uVar2 = FUN_00100e10(param_1,uVar2);
      }
      else {
        uVar2 = 0;
      }
    }
  }
  return uVar2;
}

// ==== FUN_00101b28 @ 00101b28, 400 bytes
undefined * FUN_00101b28(undefined *param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined *puVar3;
  int iVar4;
  bool bVar5;
  
  iVar4 = 100;
  uVar2 = DAT_b8000114;
  DAT_b8000124 = 0xfffffffe;
  uVar1 = DAT_b0000004;
  if ((uVar1 & 0x30) == 0x20) {
    uVar1 = DAT_b000301c;
    DAT_b000301c = uVar1 | 1;
    puVar3 = &DAT_00017700;
    if (param_1 < (undefined *)0x17700) {
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 | 0x40000;
    }
    else {
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 & 0xfffbffff;
    }
  }
  else {
    puVar3 = (undefined *)0xfa00;
  }
  if (param_1 < puVar3) {
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xfffffff8 | param_2;
    uVar1 = DAT_b0000004;
    if ((uVar1 & 0x30) != 0) {
      uVar1 = DAT_b000301c;
      DAT_b000301c = uVar1 | 1;
      uVar1 = DAT_b0003000;
      DAT_b0003000 = uVar1 | 0x40000;
    }
  }
  else {
    uVar1 = DAT_b000020c;
    DAT_b000020c = uVar1 & 0xfffffff8 | param_2;
    uVar1 = DAT_b0003000;
    DAT_b0003000 = uVar1 & 0xfffbffff;
    do {
      bVar5 = iVar4 != 0;
      iVar4 = iVar4 + -1;
    } while (bVar5);
    uVar1 = DAT_b0000004;
    if ((uVar1 & 0x30) == 0x20) {
      uVar1 = DAT_b000301c;
      DAT_b000301c = uVar1 | 1;
    }
    else {
      uVar1 = DAT_b0000004;
      if ((uVar1 & 0x30) != 0) {
        uVar1 = DAT_b000301c;
        DAT_b000301c = uVar1 & 0xfffffffe;
      }
    }
  }
  DAT_b8000120 = uVar2;
  iVar4 = 0x1000;
  do {
    bVar5 = iVar4 != 0;
    iVar4 = iVar4 + -1;
  } while (bVar5);
  return puVar3;
}

// ==== sys_set_clock_divider @ 00101cb8, 300 bytes
// prints "system divider = %d", "Jump to SRAM"
/* prints "system divider = %d", "Jump to SRAM" */

undefined4 sys_set_clock_divider(uint param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  
  if (param_1 < 8) {
    uVar3 = DAT_b000020c;
    if ((uVar3 & 0x18) == 0) {
      uVar2 = FUN_001014c8();
    }
    else {
      uVar2 = FUN_001014c8();
      uVar3 = DAT_b000020c;
      if ((uVar3 & 0x18) == 0x18) {
        uVar2 = FUN_00100e10(3,uVar2);
      }
      else {
        uVar2 = FUN_00100e10(2,uVar2);
      }
    }
    uVar1 = DAT_b8000114;
    DAT_b8000124 = 0xffffffff;
    FUN_0010c9b8(0x807c4000,0xff000000,0x2000);
    FUN_0010c9b8(0xff000000,0x80100b28,0x2000);
    dbg_printf("system divider = %d\n",param_1);
    dbg_printf("Jump to SRAM\n");
    uVar3 = FUN_0010d398(uVar2,param_1 + 1);
    (*(code *)&SUB_ff001000)(uVar3 >> 1,param_1);
    FUN_0010c9b8(0xff000000,0x807c4000,0x2000);
    DAT_b8000124 = 0xffffffff;
    DAT_b8000120 = uVar1;
    uVar2 = 0;
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

// ==== FUN_00101de4 @ 00101de4, 196 bytes
undefined4 FUN_00101de4(int param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  if ((param_2 == 1) && (uVar1 = DAT_b000020c, (uVar1 & 0x18) == param_1 * 8)) {
    uVar2 = 0xffffffff;
  }
  else {
    if (param_1 == 2) {
      if (param_2 == 1) {
        uVar1 = DAT_b0000220;
        DAT_b0000220 = uVar1 | 0x10000;
      }
      else {
        uVar1 = DAT_b0000220;
        DAT_b0000220 = uVar1 & 0xfffeffff;
      }
    }
    else if (param_1 == 3) {
      if (param_2 == 1) {
        uVar1 = DAT_b0000224;
        DAT_b0000224 = uVar1 | 0x10000;
      }
      else {
        uVar1 = DAT_b0000224;
        DAT_b0000224 = uVar1 & 0xfffeffff;
      }
    }
    if (param_2 != 1) {
      FUN_00102f94(1);
    }
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00101ea8 @ 00101ea8, 208 bytes
// called by: FUN_00101fec, FUN_00102150, FUN_00102104
void FUN_00101ea8(void)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 unaff_r5;
  
  uVar2 = FUN_001014c8();
  uVar1 = DAT_b000020c;
  if ((uVar1 & 0x18) == 0x18) {
    uVar2 = FUN_00100e10(3,uVar2);
    uVar1 = DAT_b000020c;
    unaff_r5 = FUN_0010d398(uVar2,(uVar1 & 7) + 1);
  }
  else {
    uVar1 = DAT_b000020c;
    if ((uVar1 & 0x18) == 0x10) {
      uVar2 = FUN_00100e10(2,uVar2);
      uVar1 = DAT_b000020c;
      unaff_r5 = FUN_0010d398(uVar2,(uVar1 & 7) + 1);
    }
    else {
      uVar1 = DAT_b000020c;
      if ((uVar1 & 0x18) == 0) {
        unaff_r5 = FUN_001014c8();
      }
    }
  }
  uVar1 = DAT_b000021c;
  FUN_0010d398(unaff_r5,(uVar1 & 0xf) + 1);
  return;
}

// ==== FUN_00101fec @ 00101fec, 280 bytes
void FUN_00101fec(undefined4 param_1)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 unaff_r5;
  
  uVar2 = FUN_001014c8();
  uVar1 = DAT_b000020c;
  if ((uVar1 & 0x18) == 0x18) {
    uVar2 = FUN_00100e10(3,uVar2);
    uVar1 = DAT_b000020c;
    unaff_r5 = FUN_0010d398(uVar2,(uVar1 & 7) + 1);
  }
  else {
    uVar1 = DAT_b000020c;
    if ((uVar1 & 0x18) == 0x10) {
      uVar2 = FUN_00100e10(2,uVar2);
      uVar1 = DAT_b000020c;
      unaff_r5 = FUN_0010d398(uVar2,(uVar1 & 7) + 1);
    }
    else {
      uVar1 = DAT_b000020c;
      if ((uVar1 & 0x18) == 0) {
        unaff_r5 = FUN_001014c8();
      }
    }
  }
  iVar3 = FUN_0010d398(unaff_r5,param_1);
  if (iVar3 == 1) {
    uVar1 = DAT_b000021c;
    DAT_b000021c = uVar1 & 0xfffffff0;
  }
  else {
    iVar3 = FUN_0010d398(unaff_r5,param_1);
    if (iVar3 == 2) {
      uVar1 = DAT_b000021c;
      DAT_b000021c = uVar1 & 0xfffffff0 | 1;
    }
  }
  FUN_00101ea8();
  return;
}

// ==== FUN_00102104 @ 00102104, 76 bytes
// called by: FUN_00102150, FUN_0012acf8
void FUN_00102104(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = FUN_00101ea8();
  uVar1 = DAT_b000021c;
  if ((uVar1 & 0xf) == 0) {
    uVar2 = uVar2 >> 1;
  }
  uVar1 = DAT_b000021c;
  FUN_0010d398(uVar2,((uVar1 & 0xf00) >> 8) + 1);
  return;
}

// ==== FUN_00102150 @ 00102150, 196 bytes
undefined4 FUN_00102150(uint param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int extraout_r1;
  
  if (param_1 < 0xea61) {
    uVar3 = FUN_00101ea8();
    uVar1 = DAT_b000021c;
    if ((uVar1 & 0xf) == 0) {
      uVar3 = uVar3 >> 1;
    }
    FUN_0010d398(uVar3,param_1);
    if (extraout_r1 == 0) {
      iVar4 = FUN_0010d398(uVar3,param_1);
      if (iVar4 == 0) {
        uVar3 = 0;
      }
      else {
        iVar4 = FUN_0010d398(uVar3,param_1);
        uVar3 = iVar4 - 1;
      }
    }
    else {
      uVar3 = FUN_0010d398(uVar3,param_1);
    }
    if (0xf < uVar3) {
      uVar3 = 0xf;
    }
    uVar1 = DAT_b000021c;
    DAT_b000021c = uVar1 & 0xfffff0ff | uVar3 << 8;
    uVar2 = FUN_00102104();
  }
  else {
    uVar2 = 0xb0000001;
  }
  return uVar2;
}

// ==== FUN_00102218 @ 00102218, 520 bytes
void FUN_00102218(void)

{
  uint uVar1;
  int local_8;
  
  uVar1 = DAT_b8002018;
  if ((uVar1 & 1) != 0) {
    DAT_00182d28 = DAT_00182d28 + 1;
    DAT_b8002018 = 1;
    if (0xffffffe < DAT_00182d28) {
      DAT_00182d28 = 0;
    }
    if (DAT_00182d34 != '\0') {
      for (local_8 = 0; local_8 < 10; local_8 = local_8 + 1) {
        if ((*(int *)(&DAT_007bcbe4 + local_8 * 0x10) != 0) &&
           (*(int *)(&DAT_007bcbec + local_8 * 0x10) = *(int *)(&DAT_007bcbec + local_8 * 0x10) + -1
           , *(int *)(&DAT_007bcbec + local_8 * 0x10) == 0)) {
          (**(code **)(&DAT_007bcbf0 + local_8 * 0x10))();
          *(undefined4 *)(&DAT_007bcbec + local_8 * 0x10) =
               *(undefined4 *)(&DAT_007bcbe8 + local_8 * 0x10);
        }
      }
    }
  }
  uVar1 = DAT_b8002018;
  if ((uVar1 & 2) != 0) {
    DAT_00182d40 = DAT_00182d40 + 1;
    DAT_b8002018 = 2;
    if (0xffffffe < DAT_00182d28) {
      DAT_00182d40 = 0;
    }
    if (DAT_00182d4c != '\0') {
      for (local_8 = 0; local_8 < 10; local_8 = local_8 + 1) {
        if ((*(int *)(&DAT_007bcc84 + local_8 * 0x10) != 0) &&
           (*(int *)(&DAT_007bcc8c + local_8 * 0x10) = *(int *)(&DAT_007bcc8c + local_8 * 0x10) + -1
           , *(int *)(&DAT_007bcc8c + local_8 * 0x10) == 0)) {
          (**(code **)(&DAT_007bcc90 + local_8 * 0x10))();
          *(undefined4 *)(&DAT_007bcc8c + local_8 * 0x10) =
               *(undefined4 *)(&DAT_007bcc88 + local_8 * 0x10);
        }
      }
    }
  }
  return;
}

// ==== FUN_00102420 @ 00102420, 76 bytes
// called by: FUN_00102f94, FUN_001052cc
undefined4 FUN_00102420(int param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_00182d28;
  if ((param_1 != 0) && (uVar1 = DAT_00182d40, param_1 != 1)) {
    uVar1 = 0;
  }
  return uVar1;
}

// ==== FUN_0010250c @ 0010250c, 76 bytes
// called by: FUN_001286c8
undefined4 FUN_0010250c(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  uVar1 = param_2;
  if ((param_1 != 0) && (uVar1 = DAT_00182d24, param_1 == 1)) {
    DAT_00182d3c = param_2;
  }
  DAT_00182d24 = uVar1;
  return 0;
}

// ==== FUN_00102558 @ 00102558, 552 bytes
// called by: FUN_00102f94, FUN_001286c8
undefined4 FUN_00102558(int param_1,undefined4 param_2,uint param_3)

{
  uint uVar1;
  undefined4 uVar2;
  int local_18;
  
  if (param_1 == 0) {
    DAT_00182d35 = 1;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 | 0x100;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 | 0x10;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 & 0xffffffef;
    DAT_00182d30 = param_2;
    for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
      *(undefined4 *)(&DAT_007bcbe4 + local_18 * 0x10) = 0;
    }
    DAT_00182d20 = FUN_00100520(1,0xe,FUN_00102218);
    FUN_0010043c(0xe);
    FUN_00100790(0xe,1);
    DAT_00182d28 = 0;
    uVar2 = FUN_0010d398(DAT_00182d24,param_2);
    DAT_b8002008 = uVar2;
    uVar1 = DAT_b8002000;
    DAT_b8002000 = uVar1 & 0x87ffff00 | (param_3 | 0xc) << 0x1b;
  }
  else if (param_1 == 1) {
    DAT_00182d4d = 1;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 | 0x200;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 | 0x20;
    uVar1 = DAT_b0000018;
    DAT_b0000018 = uVar1 & 0xffffffdf;
    DAT_00182d48 = param_2;
    for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
      *(undefined4 *)(&DAT_007bcc84 + local_18 * 0x10) = 0;
    }
    DAT_00182d38 = FUN_00100520(1,0xf,FUN_00102218);
    FUN_0010043c(0xf);
    FUN_00100790(0xf,1);
    DAT_00182d40 = 0;
    uVar2 = FUN_0010d398(DAT_00182d3c,param_2);
    DAT_b800200c = uVar2;
    uVar1 = DAT_b8002004;
    DAT_b8002004 = uVar1 & 0x87ffff00 | (param_3 | 0xc) << 0x1b;
  }
  FUN_00100830(0x7f);
  return 0;
}

// ==== FUN_00102780 @ 00102780, 264 bytes
undefined4 FUN_00102780(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    DAT_00182d35 = 0;
    FUN_00100520(1,0xe,DAT_00182d20);
    FUN_00100408(0xe);
    DAT_b8002000 = 0;
    DAT_b8002018 = 1;
    DAT_00182d2c = 0;
    DAT_00182d34 = 0;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 & 0xfffffeff;
  }
  else if (param_1 == 1) {
    DAT_00182d4d = 0;
    FUN_00100520(1,0xf,DAT_00182d38);
    FUN_00100408(0xf);
    DAT_b8002004 = 0;
    DAT_b8002018 = 2;
    DAT_00182d44 = 0;
    DAT_00182d4c = 0;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 & 0xfffffdff;
  }
  return 0;
}

// ==== FUN_00102888 @ 00102888, 44 bytes
// called by: FUN_0012e770, FUN_0012a4f0, FUN_0012ea00
void FUN_00102888(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 | 1;
  return;
}

// ==== FUN_001028b4 @ 001028b4, 44 bytes
// called by: FUN_00119b70, FUN_0012ea00
void FUN_001028b4(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 | 8;
  return;
}

// ==== FUN_001028e0 @ 001028e0, 44 bytes
// called by: FUN_0012a4f0
void FUN_001028e0(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 & 0xffffff7f;
  return;
}

// ==== FUN_0010290c @ 0010290c, 44 bytes
void FUN_0010290c(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 & 0xfffffffd;
  return;
}

// ==== FUN_00102938 @ 00102938, 44 bytes
// called by: FUN_0012a964, lcd_boot_proc, FUN_0012a4f0
void FUN_00102938(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 | 0x80;
  return;
}

// ==== FUN_00102964 @ 00102964, 44 bytes
// called by: FUN_0012a964, lcd_boot_proc, FUN_0012a4f0
void FUN_00102964(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 | 2;
  return;
}

// ==== FUN_00102990 @ 00102990, 92 bytes
// called by: FUN_0012a964, lcd_boot_proc, FUN_0012a4f0
undefined4 FUN_00102990(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 | 0x40;
  uVar2 = FUN_00100520(param_1,1,param_2);
  FUN_0010043c(1);
  FUN_00100830(0x7f);
  return uVar2;
}

// ==== FUN_001029ec @ 001029ec, 60 bytes
// called by: FUN_0012a964, lcd_boot_proc, FUN_0012a4f0
undefined4 FUN_001029ec(int param_1)

{
  uint uVar1;
  
  uVar1 = DAT_b800201c;
  DAT_b800201c = uVar1 & 0xffffffcf | 0x400 | param_1 << 4;
  return 0;
}

// ==== FUN_00102a28 @ 00102a28, 444 bytes
// called by: FUN_001286c8
int FUN_00102a28(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int local_10;
  
  iVar1 = 0;
  if (param_1 == 0) {
    DAT_00182d34 = 1;
    DAT_00182d2c = DAT_00182d2c + 1;
    for (local_10 = 0; local_10 < 10; local_10 = local_10 + 1) {
      if (*(int *)(&DAT_007bcbe4 + local_10 * 0x10) == 0) {
        *(undefined4 *)(&DAT_007bcbe4 + local_10 * 0x10) = 1;
        *(undefined4 *)(&DAT_007bcbe8 + local_10 * 0x10) = param_2;
        *(undefined4 *)(&DAT_007bcbec + local_10 * 0x10) = param_2;
        *(undefined4 *)(&DAT_007bcbf0 + local_10 * 0x10) = param_3;
        iVar1 = local_10 + 1;
        break;
      }
    }
    if (local_10 == 10) {
      iVar1 = 0;
    }
  }
  else if (param_1 == 1) {
    DAT_00182d4c = 1;
    DAT_00182d44 = DAT_00182d44 + 1;
    for (local_10 = 0; local_10 < 10; local_10 = local_10 + 1) {
      if (*(int *)(&DAT_007bcc84 + local_10 * 0x10) == 0) {
        *(undefined4 *)(&DAT_007bcc84 + local_10 * 0x10) = 1;
        *(undefined4 *)(&DAT_007bcc88 + local_10 * 0x10) = param_2;
        *(undefined4 *)(&DAT_007bcc8c + local_10 * 0x10) = param_2;
        *(undefined4 *)(&DAT_007bcc90 + local_10 * 0x10) = param_3;
        iVar1 = local_10 + 1;
        break;
      }
    }
    if (local_10 == 10) {
      iVar1 = 0;
    }
  }
  return iVar1;
}

// ==== FUN_00102c98 @ 00102c98, 304 bytes
// called by: FUN_00102f14
int FUN_00102c98(int param_1,uint param_2,uint param_3,uint param_4,uint param_5,uint param_6)

{
  int iVar1;
  uint uVar2;
  
  if ((((param_2 == 0) || (0xc < param_2)) || (param_3 == 0)) ||
     (((0x1f < param_3 || (0x17 < param_4)) || ((0x3b < param_5 || (0x3b < param_6)))))) {
    iVar1 = 1;
  }
  else {
    iVar1 = (param_1 + -0x7b2) * 0x1e13380 + (param_1 - 0x7b1U >> 2) * 0x15180;
    if ((param_1 - 0x7b0U & 3) == 0) {
      DAT_00182d5c = &DAT_00263b80;
    }
    else {
      DAT_00182d5c = &DAT_0024ea00;
    }
    for (uVar2 = 0; uVar2 < param_2 - 1; uVar2 = uVar2 + 1) {
      iVar1 = iVar1 + (&DAT_00182d58)[uVar2];
    }
    iVar1 = iVar1 + (param_3 - 1) * 0x15180 + param_4 * 0xe10 + param_5 * 0x3c + param_6;
  }
  return iVar1;
}

// ==== FUN_00102dc8 @ 00102dc8, 328 bytes
// called by: FUN_00102f50
void FUN_00102dc8(uint param_1,int *param_2)

{
  undefined4 extraout_r1;
  undefined4 extraout_r1_00;
  int extraout_r1_01;
  int iVar1;
  uint uVar2;
  
  iVar1 = 0x7b2;
  while( true ) {
    if (iVar1 + ((int)(iVar1 + ((uint)(iVar1 >> 0x1f) >> 0x1e)) >> 2) * -4 == 0) {
      uVar2 = 0x1e28500;
    }
    else {
      uVar2 = 0x1e13380;
    }
    if (param_1 < uVar2) break;
    param_1 = param_1 - uVar2;
    iVar1 = iVar1 + 1;
  }
  *param_2 = iVar1;
  if (iVar1 + ((int)(iVar1 + ((uint)(iVar1 >> 0x1f) >> 0x1e)) >> 2) * -4 == 0) {
    DAT_00182d5c = &DAT_00263b80;
  }
  else {
    DAT_00182d5c = &DAT_0024ea00;
  }
  param_2[1] = 1;
  for (iVar1 = 0; (iVar1 < 0xb && ((uint)(&DAT_00182d58)[iVar1] <= param_1)); iVar1 = iVar1 + 1) {
    param_1 = param_1 - (&DAT_00182d58)[iVar1];
    param_2[1] = param_2[1] + 1;
  }
  iVar1 = FUN_0010d398(param_1,&DAT_00015180);
  param_2[2] = iVar1 + 1;
  FUN_0010d398(param_1,&DAT_00015180);
  iVar1 = FUN_0010d398(extraout_r1,0xe10);
  param_2[3] = iVar1;
  FUN_0010d398(extraout_r1,0xe10);
  iVar1 = FUN_0010d398(extraout_r1_00,0x3c);
  param_2[4] = iVar1;
  FUN_0010d398(extraout_r1_00,0x3c);
  param_2[5] = extraout_r1_01;
  return;
}

// ==== FUN_00102f14 @ 00102f14, 60 bytes
void FUN_00102f14(void)

{
  undefined4 in_stack_00000000;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  undefined4 in_stack_0000000c;
  undefined4 in_stack_00000010;
  undefined4 in_stack_00000014;
  
  DAT_00182d50 = DAT_00182d28;
  DAT_00182d54 = FUN_00102c98(in_stack_00000000,in_stack_00000004,in_stack_00000008,
                              in_stack_0000000c,in_stack_00000010,in_stack_00000014);
  return;
}

// ==== FUN_00102f50 @ 00102f50, 68 bytes
void FUN_00102f50(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = FUN_0010d398(DAT_00182d28 - DAT_00182d50,DAT_00182d30);
  FUN_00102dc8(iVar1 + DAT_00182d54,param_1);
  return;
}

// ==== FUN_00102f94 @ 00102f94, 96 bytes
// called by: FUN_00101de4
uint FUN_00102f94(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  if (DAT_00182d35 == '\0') {
    FUN_00102558(0,100,1,param_4,param_4);
  }
  iVar1 = FUN_00102420(0);
  do {
    iVar2 = FUN_00102420(0);
  } while ((uint)(iVar2 - iVar1) <= param_1);
  return iVar2 - iVar1;
}

// ==== FUN_00103060 @ 00103060, 156 bytes
// called by: dbg_printf, FUN_00103ecc, FUN_0012c4d4
void FUN_00103060(uint param_1)

{
  uint uVar1;
  
  DAT_00182d9c = (param_1 & 1) << 8;
  if (param_1 == 0) {
    uVar1 = DAT_b0000218;
    DAT_b0000218 = uVar1 & 0xffffff00;
    uVar1 = DAT_b000008c;
    DAT_b000008c = uVar1 & 0xffffffc3 | 0x14;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 | 8;
  }
  else if (param_1 == 1) {
    uVar1 = DAT_b0000218;
    DAT_b0000218 = uVar1 & 0xffff00ff;
    uVar1 = DAT_b0000080;
    DAT_b0000080 = uVar1 | 0xf00000;
    uVar1 = DAT_b0000208;
    DAT_b0000208 = uVar1 | 0x10;
  }
  DAT_00182da0 = param_1;
  return;
}

// ==== FUN_001030fc @ 001030fc, 76 bytes
// called by: FUN_00104120
uint FUN_001030fc(uint param_1,undefined4 param_2)

{
  if (param_1 < 2) {
    if (param_1 == 0) {
      *(undefined4 *)(DAT_00182da0 * 8 + 0x7bcd24) = param_2;
    }
    else if (param_1 == 1) {
      *(undefined4 *)(DAT_00182da0 * 8 + 0x7bcd28) = param_2;
    }
  }
  return param_1;
}

// ==== FUN_00103148 @ 00103148, 768 bytes
ulonglong FUN_00103148(void)

{
  uint uVar1;
  byte bVar2;
  byte bVar3;
  uint local_10;
  
  bVar2 = *(byte *)((int)&DAT_b8008003 + DAT_00182d9c + 1);
  bVar3 = bVar2 & (&UNK_b800801c)[DAT_00182d9c];
  if ((bVar3 & 2) != 0) {
    if (DAT_00182d94 == DAT_00182d98) {
      *(byte *)((int)&DAT_b8008003 + DAT_00182d9c + 1) =
           (byte)*(undefined4 *)((int)&DAT_b8008003 + DAT_00182d9c + 1) & 0xfd;
    }
    else {
      for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
        (&DAT_b8008000)[DAT_00182d9c] = (&DAT_007bcd98)[DAT_00182d94];
        if (DAT_00182d94 == 0xff) {
          DAT_00182d94 = 0;
        }
        else {
          DAT_00182d94 = DAT_00182d94 + 1;
        }
        if (DAT_00182d94 == DAT_00182d98) break;
      }
    }
  }
  if ((bVar3 & 1) != 0) {
    uVar1 = (*(uint *)((int)&DAT_b8008017 + DAT_00182d9c + 1) & 0x3f00) >> 8;
    for (local_10 = 0; local_10 < uVar1; local_10 = local_10 + 1) {
      if (DAT_00182d90 == 100) {
        DAT_00182d90 = 0;
      }
      (&DAT_007bcd34)[DAT_00182d90] = (&DAT_b8008000)[DAT_00182d9c];
      DAT_00182d90 = DAT_00182d90 + 1;
    }
    if (*(int *)(DAT_00182da0 * 8 + 0x7bcd24) != 0) {
      (**(code **)(DAT_00182da0 * 8 + 0x7bcd24))(&DAT_007bcd34,uVar1);
    }
    DAT_00182d90 = 0;
  }
  if ((bVar3 & 0x10) != 0) {
    uVar1 = (*(uint *)((int)&DAT_b8008017 + DAT_00182d9c + 1) & 0x3f00) >> 8;
    for (local_10 = 0; local_10 < uVar1; local_10 = local_10 + 1) {
      if (DAT_00182d90 == 100) {
        DAT_00182d90 = 0;
      }
      (&DAT_007bcd34)[DAT_00182d90] = (&DAT_b8008000)[DAT_00182d9c];
      DAT_00182d90 = DAT_00182d90 + 1;
    }
    if (*(int *)(DAT_00182da0 * 8 + 0x7bcd28) != 0) {
      (**(code **)(DAT_00182da0 * 8 + 0x7bcd28))(&DAT_007bcd34,uVar1);
    }
    DAT_00182d90 = 0;
  }
  return (ulonglong)CONCAT14(bVar2,(uint)bVar3);
}

// ==== FUN_00103448 @ 00103448, 56 bytes
// called by: FUN_00103514
void FUN_00103448(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = FUN_0010d398(DAT_00182d8c,param_1);
  *(uint *)((int)&DAT_b8008023 + DAT_00182d9c + 1) = iVar1 - 2U | 0x30000000;
  return;
}

// ==== FUN_00103480 @ 00103480, 148 bytes
// called by: FUN_00104158
void FUN_00103480(int param_1)

{
  if (param_1 == 1) {
    *(uint *)((int)&DAT_b8008003 + DAT_00182d9c + 1) =
         *(uint *)((int)&DAT_b8008003 + DAT_00182d9c + 1) | 1;
  }
  else if (param_1 == 2) {
    *(uint *)((int)&DAT_b8008003 + DAT_00182d9c + 1) =
         *(uint *)((int)&DAT_b8008003 + DAT_00182d9c + 1) | 0x810;
  }
  else if (param_1 == 0xff) {
    *(undefined4 *)((int)&DAT_b8008003 + DAT_00182d9c + 1) = 0;
  }
  return;
}

// ==== FUN_00103514 @ 00103514, 468 bytes
// called by: FUN_0010413c, dbg_printf, FUN_00103ecc, FUN_0012c4d4
undefined4 FUN_00103514(int param_1)

{
  undefined4 uVar1;
  
  if (((*(char *)(param_1 + 0xe) == '\0') || (*(char *)(param_1 + 0xe) == '\x18')) ||
     (*(char *)(param_1 + 0xe) == '\b')) {
    if (((*(char *)(param_1 + 0xc) == '\0') || (*(char *)(param_1 + 0xc) == '\x01')) ||
       ((*(char *)(param_1 + 0xc) == '\x02' || (*(char *)(param_1 + 0xc) == '\x03')))) {
      if ((*(char *)(param_1 + 0xd) == '\0') || (*(char *)(param_1 + 0xd) == '\x04')) {
        if (*(uint *)(param_1 + 8) < 0x4b0) {
          uVar1 = 0xfffffffc;
        }
        else {
          if (DAT_00182dc4 == '\0') {
            *(undefined4 *)((int)&DAT_b8008007 + DAT_00182d9c + 1) = 7;
            DAT_00182dc4 = '\x01';
          }
          DAT_00182d8c = *(undefined4 *)(param_1 + 4);
          FUN_00103448(*(undefined4 *)(param_1 + 8));
          *(uint *)((int)&DAT_b800800b + DAT_00182d9c + 1) =
               (uint)(byte)(*(byte *)(param_1 + 0xe) | *(byte *)(param_1 + 0xc) |
                           *(byte *)(param_1 + 0xd));
          *(undefined4 *)((int)&DAT_b800801f + DAT_00182d9c + 1) = 0xa0;
          *(uint *)((int)&DAT_b8008007 + DAT_00182d9c + 1) = (uint)*(byte *)(param_1 + 0xf) << 4 | 1
          ;
          *(undefined4 *)((int)&DAT_b800801f + DAT_00182d9c + 1) = 0x7f;
          if (DAT_00182d9c == 0) {
            DAT_00182d98 = 0;
            DAT_00182d94 = 0;
            FUN_00100520(1,0xd,FUN_00103148);
            FUN_0010043c(0xd);
          }
          else {
            DAT_00182d98 = 0;
            DAT_00182d94 = 0;
            FUN_00100520(1,0x18,FUN_00103148);
            FUN_0010043c(0x18);
          }
          DAT_00182d88 = 1;
          uVar1 = 0;
        }
      }
      else {
        uVar1 = 0xfffffffd;
      }
    }
    else {
      uVar1 = 0xfffffffe;
    }
  }
  else {
    uVar1 = 0xffffffff;
  }
  return uVar1;
}

// ==== FUN_001036e8 @ 001036e8, 452 bytes
// called by: dbg_printf, FUN_001038d4, FUN_00103ae4, FUN_00103ecc, FUN_001038fc, FUN_001038ac
int FUN_001036e8(int param_1)

{
  int iVar1;
  
  if (DAT_00182d89 == '\x01') {
    do {
      if (DAT_00182d98 == 0xff) {
        iVar1 = 0;
      }
      else {
        iVar1 = DAT_00182d98 + 1;
      }
    } while (iVar1 == DAT_00182d94);
    (&DAT_007bcd98)[DAT_00182d98] = (char)param_1;
    if (DAT_00182d98 == 0xff) {
      DAT_00182d98 = 0;
    }
    else {
      DAT_00182d98 = DAT_00182d98 + 1;
    }
    if (param_1 == 10) {
      do {
        if (DAT_00182d98 == 0xff) {
          iVar1 = 0;
        }
        else {
          iVar1 = DAT_00182d98 + 1;
        }
      } while (iVar1 == DAT_00182d94);
      (&DAT_007bcd98)[DAT_00182d98] = 0xd;
      if (DAT_00182d98 == 0xff) {
        DAT_00182d98 = 0;
      }
      else {
        DAT_00182d98 = DAT_00182d98 + 1;
      }
    }
    if ((*(uint *)((int)&DAT_b8008003 + DAT_00182d9c + 1) & 2) == 0) {
      *(undefined4 *)((int)&DAT_b8008003 + DAT_00182d9c + 1) = 2;
    }
  }
  else {
    do {
    } while ((*(uint *)((int)&DAT_b8008017 + DAT_00182d9c + 1) & 0x400000) == 0);
    (&DAT_b8008000)[DAT_00182d9c] = (char)param_1;
    if (param_1 == 10) {
      do {
      } while ((*(uint *)((int)&DAT_b8008017 + DAT_00182d9c + 1) & 0x400000) == 0);
      (&DAT_b8008000)[DAT_00182d9c] = 0xd;
    }
  }
  return param_1;
}

// ==== FUN_001038ac @ 001038ac, 40 bytes
// called by: FUN_00103ae4
void FUN_001038ac(char *param_1)

{
  for (; *param_1 != '\0'; param_1 = param_1 + 1) {
    FUN_001036e8(*param_1);
  }
  return;
}

// ==== FUN_001038d4 @ 001038d4, 40 bytes
// called by: FUN_00103ae4, FUN_00103924
void FUN_001038d4(undefined4 param_1,int param_2)

{
  bool bVar1;
  
  while (bVar1 = param_2 != 0, param_2 = param_2 + -1, bVar1) {
    FUN_001036e8(param_1);
  }
  return;
}

// ==== FUN_001038fc @ 001038fc, 40 bytes
// called by: FUN_00103924
void FUN_001038fc(int param_1,int param_2)

{
  while (0 < param_2) {
    FUN_001036e8(*(undefined1 *)(param_1 + param_2 + -1));
    param_2 = param_2 + -1;
  }
  return;
}

// ==== FUN_00103924 @ 00103924, 448 bytes
// called by: FUN_00103ae4
void FUN_00103924(uint param_1,int param_2,int param_3,int param_4)

{
  bool bVar1;
  char cVar2;
  uint extraout_r1;
  int iVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  char local_4c [40];
  
  iVar3 = 0;
  bVar1 = false;
  if (param_4 == 0) {
    param_4 = 0x20;
  }
  bVar6 = -1 < param_3;
  if (!bVar6) {
    param_3 = -param_3;
  }
  if ((param_3 < 0) || (0x50 < param_3)) {
    param_3 = 0;
  }
  if ((param_2 < 0) && (param_2 = -param_2, (int)param_1 < 0)) {
    bVar1 = true;
    param_1 = -param_1;
  }
  do {
    if (param_2 == 0x10) {
      uVar5 = param_1 & 0xf;
      param_1 = param_1 >> 4;
    }
    else {
      FUN_0010d398(param_1,param_2);
      uVar5 = extraout_r1 & 0xffff;
      param_1 = FUN_0010d398(param_1,param_2);
    }
    if (uVar5 < 10) {
      cVar2 = '0';
    }
    else {
      cVar2 = '7';
    }
    local_4c[iVar3] = cVar2 + (char)uVar5;
    iVar4 = iVar3 + 1;
    if (((param_1 != 0) && (param_2 == 10)) &&
       ((iVar4 == 3 || ((iVar4 == 7 || (iVar4 == 0xb || iVar4 == 0xf)))))) {
      iVar4 = iVar3 + 2;
      local_4c[iVar3 + 1] = ',';
    }
    iVar3 = iVar4;
  } while (param_1 != 0);
  if (bVar1) {
    local_4c[iVar4] = '-';
    iVar4 = iVar4 + 1;
  }
  if (iVar4 < param_3) {
    if (bVar6) {
      FUN_001038d4(param_4,param_3 - iVar4);
    }
    FUN_001038fc(local_4c,iVar4);
    if (!bVar6) {
      FUN_001038d4(param_4,param_3 - iVar4);
    }
  }
  else {
    FUN_001038fc(local_4c,iVar4);
  }
  return;
}

// ==== FUN_00103ae4 @ 00103ae4, 540 bytes
// called by: dbg_printf, FUN_00103ecc
byte * FUN_00103ae4(byte *param_1,uint param_2)

{
  bool bVar1;
  uint uVar2;
  byte *pbVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  
  uVar2 = 0;
  bVar1 = false;
  iVar5 = 0;
  uVar6 = 0x20;
  if (*param_1 == 0x30) {
    uVar6 = 0x30;
  }
  do {
    pbVar3 = param_1 + 1;
    uVar4 = (uint)*param_1;
    if (uVar4 == 0) break;
    if ((uVar4 < 0x30) || (0x39 < uVar4)) {
      if (uVar4 == 100) {
LAB_00103c74:
        iVar5 = -10;
      }
      else if (uVar4 < 0x65) {
        if (uVar4 == 0x2d) {
          bVar1 = true;
        }
        else {
          if (uVar4 < 0x2e) {
            if (uVar4 == 0) {
              return param_1;
            }
            if (uVar4 == 0x25) {
              FUN_001036e8(0x25);
              return pbVar3;
            }
          }
          else {
            if (uVar4 == 0x58) {
              iVar5 = 0x10;
              goto LAB_00103cbc;
            }
            if (uVar4 == 99) {
              if (bVar1) {
                FUN_001036e8(param_2 & 0x7f);
              }
              if (0 < (int)uVar2) {
                FUN_001038d4(uVar6,uVar2 - 1);
              }
              if (bVar1) {
                return pbVar3;
              }
              FUN_001036e8(param_2 & 0x7f);
              return pbVar3;
            }
          }
LAB_00103cac:
          iVar5 = 3;
        }
      }
      else {
        if (uVar4 == 0x73) {
          if (bVar1) {
            FUN_001038ac(param_2);
          }
          uVar4 = FUN_0010c968(param_2);
          if (uVar4 < uVar2) {
            iVar5 = FUN_0010c968(param_2);
            FUN_001038d4(uVar6,uVar2 - iVar5);
          }
          if (bVar1) {
            return pbVar3;
          }
          FUN_001038ac(param_2);
          return pbVar3;
        }
        if (uVar4 < 0x74) {
          if (uVar4 == 0x69) goto LAB_00103c74;
          if (uVar4 != 0x6f) goto LAB_00103cac;
          iVar5 = 8;
        }
        else if (uVar4 == 0x75) {
          iVar5 = 10;
        }
        else {
          if (uVar4 != 0x78) goto LAB_00103cac;
          iVar5 = 0x10;
        }
      }
    }
    else {
      uVar2 = (uVar4 - 0x30) + uVar2 * 10;
    }
LAB_00103cbc:
    param_1 = pbVar3;
  } while (iVar5 == 0);
  if (bVar1) {
    uVar2 = -uVar2;
  }
  FUN_00103924(param_2,iVar5,uVar2,uVar6);
  return pbVar3;
}

// ==== FUN_00103d00 @ 00103d00, 36 bytes
// called by: dbg_printf, FUN_001014c8, FUN_00103ecc
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00103d00(void)

{
  undefined4 uVar1;
  
  if (_DAT_ffff3eb4 == 0x50423238) {
    uVar1 = 0x47;
  }
  else {
    uVar1 = 0x41;
  }
  return uVar1;
}

// ==== dbg_printf @ 00103d24, 288 bytes
// called by: FUN_0010c414, config_load, lcd_boot_proc, usb_receive_data, FUN_00104c60, FUN_00104c24, sys_set_clock_divider, FUN_0012c6d0, FUN_0012c870, FUN_001296f8, FUN_00128914, FUN_00103e8c, FUN_0012d614, lcm_init, lcd_boot_restart, FUN_0012acf8
// every debug format string goes through it
/* every debug format string goes through it */

undefined8 dbg_printf(char *param_1,int param_2,undefined *param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  char *local_28;
  int local_24;
  undefined *local_20;
  undefined4 local_1c;
  char *local_10;
  int local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = param_4;
  puStack_8 = param_3;
  local_c = param_2;
  local_28 = param_1;
  local_24 = param_2;
  local_20 = param_3;
  local_1c = param_4;
  if (DAT_00182da4 == -1) {
    iVar3 = FUN_00103d00();
    if ((iVar3 == 0x47) && (uVar2 = DAT_b0000004, (uVar2 & 4) == 4)) goto LAB_00103d5c;
  }
  else if (DAT_00182da4 == 0) goto LAB_00103d5c;
  DAT_00182d89 = 0;
  if (DAT_00182d88 == '\0') {
    FUN_00103060(1);
    local_28 = &DAT_00000001;
    local_24 = FUN_001014c8();
    local_24 = local_24 * 1000;
    local_20 = &DAT_0001c200;
    local_1c = 3;
    FUN_00103514(&local_28);
  }
  piVar4 = &local_c;
  local_10 = param_1;
  while (*local_10 != '\0') {
    if (*local_10 == '%') {
      local_10 = (char *)FUN_00103ae4(local_10 + 1,*piVar4);
      piVar4 = piVar4 + 1;
    }
    else {
      cVar1 = *local_10;
      local_10 = local_10 + 1;
      FUN_001036e8(cVar1);
    }
  }
LAB_00103d5c:
  return CONCAT44(local_24,local_28);
}

// ==== FUN_00103e8c @ 00103e8c, 64 bytes
void FUN_00103e8c(int param_1)

{
  DAT_00182da4 = (uint)(param_1 == 1);
  dbg_printf("u32DbgMessage = 0x%x\n",DAT_00182da4);
  return;
}

// ==== FUN_00103ecc @ 00103ecc, 288 bytes
// called by: usb_receive_cmd, FUN_00129f54
undefined8 FUN_00103ecc(char *param_1,int param_2,undefined *param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  int *piVar4;
  char *local_28;
  int local_24;
  undefined *local_20;
  undefined4 local_1c;
  char *local_10;
  int local_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = param_4;
  puStack_8 = param_3;
  local_c = param_2;
  local_28 = param_1;
  local_24 = param_2;
  local_20 = param_3;
  local_1c = param_4;
  if (DAT_00182da4 == -1) {
    iVar3 = FUN_00103d00();
    if ((iVar3 == 0x47) && (uVar2 = DAT_b0000004, (uVar2 & 4) == 4)) goto LAB_00103f04;
  }
  else if (DAT_00182da4 == 0) goto LAB_00103f04;
  DAT_00182d89 = 1;
  if (DAT_00182d88 == '\0') {
    FUN_00103060(1);
    local_28 = &DAT_00000001;
    local_24 = FUN_001014c8();
    local_24 = local_24 * 1000;
    local_20 = &DAT_0001c200;
    local_1c = 3;
    FUN_00103514(&local_28);
  }
  piVar4 = &local_c;
  local_10 = param_1;
  while (*local_10 != '\0') {
    if (*local_10 == '%') {
      local_10 = (char *)FUN_00103ae4(local_10 + 1,*piVar4);
      piVar4 = piVar4 + 1;
    }
    else {
      cVar1 = *local_10;
      local_10 = local_10 + 1;
      FUN_001036e8(cVar1);
    }
  }
LAB_00103f04:
  return CONCAT44(local_24,local_28);
}

