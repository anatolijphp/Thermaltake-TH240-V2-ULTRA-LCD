// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00124000-0x00127fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_00124394 @ 00124394, 368 bytes
// called by: FUN_00118250, FUN_001180e8, FUN_001192f8
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00124394(int param_1,short *param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 uStack_28;
  undefined4 uStack_24;
  int local_20 [2];
  
  local_20[0] = FUN_0010e384();
  if ((*(uint *)(local_20[0] + 0x28) & 2) == 0) {
    return;
  }
  uVar3 = 0;
  if (param_2[2] < *param_2) {
    return;
  }
  if (param_2[3] < param_2[1]) {
    return;
  }
  if ((*(uint *)(local_20[0] + 0x28) & 0x801) != 1) {
    local_20[0] = FUN_0010e24c(param_1);
    FUN_00114ef0(&uStack_28,param_2,local_20[0]);
    if ((short)uStack_28 <= (short)uStack_24) {
      if (uStack_28._2_2_ <= uStack_24._2_2_) {
        if ((*(uint *)(local_20[0] + 0x28) & 0x4020) == 0) {
          *(undefined4 *)(local_20[0] + 8) = uStack_28;
          *(undefined4 *)(local_20[0] + 0xc) = uStack_24;
          *(uint *)(local_20[0] + 0x28) = *(uint *)(local_20[0] + 0x28) | param_3;
          DAT_001830b8 = DAT_001830b8 + 1;
          do {
            iVar1 = param_1;
            iVar2 = FUN_0010e384();
            param_1 = *(int *)(iVar2 + 0x18);
          } while (*(int *)(iVar2 + 0x18) != 0);
          if ((DAT_007c60c0 != iVar1) &&
             (uVar3 = (uint)(DAT_007c60c4 == iVar1), DAT_007c60c4 != iVar1)) {
            if (DAT_007c60c8 == iVar1) {
              uVar3 = 2;
            }
            else if (DAT_007c60cc == iVar1) {
              uVar3 = 3;
            }
            else {
              uVar3 = 0xffffffff;
            }
          }
          DAT_001830c0 = DAT_001830c0 | 1 << (uVar3 & 0xff);
          FUN_00112f5c();
        }
        else {
          FUN_00112958(local_20[0] + 8,local_20[0] + 8,&uStack_28);
        }
      }
    }
    FUN_0010e334(local_20);
    return;
  }
  return;
}

// ==== FUN_00124510 @ 00124510, 1132 bytes
// called by: FUN_0010f030
undefined4 FUN_00124510(int param_1)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined2 *puVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int *piVar11;
  int aiStack_80 [7];
  int local_64;
  int local_60;
  int local_58;
  undefined2 *local_54;
  int local_50 [3];
  uint local_44;
  uint local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  
  iVar9 = 0;
  (*DAT_00183070)(aiStack_80,0);
  local_3c = 0;
  if (0 < *(int *)(&DAT_00007440 + param_1)) {
    do {
      iVar7 = 0;
      local_40 = 0;
      local_44 = 0;
      if (0 < *(int *)(&DAT_00007448 + param_1)) {
        do {
          iVar6 = *(int *)(&DAT_0000744c + param_1 + iVar7 * 4);
          local_50[0] = FUN_0010e24c(*(undefined4 *)(&DAT_00007474 + param_1));
          puVar5 = (undefined2 *)(local_50[0] + iVar9 * 0x80);
          local_38 = param_1 + iVar6 + 0x7000;
          local_54 = (undefined2 *)(local_44 + aiStack_80[iVar6]);
          local_28 = (int)local_54;
          iVar8 = *(int *)(&DAT_0000730c +
                          param_1 + (uint)(byte)(&DAT_00007325)[param_1 + iVar6] * 4);
          local_60 = local_40 + *(int *)(&DAT_000073f8 + param_1 + iVar6 * 4);
          local_2c = local_60;
          iVar10 = param_1 + iVar6 * 0x10;
          local_30 = iVar10;
          if (((int)local_54 < *(int *)(&DAT_000073b8 + iVar10)) &&
             (local_60 < *(int *)(&DAT_000073bc + iVar10))) {
            iVar2 = *(int *)(&DAT_000073c0 + iVar10);
            local_58 = FUN_0010e24c(*(undefined4 *)(&DAT_000073c4 + iVar10));
            local_34 = *(int *)(&DAT_000073b8 + iVar10) * local_60 * iVar2 +
                       (int)local_54 * iVar2 + local_58;
          }
          else {
            local_34 = 0;
          }
          local_54 = (undefined2 *)local_28;
          piVar11 = (int *)(&DAT_00007378 + local_30);
          local_60 = local_2c;
          if ((local_28 < *piVar11) && (local_2c < *(int *)(&DAT_0000737c + local_30))) {
            iVar10 = *(int *)(&DAT_00007380 + local_30);
            local_64 = FUN_0010e24c(*(undefined4 *)(&DAT_00007384 + local_30));
            local_54 = (undefined2 *)
                       (*piVar11 * iVar10 * local_60 + iVar10 * (int)local_54 + local_64);
            if (local_54 == (undefined2 *)0x0) goto LAB_00124818;
            if (local_34 != 0) {
              *puVar5 = *local_54;
              (*DAT_00183074)(puVar5 + 1,local_34 + 2,0x7e);
              uVar3 = 0x3f;
              if (puVar5[DAT_00181597] == 0) {
                uVar3 = 0x3e;
                do {
                  if (puVar5[(byte)(&DAT_00181558)[uVar3]] != 0) break;
                  if (puVar5[(byte)(&DAT_00181557)[uVar3]] != 0) {
                    uVar3 = uVar3 - 1;
                    break;
                  }
                  uVar3 = uVar3 - 2;
                } while (0 < (int)uVar3);
                if (0 < (int)(uVar3 + 1)) goto LAB_00124774;
                uVar4 = 0;
              }
              else {
LAB_00124774:
                uVar4 = ~uVar3 & 1;
              }
              uVar4 = uVar3 - uVar4;
              for (; (int)uVar4 < (int)uVar3; uVar3 = uVar3 - 1) {
                sVar1 = puVar5[(byte)(&DAT_00181558)[uVar3]];
                if (sVar1 != 0) {
                  puVar5[(byte)(&DAT_00181558)[uVar3]] = *(short *)(iVar8 + uVar3 * 2) * sVar1;
                }
              }
              if (-1 < (int)uVar4) {
                do {
                  sVar1 = puVar5[(byte)(&DAT_00181558)[uVar4]];
                  if (sVar1 != 0) {
                    puVar5[(byte)(&DAT_00181558)[uVar4]] = *(short *)(iVar8 + uVar4 * 2) * sVar1;
                  }
                  sVar1 = puVar5[(byte)(&DAT_00181557)[uVar4]];
                  if (sVar1 != 0) {
                    puVar5[(byte)(&DAT_00181557)[uVar4]] =
                         *(short *)(iVar8 + uVar4 * 2 + -2) * sVar1;
                  }
                  uVar4 = uVar4 - 2;
                } while (uVar4 < 0x80000000);
              }
              goto LAB_00124818;
            }
            if (local_50[0] != 0) {
              FUN_0010e334(local_50);
            }
LAB_00124844:
            FUN_0010e334(&local_64);
          }
          else {
            local_54 = (undefined2 *)0x0;
LAB_00124818:
            if (local_50[0] != 0) {
              FUN_0010e334(local_50);
            }
            if (local_34 != 0) {
              FUN_0010e334(&local_58);
            }
            if (local_54 != (undefined2 *)0x0) goto LAB_00124844;
          }
          iVar9 = iVar9 + 1;
          if ((&DAT_0000732d)[param_1] == '\x01') {
            aiStack_80[iVar6] = aiStack_80[iVar6] + 1;
          }
          else {
            local_44 = local_44 + 1;
            if (local_44 == *(byte *)(local_38 + 0x31d)) {
              local_40 = local_40 + 1;
              local_44 = 0;
              if (local_40 == *(byte *)(local_38 + 0x321)) {
                local_40 = 0;
                aiStack_80[iVar6] = (uint)*(byte *)(local_38 + 0x31d) + aiStack_80[iVar6];
              }
            }
          }
          iVar7 = iVar7 + 1;
        } while (iVar7 < *(int *)(&DAT_00007448 + param_1));
      }
      local_3c = local_3c + 1;
    } while (local_3c < *(int *)(&DAT_00007440 + param_1));
  }
  uVar3 = (uint)(byte)(&DAT_0000732d)[param_1];
  if (uVar3 == 1) {
    param_1 = param_1 + (uint)(byte)(&DAT_0000732e)[param_1] * 4;
    *(int *)(&DAT_000073f8 + param_1) = *(int *)(&DAT_000073f8 + param_1) + 1;
  }
  else if (uVar3 != 0) {
    iVar9 = 0;
    do {
      iVar7 = param_1 + iVar9;
      uVar3 = uVar3 - 1;
      iVar9 = iVar9 + 1;
      iVar6 = param_1 + (uint)(byte)(&DAT_0000732e)[iVar7] * 4;
      *(uint *)(&DAT_000073f8 + iVar6) =
           (uint)(byte)(&DAT_00007321)[(uint)(byte)(&DAT_0000732e)[iVar7] + param_1] +
           *(int *)(&DAT_000073f8 + iVar6);
    } while (uVar3 != 0);
  }
  return 0;
}

// ==== FUN_00124a6c @ 00124a6c, 28 bytes
void FUN_00124a6c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  int aiStack_18 [2];
  
  FUN_00112660();
  FUN_00110c10(param_1);
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
  aiStack_18[0] = param_4;
  FUN_0010e4c0(1 << uVar4);
  uVar5 = (&DAT_007bd140)[uVar3];
  (&DAT_007bd160)[uVar3] = uVar5;
  aiStack_18[0] = FUN_0010e7f0(uVar4);
  if (aiStack_18[0] == 0) {
    return;
  }
  pcVar1 = (code *)(**(code **)(*(int *)(aiStack_18[0] + 0xc) + 0x24))(aiStack_18,0x22);
  if (pcVar1 == (code *)0x0) {
    return;
  }
  (*pcVar1)(aiStack_18[0],uVar5);
  return;
}

// ==== FUN_00124a88 @ 00124a88, 28 bytes
void FUN_00124a88(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  int aiStack_18 [2];
  
  FUN_00112660();
  FUN_0011158c(param_1);
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
  aiStack_18[0] = param_4;
  FUN_0010e4c0(1 << uVar4);
  uVar5 = (&DAT_007bd140)[uVar3];
  (&DAT_007bd160)[uVar3] = uVar5;
  aiStack_18[0] = FUN_0010e7f0(uVar4);
  if (aiStack_18[0] == 0) {
    return;
  }
  pcVar1 = (code *)(**(code **)(*(int *)(aiStack_18[0] + 0xc) + 0x24))(aiStack_18,0x22);
  if (pcVar1 == (code *)0x0) {
    return;
  }
  (*pcVar1)(aiStack_18[0],uVar5);
  return;
}

// ==== FUN_00124aa4 @ 00124aa4, 88 bytes
void FUN_00124aa4(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = 0;
  do {
    if (DAT_00182f54 == 0) {
      uVar1 = DAT_001830c0 & 1 << (uVar2 & 0xff);
    }
    else {
      uVar1 = DAT_00182f54 & DAT_001830c0 & 1 << (uVar2 & 0xff);
    }
    if (uVar1 != 0) {
      FUN_00112674(uVar2);
    }
    uVar2 = uVar2 + 1;
  } while ((int)uVar2 < 4);
  return;
}

