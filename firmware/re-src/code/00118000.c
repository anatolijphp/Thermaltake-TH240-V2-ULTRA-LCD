// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00118000-0x0011bfff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_001180e8 @ 001180e8, 352 bytes
// called by: FUN_001174f0, FUN_00117fa8
void FUN_001180e8(int param_1,undefined2 *param_2)

{
  int iVar1;
  uint uVar2;
  short *local_30;
  int local_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  
  if (param_1 != 0) {
    local_30 = (short *)FUN_0010e24c();
    uVar2 = *(uint *)(local_30 + 0x14);
    if ((uVar2 & 2) != 0) {
      local_28 = *(undefined4 *)local_30;
      uStack_24 = *(undefined4 *)(local_30 + 2);
      if ((uVar2 & 0x10000) != 0) {
        local_2c = FUN_0010e24c(param_1);
        if ((*(byte *)(local_2c + 0x28) & 2) != 0) {
          if ((*(int *)(local_2c + 0x24) != 0) && (DAT_001830e4 != param_1)) {
            FUN_00110e78();
            *(undefined4 *)(local_2c + 0x24) = 0;
          }
          for (iVar1 = FUN_00117f58(param_1); iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x20)) {
            FUN_00119230(iVar1);
            iVar1 = FUN_0010e384(iVar1);
          }
        }
        FUN_0010e334(&local_2c);
      }
      if (param_2 != (undefined2 *)0x0) {
        local_20 = *param_2;
        local_1e = param_2[1];
        local_1c = param_2[2];
        local_1a = param_2[3];
        FUN_001129e8(&local_20,(int)*local_30,(int)local_30[1]);
        FUN_00114e9c(&local_28,&local_20);
      }
      iVar1 = FUN_00118820(&local_28,param_1);
      if (iVar1 != 0) {
        if ((uVar2 & 0x801) == 1) {
          FUN_001192f8(&local_28,*(undefined4 *)(local_30 + 0xc),*(undefined4 *)(local_30 + 0x10),
                       0x20);
        }
        else {
          FUN_00124394(param_1,&local_28,0x20);
        }
      }
    }
    FUN_0010e334(&local_30);
    return;
  }
  return;
}

// ==== FUN_00118250 @ 00118250, 176 bytes
// called by: FUN_00118250, FUN_001192f8
void FUN_00118250(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int local_20 [2];
  undefined1 auStack_18 [8];
  
  if (param_1 == 0) {
    return;
  }
  local_20[0] = FUN_0010e24c();
  if (((*(byte *)(local_20[0] + 0x28) & 2) != 0) &&
     (iVar1 = FUN_00114ef0(auStack_18,param_2,local_20[0]), iVar1 != 0)) {
    FUN_0010e334(local_20);
    FUN_00124394(param_1,auStack_18,param_3);
    iVar1 = FUN_00117f58(param_1);
    if (iVar1 != 0) {
      do {
        FUN_00118250(iVar1,auStack_18,param_3);
        iVar1 = FUN_0010e384(iVar1);
        iVar1 = *(int *)(iVar1 + 0x20);
      } while (iVar1 != 0);
      return;
    }
    return;
  }
  FUN_0010e334(local_20);
  return;
}

// ==== FUN_00118300 @ 00118300, 36 bytes
// called by: screen_create_proc
bool FUN_00118300(int param_1)

{
  bool bVar1;
  
  bVar1 = DAT_001830d8 != (undefined *)0x0;
  DAT_001830d8 = (undefined *)0x0;
  if (param_1 != 0) {
    DAT_001830d8 = &DAT_00181e2c;
  }
  return bVar1;
}

// ==== thunk_FUN_00119460 @ 0011832c, 4 bytes
// called by: FUN_0011997c
void thunk_FUN_00119460(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *unaff_r10;
  bool bVar5;
  undefined4 *puStack_78;
  int iStack_74;
  undefined4 uStack_70;
  undefined4 uStack_6c;
  undefined4 uStack_68;
  int *piStack_64;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  int *piStack_4c;
  int iStack_48;
  int iStack_44;
  undefined4 uStack_38;
  undefined4 uStack_34;
  int iStack_30;
  int iStack_2c;
  
  uStack_70 = 0;
  uStack_6c = 0;
  uStack_68 = 0;
  piStack_64 = (int *)0x0;
  if (param_1 == 0 || param_2 == 0 && param_3 == 0) {
    return;
  }
  puStack_78 = (undefined4 *)FUN_0010e24c(param_1);
  uStack_38 = *puStack_78;
  uStack_34 = puStack_78[1];
  FUN_001129e8(puStack_78,param_2,param_3);
  FUN_001129e8(puStack_78 + 2,param_2,param_3);
  iVar2 = puStack_78[7];
  FUN_0010e334(&puStack_78);
  bVar5 = iVar2 != 0;
  if (bVar5) {
    unaff_r10 = &iStack_48;
  }
  uStack_58 = 0;
  uStack_54 = 0;
  uStack_50 = 0;
  piStack_4c = (int *)0x0;
  while (bVar5) {
    iStack_74 = FUN_0010e24c(iVar2);
    FUN_001129e8(iStack_74,param_2,param_3);
    FUN_001129e8(iStack_74 + 8,param_2,param_3);
    uVar3 = *(undefined4 *)(iStack_74 + 0x1c);
    iVar4 = *(int *)(iStack_74 + 0x20);
    FUN_0010e334(&iStack_74);
    FUN_00124b64(uVar3,param_2,param_3);
    uStack_58 = 3;
    piStack_4c = unaff_r10;
    iStack_48 = param_2;
    iStack_44 = param_3;
    FUN_001198a4(iVar2,&uStack_58);
    bVar5 = iVar4 != 0;
    iVar2 = iVar4;
  }
  puStack_78 = (undefined4 *)FUN_0010e24c(param_1);
  uVar1 = (uint)*(byte *)(puStack_78 + 10);
  bVar5 = (*(byte *)(puStack_78 + 10) & 2) != 0;
  if (bVar5) {
    uVar1 = puStack_78[6];
  }
  if (bVar5 && uVar1 != 0) {
    FUN_001192bc(puStack_78,uVar1,puStack_78[8]);
    FUN_001192bc(&uStack_38,puStack_78[6],puStack_78[8]);
  }
  FUN_0010e334(&puStack_78);
  piStack_64 = &iStack_30;
  uStack_70 = 3;
  iStack_30 = param_2;
  iStack_2c = param_3;
  FUN_001198a4(param_1,&uStack_70);
  return;
}

// ==== FUN_00118330 @ 00118330, 76 bytes
// called by: FUN_00117768
void FUN_00118330(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_20 [2];
  int iStack_18;
  undefined4 local_14;
  
  local_20[0] = 0x26;
  if (param_1 == 0) {
    return;
  }
  local_14 = param_2;
  iVar1 = FUN_0010e384();
  if (*(int *)(iVar1 + 0x18) != 0) {
    iStack_18 = param_1;
    FUN_001198a4(*(int *)(iVar1 + 0x18),local_20);
    return;
  }
  return;
}

// ==== FUN_0011837c @ 0011837c, 92 bytes
// called by: FUN_00112a2c
/* WARNING: Restarted to delay deadcode elimination for space: stack */

