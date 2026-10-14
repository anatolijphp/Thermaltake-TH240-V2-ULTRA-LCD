// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00110000-0x00113fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_0011091c @ 0011091c, 636 bytes
// called by: FUN_0010fff0, FUN_0011b2e8, jpeg_size
undefined4 FUN_0011091c(void)

{
  short sVar1;
  ushort uVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  bool bVar9;
  bool bVar10;
  byte local_48 [4];
  byte local_44 [4];
  byte local_40 [4];
  undefined1 local_3c [4];
  undefined1 local_38 [4];
  undefined4 *local_34 [2];
  char local_2c [4];
  undefined1 auStack_28 [16];
  
  uVar7 = 0;
  puVar3 = (undefined4 *)FUN_0010e24c();
  *puVar3 = FUN_001238b0;
  *(undefined1 *)(puVar3 + 7) = 1;
  puVar3[6] = 0;
  local_34[0] = puVar3;
  iVar4 = FUN_001238b0(puVar3,local_48);
  if (iVar4 == 0) {
    uVar5 = (*(code *)*puVar3)(puVar3,local_44);
    bVar9 = uVar5 == 0;
    if (bVar9) {
      uVar5 = (uint)local_48[0];
    }
    bVar10 = bVar9 && uVar5 == 0xff;
    if (bVar9 && uVar5 == 0xff) {
      uVar5 = (uint)local_44[0];
      bVar10 = uVar5 == 0xd8;
    }
    if (bVar10) {
      local_2c[0] = (char)uVar5;
      iVar4 = FUN_00125054(local_34[0],local_2c);
      puVar3 = local_34[0];
      if (iVar4 == 0) {
        if (local_2c[0] != -0x40 && local_2c[0] != -0x3f) {
          if (local_2c[0] != -0x3e) goto LAB_00110b70;
          *(undefined1 *)((int)local_34[0] + 0x7409) = 1;
        }
        iVar4 = (*(code *)*local_34[0])(local_34[0],local_44);
        if ((iVar4 == 0) && (iVar4 = (*(code *)*puVar3)(puVar3,local_3c), iVar4 == 0)) {
          uVar2 = CONCAT11(local_44[0],local_3c[0]);
          iVar4 = (*(code *)*puVar3)(puVar3,auStack_28);
          if (iVar4 == 0) {
            iVar4 = (*(code *)*puVar3)(puVar3,local_3c);
            if ((iVar4 == 0) && (iVar4 = (*(code *)*puVar3)(puVar3,local_38), iVar4 == 0)) {
              *(ushort *)((int)puVar3 + 0x12) = CONCAT11(local_3c[0],local_38[0]);
              iVar4 = (*(code *)*puVar3)(puVar3,local_3c);
              if ((iVar4 == 0) && (iVar4 = (*(code *)*puVar3)(puVar3,local_38), iVar4 == 0)) {
                *(ushort *)(puVar3 + 4) = CONCAT11(local_3c[0],local_38[0]);
                iVar4 = (*(code *)*puVar3)(puVar3,puVar3 + 0x1cc7);
                if (iVar4 == 0) {
                  bVar9 = *(short *)((int)puVar3 + 0x12) != 0;
                  sVar1 = 0;
                  if (bVar9) {
                    sVar1 = *(short *)(puVar3 + 4);
                  }
                  if (bVar9 && sVar1 != 0) {
                    uVar5 = (uint)*(byte *)(puVar3 + 0x1cc7);
                    bVar9 = uVar5 == 4;
                    if (uVar5 < 5) {
                      bVar9 = uVar5 * 3 == uVar2 - 8;
                    }
                    if (bVar9) {
                      iVar4 = 0;
                      if (uVar5 != 0) {
                        do {
                          iVar8 = (int)puVar3 + iVar4;
                          iVar6 = (*(code *)*puVar3)(puVar3,&DAT_00007329 + iVar8);
                          if ((iVar6 != 0) ||
                             (iVar6 = (*(code *)*puVar3)(puVar3,local_40), iVar6 != 0))
                          goto LAB_00110b6c;
                          (&DAT_0000731d)[iVar8] = local_40[0] >> 4;
                          (&DAT_00007321)[iVar8] = local_40[0] & 0xf;
                          iVar6 = (*(code *)*puVar3)(puVar3,&DAT_00007325 + iVar8);
                          if (iVar6 != 0) goto LAB_00110b6c;
                          iVar4 = iVar4 + 1;
                        } while (iVar4 < (int)(uint)*(byte *)(puVar3 + 0x1cc7));
                      }
                      goto LAB_00110b70;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
LAB_00110b6c:
  uVar7 = 1;
LAB_00110b70:
  FUN_0010e334(local_34);
  return uVar7;
}

// ==== FUN_00110b9c @ 00110b9c, 112 bytes
// called by: FUN_00110ed8
void FUN_00110b9c(int param_1)

{
  int iVar1;
  int iStack_10;
  int iStack_c;
  
  if ((param_1 == 0) && (*(int *)(DAT_00182efc + 100) == 0)) {
    return;
  }
  iStack_10 = FUN_0010e24c();
  iVar1 = *(int *)(iStack_10 + 0x14);
  FUN_0010e334(&iStack_10);
  if (iVar1 != 0) {
    iStack_c = FUN_0010e24c(iVar1);
    (**(code **)(*(int *)(iStack_c + 8) + 8))();
    FUN_0010e334(&iStack_c);
    return;
  }
  return;
}

// ==== FUN_00110c10 @ 00110c10, 12 bytes
// called by: FUN_00124a6c, FUN_00110ed8
void FUN_00110c10(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  short sStack_30;
  short sStack_2e;
  short sStack_2c;
  short sStack_2a;
  int *piStack_28;
  undefined4 *puStack_24;
  
  if (param_1 != 0) {
    uVar1 = thunk_FUN_00116b48(0);
    iVar5 = *(int *)(DAT_00182efc + 100);
    piStack_28 = (int *)FUN_0010e24c(param_1);
    uVar2 = FUN_00112bb8(*(undefined4 *)(*piStack_28 + 0x18));
    sStack_30 = (short)piStack_28[1];
    iVar4 = (int)sStack_30;
    sStack_2e = *(short *)((int)piStack_28 + 6);
    iVar6 = (int)sStack_2e;
    sStack_2c = (short)piStack_28[2] + sStack_30 + -1;
    sStack_2a = *(short *)((int)piStack_28 + 10) + sStack_2e + -1;
    FUN_0011740c();
    FUN_0010e334(&piStack_28);
    iVar3 = FUN_00118f24(&sStack_30);
    while (iVar3 != 0) {
      FUN_001119fc(param_1,iVar4,iVar6);
      iVar3 = FUN_001189d8();
    }
    FUN_00112bb8(uVar2);
    if (iVar5 == 0) {
      FUN_00112ba4();
    }
    else {
      if (*(int *)(DAT_00182efc + 100) != 0) {
        FUN_00112ba4();
      }
      FUN_001176e0();
      puStack_24 = (undefined4 *)FUN_0010e24c(iVar5);
      *(int *)(DAT_00182efc + 100) = iVar5;
      FUN_0010e634(*puStack_24);
      FUN_0010e334(&puStack_24);
      FUN_00116e10();
      FUN_00116a7c();
    }
    thunk_FUN_00116b48(uVar1);
    return;
  }
  return;
}

// ==== FUN_00110c20 @ 00110c20, 308 bytes
// called by: idle_screen_proc
void FUN_00110c20(int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  short local_30;
  short local_2e;
  short local_2c;
  short local_2a;
  int *local_28;
  undefined4 *local_24;
  
  if (param_1 != 0) {
    uVar1 = thunk_FUN_00116b48(0);
    iVar4 = *(int *)(DAT_00182efc + 100);
    local_28 = (int *)FUN_0010e24c(param_1);
    uVar2 = FUN_00112bb8(*(undefined4 *)(*local_28 + 0x18));
    if (param_2 == -0xfff) {
      param_2 = (int)(short)local_28[1];
      param_3 = (int)*(short *)((int)local_28 + 6);
    }
    local_30 = (short)param_2;
    local_2e = (short)param_3;
    local_2c = (short)local_28[2] + local_30 + -1;
    local_2a = *(short *)((int)local_28 + 10) + local_2e + -1;
    FUN_0011740c();
    FUN_0010e334(&local_28);
    iVar3 = FUN_00118f24(&local_30);
    while (iVar3 != 0) {
      FUN_001119fc(param_1,param_2,param_3);
      iVar3 = FUN_001189d8();
    }
    FUN_00112bb8(uVar2);
    if (iVar4 == 0) {
      FUN_00112ba4();
    }
    else {
      if (*(int *)(DAT_00182efc + 100) != 0) {
        FUN_00112ba4();
      }
      FUN_001176e0();
      local_24 = (undefined4 *)FUN_0010e24c(iVar4);
      *(int *)(DAT_00182efc + 100) = iVar4;
      FUN_0010e634(*local_24);
      FUN_0010e334(&local_24);
      FUN_00116e10();
      FUN_00116a7c();
    }
    thunk_FUN_00116b48(uVar1);
    return;
  }
  return;
}

// ==== FUN_00110d58 @ 00110d58, 212 bytes
// called by: FUN_00110ed8
undefined4 FUN_00110d58(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  piVar2 = (int *)(&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)];
  uVar3 = 0;
  do {
    if (*(int *)piVar2[3] == 4) {
      if (piVar2 != (int *)0x0) goto LAB_00110dc8;
      break;
    }
    piVar2 = (int *)*piVar2;
  } while (piVar2 != (int *)0x0);
  piVar2 = (int *)FUN_0010e7f0((uint)*(byte *)(DAT_00182efc + 0x11));
  if (piVar2 == (int *)0x0) {
    return 0;
  }
LAB_00110dc8:
  iVar1 = (**(code **)(piVar2[3] + 0x2c))(piVar2,1);
  if (iVar1 != 0) {
    (**(code **)(piVar2[4] + 8))();
    uVar3 = FUN_00111618(param_1,param_2,param_3,param_4);
  }
  return uVar3;
}

// ==== FUN_00110e38 @ 00110e38, 64 bytes
// called by: FUN_0011b2e8, screen_create_proc
undefined4 FUN_00110e38(undefined4 param_1)

{
  undefined4 uVar1;
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    uVar1 = FUN_00111618(param_1);
    return uVar1;
  }
  return 0;
}

// ==== FUN_00110e78 @ 00110e78, 92 bytes
// called by: FUN_00119230, FUN_001180e8, FUN_0011b2e8, FUN_00110ed8, FUN_00117768
void FUN_00110e78(int param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  undefined4 *local_10;
  
  if (param_1 == 0) {
    return;
  }
  local_10 = param_4;
  if (*(int *)(DAT_00182efc + 100) == param_1) {
    FUN_00112ba4();
  }
  local_10 = (undefined4 *)FUN_0010e24c(param_1);
  if (local_10[5] != 0) {
    FUN_00113e2c();
  }
  thunk_FUN_0010e104(*local_10);
  FUN_0010e334(&local_10);
  FUN_0010e0a8(param_1);
  return;
}

// ==== FUN_00110ed8 @ 00110ed8, 496 bytes
// called by: FUN_001179fc
undefined4
FUN_00110ed8(short *param_1,code *param_2,undefined4 param_3,int param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int local_3c;
  int local_38;
  short *psStack_34;
  code *pcStack_30;
  undefined4 local_2c;
  int iStack_28;
  
  uVar4 = (uint)*(byte *)(DAT_00182efc + 0x11);
  psStack_34 = param_1;
  pcStack_30 = param_2;
  local_2c = param_3;
  iStack_28 = param_4;
  if ((char)(&DAT_00182f4c)[uVar4] < '\x02') {
    iVar1 = FUN_00115f8c(uVar4);
    iVar2 = FUN_00116028(uVar4);
  }
  else {
    iVar1 = FUN_001160c0();
    iVar2 = FUN_001161b8(uVar4);
  }
  if (param_1 == (short *)0x0) {
    iVar7 = 0;
    iVar6 = 0;
  }
  else {
    FUN_0010ec0c(&local_3c,&local_38);
    iVar7 = (int)*param_1;
    iVar6 = (int)param_1[1];
    if (iVar7 < 0) {
      iVar7 = 0;
    }
    if (iVar6 < 0) {
      iVar6 = 0;
    }
    iVar3 = local_3c + iVar1 + -1;
    iVar1 = (int)param_1[2];
    if (iVar3 < param_1[2]) {
      iVar1 = iVar3;
    }
    iVar3 = local_38 + iVar2 + -1;
    iVar2 = (int)param_1[3];
    if (iVar3 < param_1[3]) {
      iVar2 = iVar3;
    }
    iVar1 = (iVar1 - iVar7) + 1;
    iVar2 = (iVar2 - iVar6) + 1;
  }
  if (param_4 == 0) {
    param_4 = -iVar2;
  }
  iVar3 = iVar1;
  if (0 < iVar1) {
    iVar3 = iVar2;
  }
  if (0 < iVar3) {
    iVar1 = FUN_00110d58(iVar7,iVar6,iVar1,param_4,param_5);
    if (iVar1 == 0) {
      (*param_2)(local_2c);
      return 1;
    }
    iVar3 = FUN_0011113c();
    if ((iVar3 != iVar2) && (*(int *)(DAT_00182efc + 0x44) != 0)) {
      FUN_0011990c(*(int *)(DAT_00182efc + 0x44),0x32);
    }
    FUN_0011147c(iVar1);
    iVar5 = 0;
    do {
      if (iVar2 - iVar5 < iVar3) {
        FUN_00111430(iVar1);
      }
      if (iVar5 != 0) {
        FUN_001114e0(iVar1,iVar7,iVar6 + iVar5);
        FUN_00110b9c(iVar1);
      }
      (*param_2)(local_2c);
      if (DAT_00183124 != (code *)0x0) {
        (*DAT_00183124)(0xff);
      }
      FUN_00110c10(iVar1);
      iVar5 = iVar5 + iVar3;
    } while (iVar5 < iVar2);
    FUN_00110e78(iVar1);
    FUN_0011147c(0);
    if ((iVar3 != iVar2) && (*(int *)(DAT_00182efc + 0x44) != 0)) {
      FUN_0011990c(*(int *)(DAT_00182efc + 0x44),0x33);
    }
  }
  return 0;
}

// ==== FUN_001110d4 @ 001110d4, 64 bytes
// called by: FUN_001119fc
undefined4 FUN_001110d4(int param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined4 uVar1;
  int *local_10;
  
  uVar1 = 0;
  if (param_1 != 0) {
    local_10 = param_4;
    local_10 = (int *)FUN_0010e24c();
    (**(code **)(*(int *)(*local_10 + 0x10) + 8))();
    uVar1 = FUN_0011728c();
    FUN_0010e334(&local_10);
  }
  return uVar1;
}

// ==== FUN_00111114 @ 00111114, 40 bytes
// called by: FUN_001117ac, FUN_00111208, FUN_0011b2e8, FUN_001265c8, FUN_001119fc
int FUN_00111114(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  if (param_1 == 0) {
    return 0;
  }
  local_10 = param_4;
  local_10 = FUN_0010e24c();
  iVar1 = local_10 + 0x18;
  FUN_0010e334(&local_10);
  return iVar1;
}

// ==== FUN_0011113c @ 0011113c, 64 bytes
// called by: FUN_00110ed8
int FUN_0011113c(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  iVar1 = 0;
  if ((param_1 != 0) || (*(int *)(DAT_00182efc + 100) != 0)) {
    local_10 = param_4;
    local_10 = FUN_0010e24c();
    iVar1 = (int)*(short *)(local_10 + 10);
    FUN_0010e334(&local_10);
  }
  return iVar1;
}

// ==== FUN_00111180 @ 00111180, 112 bytes
// called by: screen_create_proc
bool FUN_00111180(int param_1)

{
  code *pcVar1;
  bool bVar2;
  
  if (DAT_00182fcc == '\0') {
    DAT_00182fcc = '\x01';
    DAT_00182fe4 = &LAB_00124d1c;
    FUN_00115274(&DAT_00182fe4);
  }
  pcVar1 = FUN_00124a88;
  bVar2 = DAT_00182fd4 == FUN_00124a88;
  if (param_1 == 0) {
    DAT_00182fd8 = FUN_00110c10;
    pcVar1 = FUN_0011158c;
  }
  else {
    DAT_00182fd8 = FUN_00124a6c;
  }
  DAT_00182fd4 = pcVar1;
  return bVar2;
}

// ==== FUN_00111208 @ 00111208, 528 bytes
// called by: FUN_001196dc, FUN_00124e84
void FUN_00111208(int param_1)

{
  short sVar1;
  int iVar2;
  short sVar3;
  short sVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  bool bVar14;
  bool bVar15;
  int *local_2c;
  int local_28;
  
  iVar12 = 0;
  do {
    local_2c = (int *)FUN_0010e24c(param_1);
    iVar11 = *local_2c;
    iVar8 = local_2c[1];
    iVar13 = local_2c[9];
    FUN_0010e334(&local_2c);
    sVar1 = (short)iVar11;
    iVar11 = iVar11 >> 0x10;
    iVar2 = (int)sVar1;
    iVar9 = ((short)iVar8 - iVar2) + 1;
    iVar10 = ((iVar8 >> 0x10) - iVar11) + 1;
    iVar8 = iVar9;
    if (0 < iVar9) {
      iVar8 = iVar10;
    }
    if (0 < iVar8) {
      if (iVar13 == 0) {
        iVar8 = FUN_00117f38();
        if (param_1 == iVar8) {
          iVar13 = FUN_00116064();
          iVar5 = FUN_0011615c();
          bVar15 = SBORROW4(iVar9,iVar13);
          iVar8 = iVar9 - iVar13;
          bVar14 = iVar9 == iVar13;
          if (iVar9 <= iVar13) {
            bVar15 = SBORROW4(iVar10,iVar5);
            iVar8 = iVar10 - iVar5;
            bVar14 = iVar10 == iVar5;
          }
          if (!bVar14 && iVar8 < 0 == bVar15) {
            if (iVar13 < iVar9) {
              iVar9 = iVar13;
            }
            if (iVar5 < iVar10) {
              iVar10 = iVar5;
            }
            FUN_00118650(param_1,iVar9,iVar10);
          }
        }
        iVar11 = FUN_00111618(iVar2,iVar11,iVar9,iVar10,1,&DAT_001822c4,&PTR_LAB_00181e70);
        local_2c = (int *)FUN_0010e24c(param_1);
        local_2c[9] = iVar11;
        if (iVar11 == 0) {
          FUN_0010e334(&local_2c);
          FUN_001195dc(param_1);
          goto LAB_001113f8;
        }
        local_2c[10] = local_2c[10] | 0x10000;
        FUN_0010e334(&local_2c);
        uVar6 = FUN_0011147c(iVar11);
        FUN_00113ef4(1);
        uVar7 = FUN_00111114(iVar11);
        (*DAT_00183070)(uVar7,DAT_00183128,iVar9 * iVar10 * 4);
        uVar7 = DAT_00182f48;
        DAT_001830e4 = param_1;
        DAT_00182f48 = FUN_00117f88(param_1);
        FUN_001195dc(param_1);
        DAT_001830e4 = 0;
        DAT_00182f48 = uVar7;
        FUN_00113ef4();
        FUN_0011147c(uVar6);
      }
      else {
        local_28 = FUN_0010e24c(iVar13);
        sVar3 = *(short *)(local_28 + 6);
        sVar4 = *(short *)(local_28 + 4);
        FUN_0010e334(&local_28);
        if (sVar1 != sVar4 || iVar11 != sVar3) {
          FUN_001114e0(iVar13,iVar2,iVar11);
        }
      }
      iVar11 = FUN_0010e384(param_1);
      iVar11 = *(int *)(iVar11 + 0x24);
      if (iVar11 != 0) {
        if (iVar11 == 0) {
          return;
        }
        iVar12 = FUN_0010e24c();
        sVar1 = *(short *)(iVar12 + 4);
        sVar3 = *(short *)(iVar12 + 6);
        FUN_0010e334(&stack0xffffffe8);
        iVar12 = FUN_00118f24(&stack0xffffffe0);
        if (iVar12 != 0) {
          do {
            FUN_001119a4(iVar11,(int)sVar1,(int)sVar3);
            iVar12 = FUN_001189d8();
          } while (iVar12 != 0);
          return;
        }
        return;
      }
    }
LAB_001113f8:
    iVar12 = iVar12 + 1;
    if (4 < iVar12) {
      return;
    }
  } while( true );
}

// ==== FUN_00111430 @ 00111430, 72 bytes
// called by: FUN_00110ed8
void FUN_00111430(int param_1,int param_2,undefined4 param_3,int param_4)

{
  int iStack_10;
  
  if ((param_1 == 0) && (*(int *)(DAT_00182efc + 100) == 0)) {
    return;
  }
  iStack_10 = param_4;
  iStack_10 = FUN_0010e24c();
  if (*(short *)(iStack_10 + 10) < param_2) {
    param_2 = (int)*(short *)(iStack_10 + 10);
  }
  *(short *)(iStack_10 + 10) = (short)param_2;
  FUN_0010e334(&iStack_10);
  return;
}

// ==== FUN_0011147c @ 0011147c, 96 bytes
// called by: FUN_00111208, lcd_boot_proc, FUN_00110ed8, idle_screen_proc, usb_command
int FUN_0011147c(int param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 *local_18;
  
  iVar1 = *(int *)(DAT_00182efc + 100);
  local_18 = param_4;
  if (param_1 == 0) {
    FUN_00112ba4();
  }
  else {
    if (iVar1 != 0) {
      FUN_00112ba4();
    }
    FUN_001176e0();
    local_18 = (undefined4 *)FUN_0010e24c(param_1);
    *(int *)(DAT_00182efc + 100) = param_1;
    FUN_0010e634(*local_18);
    FUN_0010e334(&local_18);
    FUN_00116e10();
    FUN_00116a7c();
  }
  return iVar1;
}

// ==== FUN_001114e0 @ 001114e0, 168 bytes
// called by: FUN_00111208, FUN_00110ed8
void FUN_001114e0(int param_1,undefined2 param_2,undefined2 param_3)

{
  short sVar1;
  short sVar2;
  int iStack_18;
  undefined2 *apuStack_14 [2];
  
  if ((param_1 == 0) && (*(int *)(DAT_00182efc + 100) == 0)) {
    return;
  }
  iStack_18 = FUN_0010e24c();
  *(undefined2 *)(iStack_18 + 6) = param_3;
  *(undefined2 *)(iStack_18 + 4) = param_2;
  FUN_00116a7c();
  if (*(int *)(iStack_18 + 0x14) != 0) {
    apuStack_14[0] = (undefined2 *)FUN_0010e24c();
    sVar1 = *(short *)(iStack_18 + 8);
    apuStack_14[0][2] = sVar1;
    sVar2 = *(short *)(iStack_18 + 10);
    apuStack_14[0][3] = sVar2;
    if (sVar1 != 0 && sVar2 != 0) {
      *apuStack_14[0] = param_2;
      apuStack_14[0][1] = param_3;
    }
    FUN_0010e334(apuStack_14);
  }
  FUN_0010e334(&iStack_18);
  return;
}

// ==== FUN_0011158c @ 0011158c, 140 bytes
// called by: FUN_00124a88
void FUN_0011158c(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  short sStack_20;
  short sStack_1e;
  short sStack_1c;
  short sStack_1a;
  int aiStack_18 [2];
  
  if (param_1 == 0) {
    return;
  }
  aiStack_18[0] = FUN_0010e24c();
  sStack_20 = *(short *)(aiStack_18[0] + 4);
  iVar3 = (int)sStack_20;
  sStack_1e = *(short *)(aiStack_18[0] + 6);
  iVar2 = (int)sStack_1e;
  sStack_1c = *(short *)(aiStack_18[0] + 8) + sStack_20 + -1;
  sStack_1a = *(short *)(aiStack_18[0] + 10) + sStack_1e + -1;
  FUN_0010e334(aiStack_18);
  iVar1 = FUN_00118f24(&sStack_20);
  if (iVar1 != 0) {
    do {
      FUN_001119a4(param_1,iVar3,iVar2);
      iVar1 = FUN_001189d8();
    } while (iVar1 != 0);
    return;
  }
  return;
}

// ==== FUN_00111618 @ 00111618, 400 bytes
// called by: FUN_00110e38, FUN_00111208, FUN_00110d58
int FUN_00111618(undefined4 param_1,undefined4 param_2,int param_3,int param_4,uint param_5,
                undefined4 param_6,undefined4 param_7)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *local_3c [2];
  undefined4 local_34;
  undefined4 local_30;
  int iStack_2c;
  int iStack_28;
  
  iVar6 = 0;
  local_34 = param_1;
  local_30 = param_2;
  iStack_2c = param_3;
  iStack_28 = param_4;
  iVar2 = FUN_0010e4d8(param_6,param_7,0,*(undefined1 *)(DAT_00182efc + 0x11));
  if (iVar2 != 0) {
    (**(code **)(*(int *)(iVar2 + 0x10) + 8))();
    iVar3 = FUN_0011728c();
    uVar1 = param_3 * iVar3 + 7U >> 3;
    iVar4 = FUN_0010e288();
    if ((param_5 & 1) == 0) {
      iVar4 = ((int)(iVar4 + ((uint)(iVar4 >> 0x1f) >> 0x1e)) >> 2) * 3;
    }
    iVar5 = param_4;
    if (param_4 < 1) {
      if (uVar1 == 0) {
        iVar5 = 0;
      }
      else {
        iVar5 = FUN_0010d398(iVar4 + -0x18,uVar1);
      }
      if (-param_4 < iVar5) {
        iVar5 = -param_4;
      }
    }
    if ((param_5 & 1) == 0) {
      iVar6 = FUN_00113c90(local_34,local_30,param_3,iVar5,0);
    }
    if (iVar5 < 1) {
      thunk_FUN_0010e104(iVar2);
      return 0;
    }
    if ((param_5 & 1) == 0) {
      iVar4 = FUN_0010dfec(iVar5 * uVar1 + 0x18);
    }
    else {
      iVar4 = FUN_0010dfe0();
    }
    if (iVar4 != 0) {
      local_3c[0] = (int *)FUN_0010e24c();
      *local_3c[0] = iVar2;
      *(short *)(local_3c[0] + 1) = (short)local_34;
      *(short *)((int)local_3c[0] + 6) = (short)local_30;
      *(short *)(local_3c[0] + 2) = (short)param_3;
      *(short *)((int)local_3c[0] + 10) = (short)iVar5;
      local_3c[0][3] = uVar1;
      local_3c[0][4] = iVar3;
      local_3c[0][5] = iVar6;
      FUN_0010e334(local_3c);
      *(undefined4 *)(iVar2 + 0x10) = param_7;
      *(int *)(iVar2 + 8) = iVar4;
      *(uint *)(iVar2 + 0x18) = (uint)*(byte *)(DAT_00182efc + 0x11);
      return iVar4;
    }
    thunk_FUN_0010e104(iVar2);
    if (iVar6 != 0) {
      FUN_0010e0a8(iVar6);
    }
  }
  return 0;
}

// ==== FUN_001117ac @ 001117ac, 484 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_001117ac(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iStack_40;
  int iStack_3c;
  int iStack_38;
  int local_34;
  undefined4 local_30;
  int iStack_2c;
  int iStack_28;
  
  local_30 = param_1;
  iStack_2c = param_2;
  iStack_28 = param_3;
  if (DAT_00183060 != (code *)0x0) {
    iVar1 = **(int **)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc);
    if (*(int *)(DAT_00182efc + 100) == 0) {
      if (iVar1 == 0) goto LAB_0011182c;
    }
    else if ((iVar1 != 7) &&
            (*(undefined ***)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10) ==
             &PTR_LAB_00181e70)) {
LAB_0011182c:
      local_34 = FUN_0010e24c(param_1);
      iVar6 = (int)*(short *)(local_34 + 8);
      iVar5 = (int)*(short *)(local_34 + 10);
      uVar7 = *(undefined4 *)(local_34 + 0xc);
      FUN_0010e334(&local_34);
      iVar3 = ((param_2 + iVar6) - (int)*(short *)(DAT_00182efc + 0xc)) + -1;
      iVar1 = iVar6;
      if (0 < iVar3) {
        iVar1 = iVar6 - iVar3;
      }
      if (param_2 < *(short *)(DAT_00182efc + 8)) {
        iVar3 = *(short *)(DAT_00182efc + 8) - param_2;
        param_2 = param_2 + iVar3;
        iVar1 = iVar1 - iVar3;
      }
      else {
        iVar3 = 0;
      }
      if (iVar1 < 1) {
        return;
      }
      iVar4 = ((param_3 + iVar5) - (int)*(short *)(DAT_00182efc + 0xe)) + -1;
      if (0 < iVar4) {
        iVar5 = iVar5 - iVar4;
      }
      if (param_3 < *(short *)(DAT_00182efc + 10)) {
        iVar4 = *(short *)(DAT_00182efc + 10) - param_3;
        param_3 = param_3 + iVar4;
        iVar5 = iVar5 - iVar4;
      }
      else {
        iVar4 = 0;
      }
      if (iVar5 < 1) {
        return;
      }
      iVar2 = FUN_00111114(local_30);
      iVar3 = iVar2 + iVar6 * iVar4 * 4 + iVar3 * 4;
      if (*(int *)(DAT_00182efc + 100) == 0) {
        iStack_38 = iVar6 << 2;
        iStack_40 = iVar1;
        iStack_3c = iVar5;
        (*DAT_00183064)(*(undefined1 *)(DAT_00182efc + 0x11),param_2,param_3,iVar3);
        return;
      }
      iStack_40 = FUN_0010e24c();
      iVar4 = *(int *)(iStack_40 + 0xc);
      iVar6 = (param_3 - *(short *)(iStack_40 + 6)) * iVar4 + iStack_40 + 0x18 + param_2 * 4 +
              *(short *)(iStack_40 + 4) * -4;
      FUN_0010e334(&iStack_40);
      if (iVar6 != 0) {
        iStack_40 = iVar4;
        iStack_3c = uVar7;
        (*DAT_00183060)(iVar6,iVar3,iVar1,iVar5);
        return;
      }
      return;
    }
  }
  FUN_001265c8(param_1,param_2,param_3);
  return;
}

// ==== FUN_001119a4 @ 001119a4, 88 bytes
// called by: FUN_0011158c
void FUN_001119a4(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  code *pcVar1;
  int *local_18;
  
  local_18 = param_4;
  local_18 = (int *)FUN_0010e24c();
  pcVar1 = (code *)(**(code **)(*(int *)(*local_18 + 0xc) + 0x24))(0,0x16);
  FUN_0010e334(&local_18);
  if (pcVar1 == (code *)0x0) {
    return;
  }
  (*pcVar1)(param_1,param_2,param_3);
  return;
}

// ==== FUN_001119fc @ 001119fc, 3160 bytes
// called by: FUN_00110c20
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_001119fc(int param_1,int param_2,int param_3)

{
  short sVar1;
  uint uVar2;
  code cVar3;
  undefined2 uVar4;
  int iVar5;
  code *pcVar6;
  undefined4 *puVar7;
  undefined4 uVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  int *piVar12;
  int *piVar13;
  undefined2 *puVar14;
  int iVar15;
  uint uVar16;
  code *pcVar17;
  code *pcVar18;
  undefined2 *puStack_88;
  int local_84;
  int local_78;
  int iStack_74;
  int *local_70;
  int iStack_6c;
  uint local_64;
  code *local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50 [3];
  undefined4 *apuStack_44 [2];
  int *local_3c;
  code *local_38;
  int local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  int local_28;
  
  if (param_1 == 0) {
    return;
  }
  local_50[0] = FUN_0010e24c();
  iVar11 = *(int *)(local_50[0] + 0x14);
  FUN_0010e334(local_50);
  iVar5 = FUN_001110d4(param_1);
  if ((iVar5 < 9) || (iVar11 != 0)) {
    local_30 = DAT_00182384;
    uStack_2c = DAT_00182388;
    local_54 = param_3;
    local_3c = (int *)FUN_0010e24c(param_1);
    local_84 = local_3c[3];
    iVar5 = local_3c[5];
    local_78 = (int)(short)local_3c[2];
    local_58 = (int)*(short *)((int)local_3c + 10);
    uVar16 = local_3c[4];
    local_60 = *(code **)(*(int *)(*local_3c + 0x10) + 4);
    local_64 = FUN_0010eb64(*(undefined1 *)(DAT_00182efc + 0x11));
    pcVar6 = (code *)FUN_0010ecb0(*(undefined1 *)(DAT_00182efc + 0x11));
    FUN_0010e334(&local_3c);
    if (uVar16 < 9) {
      local_5c = 1;
    }
    else if (uVar16 < 0x11) {
      local_5c = 2;
    }
    else {
      local_5c = 4;
    }
    if (local_60 == pcVar6) {
      FUN_00111114(param_1);
      param_3 = local_54;
      iVar11 = local_78;
      iVar15 = local_58;
      if (iVar5 != 0) {
        local_34 = FUN_0010e24c(iVar5);
        iVar5 = 0;
        if (0 < local_58) {
          do {
            local_70 = (int *)0x0;
            iVar11 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iVar5);
            if (iVar11 == local_78) {
              iVar10 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iVar5 + 1);
              iVar15 = iVar5;
              for (; iVar10 == iVar11;
                  iVar10 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iVar10))
              {
                iVar10 = iVar15 + 2;
                iVar15 = iVar15 + 1;
              }
              FUN_00115678(param_2,local_54 + iVar5,local_78,(iVar15 - iVar5) + 1);
              iVar5 = iVar15;
            }
            else if (uVar16 == 1) {
              if (iVar11 != 0) {
                iVar15 = local_54 + iVar5;
                do {
                  FUN_00116f78((int)local_70 + param_2,iVar15,iVar11,1);
                  local_70 = (int *)((int)local_70 + iVar11);
                  iVar11 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iVar5);
                } while (iVar11 != 0);
              }
            }
            else if (iVar11 != 0) {
              iVar15 = local_54 + iVar5;
              do {
                FUN_00115678((int)local_70 + param_2,iVar15,iVar11,1);
                local_70 = (int *)((int)local_70 + iVar11);
                iVar11 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iVar5);
              } while (iVar11 != 0);
            }
            iVar5 = iVar5 + 1;
          } while (iVar5 < local_58);
        }
        FUN_0010e334(&local_34);
        return;
      }
      goto LAB_001121bc;
    }
    local_38 = (code *)FUN_0010ec98(*(undefined1 *)(DAT_00182efc + 0x11));
    local_28 = FUN_0010dfe0(local_78 << 2);
    if (local_28 == 0) {
      return;
    }
    puStack_88 = (undefined2 *)FUN_00111114(param_1);
    if (iVar5 == 0) {
      FUN_00115678(param_2,local_54,local_78,local_58);
      iVar5 = local_28;
    }
    else {
      local_34 = FUN_0010e24c(iVar5);
      iStack_6c = 0;
      if (0 < local_58) {
        do {
          local_70 = (int *)0x0;
          iVar5 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iStack_6c);
          if (iVar5 == local_78) {
            puVar7 = (undefined4 *)FUN_0010e24c(local_28);
            pcVar18 = local_38;
            pcVar6 = local_60;
            uVar2 = local_64;
            puVar9 = puVar7;
            puVar14 = puStack_88;
            iVar11 = iVar5;
            apuStack_44[0] = puVar7;
            if (uVar16 < 9) {
              do {
                (*pcVar6)(*(undefined1 *)puVar14);
                uVar8 = (*pcVar18)();
                iVar11 = iVar11 + -1;
                *puVar9 = uVar8;
                puVar9 = puVar9 + 1;
                puVar14 = (undefined2 *)((int)puVar14 + 1);
              } while (iVar11 != 0);
            }
            else {
              do {
                (*pcVar6)(*puVar14);
                uVar8 = (*pcVar18)();
                iVar11 = iVar11 + -1;
                *puVar9 = uVar8;
                puVar9 = puVar9 + 1;
                puVar14 = puVar14 + 1;
              } while (iVar11 != 0);
            }
            if (uVar2 < 0x20) {
              FUN_00113f28(puVar7,iVar5,uVar2);
            }
            FUN_00115678(param_2,local_54 + iStack_6c,local_78,1);
            FUN_0010e334(apuStack_44);
          }
          else if (uVar16 == 1) {
            if (iVar5 != 0) {
              iVar11 = local_54 + iStack_6c;
              do {
                FUN_00116f78((int)local_70 + param_2,iVar11,iVar5,1);
                local_70 = (int *)((int)local_70 + iVar5);
                iVar5 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iStack_6c);
              } while (iVar5 != 0);
            }
          }
          else if (iVar5 != 0) {
            iStack_74 = local_54 + iStack_6c;
            do {
              puVar9 = (undefined4 *)FUN_0010e24c(local_28);
              pcVar18 = local_38;
              pcVar6 = local_60;
              uVar2 = local_64;
              apuStack_44[0] = puVar9;
              if (uVar16 < 9) {
                puVar7 = puVar9;
                puVar14 = (undefined2 *)((int)local_70 * local_5c + (int)puStack_88);
                iVar11 = iVar5;
                do {
                  (*pcVar6)(*(undefined1 *)puVar14);
                  uVar8 = (*pcVar18)();
                  iVar11 = iVar11 + -1;
                  *puVar7 = uVar8;
                  puVar7 = puVar7 + 1;
                  puVar14 = (undefined2 *)((int)puVar14 + 1);
                } while (iVar11 != 0);
              }
              else {
                puVar7 = puVar9;
                iVar11 = iVar5;
                puVar14 = (undefined2 *)((int)local_70 * local_5c + (int)puStack_88);
                do {
                  (*pcVar6)(*puVar14);
                  uVar8 = (*pcVar18)();
                  iVar11 = iVar11 + -1;
                  *puVar7 = uVar8;
                  puVar7 = puVar7 + 1;
                  puVar14 = puVar14 + 1;
                } while (iVar11 != 0);
              }
              if (uVar2 < 0x20) {
                FUN_00113f28(puVar9,iVar5,uVar2);
              }
              FUN_00115678(param_2 + (int)local_70,iStack_74,iVar5,1);
              FUN_0010e334(apuStack_44);
              local_70 = (int *)((int)local_70 + iVar5);
              iVar5 = (**(code **)(*(int *)(local_34 + 8) + 0x14))(local_34,&local_70,iStack_6c);
            } while (iVar5 != 0);
          }
          puStack_88 = (undefined2 *)((int)puStack_88 + local_84);
          iStack_6c = iStack_6c + 1;
        } while (iStack_6c < local_58);
      }
      FUN_0010e334(&local_34);
      iVar5 = local_28;
    }
  }
  else {
    local_78 = param_2;
    local_70 = (int *)FUN_0010e24c(param_1);
    sVar1 = (short)local_70[2];
    iVar11 = (int)sVar1;
    iVar15 = (int)*(short *)((int)local_70 + 10);
    puVar7 = *(undefined4 **)(*local_70 + 0x10);
    FUN_0010e334(&local_70);
    pcVar18 = (code *)puVar7[1];
    puVar9 = *(undefined4 **)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10);
    pcVar6 = (code *)*puVar9;
    uVar16 = FUN_0010eb64();
    if (uVar16 < 9) {
      iVar10 = 1;
    }
    else if (uVar16 < 0x11) {
      iVar10 = 2;
    }
    else {
      iVar10 = 4;
    }
    param_2 = local_78;
    if (puVar7 == puVar9) {
LAB_001121bc:
      FUN_00115678(param_2,param_3,iVar11,iVar15);
      return;
    }
    if (iVar5 == 0x20) {
      local_5c = FUN_0010dfe0((int)(short)iVar10 * (int)sVar1);
      if (local_5c == 0) {
        return;
      }
      local_60 = (code *)FUN_0010e24c();
      local_70 = (int *)FUN_0010e24c(param_1);
      piVar12 = local_70 + 6;
      if (puVar9[4] == 0) {
        do {
          piVar13 = piVar12;
          iVar5 = iVar11;
          pcVar18 = local_60;
          if (iVar10 == 1) {
            do {
              piVar12 = piVar13 + 1;
              cVar3 = (code)(*pcVar6)(*piVar13);
              iVar5 = iVar5 + -1;
              *pcVar18 = cVar3;
              piVar13 = piVar12;
              pcVar18 = pcVar18 + 1;
            } while (iVar5 != 0);
          }
          else if (iVar10 == 2) {
            do {
              piVar12 = piVar13 + 1;
              uVar4 = (*pcVar6)(*piVar13);
              iVar5 = iVar5 + -1;
              *(undefined2 *)pcVar18 = uVar4;
              piVar13 = piVar12;
              pcVar18 = pcVar18 + 2;
            } while (iVar5 != 0);
          }
          else if (iVar10 == 4) {
            do {
              piVar12 = piVar13 + 1;
              uVar8 = (*pcVar6)(*piVar13);
              iVar5 = iVar5 + -1;
              *(undefined4 *)pcVar18 = uVar8;
              piVar13 = piVar12;
              pcVar18 = pcVar18 + 4;
            } while (iVar5 != 0);
          }
          FUN_00115678(local_78,param_3,iVar11,1);
          iVar15 = iVar15 + -1;
          param_3 = param_3 + 1;
        } while (iVar15 != 0);
      }
      else {
        do {
          if (iVar10 == 1) {
            (*(code *)puVar9[4])(piVar12,local_60,iVar11,1);
            piVar12 = piVar12 + iVar11;
          }
          else if (iVar10 == 2) {
            (*(code *)puVar9[4])(piVar12,local_60,iVar11,2);
            piVar12 = piVar12 + iVar11;
          }
          else if (iVar10 == 4) {
            (*(code *)puVar9[4])(piVar12,local_60,iVar11,4);
            piVar12 = piVar12 + iVar11;
          }
          FUN_00115678(local_78,param_3,iVar11,1);
          iVar15 = iVar15 + -1;
          param_3 = param_3 + 1;
        } while (iVar15 != 0);
      }
      FUN_0010e334(&local_70);
      FUN_0010e334(&local_60);
      FUN_0010e0a8(local_5c);
      return;
    }
    if (iVar5 != 0x10) {
      return;
    }
    local_5c = FUN_0010dfe0((int)(short)iVar10 * (int)sVar1);
    if (local_5c == 0) {
      return;
    }
    local_60 = (code *)FUN_0010e24c();
    local_70 = (int *)FUN_0010e24c(param_1);
    piVar12 = local_70 + 6;
    iVar5 = 0;
    if (puVar9[4] != 0) {
      iVar5 = puVar7[5];
    }
    if (puVar9[4] != 0 && iVar5 != 0) {
      local_58 = FUN_0010dfe0((int)(short)local_70[2] << 2);
      if (local_58 == 0) {
        puStack_88 = (undefined2 *)0x0;
      }
      else {
        puStack_88 = (undefined2 *)FUN_0010e24c();
        if (puStack_88 != (undefined2 *)0x0) {
          do {
            pcVar6 = local_60;
            if (iVar10 == 1) {
              (*(code *)puVar7[5])(piVar12,puStack_88,iVar11,2);
              (*(code *)puVar9[4])(puStack_88,pcVar6,iVar11,1);
              piVar12 = (int *)((int)piVar12 + iVar11 * 2);
            }
            else if (iVar10 == 2) {
              (*(code *)puVar7[5])(piVar12,puStack_88,iVar11,2);
              (*(code *)puVar9[4])(puStack_88,pcVar6,iVar11,2);
              piVar12 = (int *)((int)piVar12 + iVar11 * 2);
            }
            else if (iVar10 == 4) {
              (*(code *)puVar7[5])(piVar12,puStack_88,iVar11,2);
              (*(code *)puVar9[4])(puStack_88,pcVar6,iVar11,4);
              piVar12 = (int *)((int)piVar12 + iVar11 * 2);
            }
            FUN_00115678(local_78,param_3,iVar11,1);
            iVar15 = iVar15 + -1;
            param_3 = param_3 + 1;
          } while (iVar15 != 0);
          FUN_0010e334(&puStack_88);
          FUN_0010e0a8(local_58);
        }
      }
    }
    else {
      local_64 = iVar10 << 3;
      do {
        piVar13 = piVar12;
        iVar5 = iVar11;
        pcVar17 = local_60;
        if (iVar10 == 1) {
          do {
            piVar12 = (int *)((int)piVar13 + 2);
            (*pcVar18)((short)*piVar13);
            cVar3 = (code)(*pcVar6)();
            iVar5 = iVar5 + -1;
            *pcVar17 = cVar3;
            piVar13 = piVar12;
            pcVar17 = pcVar17 + 1;
          } while (iVar5 != 0);
        }
        else if (iVar10 == 2) {
          do {
            piVar12 = (int *)((int)piVar13 + 2);
            (*pcVar18)((short)*piVar13);
            uVar4 = (*pcVar6)();
            iVar5 = iVar5 + -1;
            *(undefined2 *)pcVar17 = uVar4;
            piVar13 = piVar12;
            pcVar17 = pcVar17 + 2;
          } while (iVar5 != 0);
        }
        else if (iVar10 == 4) {
          do {
            piVar12 = (int *)((int)piVar13 + 2);
            (*pcVar18)((short)*piVar13);
            uVar8 = (*pcVar6)();
            iVar5 = iVar5 + -1;
            *(undefined4 *)pcVar17 = uVar8;
            piVar13 = piVar12;
            pcVar17 = pcVar17 + 4;
          } while (iVar5 != 0);
        }
        FUN_00115678(local_78,param_3,iVar11,1);
        iVar15 = iVar15 + -1;
        param_3 = param_3 + 1;
      } while (iVar15 != 0);
    }
    FUN_0010e334(&local_70);
    FUN_0010e334(&local_60);
    iVar5 = local_5c;
  }
  FUN_0010e0a8(iVar5);
  return;
}