// ==== FUN_00124b04 @ 00124b04, 88 bytes
void FUN_00124b04(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = 0;
  do {
    if (DAT_00182f54 == 0) {
      uVar1 = DAT_001830c0 & 1 << (uVar2 & 0xff);
    }
    else {
      uVar1 = DAT_00182f54 & DAT_001830c0 & 1 << (uVar2 & 0xff);
    }
    if (uVar1 != 0) {
      FUN_001128ac(uVar2);
    }
    uVar2 = uVar2 + 1;
  } while ((int)uVar2 < 4);
  return;
}

// ==== FUN_00124b64 @ 00124b64, 180 bytes
// called by: FUN_00124b64, FUN_00119460
void FUN_00124b64(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *unaff_r9;
  int aiStack_48 [2];
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 *local_34;
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  if (param_1 != 0) {
    unaff_r9 = &uStack_28;
  }
  local_40 = 0;
  local_3c = 0;
  local_38 = 0;
  local_34 = (undefined4 *)0x0;
  if (param_1 != 0) {
    do {
      aiStack_48[0] = FUN_0010e24c(param_1);
      FUN_001129e8(aiStack_48[0],param_2,param_3);
      FUN_001129e8(aiStack_48[0] + 8,param_2,param_3);
      uVar1 = *(undefined4 *)(aiStack_48[0] + 0x1c);
      iVar2 = *(int *)(aiStack_48[0] + 0x20);
      FUN_0010e334(aiStack_48);
      FUN_00124b64(uVar1,param_2,param_3);
      local_40 = 3;
      local_34 = unaff_r9;
      uStack_28 = param_2;
      uStack_24 = param_3;
      FUN_001198a4(param_1,&local_40);
      param_1 = iVar2;
    } while (iVar2 != 0);
    return;
  }
  return;
}

// ==== FUN_00124c38 @ 00124c38, 84 bytes
void FUN_00124c38(void)

{
  DAT_00182f20 = 0;
  (*DAT_00183070)(&DAT_007bd0a8,0,0x78);
  (*DAT_00183070)(&DAT_007bd120,0,0x10);
  DAT_00182efc = &DAT_007bd0a8;
  DAT_00182ee9 = 0;
  DAT_00182f0c = 0;
  DAT_00182f00 = 0;
  return;
}

// ==== FUN_00124c9c @ 00124c9c, 104 bytes
void FUN_00124c9c(void)

{
  (*DAT_00183070)(&DAT_007bd130,0,0x10);
  (*DAT_00183070)(&DAT_007bd140,0,0x10);
  (*DAT_00183070)(&DAT_007bd150,0,0x10);
  (*DAT_00183070)(&DAT_007bd160,0,0x10);
  DAT_00182f54 = 0;
  return;
}

// ==== FUN_00124d40 @ 00124d40, 60 bytes
void FUN_00124d40(void)

{
  FUN_0010e104(DAT_00183048);
  FUN_0010e104(DAT_0018304c);
  FUN_0010e104(DAT_00183050);
  DAT_00183050 = 0;
  DAT_0018304c = 0;
  DAT_00183048 = 0;
  DAT_00183054 = 0;
  DAT_00183058 = 0;
  return;
}

// ==== FUN_00124d80 @ 00124d80, 32 bytes
void FUN_00124d80(void)

{
  FUN_0010e104(DAT_001830a8);
  DAT_001830a8 = 0;
  DAT_001830a4 = 0;
  return;
}

// ==== FUN_00124da4 @ 00124da4, 196 bytes
void FUN_00124da4(void)