undefined4 FUN_0011837c(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  code *in_lr;
  undefined4 auStack_20 [3];
  undefined4 *puStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  code *local_4;
  
  if ((&DAT_007c60f0)[DAT_001830c4] != 0) {
    auStack_20[0] = 0xe;
    puStack_14 = &uStack_10;
    local_4 = in_lr;
    uStack_10 = param_1;
    uStack_c = param_2;
    FUN_001198a4((&DAT_007c60f0)[DAT_001830c4],auStack_20);
                    /* WARNING: Could not recover jumptable at 0x001183d4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (*local_4)(1);
    return uVar1;
  }
  return 0;
}

// ==== FUN_001183e0 @ 001183e0, 360 bytes
// called by: FUN_00118650, FUN_0011997c
void FUN_001183e0(int param_1,int param_2,int param_3)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iStack_30;
  int iStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  int aiStack_18 [2];
  
  if ((param_2 != 0 || param_3 != 0) && param_1 != 0) {
    piVar1 = (int *)FUN_0010e24c(param_1);
    (&iStack_30)[1] = (int)piVar1;
    iVar2 = *piVar1;
    iVar3 = piVar1[1];
    (&iStack_30)[2] = iVar2;
    (&iStack_30)[3] = iVar3;
    (&iStack_30)[4] = iVar2;
    (&iStack_30)[5] = iVar3;
    if (param_2 != 0) {
      if (((piVar1[10] & 0x80U) == 0) || ((piVar1[10] & 0x200U) != 0)) {
        *(short *)(&iStack_30 + 5) = (short)(&iStack_30)[5] + (short)param_2;
      }
      else {
        *(short *)(&iStack_30 + 4) = (short)(&iStack_30)[4] - (short)param_2;
      }
    }
    if (param_3 != 0) {
      if (((piVar1[10] & 0x100U) == 0) || ((piVar1[10] & 0x400U) != 0)) {
        *(short *)((int)&iStack_30 + 0x16) = *(short *)((int)&iStack_30 + 0x16) + (short)param_3;
      }
      else {
        *(short *)((int)&iStack_30 + 0x12) = *(short *)((int)&iStack_30 + 0x12) - (short)param_3;
      }
    }
    if ((piVar1[10] & 0x10000U) != 0) {
      FUN_00119230(param_1);
    }
    piVar1 = (int *)(&iStack_30)[1];
    iVar2 = (&iStack_30)[5];
    *piVar1 = (&iStack_30)[4];
    piVar1[1] = iVar2;
    if ((*(byte *)(piVar1 + 10) & 2) != 0) {
      FUN_00112958(&iStack_30 + 6,&iStack_30 + 2,&iStack_30 + 4);
      iVar2 = (&iStack_30)[1];
      FUN_00119458(&iStack_30 + 6,*(undefined4 *)(iVar2 + 0x18),*(undefined4 *)(iVar2 + 0x20));
    }
    iStack_30 = (int)*(short *)((int)&iStack_30 + 0x16) - (int)*(short *)((int)&iStack_30 + 0xe);
    FUN_0011997c((&iStack_30)[1],(int)(short)(&iStack_30)[4] - (int)(short)(&iStack_30)[2],
                 (int)*(short *)((int)&iStack_30 + 0x12) - (int)*(short *)((int)&iStack_30 + 10),
                 (int)(short)(&iStack_30)[5] - (int)(short)(&iStack_30)[3]);
    FUN_00114e9c((&iStack_30)[1] + 8);
    FUN_0011995c(param_1,5);
    FUN_0010e334(&iStack_30 + 1);
    return;
  }
  return;
}

// ==== FUN_00118548 @ 00118548, 168 bytes
// called by: FUN_001174f0, FUN_00117fa8, FUN_001179fc, FUN_00117768
undefined4 FUN_00118548(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  short *psVar5;
  undefined4 uVar6;
  
  if (param_1 == 0) {
    param_1 = DAT_001830c8;
  }
  uVar6 = *(undefined4 *)(DAT_00182efc + 0x44);
  *(int *)(DAT_00182efc + 0x44) = param_1;
  iVar1 = param_1;
  do {
    iVar2 = iVar1;
    iVar3 = FUN_0010e384();
    iVar1 = *(int *)(iVar3 + 0x18);
  } while (*(int *)(iVar3 + 0x18) != 0);
  if (DAT_007c60c0 == iVar2) {
    uVar4 = 0;
  }
  else if (DAT_007c60c4 == iVar2) {
    uVar4 = 1;
  }
  else if (DAT_007c60c8 == iVar2) {
    uVar4 = 2;
  }
  else {
    if (DAT_007c60cc != iVar2) goto LAB_001185c8;
    uVar4 = 3;
  }
  FUN_00112bb8(uVar4);
LAB_001185c8:
  FUN_00116a7c();
  psVar5 = (short *)FUN_0010e384(param_1);
  iVar1 = DAT_00182efc;
  *(int *)(DAT_00182efc + 0x48) = (int)*psVar5;
  *(int *)(iVar1 + 0x4c) = (int)psVar5[1];
  return uVar6;
}

// ==== thunk_FUN_0011990c @ 001185fc, 4 bytes
// called by: FUN_001195dc
void thunk_FUN_0011990c(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  
  uStack_20 = 0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  iVar1 = FUN_0010e384(param_1);
  if (*(code **)(iVar1 + 0x10) != (code *)0x0) {
    uStack_20 = param_2;
    uStack_1c = param_1;
    (**(code **)(iVar1 + 0x10))(&uStack_20);
    return;
  }
  return;
}

// ==== FUN_00118638 @ 00118638, 20 bytes
// called by: screen_create_proc
undefined4 FUN_00118638(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_001830d0;
  DAT_001830d0 = param_1;
  return uVar1;
}

// ==== FUN_00118650 @ 00118650, 100 bytes
// called by: FUN_00111208
void FUN_00118650(int param_1,int param_2,int param_3,short *param_4)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  short *local_18;
  
  if (param_1 == 0) {
    return;
  }
  local_18 = param_4;
  local_18 = (short *)FUN_0010e24c();
  sVar1 = local_18[2];
  sVar2 = *local_18;
  sVar3 = local_18[3];
  sVar4 = local_18[1];
  FUN_0010e334(&local_18);
  FUN_001183e0(param_1,(param_2 - ((int)sVar1 - (int)sVar2)) + -1,
               (param_3 - ((int)sVar3 - (int)sVar4)) + -1);
  return;
}

// ==== FUN_001186b4 @ 001186b4, 328 bytes
// called by: FUN_001189d8, FUN_00118f24
void FUN_001186b4(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *local_30;
  short *local_2c;
  undefined4 local_28;
  undefined2 local_24;
  undefined2 local_22;
  undefined4 local_20;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined4 local_18;
  undefined4 local_14;
  
  if (*(char *)(DAT_00182efc + 0x50) == '\0') {
    local_30 = (undefined4 *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
    local_18 = *local_30;
    local_14 = local_30[1];
    FUN_0010e334(&local_30);
    if (DAT_001830e0 != 0) {
      FUN_00118820(&local_18);
    }
    puVar3 = &local_18;
    puVar1 = *(undefined2 **)(DAT_00182efc + 0x40);
    if (puVar1 != (undefined2 *)0x0) {
      local_28._0_2_ = *puVar1;
      local_28._2_2_ = puVar1[1];
      local_24 = puVar1[2];
      local_22 = puVar1[3];
      iVar2 = DAT_001830e0;
      if (DAT_001830e0 == 0) {
        iVar2 = *(int *)(DAT_00182efc + 0x44);
      }
      local_2c = (short *)FUN_0010e24c(iVar2);
      FUN_001129e8(&local_28,(int)*local_2c,(int)local_2c[1]);
      FUN_0010e334(&local_2c);
      FUN_00114e9c(&local_28,puVar3);
      puVar3 = &local_28;
    }
  }
  else {
    puVar1 = *(undefined2 **)(DAT_00182efc + 0x40);
    puVar3 = &DAT_007c6108;
    if (puVar1 != (undefined2 *)0x0) {
      local_20._0_2_ = *puVar1;
      local_20._2_2_ = puVar1[1];
      local_1c = puVar1[2];
      local_1a = puVar1[3];
      iVar2 = DAT_001830e0;
      if (DAT_001830e0 == 0) {
        iVar2 = *(int *)(DAT_00182efc + 0x44);
      }
      local_2c = (short *)FUN_0010e24c(iVar2);
      FUN_001129e8(&local_20,(int)*local_2c,(int)local_2c[1]);
      FUN_0010e334(&local_2c);
      FUN_00114e9c(&local_20,&DAT_007c6108);
      puVar3 = &local_20;
    }
  }
  FUN_00116a28(puVar3);
  return;
}

// ==== FUN_00118808 @ 00118808, 20 bytes
// called by: FUN_00117fa8
void FUN_00118808(undefined4 *param_1)

{
  *param_1 = DAT_001830cc;
  DAT_001830cc = param_1;
  return;
}

// ==== FUN_00118820 @ 00118820, 160 bytes
// called by: FUN_001186b4, FUN_00118f24, FUN_001180e8, FUN_001179fc
undefined4 FUN_00118820(undefined4 param_1,undefined4 *param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  int local_10;
  
  local_10 = param_4;
  while( true ) {
    local_10 = FUN_0010e24c(param_2);
    if ((*(byte *)(local_10 + 0x28) & 2) == 0) {
      FUN_0010e334(&local_10);
      return 0;
    }
    FUN_00114e9c(param_1,local_10);
    puVar1 = *(undefined4 **)(local_10 + 0x18);
    if (puVar1 == (undefined4 *)0x0) break;
    FUN_0010e334(&local_10);
    param_2 = puVar1;
  }
  FUN_0010e334(&local_10);
  puVar1 = DAT_007c60c0;
  if (DAT_007c60c0 != param_2) {
    puVar1 = DAT_007c60c4;
  }
  if (DAT_007c60c0 != param_2 && puVar1 != param_2) {
    puVar1 = &DAT_007c60c0;
    if (DAT_007c60c8 != param_2) {
      puVar1 = DAT_007c60cc;
    }
    if (DAT_007c60c8 != param_2 && puVar1 != param_2) {
      return 0;
    }
  }
  return 1;
}

// ==== FUN_001188c4 @ 001188c4, 180 bytes
// called by: FUN_00117768
void FUN_001188c4(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_18;
  undefined4 *puStack_14;
  
  iVar1 = FUN_001130f8(&iStack_18);
  if (iVar1 != 0) {
    do {
      iVar2 = FUN_001130bc(iVar1,1);
      if ((iStack_18 == 0 || iVar2 == 0) ||
         (iVar3 = FUN_0010e384(iStack_18), iVar2 = iStack_18, *(int *)(iVar3 + 4) != param_1)) {
        iVar1 = FUN_00113140(iVar1,&iStack_18);
      }
      else {
        iVar1 = FUN_00113140(iVar1,&iStack_18);
        if (iVar2 != 0) {
          puStack_14 = (undefined4 *)FUN_0010e24c(iVar2);
          FUN_00112f8c(*puStack_14);
          FUN_0010e334(&puStack_14);
          FUN_0010e0a8(iVar2);
        }
      }
    } while (iVar1 != 0);
    return;
  }
  return;
}

// ==== FUN_00118978 @ 00118978, 48 bytes
// called by: FUN_001198a4, FUN_00126c5c
void FUN_00118978(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_10;
  
  if (param_1 == 0 || param_2 == 0) {
    return;
  }
  local_10 = param_4;
  local_10 = FUN_0010e24c();
  FUN_001189a8(local_10,param_2);
  FUN_0010e334(&local_10);
  return;
}

// ==== FUN_001189a8 @ 001189a8, 48 bytes
// called by: FUN_00118978
void FUN_001189a8(short *param_1,undefined2 *param_2)

{
  param_2[1] = 0;
  *param_2 = 0;
  param_2[2] = param_1[2] - *param_1;
  param_2[3] = param_1[3] - param_1[1];
  return;
}

// ==== FUN_001189d8 @ 001189d8, 1256 bytes
// called by: FUN_001143a8, FUN_0010e41c, FUN_001195dc, FUN_00110c20, FUN_00118f24, FUN_0011b2e8, FUN_0010eaec, FUN_0011158c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_001189d8(void)

{
  bool bVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined4 uStack_48;
  undefined4 uStack_44;
  undefined4 *puStack_40;
  undefined4 *puStack_3c;
  undefined4 *puStack_38;
  undefined4 uStack_34;
  int aiStack_30 [2];
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  if (*(char *)(DAT_00182efc + 0x50) == '\0') {
    return 0;
  }
  if (DAT_007c6114 < 2) {
    DAT_007c6110 = DAT_007c6110 + 1;
    uStack_44 = DAT_007c610c;
    sVar2 = DAT_007c6102;
    if (DAT_007c6110 == 0) goto LAB_00118a74;
    iVar3 = ((DAT_007c610c & 0xffff) + 1) * 0x10000;
    uStack_48 = CONCAT22(DAT_007c6108._2_2_,(short)((uint)iVar3 >> 0x10));
    if (iVar3 >> 0x10 <= (int)_DAT_007c6104) goto LAB_00118a78;
    do {
      sVar2 = DAT_007c610c._2_2_ + 1;
LAB_00118a74:
      uStack_48 = CONCAT22(sVar2,_DAT_007c6100);
LAB_00118a78:
      if (DAT_007c6106 < uStack_48._2_2_) break;
      aiStack_30[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
      if ((short)uStack_48 == _DAT_007c6100) {
        uStack_44 = CONCAT22(DAT_007c6106,_DAT_007c6104);
        iVar3 = *(int *)(DAT_00182efc + 0x44);
        while (iVar3 != 0) {
          iVar4 = FUN_0010e384();
          iVar3 = *(int *)(iVar4 + 0x20);
          while (iVar3 != 0) {
            puStack_3c = (undefined4 *)FUN_0010e24c(iVar3);
            uVar6 = puStack_3c[10];
            if ((uVar6 & 2) != 0) {
              uStack_28 = *puStack_3c;
              uStack_24 = puStack_3c[1];
              iVar3 = FUN_00112b2c(&uStack_48,&uStack_28);
              if (iVar3 != 0) {
                if ((uVar6 & 1) == 0) {
                  if (uStack_48._2_2_ < *(short *)((int)puStack_3c + 2)) {
                    iVar3 = (int)uStack_44._2_2_;
                    if (uStack_28._2_2_ + -1 <= (int)uStack_44._2_2_) {
                      iVar3 = uStack_28._2_2_ + -1;
                    }
                    sVar2 = (short)iVar3;
                  }
                  else {
                    sVar2 = uStack_44._2_2_;
                    if (uStack_24._2_2_ <= uStack_44._2_2_) {
                      sVar2 = uStack_24._2_2_;
                    }
                  }
                  uStack_44 = CONCAT22(sVar2,(undefined2)uStack_44);
                }
                else {
                  for (iVar3 = puStack_3c[7]; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x20)) {
                    FUN_00122b34(iVar3,&uStack_48,&uStack_28);
                    iVar3 = FUN_0010e384(iVar3);
                  }
                }
              }
            }
            iVar3 = puStack_3c[8];
            FUN_0010e334(&puStack_3c);
          }
          iVar3 = *(int *)(iVar4 + 0x18);
        }
        FUN_00122b34(*(undefined4 *)(aiStack_30[0] + 0x1c),&uStack_48,0);
      }
LAB_00118b88:
      do {
        uStack_44 = CONCAT22(uStack_44._2_2_,(short)uStack_48);
        for (iVar3 = *(int *)(DAT_00182efc + 0x44); iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x18)) {
          iVar3 = FUN_0010e384();
          iVar4 = *(int *)(iVar3 + 0x20);
          bVar1 = false;
          if (iVar4 != 0) {
            do {
              puStack_38 = (undefined4 *)FUN_0010e24c(iVar4);
              uVar6 = puStack_38[10];
              if ((uVar6 & 2) != 0) {
                uStack_28 = *puStack_38;
                uStack_24 = puStack_38[1];
                iVar4 = FUN_00112b2c(&uStack_48,&uStack_28);
                if (iVar4 != 0) {
                  if ((uVar6 & 1) == 0) {
                    bVar1 = true;
                    uStack_48 = CONCAT22(uStack_48._2_2_,(short)uStack_24 + 1);
                  }
                  else {
                    for (iVar4 = puStack_38[7]; iVar4 != 0; iVar4 = *(int *)(iVar4 + 0x20)) {
                      iVar5 = FUN_0012297c(iVar4,&uStack_48,&uStack_28);
                      if (iVar5 != 0) {
                        bVar1 = true;
                      }
                      iVar4 = FUN_0010e384(iVar4);
                    }
                  }
                }
              }
              iVar4 = puStack_38[8];
              FUN_0010e334(&puStack_38);
            } while (iVar4 != 0);
            if (bVar1) goto LAB_00118b88;
          }
        }
        iVar3 = FUN_0012297c(*(undefined4 *)(aiStack_30[0] + 0x1c),&uStack_48,0);
      } while (iVar3 != 0);
      uStack_44 = CONCAT22(uStack_44._2_2_,_DAT_007c6104);
      if ((short)uStack_48 <= _DAT_007c6104) goto LAB_00118d0c;
      DAT_007c6108 = uStack_48;
      DAT_007c610c = uStack_44;
      FUN_0010e334(aiStack_30);
    } while( true );
  }
LAB_00118a08:
  DAT_007c6114 = DAT_007c6114 + -1;
  return 0;
LAB_00118d0c:
  iVar3 = *(int *)(DAT_00182efc + 0x44);
  while (iVar3 != 0) {
    iVar4 = FUN_0010e384();
    iVar3 = *(int *)(iVar4 + 0x20);
    while (iVar3 != 0) {
      puStack_3c = (undefined4 *)FUN_0010e24c(iVar3);
      uVar6 = puStack_3c[10];
      if ((uVar6 & 2) != 0) {
        uStack_28 = *puStack_3c;
        uStack_24 = puStack_3c[1];
        iVar3 = FUN_00112b2c(&uStack_48,&uStack_28);
        if (iVar3 != 0) {
          if ((uVar6 & 1) == 0) {
            uStack_44 = CONCAT22(uStack_44._2_2_,(short)uStack_28 + -1);
          }
          else {
            for (iVar3 = puStack_3c[7]; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x20)) {
              FUN_00122a60(iVar3,&uStack_48,&uStack_28);
              iVar3 = FUN_0010e384(iVar3);
            }
          }
        }
      }
      iVar3 = puStack_3c[8];
      FUN_0010e334(&puStack_3c);
    }
    iVar3 = *(int *)(iVar4 + 0x18);
  }
  iVar3 = *(int *)(aiStack_30[0] + 0x1c);
  while (iVar3 != 0) {
    puStack_40 = (undefined4 *)FUN_0010e24c(iVar3);
    uVar6 = puStack_40[10];
    if ((uVar6 & 2) != 0) {
      puStack_38 = (undefined4 *)*puStack_40;
      uStack_34 = puStack_40[1];
      iVar3 = FUN_00112b2c(&uStack_48,&puStack_38);
      if (iVar3 != 0) {
        if ((uVar6 & 1) == 0) {
          uStack_44 = CONCAT22(uStack_44._2_2_,(short)puStack_38 + -1);
        }
        else {
          for (iVar3 = puStack_40[7]; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x20)) {
            FUN_00122a60(iVar3,&uStack_48,&puStack_38);
            iVar3 = FUN_0010e384(iVar3);
          }
        }
      }
    }
    iVar3 = puStack_40[8];
    FUN_0010e334(&puStack_40);
  }
  FUN_0010e334(aiStack_30);
  if (DAT_007c6110 < 0x4e21) {
    DAT_007c6108 = uStack_48;
    DAT_007c610c = uStack_44;
    FUN_001186b4();
    return 1;
  }
  goto LAB_00118a08;
}

// ==== FUN_00118ed0 @ 00118ed0, 84 bytes
// called by: FUN_00117424
int FUN_00118ed0(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_00117f70();
  if (iVar1 == 0) {
LAB_00118efc:
    iVar1 = 0;
  }
  else {
    iVar1 = FUN_0010e384();
    iVar1 = *(int *)(iVar1 + 0x1c);
    if (iVar1 != 0) {
      do {
        if (iVar1 == param_1) goto LAB_00118efc;
        iVar2 = FUN_0010e384(iVar1);
        iVar2 = *(int *)(iVar2 + 0x20);
        if (iVar2 != param_1) {
          iVar1 = iVar2;
        }
      } while (iVar2 != param_1 && iVar2 != 0);
    }
  }
  return iVar1;
}

// ==== FUN_00118f24 @ 00118f24, 460 bytes
// called by: FUN_001143a8, FUN_0010e41c, FUN_001195dc, FUN_00110c20, FUN_0011b2e8, FUN_0010eaec, FUN_0011158c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00118f24(int param_1)

{
  undefined2 *puVar1;
  short **ppsVar2;
  int iVar3;
  undefined4 uVar4;
  short *psStack_30;
  short *psStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined2 uStack_20;
  undefined2 uStack_1e;
  undefined2 uStack_1c;
  undefined2 uStack_1a;
  
  ppsVar2 = &psStack_30;
  if (*(char *)(DAT_00182efc + 0x50) == '\0') {
    FUN_001186b4();
    return 1;
  }
  DAT_007c6114 = DAT_007c6114 + 1;
  if (1 < DAT_007c6114) {
    return 1;
  }
  psStack_2c = (short *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
  DAT_007c6110 = 0xffffffff;
  if (DAT_001830b4 == '\0') {
    if ((*(byte *)(psStack_2c + 0x14) & 2) == 0) {
      DAT_007c6114 = DAT_007c6114 + -1;
      FUN_0010e334(&psStack_2c);
      return 0;
    }
    uStack_28 = *(undefined4 *)psStack_2c;
    uStack_24 = *(undefined4 *)(psStack_2c + 2);
  }
  else {
    uStack_28 = *(undefined4 *)(psStack_2c + 4);
    uStack_24 = *(undefined4 *)(psStack_2c + 6);
  }
  if (param_1 != 0) {
    FUN_00114e9c(&uStack_28,param_1);
  }
  if (*(int *)(DAT_00182efc + 0x40) == 0) {
    ppsVar2 = &psStack_2c;
  }
  else {
    psStack_30 = psStack_2c;
    puVar1 = *(undefined2 **)(DAT_00182efc + 0x40);
    uStack_20 = *puVar1;
    uStack_1e = puVar1[1];
    uStack_1c = puVar1[2];
    uStack_1a = puVar1[3];
    if (DAT_001830e0 != 0) {
      FUN_0010e334(&psStack_30);
      psStack_30 = (short *)FUN_0010e24c(DAT_001830e0);
    }
    FUN_001129e8(&uStack_20,(int)*psStack_30,(int)psStack_30[1]);
    FUN_00114e9c(&uStack_28,&uStack_20);
  }
  FUN_0010e334(ppsVar2);
  if (((DAT_001830e0 == 0) || (iVar3 = FUN_00118820(&uStack_28), iVar3 != 0)) &&
     (iVar3 = FUN_00118820(&uStack_28,*(undefined4 *)(DAT_00182efc + 0x44)), iVar3 != 0)) {
    _DAT_007c6100 = uStack_28;
    _DAT_007c6104 = uStack_24;
    uVar4 = FUN_001189d8();
    return uVar4;
  }
  DAT_007c6114 = DAT_007c6114 + -1;
  return 0;
}

// ==== FUN_00119100 @ 00119100, 304 bytes
// called by: FUN_001174f0
void FUN_00119100(undefined4 param_1,int param_2)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18 [2];
  
  if (param_2 == 0) {
    return;
  }
  piVar2 = &local_20;
  local_20 = FUN_0010e24c();
  *(int *)(local_20 + 0x18) = param_2;
  *(undefined4 *)(local_20 + 0x20) = 0;
  local_18[0] = FUN_0010e24c(param_2);
  bVar1 = *(byte *)(local_20 + 0x28);
  iVar3 = *(int *)(local_18[0] + 0x1c);
  if (iVar3 == 0) {
    *(undefined4 *)(local_18[0] + 0x1c) = param_1;
    FUN_0010e334(&local_20);
    FUN_0010e334(local_18);
    return;
  }
  local_1c = FUN_0010e24c(iVar3);
  if (((bVar1 & 8) == 0) && ((*(byte *)(local_1c + 0x28) & 8) != 0)) {
    *(int *)(local_20 + 0x20) = iVar3;
    *(undefined4 *)(local_18[0] + 0x1c) = param_1;
    FUN_0010e334(&local_20);
    FUN_0010e334(local_18);
    piVar2 = &local_1c;
  }
  else {
    FUN_0010e334(local_18);
    FUN_0010e334(&local_1c);
    iVar3 = FUN_0010e384(iVar3);
    do {
      iVar4 = *(int *)(iVar3 + 0x20);
      local_1c = iVar3;
      if (iVar4 == 0) {
        *(undefined4 *)(iVar3 + 0x20) = param_1;
        goto LAB_0011921c;
      }
      iVar3 = FUN_0010e384(iVar4);
    } while (((bVar1 & 8) != 0) || ((*(byte *)(iVar3 + 0x28) & 8) == 0));
    *(undefined4 *)(local_1c + 0x20) = param_1;
    *(int *)(local_20 + 0x20) = iVar4;
    piVar2 = &local_20;
  }
LAB_0011921c:
  FUN_0010e334(piVar2);
  return;
}

// ==== FUN_00119230 @ 00119230, 136 bytes
// called by: FUN_00119230, FUN_001180e8, FUN_001183e0
void FUN_00119230(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  if (param_1 != 0) {
    local_10 = param_4;
    local_10 = FUN_0010e24c();
    if ((*(byte *)(local_10 + 0x28) & 2) != 0) {
      if ((*(int *)(local_10 + 0x24) != 0) && (DAT_001830e4 != param_1)) {
        FUN_00110e78();
        *(undefined4 *)(local_10 + 0x24) = 0;
      }
      for (iVar1 = FUN_00117f58(param_1); iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x20)) {
        FUN_00119230(iVar1);
        iVar1 = FUN_0010e384(iVar1);
      }
    }
    FUN_0010e334(&local_10);
    return;
  }
  return;
}

// ==== FUN_001192bc @ 001192bc, 60 bytes
// called by: FUN_00119460
void FUN_001192bc(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  int iStack_3c;
  int aiStack_38 [4];
  undefined1 auStack_28 [8];
  undefined1 auStack_20 [16];
  
  iVar1 = FUN_0010e384(param_2);
  if ((*(uint *)(iVar1 + 0x28) & 0x10000) == 0) {
    uVar3 = 0x20;
  }
  else {
    uVar3 = 0x4000;
  }
  if (param_2 != 0) {
    iStack_3c = FUN_0010e24c(param_2);
    uVar4 = *(uint *)(iStack_3c + 0x28);
    if (((uVar4 & 2) != 0) && (iVar1 = FUN_00114ef0(auStack_28,param_1,iStack_3c), iVar1 != 0)) {
      if ((uVar4 & 0x801) == 1) {
        FUN_001192f8(auStack_28,*(undefined4 *)(iStack_3c + 0x18),*(undefined4 *)(iStack_3c + 0x20),
                     uVar3);
      }
      else {
        FUN_00124394(param_2,auStack_28,uVar3);
      }
      for (iVar1 = *(int *)(iStack_3c + 0x1c); (iVar1 != 0 && (iVar1 != param_3));
          iVar1 = *(int *)(iVar1 + 0x20)) {
        if (iVar1 != 0) {
          aiStack_38[0] = FUN_0010e24c(iVar1);
          if (((*(byte *)(aiStack_38[0] + 0x28) & 2) == 0) ||
             (iVar2 = FUN_00114ef0(auStack_20,auStack_28,aiStack_38[0]), iVar2 == 0)) {
            FUN_0010e334(aiStack_38);
          }
          else {
            FUN_0010e334(aiStack_38);
            FUN_00124394(iVar1,auStack_20,uVar3);
            for (iVar2 = FUN_00117f58(iVar1); iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x20)) {
              FUN_00118250(iVar2,auStack_20,uVar3);
              iVar2 = FUN_0010e384(iVar2);
            }
          }
        }
        iVar1 = FUN_0010e384(iVar1);
      }
    }
    FUN_0010e334(&iStack_3c);
    return;
  }
  return;
}

// ==== FUN_001192f8 @ 001192f8, 348 bytes
// called by: FUN_001180e8, FUN_00117768, FUN_001192f8
void FUN_001192f8(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int local_3c;
  int local_38 [4];
  undefined1 auStack_28 [8];
  undefined1 auStack_20 [8];
  
  if (param_2 != 0) {
    local_3c = FUN_0010e24c(param_2);
    uVar3 = *(uint *)(local_3c + 0x28);
    if (((uVar3 & 2) != 0) && (iVar1 = FUN_00114ef0(auStack_28,param_1,local_3c), iVar1 != 0)) {
      if ((uVar3 & 0x801) == 1) {
        FUN_001192f8(auStack_28,*(undefined4 *)(local_3c + 0x18),*(undefined4 *)(local_3c + 0x20),
                     param_4);
      }
      else {
        FUN_00124394(param_2,auStack_28,param_4);
      }
      for (iVar1 = *(int *)(local_3c + 0x1c); (iVar1 != 0 && (iVar1 != param_3));
          iVar1 = *(int *)(iVar1 + 0x20)) {
        if (iVar1 != 0) {
          local_38[0] = FUN_0010e24c(iVar1);
          if (((*(byte *)(local_38[0] + 0x28) & 2) == 0) ||
             (iVar2 = FUN_00114ef0(auStack_20,auStack_28,local_38[0]), iVar2 == 0)) {
            FUN_0010e334(local_38);
          }
          else {
            FUN_0010e334(local_38);
            FUN_00124394(iVar1,auStack_20,param_4);
            for (iVar2 = FUN_00117f58(iVar1); iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x20)) {
              FUN_00118250(iVar2,auStack_20,param_4);
              iVar2 = FUN_0010e384(iVar2);
            }
          }
        }
        iVar1 = FUN_0010e384(iVar1);
      }
    }
    FUN_0010e334(&local_3c);
    return;
  }
  return;
}

// ==== FUN_00119458 @ 00119458, 8 bytes
// called by: FUN_00117424, FUN_001183e0
void FUN_00119458(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iStack_3c;
  int aiStack_38 [4];
  undefined1 auStack_28 [8];
  undefined1 auStack_20 [8];
  
  if (param_2 != 0) {
    iStack_3c = FUN_0010e24c(param_2);
    uVar3 = *(uint *)(iStack_3c + 0x28);
    if (((uVar3 & 2) != 0) && (iVar1 = FUN_00114ef0(auStack_28,param_1,iStack_3c), iVar1 != 0)) {
      if ((uVar3 & 0x801) == 1) {
        FUN_001192f8(auStack_28,*(undefined4 *)(iStack_3c + 0x18),*(undefined4 *)(iStack_3c + 0x20),
                     0x20);
      }
      else {
        FUN_00124394(param_2,auStack_28,0x20);
      }
      for (iVar1 = *(int *)(iStack_3c + 0x1c); (iVar1 != 0 && (iVar1 != param_3));
          iVar1 = *(int *)(iVar1 + 0x20)) {
        if (iVar1 != 0) {
          aiStack_38[0] = FUN_0010e24c(iVar1);
          if (((*(byte *)(aiStack_38[0] + 0x28) & 2) == 0) ||
             (iVar2 = FUN_00114ef0(auStack_20,auStack_28,aiStack_38[0]), iVar2 == 0)) {
            FUN_0010e334(aiStack_38);
          }
          else {
            FUN_0010e334(aiStack_38);
            FUN_00124394(iVar1,auStack_20,0x20);
            for (iVar2 = FUN_00117f58(iVar1); iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x20)) {
              FUN_00118250(iVar2,auStack_20,0x20);
              iVar2 = FUN_0010e384(iVar2);
            }
          }
        }
        iVar1 = FUN_0010e384(iVar1);
      }
    }
    FUN_0010e334(&iStack_3c);
    return;
  }
  return;
}

// ==== FUN_00119460 @ 00119460, 380 bytes
void FUN_00119460(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *unaff_r10;
  bool bVar5;
  undefined4 *puStack_78;
  int iStack_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  int *local_64;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  int *piStack_4c;
  int iStack_48;
  int iStack_44;
  undefined4 uStack_38;
  undefined4 uStack_34;
  int iStack_30;
  int iStack_2c;
  
  local_70 = 0;
  local_6c = 0;
  local_68 = 0;
  local_64 = (int *)0x0;
  if (param_1 == 0 || param_2 == 0 && param_3 == 0) {
    return;
  }
  puStack_78 = (undefined4 *)FUN_0010e24c(param_1);
  uStack_38 = *puStack_78;
  uStack_34 = puStack_78[1];
  FUN_001129e8(puStack_78,param_2,param_3);
  FUN_001129e8(puStack_78 + 2,param_2,param_3);
  iVar2 = puStack_78[7];
  FUN_0010e334(&puStack_78);
  bVar5 = iVar2 != 0;
  if (bVar5) {
    unaff_r10 = &iStack_48;
  }
  uStack_58 = 0;
  uStack_54 = 0;
  uStack_50 = 0;
  piStack_4c = (int *)0x0;
  while (bVar5) {
    iStack_74 = FUN_0010e24c(iVar2);
    FUN_001129e8(iStack_74,param_2,param_3);
    FUN_001129e8(iStack_74 + 8,param_2,param_3);
    uVar3 = *(undefined4 *)(iStack_74 + 0x1c);
    iVar4 = *(int *)(iStack_74 + 0x20);
    FUN_0010e334(&iStack_74);
    FUN_00124b64(uVar3,param_2,param_3);
    uStack_58 = 3;
    piStack_4c = unaff_r10;
    iStack_48 = param_2;
    iStack_44 = param_3;
    FUN_001198a4(iVar2,&uStack_58);
    bVar5 = iVar4 != 0;
    iVar2 = iVar4;
  }
  puStack_78 = (undefined4 *)FUN_0010e24c(param_1);
  uVar1 = (uint)*(byte *)(puStack_78 + 10);
  bVar5 = (*(byte *)(puStack_78 + 10) & 2) != 0;
  if (bVar5) {
    uVar1 = puStack_78[6];
  }
  if (bVar5 && uVar1 != 0) {
    FUN_001192bc(puStack_78,uVar1,puStack_78[8]);
    FUN_001192bc(&uStack_38,puStack_78[6],puStack_78[8]);
  }
  FUN_0010e334(&puStack_78);
  local_64 = &iStack_30;
  local_70 = 3;
  iStack_30 = param_2;
  iStack_2c = param_3;
  FUN_001198a4(param_1,&local_70);
  return;
}

// ==== FUN_001195dc @ 001195dc, 248 bytes
// called by: FUN_00111208, FUN_001196dc, FUN_00124e84
void FUN_001195dc(undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined1 *puStack_24;
  int local_20;
  
  local_20 = FUN_0010e24c();
  uVar2 = *(uint *)(local_20 + 0x28);
  local_38 = *(undefined4 *)(local_20 + 8);
  uStack_34 = *(undefined4 *)(local_20 + 0xc);
  iVar1 = *(int *)(local_20 + 0x10);
  FUN_0010e334(&local_20);
  if (iVar1 == 0 || (uVar2 & 2) == 0) {
    return;
  }
  thunk_FUN_0011990c(param_1,0x2e);
  DAT_001830b4 = DAT_001830b4 + '\x01';
  if ((uVar2 & 0x1000) == 0) {
    iVar1 = FUN_00118f24(&local_38);
    while (iVar1 != 0) {
      uStack_30 = 0xf;
      uStack_2c = param_1;
      puStack_24 = (undefined1 *)&local_38;
      FUN_0010d884();
      *(undefined4 *)(DAT_00182efc + 0x40) = 0;
      FUN_001198a4(param_1,&uStack_30);
      iVar1 = FUN_001189d8();
    }
  }
  else {
    uStack_30 = 0xf;
    uStack_2c = param_1;
    puStack_24 = (undefined1 *)&local_38;
    FUN_0010d884();
    *(undefined4 *)(DAT_00182efc + 0x40) = 0;
    FUN_001198a4(param_1,&uStack_30);
  }
  DAT_001830b4 = DAT_001830b4 + -1;
  thunk_FUN_0011990c(param_1,0x2f);
  return;
}

// ==== FUN_001196dc @ 001196dc, 436 bytes
// called by: FUN_001179fc, FUN_00126d70
void FUN_001196dc(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iStack_30;
  short *psStack_2c;
  int local_28;
  
  local_28 = FUN_0010e24c();
  if ((*(uint *)(local_28 + 0x28) & 0x801) != 1) {
    FUN_0010e334(&local_28);
    iVar1 = FUN_0010e384(param_1);
    if ((*(uint *)(iVar1 + 0x28) & 0x10000) == 0) {
      pcVar3 = FUN_001195dc;
    }
    else {
      pcVar3 = FUN_00111208;
    }
    (*pcVar3)(param_1);
  }
  if (DAT_001830dc != 0) {
    FUN_00124e84(param_1);
    iVar1 = FUN_0010e384(param_1);
    iVar4 = *(int *)(iVar1 + 0x18);
    iVar1 = *(int *)(iVar1 + 0x20);
    if (iVar4 == 0) {
      return;
    }
    do {
      for (; iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x20)) {
        iVar2 = FUN_0010e384(iVar1);
        if ((~*(uint *)(iVar2 + 0x28) & 3) == 0) {
          iStack_30 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
          psStack_2c = (short *)FUN_0010e24c(iVar1);
          iVar2 = FUN_00112b2c(iStack_30 + 8,psStack_2c);
          if (iVar2 == 0) {
            FUN_0010e334(&iStack_30);
            FUN_0010e334(&psStack_2c);
          }
          else {
            uVar6 = *(undefined4 *)(DAT_00182efc + 0x48);
            uVar5 = *(undefined4 *)(DAT_00182efc + 0x4c);
            FUN_00114ef0(psStack_2c + 4,psStack_2c,iStack_30 + 8);
            FUN_0010e334(&iStack_30);
            iVar2 = DAT_00182efc;
            DAT_001830e0 = iVar1;
            *(int *)(DAT_00182efc + 0x48) = (int)*psStack_2c;
            *(int *)(iVar2 + 0x4c) = (int)psStack_2c[1];
            FUN_0010e334(&psStack_2c);
            iVar2 = FUN_0010e384(iVar1);
            if ((*(uint *)(iVar2 + 0x28) & 0x10000) == 0) {
              pcVar3 = FUN_001195dc;
            }
            else {
              pcVar3 = FUN_00111208;
            }
            (*pcVar3)(iVar1);
            iVar2 = DAT_00182efc;
            DAT_001830e0 = 0;
            *(undefined4 *)(DAT_00182efc + 0x48) = uVar6;
            *(undefined4 *)(iVar2 + 0x4c) = uVar5;
          }
        }
        FUN_00124e84(iVar1);
        iVar1 = FUN_0010e384(iVar1);
      }
      iVar1 = FUN_0010e384(iVar4);
      iVar4 = *(int *)(iVar1 + 0x18);
      iVar1 = *(int *)(iVar1 + 0x20);
    } while (iVar4 != 0);
    return;
  }
  return;
}

// ==== FUN_001198a4 @ 001198a4, 264 bytes
// called by: FUN_001195dc, FUN_001174f0, FUN_0011837c, FUN_00124b64, FUN_0011995c, FUN_00119460, FUN_00118330, FUN_00117768
/* WARNING: Removing unreachable block (ram,0x001181a4) */

void FUN_001198a4(int param_1,int *param_2)

{
  int iVar1;
  code *pcVar2;
  int iVar3;
  undefined4 unaff_r4;
  undefined4 unaff_r5;
  uint uVar4;
  undefined4 *puStack_30;
  int iStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  
  uStack_10 = unaff_r4;
  uStack_c = unaff_r5;
  while( true ) {
    if (0x13 < DAT_001830fc) {
      return;
    }
    iVar1 = FUN_0010e384(param_1);
    uStack_14 = uStack_10;
    pcVar2 = *(code **)(iVar1 + 0x10);
    param_2[1] = param_1;
    if (pcVar2 != (code *)0x0) {
      DAT_001830fc = DAT_001830fc + 1;
      (*pcVar2)(param_2);
      DAT_001830fc = DAT_001830fc + -1;
      return;
    }
    iVar3 = *param_2;
    iVar1 = param_2[1];
    if (iVar3 == 0x17) goto LAB_00117744;
    if (0x17 < iVar3) break;
    if (iVar3 != 0xe) {
      if (iVar3 == 0x14) {
        FUN_00118978(iVar1,param_2[3]);
      }
      goto LAB_00117740;
    }
    if (param_2 == (int *)0x0 || iVar1 == 0) {
      return;
    }
    iVar3 = FUN_0010e384();
    param_1 = *(int *)(iVar3 + 0x18);
    if (param_1 == 0) {
      return;
    }
    param_2[2] = iVar1;
  }
  if (iVar3 == 0x22) {
    iVar1 = 0x12345678;
    goto LAB_00117744;
  }
  if (iVar3 == 0x28) {
    if (iVar1 == 0) {
      return;
    }
    uStack_10 = uStack_c;
    puStack_30 = (undefined4 *)FUN_0010e24c();
    uVar4 = puStack_30[10];
    if ((uVar4 & 2) != 0) {
      uStack_28 = *puStack_30;
      uStack_24 = puStack_30[1];
      if ((uVar4 & 0x10000) != 0) {
        iStack_2c = FUN_0010e24c(iVar1);
        if ((*(byte *)(iStack_2c + 0x28) & 2) != 0) {
          if ((*(int *)(iStack_2c + 0x24) != 0) && (DAT_001830e4 != iVar1)) {
            FUN_00110e78();
            *(undefined4 *)(iStack_2c + 0x24) = 0;
          }
          for (iVar3 = FUN_00117f58(iVar1); iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x20)) {
            FUN_00119230(iVar3);
            iVar3 = FUN_0010e384(iVar3);
          }
        }
        FUN_0010e334(&iStack_2c);
      }
      iVar3 = FUN_00118820(&uStack_28,iVar1);
      if (iVar3 != 0) {
        if ((uVar4 & 0x801) == 1) {
          FUN_001192f8(&uStack_28,puStack_30[6],puStack_30[8],0x20);
        }
        else {
          FUN_00124394(iVar1,&uStack_28,0x20);
        }
      }
    }
    FUN_0010e334(&puStack_30);
    return;
  }
LAB_00117740:
  iVar1 = 0;
LAB_00117744:
  param_2[3] = iVar1;
  return;
}

// ==== FUN_0011990c @ 0011990c, 80 bytes
// called by: FUN_00110ed8
void FUN_0011990c(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_20 = 0;
  local_1c = 0;
  local_18 = 0;
  local_14 = 0;
  iVar1 = FUN_0010e384(param_1);
  if (*(code **)(iVar1 + 0x10) != (code *)0x0) {
    local_20 = param_2;
    local_1c = param_1;
    (**(code **)(iVar1 + 0x10))(&local_20);
    return;
  }
  return;
}

// ==== FUN_0011995c @ 0011995c, 32 bytes
// called by: FUN_001183e0
void FUN_0011995c(undefined4 param_1,undefined4 param_2)

{
  undefined4 local_18;
  undefined4 local_14;
  
  local_18 = param_2;
  local_14 = param_1;
  FUN_001198a4(param_1,&local_18);
  return;
}

// ==== FUN_0011997c @ 0011997c, 500 bytes
// called by: FUN_001183e0
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_0011997c(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint *puStack_38;
  int iStack_34;
  int iStack_30;
  int iStack_2c;
  int iStack_28;
  int iVar2;
  
  uVar8 = *(uint *)(param_1 + 0x1c);
  iStack_34 = param_1;
  iStack_30 = param_2;
  iStack_2c = param_3;
  iStack_28 = param_4;
  if (uVar8 == 0) {
    return;
  }
  do {
    puStack_38 = (uint *)FUN_0010e24c(uVar8);
    uVar5 = *puStack_38;
    uVar7 = puStack_38[1];
    uVar3 = puStack_38[10] & 0x280;
    if (uVar3 == 0x80) {
      uVar3 = uVar5 & 0xffff0000 | uVar5 + iStack_28 & 0xffff;
LAB_00119b18:
      uVar6 = uVar7 & 0xffff0000 | uVar7 + iStack_28 & 0xffff;
    }
    else {
      if (uVar3 == 0x280) {
        uVar3 = uVar5 + iStack_30 & 0xffff | uVar5 & 0xffff0000;
        goto LAB_00119b18;
      }
      uVar3 = uVar5 & 0xffff0000 | uVar5 + iStack_30 & 0xffff;
      uVar6 = uVar7 & 0xffff0000 | uVar7 + iStack_30 & 0xffff;
    }
    uVar4 = puStack_38[10] & 0x500;
    iVar1 = (int)uVar3 >> 0x10;
    iVar2 = (int)uVar6 >> 0x10;
    if (uVar4 == 0x100) {
      uVar3 = uVar3 & 0xffff | (param_5 + iVar1) * 0x10000;
      uVar6 = uVar6 & 0xffff | (param_5 + iVar2) * 0x10000;
    }
    else if (uVar4 == 0x500) {
      uVar3 = uVar3 & 0xffff | (iVar1 + iStack_2c) * 0x10000;
      uVar6 = uVar6 & 0xffff | (iVar2 + param_5) * 0x10000;
    }
    else {
      uVar3 = uVar3 & 0xffff | (iStack_2c + iVar1) * 0x10000;
      uVar6 = uVar6 & 0xffff | (iStack_2c + iVar2) * 0x10000;
    }
    if ((int)(short)uVar3 != (int)(short)uVar5 || (int)uVar3 >> 0x10 != (int)uVar5 >> 0x10) {
      thunk_FUN_00119460(uVar8);
    }
    if ((int)(short)uVar6 - (int)(short)uVar3 != (int)(short)uVar7 - (int)(short)uVar5 ||
        ((int)uVar6 >> 0x10) - ((int)uVar3 >> 0x10) != ((int)uVar7 >> 0x10) - ((int)uVar5 >> 0x10))
    {
      FUN_001183e0(uVar8);
    }
    uVar8 = puStack_38[8];
    FUN_0010e334(&puStack_38);
    if (uVar8 == 0) {
      return;
    }
  } while( true );
}

// ==== FUN_00119b70 @ 00119b70, 24 bytes
void FUN_00119b70(void)

{
  FUN_001028b4();
  DAT_00182bcc = 1;
  return;
}

// ==== FUN_00119b8c @ 00119b8c, 296 bytes
// called by: FUN_0010e12c, FUN_00119cb8, FUN_0010e0a8, FUN_00121a30, FUN_00123b40, FUN_001257f8
uint FUN_00119b8c(uint param_1,int *param_2)

{
  uint uVar1;
  int *piVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  bool bVar6;
  
  piVar2 = (int *)((int)param_2 + param_1);
  piVar4 = (int *)((int)DAT_007bd014 + DAT_007bcfe8 + -1);
  bVar6 = piVar2 == piVar4;
  if (piVar2 <= piVar4) {
    bVar6 = *piVar2 == 0;
  }
  if (bVar6) {
    param_1 = param_1 + piVar2[1];
    FUN_0012576c();
  }
  if ((DAT_007bd014 < param_2) && (piVar2 = (int *)((int)param_2 - param_2[-1]), *piVar2 == 0)) {
    param_1 = param_1 + piVar2[1];
    FUN_0012576c(piVar2);
    param_2 = piVar2;
  }
  uVar3 = 0xf;
  do {
    if ((uint)(&DAT_007bd01c)[uVar3 * 2] <= param_1) break;
    if ((&DAT_007bd014)[uVar3 * 2] <= param_1) {
      uVar3 = uVar3 - 1;
      break;
    }
    uVar3 = uVar3 - 2;
  } while (uVar3 < 0x80000000);
  uVar1 = DAT_007bcfe4;
  if (uVar3 < 0x10) {
    piVar2 = (int *)(uVar3 * 8 + 0x7bd020);
    param_2[2] = *piVar2;
    param_2[4] = (int)param_2;
    *param_2 = 0;
    param_2[1] = param_1;
    *(uint *)((int)param_2 + (param_1 - 4)) = param_1;
    iVar5 = *piVar2;
    piVar4 = param_2 + 2;
    if (iVar5 != 0) {
      *(int **)(iVar5 + 4) = piVar4;
    }
    *piVar4 = iVar5;
    param_2[3] = 0;
    *piVar2 = (int)piVar4;
    DAT_007bcfdc = DAT_007bcfdc + param_1;
    uVar1 = DAT_007bcfe4;
    if ((DAT_007bd005 == '\0') &&
       (uVar1 = DAT_007bcfe8 - DAT_007bcfdc, (uint)(DAT_007bcfe8 - DAT_007bcfdc) < DAT_007bcfe4)) {
      uVar1 = DAT_007bcfe4;
    }
  }
  DAT_007bcfe4 = uVar1;
  return param_1;
}

// ==== FUN_00119cb8 @ 00119cb8, 792 bytes
// called by: FUN_00121a30, FUN_0010dfec
int FUN_00119cb8(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  int *piVar6;
  int *piVar7;
  int *piVar8;
  int *piVar9;
  uint uVar10;
  bool bVar11;
  
  FUN_00123b40();
  piVar4 = DAT_007bd01c;
  if (DAT_007bd01c <= param_1) {
    piVar4 = (int *)((int)param_1 + 3U & 0xfffffffc);
  }
  piVar4 = piVar4 + 3;
  if (piVar4 <= DAT_007bcfdc) {
    if (DAT_007bcfe0 <= DAT_007bd00c) {
      DAT_007bd00c = DAT_007bd00c - 1;
      iVar2 = FUN_00121a30();
      DAT_007bd00c = DAT_007bd00c + 1;
      if (iVar2 != 0) {
        return 0;
      }
    }
    iVar1 = DAT_007bd000;
    DAT_007bcfe0 = DAT_007bcfe0 - 1;
    iVar2 = DAT_007bd000 * 8;
    DAT_007bd000 = *(int *)(DAT_007bd010 + DAT_007bd000 * 8 + 4);
    *(undefined1 *)(DAT_007bd010 + iVar2) = 1;
    if (iVar1 != 0) {
LAB_00119d64:
      uVar5 = 0xf;
      do {
        if ((&DAT_007bd01c)[uVar5 * 2] <= piVar4) break;
        if ((&DAT_007bd014)[uVar5 * 2] <= piVar4) {
          uVar5 = uVar5 - 1;
          break;
        }
        uVar5 = uVar5 - 2;
      } while (uVar5 < 0x80000000);
      for (; uVar5 < 0x10; uVar5 = uVar5 + 1) {
        iVar2 = *(int *)(uVar5 * 8 + 0x7bd020);
        if (iVar2 != 0) {
          param_1 = *(int **)(iVar2 + 8);
        }
        if (iVar2 != 0 && param_1 != (int *)0x0) {
          piVar7 = (int *)param_1[1];
          while( true ) {
            if (piVar4 <= piVar7) {
              if ((int *)((int)piVar7 - (int)piVar4) < DAT_007bd01c + 3) {
                piVar4 = piVar7;
              }
              FUN_0012576c(param_1);
              *(int **)(DAT_007bd010 + iVar1 * 8 + 4) = param_1;
              *param_1 = iVar1;
              param_1[1] = (int)piVar4;
              iVar2 = 0;
              param_1[2] = 0;
              param_1[4] = (int)param_1;
              if ((int)piVar4 < (int)piVar7) {
                iVar2 = (int)piVar7 - (int)piVar4;
              }
              *(int **)((int)param_1 + (int)piVar4 + -4) = piVar4;
              if ((int)piVar4 < (int)piVar7) {
                FUN_00119b8c(iVar2);
              }
              return iVar1;
            }
            iVar2 = param_1[2];
            if (iVar2 != 0) {
              param_1 = *(int **)(iVar2 + 8);
            }
            if (iVar2 == 0 || param_1 == (int *)0x0) break;
            piVar7 = (int *)param_1[1];
          }
        }
      }
      piVar9 = (int *)((int)DAT_007bd014 + DAT_007bcfe8 + -1);
      piVar7 = DAT_007bd014;
      do {
        uVar5 = piVar7[1];
        if ((*piVar7 == 0) && (piVar8 = (int *)((int)piVar7 + uVar5), piVar8 < piVar9)) {
          iVar2 = *piVar8;
          uVar10 = piVar8[1];
          if ((iVar2 != 0) && (*(char *)(DAT_007bd010 + iVar2 * 8) == '\x01')) {
            bVar11 = iVar2 == DAT_007bd008;
            uVar3 = 0xf;
            do {
              if ((&DAT_007bd01c)[uVar3 * 2] <= uVar5) break;
              if ((&DAT_007bd014)[uVar3 * 2] <= uVar5) {
                uVar3 = uVar3 - 1;
                break;
              }
              uVar3 = uVar3 - 2;
            } while (uVar3 < 0x80000000);
            if (uVar3 < 0x10) {
              DAT_007bcfdc = (int *)((int)DAT_007bcfdc - uVar5);
              iVar2 = piVar7[2];
              piVar6 = (int *)piVar7[3];
              if (iVar2 != 0) {
                *(int **)(iVar2 + 4) = piVar6;
              }
              if (piVar6 == (int *)0x0) {
                *(int *)(uVar3 * 8 + 0x7bd020) = iVar2;
              }
              else {
                *piVar6 = iVar2;
              }
            }
            *(int **)(DAT_007bd010 + *piVar8 * 8 + 4) = piVar7;
            FUN_0010ca8c(piVar7,piVar8,uVar10);
            if (bVar11) {
              DAT_007bd010 = DAT_007bd010 - ((int)piVar8 - (int)piVar7);
            }
            piVar8 = (int *)FUN_00119b8c(uVar5,(int)piVar7 + uVar10);
            uVar5 = uVar10;
            if ((piVar4 != (int *)0x0) && (param_1 = (int *)&DAT_007bcfdc, piVar4 <= piVar8))
            goto LAB_00119d64;
          }
        }
        piVar7 = (int *)((int)piVar7 + uVar5);
        if (piVar9 <= piVar7) {
          DAT_007bcfe0 = DAT_007bcfe0 + 1;
          *(int *)(DAT_007bd010 + iVar1 * 8 + 4) = DAT_007bd000;
          *(undefined1 *)(DAT_007bd010 + iVar1 * 8) = 0;
          DAT_007bd000 = iVar1;
          return 0;
        }
      } while( true );
    }
  }
  return 0;
}

// ==== FUN_00119fd4 @ 00119fd4, 812 bytes
// called by: FUN_0010fff0, FUN_00123c60
void FUN_00119fd4(int param_1)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  byte *pbVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  byte *pbVar15;
  uint uVar16;
  int *piVar17;
  int *piVar18;
  int local_50 [9];
  int local_2c;
  int local_28;
  
  local_50[0] = 0;
  local_50[1] = 0;
  local_50[2] = 0;
  local_50[3] = 0;
  local_50[4] = 0;
  local_50[5] = 0;
  local_50[6] = 0;
  local_50[7] = 0;
  uVar9 = (uint)(byte)(&DAT_0000731c)[param_1];
  uVar13 = 0;
  uVar12 = 0;
  if (0 < (int)(uVar9 - 1)) {
    puVar4 = &DAT_0000731c;
    puVar5 = &DAT_00007320;
    if (((&DAT_0000731c)[param_1] & 1) == 0) {
      if ((byte)(&DAT_0000731d)[param_1] != 0) {
        uVar13 = (uint)(byte)(&DAT_0000731d)[param_1];
      }
      if ((byte)(&DAT_00007321)[param_1] != 0) {
        uVar12 = (uint)(byte)(&DAT_00007321)[param_1];
      }
      puVar5 = &DAT_00007321;
      puVar4 = &DAT_0000731d;
    }
    bVar1 = (puVar4 + param_1)[1];
    bVar2 = (puVar5 + param_1)[1];
    pbVar11 = puVar5 + param_1;
    pbVar15 = puVar4 + param_1;
    for (iVar10 = (int)(uVar9 - 1) >> 1; iVar10 != 0; iVar10 = iVar10 + -1) {
      uVar16 = (uint)pbVar15[2];
      uVar14 = (uint)pbVar11[2];
      if (uVar13 < bVar1) {
        uVar13 = (uint)bVar1;
      }
      if (uVar12 < bVar2) {
        uVar12 = (uint)bVar2;
      }
      bVar1 = pbVar15[3];
      bVar2 = pbVar11[3];
      if (uVar13 < uVar16) {
        uVar13 = uVar16;
      }
      if (uVar12 < uVar14) {
        uVar12 = uVar14;
      }
      pbVar11 = pbVar11 + 2;
      pbVar15 = pbVar15 + 2;
    }
  }
  if (-1 < (int)(uVar9 - 1)) {
    iVar10 = (uVar9 - 1) + param_1;
    if (uVar13 < (byte)(&DAT_0000731d)[iVar10]) {
      uVar13 = (uint)(byte)(&DAT_0000731d)[iVar10];
    }
    if (uVar12 < (byte)(&DAT_00007321)[iVar10]) {
      uVar12 = (uint)(byte)(&DAT_00007321)[iVar10];
    }
  }
  if (uVar9 != 0) {
    local_28 = uVar13 - 1;
    local_2c = uVar12 - 1;
    pbVar15 = &DAT_0000731d + param_1;
    pbVar11 = &DAT_00007321 + param_1;
    piVar17 = local_50 + 4;
    piVar18 = local_50;
    local_50[8] = uVar9;
    do {
      iVar10 = divide((uint)*(ushort *)(param_1 + 0x10) * (uint)*pbVar15 + local_28,uVar13);
      *piVar18 = (int)(iVar10 + 7 + ((uint)(iVar10 + 7 >> 0x1f) >> 0x1d)) >> 3;
      pbVar15 = pbVar15 + 1;
      iVar10 = divide((uint)*(ushort *)(param_1 + 0x12) * (uint)*pbVar11 + local_2c,uVar12);
      *piVar17 = (int)(iVar10 + 7 + ((uint)(iVar10 + 7 >> 0x1f) >> 0x1d)) >> 3;
      pbVar11 = pbVar11 + 1;
      local_50[8] = local_50[8] - 1;
      piVar17 = piVar17 + 1;
      piVar18 = piVar18 + 1;
    } while (local_50[8] != 0);
  }
  cVar3 = (&DAT_0000732d)[param_1];
  if (cVar3 != '\x01') {
    uVar6 = divide(uVar13 + (*(ushort *)(param_1 + 0x10) + 7 >> 3) + -1,uVar13);
    *(undefined4 *)(&DAT_00007440 + param_1) = uVar6;
    uVar6 = divide(uVar12 + (*(ushort *)(param_1 + 0x12) + 7 >> 3) + -1,uVar12);
    *(undefined4 *)(&DAT_00007448 + param_1) = 0;
    *(undefined4 *)(&DAT_00007444 + param_1) = uVar6;
    iVar10 = 0;
    if (cVar3 == '\0') {
      return;
    }
    do {
      uVar13 = (uint)(byte)(&DAT_0000732e)[param_1 + iVar10];
      uVar12 = (int)(short)(ushort)(byte)(&DAT_0000731d)[param_1 + uVar13] *
               (int)(short)(ushort)(byte)(&DAT_00007321)[param_1 + uVar13];
      if (uVar12 != 0) {
        iVar7 = 0;
        if (uVar12 == 0) {
          uVar9 = 0;
        }
        else {
          uVar9 = ~(uVar12 - 1) & 1;
          if (uVar9 != 0) {
            iVar7 = 1;
            *(uint *)(&DAT_0000744c + param_1 + *(int *)(&DAT_00007448 + param_1) * 4) = uVar13;
          }
        }
        if (uVar9 < uVar12) {
          do {
            uVar9 = uVar9 + 2;
            *(uint *)(&DAT_0000744c + param_1 + (*(int *)(&DAT_00007448 + param_1) + iVar7) * 4) =
                 uVar13;
            iVar8 = iVar7 + 1;
            iVar7 = iVar7 + 2;
            *(uint *)(&DAT_0000744c + param_1 + (*(int *)(&DAT_00007448 + param_1) + iVar8) * 4) =
                 uVar13;
          } while ((int)uVar9 < (int)uVar12);
        }
        *(uint *)(&DAT_00007448 + param_1) = *(int *)(&DAT_00007448 + param_1) + uVar12;
      }
      iVar10 = iVar10 + 1;
    } while (iVar10 < (int)(uint)(byte)(&DAT_0000732d)[param_1]);
    return;
  }
  uVar12 = (uint)(byte)(&DAT_0000732e)[param_1];
  *(int *)(&DAT_00007440 + param_1) = local_50[uVar12];
  *(int *)(&DAT_00007444 + param_1) = local_50[uVar12 + 4];
  *(undefined4 *)(&DAT_00007448 + param_1) = 1;
  *(uint *)(&DAT_0000744c + param_1) = uVar12;
  return;
}