// ==== FUN_00112660 @ 00112660, 16 bytes
// called by: FUN_00124a6c, idle_screen_proc, FUN_00124a88
void FUN_00112660(void)

{
  char cVar1;
  uint uVar2;
  code *pcVar3;
  int in_r3;
  int iVar4;
  undefined4 uVar5;
  int iStack_18;
  
  uVar2 = (uint)*(byte *)(DAT_00182efc + 0x11);
  if (DAT_00182f7c != 0) {
    uVar2 = 0;
  }
  cVar1 = (&DAT_00182f4c)[uVar2];
  if (cVar1 < 2) {
    return;
  }
  iVar4 = (&DAT_007bd150)[uVar2];
  (&DAT_007bd150)[uVar2] = iVar4 + 1;
  if (iVar4 + 1 != 1) {
    return;
  }
  uVar5 = (&DAT_007bd140)[uVar2];
  do {
    iVar4 = (&DAT_007bd140)[uVar2];
    (&DAT_007bd140)[uVar2] = iVar4 + 1;
    if (iVar4 + 1 == (int)cVar1) {
      (&DAT_007bd140)[uVar2] = 0;
    }
    iVar4 = (&DAT_007bd140)[uVar2];
  } while (iVar4 == (&DAT_007bd130)[uVar2]);
  iStack_18 = in_r3;
  iStack_18 = FUN_0010e7f0();
  if (iStack_18 == 0) {
    return;
  }
  pcVar3 = (code *)(**(code **)(*(int *)(iStack_18 + 0xc) + 0x24))(&iStack_18,0x21);
  if (pcVar3 == (code *)0x0) {
    return;
  }
  (*pcVar3)(iStack_18,uVar5,iVar4);
  return;
}