{
  DAT_001830b5 = 0;
  DAT_001830f0 = 0;
  DAT_001830cc = 0;
  DAT_007c60f0 = 0;
  DAT_007c60e0 = 0;
  DAT_007c60f4 = 0;
  DAT_007c60e4 = 0;
  DAT_007c60f8 = 0;
  DAT_007c60e8 = 0;
  DAT_007c60fc = 0;
  DAT_007c60ec = 0;
  DAT_001830dc = 0;
  DAT_001830e0 = 0;
  DAT_001830d0 = 0;
  DAT_001830d8 = 0;
  DAT_001830ec = 0;
  (*DAT_00183070)(&DAT_007c6100,0,0x18);
  DAT_001830e8 = 0;
  DAT_007c60c0 = 0;
  DAT_007c60c4 = 0;
  DAT_007c60c8 = 0;
  DAT_007c60cc = 0;
  (*DAT_00183070)(&DAT_007c6158,0,0x20);
  (*DAT_00183070)(&DAT_007c6178,0,0x20);
                    /* WARNING: Could not recover jumptable at 0x00124e64. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_00183070)(&DAT_007c6198,0,0x20);
  return;
}

// ==== FUN_00124e84 @ 00124e84, 448 bytes
// called by: FUN_001196dc, FUN_00124e84
void FUN_00124e84(undefined4 param_1)

{
  int iVar1;
  code *pcVar2;
  int unaff_r4;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  bool bVar7;
  int local_40;
  int local_3c;
  int local_38;
  short *local_34;
  int local_30;
  undefined4 local_28;
  
  local_28 = param_1;
  local_40 = FUN_0010e24c(param_1);
  bVar7 = (*(byte *)(local_40 + 0x28) & 2) != 0;
  if (bVar7) {
    unaff_r4 = *(int *)(local_40 + 0x1c);
  }
  if (bVar7 && unaff_r4 != 0) {
    do {
      local_3c = FUN_0010e24c(unaff_r4);
      if (((~*(uint *)(local_3c + 0x28) & 3) == 0) &&
         (iVar1 = FUN_00112b2c(local_3c,local_40 + 8), iVar1 != 0)) {
        uVar3 = *(undefined4 *)(local_40 + 8);
        uVar4 = *(undefined4 *)(local_40 + 0xc);
        FUN_0010e334(&local_40);
        FUN_0010e334(&local_3c);
        local_38 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
        local_34 = (short *)FUN_0010e24c(unaff_r4);
        iVar1 = FUN_00112b2c(local_38 + 8,local_34);
        if (iVar1 == 0) {
          FUN_0010e334(&local_38);
          FUN_0010e334(&local_34);
        }
        else {
          uVar6 = *(undefined4 *)(DAT_00182efc + 0x48);
          uVar5 = *(undefined4 *)(DAT_00182efc + 0x4c);
          FUN_00114ef0(local_34 + 4,local_34,local_38 + 8);
          FUN_0010e334(&local_38);
          iVar1 = DAT_00182efc;
          DAT_001830e0 = unaff_r4;
          *(int *)(DAT_00182efc + 0x48) = (int)*local_34;
          *(int *)(iVar1 + 0x4c) = (int)local_34[1];
          FUN_0010e334(&local_34);
          local_30 = unaff_r4;
          iVar1 = FUN_0010e384(unaff_r4);
          if ((*(uint *)(iVar1 + 0x28) & 0x10000) == 0) {
            pcVar2 = FUN_001195dc;
          }
          else {
            pcVar2 = FUN_00111208;
          }
          (*pcVar2)(local_30);
          iVar1 = DAT_00182efc;
          DAT_001830e0 = 0;
          *(undefined4 *)(DAT_00182efc + 0x48) = uVar6;
          *(undefined4 *)(iVar1 + 0x4c) = uVar5;
          local_3c = FUN_0010e384(unaff_r4);
          FUN_00124e84(unaff_r4);
          local_3c = FUN_0010e384(unaff_r4);
          local_40 = FUN_0010e384(local_28);
        }
        local_40 = FUN_0010e24c(local_28);
        local_3c = FUN_0010e24c(unaff_r4);
        *(undefined4 *)(local_40 + 8) = uVar3;
        *(undefined4 *)(local_40 + 0xc) = uVar4;
      }
      unaff_r4 = *(int *)(local_3c + 0x20);
      FUN_0010e334(&local_3c);
    } while (unaff_r4 != 0);
  }
  FUN_0010e334(&local_40);
  return;
}

// ==== FUN_00125054 @ 00125054, 1604 bytes
// called by: FUN_0011091c, FUN_00123c60, FUN_0010f030
undefined4 FUN_00125054(undefined4 *param_1,undefined1 *param_2)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  undefined4 *puVar7;
  int iVar8;
  bool bVar9;
  bool bVar10;
  uint local_38;
  undefined1 local_34 [4];
  undefined4 *local_30;
  byte local_2c [4];
  byte local_28 [4];
  
  local_30 = param_1 + 0x1d0d;
LAB_00125070:
  do {
    do {
      iVar2 = (*(code *)*param_1)(param_1,&local_38);
      if (iVar2 != 0) {
switchD_00125154_caseD_c8:
        return 1;
      }
      while ((char)local_38 != -1) {
        iVar2 = (*(code *)*param_1)(param_1,&local_38);
        if (iVar2 != 0) {
          return 1;
        }
      }
      do {
        iVar2 = (*(code *)*param_1)(param_1,&local_38);
        if (iVar2 != 0) {
          return 1;
        }
        uVar3 = local_38 & 0xff;
      } while (uVar3 == 0xff);
    } while (uVar3 == 0);
    *param_2 = (char)local_38;
    if (uVar3 == 0xce) {
switchD_00125154_caseD_c7:
      return 0;
    }
    if (uVar3 < 0xcf) {
      if (uVar3 == 0xc6) {
        return 0;
      }
      if (uVar3 < 199) {
        if (uVar3 == 0xc2) {
          return 0;
        }
        if (uVar3 < 0xc3) {
          if (uVar3 == 1) {
            return 1;
          }
          if (uVar3 == 0xc0 || uVar3 == 0xc1) {
            return 0;
          }
        }
        else {
          if (uVar3 == 0xc3) {
            return 0;
          }
          if (uVar3 == 0xc4) {
            iVar2 = (*(code *)*param_1)(param_1,&local_38);
            if (iVar2 != 0) {
              return 1;
            }
            iVar2 = (*(code *)*param_1)(param_1,local_34);
            if (iVar2 != 0) {
              return 1;
            }
            for (iVar2 = CONCAT11((char)local_38,local_34[0]) - 2; 0x10 < iVar2;
                iVar2 = iVar2 - iVar4) {
              iVar4 = (*(code *)*param_1)(param_1,local_2c);
              if (iVar4 != 0) {
                return 1;
              }
              uVar3 = (local_2c[0] & 0xf) + (local_2c[0] >> 2 & 4);
              local_2c[0] = (byte)uVar3;
              if (7 < uVar3) {
                return 1;
              }
              iVar8 = 1;
              *(undefined1 *)((int)param_1 + uVar3 + 0x2c) = 1;
              iVar4 = 0;
              *(undefined1 *)((int)param_1 + uVar3 * 0x11 + 0x34) = 0;
              pbVar6 = (byte *)((int)param_1 + uVar3 * 0x11 + 0x35);
              do {
                iVar5 = (*(code *)*param_1)(param_1,local_28);
                if (iVar5 != 0) {
                  return 1;
                }
                iVar8 = iVar8 + 1;
                *pbVar6 = local_28[0];
                iVar4 = iVar4 + (uint)local_28[0];
                pbVar6 = pbVar6 + 1;
              } while (iVar8 < 0x11);
              bVar10 = SBORROW4(iVar4,0xff);
              iVar8 = iVar4 + -0xff;
              bVar9 = iVar4 == 0xff;
              iVar2 = iVar2 + -0x11;
              if (iVar4 < 0x100) {
                bVar10 = SBORROW4(iVar4,iVar2);
                iVar8 = iVar4 - iVar2;
                bVar9 = iVar4 == iVar2;
              }
              if (!bVar9 && iVar8 < 0 == bVar10) {
                return 1;
              }
              iVar8 = 0;
              puVar7 = param_1 + (uint)local_2c[0] * 0x40 + 0x2f;
              if (0 < iVar4) {
                do {
                  iVar5 = (*(code *)*param_1)(param_1,puVar7);
                  if (iVar5 != 0) {
                    return 1;
                  }
                  iVar8 = iVar8 + 1;
                  puVar7 = (undefined4 *)((int)puVar7 + 1);
                } while (iVar8 < iVar4);
              }
            }
            goto LAB_00125070;
          }
          if (uVar3 == 0xc5) {
            return 0;
          }
        }
      }
      else {
        switch(uVar3) {
        case 199:
        case 0xc9:
        case 0xca:
        case 0xcb:
        case 0xcd:
          goto switchD_00125154_caseD_c7;
        case 200:
        case 0xcc:
          goto switchD_00125154_caseD_c8;
        }
      }
switchD_00125154_default:
      iVar2 = (*(code *)*param_1)(param_1,&local_38);
      if ((iVar2 == 0) && (iVar2 = (*(code *)*param_1)(param_1,local_34), iVar2 == 0)) {
        iVar2 = CONCAT11((char)local_38,local_34[0]) - 2;
        if (0 < iVar2) {
          if ((int)param_1[6] < iVar2) {
            do {
              iVar2 = iVar2 - param_1[6];
              if (*(char *)(param_1 + 7) != '\0') {
                param_1[3] = 0;
                *(undefined1 *)(param_1 + 7) = 0;
              }
              iVar4 = (*(code *)param_1[1])(param_1[2],&local_38,0x1000,param_1[3]);
              if (iVar4 == 0) {
                if (param_1[6] == 0) goto LAB_00125070;
              }
              else {
                param_1[6] = iVar4;
                param_1[3] = param_1[3] + iVar4;
                param_1[5] = local_38;
              }
            } while ((int)param_1[6] < iVar2);
          }
          param_1[5] = param_1[5] + iVar2;
          param_1[6] = param_1[6] - iVar2;
        }
      }
      goto LAB_00125070;
    }
    if (uVar3 == 0xd6) {
      return 1;
    }
    if (uVar3 < 0xd7) {
      switch(uVar3) {
      case 0xcf:
        goto switchD_00125154_caseD_c7;
      case 0xd0:
      case 0xd1:
      case 0xd2:
      case 0xd3:
      case 0xd4:
      case 0xd5:
        goto switchD_00125154_caseD_c8;
      }
      goto switchD_00125154_default;
    }
    if (uVar3 == 0xda) {
      return 0;
    }
    if (uVar3 < 0xdb) {
      if (uVar3 == 0xd7) {
        return 1;
      }
      if (uVar3 == 0xd8 || uVar3 == 0xd9) {
        return 0;
      }
      goto switchD_00125154_default;
    }
    if (uVar3 == 0xdb) {
      iVar2 = (*(code *)*param_1)(param_1,&local_38);
      if (iVar2 != 0) {
        return 1;
      }
      iVar2 = (*(code *)*param_1)(param_1,local_34);
      if (iVar2 != 0) {
        return 1;
      }
      iVar2 = CONCAT11((char)local_38,local_34[0]) - 2;
      while (0 < iVar2) {
        iVar4 = (*(code *)*param_1)(param_1,local_2c);
        if (iVar4 != 0) {
          return 1;
        }
        uVar3 = local_2c[0] & 0xf;
        bVar1 = local_2c[0] >> 4;
        local_2c[0] = (byte)uVar3;
        if (3 < uVar3) {
          return 1;
        }
        puVar7 = param_1 + uVar3 * 0x20 + 0x1c43;
        iVar4 = 0;
        param_1[uVar3 + 0x1cc3] = puVar7;
        do {
          if (bVar1 == 0) {
            iVar8 = (*(code *)*param_1)(param_1,local_28);
            if (iVar8 != 0) {
              return 1;
            }
            *(ushort *)((int)puVar7 + iVar4 * 2) = (ushort)local_28[0];
          }
          else {
            iVar8 = (*(code *)*param_1)(param_1,&local_38);
            if (iVar8 != 0) {
              return 1;
            }
            iVar8 = (*(code *)*param_1)(param_1,local_34);
            if (iVar8 != 0) {
              return 1;
            }
            *(ushort *)((int)puVar7 + (uint)(byte)(&DAT_00181558)[iVar4] * 2) =
                 CONCAT11((char)local_38,local_34[0]);
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < 0x40);
        if (bVar1 == 0) {
          iVar2 = iVar2 + -0x41;
        }
        else {
          iVar2 = iVar2 + -0x81;
        }
      }
    }
    else {
      if (uVar3 != 0xdd) {
        if (uVar3 != 0xee) goto switchD_00125154_default;
        iVar2 = (*(code *)*param_1)(param_1,&local_38);
        if (iVar2 != 0) {
          return 1;
        }
        iVar2 = (*(code *)*param_1)(param_1,local_34);
        if (iVar2 != 0) {
          return 1;
        }
        if ((int)(CONCAT11((char)local_38,local_34[0]) - 2) < 0xc) {
          return 1;
        }
        iVar2 = 0xb;
        if ((int)param_1[6] < 0xb) {
          do {
            iVar2 = iVar2 - param_1[6];
            if (*(char *)(param_1 + 7) != '\0') {
              param_1[3] = 0;
              *(undefined1 *)(param_1 + 7) = 0;
            }
            iVar4 = (*(code *)param_1[1])(param_1[2],&local_38,0x1000,param_1[3]);
            if (iVar4 == 0) {
              if (param_1[6] == 0) {
                return 1;
              }
            }
            else {
              param_1[6] = iVar4;
              param_1[3] = param_1[3] + iVar4;
              param_1[5] = local_38;
            }
          } while ((int)param_1[6] < iVar2);
          iVar4 = param_1[6] - iVar2;
          param_1[5] = param_1[5] + iVar2;
        }
        else {
          iVar4 = param_1[6] + -0xb;
          param_1[5] = param_1[5] + 0xb;
        }
        param_1[6] = iVar4;
        uVar3 = (*(code *)*param_1)(param_1,local_2c);
        bVar9 = uVar3 == 0;
        if (bVar9) {
          uVar3 = (uint)local_2c[0];
        }
        if (!bVar9 || uVar3 != 1) {
          return 1;
        }
        goto LAB_00125070;
      }
      iVar2 = (*(code *)*param_1)(param_1,&local_38);
      if (iVar2 != 0) {
        return 1;
      }
      iVar2 = (*(code *)*param_1)(param_1,local_34);
      if (iVar2 != 0) {
        return 1;
      }
      if (CONCAT11((char)local_38,local_34[0]) != 4) {
        return 1;
      }
      iVar2 = FUN_001233c0(param_1,local_30);
    }
    if (iVar2 != 0) {
      return 1;
    }
  } while( true );
}

// ==== FUN_0012576c @ 0012576c, 136 bytes
// called by: FUN_0010e12c, FUN_00119cb8, FUN_00119b8c
void FUN_0012576c(int param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = *(uint *)(param_1 + 4);
  uVar2 = 0xf;
  do {
    if ((uint)(&DAT_007bd01c)[uVar2 * 2] <= uVar4) break;
    if ((uint)(&DAT_007bd014)[uVar2 * 2] <= uVar4) {
      uVar2 = uVar2 - 1;
      break;
    }
    uVar2 = uVar2 - 2;
  } while (uVar2 < 0x80000000);
  if (uVar2 < 0x10) {
    DAT_007bcfdc = DAT_007bcfdc - uVar4;
    iVar3 = *(int *)(param_1 + 8);
    piVar1 = *(int **)(param_1 + 0xc);
    if (iVar3 != 0) {
      *(int **)(iVar3 + 4) = piVar1;
    }
    if (piVar1 == (int *)0x0) {
      *(int *)(uVar2 * 8 + 0x7bd020) = iVar3;
    }
    else {
      *piVar1 = iVar3;
    }
    return;
  }
  return;
}

// ==== FUN_001257f8 @ 001257f8, 328 bytes
// called by: FUN_0010e12c
undefined4 FUN_001257f8(uint param_1)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  int *piVar4;
  uint uVar5;
  int *piVar6;
  int *piVar7;
  uint uVar8;
  bool bVar9;
  
  piVar7 = (int *)((int)DAT_007bd014 + DAT_007bcfe8 + -1);
  piVar4 = DAT_007bd014;
  do {
    uVar5 = piVar4[1];
    if ((*piVar4 == 0) && (piVar6 = (int *)((int)piVar4 + uVar5), piVar6 < piVar7)) {
      iVar1 = *piVar6;
      uVar8 = piVar6[1];
      if ((iVar1 != 0) && (*(char *)(DAT_007bd010 + iVar1 * 8) == '\x01')) {
        bVar9 = iVar1 == DAT_007bd008;
        uVar2 = 0xf;
        do {
          if ((uint)(&DAT_007bd01c)[uVar2 * 2] <= uVar5) break;
          if ((&DAT_007bd014)[uVar2 * 2] <= uVar5) {
            uVar2 = uVar2 - 1;
            break;
          }
          uVar2 = uVar2 - 2;
        } while (uVar2 < 0x80000000);
        if (uVar2 < 0x10) {
          DAT_007bcfdc = DAT_007bcfdc - uVar5;
          iVar1 = piVar4[2];
          piVar3 = (int *)piVar4[3];
          if (iVar1 != 0) {
            *(int **)(iVar1 + 4) = piVar3;
          }
          if (piVar3 == (int *)0x0) {
            *(int *)(uVar2 * 8 + 0x7bd020) = iVar1;
          }
          else {
            *piVar3 = iVar1;
          }
        }
        *(int **)(DAT_007bd010 + *piVar6 * 8 + 4) = piVar4;
        FUN_0010ca8c(piVar4,piVar6,uVar8);
        if (bVar9) {
          DAT_007bd010 = DAT_007bd010 - ((int)piVar6 - (int)piVar4);
        }
        uVar2 = FUN_00119b8c(uVar5,(int)piVar4 + uVar8);
        uVar5 = uVar8;
        if ((param_1 != 0) && (param_1 <= uVar2)) {
          return 0;
        }
      }
    }
    piVar4 = (int *)((int)piVar4 + uVar5);
    if (piVar7 <= piVar4) {
      return 1;
    }
  } while( true );
}

// ==== FUN_00125944 @ 00125944, 96 bytes
void FUN_00125944(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_10;
  
  local_10 = 0;
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 0) {
      return;
    }
  }
  *(undefined4 *)(iVar1 + 0x2c) = param_2;
  local_10 = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),10,&local_10);
  return;
}

// ==== FUN_001259a4 @ 001259a4, 28 bytes
void FUN_001259a4(int param_1,undefined4 param_2)

{
  undefined4 local_8 [2];
  
  local_8[0] = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),0xb,local_8);
  return;
}

// ==== FUN_001259c0 @ 001259c0, 80 bytes
void FUN_001259c0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  iVar1 = DAT_00182efc;
  if (*(int *)(DAT_00182efc + 0x34) != param_1) {
    *(int *)(DAT_00182efc + 0x34) = param_1;
    uVar2 = (*(code *)**(undefined4 **)((&DAT_007bd120)[*(byte *)(iVar1 + 0x11)] + 0x10))();
    if ((*(byte *)(DAT_00182efc + 0x10) & 4) == 0) {
      puVar3 = *(undefined4 **)(DAT_00182efc + 0x38);
    }
    else {
      puVar3 = *(undefined4 **)(DAT_00182efc + 0x3c);
    }
    *puVar3 = uVar2;
    return;
  }
  return;
}

// ==== FUN_00125a18 @ 00125a18, 36 bytes
void FUN_00125a18(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 local_10;
  undefined4 uStack_c;
  
  local_10 = param_2;
  uStack_c = param_3;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),0xd,&local_10);
  return;
}

// ==== FUN_00125a3c @ 00125a3c, 28 bytes
void FUN_00125a3c(int param_1,undefined4 param_2)

{
  undefined4 local_8 [2];
  
  local_8[0] = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),0xc,local_8);
  return;
}

// ==== FUN_00125a58 @ 00125a58, 80 bytes
void FUN_00125a58(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  iVar1 = DAT_00182efc;
  if (*(int *)(DAT_00182efc + 0x30) != param_1) {
    *(int *)(DAT_00182efc + 0x30) = param_1;
    uVar2 = (*(code *)**(undefined4 **)((&DAT_007bd120)[*(byte *)(iVar1 + 0x11)] + 0x10))();
    if ((*(byte *)(DAT_00182efc + 0x10) & 4) == 0) {
      puVar3 = *(undefined4 **)(DAT_00182efc + 0x3c);
    }
    else {
      puVar3 = *(undefined4 **)(DAT_00182efc + 0x38);
    }
    *puVar3 = uVar2;
    return;
  }
  return;
}

// ==== FUN_00125ab0 @ 00125ab0, 156 bytes
void FUN_00125ab0(int param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar1;
    FUN_0011528c(uVar1,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      return;
    }
  }
  iVar2 = *(int *)(param_1 + 8);
  if (param_2 == 0x21) {
    *(undefined4 *)(iVar2 + 0x38) = param_3;
    return;
  }
  if (0x21 < param_2) {
    if (param_2 == 0x23) {
      *(undefined4 *)(iVar2 + 0x44) = param_3;
      return;
    }
    if (param_2 != 0x24) {
      if (param_2 == 0x25) {
        *(undefined4 *)(iVar2 + 0x40) = param_3;
      }
      return;
    }
    *(undefined4 *)(iVar2 + 0x4c) = param_3;
    return;
  }
  if (param_2 == 3) {
    *(undefined4 *)(iVar2 + 0x48) = param_3;
    return;
  }
  if (param_2 != 0x19) {
    if (param_2 == 0x20) {
      *(undefined4 *)(iVar2 + 0x3c) = param_3;
    }
    return;
  }
  *(undefined4 *)(iVar2 + 0x34) = param_3;
  return;
}

// ==== FUN_00125bb0 @ 00125bb0, 304 bytes
void FUN_00125bb0(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_20;
  int local_1c;
  
  local_20 = 0;
  local_1c = 0;
  iVar4 = *(int *)(param_1 + 8);
  uVar1 = FUN_00115cf8(*(undefined4 *)(param_1 + 0x18));
  iVar2 = FUN_00115d34(*(undefined4 *)(param_1 + 0x18));
  iVar3 = FUN_00115f50(*(undefined4 *)(param_1 + 0x18));
  switch(uVar1 | iVar2 << 1 | iVar3 << 2) {
  case 0:
    local_20 = param_2;
    local_1c = param_3;
    break;
  case 1:
    local_20 = (*(int *)(iVar4 + 0x18) - *(int *)(iVar4 + 0x10)) - param_2;
    local_1c = param_3;
    break;
  case 2:
    local_1c = (*(int *)(iVar4 + 0x1c) - *(int *)(iVar4 + 0x14)) - param_3;
    local_20 = param_2;
    break;
  case 3:
    local_20 = (*(int *)(iVar4 + 0x18) - *(int *)(iVar4 + 0x10)) - param_2;
    local_1c = (*(int *)(iVar4 + 0x1c) - *(int *)(iVar4 + 0x14)) - param_3;
    break;
  case 4:
    local_20 = param_3;
    local_1c = param_2;
    break;
  case 5:
    local_20 = (*(int *)(iVar4 + 0x1c) - *(int *)(iVar4 + 0x14)) - param_3;
    local_1c = param_2;
    break;
  case 6:
    local_1c = (*(int *)(iVar4 + 0x18) - *(int *)(iVar4 + 0x10)) - param_2;
    local_20 = param_3;
    break;
  case 7:
    local_20 = (*(int *)(iVar4 + 0x1c) - *(int *)(iVar4 + 0x14)) - param_3;
    local_1c = (*(int *)(iVar4 + 0x18) - *(int *)(iVar4 + 0x10)) - param_2;
  }
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),3,&local_20);
  return;
}

// ==== FUN_00125d74 @ 00125d74, 168 bytes
void FUN_00125d74(undefined4 param_1,int param_2,int param_3,undefined2 param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18;
  
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_18 + 6);
  iVar4 = *(int *)(local_18 + 0xc);
  sVar2 = *(short *)(local_18 + 4);
  iVar3 = local_18 + 0x18;
  FUN_0010e334(&local_18);
  *(undefined2 *)(iVar4 * (param_3 - sVar1) + iVar3 + param_2 * 2 + sVar2 * -2) = param_4;
  if (*(int *)(local_20 + 0x14) != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00125e20 @ 00125e20, 168 bytes
void FUN_00125e20(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18;
  
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_18 + 6);
  iVar4 = *(int *)(local_18 + 0xc);
  sVar2 = *(short *)(local_18 + 4);
  iVar3 = local_18 + 0x18;
  FUN_0010e334(&local_18);
  *(undefined4 *)(iVar4 * (param_3 - sVar1) + iVar3 + param_2 * 4 + sVar2 * -4) = param_4;
  if (*(int *)(local_20 + 0x14) != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00125ecc @ 00125ecc, 168 bytes
void FUN_00125ecc(undefined4 param_1,int param_2,int param_3,undefined1 param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18;
  
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_18 + 6);
  iVar4 = *(int *)(local_18 + 0xc);
  sVar2 = *(short *)(local_18 + 4);
  iVar3 = local_18 + 0x18;
  FUN_0010e334(&local_18);
  *(undefined1 *)((iVar4 * (param_3 - sVar1) + iVar3 + param_2) - (int)sVar2) = param_4;
  if (*(int *)(local_20 + 0x14) != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00125f78 @ 00125f78, 72 bytes
void FUN_00125f78(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 uStack_14;
  undefined4 uStack_10;
  
  puVar1 = &uStack_10;
  (**(code **)param_1[2])();
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x18))(*param_1,uStack_14,uStack_10,param_4,puVar1);
  return;
}

// ==== FUN_00125fc0 @ 00125fc0, 112 bytes
void FUN_00125fc0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int local_1c;
  int local_18 [2];
  
  puVar1 = (undefined4 *)param_1[2];
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_1c,local_18);
  *(short *)(local_1c * *(int *)(puVar1[10] + 0x1c) + local_18[0] * puVar1[3] + puVar1[1]) =
       (short)param_4;
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x18))(*param_1,local_1c,local_18[0],param_4);
  return;
}

// ==== FUN_00126030 @ 00126030, 112 bytes
void FUN_00126030(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int local_1c;
  int local_18 [2];
  
  puVar1 = (undefined4 *)param_1[2];
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_1c,local_18);
  *(undefined4 *)(local_1c * *(int *)(puVar1[10] + 0x1c) + local_18[0] * puVar1[3] + puVar1[1]) =
       param_4;
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x18))(*param_1,local_1c,local_18[0],param_4);
  return;
}

// ==== FUN_001260a0 @ 001260a0, 112 bytes
void FUN_001260a0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int local_1c;
  int local_18 [2];
  
  puVar1 = (undefined4 *)param_1[2];
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_1c,local_18);
  *(char *)(local_1c * *(int *)(puVar1[10] + 0x1c) + local_18[0] * puVar1[3] + puVar1[1]) =
       (char)param_4;
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x18))(*param_1,local_1c,local_18[0],param_4);
  return;
}

// ==== FUN_00126110 @ 00126110, 620 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00126110(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  bool bVar11;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  undefined4 uStack_28;
  
  local_40 = 0;
  local_3c = 0;
  local_38 = 0;
  local_34 = 0;
  local_30 = 0;
  local_2c = 0;
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 0) {
      return;
    }
  }
  piVar7 = &local_40;
  if (*(code **)(iVar1 + 0x48) != (code *)0x0) {
    (**(code **)(iVar1 + 0x48))(*(undefined4 *)(param_1 + 0x18),param_2,param_3);
    return;
  }
  *(int *)(iVar1 + 0x28) = param_3;
  *(int *)(iVar1 + 0x24) = param_2;
  iVar3 = FUN_001160b0();
  iVar4 = FUN_001161a8();
  iVar8 = *(int *)(iVar1 + 0x10);
  iVar9 = *(int *)(iVar1 + 0x14);
  iVar5 = (**(code **)(*(int *)(param_1 + 0xc) + 0x28))(param_1,8);
  local_30 = (int)(iVar5 + 7 + ((uint)(iVar5 + 7 >> 0x1f) >> 0x1d)) >> 3;
  if (param_2 < 0) {
    local_2c = local_2c - param_2 * local_30;
    local_40 = 0;
    local_38 = iVar8 + param_2;
  }
  else {
    local_40 = param_2;
    local_38 = iVar8;
    if (iVar3 < param_2 + iVar8) {
      local_38 = iVar3 - param_2;
    }
  }
  if (param_3 < 0) {
    local_2c = local_2c - param_3 * iVar8 * local_30;
    local_3c = 0;
    local_34 = iVar9 + param_3;
  }
  else {
    local_3c = param_3;
    local_34 = iVar9;
    if (iVar4 < param_3 + iVar9) {
      local_34 = iVar4 - param_3;
    }
  }
  iVar5 = local_38;
  if (0 < local_38) {
    iVar5 = local_34;
  }
  if (0 < iVar5) {
    bVar11 = SBORROW4(iVar3,local_40);
    iVar5 = iVar3 - local_40;
    bVar10 = iVar3 == local_40;
    if (local_40 < iVar3) {
      bVar11 = SBORROW4(iVar4,local_3c);
      iVar5 = iVar4 - local_3c;
      bVar10 = iVar4 == local_3c;
    }
    if (!bVar10 && iVar5 < 0 == bVar11) {
      if (*(int *)(iVar1 + 0x30) == 0) {
        uStack_28 = 0;
        iVar1 = *(int *)(param_1 + 8);
        if (iVar1 == 0) {
          uVar2 = FUN_0010e12c(0x50);
          *(undefined4 *)(param_1 + 8) = uVar2;
          FUN_0011528c(uVar2,0,0x50);
          iVar1 = *(int *)(param_1 + 8);
          if (iVar1 == 0) goto LAB_00126364;
        }
        *(undefined4 *)(iVar1 + 0x30) = 1;
        uStack_28 = 1;
        FUN_00116f24(*(undefined4 *)(param_1 + 0x18),9,&uStack_28);
      }
LAB_00126364:
      uVar2 = *(undefined4 *)(param_1 + 0x18);
      uVar6 = 8;
      goto LAB_00126370;
    }
  }
  if (*(int *)(iVar1 + 0x30) != 1) {
    return;
  }
  uStack_28 = 0;
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 0) {
      return;
    }
  }
  *(undefined4 *)(iVar1 + 0x30) = 0;
  uStack_28 = 0;
  uVar2 = *(undefined4 *)(param_1 + 0x18);
  piVar7 = &uStack_28;
  uVar6 = 9;
LAB_00126370:
  FUN_00116f24(uVar2,uVar6,piVar7);
  return;
}

// ==== FUN_0012637c @ 0012637c, 136 bytes
void FUN_0012637c(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_18;
  undefined4 local_14;
  
  local_18 = 0;
  local_14 = 0;
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 0) {
      return;
    }
  }
  if (*(int *)(iVar1 + 0x20) == 0) {
    *(undefined4 *)(iVar1 + 0x20) = param_2;
  }
  *(undefined4 *)(iVar1 + 0x10) = param_2;
  *(undefined4 *)(iVar1 + 0x14) = param_3;
  local_18 = param_2;
  local_14 = param_3;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),7,&local_18);
  return;
}

// ==== FUN_00126404 @ 00126404, 100 bytes
void FUN_00126404(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 local_10;
  
  local_10 = 0;
  puVar1 = *(undefined4 **)(param_1 + 8);
  if (puVar1 == (undefined4 *)0x0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    puVar1 = *(undefined4 **)(param_1 + 8);
    if (puVar1 == (undefined4 *)0x0) {
      return;
    }
  }
  puVar1[1] = param_2;
  *puVar1 = param_2;
  local_10 = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),2,&local_10);
  return;
}

// ==== FUN_00126468 @ 00126468, 80 bytes
void FUN_00126468(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar1;
    FUN_0011528c(uVar1,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      return;
    }
  }
  puVar2 = *(undefined4 **)(param_1 + 8);
  puVar2[2] = param_2;
  *puVar2 = *param_2;
  return;
}

// ==== FUN_001264b8 @ 001264b8, 84 bytes
void FUN_001264b8(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar1;
    FUN_0011528c(uVar1,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      return;
    }
  }
  iVar2 = *(int *)(param_1 + 8);
  *(undefined4 *)(iVar2 + 0x18) = param_2;
  *(undefined4 *)(iVar2 + 0x1c) = param_3;
  *(undefined4 *)(iVar2 + 0x20) = param_2;
  return;
}

// ==== FUN_0012650c @ 0012650c, 96 bytes
void FUN_0012650c(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_10;
  
  local_10 = 0;
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    uVar2 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar2;
    FUN_0011528c(uVar2,0,0x50);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 0) {
      return;
    }
  }
  *(undefined4 *)(iVar1 + 0x30) = param_2;
  local_10 = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),9,&local_10);
  return;
}

// ==== FUN_0012656c @ 0012656c, 92 bytes
void FUN_0012656c(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 local_10;
  
  local_10 = 0;
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar1;
    FUN_0011528c(uVar1,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      return;
    }
  }
  local_10 = param_2;
  FUN_00116f24(*(undefined4 *)(param_1 + 0x18),0xe,&local_10);
  return;
}

// ==== FUN_001265c8 @ 001265c8, 636 bytes
// called by: FUN_001117ac
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_001265c8(undefined4 param_1,int param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  uint local_54;
  int iStack_50;
  int iStack_4c;
  int local_44;
  uint local_40;
  undefined4 local_34;
  int iStack_30;
  int iStack_2c;
  undefined4 *local_28;
  
  local_34 = param_1;
  iStack_30 = param_2;
  iStack_2c = param_3;
  local_28 = param_4;
  local_44 = FUN_0010e24c(param_1);
  iVar5 = (int)*(short *)(local_44 + 8);
  iStack_4c = (int)*(short *)(local_44 + 10);
  FUN_0010e334(&local_44);
  local_54 = (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x28))
                       ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],8);
  if (local_54 < 0x18) {
    if (local_54 == 3) {
      local_54 = 8;
    }
  }
  else {
    local_54 = 0x20;
  }
  pcVar7 = (code *)*local_28;
  local_40 = (uint)((undefined1 *)local_28[1] != &LAB_001163d0);
  iVar1 = FUN_00113ec8(0);
  if (iVar1 != 0) {
    return;
  }
  iVar6 = ((param_2 + iVar5) - (int)*(short *)(DAT_00182efc + 0xc)) + -1;
  iVar1 = iVar5;
  if (0 < iVar6) {
    iVar1 = iVar5 - iVar6;
  }
  if (param_2 < *(short *)(DAT_00182efc + 8)) {
    iStack_50 = *(short *)(DAT_00182efc + 8) - param_2;
    param_2 = param_2 + iStack_50;
    iVar1 = iVar1 - iStack_50;
  }
  else {
    iStack_50 = 0;
  }
  if (iVar1 < 1) {
    return;
  }
  iVar6 = ((param_3 + iStack_4c) - (int)*(short *)(DAT_00182efc + 0xe)) + -1;
  if (0 < iVar6) {
    iStack_4c = iStack_4c - iVar6;
  }
  if (param_3 < *(short *)(DAT_00182efc + 10)) {
    iVar6 = *(short *)(DAT_00182efc + 10) - param_3;
    param_3 = param_3 + iVar6;
    iStack_4c = iStack_4c - iVar6;
  }
  else {
    iVar6 = 0;
  }
  if (0 < iStack_4c) {
    do {
      iVar2 = FUN_00111114(local_34);
      puVar3 = (undefined4 *)
               FUN_001146cc(param_2,param_3,iVar2 + iVar5 * iVar6 * 4 + iStack_50 * 4,iVar1);
      if (puVar3 == (undefined4 *)0x0) {
        return;
      }
      if ((local_40 != 0) && (iVar2 = FUN_00114150(puVar3,iVar1,local_54,local_28), iVar2 == 0)) {
        iVar2 = iVar5;
        if ((code *)local_28[4] == (code *)0x0) {
          do {
            uVar4 = (*pcVar7)(*puVar3);
            iVar2 = iVar2 + -1;
            *puVar3 = uVar4;
            puVar3 = puVar3 + 1;
          } while (iVar2 != 0);
        }
        else {
          (*(code *)local_28[4])(puVar3,puVar3,iVar5,4);
        }
      }
      FUN_00115678(param_2,param_3,iVar1,1);
      iVar6 = iVar6 + 1;
      iStack_4c = iStack_4c + -1;
      param_3 = param_3 + 1;
    } while (iStack_4c != 0);
    return;
  }
  return;
}

// ==== FUN_00126860 @ 00126860, 84 bytes
void FUN_00126860(int param_1,int param_2,int param_3)

{
  ushort uVar1;
  ushort uVar2;
  
  uVar1 = *(ushort *)
           (**(int **)(param_1 + 8) + (param_3 * (*(int **)(param_1 + 8))[8] + param_2) * 2);
  uVar2 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  *(ushort *)(**(int **)(param_1 + 8) + (param_3 * (*(int **)(param_1 + 8))[8] + param_2) * 2) =
       uVar2 ^ uVar1;
  return;
}

// ==== FUN_001268b4 @ 001268b4, 188 bytes
void FUN_001268b4(int param_1,int param_2,int param_3)

{
  ushort uVar1;
  ushort *puVar2;
  int local_20;
  int local_1c;
  int local_18;
  
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar2 = (ushort *)
           (*(int *)(local_18 + 0xc) * (param_3 - *(short *)(local_18 + 6)) + local_18 + 0x18 +
            param_2 * 2 + *(short *)(local_18 + 4) * -2);
  FUN_0010e334(&local_18);
  uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  *puVar2 = uVar1 ^ *puVar2;
  if (*(int *)(local_20 + 0x14) != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00126974 @ 00126974, 192 bytes
void FUN_00126974(undefined4 param_1,int param_2,int param_3)

{
  uint uVar1;
  uint *puVar2;
  int *local_20;
  int local_1c;
  int local_18 [2];
  
  local_20 = (int *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar2 = (uint *)(*(int *)(local_18[0] + 0xc) * (param_3 - *(short *)(local_18[0] + 6)) +
                    local_18[0] + 0x18 + param_2 * 4 + *(short *)(local_18[0] + 4) * -4);
  FUN_0010e334(local_18);
  uVar1 = (**(code **)(*(int *)(*local_20 + 0x10) + 8))();
  *puVar2 = uVar1 ^ *puVar2;
  if (local_20[5] != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00126a38 @ 00126a38, 188 bytes
void FUN_00126a38(int param_1,int param_2,int param_3)

{
  byte bVar1;
  byte *pbVar2;
  int local_20;
  int local_1c;
  int local_18;
  
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  pbVar2 = (byte *)((*(int *)(local_18 + 0xc) * (param_3 - *(short *)(local_18 + 6)) +
                     local_18 + 0x18 + param_2) - (int)*(short *)(local_18 + 4));
  FUN_0010e334(&local_18);
  bVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  *pbVar2 = bVar1 ^ *pbVar2;
  if (*(int *)(local_20 + 0x14) != 0) {
    local_1c = FUN_0010e24c();
    (*(code *)**(undefined4 **)(local_1c + 8))(local_1c,param_2,param_3);
    FUN_0010e334(&local_1c);
  }
  FUN_0010e334(&local_20);
  return;
}

// ==== FUN_00126af8 @ 00126af8, 64 bytes
void FUN_00126af8(int *param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  undefined4 uStack_14;
  undefined4 auStack_10 [2];
  
  puVar2 = auStack_10;
  (**(code **)param_1[2])();
  pcVar1 = *(code **)(*(int *)(*param_1 + 0xc) + 0x1c);
  (*pcVar1)(*param_1,uStack_14,auStack_10[0],pcVar1,puVar2);
  return;
}

// ==== FUN_00126b38 @ 00126b38, 100 bytes
void FUN_00126b38(int param_1,undefined4 param_2,undefined4 param_3)

{
  ushort uVar1;
  ushort uVar2;
  
  uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  uVar2 = (**(code **)(*(int *)(param_1 + 0xc) + 0x14))(param_1,param_2,param_3);
                    /* WARNING: Could not recover jumptable at 0x00126b98. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(param_1 + 0xc) + 0x18))(param_1,param_2,param_3,uVar2 ^ uVar1);
  return;
}

// ==== FUN_00126b9c @ 00126b9c, 88 bytes
void FUN_00126b9c(int param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  uVar2 = (**(code **)(*(int *)(param_1 + 0xc) + 0x14))(param_1,param_2,param_3);
                    /* WARNING: Could not recover jumptable at 0x00126bf0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(param_1 + 0xc) + 0x18))(param_1,param_2,param_3,uVar2 ^ uVar1);
  return;
}

// ==== FUN_00126bf4 @ 00126bf4, 92 bytes
void FUN_00126bf4(int param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  byte bVar2;
  
  bVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
  bVar2 = (**(code **)(*(int *)(param_1 + 0xc) + 0x14))(param_1,param_2,param_3);
                    /* WARNING: Could not recover jumptable at 0x00126c4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(param_1 + 0xc) + 0x18))(param_1,param_2,param_3,bVar2 ^ bVar1);
  return;
}

// ==== FUN_00126c50 @ 00126c50, 8 bytes
// called by: FUN_0010d298
undefined * FUN_00126c50(void)

{
  return &DAT_007c6218;
}

// ==== FUN_00126c5c @ 00126c5c, 284 bytes
/* WARNING: Removing unreachable block (ram,0x001181a4) */

void FUN_00126c5c(int *param_1)

{
  int iVar1;
  code *pcVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puStack_30;
  int iStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  if (*param_1 == 0xe) {
    if (((int *)param_1[3])[1] != 1) {
      return;
    }
    if (DAT_00183010 == 0) {
      DAT_00183010 = *(int *)param_1[3];
    }
    if (DAT_00183034 == (code *)0x0) {
      return;
    }
                    /* WARNING: Could not recover jumptable at 0x00112f6c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_00183034)();
    return;
  }
  if (*param_1 == 0xf) {
    iVar1 = param_1[1];
    if (DAT_007c60c0 == iVar1) {
      iVar1 = 0;
    }
    else if (DAT_007c60c4 == iVar1) {
      iVar1 = 1;
    }
    else if (DAT_007c60c8 == iVar1) {
      iVar1 = 2;
    }
    else if (DAT_007c60cc == iVar1) {
      iVar1 = 3;
    }
    else {
      iVar1 = -1;
    }
    if ((&DAT_007c60d0)[iVar1] != 0x12345678) {
      thunk_FUN_001169f0();
      FUN_0010e41c();
    }
  }
  iVar3 = *param_1;
  iVar1 = param_1[1];
  if (iVar3 == 0x17) {
    param_1[3] = iVar1;
    return;
  }
  if (iVar3 < 0x18) {
    if (iVar3 == 0xe) {
      while( true ) {
        if (param_1 == (int *)0x0 || iVar1 == 0) {
          return;
        }
        iVar3 = FUN_0010e384();
        iVar3 = *(int *)(iVar3 + 0x18);
        if (iVar3 == 0) {
          return;
        }
        param_1[2] = iVar1;
        if (0x13 < DAT_001830fc) {
          return;
        }
        iVar1 = FUN_0010e384(iVar3);
        pcVar2 = *(code **)(iVar1 + 0x10);
        param_1[1] = iVar3;
        if (pcVar2 != (code *)0x0) {
          DAT_001830fc = DAT_001830fc + 1;
          (*pcVar2)(param_1);
          DAT_001830fc = DAT_001830fc + -1;
          return;
        }
        iVar3 = *param_1;
        iVar1 = param_1[1];
        if (iVar3 == 0x17) goto LAB_00117744;
        if (0x17 < iVar3) break;
        if (iVar3 != 0xe) {
          if (iVar3 == 0x14) {
            FUN_00118978(iVar1,param_1[3]);
          }
LAB_00117740:
          iVar1 = 0;
LAB_00117744:
          param_1[3] = iVar1;
          return;
        }
      }
      if (iVar3 == 0x22) {
        iVar1 = 0x12345678;
        goto LAB_00117744;
      }
      if (iVar3 != 0x28) goto LAB_00117740;
FUN_001180e8:
      if (iVar1 == 0) {
        return;
      }
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
    if (iVar3 == 0x14) {
      FUN_00118978(iVar1,param_1[3]);
    }
  }
  else {
    if (iVar3 == 0x22) {
      param_1[3] = 0x12345678;
      return;
    }
    if (iVar3 == 0x28) goto FUN_001180e8;
  }
  param_1[3] = 0;
  return;
}

// ==== FUN_00126d70 @ 00126d70, 108 bytes
void FUN_00126d70(undefined4 *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int local_18;
  
  local_18 = param_4;
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
  uVar2 = *(undefined4 *)(local_18 + 8);
  uVar1 = *(undefined4 *)(DAT_00182efc + 0xc);
  uVar3 = *(undefined4 *)(local_18 + 0xc);
  *(undefined4 *)(local_18 + 8) = *(undefined4 *)(DAT_00182efc + 8);
  *(undefined4 *)(local_18 + 0xc) = uVar1;
  FUN_0010e334(&local_18);
  FUN_001196dc(*param_1);
  local_18 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 0x44));
  *(undefined4 *)(local_18 + 8) = uVar2;
  *(undefined4 *)(local_18 + 0xc) = uVar3;
  FUN_0010e334(&local_18);
  return;
}

// ==== store_read @ 00126de0, 44 bytes
// called by: config_load
// ? called by config_load to read the record
/* ? called by config_load to read the record */

void store_read(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5)

{
  FUN_00126e0c(param_1,*(undefined4 *)(*(int *)(param_1 + 0x18) + 8),param_2,param_3,param_4,param_5
              );
  return;
}

// ==== FUN_00126e0c @ 00126e0c, 212 bytes
// called by: store_read
uint FUN_00126e0c(int param_1,uint param_2,uint param_3,int param_4,uint param_5,uint param_6)

{
  int iVar1;
  uint uVar2;
  int iStack_30;
  uint local_2c;
  uint uStack_28;
  int local_24;
  
  if ((((param_2 != 0xffffffff && param_4 != 0) &&
       (iStack_30 = param_1, local_2c = param_2, uStack_28 = param_3, local_24 = param_4,
       iVar1 = (**(code **)(param_1 + 4))(param_2,&iStack_30,0x10), iVar1 != 0)) &&
      (uVar2 = (local_2c & 0x7fffffff) - 0x10, param_3 < uVar2)) && (param_6 == uVar2)) {
    if (uVar2 - param_3 < param_5) {
      param_5 = uVar2 - param_3;
    }
    iVar1 = (**(code **)(param_1 + 4))(param_2 + param_3 + 0x10,param_4,param_5);
    if (iVar1 != 0) {
      local_24 = local_24 - *(int *)(*(int *)(param_1 + 0x18) + 0xc);
      if ((local_2c & 0x80000000) != 0) {
        param_4 = 0;
      }
      iVar1 = FUN_00128e60(param_4,uVar2);
      if (iVar1 == local_24) {
        return uVar2;
      }
    }
  }
  return 0;
}

// ==== store_write @ 00126ee0, 124 bytes
// called by: config_load, config_save, usb_command
// ? config_save's write (buffer, 0x114)
/* ? config_save's write (buffer, 0x114) */

undefined8 store_write(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  int iStack_18;
  
  uStack_20 = param_2;
  uStack_1c = param_3;
  iStack_18 = param_4;
  iVar1 = FUN_00129078(param_1,&uStack_20);
  if ((iVar1 != 0) && (iVar1 = FUN_001292c0(&uStack_20,0,param_2,param_3), iVar1 != 0)) {
    if (param_4 != 0) {
      param_2 = 0;
    }
    iVar1 = FUN_00128e60(param_2,param_3);
    iVar1 = FUN_00129224(&uStack_20,*(int *)(*(int *)(param_1 + 0x18) + 0xc) + iVar1);
    if (iVar1 != 0) goto LAB_00126f58;
  }
  param_3 = 0;
LAB_00126f58:
  return CONCAT44(uStack_20,param_3);
}

// ==== FUN_00126f5c @ 00126f5c, 14 bytes
// called by: FUN_0010c768
undefined4 FUN_00126f5c(int param_1)

{
  if (param_1 - 0x30U < 10) {
    return 1;
  }
  return 0;
}

// ==== FUN_00126f6c @ 00126f6c, 4 bytes
// called by: FUN_00100200, FUN_0010d308
void FUN_00126f6c(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

// ==== FUN_00126f70 @ 00126f70, 4 bytes
// called by: FUN_0010d364
void FUN_00126f70(void)

{
  return;
}

// ==== FUN_00126f74 @ 00126f74, 48 bytes
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_00126f74(int param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 unaff_r4;
  
  uVar2 = FUN_0010d398(&DAT_009c18f0);
  iVar3 = uVar2 - 11000;
  if (param_1 != 4) {
    return uVar2;
  }
  cVar1 = '\x01';
  if (DAT_00383175 == '\0') {
LAB_0012883c:
    DAT_00383176 = 0;
    DAT_00383160 = iVar3;
    _DAT_00383164 = iVar3;
  }
  else {
    uVar2 = iVar3 - DAT_00383160;
    if ((int)uVar2 < 0) {
      uVar4 = _DAT_00383158;
      if ((int)_DAT_00383158 < (int)(~uVar2 + 1)) {
        cVar1 = '\x02';
      }
    }
    else {
      uVar4 = uVar2;
      if (_DAT_0038315c < (int)uVar2) {
        cVar1 = '\x03';
      }
    }
    if (DAT_00383175 == cVar1) {
      if (DAT_00383174 < 4) {
        DAT_00383174 = DAT_00383174 + 1;
        _DAT_00383168 = _DAT_00383168 + iVar3;
        if (_DAT_0038316c < iVar3) {
          _DAT_0038316c = iVar3;
        }
        if (iVar3 < _DAT_00383170) {
          _DAT_00383170 = iVar3;
        }
      }
      if (1 < DAT_00383174) {
        iVar3 = (_DAT_00383168 - _DAT_0038316c) - _DAT_00383170;
        _DAT_00383164 = divide(iVar3,DAT_00383174 - 1,iVar3,uVar4,unaff_r4);
      }
      goto LAB_001288c8;
    }
    if (DAT_00383175 == '\0') goto LAB_0012883c;
  }
  DAT_00383174 = 0;
  _DAT_00383168 = iVar3;
  _DAT_0038316c = iVar3;
  _DAT_00383170 = iVar3;
  DAT_00383175 = cVar1;
LAB_001288c8:
  if (3 < DAT_00383174) {
    DAT_00383160 = _DAT_00383164;
    DAT_00383175 = '\x01';
    DAT_00383176 = DAT_00383176 << 1 | 1;
  }
  DAT_00383177 = 1;
  return (uint)DAT_00383176;
}

// ==== lcd_boot_proc @ 00126fac, 1036 bytes
// refs "lcd_boot_proc reboot"; boot screen / animation
/* refs "lcd_boot_proc reboot"; boot screen / animation */

undefined4 lcd_boot_proc(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int local_4c;
  int local_48;
  int local_44;
  uint local_40;
  int local_3c;
  int iStack_38;
  int local_34;
  uint local_30;
  undefined4 local_28;
  int iStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  local_28 = param_1;
  iStack_24 = param_2;
  uStack_20 = param_3;
  uStack_1c = param_4;
  if (param_2 == 3) {
    FUN_00106924();
    FUN_00105eb8();
    DAT_00182b18 = &LAB_0012a45c;
    FUN_0010e41c();
    FUN_0011147c(0);
    DAT_00182ac0 = 0;
    iVar3 = file_size(&DAT_00182b78);
    if (iVar3 < 1) {
      FUN_0012b5c0(&local_28,0,0);
    }
    else {
      iVar3 = 0;
      while( true ) {
        iVar2 = file_read(&DAT_00182b78,iVar3,&local_4c,0x10);
        if ((iVar2 < 1) ||
           (local_44 + (local_40 & 0xffff) + (local_40 >> 0x10 & 0xfff) + local_48 + local_4c != 0))
        goto LAB_00127154;
        if (local_48 == 0) break;
        if (iVar3 == 0) {
          local_3c = local_4c;
          iStack_38 = local_48;
          local_34 = local_44;
          local_30 = local_40;
        }
        iVar3 = iVar3 + local_48;
        FUN_0012bd70(local_28,0,1,0xffff,0);
      }
      uVar1 = local_30 >> 0x10;
      if ((local_30 & 0xfff0000) != 0) {
        DAT_00182ac8 = mem_alloc(local_34);
        iVar3 = file_read(&DAT_00182b78,(local_30 & 0xffff) + 0x10);
        if (0 < iVar3) {
          iVar3 = 0;
          goto LAB_0012714c;
        }
      }
LAB_00127154:
      if (DAT_00182ac0 == 0) {
LAB_001271c0:
        if (DAT_00182ac8 != 0) {
          mem_free();
          DAT_00182ac8 = 0;
        }
        FUN_0012b5c0(&local_28,0,0);
      }
      else {
        iVar3 = DAT_00182ac8 + DAT_00182ac4 * 0x10;
        iVar3 = file_read(&DAT_00182b78,*(int *)(iVar3 + 4) + (uint)*(ushort *)(iVar3 + 0xc),
                          DAT_00182acc[1],*(undefined4 *)(iVar3 + 8));
        if (0 < iVar3) {
          DAT_00182acc[3] = iVar3;
        }
        DAT_00182ac4 = DAT_00182ac4 + 1;
      }
      FUN_0012bd70(local_28,0,1,0xffff,0);
    }
    FUN_0010d3f8(100);
    FUN_00106924();
    FUN_00105eb8();
    DAT_00182b14 = FUN_0012a298;
  }
  else if (param_2 == 6) {
    dbg_printf("lcd_boot_proc exit\n");
    DAT_00182b34 = 0;
    DAT_00182bbc = 0;
    DAT_00182ac0 = 0;
    if (DAT_00182ac8 != 0) {
      mem_free();
      DAT_00182ac8 = 0;
    }
    FUN_0012b7e4(DAT_00182b2c,0x20,0,0,0);
  }
  else if (param_2 == 0x15) {
    if ((DAT_00182bc0 == 0) && (iVar3 = FUN_00105e70(), iVar3 != 0)) {
      if (DAT_00182ac4 < DAT_00182ac0) {
        iVar3 = DAT_00182ac8 + DAT_00182ac4 * 0x10;
        file_read(&DAT_00182b78,*(int *)(iVar3 + 4) + (uint)*(ushort *)(iVar3 + 0xc),&DAT_0066ab00,
                  *(undefined4 *)(iVar3 + 8));
        DAT_00182ac4 = DAT_00182ac4 + 1;
        *(undefined4 *)(*DAT_00182acc + 0xc) = 0xffffffff;
        FUN_001056b4();
        FUN_001065d4(8,&DAT_00006021,0);
        FUN_001065d4(0,*(uint *)(*DAT_00182acc + 4) | 0x80000000);
        FUN_001065d4(0x11,&LAB_001155a4,0);
        FUN_001065d4(4,&DAT_0066ab00,0);
        FUN_001065d4(0xc,0);
      }
      else {
        *(undefined4 *)(*DAT_00182acc + 0xc) = 0;
        if (g_config.idle_state == 1) {
          DAT_00182ac4 = 0;
        }
        else {
          FUN_0012b5c0(&local_28,0,0);
        }
      }
    }
  }
  else if (param_2 == 0x19) {
    dbg_printf("lcd_boot_proc reboot\n");
    DAT_00182bcc = 1;
    FUN_0010e41c();
    FUN_00102990(1,FUN_00119b70);
    FUN_001029ec(0);
    FUN_00102938();
    FUN_00102964();
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  return 0;
LAB_0012714c:
  if ((int)(uVar1 & 0xfff) <= iVar3) goto LAB_00127154;
  iVar2 = DAT_00182ac8 + iVar3 * 0x10;
  if (*(int *)(iVar2 + 4) +
      *(int *)(iVar2 + 8) + (uint)*(ushort *)(iVar2 + 0xc) + (*(ushort *)(iVar2 + 0xe) & 0xffff0fff)
      + *(int *)(DAT_00182ac8 + iVar3 * 0x10) != 0) {
    DAT_00182ac0 = 0;
    goto LAB_001271c0;
  }
  iVar3 = iVar3 + 1;
  DAT_00182ac0 = DAT_00182ac0 + 1;
  goto LAB_0012714c;
}

// ==== FUN_00127428 @ 00127428, 52 bytes
// called by: FUN_0012745c
uint FUN_00127428(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = param_1 - 0x30;
  if (9 < uVar1) {
    if (param_1 - 0x61U < 0x1a) {
      uVar1 = param_1 - 0x57;
    }
    else {
      uVar1 = param_2;
      if (param_1 - 0x41U < 6) {
        uVar1 = param_1 - 0x37;
      }
    }
  }
  return uVar1;
}

// ==== FUN_0012745c @ 0012745c, 112 bytes
// called by: usb_command
undefined4 FUN_0012745c(char *param_1,byte *param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  while( true ) {
    if (*param_1 == '\0' || param_3 * 2 <= (int)uVar2) {
      return 1;
    }
    *param_2 = *param_2 << 4;
    iVar1 = FUN_00127428(*param_1,0xffffffff);
    if (iVar1 == -1) break;
    param_1 = param_1 + 1;
    *param_2 = (byte)iVar1 | *param_2;
    param_2 = param_2 + (uVar2 & 1);
    uVar2 = uVar2 + 1;
  }
  return 0;
}

// ==== usb_command @ 001274cc, 1964 bytes
// the command switch: 0x16 colour, 0x0A/0x0B uploads, 0x85 serial, ...
/* WARNING: Type propagation algorithm not settling */
/* the command switch: 0x16 colour, 0x0A/0x0B uploads, 0x85 serial, ... */

undefined4 usb_command(undefined4 param_1,int param_2,int param_3,uint *param_4)

{
  byte bVar1;
  undefined *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  undefined4 uVar7;
  undefined4 local_160;
  undefined4 local_15c;
  undefined1 auStack_ac [16];
  char local_9c;
  undefined1 auStack_9b [43];
  byte local_70 [60];
  undefined4 uStack_34;
  int iStack_30;
  int local_2c;
  uint *puStack_28;
  
  if (param_2 == 3) {
    return 0xffffffff;
  }
  if (param_2 != 0x35) {
    return 0xffffffff;
  }
  uStack_34 = param_1;
  iStack_30 = param_2;
  local_2c = param_3;
  puStack_28 = param_4;
  if ((g_config.idle_state == 2) && (DAT_00182bc4 = 0, DAT_00182af8 == 0)) {
    DAT_00182af8 = 1;
    FUN_0010d3f8();
  }
  if (local_2c == 0x16) {
    DAT_00182b38 = 1;
    g_config.text_colour = *param_4;
    return 0xffffffff;
  }
  if (0x16 < local_2c) {
    uVar7 = 0x105;
    if (local_2c == 0x81) {
      param_4 = (uint *)0x0;
      uVar7 = 1;
      pbVar6 = &g_config.rotation;
      iVar3 = 0x81;
      local_160 = 0;
      goto LAB_00127c6c;
    }
    if (local_2c < 0x82) {
      if (local_2c == 0x1b) {
        DAT_007bcabc = (byte)*param_4;
      }
      else {
        if (local_2c < 0x1c) {
          if (local_2c != 0x17) {
            if (local_2c == 0x18) {
              DAT_00182b38 = 1;
              g_config.idle_hide = *param_4;
              return 0xffffffff;
            }
            if (local_2c != 0x19) {
              if (local_2c == 0x1a) {
                DAT_00182b38 = 1;
                g_config.temp_scale = (byte)*param_4;
                return 0xffffffff;
              }
              return 0;
            }
            DAT_00182b38 = 1;
            g_config.unknown_108 = *param_4;
            return 0xffffffff;
          }
          g_config.idle_text_x = *param_4;
          g_config.idle_text_y = param_4[1];
          goto LAB_00127a54;
        }
        if (local_2c != 0x1c) {
          if (local_2c == 0x1d) {
            DAT_00182b38 = 1;
            g_config.unknown_10c = *param_4;
            return 0xffffffff;
          }
          if (local_2c == 0x1e) {
            g_config.idle_state = (byte)*param_4;
            uVar4 = (uint)g_config.idle_state;
            uVar5 = uVar4;
            if (uVar4 == 1) {
              param_4 = &DAT_00182b34;
              uVar5 = DAT_00182b34;
            }
            if (uVar4 != 1 || uVar5 != 0) {
              DAT_00182b38 = 1;
              return 0xffffffff;
            }
            local_160 = 0;
            local_15c = 0;
            uVar5 = FUN_0012b580(&DAT_00182b1c,lcd_boot_proc,0);
            *param_4 = uVar5;
            DAT_00182b38 = 1;
            return 0xffffffff;
          }
          if (local_2c != 0x80) {
            return 0;
          }
          pbVar6 = &DAT_00182bb4;
          param_4 = (uint *)0x0;
          uVar7 = 8;
          local_160 = 0;
          iVar3 = 0x80;
          goto LAB_00127c6c;
        }
        FUN_0010c9b8(&DAT_007bcadd,param_4,0x105);
      }
      FUN_0010c9b8(&local_160,&DAT_007bcabc,0x126);
      store_write(&DAT_00182b5c,&local_160,0x126,0);
      return 0xffffffff;
    }
    if (local_2c == 0x86) {
      param_4 = (uint *)0x0;
      pbVar6 = &DAT_007bcadd;
      iVar3 = 0x86;
      local_160 = 0;
      goto LAB_00127c6c;
    }
    if (local_2c < 0x87) {
      if (local_2c == 0x82) {
        pbVar6 = (byte *)&g_coolant;
        param_4 = (uint *)0x0;
        uVar7 = 4;
        local_160 = 0;
        iVar3 = 0x82;
      }
      else if (local_2c == 0x83) {
        param_4 = (uint *)0x0;
        uVar7 = 1;
        pbVar6 = &g_config.temp_scale;
        local_160 = 0;
        iVar3 = 0x83;
      }
      else if (local_2c == 0x84) {
        pbVar6 = &DAT_007bcabc;
        param_4 = (uint *)0x0;
        uVar7 = 1;
        local_160 = 0;
        iVar3 = 0x84;
      }
      else {
        if (local_2c != 0x85) {
          return 0;
        }
        param_4 = (uint *)0x0;
        uVar7 = 0x20;
        pbVar6 = &DAT_007bcabd;
        local_160 = 0;
        iVar3 = 0x85;
      }
      goto LAB_00127c6c;
    }
    if (local_2c == 0x87) {
      local_15c = CONCAT22(local_15c._2_2_,0x75);
      param_4 = (uint *)0x0;
      uVar7 = 2;
      pbVar6 = (byte *)&local_15c;
      iVar3 = 0x87;
      local_160 = 0;
      goto LAB_00127c6c;
    }
    if (local_2c != 0xfd) {
      if (local_2c != 0xfe) {
        if (local_2c != 0xff) {
          return 0;
        }
        if (*param_4 == 1) {
          local_160 = 0;
          FUN_0012bac8(DAT_00182b30,0x1d,0);
          return 0xffffffff;
        }
        return 0xffffffff;
      }
      FUN_0010c9b8(auStack_ac,param_4,0x3c);
      FUN_0012a848(&local_15c,"Shenzhen Xinyao Technology Co., Ltd.",0x25);
      FUN_0012a720(&local_15c,auStack_ac,auStack_ac,0x3c);
      local_70[0] = 0;
      local_70[1] = 0;
      local_70[2] = 0;
      local_70[3] = 0;
      if (local_9c == '+') {
        local_9c = '0';
        FUN_0012745c(auStack_ac,local_70 + 4,8);
        iVar3 = FUN_0010c8a0(&DAT_00182bb4,local_70 + 4,8);
        if (iVar3 == 0) {
          FUN_0010c9b8(&DAT_007bcabd,auStack_9b,0x20);
          store_write(&DAT_00182b5c,&DAT_007bcabc,0x126,0);
          local_70[0] = 1;
          local_70[1] = 0;
          local_70[2] = 0;
          local_70[3] = 0;
        }
      }
      param_4 = (uint *)0x0;
      uVar7 = 4;
      pbVar6 = local_70;
      local_160 = 0;
      iVar3 = 0xfe;
      goto LAB_00127c6c;
    }
    goto LAB_00127850;
  }
  switch(local_2c) {
  case 0:
    return 0;
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
  case 8:
    DAT_00182bbd = 0;
    DAT_00182bc0 = g_config.unknown_108;
    FUN_001056b4();
    FUN_001065d4(8,&DAT_00006021,0);
    uVar7 = FUN_001083f8();
    FUN_001065d4(0,uVar7);
    FUN_001065d4(0x11,&DAT_001155ac,0);
    FUN_001065d4(4,(uint)param_4 | 0x80000000,0);
    FUN_001065d4(0xc,0);
    return 0xffffffff;
  case 9:
    FUN_0010c9b8(&local_160,param_4,4);
    if (local_160._3_1_ == '\0') {
      DAT_00182bbd = 0;
      DAT_00182bc0 = 0;
      DAT_00182bc8 = 0;
      FUN_0010d3f8(g_config.brightness);
      return 0xffffffff;
    }
    DAT_00182bc0 = 0xffffffff;
    bVar1 = 0;
    if (local_160._2_1_ == '\0' && (local_160._1_1_ == '\0' && (char)local_160 == '\0')) {
      bVar1 = 0x80;
    }
    FUN_0010d3f8(bVar1 | g_config.brightness);
    local_15c = CONCAT13(0xff,(int3)local_160);
    FUN_0011147c(0);
    text_set_colour(~local_15c & 0xff000000 | (local_15c & 0xffffff) >> 0x10 | local_15c & 0xff00 |
                    (local_15c & 0xff) << 0x10);
    thunk_FUN_001169f0(0);
    FUN_00112f40(2);
    FUN_0010eaec(0,0,0x1e0);
    DAT_00182bc8 = g_config.unknown_10c;
    return 0xffffffff;
  case 10:
    DAT_00182bbd = 1;
    DAT_00182bc0 = g_config.unknown_108;
    if (*param_4 == 0) {
      puVar2 = &DAT_00182b94;
    }
    else {
      puVar2 = &DAT_00182b78;
    }
    iVar3 = FUN_00128b70(puVar2,&DAT_007bc300,param_4[1]);
    if (iVar3 < 1) {
      param_4[1] = 0xffffffff;
    }
    local_160 = 8;
    pbVar6 = *(byte **)(DAT_00182b10 + 0x18);
    uVar7 = 0xc;
    iVar3 = 10;
    goto LAB_00127c6c;
  case 0xb:
    DAT_00182bbd = 1;
    DAT_00182bc0 = g_config.unknown_108;
    uVar5 = FUN_00128c3c(&DAT_007bc300,*param_4,param_4 + 3,param_4[1]);
    if ((int)uVar5 < 1) {
      param_4[2] = uVar5;
    }
    else {
      uVar5 = param_4[2];
    }
    if (99 < (int)uVar5) {
      uVar5 = FUN_00128cec(&DAT_007bc300);
      if ((int)uVar5 < 1) {
        param_4[2] = uVar5;
      }
      local_160 = 0;
      FUN_0012b7e4(DAT_00182b2c,0x20,0);
    }
    local_160 = 0xc;
    pbVar6 = *(byte **)(DAT_00182b10 + 0x18);
    uVar7 = 0xc;
    iVar3 = 0xb;
    goto LAB_00127c6c;
  case 0xc:
    if (*param_4 == 0) {
      DAT_00182b10 = &DAT_00182b94;
    }
    else {
      DAT_00182b10 = &DAT_00182b78;
    }
    uVar5 = file_size();
    param_4[1] = uVar5;
    local_160 = 8;
    uVar7 = 0xc;
    pbVar6 = *(byte **)(DAT_00182b10 + 0x18);
    iVar3 = 0xc;
    goto LAB_00127c6c;
  case 0xd:
    iVar3 = 0;
    if (DAT_00182b10 == (undefined *)0x0) {
      uVar5 = 0xffffffe9;
    }
    else {
      iVar3 = mem_alloc(param_4[1]);
      if (iVar3 == 0) {
        uVar5 = 0xffffffeb;
      }
      else {
        uVar5 = file_read(DAT_00182b10,*param_4,iVar3,param_4[1]);
        if (0 < (int)uVar5) {
          param_4[1] = uVar5;
          goto LAB_00127818;
        }
      }
    }
    param_4[2] = uVar5;
LAB_00127818:
    local_160 = 0xc;
    usb_receive_data(0xd,iVar3);
    if (iVar3 != 0) {
      mem_free(iVar3);
      return 0xffffffff;
    }
    return 0xffffffff;
  case 0xe:
    return 0;
  case 0xf:
    return 0;
  case 0x10:
    if ((byte)*param_4 == 0) {
      g_config.brightness = g_config.brightness & 0x7f;
    }
    else {
      g_config.brightness = g_config.brightness | 0x80;
    }
    goto LAB_001279d4;
  case 0x11:
    g_config.rotation = (byte)*param_4;
    display_set_rotation((&DAT_00180224)[g_config.rotation & 3]);
LAB_00127a54:
    local_160 = 0;
    FUN_0012b7e4(DAT_00182b2c,0x20,0);
    DAT_00182b38 = 1;
    return 0xffffffff;
  case 0x12:
    g_config.brightness = g_config.brightness & 0x80 | (byte)*param_4 & 0x7f;
LAB_001279d4:
    FUN_0010d3f8(g_config.brightness);
    DAT_00182b38 = 1;
    return 0xffffffff;
  case 0x13:
    param_4 = (uint *)0x0;
    uVar7 = 0;
    pbVar6 = (byte *)0x0;
    iVar3 = 0x13;
    local_160 = 0;
    goto LAB_00127c6c;
  case 0x14:
    DAT_00182b38 = 1;
    g_config.boot_frame_interval = (byte)*param_4;
    return 0xffffffff;
  case 0x15:
    param_4 = (uint *)0x0;
    uVar7 = 0;
    pbVar6 = (byte *)0x0;
    iVar3 = 0x15;
    local_160 = 0;
    goto LAB_00127c6c;
  default:
    return 0;
  }
LAB_00127850:
  param_4 = (uint *)0x0;
  uVar7 = 0;
  pbVar6 = (byte *)0x0;
  local_160 = 0;
  iVar3 = local_2c;
LAB_00127c6c:
  usb_receive_data(iVar3,pbVar6,uVar7,param_4);
  return 0xffffffff;
}

// ==== config_load @ 00127d00, 392 bytes
// refs "load config ok"; loads the record from the flash store at boot
/* refs "load config ok"; loads the record from the flash store at boot */

undefined4 config_load(undefined4 param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 local_138 [74];
  
  if (param_2 == 3) {
    FUN_0012acf8();
    FUN_00128f28(&DAT_00182b40);
    FUN_00128f28(&DAT_00182b5c);
    FUN_00128a24(&DAT_00182b78);
    FUN_00128a24(&DAT_00182b94);
    local_138[0] = 0x126;
    iVar1 = store_read(&DAT_00182b5c,0,&DAT_007bcabc,0x126);
    if (iVar1 == 0) {
      dbg_printf("load eeprom faild\n");
      FUN_0010c91c(&DAT_007bcabd,"A247392SS000000");
      FUN_0010cb60(local_138,&DAT_007bcabc,0x128);
      store_write(&DAT_00182b5c,local_138,0x126,0);
    }
    local_138[0] = 0x114;
    iVar1 = store_read(&DAT_00182b40,0,&g_config,0x114);
    if (iVar1 == 0) {
      dbg_printf("load config faild\n");
      config_defaults();
    }
    else {
      dbg_printf("load config ok\n");
    }
    display_set_rotation((&DAT_00180224)[g_config.rotation & 3]);
    g_config.unknown_108 = 1000;
    local_138[0] = 0;
    FUN_0012bd70(param_1,0,1,0xffff);
  }
  else if (param_2 == 0x15) {
    if (DAT_00182b38 == 1) {
      DAT_00182b3c = FUN_0012be44();
      DAT_00182b38 = DAT_00182b38 + 1;
    }
    else if ((DAT_00182b38 == 2) && (uVar2 = FUN_0012be54(DAT_00182b3c), 299 < uVar2)) {
      config_save();
      DAT_00182b38 = 0;
    }
  }
  else if (param_2 == 0x1d) {
    config_defaults();
    config_save();
  }
  return 0;
}

// ==== config_defaults @ 00127efc, 112 bytes
// called by: config_load
// fills the record: brightness 100, "c:\syst\boot", "c:\syst\wapper.jpg"
/* WARNING: Removing unreachable block (ram,0x0010c928) */
/* WARNING: Removing unreachable block (ram,0x0010c930) */
/* WARNING: Removing unreachable block (ram,0x0010c93a) */
/* WARNING: Removing unreachable block (ram,0x0010c946) */
/* WARNING: Removing unreachable block (ram,0x0010c92e) */
/* fills the record: brightness 100, "c:\syst\boot", "c:\syst\wapper.jpg" */

void config_defaults(void)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  
  FUN_0010cc30(&g_config,0x114);
  g_config.rotation = 0;
  g_config.brightness = 100;
  g_config.idle_text_x = 0;
  g_config.idle_text_y = 100;
  g_config.text_colour = 0xcccccccc;
  g_config.unknown_108 = (uint)&DAT_00001388;
  g_config.unknown_10c = (uint)&DAT_00001388;
  g_config.boot_frame_interval = 0x14;
  g_config.idle_state = 0;
  FUN_0010c91c(0x7bc9ab,"c:\\syst\\boot");
  pcVar3 = g_config.idle_path;
  pcVar2 = "c:\\syst\\wapper.jpg";
  do {
    cVar1 = *pcVar2;
    *pcVar3 = cVar1;
    if (cVar1 == '\0') {
      return;
    }
    cVar1 = pcVar2[1];
    pcVar2 = pcVar2 + 2;
    pcVar3[1] = cVar1;
    pcVar3 = pcVar3 + 2;
  } while (cVar1 != '\0');
  return;
}

// ==== config_save @ 00127f94, 52 bytes
// called by: config_load
// writes the 0x114-byte record back to the store
/* writes the 0x114-byte record back to the store */

void config_save(void)

{
  undefined1 auStack_118 [280];
  
  FUN_0010cb60(auStack_118,&g_config,0x114);
  store_write(&DAT_00182b40,auStack_118,0x114,0);
  return;
}

// ==== FUN_00127fd0 @ 00127fd0, 44 bytes
// called by: FUN_001284b8
int FUN_00127fd0(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  
  iVar1 = divide((param_5 - param_4) * (param_3 - param_1),param_2 - param_1);
  return iVar1 + param_4;
}

// ==== FUN_00127ffc @ 00127ffc, 144 bytes
// called by: FUN_001284b8
undefined4 FUN_00127ffc(int *param_1,int param_2,int param_3,int param_4,int *param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  param_2 = param_2 + -1;
  iVar6 = 0;
  iVar2 = param_2;
  if (*(int *)((int)param_1 + param_3) < *param_1) {
    iVar5 = 1;
  }
  else {
    iVar5 = 0;
  }
  while( true ) {
    if (iVar2 < iVar6) {
      if (iVar2 < 0) {
        *param_5 = iVar6;
      }
      else {
        if (iVar6 <= param_2) {
          *param_5 = iVar2;
          return 0;
        }
        *param_5 = param_2;
      }
      return 0xffffffff;
    }
    iVar1 = iVar6 + iVar2 >> 1;
    iVar3 = *(int *)((int)param_1 + iVar1 * param_3);
    iVar4 = iVar3;
    if (param_4 != iVar3 && iVar3 <= param_4) {
      iVar4 = 0;
    }
    if (param_4 < iVar3) {
      iVar4 = 1;
    }
    if (param_4 == iVar3) break;
    if (iVar4 == iVar5) {
      iVar6 = iVar1 + 1;
    }
    else {
      iVar2 = iVar1 + -1;
    }
  }
  *param_5 = iVar1;
  return 1;
}