// ==== FUN_0011a428 @ 0011a428, 160 bytes
int FUN_0011a428(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  
  iVar1 = FUN_00115d70();
  iVar7 = 0;
  uVar6 = 0xffffff;
  piVar2 = (int *)FUN_00115e28();
  if (piVar2 != (int *)0x0) {
    if (*piVar2 == param_1) {
      return 0;
    }
    iVar3 = 1;
    if (1 < iVar1) {
      iVar5 = 1;
      if (0 < iVar1 + -1) {
        do {
          if (piVar2[iVar3] == param_1) {
            return iVar3;
          }
          iVar5 = iVar5 + 1;
          iVar3 = iVar3 + 1;
        } while (iVar5 <= iVar1 + -1);
      }
    }
    iVar3 = 0;
    do {
      uVar4 = FUN_0010e3a4(param_1,piVar2[iVar3]);
      if (uVar4 < uVar6) {
        uVar6 = uVar4;
        iVar7 = iVar3;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < iVar1);
  }
  return iVar7;
}

// ==== FUN_0011a52c @ 0011a52c, 204 bytes
void FUN_0011a52c(int param_1,int param_2,int param_3)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  bool bVar6;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      return;
    }
  }
  piVar5 = *(int **)(param_1 + 8);
  if (param_2 != param_3) {
    iVar3 = (**(code **)(*(int *)(param_1 + 0xc) + 0x28))(param_1,8);
    iVar4 = piVar5[2];
    bVar6 = iVar4 == 0;
    if (bVar6) {
      iVar4 = piVar5[1];
    }
    uVar1 = (uint)(piVar5[5] * piVar5[6] * iVar3) >> 3;
    if (bVar6) {
      iVar3 = uVar1 * param_2 + iVar4;
      iVar4 = uVar1 * param_3 + iVar4;
    }
    else {
      iVar3 = *(int *)(iVar4 + param_2 * 4);
      iVar4 = *(int *)(iVar4 + param_3 * 4);
    }
    if ((code *)piVar5[0xe] == (code *)0x0) {
      (*DAT_00183074)(iVar4,iVar3);
    }
    else {
      (*(code *)piVar5[0xe])(*(undefined4 *)(param_1 + 0x18),param_2,param_3);
    }
    *piVar5 = iVar4;
    return;
  }
  return;
}