// ==== FUN_00112674 @ 00112674, 184 bytes
// called by: FUN_00124aa4
void FUN_00112674(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  char cVar1;
  code *pcVar2;
  int iVar3;
  undefined4 uVar4;
  int iStack_18;
  
  if (DAT_00182f7c != 0) {
    param_1 = 0;
  }
  cVar1 = (&DAT_00182f4c)[param_1];
  if (cVar1 < 2) {
    return;
  }
  iVar3 = (&DAT_007bd150)[param_1];
  (&DAT_007bd150)[param_1] = iVar3 + 1;
  if (iVar3 + 1 != 1) {
    return;
  }
  uVar4 = (&DAT_007bd140)[param_1];
  do {
    iVar3 = (&DAT_007bd140)[param_1];
    (&DAT_007bd140)[param_1] = iVar3 + 1;
    if (iVar3 + 1 == (int)cVar1) {
      (&DAT_007bd140)[param_1] = 0;
    }
    iVar3 = (&DAT_007bd140)[param_1];
  } while (iVar3 == (&DAT_007bd130)[param_1]);
  iStack_18 = param_4;
  iStack_18 = FUN_0010e7f0();
  if (iStack_18 == 0) {
    return;
  }
  pcVar2 = (code *)(**(code **)(*(int *)(iStack_18 + 0xc) + 0x24))(&iStack_18,0x21);
  if (pcVar2 == (code *)0x0) {
    return;
  }
  (*pcVar2)(iStack_18,uVar4,iVar3);
  return;
}