// ==== FUN_0011a5fc @ 0011a5fc, 480 bytes
undefined4 FUN_0011a5fc(int param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  bool bVar8;
  bool bVar9;
  int local_28;
  
  uVar7 = 0;
  if (*(int *)(&DAT_00007430 + param_1) != 0) {
    *(int *)(&DAT_00007430 + param_1) = *(int *)(&DAT_00007430 + param_1) + -1;
    return 0;
  }
  iVar3 = param_1 + param_2 * 0x10;
  iVar2 = *(int *)(&DAT_000073b8 + iVar3);
  bVar9 = SBORROW4(iVar2,param_3);
  iVar6 = iVar2 - param_3;
  bVar8 = iVar2 == param_3;
  if (param_3 < iVar2) {
    iVar2 = *(int *)(&DAT_000073bc + iVar3);
    bVar9 = SBORROW4(iVar2,param_4);
    iVar6 = iVar2 - param_4;
    bVar8 = iVar2 == param_4;
  }
  if (!bVar8 && iVar6 < 0 == bVar9) {
    iVar6 = *(int *)(&DAT_000073c0 + iVar3);
    local_28 = param_4;
    local_28 = FUN_0010e24c(*(undefined4 *)(&DAT_000073c4 + iVar3));
    iVar6 = *(int *)(&DAT_000073b8 + iVar3) * param_4 * iVar6 + param_3 * iVar6 + local_28;
    if (iVar6 != 0) {
      uVar5 = (uint)(byte)(&DAT_00007374)[param_1];
      if (uVar5 <= (byte)(&DAT_00007375)[param_1]) {
        do {
          iVar2 = param_1 + (uint)(byte)(&DAT_00007336)[param_1 + param_2] * 0xd00 + 0x8bc;
          uVar4 = *(uint *)(iVar2 + ((*(ushort *)(param_1 + 0x24) & 0xff00) >> 6));
          if ((int)uVar4 < 0) {
            FUN_00122c54(param_1,8);
            do {
              iVar3 = FUN_00122c54(param_1,1);
              uVar4 = *(uint *)(iVar2 + ((1 - iVar3) + ~uVar4) * 4 + 0x500);
            } while ((int)uVar4 < 0);
          }
          else {
            FUN_00122c54(param_1,*(undefined1 *)(iVar2 + uVar4 + 0x400));
          }
          uVar1 = (int)uVar4 >> 4;
          uVar4 = uVar4 & 0xf;
          if (uVar4 == 0) {
            if (uVar1 != 0xf) {
              iVar6 = 1 << (uVar1 & 0xff);
              *(int *)(&DAT_00007430 + param_1) = iVar6;
              if (uVar1 != 0) {
                iVar6 = FUN_00122c54(param_1);
                iVar6 = iVar6 + *(int *)(&DAT_00007430 + param_1);
                *(int *)(&DAT_00007430 + param_1) = iVar6;
              }
              *(int *)(&DAT_00007430 + param_1) = iVar6 + -1;
              break;
            }
            iVar2 = uVar5 + 0xf;
            if (0x3f < iVar2) goto LAB_0011a720;
          }
          else {
            iVar2 = uVar5 + uVar1;
            if (0x3f < iVar2) {
LAB_0011a720:
              uVar7 = 1;
              break;
            }
            iVar3 = FUN_00122c54(param_1,uVar4);
            if (iVar3 < *(int *)(&DAT_001815a8 + uVar4 * 4)) {
              iVar3 = iVar3 + *(int *)(&DAT_001815e8 + uVar4 * 4);
            }
            *(short *)(iVar6 + (uint)(byte)(&DAT_00181558)[iVar2] * 2) =
                 (short)(iVar3 << (&DAT_00007376)[param_1]);
          }
          uVar5 = iVar2 + 1;
        } while ((int)uVar5 <= (int)(uint)(byte)(&DAT_00007375)[param_1]);
      }
      FUN_0010e334(&local_28);
      return uVar7;
    }
  }
  return 1;
}

// ==== FUN_0011a7e8 @ 0011a7e8, 720 bytes
undefined4 FUN_0011a7e8(int param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  short *psVar11;
  uint uVar12;
  int iVar13;
  bool bVar14;
  bool bVar15;
  undefined4 local_44;
  int local_40;
  int local_3c;
  int local_34;
  int iStack_30;
  int iStack_2c;
  int iStack_28;
  
  uVar1 = 1 << (&DAT_00007376)[param_1];
  uVar2 = -1 << (&DAT_00007376)[param_1];
  local_44 = 0;
  iVar8 = param_1 + param_2 * 0x10;
  iVar6 = *(int *)(&DAT_000073b8 + iVar8);
  bVar15 = SBORROW4(iVar6,param_3);
  iVar13 = iVar6 - param_3;
  bVar14 = iVar6 == param_3;
  if (param_3 < iVar6) {
    iVar6 = *(int *)(&DAT_000073bc + iVar8);
    bVar15 = SBORROW4(iVar6,param_4);
    iVar13 = iVar6 - param_4;
    bVar14 = iVar6 == param_4;
  }
  if (bVar14 || iVar13 < 0 != bVar15) {
    return 1;
  }
  iVar13 = *(int *)(&DAT_000073c0 + iVar8);
  local_34 = param_1;
  iStack_30 = param_2;
  iStack_2c = param_3;
  iStack_28 = param_4;
  local_40 = FUN_0010e24c(*(undefined4 *)(&DAT_000073c4 + iVar8));
  uVar9 = (uint)(byte)(&DAT_00007374)[param_1];
  iVar13 = *(int *)(&DAT_000073b8 + iVar8) * param_4 * iVar13 + param_3 * iVar13 + local_40;
  if (iVar13 == 0) {
    return 1;
  }
  sVar4 = (short)uVar2;
  if (*(int *)(&DAT_00007430 + param_1) == 0) {
    if ((byte)(&DAT_00007375)[param_1] < uVar9) goto LAB_0011aa94;
    local_3c = local_34 + param_2 + 0x7000;
    do {
      iVar6 = local_34;
      iVar8 = local_34 + (uint)*(byte *)(local_3c + 0x336) * 0xd00 + 0x8bc;
      uVar10 = *(uint *)(iVar8 + ((*(ushort *)(local_34 + 0x24) & 0xff00) >> 6));
      if ((int)uVar10 < 0) {
        FUN_00122c54(local_34,8);
        do {
          iVar7 = FUN_00122c54(iVar6,1);
          uVar10 = *(uint *)(iVar8 + ((1 - iVar7) + ~uVar10) * 4 + 0x500);
        } while ((int)uVar10 < 0);
      }
      else {
        FUN_00122c54(local_34,*(undefined1 *)(iVar8 + uVar10 + 0x400));
      }
      uVar12 = (int)uVar10 >> 4;
      if ((uVar10 & 0xf) == 0) {
        uVar10 = 0;
        if (uVar12 != 0xf) {
          *(int *)(&DAT_00007430 + param_1) = 1 << (uVar12 & 0xff);
          if (uVar12 != 0) {
            iVar6 = FUN_00122c54(local_34,uVar12);
            *(int *)(&DAT_00007430 + param_1) = iVar6 + *(int *)(&DAT_00007430 + param_1);
          }
          break;
        }
      }
      else {
        if ((uVar10 & 0xf) != 1) {
          local_44 = 1;
          goto LAB_0011aa94;
        }
        iVar6 = FUN_00122c54(local_34,1);
        uVar10 = uVar2;
        if (iVar6 != 0) {
          uVar10 = uVar1;
        }
      }
      do {
        psVar11 = (short *)(iVar13 + (uint)(byte)(&DAT_00181558)[uVar9] * 2);
        if (*psVar11 == 0) {
          uVar12 = uVar12 - 1;
          if ((int)uVar12 < 0) break;
        }
        else {
          iVar6 = FUN_00122c54(local_34,1);
          if (iVar6 != 0) {
            sVar3 = *psVar11;
            if (((int)sVar3 & uVar1) == 0) {
              sVar5 = sVar4;
              if (-1 < sVar3) {
                sVar5 = (short)uVar1;
              }
              *psVar11 = sVar3 + sVar5;
            }
          }
        }
        uVar9 = uVar9 + 1;
      } while ((int)uVar9 <= (int)(uint)(byte)(&DAT_00007375)[param_1]);
      iVar6 = 0;
      if (uVar10 != 0) {
        iVar6 = uVar9 - 0x40;
      }
      if (iVar6 < 0 != (uVar10 != 0 && SBORROW4(uVar9,0x40))) {
        *(short *)(iVar13 + (uint)(byte)(&DAT_00181558)[uVar9] * 2) = (short)uVar10;
      }
      uVar9 = uVar9 + 1;
    } while ((int)uVar9 <= (int)(uint)(byte)(&DAT_00007375)[param_1]);
  }
  if (0 < *(int *)(&DAT_00007430 + param_1)) {
    if ((int)uVar9 <= (int)(uint)(byte)(&DAT_00007375)[param_1]) {
      do {
        psVar11 = (short *)(iVar13 + (uint)(byte)(&DAT_00181558)[uVar9] * 2);
        if ((*psVar11 != 0) && (iVar6 = FUN_00122c54(local_34,1), iVar6 != 0)) {
          sVar3 = *psVar11;
          if (((int)sVar3 & uVar1) == 0) {
            sVar5 = sVar4;
            if (-1 < sVar3) {
              sVar5 = (short)uVar1;
            }
            *psVar11 = sVar3 + sVar5;
          }
        }
        uVar9 = uVar9 + 1;
      } while ((int)uVar9 <= (int)(uint)(byte)(&DAT_00007375)[param_1]);
    }
    *(int *)(&DAT_00007430 + param_1) = *(int *)(&DAT_00007430 + param_1) + -1;
  }
LAB_0011aa94:
  FUN_0010e334(&local_40);
  return local_44;
}