// ==== FUN_0011273c @ 0011273c, 84 bytes
// called by: FUN_00116e40
void FUN_0011273c(int param_1)

{
  uint uVar1;
  
  uVar1 = (uint)*(byte *)(DAT_00182efc + 0x11);
  if (DAT_00182f80 == (code *)0x0) {
    DAT_00182f80 = FUN_00124c9c;
    FUN_00115274();
  }
  if (DAT_00182f7c != 0) {
    uVar1 = 0;
  }
  if (param_1 < 1) {
    param_1 = 1;
  }
  (&DAT_00182f4c)[uVar1] = (char)param_1;
  return;
}

// ==== FUN_001127a4 @ 001127a4, 64 bytes
// called by: FUN_001173b0
void FUN_001127a4(undefined4 param_1)

{
  uint uVar1;
  
  uVar1 = (uint)*(byte *)(DAT_00182efc + 0x11);
  if (DAT_00182f7c != 0) {
    uVar1 = 0;
  }
  if ((int)(&DAT_007bd160)[uVar1] < 0) {
    return;
  }
  (&DAT_007bd130)[uVar1] = param_1;
  (&DAT_007bd160)[uVar1] = 0xffffffff;
  return;
}

// ==== FUN_001127f4 @ 001127f4, 164 bytes
// called by: idle_screen_proc
void FUN_001127f4(void)

{
  code *pcVar1;
  int iVar2;
  int in_r3;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  int iStack_18;
  
  uVar4 = (uint)*(byte *)(DAT_00182efc + 0x11);
  uVar3 = uVar4;
  if (DAT_00182f7c != 0) {
    uVar3 = 0;
  }
  if ((char)(&DAT_00182f4c)[uVar3] < '\x02') {
    return;
  }
  iVar2 = (&DAT_007bd150)[uVar3];
  (&DAT_007bd150)[uVar3] = iVar2 + -1;
  if (iVar2 + -1 != 0) {
    return;
  }
  iStack_18 = in_r3;
  FUN_0010e4c0(1 << uVar4);
  uVar5 = (&DAT_007bd140)[uVar3];
  (&DAT_007bd160)[uVar3] = uVar5;
  iStack_18 = FUN_0010e7f0(uVar4);
  if (iStack_18 == 0) {
    return;
  }
  pcVar1 = (code *)(**(code **)(*(int *)(iStack_18 + 0xc) + 0x24))(&iStack_18,0x22);
  if (pcVar1 == (code *)0x0) {
    return;
  }
  (*pcVar1)(iStack_18,uVar5);
  return;
}