// ==== FUN_0011aabc @ 0011aabc, 332 bytes
// called by: FUN_0010fff0
undefined4 FUN_0011aabc(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined2 *puVar4;
  int iVar5;
  bool bVar6;
  bool bVar7;
  int local_28;
  
  iVar2 = param_1 + param_2 * 0x10;
  iVar1 = *(int *)(&DAT_00007378 + iVar2);
  bVar7 = SBORROW4(iVar1,param_3);
  iVar5 = iVar1 - param_3;
  bVar6 = iVar1 == param_3;
  if (param_3 < iVar1) {
    iVar1 = *(int *)(&DAT_0000737c + iVar2);
    bVar7 = SBORROW4(iVar1,param_4);
    iVar5 = iVar1 - param_4;
    bVar6 = iVar1 == param_4;
  }
  if (!bVar6 && iVar5 < 0 == bVar7) {
    iVar5 = *(int *)(&DAT_00007380 + iVar2);
    local_28 = param_4;
    local_28 = FUN_0010e24c(*(undefined4 *)(&DAT_00007384 + iVar2));
    puVar4 = (undefined2 *)
             (*(int *)(&DAT_00007378 + iVar2) * param_4 * iVar5 + param_3 * iVar5 + local_28);
    if (puVar4 != (undefined2 *)0x0) {
      iVar5 = param_1 + (uint)(byte)(&DAT_00007332)[param_1 + param_2] * 0xd00 + 0x8bc;
      uVar3 = *(uint *)(iVar5 + ((*(ushort *)(param_1 + 0x24) & 0xff00) >> 6));
      if ((int)uVar3 < 0) {
        FUN_00122c54(param_1,8);
        do {
          iVar1 = FUN_00122c54(param_1,1);
          uVar3 = *(uint *)(iVar5 + ((1 - iVar1) + ~uVar3) * 4 + 0x500);
        } while ((int)uVar3 < 0);
      }
      else {
        FUN_00122c54(param_1,*(undefined1 *)(iVar5 + uVar3 + 0x400));
      }
      iVar5 = 0;
      if ((uVar3 != 0) &&
         (iVar5 = FUN_00122c54(param_1,uVar3), iVar5 < *(int *)(&DAT_001815a8 + uVar3 * 4))) {
        iVar5 = iVar5 + *(int *)(&DAT_001815e8 + uVar3 * 4);
      }
      iVar1 = param_1 + param_2 * 4;
      iVar2 = *(int *)(&DAT_00007364 + iVar1);
      *(int *)(&DAT_00007364 + iVar1) = iVar2 + iVar5;
      *puVar4 = (short)(iVar2 + iVar5 << (uint)(byte)(&DAT_00007376)[param_1]);
      FUN_0010e334(&local_28);
      return 0;
    }
  }
  return 1;
}

// ==== FUN_0011ac14 @ 0011ac14, 160 bytes
undefined4 FUN_0011ac14(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  ushort *puVar3;
  int iVar4;
  undefined4 uVar5;
  bool bVar6;
  bool bVar7;
  int local_20;
  
  uVar5 = 0;
  local_20 = param_4;
  iVar1 = FUN_00122c54(param_1,1);
  if (iVar1 != 0) {
    iVar4 = param_1 + param_2 * 0x10;
    iVar2 = *(int *)(&DAT_00007378 + iVar4);
    bVar7 = SBORROW4(iVar2,param_3);
    iVar1 = iVar2 - param_3;
    bVar6 = iVar2 == param_3;
    if (param_3 < iVar2) {
      iVar2 = *(int *)(&DAT_0000737c + iVar4);
      bVar7 = SBORROW4(iVar2,param_4);
      iVar1 = iVar2 - param_4;
      bVar6 = iVar2 == param_4;
    }
    if (!bVar6 && iVar1 < 0 == bVar7) {
      iVar1 = *(int *)(&DAT_00007380 + iVar4);
      local_20 = FUN_0010e24c(*(undefined4 *)(&DAT_00007384 + iVar4));
      puVar3 = (ushort *)
               (param_4 * iVar1 * *(int *)(&DAT_00007378 + iVar4) + param_3 * iVar1 + local_20);
      if (puVar3 != (ushort *)0x0) {
        *puVar3 = *puVar3 | (ushort)(1 << (uint)(byte)(&DAT_00007376)[param_1]);
        FUN_0010e334(&local_20);
        return 0;
      }
    }
    uVar5 = 1;
  }
  return uVar5;
}

// ==== FUN_0011acb8 @ 0011acb8, 1536 bytes
// called by: FUN_0010f030
undefined4 FUN_0011acb8(undefined4 *param_1)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  int iVar7;
  short *psVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  short *psVar12;
  int iVar13;
  uint uVar14;
  byte local_64 [4];
  int local_60;
  int local_50 [2];
  undefined4 *local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  
  local_40 = 0;
  local_48 = param_1 + 0x1c00;
  local_50[0] = FUN_0010e24c(param_1[0x1d1e]);
  local_60 = 0;
  local_44 = 0;
  if ((int)local_48[0x110] < 1) {
LAB_0011b280:
    FUN_0010e334(local_50);
    return 0;
  }
LAB_0011acfc:
  if ((local_48[0x10d] != 0) && (local_48[0x10e] == 0)) {
    iVar7 = 2000;
    do {
      iVar4 = (*(code *)*param_1)(param_1,local_64);
      if (iVar4 != 0) break;
      if (local_64[0] == 0xff) {
        if (iVar7 != 0) {
          if (iVar7 < 1) goto LAB_0011ad94;
          goto LAB_0011ad5c;
        }
        break;
      }
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  goto LAB_0011ae10;
  while ((local_64[0] == 0xff && (iVar7 = iVar7 + -1, 0 < iVar7))) {
LAB_0011ad5c:
    iVar4 = (*(code *)*param_1)(param_1,local_64);
    if (iVar4 != 0) goto LAB_0011ae10;
  }
  if (iVar7 != 0) {
LAB_0011ad94:
    if (param_1[0x1d0f] + 0xd0 == (uint)local_64[0]) {
      (*DAT_00183070)(param_1 + 0x1cd9,0,(uint)*(byte *)(param_1 + 0x1cc7) << 2);
      param_1[0x1d0c] = 0;
      param_1[0x1d0e] = param_1[0x1d0d];
      param_1[0x1d0f] = *(byte *)(param_1 + 0x1d0f) + 1 & 7;
      param_1[10] = 0x10;
      FUN_00122c54(param_1,0x10);
      FUN_00122c54(param_1,0x10);
    }
  }
LAB_0011ae10:
  local_38 = 0;
  while ((local_38 < (int)local_48[0x112] && (local_40 == 0))) {
    iVar11 = param_1[local_38 + 0x1d13];
    local_3c = FUN_0010e24c(local_48[0x11d]);
    iVar4 = (int)param_1 + iVar11;
    psVar8 = (short *)(local_3c + local_60 * 0x80);
    bVar1 = (&DAT_00007332)[iVar4];
    psVar12 = (short *)param_1[(byte)(&DAT_00007325)[iVar4] + 0x1cc3];
    iVar7 = (uint)bVar1 * 0xd00 + 0x8bc;
    uVar9 = *(uint *)((int)param_1 + ((*(ushort *)(param_1 + 9) & 0xff00) >> 6) + iVar7);
    if ((int)uVar9 < 0) {
      FUN_00122c54(param_1,8);
      do {
        iVar7 = FUN_00122c54(param_1,1);
        uVar9 = param_1[(uint)bVar1 * 0x340 + (~uVar9 - iVar7) + 0x370];
      } while ((int)uVar9 < 0);
    }
    else {
      FUN_00122c54(param_1,*(undefined1 *)((int)param_1 + uVar9 + iVar7 + 0x400));
    }
    iVar7 = 0;
    if ((uVar9 != 0) &&
       (iVar7 = FUN_00122c54(param_1,uVar9), iVar7 < *(int *)(&DAT_001815a8 + uVar9 * 4))) {
      iVar7 = iVar7 + *(int *)(&DAT_001815e8 + uVar9 * 4);
    }
    iVar5 = param_1[iVar11 + 0x1cd9];
    iVar13 = 1;
    param_1[iVar11 + 0x1cd9] = iVar5 + iVar7;
    *psVar8 = *psVar12 * (short)(iVar5 + iVar7);
    bVar1 = (&DAT_00007336)[iVar4];
    uVar9 = (uint)*(byte *)(local_50[0] + local_60);
    iVar7 = (uint)bVar1 * 0xd00 + 0x8bc;
    do {
      uVar10 = *(uint *)((int)param_1 + ((*(ushort *)(param_1 + 9) & 0xff00) >> 6) + iVar7);
      if ((int)uVar10 < 0) {
        FUN_00122c54(param_1,8);
        do {
          iVar4 = FUN_00122c54(param_1,1);
          uVar10 = param_1[(uint)bVar1 * 0x340 + (~uVar10 - iVar4) + 0x370];
        } while ((int)uVar10 < 0);
      }
      else {
        FUN_00122c54(param_1,*(undefined1 *)((int)param_1 + uVar10 + iVar7 + 0x400));
      }
      iVar4 = (int)uVar10 >> 4;
      uVar10 = uVar10 & 0xf;
      if (uVar10 == 0) {
        if (iVar4 != 0xf) break;
        if (0x3f < iVar13 + 0xf) {
LAB_0011b0bc:
          local_40 = 1;
          goto LAB_0011b21c;
        }
        if (iVar13 < (int)uVar9) {
          iVar4 = uVar9 - iVar13;
          if (0x10 < iVar4) {
            iVar4 = 0x10;
          }
          uVar10 = iVar4 - 1;
          if (0 < (int)uVar10) {
            pbVar6 = &DAT_00181557 + iVar13;
            if ((uVar10 & 1) != 0) {
              psVar8[(byte)(&DAT_00181558)[iVar13]] = 0;
              pbVar6 = &DAT_00181558 + iVar13;
            }
            bVar2 = pbVar6[1];
            for (iVar4 = (int)uVar10 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
              bVar3 = pbVar6[2];
              psVar8[bVar2] = 0;
              bVar2 = pbVar6[3];
              psVar8[bVar3] = 0;
              pbVar6 = pbVar6 + 2;
            }
          }
          if (-1 < (int)uVar10) {
            psVar8[(byte)(&DAT_00181558)[iVar13 + uVar10]] = 0;
          }
        }
        iVar11 = iVar13 + 0xf;
      }
      else {
        iVar11 = iVar13;
        if (iVar4 != 0) {
          iVar11 = iVar13 + iVar4;
          if (0x3f < iVar11) goto LAB_0011b0bc;
          if (iVar13 < (int)uVar9) {
            if ((int)(uVar9 - iVar13) <= iVar4) {
              iVar4 = uVar9 - iVar13;
            }
            uVar14 = iVar4 - 1;
            if (0 < (int)uVar14) {
              pbVar6 = &DAT_00181557 + iVar13;
              if ((uVar14 & 1) != 0) {
                psVar8[(byte)(&DAT_00181558)[iVar13]] = 0;
                pbVar6 = &DAT_00181558 + iVar13;
              }
              bVar2 = pbVar6[1];
              for (iVar4 = (int)uVar14 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
                bVar3 = pbVar6[2];
                psVar8[bVar2] = 0;
                bVar2 = pbVar6[3];
                psVar8[bVar3] = 0;
                pbVar6 = pbVar6 + 2;
              }
            }
            if (-1 < (int)uVar14) {
              psVar8[(byte)(&DAT_00181558)[iVar13 + uVar14]] = 0;
            }
          }
        }
        iVar4 = FUN_00122c54(param_1,uVar10);
        if (iVar4 < *(int *)(&DAT_001815a8 + uVar10 * 4)) {
          iVar4 = iVar4 + *(int *)(&DAT_001815e8 + uVar10 * 4);
        }
        psVar8[(byte)(&DAT_00181558)[iVar11]] = psVar12[iVar11] * (short)iVar4;
      }
      iVar13 = iVar11 + 1;
    } while (iVar13 < 0x40);
    if (iVar13 < (int)uVar9) {
      iVar7 = iVar13;
      if (iVar13 < (int)uVar9) {
        uVar9 = uVar9 - iVar13;
        if (0 < (int)(uVar9 - 1)) {
          pbVar6 = &DAT_00181557 + iVar13;
          if ((uVar9 & 1) == 0) {
            psVar8[(byte)(&DAT_00181558)[iVar13]] = 0;
            pbVar6 = &DAT_00181558 + iVar13;
          }
          bVar1 = pbVar6[1];
          for (iVar7 = (int)(uVar9 - 1) >> 1; iVar7 != 0; iVar7 = iVar7 + -1) {
            bVar2 = pbVar6[2];
            psVar8[bVar1] = 0;
            bVar1 = pbVar6[3];
            psVar8[bVar2] = 0;
            pbVar6 = pbVar6 + 2;
          }
        }
        if (-1 < (int)(uVar9 - 1)) {
          psVar8[(byte)(&DAT_00181558)[iVar13 + (uVar9 - 1)]] = 0;
        }
      }
      else {
        do {
          iVar4 = iVar7 + 1;
          psVar8[(byte)(&DAT_00181558)[iVar7]] = 0;
          iVar7 = iVar4;
        } while (iVar4 < (int)uVar9);
      }
    }
    *(char *)(local_50[0] + local_60) = (char)iVar13;
    local_60 = local_60 + 1;
LAB_0011b21c:
    FUN_0010e334(&local_3c);
    local_38 = local_38 + 1;
  }
  local_44 = local_44 + 1;
  local_48[0x10e] = local_48[0x10e] + -1;
  if ((int)local_48[0x110] <= local_44) goto LAB_0011b280;
  goto LAB_0011acfc;
}

// ==== FUN_0011b2c8 @ 0011b2c8, 32 bytes
// called by: FUN_00117768
void FUN_0011b2c8(int param_1)

{
  do {
    FUN_00117768(*(undefined4 *)(param_1 + 0x1c));
  } while (*(int *)(param_1 + 0x1c) != 0);
  return;
}

// ==== FUN_0011b2e8 @ 0011b2e8, 1684 bytes
// called by: jpeg_draw
/* WARNING: Restarted to delay deadcode elimination for space: stack */

int FUN_0011b2e8(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  undefined **ppuVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  uint *puVar12;
  undefined4 *puVar13;
  uint unaff_r10;
  int local_90;
  undefined *puStack_8c;
  undefined **ppuStack_88;
  undefined4 uStack_84;
  uint *puStack_80;
  undefined4 uStack_7c;
  uint *apuStack_78 [2];
  uint uStack_70;
  int local_6c;
  int iStack_68;
  int iStack_5c;
  short sStack_58;
  short sStack_56;
  short sStack_54;
  short sStack_52;
  undefined4 local_50;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  int iStack_28;
  
  local_34 = param_1;
  local_30 = param_2;
  local_2c = param_3;
  iStack_28 = param_4;
  local_6c = FUN_0010e24c(param_1);
  local_50 = FUN_00115ce0();
  *(undefined4 *)(local_6c + 8) = local_2c;
  *(undefined4 *)(local_6c + 4) = local_30;
  FUN_0010e334(&local_6c);
  uVar6 = local_34;
  iVar3 = FUN_0011091c(local_34);
  local_90 = FUN_0010e24c(uVar6);
  sVar1 = *(short *)(local_90 + 0x10);
  sVar2 = *(short *)(local_90 + 0x12);
  FUN_0010e334(&local_90);
  if (iVar3 != 0) {
    return 1;
  }
  iVar3 = 1;
  iStack_28 = *(int *)(DAT_00182efc + 0x48) + param_4;
  param_5 = *(int *)(DAT_00182efc + 0x4c) + param_5;
  sStack_58 = (short)iStack_28;
  sStack_56 = (short)param_5;
  sStack_54 = sVar1 + sStack_58 + -1;
  sStack_52 = sStack_56 + -1 + sVar2;
  iVar4 = FUN_00118f24(&sStack_58);
  do {
    if (iVar4 == 0) {
      FUN_00116370(local_50);
      FUN_00116aa8();
      return iVar3;
    }
    local_6c = FUN_0010e24c(local_34);
    (*DAT_00183070)(local_6c,0,&DAT_00008498);
    *(undefined4 *)(local_6c + 8) = local_2c;
    *(undefined4 *)(local_6c + 4) = local_30;
    FUN_0010e334(&local_6c);
    iVar3 = FUN_0010fff0(local_34);
    iVar4 = FUN_0010e24c(local_34);
    puVar13 = *(undefined4 **)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10);
    local_6c = iVar4;
    if (iVar3 == 0) {
      iStack_48 = iStack_28;
      iStack_5c = param_5;
      DAT_00182f70 = FUN_00115c40();
      if (DAT_00182f70 == 0x18) {
        DAT_00182f70 = 0x20;
      }
      iVar3 = FUN_00116064();
      iVar5 = FUN_0011615c();
      iVar10 = (uint)*(ushort *)(iVar4 + 0x12) * (uint)*(ushort *)(iVar4 + 0x10);
      if ((iVar10 - iVar3 * iVar5 != 0 && iVar3 * iVar5 <= iVar10) ||
         (DAT_00182f6c = 1, 0x31fe0 < DAT_00182f78)) {
        DAT_00182f6c = 0;
      }
      if (DAT_00182f6c != 0) {
        if (DAT_00182f70 == 0x20) {
          iVar3 = iVar3 * 4;
        }
        else {
          iVar3 = iVar3 * 2;
        }
        local_90 = 1;
        puStack_8c = &DAT_00182338;
        ppuStack_88 = &PTR_LAB_00181e58;
        iStack_44 = FUN_00110e38(0,0,iVar3 + 1,iVar5);
        if (iStack_44 == 0) {
          DAT_00182f6c = 0;
        }
        else {
          iVar3 = FUN_00111114();
          DAT_00182f64 = (uint *)(iVar3 + 0x1fU & 0xffffffe0 | 0x80000000);
          local_90 = 1;
          puStack_8c = &DAT_00182338;
          ppuStack_88 = &PTR_LAB_00181e58;
          DAT_00182f68 = DAT_00182f64;
          iStack_40 = FUN_00110e38(0,0,200,0x400);
          if (iStack_40 == 0) {
            FUN_00110e78(iStack_44);
            DAT_00182f6c = 0;
          }
          else {
            iVar3 = FUN_00111114();
            unaff_r10 = iVar3 + 0x1fU & 0xffffffe0;
            (*DAT_00183074)(unaff_r10,DAT_00182f74,DAT_00182f78);
          }
        }
      }
      iStack_3c = 0;
      if (DAT_00182f6c != 0) {
        FUN_00105eb8();
        FUN_001056b4();
        FUN_001065d4(4,unaff_r10 | 0x80000000,0);
        if (DAT_00182f70 == 0x20) {
          FUN_001065d4(8,&DAT_00014021,0);
        }
        else {
          FUN_001065d4(8,&DAT_00006021,0);
        }
        FUN_001065d4(0xf,*(undefined2 *)(iVar4 + 0x12),*(undefined2 *)(iVar4 + 0x10));
        FUN_001065d4(0xe,*(undefined2 *)(iVar4 + 0x10),0);
        FUN_001065d4(0,DAT_00182f64);
        FUN_001065d4(0xc,0);
        iVar3 = FUN_00105e4c();
        if (iVar3 == 0) {
          FUN_00110e78(iStack_44);
          FUN_00110e78(iStack_40);
          DAT_00182f6c = 0;
        }
        FUN_00106924();
      }
      unaff_r10 = 0;
      if (*(short *)(iVar4 + 0x12) != 0) {
        do {
          if ((DAT_00182f6c == 0) && (iVar3 = FUN_0010f030(iVar4), iVar3 != 0)) {
            iStack_3c = 1;
            break;
          }
          iVar3 = iStack_48;
          iStack_68 = iStack_5c + unaff_r10;
          if ((&DAT_00007494)[iVar4] == '\0') {
            apuStack_78[0] = (uint *)FUN_0010e24c(*(undefined4 *)(&DAT_0000748c + iVar4));
          }
          else {
            apuStack_78[0] = (uint *)FUN_0010e24c(*(undefined4 *)(&DAT_00007490 + iVar4));
          }
          if ((&DAT_0000740a)[iVar4] == '\0') {
            puVar7 = apuStack_78[0];
            for (uVar11 = (uint)*(ushort *)(iVar4 + 0x10); uVar11 != 0; uVar11 = uVar11 - 1) {
              uVar6 = (*(code *)*puVar13)((uint)(byte)*puVar7 * 0x10101);
              FUN_00116c9c(iVar3,iStack_68,uVar6);
              puVar7 = (uint *)((int)puVar7 + 1);
              iVar3 = iVar3 + 1;
            }
          }
          else {
            uVar11 = (uint)*(ushort *)(iVar4 + 0x10);
            if (DAT_00182f6c == 0) {
              if (uVar11 < 2) {
                uVar11 = 1;
              }
              puVar7 = apuStack_78[0] + -1;
              if ((uVar11 & 1) != 0) {
                *apuStack_78[0] = *apuStack_78[0] | 0xff000000;
                puVar7 = apuStack_78[0];
              }
              for (iVar5 = (int)uVar11 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
                puVar7[2] = puVar7[2] | 0xff000000;
                puVar7[1] = puVar7[1] | 0xff000000;
                puVar7 = puVar7 + 2;
              }
            }
            else if (DAT_00182f70 == 0x20) {
              if (uVar11 < 2) {
                uVar11 = 1;
              }
              iVar5 = 1;
              puVar7 = apuStack_78[0];
              puVar12 = DAT_00182f68;
              do {
                DAT_00182f68 = puVar12 + 1;
                iVar5 = iVar5 + 1;
                *puVar7 = *puVar12;
                puVar7 = puVar7 + 1;
                puVar12 = DAT_00182f68;
              } while (iVar5 <= (int)uVar11);
            }
            else {
              if (uVar11 < 2) {
                uVar11 = 1;
              }
              iVar5 = 1;
              puVar7 = apuStack_78[0];
              puVar12 = DAT_00182f64;
              do {
                DAT_00182f64 = (uint *)((int)puVar12 + 2);
                iVar5 = iVar5 + 1;
                *(short *)puVar7 = (short)*puVar12;
                puVar7 = (uint *)((int)puVar7 + 2);
                puVar12 = DAT_00182f64;
              } while (iVar5 <= (int)uVar11);
            }
            uStack_70 = (uint)*(ushort *)(iVar4 + 0x10);
            ppuVar8 = (undefined **)FUN_0010eb64(*(undefined1 *)(DAT_00182efc + 0x11));
            if (ppuVar8 == (undefined **)0x18) {
              ppuVar8 = (undefined **)0x20;
            }
            if ((DAT_00182f6c == 0) &&
               (iVar5 = FUN_00114150(apuStack_78[0],uStack_70,ppuVar8,puVar13), iVar5 == 0)) {
              puVar7 = apuStack_78[0];
              uVar11 = uStack_70;
              if ((code *)puVar13[4] == (code *)0x0) {
                do {
                  uVar9 = (*(code *)*puVar13)(*puVar7);
                  uVar11 = uVar11 - 1;
                  *puVar7 = uVar9;
                  puVar7 = puVar7 + 1;
                } while (uVar11 != 0);
              }
              else {
                (*(code *)puVar13[4])(apuStack_78[0],apuStack_78[0],uStack_70,4);
              }
            }
            uStack_7c = 0;
            uStack_84 = 0;
            puStack_80 = apuStack_78[0];
            local_90 = 1;
            puStack_8c = (undefined *)0x1;
            ppuStack_88 = ppuVar8;
            FUN_00115678(iVar3,iStack_68,uStack_70,1);
          }
          FUN_0010e334(apuStack_78);
          unaff_r10 = unaff_r10 + 1;
        } while ((int)unaff_r10 < (int)(uint)*(ushort *)(iVar4 + 0x12));
      }
      iVar3 = iStack_3c;
      if (DAT_00182f6c != 0) {
        FUN_00110e78(iStack_44);
        FUN_00110e78(iStack_40);
        iVar3 = iStack_3c;
      }
    }
    iVar4 = local_6c;
    if (*(int *)(&DAT_00007480 + local_6c) != 0) {
      FUN_0010e334(&DAT_00007480 + local_6c);
    }
    if (*(int *)(&DAT_0000748c + iVar4) != 0) {
      FUN_0010e0a8();
    }
    if (*(int *)(&DAT_00007490 + iVar4) != 0) {
      FUN_0010e0a8();
    }
    if (*(int *)(&DAT_00007474 + iVar4) != 0) {
      FUN_0010e0a8();
    }
    if (*(int *)(&DAT_0000747c + iVar4) != 0) {
      FUN_0010e0a8();
    }
    iVar5 = 0;
    if ((&DAT_0000731c)[iVar4] != '\0') {
      do {
        iVar10 = iVar4 + iVar5 * 0x10;
        if (*(int *)(&DAT_000073c4 + iVar10) != 0) {
          FUN_0010e0a8();
        }
        if (*(int *)(&DAT_00007384 + iVar10) != 0) {
          FUN_0010e0a8();
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)(uint)(byte)(&DAT_0000731c)[iVar4]);
    }
    if (*(int *)(&DAT_00007478 + iVar4) != 0) {
      FUN_0010e0a8();
    }
    FUN_0010e334(&local_6c);
    iVar4 = FUN_001189d8();
  } while( true );
}