// ==== FUN_001128ac @ 001128ac, 156 bytes
// called by: FUN_00124b04
void FUN_001128ac(uint param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int iStack_18;
  
  uVar3 = param_1;
  if (DAT_00182f7c != 0) {
    uVar3 = 0;
  }
  if ((char)(&DAT_00182f4c)[uVar3] < '\x02') {
    return;
  }
  iVar2 = (&DAT_007bd150)[uVar3];
  (&DAT_007bd150)[uVar3] = iVar2 + -1;
  if (iVar2 + -1 != 0) {
    return;
  }
  iStack_18 = param_4;
  FUN_0010e4c0(1 << (param_1 & 0xff));
  uVar4 = (&DAT_007bd140)[uVar3];
  (&DAT_007bd160)[uVar3] = uVar4;
  iStack_18 = FUN_0010e7f0(param_1);
  if (iStack_18 == 0) {
    return;
  }
  pcVar1 = (code *)(**(code **)(*(int *)(iStack_18 + 0xc) + 0x24))(&iStack_18,0x22);
  if (pcVar1 == (code *)0x0) {
    return;
  }
  (*pcVar1)(iStack_18,uVar4);
  return;
}

// ==== FUN_00112958 @ 00112958, 144 bytes
// called by: FUN_001183e0, FUN_00124394
void FUN_00112958(short *param_1,short *param_2,short *param_3)

{
  short *psVar1;
  short sVar2;
  
  if (param_1 == (short *)0x0) {
    return;
  }
  psVar1 = param_3;
  if (param_2 != (short *)0x0) {
    psVar1 = param_2;
  }
  if (param_2 != (short *)0x0 && param_3 != (short *)0x0) {
    sVar2 = *param_3;
    if (*psVar1 < *param_3) {
      sVar2 = *psVar1;
    }
    *param_1 = sVar2;
    sVar2 = param_3[1];
    if (psVar1[1] < param_3[1]) {
      sVar2 = psVar1[1];
    }
    param_1[1] = sVar2;
    sVar2 = param_3[2];
    if (param_3[2] < psVar1[2]) {
      sVar2 = psVar1[2];
    }
    param_1[2] = sVar2;
    sVar2 = psVar1[3];
    if (psVar1[3] <= param_3[3]) {
      sVar2 = param_3[3];
    }
    param_1[3] = sVar2;
    return;
  }
  *param_1 = *psVar1;
  param_1[1] = psVar1[1];
  param_1[2] = psVar1[2];
  param_1[3] = psVar1[3];
  return;
}

// ==== FUN_001129e8 @ 001129e8, 68 bytes
// called by: FUN_001186b4, FUN_00124b64, FUN_00118f24, FUN_00119460, FUN_001180e8
void FUN_001129e8(short *param_1,short param_2,short param_3)

{
  if (param_1 == (short *)0x0) {
    return;
  }
  *param_1 = *param_1 + param_2;
  param_1[2] = param_2 + param_1[2];
  param_1[1] = param_1[1] + param_3;
  param_1[3] = param_3 + param_1[3];
  return;
}

// ==== FUN_00112a2c @ 00112a2c, 64 bytes
// called by: FUN_001179fc
undefined4 FUN_00112a2c(void)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  if (DAT_00183008 != DAT_0018300c) {
    uVar1 = *DAT_00183008;
    puVar2 = DAT_00183008 + 1;
    if (DAT_00183008 == (undefined4 *)0x7c60a0) {
      DAT_00183008 = (undefined4 *)0x7c6050;
    }
    else {
      DAT_00183008 = DAT_00183008 + 2;
    }
    FUN_0011837c(uVar1,*puVar2);
    return 1;
  }
  return 0;
}

// ==== FUN_00112a74 @ 00112a74, 180 bytes
// called by: FUN_001146cc
void FUN_00112a74(int param_1,int param_2,int param_3,int param_4,undefined4 param_5,int *param_6)

{
  if (param_1 < *(short *)(DAT_00182efc + 8)) {
    param_1 = (int)*(short *)(DAT_00182efc + 8);
  }
  if (*(short *)(DAT_00182efc + 0xc) < param_3) {
    param_3 = (int)*(short *)(DAT_00182efc + 0xc);
  }
  if (param_3 < param_1) {
    return;
  }
  if (param_2 < *(short *)(DAT_00182efc + 10)) {
    param_2 = (int)*(short *)(DAT_00182efc + 10);
  }
  if (*(short *)(DAT_00182efc + 0xe) < param_4) {
    param_4 = (int)*(short *)(DAT_00182efc + 0xe);
  }
  if (param_4 < param_2) {
    return;
  }
  do {
    if (*(int *)param_6[3] == 0 || *(int *)param_6[3] == 4) break;
    param_6 = (int *)*param_6;
  } while (param_6 != (int *)0x0);
  FUN_00116638();
  return;
}

// ==== FUN_00112b2c @ 00112b2c, 76 bytes
// called by: FUN_001189d8, FUN_001143a8, FUN_00122b34, FUN_001196dc, FUN_0012297c, FUN_00122a60, FUN_00124e84
undefined4 FUN_00112b2c(short *param_1,short *param_2)

{
  if ((((param_1[1] <= param_2[3]) && (param_2[1] <= param_1[3])) && (*param_1 <= param_2[2])) &&
     (*param_2 <= param_1[2])) {
    return 1;
  }
  return 0;
}

// ==== FUN_00112b78 @ 00112b78, 20 bytes
// called by: FUN_001179fc
/* WARNING: Removing unreachable block (ram,0x0010cbb8) */
/* WARNING: Removing unreachable block (ram,0x0010cbb0) */
/* WARNING: Removing unreachable block (ram,0x0010cba0) */
/* WARNING: Removing unreachable block (ram,0x0010cbac) */
/* WARNING: Removing unreachable block (ram,0x0010cbb4) */
/* WARNING: Removing unreachable block (ram,0x0010cbbc) */
/* WARNING: Removing unreachable block (ram,0x0010cbc0) */

undefined8 FUN_00112b78(undefined4 *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  
  puVar3 = DAT_00182efc;
  uVar1 = 0x58;
  do {
    uVar5 = uVar1;
    puVar4 = param_1;
    puVar2 = puVar3;
    uVar6 = puVar4[1];
    uVar7 = puVar4[2];
    uVar8 = puVar4[3];
    *puVar2 = *puVar4;
    puVar2[1] = uVar6;
    puVar2[2] = uVar7;
    puVar2[3] = uVar8;
    uVar6 = puVar4[5];
    uVar7 = puVar4[6];
    uVar8 = puVar4[7];
    param_1 = puVar4 + 8;
    puVar2[4] = puVar4[4];
    puVar2[5] = uVar6;
    puVar2[6] = uVar7;
    puVar2[7] = uVar8;
    puVar3 = puVar2 + 8;
    uVar1 = uVar5 - 0x20;
  } while (0x1f < uVar5);
  if ((bool)((byte)(uVar5 - 0x20 >> 4) & 1)) {
    uVar6 = *param_1;
    uVar7 = puVar4[9];
    uVar8 = puVar4[10];
    uVar9 = puVar4[0xb];
    param_1 = puVar4 + 0xc;
    *puVar3 = uVar6;
    puVar2[9] = uVar7;
    puVar2[10] = uVar8;
    puVar2[0xb] = uVar9;
    puVar3 = puVar2 + 0xc;
  }
  if ((int)(uVar5 << 0x1c) < 0) {
    uVar6 = *param_1;
    uVar7 = param_1[1];
    param_1 = param_1 + 2;
    *puVar3 = uVar6;
    puVar3[1] = uVar7;
    puVar3 = puVar3 + 2;
  }
  return CONCAT44(param_1,puVar3);
}

// ==== FUN_00112b90 @ 00112b90, 16 bytes
// called by: FUN_001179fc
/* WARNING: Removing unreachable block (ram,0x0010cbb8) */
/* WARNING: Removing unreachable block (ram,0x0010cbb0) */
/* WARNING: Removing unreachable block (ram,0x0010cba0) */
/* WARNING: Removing unreachable block (ram,0x0010cbac) */
/* WARNING: Removing unreachable block (ram,0x0010cbb4) */
/* WARNING: Removing unreachable block (ram,0x0010cbbc) */
/* WARNING: Removing unreachable block (ram,0x0010cbc0) */

undefined8 FUN_00112b90(undefined4 *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 *puVar9;
  
  puVar9 = DAT_00182efc;
  uVar1 = 0x58;
  do {
    uVar4 = uVar1;
    puVar3 = puVar9;
    puVar2 = param_1;
    uVar5 = puVar3[1];
    uVar6 = puVar3[2];
    uVar7 = puVar3[3];
    *puVar2 = *puVar3;
    puVar2[1] = uVar5;
    puVar2[2] = uVar6;
    puVar2[3] = uVar7;
    uVar5 = puVar3[5];
    uVar6 = puVar3[6];
    uVar7 = puVar3[7];
    puVar9 = puVar3 + 8;
    puVar2[4] = puVar3[4];
    puVar2[5] = uVar5;
    puVar2[6] = uVar6;
    puVar2[7] = uVar7;
    param_1 = puVar2 + 8;
    uVar1 = uVar4 - 0x20;
  } while (0x1f < uVar4);
  if ((bool)((byte)(uVar4 - 0x20 >> 4) & 1)) {
    uVar5 = *puVar9;
    uVar6 = puVar3[9];
    uVar7 = puVar3[10];
    uVar8 = puVar3[0xb];
    puVar9 = puVar3 + 0xc;
    *param_1 = uVar5;
    puVar2[9] = uVar6;
    puVar2[10] = uVar7;
    puVar2[0xb] = uVar8;
    param_1 = puVar2 + 0xc;
  }
  if ((int)(uVar4 << 0x1c) < 0) {
    uVar5 = *puVar9;
    uVar6 = puVar9[1];
    puVar9 = puVar9 + 2;
    *param_1 = uVar5;
    param_1[1] = uVar6;
    param_1 = param_1 + 2;
  }
  return CONCAT44(puVar9,param_1);
}

// ==== FUN_00112ba4 @ 00112ba4, 20 bytes
// called by: FUN_00110c20, FUN_00112bb8, FUN_00110e78, FUN_0011147c
void FUN_00112ba4(void)

{
  FUN_0011697c();
  FUN_00116e10();
  *(undefined1 *)(DAT_00182efc + 0x50) = 1;
  return;
}

// ==== FUN_00112bb8 @ 00112bb8, 72 bytes
// called by: FUN_00110c20, FUN_00118548, FUN_001163d4
undefined1 FUN_00112bb8(uint param_1)

{
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = *(undefined1 *)(DAT_00182efc + 0x11);
  if ((param_1 < 4) && (iVar2 = FUN_0010e7f0(param_1), iVar2 != 0)) {
    *(char *)(DAT_00182efc + 0x11) = (char)param_1;
    FUN_00112ba4();
    DAT_001830c4 = param_1;
  }
  return uVar1;
}

// ==== thunk_FUN_001169f0 @ 00112c08, 4 bytes
// called by: idle_screen_proc, FUN_001179fc, FUN_00126c5c, usb_command
void thunk_FUN_001169f0(void)

{
                    /* WARNING: Could not recover jumptable at 0x001169fc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_00183084 + 4))();
  return;
}

// ==== text_set_colour @ 00112c0c, 4 bytes
// called by: idle_screen_proc, FUN_0010dce4, FUN_001179fc, usb_command
// colour from config +0xF8 (byte-swapped, top byte inverted)
/* colour from config +0xF8 (byte-swapped, top byte inverted) */

void text_set_colour(void)

{
                    /* WARNING: Could not recover jumptable at 0x00116ab4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*DAT_00183084)();
  return;
}

// ==== thunk_FUN_00116b48 @ 00112c20, 4 bytes
// called by: FUN_00110c20, FUN_0010de9c
void thunk_FUN_00116b48(void)

{
                    /* WARNING: Could not recover jumptable at 0x00116b54. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_00183084 + 8))();
  return;
}

// ==== font_select @ 00112c24, 28 bytes
// called by: idle_screen_proc
// font_select(font handle)
/* font_select(font handle) */

undefined4 font_select(int param_1)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(DAT_00182efc + 0x1c);
  if (param_1 != 0) {
    *(int *)(DAT_00182efc + 0x1c) = param_1;
  }
  return uVar1;
}

// ==== FUN_00112c44 @ 00112c44, 60 bytes
// called by: FUN_0010ece4
void FUN_00112c44(undefined4 param_1,undefined4 param_2)

{
  DAT_00182f5c = (undefined2)param_1;
  DAT_00182f5e = (undefined2)param_2;
                    /* WARNING: Could not emulate address calculation at 0x00112c64 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x20))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2);
  return;
}

// ==== display_set_rotation @ 00112c90, 604 bytes
// called by: config_load, usb_command, screen_create_proc
// applied at boot with table[config[1] & 3]
/* applied at boot with table[config[1] & 3] */

undefined4 display_set_rotation(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined1 *puVar7;
  undefined **ppuVar8;
  int iVar9;
  
  uVar1 = *(undefined1 *)(DAT_00182efc + 0x11);
  iVar2 = FUN_00115c8c(uVar1);
  if (iVar2 < 8) {
    ppuVar8 = &PTR_FUN_00181e88;
  }
  else if (iVar2 == 8) {
    ppuVar8 = &PTR_FUN_00181ee8;
  }
  else if (iVar2 < 0x11) {
    ppuVar8 = &PTR_FUN_00181ea8;
  }
  else {
    ppuVar8 = &PTR_FUN_00181ec8;
  }
  iVar2 = FUN_0010e7b0(uVar1,0);
  if (iVar2 == 0) {
    return 1;
  }
  if (*(undefined **)(iVar2 + 0xc) == &DAT_00181628) {
    iVar9 = *(int *)(iVar2 + 8);
    if (*(int *)(iVar9 + 4) != 0) {
      FUN_0010e104();
    }
    FUN_0010e104(iVar9);
    FUN_0010e754(iVar2);
    thunk_FUN_0010e104(iVar2);
  }
  if (param_1 == 0) {
    return 1;
  }
  uVar3 = FUN_001160c0(uVar1);
  uVar4 = FUN_001161b8(uVar1);
  iVar5 = FUN_00115f8c(uVar1);
  iVar2 = FUN_00116028(uVar1);
  iVar9 = FUN_0010e7f0(uVar1);
  if (iVar9 == 0) {
    return 1;
  }
  iVar9 = FUN_0010e4d8(&DAT_00181628,*(undefined4 *)(iVar9 + 0x10),1,uVar1);
  if (iVar9 == 0) {
    return 1;
  }
  FUN_0010e634();
  puVar6 = (undefined4 *)FUN_0010e12c(0x2c);
  *(undefined4 **)(iVar9 + 8) = puVar6;
  if (puVar6 == (undefined4 *)0x0) {
    FUN_0010e754(iVar9);
    thunk_FUN_0010e104(iVar9);
    return 1;
  }
  puVar6[4] = param_1;
  puVar6[5] = uVar3;
  puVar6[6] = uVar4;
  puVar6[7] = iVar5;
  puVar6[8] = iVar2;
  puVar6[10] = ppuVar8;
  switch(param_1) {
  case 1:
    puVar6[9] = 0xffffffff;
    *puVar6 = &LAB_00124a00;
    break;
  case 2:
    puVar6[9] = 1;
    *puVar6 = &LAB_00124a4c;
    break;
  case 3:
    puVar6[9] = 0xffffffff;
    *puVar6 = &LAB_00124a1c;
    break;
  case 4:
    puVar7 = &LAB_00124988;
    puVar6[9] = iVar5;
    goto LAB_00112ea4;
  case 5:
    puVar7 = &LAB_00124998;
    puVar6[9] = iVar5;
LAB_00112ea4:
    *puVar6 = puVar7;
    break;
  case 6:
    puVar6[9] = -iVar5;
    *puVar6 = &LAB_001249e4;
    break;
  case 7:
    puVar6[9] = -iVar5;
    *puVar6 = &LAB_001249b4;
  }
  if (ppuVar8[7] != (undefined *)0x0) {
    iVar5 = iVar5 * (int)ppuVar8[7];
    puVar6[3] = iVar5;
    iVar2 = FUN_0010e12c(iVar2 * iVar5);
    puVar6[1] = iVar2;
    if (iVar2 == 0) {
      FUN_0010e754(iVar9);
      thunk_FUN_0010e104(iVar9);
      FUN_0010e104(puVar6);
      return 1;
    }
  }
  return 0;
}

// ==== FUN_00112f40 @ 00112f40, 24 bytes
// called by: idle_screen_proc, usb_command
int FUN_00112f40(undefined2 param_1)

{
  short sVar1;
  
  sVar1 = *(short *)(DAT_00182efc + 0x2a);
  *(undefined2 *)(DAT_00182efc + 0x2a) = param_1;
  return (int)sVar1;
}

// ==== FUN_00112f5c @ 00112f5c, 20 bytes
// called by: FUN_00124394
void FUN_00112f5c(void)

{
  if (DAT_00183034 == (code *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00112f6c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_00183034)();
  return;
}

// ==== FUN_00112f8c @ 00112f8c, 300 bytes
// called by: FUN_001188c4
void FUN_00112f8c(int param_1)

{
  int *piVar1;
  int iVar2;
  int local_20;
  int local_1c;
  int local_18 [2];
  
  if (param_1 == 0) {
    return;
  }
  piVar1 = &local_20;
  iVar2 = DAT_00183100;
  if (DAT_00183100 == param_1) {
    local_20 = FUN_0010e24c();
    DAT_00183100 = *(int *)(local_20 + 8);
LAB_0011300c:
    FUN_0010e334(piVar1);
  }
  else {
    while (iVar2 != 0) {
      local_1c = FUN_0010e24c(iVar2);
      iVar2 = *(int *)(local_1c + 8);
      if (iVar2 == param_1) {
        local_20 = FUN_0010e24c();
        *(undefined4 *)(local_1c + 8) = *(undefined4 *)(local_20 + 8);
        FUN_0010e334(&local_20);
        piVar1 = &local_1c;
        goto LAB_0011300c;
      }
      FUN_0010e334(&local_1c);
    }
  }
  if (param_1 != 0) {
    local_18[0] = FUN_0010e24c(param_1);
    iVar2 = DAT_00183104;
    if (DAT_00183104 == param_1) {
      DAT_00183104 = *(int *)(local_18[0] + 4);
      FUN_0010e334(local_18);
    }
    else {
      while (iVar2 != 0) {
        local_20 = FUN_0010e24c(iVar2);
        iVar2 = *(int *)(local_20 + 4);
        if (iVar2 == param_1) {
          *(undefined4 *)(local_20 + 4) = *(undefined4 *)(local_18[0] + 4);
          FUN_0010e334(&local_20);
          break;
        }
        FUN_0010e334(&local_20);
      }
      FUN_0010e334(local_18);
    }
  }
  if (param_1 != 0) {
    iVar2 = param_1 * 8 + 4;
    FUN_00119b8c(*(undefined4 *)(*(int *)(DAT_007bd010 + iVar2) + 4));
    DAT_007bcfe0 = DAT_007bcfe0 + 1;
    *(int *)(DAT_007bd010 + iVar2) = DAT_007bd000;
    *(undefined1 *)(DAT_007bd010 + param_1 * 8) = 0;
    DAT_007bd000 = param_1;
    return;
  }
  return;
}

// ==== FUN_001130bc @ 001130bc, 60 bytes
// called by: FUN_001188c4
bool FUN_001130bc(int param_1,ushort param_2,undefined4 param_3,int param_4)

{
  bool bVar1;
  int local_10;
  
  if (param_1 == 0) {
    bVar1 = true;
  }
  else {
    local_10 = param_4;
    local_10 = FUN_0010e24c();
    bVar1 = (*(ushort *)(local_10 + 0xc) & param_2) != 0;
    FUN_0010e334(&local_10);
  }
  return bVar1;
}

// ==== FUN_001130f8 @ 001130f8, 68 bytes
// called by: FUN_001188c4
int FUN_001130f8(undefined4 *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  *param_1 = 0;
  iVar1 = DAT_00183100;
  if (DAT_00183100 != 0) {
    local_10 = param_4;
    local_10 = FUN_0010e24c(DAT_00183100);
    *param_1 = *(undefined4 *)(local_10 + 0x10);
    FUN_0010e334(&local_10);
  }
  return iVar1;
}

// ==== FUN_00113140 @ 00113140, 92 bytes
// called by: FUN_001188c4
int FUN_00113140(int param_1,undefined4 *param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  *param_2 = 0;
  iVar1 = 0;
  if (param_1 != 0) {
    local_10 = param_4;
    local_10 = FUN_0010e24c(param_1);
    iVar1 = *(int *)(local_10 + 8);
    if (iVar1 != 0) {
      FUN_0010e334(&local_10);
      local_10 = FUN_0010e24c(iVar1);
      *param_2 = *(undefined4 *)(local_10 + 0x10);
    }
    FUN_0010e334(&local_10);
  }
  return iVar1;
}

// ==== FUN_0011319c @ 0011319c, 2044 bytes
// called by: FUN_001139a0
/* WARNING: Type propagation algorithm not settling */

undefined4
FUN_0011319c(int param_1,int *param_2,int *param_3,int *param_4,int *param_5,int param_6,int param_7
            )

{
  uint uVar1;
  undefined4 *******pppppppuVar2;
  undefined4 *******pppppppuVar3;
  undefined4 *******pppppppuVar4;
  undefined4 *******pppppppuVar5;
  undefined4 *******pppppppuVar6;
  undefined4 *******pppppppuVar7;
  undefined4 *******pppppppuVar8;
  undefined4 *******pppppppuVar9;
  undefined4 *******pppppppuVar10;
  undefined4 *******pppppppuVar11;
  undefined4 *******pppppppuVar12;
  undefined4 ******ppppppuVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  uint uVar18;
  undefined4 uVar19;
  undefined4 uVar20;
  undefined4 uVar21;
  int iVar22;
  int iVar23;
  int unaff_r4;
  int iVar24;
  undefined4 *******pppppppuVar25;
  undefined4 *******unaff_r11;
  undefined4 *******local_74 [3];
  int local_68;
  undefined4 *******local_5c [4];
  undefined4 *******local_4c;
  undefined4 *******local_48;
  undefined4 *******local_44;
  undefined4 *******local_40;
  undefined4 *******local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  
  pppppppuVar25 = (undefined4 *******)0x0;
  local_68 = 0;
  DAT_00183018 = 0;
  if (param_1 == 2) {
    iVar22 = *param_4;
    uVar14 = FUN_0012edc0(param_4[1] - iVar22);
    uVar15 = FUN_0012edc0(param_2[1] - *param_2);
    uVar18 = FUN_0012eb14(uVar15,uVar14);
    DAT_007c60ac = 0;
    DAT_007c60a8 = uVar18;
    uVar14 = FUN_0012edc0(iVar22);
    uVar18 = FUN_0012ee9c(uVar14,uVar18);
    DAT_007c60b0 = uVar18 ^ 0x80000000;
    DAT_007c60b4 = 0;
    iVar22 = *param_5;
    uVar14 = FUN_0012edc0(param_5[1] - iVar22);
    uVar15 = FUN_0012edc0(param_3[1] - *param_3);
    uVar18 = FUN_0012eb14(uVar15,uVar14);
    DAT_007c60b8 = uVar18;
    uVar14 = FUN_0012edc0(iVar22);
    uVar18 = FUN_0012ee9c(uVar14,uVar18);
    DAT_007c60bc = uVar18 ^ 0x80000000;
    DAT_00183018 = 1;
  }
  else {
    if (2 < param_1) {
      unaff_r4 = 0;
    }
    if (param_1 == 3) {
      pppppppuVar25 = &local_44;
      unaff_r11 = local_5c + 3;
    }
    if (param_1 < 3) {
      DAT_00183018 = 0;
      return 1;
    }
    pppppppuVar2 = pppppppuVar25;
    pppppppuVar3 = pppppppuVar25;
    pppppppuVar4 = pppppppuVar25;
    pppppppuVar5 = pppppppuVar25;
    pppppppuVar6 = pppppppuVar25;
    pppppppuVar7 = pppppppuVar25;
    pppppppuVar8 = pppppppuVar25;
    pppppppuVar9 = pppppppuVar25;
    pppppppuVar10 = pppppppuVar25;
    pppppppuVar11 = pppppppuVar25;
    pppppppuVar12 = pppppppuVar25;
    if (param_1 != 3) {
      do {
        local_3c = pppppppuVar12;
        local_40 = pppppppuVar11;
        local_44 = pppppppuVar10;
        local_48 = pppppppuVar9;
        local_4c = pppppppuVar8;
        local_5c[3] = pppppppuVar7;
        local_5c[2] = pppppppuVar6;
        local_5c[1] = pppppppuVar5;
        local_5c[0] = pppppppuVar4;
        local_74[2] = pppppppuVar3;
        local_74[1] = pppppppuVar2;
        local_74[0] = pppppppuVar25;
        uVar14 = FUN_0012edc0(param_4[unaff_r4]);
        local_3c = (undefined4 *******)FUN_0012ea28(uVar14,local_3c);
        uVar15 = FUN_0012edc0(param_5[unaff_r4]);
        local_48 = (undefined4 *******)FUN_0012ea28(uVar15,local_48);
        local_34 = FUN_0012edc0(param_2[unaff_r4]);
        local_5c[2] = (undefined4 *******)FUN_0012ea28(local_34,local_5c[2]);
        local_38 = FUN_0012edc0(param_3[unaff_r4]);
        local_74[2] = (undefined4 *******)FUN_0012ea28(local_38,local_74[2]);
        uVar16 = FUN_0012ee9c(uVar14,uVar14);
        local_44 = (undefined4 *******)FUN_0012ea28(uVar16,local_44);
        uVar16 = FUN_0012ee9c(uVar14,uVar15);
        local_5c[3] = (undefined4 *******)FUN_0012ea28(uVar16,local_40);
        local_40 = local_5c[3];
        uVar16 = FUN_0012ee9c(uVar15,uVar15);
        local_4c = (undefined4 *******)FUN_0012ea28(uVar16,local_4c);
        uVar16 = FUN_0012ee9c(uVar14,local_34);
        local_5c[0] = (undefined4 *******)FUN_0012ea28(uVar16,local_5c[0]);
        uVar16 = FUN_0012ee9c(uVar15,local_34);
        local_5c[1] = (undefined4 *******)FUN_0012ea28(uVar16,local_5c[1]);
        uVar14 = FUN_0012ee9c(uVar14,local_38);
        local_74[0] = (undefined4 *******)FUN_0012ea28(uVar14,local_74[0]);
        uVar14 = FUN_0012ee9c(uVar15,local_38);
        local_74[1] = (undefined4 *******)FUN_0012ea28(uVar14,local_74[1]);
        unaff_r4 = unaff_r4 + 1;
        pppppppuVar25 = local_74[0];
        pppppppuVar2 = local_74[1];
        pppppppuVar3 = local_74[2];
        pppppppuVar4 = local_5c[0];
        pppppppuVar5 = local_5c[1];
        pppppppuVar6 = local_5c[2];
        pppppppuVar7 = local_5c[3];
        pppppppuVar8 = local_4c;
        pppppppuVar9 = local_48;
        pppppppuVar10 = local_44;
        pppppppuVar11 = local_40;
        pppppppuVar12 = local_3c;
      } while (unaff_r4 < param_1);
      local_44 = (undefined4 *******)FUN_0012eb14(local_44,local_3c);
      local_40 = (undefined4 *******)FUN_0012eb14(local_40,local_48);
      local_5c[3] = (undefined4 *******)FUN_0012eb14(local_5c[3],local_3c);
      local_4c = (undefined4 *******)FUN_0012eb14(local_4c,local_48);
      local_5c[0] = (undefined4 *******)FUN_0012eb14(local_5c[0],local_3c);
      local_5c[1] = (undefined4 *******)FUN_0012eb14(local_5c[1],local_48);
      local_74[0] = (undefined4 *******)FUN_0012eb14(local_74[0],local_3c);
      local_74[1] = (undefined4 *******)FUN_0012eb14(local_74[1],local_48);
      uVar14 = FUN_0012edc0(param_1);
      local_3c = (undefined4 *******)FUN_0012eb14(local_3c,uVar14);
      local_48 = (undefined4 *******)FUN_0012eb14(local_48,uVar14);
      local_5c[2] = (undefined4 *******)FUN_0012eb14(local_5c[2],uVar14);
      local_74[2] = (undefined4 *******)FUN_0012eb14(local_74[2],uVar14);
    }
    else {
      do {
        ppppppuVar13 = (undefined4 ******)FUN_0012edc0(param_4[unaff_r4]);
        pppppppuVar25[unaff_r4] = ppppppuVar13;
        ppppppuVar13 = (undefined4 ******)FUN_0012edc0(param_5[unaff_r4]);
        unaff_r11[unaff_r4] = ppppppuVar13;
        ppppppuVar13 = (undefined4 ******)FUN_0012edc0(param_2[unaff_r4]);
        local_5c[unaff_r4] = (undefined4 *******)ppppppuVar13;
        ppppppuVar13 = (undefined4 ******)FUN_0012edc0(param_3[unaff_r4]);
        local_74[unaff_r4] = (undefined4 *******)ppppppuVar13;
        unaff_r4 = unaff_r4 + 1;
      } while (unaff_r4 < 3);
    }
    uVar14 = FUN_0012f0c8(local_5c[3],local_48);
    uVar15 = FUN_0012f0c8(local_40,local_3c);
    uVar15 = FUN_0012ee9c(uVar15,uVar14);
    uVar16 = FUN_0012f0c8(local_4c,local_48);
    uVar17 = FUN_0012f0c8(local_44,local_3c);
    uVar16 = FUN_0012ee9c(uVar17,uVar16);
    uVar18 = FUN_0012f0c8(uVar16,uVar15);
    if ((uVar18 & 0x7fffffff) >> 0x17 == 0) {
      if (local_68 == 0) {
        return 1;
      }
    }
    else {
      uVar15 = FUN_0012f0c8(local_5c[1],local_5c[2]);
      uVar16 = FUN_0012ee9c(uVar15,uVar14);
      uVar17 = FUN_0012f0c8(local_4c,local_48);
      uVar19 = FUN_0012f0c8(local_5c[0],local_5c[2]);
      uVar20 = FUN_0012ee9c(uVar19,uVar17);
      uVar16 = FUN_0012f0c8(uVar20,uVar16);
      DAT_007c60a8 = FUN_0012eb14(uVar16,uVar18);
      uVar16 = FUN_0012f0c8(local_40,local_3c);
      uVar19 = FUN_0012ee9c(uVar19,uVar16);
      local_30 = FUN_0012f0c8(local_44,local_3c);
      uVar15 = FUN_0012ee9c(uVar15,local_30);
      uVar15 = FUN_0012f0c8(uVar15,uVar19);
      DAT_007c60ac = FUN_0012eb14(uVar15,uVar18);
      uVar15 = FUN_0012ee9c(local_44,local_5c[1]);
      uVar19 = FUN_0012ee9c(local_40,local_5c[0]);
      uVar15 = FUN_0012f0c8(uVar19,uVar15);
      uVar15 = FUN_0012ee9c(uVar15,local_48);
      uVar19 = FUN_0012ee9c(local_3c,local_5c[0]);
      uVar20 = FUN_0012ee9c(local_44,local_5c[2]);
      uVar19 = FUN_0012f0c8(uVar20,uVar19);
      uVar19 = FUN_0012ee9c(uVar19,local_4c);
      uVar20 = FUN_0012ee9c(local_40,local_5c[2]);
      uVar21 = FUN_0012ee9c(local_3c,local_5c[1]);
      uVar20 = FUN_0012f0c8(uVar21,uVar20);
      uVar20 = FUN_0012ee9c(uVar20,local_5c[3]);
      uVar19 = FUN_0012ea28(uVar20,uVar19);
      uVar15 = FUN_0012ea28(uVar19,uVar15);
      DAT_007c60b0 = FUN_0012eb14(uVar15,uVar18);
      uVar15 = FUN_0012f0c8(local_74[1],local_74[2]);
      uVar14 = FUN_0012ee9c(uVar15,uVar14);
      uVar19 = FUN_0012f0c8(local_74[0],local_74[2]);
      uVar17 = FUN_0012ee9c(uVar19,uVar17);
      uVar14 = FUN_0012f0c8(uVar17,uVar14);
      DAT_007c60b4 = FUN_0012eb14(uVar14,uVar18);
      uVar14 = FUN_0012ee9c(uVar19,uVar16);
      uVar15 = FUN_0012ee9c(uVar15,local_30);
      uVar14 = FUN_0012f0c8(uVar15,uVar14);
      DAT_007c60b8 = FUN_0012eb14(uVar14,uVar18);
      uVar14 = FUN_0012ee9c(local_44,local_74[1]);
      uVar15 = FUN_0012ee9c(local_40,local_74[0]);
      uVar14 = FUN_0012f0c8(uVar15,uVar14);
      uVar14 = FUN_0012ee9c(uVar14,local_48);
      uVar15 = FUN_0012ee9c(local_3c,local_74[0]);
      uVar16 = FUN_0012ee9c(local_44,local_74[2]);
      uVar15 = FUN_0012f0c8(uVar16,uVar15);
      uVar15 = FUN_0012ee9c(uVar15,local_4c);
      uVar16 = FUN_0012ee9c(local_40,local_74[2]);
      uVar17 = FUN_0012ee9c(local_3c,local_74[1]);
      uVar16 = FUN_0012f0c8(uVar17,uVar16);
      uVar16 = FUN_0012ee9c(uVar16,local_5c[3]);
      uVar15 = FUN_0012ea28(uVar16,uVar15);
      uVar14 = FUN_0012ea28(uVar15,uVar14);
      DAT_007c60bc = FUN_0012eb14(uVar14,uVar18);
      DAT_00183018 = 1;
    }
  }
  uVar18 = DAT_007c60a8;
  DAT_0018301c = param_6;
  DAT_00183020 = param_7;
  if ((DAT_007c60a8 & 0x7fffffff) >> 0x17 == 0) {
    iVar22 = -1;
  }
  else {
    uVar14 = FUN_0012ee9c(0,DAT_007c60ac);
    uVar14 = FUN_0012f0c8(0,uVar14);
    uVar14 = FUN_0012f0c8(uVar14,DAT_007c60b0);
    uVar14 = FUN_0012f0c8(uVar14,0x3f000000);
    FUN_0012eb14(uVar14,uVar18);
    iVar22 = FUN_0012ed28();
  }
  uVar1 = DAT_007c60b8;
  DAT_00183024 = iVar22;
  if ((DAT_007c60b8 & 0x7fffffff) >> 0x17 == 0) {
    iVar24 = -1;
  }
  else {
    uVar14 = FUN_0012ee9c(0,DAT_007c60b4);
    uVar14 = FUN_0012f0c8(0,uVar14);
    uVar14 = FUN_0012f0c8(uVar14,DAT_007c60bc);
    uVar14 = FUN_0012f0c8(uVar14,0x3f000000);
    FUN_0012eb14(uVar14,uVar1);
    iVar24 = FUN_0012ed28();
  }
  DAT_00183028 = iVar24;
  if ((uVar18 & 0x7fffffff) >> 0x17 == 0) {
    iVar23 = -1;
  }
  else {
    uVar14 = FUN_0012edc0();
    uVar14 = FUN_0012ee9c(uVar14,DAT_007c60ac);
    uVar15 = FUN_0012edc0(param_6 + -1);
    uVar14 = FUN_0012f0c8(uVar15,uVar14);
    uVar14 = FUN_0012f0c8(uVar14,DAT_007c60b0);
    uVar14 = FUN_0012f0c8(uVar14,0x3f000000);
    FUN_0012eb14(uVar14,uVar18);
    iVar23 = FUN_0012ed28();
  }
  DAT_0018302c = iVar23;
  if ((uVar1 & 0x7fffffff) >> 0x17 == 0) {
    DAT_00183030 = -1;
  }
  else {
    uVar14 = FUN_0012edc0();
    uVar14 = FUN_0012ee9c(uVar14,DAT_007c60b4);
    uVar15 = FUN_0012edc0(param_7 + -1);
    uVar14 = FUN_0012f0c8(uVar15,uVar14);
    uVar14 = FUN_0012f0c8(uVar14,DAT_007c60bc);
    uVar14 = FUN_0012f0c8(uVar14,0x3f000000);
    FUN_0012eb14(uVar14,uVar1);
    DAT_00183030 = FUN_0012ed28();
  }
  if (iVar23 < iVar22) {
    DAT_00183024 = iVar23;
    DAT_0018302c = iVar22;
  }
  if (DAT_00183030 < iVar24) {
    DAT_00183028 = DAT_00183030;
    DAT_00183030 = iVar24;
  }
  if (DAT_00183018 == 0) {
    return 1;
  }
  return 0;
}

// ==== FUN_001139a0 @ 001139a0, 256 bytes
// called by: FUN_00116e40
undefined4
FUN_001139a0(uint param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint uVar3;
  bool bVar4;
  
  if (param_1 == 0) {
    bVar4 = (~DAT_00182fa0 & 6) != 0;
    uVar3 = param_2;
    if (bVar4) {
      uVar3 = DAT_00182fa0 & 5;
    }
    DAT_00182fa4 = param_2;
    DAT_00182fa8 = param_3;
    if (bVar4 && uVar3 != 1) {
      DAT_00182fb8 = param_5;
      DAT_00182fb4 = param_4;
    }
    else {
      DAT_00182fb4 = param_5;
      DAT_00182fb8 = param_4;
    }
  }
  else {
    if (param_1 != 1) {
      return 1;
    }
    bVar4 = (~DAT_00182fa0 & 5) != 0;
    uVar3 = param_2;
    if (bVar4) {
      uVar3 = DAT_00182fa0 & 6;
    }
    DAT_00182fac = param_2;
    DAT_00182fb0 = param_3;
    if (bVar4 && uVar3 != 2) {
      DAT_00182fc0 = param_5;
      DAT_00182fbc = param_4;
    }
    else {
      DAT_00182fbc = param_5;
      DAT_00182fc0 = param_4;
    }
  }
  uVar1 = 0;
  uVar3 = 1 << (param_1 & 0xff);
  if ((uVar3 & DAT_00182f8c) != 0) {
    DAT_00182f8c = 0;
  }
  uVar3 = uVar3 & 0xff | (uint)DAT_00182f8c;
  DAT_00182f8c = (byte)uVar3;
  if (uVar3 == 3) {
    uVar1 = FUN_001160c0(*(undefined1 *)(DAT_00182efc + 0x11));
    uVar2 = FUN_001161b8(*(undefined1 *)(DAT_00182efc + 0x11));
    uVar1 = FUN_0011319c(2,&DAT_00182fa4,&DAT_00182fac,&DAT_00182fb4,&DAT_00182fbc,uVar1,uVar2);
  }
  return uVar1;
}

// ==== FUN_00113ab4 @ 00113ab4, 16 bytes
// called by: idle_screen_proc
void FUN_00113ab4(void)

{
  DAT_00182f04 = &PTR_LAB_0018165c;
  return;
}

// ==== FUN_00113acc @ 00113acc, 68 bytes
// called by: FUN_001143a8, FUN_0010dce4, FUN_00114d70, FUN_00114ca0
undefined4 FUN_00113acc(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = *param_1;
  uVar1 = (*(code *)*DAT_00182f04)(iVar3);
  iVar2 = (*(code *)DAT_00182f04[1])(iVar3);
  *param_1 = iVar2 + iVar3;
  return uVar1;
}

// ==== FUN_00113b14 @ 00113b14, 64 bytes
// called by: text_draw
int FUN_00113b14(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *unaff_r7;
  bool bVar3;
  
  bVar3 = param_2 != 0;
  if (bVar3) {
    unaff_r7 = &DAT_00182f04;
  }
  iVar2 = 0;
  while (bVar3) {
    param_2 = param_2 + -1;
    iVar1 = (**(code **)(*unaff_r7 + 4))(param_1);
    bVar3 = param_2 != 0;
    param_1 = param_1 + iVar1;
    iVar2 = iVar2 + iVar1;
  }
  return iVar2;
}

// ==== FUN_00113b58 @ 00113b58, 68 bytes
// called by: FUN_0011dd60, FUN_001221a8, FUN_0011c47c, FUN_00121ffc, FUN_00121e84, FUN_0011d0f4
void FUN_00113b58(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5)

{
  do {
    (**(code **)(*(int *)(param_1 + 8) + 4))(param_1,param_2,param_3,param_4);
    param_5 = param_5 + -1;
    param_3 = param_3 + 1;
  } while (param_5 != 0);
  return;
}

// ==== FUN_00113b9c @ 00113b9c, 164 bytes
void FUN_00113b9c(short *param_1,int param_2,int param_3,int param_4)

{
  byte bVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  
  uVar3 = param_2 - *param_1;
  if ((int)uVar3 < (int)param_1[2]) {
    uVar5 = (param_4 + uVar3) - 1;
    iVar4 = ((int)uVar5 >> 3) - ((int)uVar3 >> 3);
    pbVar6 = (byte *)((int)param_1 +
                     ((int)uVar3 >> 3) + (param_3 - param_1[1]) * *(int *)(param_1 + 8) + 0x14);
    bVar1 = (byte)(0xff >> (uVar3 & 7));
    bVar2 = (byte)(0xff << (7 - (uVar5 & 7) & 0xff));
    if (iVar4 == 0) {
      bVar2 = bVar1 & bVar2 | *pbVar6;
      pbVar7 = pbVar6;
    }
    else {
      iVar4 = iVar4 + -1;
      pbVar7 = pbVar6 + 1;
      *pbVar6 = bVar1 | *pbVar6;
      if (0 < iVar4) {
        FUN_0010cbe0(pbVar7,iVar4,0xff);
        pbVar7 = pbVar7 + iVar4;
      }
      bVar2 = *pbVar7 | bVar2;
    }
    *pbVar7 = bVar2;
    return;
  }
  return;
}

// ==== FUN_00113c90 @ 00113c90, 112 bytes
// called by: FUN_00111618
int FUN_00113c90(undefined2 param_1,undefined2 param_2,int param_3,undefined2 *param_4)

{
  int iVar1;
  int iVar2;
  undefined2 *puStack_20;
  
  iVar1 = (param_3 + 0xf >> 4) * 2;
  puStack_20 = param_4;
  iVar2 = FUN_0010dfec((int)param_4 * iVar1 + 0x14);
  if (iVar2 == 0) {
    return 0;
  }
  puStack_20 = (undefined2 *)FUN_0010e24c();
  *puStack_20 = param_1;
  puStack_20[1] = param_2;
  puStack_20[2] = (short)param_3;
  puStack_20[3] = (short)param_4;
  puStack_20[6] = 1;
  *(undefined ***)(puStack_20 + 4) = &PTR_LAB_0018236c;
  *(int *)(puStack_20 + 8) = iVar1;
  FUN_0010e334(&puStack_20);
  return iVar2;
}

// ==== FUN_00113e2c @ 00113e2c, 60 bytes
// called by: FUN_00110e78
void FUN_00113e2c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c();
  iVar1 = (*(ushort *)(local_10 + 0xc) - 1) * 0x10000;
  *(short *)(local_10 + 0xc) = (short)((uint)iVar1 >> 0x10);
  FUN_0010e334(&local_10);
  if (iVar1 >> 0x10 == 0) {
    FUN_0010e0a8(param_1);
  }
  return;
}

// ==== FUN_00113e68 @ 00113e68, 40 bytes
// called by: FUN_0010ece4
void FUN_00113e68(void)

{
  FUN_0010e094(&DAT_00183130,0x200000);
  DAT_00182ef0 = &PTR_FUN_00181d94;
  return;
}

// ==== FUN_00113e8c @ 00113e8c, 32 bytes
// called by: FUN_0010e82c
void FUN_00113e8c(int param_1)

{
  do {
  } while (0 < param_1);
  return;
}

// ==== FUN_00113eb4 @ 00113eb4, 12 bytes
// called by: FUN_0010e82c
undefined4 FUN_00113eb4(void)

{
  return DAT_001823f8;
}

// ==== FUN_00113ec4 @ 00113ec4, 4 bytes
// called by: FUN_0010ece4
void FUN_00113ec4(void)

{
  return;
}

// ==== FUN_00113ec8 @ 00113ec8, 44 bytes
// called by: FUN_001265c8
void FUN_00113ec8(int param_1)

{
  undefined1 *puVar1;
  undefined1 auStack_10 [4];
  undefined1 auStack_c [4];
  undefined1 auStack_8 [8];
  
  if (param_1 == 0) {
    puVar1 = (undefined1 *)0x0;
  }
  else {
    puVar1 = auStack_8;
  }
  FUN_00114b88(auStack_10,auStack_c,puVar1,0);
  return;
}

// ==== FUN_00113ef4 @ 00113ef4, 48 bytes
// called by: FUN_00111208
undefined1 FUN_00113ef4(int param_1)

{
  undefined1 uVar1;
  undefined4 uVar2;
  
  uVar1 = DAT_00182ee9;
  DAT_00182ee9 = param_1 != 0;
  if ((bool)DAT_00182ee9) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0xff000000;
  }
  FUN_00115618(uVar2);
  return uVar1;
}

// ==== FUN_00113f28 @ 00113f28, 552 bytes
// called by: FUN_001119fc
undefined4 FUN_00113f28(byte *param_1,uint param_2,int param_3)

{
  undefined2 uVar1;
  byte *pbVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  byte *pbVar6;
  undefined4 uVar7;
  
  if (param_3 < 9) {
    pbVar2 = param_1;
    if (param_3 == 1) {
      do {
        bVar5 = *param_1 << 7;
        uVar4 = 0;
        pbVar6 = param_1 + 4;
        if (param_2 != 1) {
          bVar5 = bVar5 | (param_1[4] & 1) << 6;
          uVar4 = 0;
          pbVar6 = param_1 + 8;
          if (param_2 != 2) {
            bVar5 = bVar5 | (param_1[8] & 1) << 5;
            uVar4 = 0;
            pbVar6 = param_1 + 0xc;
            if (param_2 != 3) {
              bVar5 = bVar5 | (param_1[0xc] & 1) << 4;
              uVar4 = 0;
              pbVar6 = param_1 + 0x10;
              if (param_2 != 4) {
                bVar5 = bVar5 | (param_1[0x10] & 1) << 3;
                uVar4 = 0;
                pbVar6 = param_1 + 0x14;
                if (param_2 != 5) {
                  bVar5 = bVar5 | (param_1[0x14] & 1) << 2;
                  uVar4 = 0;
                  pbVar6 = param_1 + 0x18;
                  if (param_2 != 6) {
                    uVar4 = param_2 - 7;
                    bVar5 = bVar5 | (param_1[0x18] & 1) << 1;
                    pbVar6 = param_1 + 0x1c;
                    if (uVar4 != 0) {
                      pbVar6 = param_1 + 0x20;
                      bVar5 = bVar5 | param_1[0x1c] & 1;
                    }
                  }
                }
              }
            }
          }
        }
        *pbVar2 = bVar5;
        param_1 = pbVar6;
        param_2 = uVar4;
        pbVar2 = pbVar2 + 1;
      } while (uVar4 != 0);
    }
    else if (param_3 == 2) {
      do {
        bVar5 = *param_1 << 6;
        uVar4 = 0;
        pbVar6 = param_1 + 4;
        if (param_2 != 1) {
          bVar5 = bVar5 | (param_1[4] & 3) << 4;
          uVar4 = 0;
          pbVar6 = param_1 + 8;
          if (param_2 != 2) {
            bVar5 = bVar5 | (param_1[8] & 3) << 2;
            uVar4 = 0;
            pbVar6 = param_1 + 0xc;
            if (param_2 != 3) {
              pbVar6 = param_1 + 0x10;
              uVar4 = param_2 - 4;
              bVar5 = bVar5 | param_1[0xc] & 3;
            }
          }
        }
        *pbVar2 = bVar5;
        param_1 = pbVar6;
        param_2 = uVar4;
        pbVar2 = pbVar2 + 1;
      } while (uVar4 != 0);
    }
    else if (param_3 == 4) {
      do {
        bVar5 = *param_1 << 4;
        uVar4 = 0;
        pbVar6 = param_1 + 4;
        if (param_2 != 1) {
          pbVar6 = param_1 + 8;
          uVar4 = param_2 - 2;
          bVar5 = bVar5 | param_1[4] & 0xf;
        }
        *pbVar2 = bVar5;
        param_1 = pbVar6;
        param_2 = uVar4;
        pbVar2 = pbVar2 + 1;
      } while (uVar4 != 0);
    }
    else {
      if ((int)param_2 < 2) {
        param_2 = 1;
      }
      pbVar2 = param_1 + -4;
      pbVar6 = param_1 + -1;
      if ((param_2 & 1) != 0) {
        *param_1 = *param_1;
        pbVar2 = param_1;
        pbVar6 = param_1;
      }
      for (iVar3 = (int)param_2 >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
        pbVar6[1] = pbVar2[4];
        pbVar2 = pbVar2 + 8;
        pbVar6 = pbVar6 + 2;
        *pbVar6 = *pbVar2;
      }
    }
  }
  else {
    if (0x10 < param_3) {
      return 0;
    }
    if ((int)param_2 < 2) {
      iVar3 = 0;
    }
    else {
      if (0 < (int)(param_2 - 1)) {
        pbVar2 = param_1 + -4;
        pbVar6 = param_1 + -2;
        if ((param_2 & 1) == 0) {
          *(undefined2 *)param_1 = *(undefined2 *)param_1;
          pbVar2 = param_1;
          pbVar6 = param_1;
        }
        uVar7 = *(undefined4 *)(pbVar2 + 4);
        for (iVar3 = (int)(param_2 - 1) >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
          uVar1 = *(undefined2 *)(pbVar2 + 8);
          *(short *)(pbVar6 + 2) = (short)uVar7;
          uVar7 = *(undefined4 *)(pbVar2 + 0xc);
          pbVar6 = pbVar6 + 4;
          *(undefined2 *)pbVar6 = uVar1;
          pbVar2 = pbVar2 + 8;
        }
      }
      iVar3 = param_2 - 1;
      if (iVar3 < 0) {
        return 1;
      }
    }
    *(short *)(param_1 + iVar3 * 2) = (short)*(undefined4 *)(param_1 + iVar3 * 4);
  }
  return 1;
}