// ==== FUN_0011b9a8 @ 0011b9a8, 44 bytes
void FUN_0011b9a8(int param_1)

{
  (*(code *)**(undefined4 **)(*(int *)(param_1 + 8) + 0x28))();
  return;
}

// ==== FUN_0011b9d4 @ 0011b9d4, 2708 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_0011b9d4(int param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,undefined4 *param_10)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  byte bVar6;
  undefined2 uVar7;
  int iVar8;
  ushort *puVar9;
  undefined2 *puVar10;
  undefined2 uVar11;
  byte *pbVar12;
  uint uVar13;
  int iVar14;
  undefined4 *puVar15;
  int iVar16;
  int *piVar17;
  int iVar18;
  undefined4 uVar19;
  uint uVar20;
  uint uVar21;
  byte *local_48;
  int local_44;
  
  iVar2 = param_9 + param_2;
  puVar15 = *(undefined4 **)(param_1 + 8);
  if (param_6 == 4) {
    iVar18 = 0;
    if (param_5 < 1) {
      return;
    }
    do {
      iVar8 = iVar18 + param_3;
      uVar13 = (uint)*param_8;
      if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
        iVar4 = iVar2;
        uVar5 = param_9;
        pbVar12 = param_8;
        uVar20 = param_4;
        if (param_10 == (undefined4 *)0x0) {
          do {
            iVar14 = 1 - uVar5;
            iVar16 = iVar8 * (*(int **)(param_1 + 8))[8] + iVar4;
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            *(short *)(**(int **)(param_1 + 8) + iVar16 * 2) =
                 (short)((0xf0U >> (iVar14 * -4 + 4U & 0xff) & uVar13) >> (iVar14 * 4 & 0xffU));
            if (uVar5 == 2) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
        else {
          do {
            iVar14 = 1 - uVar5;
            iVar16 = iVar8 * (*(int **)(param_1 + 8))[8] + iVar4;
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            *(short *)(**(int **)(param_1 + 8) + iVar16 * 2) =
                 (short)param_10[(0xf0U >> (iVar14 * -4 + 4U & 0xff) & uVar13) >>
                                 (iVar14 * 4 & 0xffU)];
            if (uVar5 == 2) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
      }
      else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
        iVar4 = iVar2;
        uVar5 = param_9;
        pbVar12 = param_8;
        uVar20 = param_4;
        if (param_10 == (undefined4 *)0x0) {
          do {
            uVar21 = (0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar13) >> ((1 - uVar5) * 4 & 0xff);
            if (uVar21 != 0) {
              *(short *)(**(int **)(param_1 + 8) + (iVar8 * (*(int **)(param_1 + 8))[8] + iVar4) * 2
                        ) = (short)uVar21;
            }
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            if (uVar5 == 2) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
        else {
          do {
            uVar21 = (0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar13) >> ((1 - uVar5) * 4 & 0xff);
            if (uVar21 != 0) {
              *(short *)(**(int **)(param_1 + 8) + (iVar8 * (*(int **)(param_1 + 8))[8] + iVar4) * 2
                        ) = (short)param_10[uVar21];
            }
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            if (uVar5 == 2) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
      }
      iVar18 = iVar18 + 1;
      param_8 = param_8 + param_7;
    } while (iVar18 < param_5);
    return;
  }
  if (4 < param_6) {
    if (param_6 == 8) {
      if ((((code *)puVar15[0x10] != (code *)0x0) && ((*(byte *)(DAT_00182efc + 0x10) & 2) == 0)) &&
         (param_10 != (undefined4 *)0x0)) {
        (*(code *)puVar15[0x10])(*(undefined4 *)(param_1 + 0x18),param_2,param_3,param_8);
        return;
      }
      iVar2 = 0;
      if (0 < param_5) {
        do {
          if (param_10 != (undefined4 *)0x0) {
            puVar10 = (undefined2 *)
                      (**(int **)(param_1 + 8) +
                      ((*(int **)(param_1 + 8))[8] * (iVar2 + param_3) + param_2) * 2);
            uVar13 = param_4;
            pbVar12 = param_8;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
              while (0 < (int)uVar13) {
                if (*pbVar12 != 0) {
                  *puVar10 = (short)param_10[*pbVar12];
                }
                puVar10 = puVar10 + 1;
                uVar13 = uVar13 - 1;
                pbVar12 = pbVar12 + 1;
              }
            }
            else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              if (7 < (int)param_4) {
                for (iVar18 = 1;
                    iVar18 <= -((int)(-param_4 + ((uint)((int)-param_4 >> 0x1f) >> 0x1d)) >> 3);
                    iVar18 = iVar18 + 1) {
                  uVar13 = uVar13 - 8;
                  *puVar10 = (short)param_10[*pbVar12];
                  puVar10[1] = (short)param_10[pbVar12[1]];
                  puVar10[2] = (short)param_10[pbVar12[2]];
                  puVar10[3] = (short)param_10[pbVar12[3]];
                  puVar10[4] = (short)param_10[pbVar12[4]];
                  puVar10[5] = (short)param_10[pbVar12[5]];
                  puVar10[6] = (short)param_10[pbVar12[6]];
                  puVar10[7] = (short)param_10[pbVar12[7]];
                  pbVar12 = pbVar12 + 8;
                  puVar10 = puVar10 + 8;
                }
              }
              if (3 < (int)uVar13) {
                iVar18 = -uVar13;
                for (iVar8 = 1; iVar8 <= -((int)(iVar18 + ((uint)(iVar18 >> 0x1f) >> 0x1e)) >> 2);
                    iVar8 = iVar8 + 1) {
                  uVar13 = uVar13 - 4;
                  *puVar10 = (short)param_10[*pbVar12];
                  puVar10[1] = (short)param_10[pbVar12[1]];
                  puVar10[2] = (short)param_10[pbVar12[2]];
                  puVar10[3] = (short)param_10[pbVar12[3]];
                  pbVar12 = pbVar12 + 4;
                  puVar10 = puVar10 + 4;
                }
              }
              if ((int)uVar13 < 1) {
                for (; uVar13 != 0; uVar13 = uVar13 - 1) {
                  *puVar10 = (short)param_10[*pbVar12];
                  pbVar12 = pbVar12 + 1;
                  puVar10 = puVar10 + 1;
                }
              }
              else {
                iVar18 = 1;
                if (0 < (int)uVar13) {
                  do {
                    iVar18 = iVar18 + 1;
                    *puVar10 = (short)param_10[*pbVar12];
                    pbVar12 = pbVar12 + 1;
                    puVar10 = puVar10 + 1;
                  } while (iVar18 <= (int)uVar13);
                }
              }
            }
          }
          iVar2 = iVar2 + 1;
          param_8 = param_8 + param_7;
        } while (iVar2 < param_5);
        return;
      }
      return;
    }
    if (param_6 != 0x10) {
      return;
    }
    if (DAT_00182ff8 != 1) {
      if (((code *)puVar15[0x13] != (code *)0x0) && ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0)) {
        (*(code *)puVar15[0x13])(*(undefined4 *)(param_1 + 0x18),param_2,param_3,param_8);
        return;
      }
      iVar2 = 0;
      if (0 < param_5) {
        do {
          (*DAT_00183074)(**(int **)(param_1 + 8) +
                          ((iVar2 + param_3) * (*(int **)(param_1 + 8))[8] + param_2) * 2,param_8,
                          param_4 << 1);
          iVar2 = iVar2 + 1;
          param_8 = param_8 + param_7;
        } while (iVar2 < param_5);
        return;
      }
      return;
    }
    FUN_001098ec(2);
    if (DAT_00182ffc == 0x10) {
      uVar19 = 2;
    }
    else {
      uVar19 = 1;
    }
    FUN_001098b4(uVar19);
    FUN_00109854(0x10000,0,0,0x10000);
    FUN_00109cc8(1);
    FUN_00109da8(4);
    FUN_00109bb8((uint)param_8 | 0x80000000,param_7,param_2 * -0x10000,param_3 * -0x10000);
    FUN_00109bf8(*puVar15,0,0,puVar15[6] << 1);
    iVar2 = FUN_00109e08();
    do {
      iVar2 = FUN_00109c88(iVar2);
    } while (iVar2 != 0);
    return;
  }
  if (param_6 != 1) {
    if (param_6 != 2) {
      return;
    }
    iVar18 = 0;
    if (param_5 < 1) {
      return;
    }
    do {
      iVar8 = iVar18 + param_3;
      uVar13 = (uint)*param_8;
      if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
        iVar4 = iVar2;
        uVar5 = param_9;
        pbVar12 = param_8;
        uVar20 = param_4;
        if (param_10 == (undefined4 *)0x0) {
          do {
            iVar14 = 3 - uVar5;
            iVar16 = iVar8 * (*(int **)(param_1 + 8))[8] + iVar4;
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            *(short *)(**(int **)(param_1 + 8) + iVar16 * 2) =
                 (short)((0xc0U >> (iVar14 * -2 + 6U & 0xff) & uVar13) >> (iVar14 * 2 & 0xffU));
            if (uVar5 == 4) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
        else {
          do {
            iVar14 = 3 - uVar5;
            iVar16 = iVar8 * (*(int **)(param_1 + 8))[8] + iVar4;
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            *(short *)(**(int **)(param_1 + 8) + iVar16 * 2) =
                 (short)param_10[(0xc0U >> (iVar14 * -2 + 6U & 0xff) & uVar13) >>
                                 (iVar14 * 2 & 0xffU)];
            if (uVar5 == 4) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
      }
      else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
        iVar4 = iVar2;
        uVar5 = param_9;
        pbVar12 = param_8;
        uVar20 = param_4;
        if (param_10 == (undefined4 *)0x0) {
          do {
            uVar21 = (0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar13) >> ((3 - uVar5) * 2 & 0xff);
            if (uVar21 != 0) {
              *(short *)(**(int **)(param_1 + 8) + (iVar8 * (*(int **)(param_1 + 8))[8] + iVar4) * 2
                        ) = (short)uVar21;
            }
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            if (uVar5 == 4) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
        else {
          do {
            uVar21 = (0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar13) >> ((3 - uVar5) * 2 & 0xff);
            if (uVar21 != 0) {
              *(short *)(**(int **)(param_1 + 8) + (iVar8 * (*(int **)(param_1 + 8))[8] + iVar4) * 2
                        ) = (short)param_10[uVar21];
            }
            uVar5 = uVar5 + 1;
            iVar4 = iVar4 + 1;
            if (uVar5 == 4) {
              pbVar12 = pbVar12 + 1;
              uVar13 = (uint)*pbVar12;
              uVar5 = 0;
            }
            uVar20 = uVar20 - 1;
          } while (uVar20 != 0);
        }
      }
      iVar18 = iVar18 + 1;
      param_8 = param_8 + param_7;
    } while (iVar18 < param_5);
    return;
  }
  if (((code *)puVar15[0xf] != (code *)0x0) && ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0)) {
    (*(code *)puVar15[0xf])(*(undefined4 *)(param_1 + 0x18),param_2,param_3,param_8);
    return;
  }
  local_44 = 0;
  if (param_5 < 1) {
    return;
  }
  do {
    local_48 = param_8;
    piVar17 = *(int **)(param_1 + 8);
    iVar18 = piVar17[8] * (local_44 + param_3) + iVar2;
    uVar19 = *param_10;
    uVar11 = (undefined2)param_10[1];
    uVar13 = (uint)((byte)(&DAT_00182184)[*param_8] >> (param_9 & 0xff));
    bVar6 = *(byte *)(DAT_00182efc + 0x10) & 3;
    uVar20 = 8 - param_9;
    uVar5 = param_4;
    if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
      do {
        uVar21 = uVar20;
        if (uVar5 <= uVar20) {
          uVar21 = uVar5;
        }
        uVar3 = uVar21;
        if (uVar21 < 0x80000000) {
          if ((int)uVar21 < 2) {
            uVar3 = 1;
          }
          iVar8 = 1;
          do {
            uVar7 = uVar11;
            if ((uVar13 & 1) == 0) {
              uVar7 = (short)uVar19;
            }
            iVar4 = iVar18 * 2;
            iVar8 = iVar8 + 1;
            uVar13 = uVar13 >> 1;
            iVar18 = iVar18 + 1;
            *(undefined2 *)(*piVar17 + iVar4) = uVar7;
          } while (iVar8 <= (int)uVar3);
        }
        else {
          do {
            uVar7 = uVar11;
            if ((uVar13 & 1) == 0) {
              uVar7 = (short)uVar19;
            }
            iVar8 = iVar18 * 2;
            uVar3 = uVar3 - 1;
            uVar13 = uVar13 >> 1;
            iVar18 = iVar18 + 1;
            *(undefined2 *)(*piVar17 + iVar8) = uVar7;
          } while (uVar3 != 0);
        }
        uVar20 = uVar20 - uVar21;
        uVar5 = uVar5 - uVar21;
        if (uVar20 == 0) {
          uVar20 = 8;
          local_48 = local_48 + 1;
          uVar13 = (uint)(byte)(&DAT_00182184)[*local_48];
        }
      } while (uVar5 != 0);
    }
    else if (bVar6 == 1) {
LAB_0011bce8:
      uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
      do {
        if ((uVar13 & 1) != 0) {
          puVar9 = (ushort *)(*piVar17 + iVar18 * 2);
          *puVar9 = *puVar9 ^ uVar1;
        }
        uVar20 = uVar20 - 1;
        uVar13 = uVar13 >> 1;
        iVar18 = iVar18 + 1;
        if (uVar20 == 0) {
          uVar20 = 8;
          local_48 = local_48 + 1;
          uVar13 = (uint)(byte)(&DAT_00182184)[*local_48];
        }
        uVar5 = uVar5 - 1;
      } while (uVar5 != 0);
    }
    else if (bVar6 == 2) {
      do {
        if ((uVar13 & 1) != 0) {
          *(undefined2 *)(*piVar17 + iVar18 * 2) = uVar11;
        }
        uVar20 = uVar20 - 1;
        uVar13 = uVar13 >> 1;
        iVar18 = iVar18 + 1;
        if (uVar20 == 0) {
          uVar20 = 8;
          local_48 = local_48 + 1;
          uVar13 = (uint)(byte)(&DAT_00182184)[*local_48];
        }
        uVar5 = uVar5 - 1;
      } while (uVar5 != 0);
    }
    else if (bVar6 == 3) goto LAB_0011bce8;
    local_44 = local_44 + 1;
    param_8 = param_8 + param_7;
    if (param_5 <= local_44) {
      return;
    }
  } while( true );
}

