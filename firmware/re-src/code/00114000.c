// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00114000-0x00117fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_00114150 @ 00114150, 20 bytes
// called by: FUN_0011b2e8, FUN_001265c8
void FUN_00114150(void)

{
  FUN_00114164();
  return;
}

// ==== FUN_00114164 @ 00114164, 580 bytes
// called by: FUN_00114150
undefined4 FUN_00114164(byte *param_1,int param_2,int param_3,undefined4 *param_4,byte *param_5)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  undefined2 uVar4;
  byte *pbVar5;
  int iVar6;
  code *pcVar7;
  
  pcVar7 = (code *)*param_4;
  if (param_5 == (byte *)0x0) {
    param_5 = param_1;
  }
  if (param_3 < 9) {
    if (param_3 == 1) {
      do {
        cVar1 = (*pcVar7)(*(undefined4 *)param_1);
        bVar3 = cVar1 << 7;
        iVar6 = 0;
        pbVar5 = param_1 + 4;
        if (param_2 != 1) {
          cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 4));
          bVar3 = bVar3 | cVar1 << 6;
          iVar6 = 0;
          pbVar5 = param_1 + 8;
          if (param_2 != 2) {
            cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 8));
            bVar3 = bVar3 | cVar1 << 5;
            iVar6 = 0;
            pbVar5 = param_1 + 0xc;
            if (param_2 != 3) {
              cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 0xc));
              bVar3 = bVar3 | cVar1 << 4;
              iVar6 = 0;
              pbVar5 = param_1 + 0x10;
              if (param_2 != 4) {
                cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 0x10));
                bVar3 = bVar3 | cVar1 << 3;
                iVar6 = 0;
                pbVar5 = param_1 + 0x14;
                if (param_2 != 5) {
                  cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 0x14));
                  bVar3 = bVar3 | cVar1 << 2;
                  iVar6 = 0;
                  pbVar5 = param_1 + 0x18;
                  if (param_2 != 6) {
                    cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 0x18));
                    bVar3 = bVar3 | cVar1 << 1;
                    iVar6 = 0;
                    pbVar5 = param_1 + 0x1c;
                    if (param_2 != 7) {
                      pbVar5 = param_1 + 0x20;
                      bVar2 = (*pcVar7)(*(undefined4 *)(param_1 + 0x1c));
                      bVar3 = bVar2 | bVar3;
                      iVar6 = param_2 + -8;
                    }
                  }
                }
              }
            }
          }
        }
        *param_5 = bVar3;
        param_1 = pbVar5;
        param_2 = iVar6;
        param_5 = param_5 + 1;
      } while (iVar6 != 0);
    }
    else if (param_3 == 2) {
      do {
        cVar1 = (*pcVar7)(*(undefined4 *)param_1);
        bVar3 = cVar1 << 6;
        iVar6 = 0;
        pbVar5 = param_1 + 4;
        if (param_2 != 1) {
          cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 4));
          bVar3 = bVar3 | cVar1 << 4;
          iVar6 = 0;
          pbVar5 = param_1 + 8;
          if (param_2 != 2) {
            cVar1 = (*pcVar7)(*(undefined4 *)(param_1 + 8));
            bVar3 = bVar3 | cVar1 << 2;
            iVar6 = 0;
            pbVar5 = param_1 + 0xc;
            if (param_2 != 3) {
              pbVar5 = param_1 + 0x10;
              bVar2 = (*pcVar7)(*(undefined4 *)(param_1 + 0xc));
              bVar3 = bVar2 | bVar3;
              iVar6 = param_2 + -4;
            }
          }
        }
        *param_5 = bVar3;
        param_1 = pbVar5;
        param_2 = iVar6;
        param_5 = param_5 + 1;
      } while (iVar6 != 0);
    }
    else if (param_3 == 4) {
      do {
        cVar1 = (*pcVar7)(*(undefined4 *)param_1);
        bVar3 = cVar1 << 4;
        iVar6 = 0;
        pbVar5 = param_1 + 4;
        if (param_2 != 1) {
          pbVar5 = param_1 + 8;
          bVar2 = (*pcVar7)(*(undefined4 *)(param_1 + 4));
          bVar3 = bVar2 | bVar3;
          iVar6 = param_2 + -2;
        }
        *param_5 = bVar3;
        param_1 = pbVar5;
        param_2 = iVar6;
        param_5 = param_5 + 1;
      } while (iVar6 != 0);
    }
    else if (param_3 == 8) {
      if ((code *)param_4[4] == (code *)0x0) {
        do {
          bVar3 = (*pcVar7)(*(undefined4 *)param_1);
          param_2 = param_2 + -1;
          *param_5 = bVar3;
          param_1 = param_1 + 4;
          param_5 = param_5 + 1;
        } while (param_2 != 0);
      }
      else {
        (*(code *)param_4[4])(param_1,param_5,param_2,1);
      }
    }
  }
  else {
    if (0x10 < param_3) {
      return 0;
    }
    if ((code *)param_4[4] == (code *)0x0) {
      do {
        uVar4 = (*pcVar7)(*(undefined4 *)param_1);
        param_2 = param_2 + -1;
        *(undefined2 *)param_5 = uVar4;
        param_1 = param_1 + 4;
        param_5 = param_5 + 2;
      } while (param_2 != 0);
    }
    else {
      (*(code *)param_4[4])(param_1,param_5,param_2,2);
    }
  }
  return 1;
}

// ==== FUN_001143a8 @ 001143a8, 784 bytes
// called by: text_draw
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_001143a8(undefined4 param_1,int param_2,short *param_3)

{
  short sVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int local_30;
  undefined4 local_2c;
  short local_28;
  short local_26;
  short local_24;
  short local_22;
  
  if (DAT_00182f58 == 0) {
    local_28 = *param_3 + *(short *)(DAT_00182efc + 0x48);
    local_26 = param_3[1] + *(short *)(DAT_00182efc + 0x4c);
    local_24 = param_3[2] + *(short *)(DAT_00182efc + 0x48);
    local_22 = param_3[3] + *(short *)(DAT_00182efc + 0x4c);
    iVar3 = FUN_00118f24(&local_28);
    while (iVar2 = DAT_00182efc, iVar3 != 0) {
      *(short *)(DAT_00182efc + 0x22) = local_28;
      *(short *)(iVar2 + 0x24) = local_26;
      local_30 = param_2;
      local_2c = param_1;
      local_2c = (*DAT_00182ff4)(param_1,&local_30,1);
      if ((*(int *)(DAT_00182efc + 0x14) == 0) ||
         (iVar3 = FUN_00112b2c(*(int *)(DAT_00182efc + 0x14),&local_28), iVar3 != 0)) {
        FUN_00114f5c(1);
        iVar3 = *(int *)(*(int *)(DAT_00182efc + 0x1c) + 0x14);
        if (iVar3 == 0) {
          while (local_30 = local_30 + -1, -1 < local_30) {
            uVar4 = FUN_00113acc(&local_2c);
            (*(code *)**(undefined4 **)(DAT_00182efc + 0x1c))(uVar4);
            if (DAT_00182f3c != (code *)0x0) {
              (*DAT_00182f3c)(uVar4);
            }
          }
        }
        else if (DAT_00182f40 == (code *)0x0) {
          (**(code **)(iVar3 + 8))(local_2c,local_30);
        }
        else {
          sVar1 = *(short *)(DAT_00182efc + 0x22);
          (**(code **)(iVar3 + 8))(local_2c,local_30);
          (*DAT_00182f40)((int)sVar1);
        }
        FUN_00114f5c(0);
      }
      iVar3 = FUN_001189d8();
    }
    *(short *)(DAT_00182efc + 0x22) =
         *(short *)(DAT_00182efc + 0x22) - *(short *)(DAT_00182efc + 0x48);
    *(short *)(iVar2 + 0x24) = *(short *)(iVar2 + 0x24) - *(short *)(iVar2 + 0x4c);
    return;
  }
  iVar3 = FUN_00118f24(0);
  if (iVar3 != 0) {
    do {
      local_30 = param_2;
      local_2c = (*DAT_00182ff4)(param_1,&local_30,1);
      if ((*(int *)(DAT_00182efc + 0x14) == 0) ||
         (iVar3 = FUN_00112b2c(*(int *)(DAT_00182efc + 0x14),&local_28), iVar3 != 0)) {
        FUN_00114f5c(1);
        iVar3 = *(int *)(*(int *)(DAT_00182efc + 0x1c) + 0x14);
        if (iVar3 == 0) {
          while (local_30 = local_30 + -1, -1 < local_30) {
            uVar4 = FUN_00113acc(&local_2c);
            (*(code *)**(undefined4 **)(DAT_00182efc + 0x1c))(uVar4);
            if (DAT_00182f3c != (code *)0x0) {
              (*DAT_00182f3c)(uVar4);
            }
          }
        }
        else if (DAT_00182f40 == (code *)0x0) {
          (**(code **)(iVar3 + 8))(local_2c,local_30);
        }
        else {
          sVar1 = *(short *)(DAT_00182efc + 0x22);
          (**(code **)(iVar3 + 8))(local_2c,local_30);
          (*DAT_00182f40)((int)sVar1);
        }
        FUN_00114f5c(0);
      }
      iVar3 = FUN_001189d8();
    } while (iVar3 != 0);
    return;
  }
  return;
}

// ==== FUN_001146cc @ 001146cc, 732 bytes
// called by: FUN_001265c8
/* WARNING: Restarted to delay deadcode elimination for space: stack */

int FUN_001146cc(int param_1,undefined4 param_2,uint *param_3,int param_4,code *param_5,
                uint *param_6)

{
  bool bVar1;
  int iVar2;
  code *pcVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  int iVar8;
  uint *puStack_60;
  uint *puStack_5c;
  int iStack_58;
  uint uStack_54;
  int iStack_50;
  int iStack_4c;
  uint uStack_48;
  undefined4 uStack_44;
  int iStack_40;
  int aiStack_3c [2];
  int iStack_34;
  undefined4 uStack_30;
  uint *puStack_2c;
  int iStack_28;
  
  *param_6 = 0;
  iVar8 = 0;
  bVar1 = false;
  iStack_34 = param_1;
  uStack_30 = param_2;
  puStack_2c = param_3;
  iStack_28 = param_4;
  iVar2 = FUN_00114b88(&puStack_5c,&puStack_60,0,aiStack_3c);
  if (iVar2 != 0) {
    return 0;
  }
  if (aiStack_3c[0] < param_4) {
    param_4 = aiStack_3c[0];
  }
  iStack_40 = (&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)];
  uStack_44 = *(undefined4 *)(iStack_40 + 0x10);
  iStack_4c = FUN_0010eb64();
  pcVar3 = (code *)FUN_0010ecb0(*(undefined1 *)(DAT_00182efc + 0x11));
  uStack_48 = (uint)(pcVar3 != (code *)&LAB_001163d0);
  iStack_50 = iStack_34 + param_4;
  do {
    uStack_54 = (*param_5)(*param_3);
    uVar5 = uStack_54 >> 0x18;
    if (uVar5 < 0xff) {
      if (!bVar1) {
        iVar6 = iStack_34 + iVar8;
        iVar2 = (int)*(short *)(DAT_00182efc + 8);
        if (iVar6 < iVar2) {
          iStack_58 = iVar2 - iVar6;
        }
        else {
          iStack_58 = 0;
          iVar2 = iVar6;
        }
        FUN_00112a74(iVar2,uStack_30,iStack_50 + -1);
        bVar1 = true;
        if (iStack_4c < 0x18) {
          FUN_001149b8(puStack_5c + iVar8 + iStack_58,param_4 - iStack_58,iStack_4c,uStack_44);
        }
        else if (uStack_48 != 0) {
          iVar2 = iStack_50 - iVar2;
          puVar7 = puStack_5c + iVar8 + iStack_58;
          do {
            uVar4 = (*pcVar3)(*puVar7);
            iVar2 = iVar2 + -1;
            *puVar7 = uVar4;
            puVar7 = puVar7 + 1;
          } while (iVar2 != 0);
        }
      }
      puVar7 = puStack_60;
      if (DAT_0018305c != (code *)0x0) {
        puStack_5c = puStack_5c + iVar8;
        puStack_60 = puStack_60 + iVar8;
        (*DAT_0018305c)(param_3,puStack_5c,puStack_60,param_4);
        do {
          if ((*param_3 & 0xff000000) == 0) {
            *puStack_60 = *puStack_5c;
          }
          else if ((*puStack_5c & 0xff000000) == 0) {
            *puStack_60 = *param_3;
          }
          param_4 = param_4 + -1;
          puStack_5c = puStack_5c + 1;
          param_3 = param_3 + 1;
          puStack_60 = puStack_60 + 1;
        } while (param_4 != 0);
        *param_6 = *param_6 | 1;
        return (int)puVar7;
      }
      if (uVar5 == 0) {
        puStack_60[iVar8] = puStack_5c[iVar8];
      }
      else {
        uVar5 = (*DAT_00183040)(uStack_54 | 0xff000000,puStack_5c[iVar8],uVar5);
        puStack_60[iVar8] = uVar5;
        *param_6 = *param_6 | 1;
      }
    }
    else {
      puStack_60[iVar8] = uStack_54;
      *param_6 = *param_6 | 1;
    }
    param_4 = param_4 + -1;
    iVar8 = iVar8 + 1;
    param_3 = param_3 + 1;
    if (param_4 == 0) {
      return (int)puStack_60;
    }
  } while( true );
}

// ==== FUN_001149b8 @ 001149b8, 464 bytes
// called by: FUN_001146cc
void FUN_001149b8(int param_1,uint param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  uint uVar2;
  undefined1 *puVar3;
  undefined2 *puVar4;
  uint uVar5;
  code *pcVar6;
  undefined4 *puVar7;
  uint uVar8;
  byte *pbVar9;
  byte *pbVar10;
  
  if ((int)param_2 < 1) {
    return;
  }
  pcVar6 = *(code **)(param_4 + 4);
  puVar7 = (undefined4 *)(param_1 + param_2 * 4);
  if (8 < param_3) {
    if (0x10 < param_3) {
      return;
    }
    if (*(code **)(param_4 + 0x14) != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x00114a44. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(param_4 + 0x14))(param_1,param_1,param_2,2);
      return;
    }
    puVar4 = (undefined2 *)(param_1 + param_2 * 2);
    do {
      puVar4 = puVar4 + -1;
      uVar1 = (*pcVar6)(*puVar4);
      param_2 = param_2 - 1;
      puVar7 = puVar7 + -1;
      *puVar7 = uVar1;
    } while (param_2 != 0);
    return;
  }
  uVar2 = param_2 - 1;
  if (param_3 == 1) {
    pbVar9 = (byte *)(param_1 + ((int)uVar2 >> 3));
    uVar8 = (uint)*pbVar9;
    uVar2 = uVar2 & 7;
    uVar5 = 0x80 >> uVar2;
    pbVar9 = pbVar9 + -1;
    do {
      puVar7 = puVar7 + -1;
      uVar1 = (*pcVar6)((uVar8 & uVar5) >> (7 - uVar2 & 0xff));
      *puVar7 = uVar1;
      if (uVar2 == 0) {
        pbVar10 = pbVar9 + -1;
        uVar8 = (uint)*pbVar9;
        uVar2 = 7;
        uVar5 = 1;
      }
      else {
        uVar5 = (uVar5 & 0x7f) << 1;
        uVar2 = uVar2 - 1;
        pbVar10 = pbVar9;
      }
      param_2 = param_2 - 1;
      pbVar9 = pbVar10;
    } while (param_2 != 0);
    return;
  }
  if (param_3 == 2) {
    pbVar9 = (byte *)(param_1 + ((int)uVar2 >> 2));
    uVar2 = uVar2 & 3;
    uVar8 = (uint)*pbVar9;
    uVar5 = 0xc0 >> (uVar2 << 1);
    pbVar9 = pbVar9 + -1;
    do {
      puVar7 = puVar7 + -1;
      uVar1 = (*pcVar6)((uVar8 & uVar5) >> (uVar2 * -2 + 6 & 0xff));
      *puVar7 = uVar1;
      if (uVar2 == 0) {
        uVar2 = 3;
        pbVar10 = pbVar9 + -1;
        uVar8 = (uint)*pbVar9;
        uVar5 = 3;
      }
      else {
        uVar5 = (uVar5 & 0x3f) << 2;
        uVar2 = uVar2 - 1;
        pbVar10 = pbVar9;
      }
      param_2 = param_2 - 1;
      pbVar9 = pbVar10;
    } while (param_2 != 0);
    return;
  }
  if (param_3 == 4) {
    pbVar9 = (byte *)(param_1 + ((int)uVar2 >> 1));
    uVar5 = ~param_2 & 1;
    uVar8 = (uint)*pbVar9;
    uVar2 = 0xf0 >> (uVar5 << 2);
    pbVar9 = pbVar9 + -1;
    do {
      puVar7 = puVar7 + -1;
      uVar1 = (*pcVar6)((uVar8 & uVar2) >> (uVar5 * -4 + 4 & 0xff));
      *puVar7 = uVar1;
      if (uVar5 == 0) {
        pbVar10 = pbVar9 + -1;
        uVar8 = (uint)*pbVar9;
        uVar5 = 1;
        uVar2 = 0xf;
      }
      else {
        uVar2 = (uVar2 & 0xf) << 4;
        uVar5 = uVar5 - 1;
        pbVar10 = pbVar9;
      }
      param_2 = param_2 - 1;
      pbVar9 = pbVar10;
    } while (param_2 != 0);
    return;
  }
  if (param_3 != 8) {
    return;
  }
  puVar3 = (undefined1 *)(param_1 + param_2);
  do {
    puVar3 = puVar3 + -1;
    uVar1 = (*pcVar6)(*puVar3);
    param_2 = param_2 - 1;
    puVar7 = puVar7 + -1;
    *puVar7 = uVar1;
  } while (param_2 != 0);
  return;
}

// ==== FUN_00114b88 @ 00114b88, 268 bytes
// called by: FUN_00113ec8, FUN_001146cc
bool FUN_00114b88(int *param_1,int *param_2,int *param_3,int *param_4)

{
  bool bVar1;
  
  bVar1 = false;
  if (DAT_00183058 == 0) {
    DAT_00183058 = 1;
    DAT_00183068 = FUN_00124d40;
    FUN_00115274(&DAT_00183068);
  }
  if ((DAT_00183054 == 0) && (DAT_00183054 = DAT_00182f44, DAT_00182f44 == 0)) {
    DAT_00183054 = FUN_00115fc8();
  }
  if (param_4 != (int *)0x0) {
    *param_4 = DAT_00183054;
  }
  if (param_1 != (int *)0x0) {
    if (DAT_00183048 == 0) {
      DAT_00183048 = FUN_0010e12c(DAT_00183054 << 2);
      bVar1 = DAT_00183048 == 0;
    }
    *param_1 = DAT_00183048;
  }
  if (param_2 != (int *)0x0) {
    if ((DAT_0018304c == 0) && (DAT_0018304c = FUN_0010e12c(DAT_00183054 << 2), DAT_0018304c == 0))
    {
      bVar1 = true;
    }
    *param_2 = DAT_0018304c;
  }
  if (param_3 != (int *)0x0) {
    if ((DAT_00183050 == 0) && (DAT_00183050 = FUN_0010e12c(DAT_00183054 << 2), DAT_00183050 == 0))
    {
      bVar1 = true;
    }
    *param_3 = DAT_00183050;
  }
  return bVar1;
}

// ==== FUN_00114ca0 @ 00114ca0, 200 bytes
// called by: text_draw
int FUN_00114ca0(int param_1,int param_2)

{
  int iVar1;
  undefined4 *puVar2;
  code *pcVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int local_20 [2];
  int local_18;
  int local_14;
  
  pcVar3 = (code *)0x0;
  local_18 = param_1;
  local_14 = param_2;
  local_18 = (*DAT_00182ff4)(param_1,&local_14);
  iVar4 = 0;
  if (local_18 != 0) {
    uVar5 = 0;
    puVar2 = *(undefined4 **)(*(int *)(DAT_00182efc + 0x1c) + 0x14);
    if (puVar2 != (undefined4 *)0x0) {
      pcVar3 = (code *)*puVar2;
    }
    if (puVar2 != (undefined4 *)0x0 && pcVar3 != (code *)0x0) {
      iVar4 = (*pcVar3)(local_18,local_14);
      return iVar4;
    }
    while (local_14 = local_14 + -1, -1 < local_14) {
      uVar5 = FUN_00113acc(&local_18);
      iVar1 = FUN_0010eb90();
      iVar4 = iVar4 + iVar1;
    }
    iVar6 = 0;
    local_20[0] = 0;
    iVar1 = FUN_0010ebac(uVar5,local_20);
    if (iVar1 < local_20[0]) {
      iVar6 = local_20[0] - iVar1;
    }
    iVar4 = iVar4 + iVar6;
  }
  return iVar4;
}

// ==== FUN_00114d70 @ 00114d70, 120 bytes
// called by: text_draw
int FUN_00114d70(int param_1,int param_2,code *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  bool bVar5;
  int local_18;
  int iStack_14;
  
  iVar3 = 0;
  if (param_1 != 0) {
    iVar2 = *(int *)(*(int *)(DAT_00182efc + 0x1c) + 0x14);
    if (iVar2 != 0) {
      param_3 = *(code **)(iVar2 + 4);
    }
    local_18 = param_1;
    iStack_14 = param_2;
    if (iVar2 != 0 && param_3 != (code *)0x0) {
      iVar3 = (*param_3)(param_1,param_2);
      return iVar3;
    }
    if (0 < param_2) {
      do {
        iVar1 = FUN_00113acc(&local_18);
        bVar4 = iVar1 != 0;
        iVar2 = iVar1;
        if (bVar4) {
          iVar2 = iVar1 + -10;
        }
        bVar5 = bVar4 && SBORROW4(iVar1,10);
        if (bVar4 && iVar1 != 10) {
          iVar3 = iVar3 + 1;
          bVar5 = SBORROW4(iVar3,param_2);
          iVar2 = iVar3 - param_2;
        }
      } while (iVar2 < 0 != bVar5);
    }
  }
  return iVar3;
}

// ==== FUN_00114dec @ 00114dec, 132 bytes
// called by: text_width
int FUN_00114dec(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (param_1 != 0) {
    iVar1 = (*(code *)*DAT_00182f04)(param_1);
    iVar2 = (*(code *)DAT_00182f04[1])(param_1);
    param_1 = param_1 + iVar2;
    while (iVar1 != 0) {
      iVar3 = iVar3 + 1;
      iVar1 = (*(code *)*DAT_00182f04)(param_1);
      iVar2 = (*(code *)DAT_00182f04[1])(param_1);
      param_1 = param_1 + iVar2;
    }
  }
  return iVar3;
}

// ==== FUN_00114e74 @ 00114e74, 40 bytes
// called by: FUN_0010dce4
int FUN_00114e74(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int local_10;
  
  iVar2 = 0;
  local_10 = 0;
  iVar1 = FUN_0010ebac(param_1,&local_10);
  if (iVar1 < local_10) {
    iVar2 = local_10 - iVar1;
  }
  return iVar2;
}

// ==== FUN_00114e9c @ 00114e9c, 84 bytes
// called by: FUN_00118820, FUN_001186b4, FUN_00118f24, FUN_001180e8, FUN_001183e0
void FUN_00114e9c(short *param_1,short *param_2)

{
  short sVar1;
  
  sVar1 = *param_2;
  if (*param_2 < *param_1) {
    sVar1 = *param_1;
  }
  *param_1 = sVar1;
  sVar1 = param_2[1];
  if (param_2[1] < param_1[1]) {
    sVar1 = param_1[1];
  }
  param_1[1] = sVar1;
  sVar1 = param_2[2];
  if (param_1[2] <= param_2[2]) {
    sVar1 = param_1[2];
  }
  param_1[2] = sVar1;
  sVar1 = param_1[3];
  if (param_2[3] < param_1[3]) {
    sVar1 = param_2[3];
  }
  param_1[3] = sVar1;
  return;
}

// ==== FUN_00114ef0 @ 00114ef0, 108 bytes
// called by: FUN_00122b34, FUN_00118250, FUN_001196dc, FUN_0012297c, FUN_00122a60, FUN_00124e84, FUN_00116a28, FUN_00124394, FUN_001192f8
bool FUN_00114ef0(undefined2 *param_1,short *param_2,short *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  
  iVar3 = (int)*param_3;
  if ((int)*param_3 < (int)*param_2) {
    iVar3 = (int)*param_2;
  }
  *param_1 = (short)iVar3;
  iVar5 = (int)param_3[1];
  if ((int)param_3[1] < (int)param_2[1]) {
    iVar5 = (int)param_2[1];
  }
  param_1[1] = (short)iVar5;
  iVar4 = (int)param_3[2];
  if ((int)param_2[2] < (int)param_3[2]) {
    iVar4 = (int)param_2[2];
  }
  param_1[2] = (short)iVar4;
  iVar2 = (int)param_2[3];
  if ((int)param_3[3] <= (int)param_2[3]) {
    iVar2 = (int)param_3[3];
  }
  bVar6 = SBORROW4(iVar4,iVar3);
  iVar1 = iVar4 - iVar3;
  param_1[3] = (short)iVar2;
  if (iVar3 <= iVar4) {
    bVar6 = SBORROW4(iVar2,iVar5);
    iVar1 = iVar2 - iVar5;
  }
  return iVar1 < 0 == bVar6;
}

// ==== FUN_00114f5c @ 00114f5c, 392 bytes
// called by: FUN_001143a8, FUN_001179fc
undefined4 FUN_00114f5c(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  code *pcVar3;
  undefined4 uVar4;
  int local_20;
  
  uVar1 = (uint)*(byte *)(DAT_00182efc + 0x11);
  uVar4 = 1;
  if (3 < uVar1) {
    return 1;
  }
  local_20 = param_4;
  if (DAT_00183118 != (code *)0x0) {
    (*DAT_00183118)();
  }
  if (param_1 == 0) {
    if (((*(int *)(&DAT_007c6118 + uVar1 * 4) == 1) &&
        (local_20 = FUN_0010e7f0(uVar1), local_20 != 0)) &&
       (pcVar3 = (code *)(**(code **)(*(int *)(local_20 + 0xc) + 0x24))(&local_20,0xd),
       pcVar3 != (code *)0x0)) {
      if (DAT_00183114 != (code *)0x0) {
        (*DAT_00183114)();
      }
      (*pcVar3)(local_20,0);
      uVar4 = 0;
    }
    iVar2 = *(int *)(&DAT_007c6118 + uVar1 * 4) - (uint)(*(int *)(&DAT_007c6118 + uVar1 * 4) != 0);
  }
  else {
    if (param_1 != 1) {
      if (param_1 != 2) {
        return 1;
      }
      local_20 = FUN_0010e7f0(uVar1);
      if (local_20 == 0) {
        return 1;
      }
      pcVar3 = (code *)(**(code **)(*(int *)(local_20 + 0xc) + 0x24))(&local_20,0xd);
      if (pcVar3 == (code *)0x0) {
        return 1;
      }
      if (DAT_00183114 != (code *)0x0) {
        (*DAT_00183114)();
      }
      (*pcVar3)(local_20,2);
      return 0;
    }
    if (*(int *)(&DAT_007c6118 + uVar1 * 4) == 0) {
      local_20 = FUN_0010e7f0(uVar1);
      if (local_20 == 0) {
        return 1;
      }
      pcVar3 = (code *)(**(code **)(*(int *)(local_20 + 0xc) + 0x24))(&local_20,0xd);
      if (pcVar3 == (code *)0x0) {
        return 1;
      }
      (*pcVar3)(local_20,1);
      *(int *)(&DAT_007c6118 + uVar1 * 4) = *(int *)(&DAT_007c6118 + uVar1 * 4) + 1;
      return 0;
    }
    iVar2 = *(int *)(&DAT_007c6118 + uVar1 * 4) + 1;
  }
  *(int *)(&DAT_007c6118 + uVar1 * 4) = iVar2;
  return uVar4;
}

// ==== FUN_00115160 @ 00115160, 300 bytes
void FUN_00115160(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  uint in_stack_00000008;
  int in_stack_0000000c;
  int in_stack_00000010;
  
  FUN_001097e0();
  FUN_00109d78(0);
  FUN_001098ec(1);
  FUN_001098b4(1);
  FUN_00109dd8(1);
  FUN_00109854(0x10000,0,0,0x10000);
  FUN_001099f0(&DAT_01000100,in_stack_00000010 << 0x10 | 0x100);
  FUN_00109aec(0,0);
  FUN_00109cc8(3);
  FUN_00109da8(4);
  FUN_00109bb8(param_1,in_stack_00000008 << 2,0,0,
               in_stack_00000008 & 0xffff | in_stack_0000000c << 0x10);
  FUN_00109bf8(param_3,0,0,in_stack_00000008 << 2,
               in_stack_00000008 & 0xffff | in_stack_0000000c << 0x10);
  FUN_00100830(0xc0);
  iVar2 = FUN_00100ba4();
  if (iVar2 != 0) {
    FUN_00100b28(8);
    FUN_00100b98();
  }
  FUN_00100830(0x3f);
  FUN_00109e08();
  FUN_00109e78();
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 & 0xffffffdf;
  return;
}

// ==== FUN_00115274 @ 00115274, 20 bytes
// called by: FUN_00114b88, FUN_00116528, FUN_00111180, FUN_0011273c
void FUN_00115274(int param_1)

{
  *(int *)(param_1 + 4) = DAT_00182f18;
  DAT_00182f18 = param_1;
  return;
}

// ==== FUN_0011528c @ 0011528c, 296 bytes
// called by: FUN_00125ab0, FUN_0012367c, FUN_0012650c, FUN_0011a52c, FUN_00123ae8, FUN_00126468, FUN_00125944, FUN_00126404, FUN_0012637c, FUN_00126110, FUN_001264b8, FUN_0012656c
void FUN_0011528c(byte *param_1,byte param_2,uint param_3)

{
  uint uVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  
  for (; param_3 != 0 && ((uint)param_1 & 3) != 0; param_3 = param_3 - 1) {
    *param_1 = param_2;
    param_1 = param_1 + 1;
  }
  uVar1 = param_3 >> 2;
  if (uVar1 != 0) {
    iVar2 = (uint)param_2 * 0x1010101;
    param_3 = param_3 & 3;
    if (3 < uVar1) {
      iVar4 = (int)(-uVar1 + ((uint)((int)-uVar1 >> 0x1f) >> 0x1e)) >> 2;
      iVar5 = -iVar4;
      pbVar3 = param_1;
      if (iVar4 != 0 && -1 < iVar5) {
        do {
          *(int *)pbVar3 = iVar2;
          *(int *)(pbVar3 + 4) = iVar2;
          *(int *)(pbVar3 + 8) = iVar2;
          iVar5 = iVar5 + -1;
          *(int *)(pbVar3 + 0xc) = iVar2;
          pbVar3 = pbVar3 + 0x10;
        } while (iVar5 != 0);
        param_1 = param_1 + iVar4 * -0x10;
        uVar1 = uVar1 + iVar4 * 4;
      }
    }
    if ((int)uVar1 < 1) {
      for (; uVar1 != 0; uVar1 = uVar1 - 1) {
        *(int *)param_1 = iVar2;
        param_1 = param_1 + 4;
      }
    }
    else {
      pbVar3 = param_1 + -4;
      if ((uVar1 & 1) != 0) {
        *(int *)param_1 = iVar2;
        pbVar3 = param_1;
      }
      for (iVar4 = (int)uVar1 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(int *)(pbVar3 + 4) = iVar2;
        pbVar3 = pbVar3 + 8;
        *(int *)pbVar3 = iVar2;
      }
      param_1 = param_1 + uVar1 * 4;
    }
  }
  if ((int)param_3 < 1) {
    if (param_3 != 0) {
      do {
        param_3 = param_3 - 1;
        *param_1 = param_2;
        param_1 = param_1 + 1;
      } while (param_3 != 0);
      return;
    }
    return;
  }
  pbVar3 = param_1 + -1;
  if ((param_3 & 1) != 0) {
    *param_1 = param_2;
    pbVar3 = param_1;
  }
  iVar2 = (int)param_3 >> 1;
  if (iVar2 != 0) {
    do {
      pbVar3[1] = param_2;
      iVar2 = iVar2 + -1;
      pbVar3 = pbVar3 + 2;
      *pbVar3 = param_2;
    } while (iVar2 != 0);
    return;
  }
  return;
}

// ==== FUN_001153b4 @ 001153b4, 228 bytes
// called by: FUN_00121e84
void FUN_001153b4(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  piVar2 = param_1;
  if (((uint)param_1 & 2) != 0) {
    param_3 = param_3 - 1;
    piVar2 = (int *)((int)param_1 + 2);
    *(short *)param_1 = (short)param_2;
  }
  uVar4 = param_3 >> 1;
  if (uVar4 != 0) {
    iVar1 = param_2 * 0x10001;
    piVar3 = piVar2;
    if (3 < uVar4) {
      *piVar2 = iVar1;
      piVar2[1] = iVar1;
      uVar4 = uVar4 - 4;
      piVar3 = piVar2 + 4;
      piVar2[2] = iVar1;
      piVar2[3] = iVar1;
      if (3 < (int)uVar4) {
        iVar5 = (int)(-uVar4 + ((uint)((int)-uVar4 >> 0x1f) >> 0x1e)) >> 2;
        iVar6 = -iVar5;
        piVar2 = piVar3;
        if (iVar5 != 0 && -1 < iVar6) {
          do {
            *piVar2 = iVar1;
            piVar2[1] = iVar1;
            piVar2[2] = iVar1;
            iVar6 = iVar6 + -1;
            piVar2[3] = iVar1;
            piVar2 = piVar2 + 4;
          } while (iVar6 != 0);
          piVar3 = piVar3 + iVar5 * -4;
          uVar4 = uVar4 + iVar5 * 4;
        }
      }
    }
    piVar2 = piVar3;
    if ((int)uVar4 < 1) {
      for (; uVar4 != 0; uVar4 = uVar4 - 1) {
        *piVar2 = iVar1;
        piVar2 = piVar2 + 1;
      }
    }
    else {
      piVar2 = piVar3 + -1;
      if ((uVar4 & 1) != 0) {
        *piVar3 = iVar1;
        piVar2 = piVar3;
      }
      for (iVar5 = (int)uVar4 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
        piVar2[1] = iVar1;
        piVar2 = piVar2 + 2;
        *piVar2 = iVar1;
      }
      piVar2 = piVar3 + uVar4;
    }
  }
  if ((param_3 & 1) != 0) {
    *(short *)piVar2 = (short)param_2;
  }
  return;
}

// ==== FUN_00115498 @ 00115498, 116 bytes
// called by: FUN_0012a298
void FUN_00115498(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  
  FUN_0010c9b8(0x80537fe0,param_1,param_2);
  DAT_b100805c = 0x12;
  DAT_b1008700 = 0x80537fe0;
  DAT_b1008060 = 0x1b8;
  uVar1 = DAT_b100805c;
  DAT_b100805c = uVar1 | 0x20;
  do {
    uVar1 = (uint)(byte)DAT_0066aa80;
    if (uVar1 == 0) {
      return;
    }
    if (DAT_00182aa8 != 0) {
      uVar1 = DAT_b100805c;
    }
  } while (DAT_00182aa8 != 0 && (uVar1 & 0x20) != 0);
  return;
}

// ==== FUN_0011551c @ 0011551c, 116 bytes
// called by: FUN_0012a298
void FUN_0011551c(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  
  FUN_0010c9b8(0x805381a0,param_1,param_2);
  DAT_b100805c = 0x14;
  DAT_b1008700 = 0x805381a0;
  DAT_b1008060 = 0x10;
  uVar1 = DAT_b100805c;
  DAT_b100805c = uVar1 | 0x20;
  do {
    uVar1 = (uint)(byte)DAT_0066aa80;
    if (uVar1 == 0) {
      return;
    }
    if (DAT_00182aa8 != 0) {
      uVar1 = DAT_b100805c;
    }
  } while (DAT_00182aa8 != 0 && (uVar1 & 0x20) != 0);
  return;
}

// ==== FUN_001155b0 @ 001155b0, 96 bytes
uint FUN_001155b0(uint param_1,uint param_2,int param_3)

{
  uint uVar1;
  
  if (param_2 >> 0x18 == 0) {
    if (param_1 >> 0x18 == 0xff) {
      return param_1 & 0xffffff | param_3 * 0x11000000 | DAT_00183088;
    }
    if (param_1 >> 0x18 == 0) {
      return param_2 | DAT_00183088;
    }
  }
  uVar1 = (*DAT_00183040)(param_1,param_2,param_3 * 0x11 & 0xff);
  return uVar1 | DAT_00183088;
}

// ==== FUN_00115618 @ 00115618, 20 bytes
// called by: FUN_00113ef4
undefined4 FUN_00115618(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = DAT_00183088;
  DAT_00183088 = param_1;
  return uVar1;
}

// ==== FUN_00115630 @ 00115630, 32 bytes
// called by: FUN_00116e10, FUN_00116bbc, FUN_001162e0, FUN_00116254
void FUN_00115630(void)

{
                    /* WARNING: Could not emulate address calculation at 0x00115640 */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)**(undefined4 **)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10))();
  return;
}

// ==== FUN_00115678 @ 00115678, 988 bytes
// called by: FUN_0010de9c, FUN_0011b2e8, FUN_001265c8, FUN_001119fc, FUN_0010d8f0
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00115678(int param_1,int param_2,int param_3,int param_4,uint param_5,uint param_6,
                 uint param_7,int param_8,byte *param_9,int param_10)

{
  uint uVar1;
  int iVar2;
  byte *pbVar3;
  uint *puVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  code *pcVar10;
  int local_64;
  int local_48;
  byte *pbStack_3c;
  int local_34;
  int local_2c;
  int local_28;
  
  if (DAT_00182f58 == (undefined4 *)0x0) {
    if ((param_6 | param_5) != 1) {
      local_48 = 0;
      local_64 = param_2;
      if (param_4 < 1) {
        return;
      }
      do {
        iVar2 = local_64 + param_6 + -1;
        if ((*(short *)(DAT_00182efc + 10) <= iVar2) && (local_64 <= *(short *)(DAT_00182efc + 0xe))
           ) {
          iVar7 = 0;
          pbStack_3c = param_9;
          iVar5 = param_1;
          if (0 < param_3) {
            do {
              iVar8 = 8;
              pbVar3 = pbStack_3c + 1;
              uVar6 = (uint)*pbStack_3c;
              do {
                iVar8 = iVar8 - param_7;
                uVar1 = uVar6 >> (8 - param_7 & 0xff);
                uVar6 = uVar6 << (param_7 & 0xff) & 0xff;
                if ((uVar1 != 0) || ((*(byte *)(DAT_00182efc + 0x10) & 2) == 0)) {
                  puVar4 = *(uint **)(DAT_00182efc + 0x3c);
                  uVar9 = *puVar4;
                  if (param_10 == 0) {
                    *puVar4 = uVar1;
                  }
                  else {
                    *puVar4 = *(uint *)(param_10 + uVar1 * 4);
                  }
                  FUN_00115b98(iVar5,local_64,iVar5 + param_5 + -1,iVar2);
                  **(uint **)(DAT_00182efc + 0x3c) = uVar9;
                }
                iVar7 = iVar7 + 1;
                iVar5 = iVar5 + param_5;
                if (param_3 <= iVar7) goto LAB_00115a18;
                pbStack_3c = pbVar3;
              } while (iVar8 != 0);
            } while( true );
          }
        }
LAB_00115a18:
        local_48 = local_48 + 1;
        param_9 = param_9 + param_8;
        local_64 = local_64 + param_6;
        if (param_4 <= local_48) {
          return;
        }
      } while( true );
    }
    iVar5 = *(short *)(DAT_00182efc + 10) - param_2;
    iVar2 = param_2;
    local_28 = param_4;
    if ((0 < iVar5) &&
       (local_28 = param_4 - iVar5, iVar2 = (int)*(short *)(DAT_00182efc + 10), local_28 < 1)) {
      return;
    }
    iVar5 = (param_2 + param_4 + -1) - (int)*(short *)(DAT_00182efc + 0xe);
    if ((0 < iVar5) && (local_28 - iVar5 < 1)) {
      return;
    }
    iVar5 = (param_1 + param_3 + -1) - (int)*(short *)(DAT_00182efc + 0xc);
    local_2c = param_3;
    if (0 < iVar5) {
      local_2c = param_3 - iVar5;
    }
    local_34 = param_1;
    if (param_1 < *(short *)(DAT_00182efc + 8)) {
      uVar6 = *(short *)(DAT_00182efc + 8) - param_1;
      local_2c = local_2c - uVar6;
      if (param_7 == 8) {
        local_34 = uVar6 + param_1;
      }
      else if ((int)param_7 < 9) {
        if (param_7 == 1) {
          local_34 = param_1 + (uVar6 & 0xfffffff8);
        }
        else if (param_7 == 2) {
          local_34 = param_1 + (uVar6 & 0xfffffffc);
        }
        else if (param_7 == 4) {
          local_34 = param_1 + (uVar6 & 0xfffffffe);
        }
      }
      else if (param_7 == 0x10) {
        local_34 = uVar6 + param_1;
      }
      else if (param_7 == 0x18 || param_7 == 0x20) {
        local_34 = uVar6 + param_1;
      }
    }
    if (local_2c < 1) {
      return;
    }
    param_1 = (&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)];
    pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 4);
    param_2 = local_34;
    param_4 = local_2c;
  }
  else {
    pcVar10 = (code *)*DAT_00182f58;
    iVar2 = param_3;
  }
  (*pcVar10)(param_1,param_2,iVar2,param_4);
  return;
}

// ==== FUN_00115b98 @ 00115b98, 160 bytes
// called by: FUN_0010e41c, FUN_00115678, FUN_0010dce4, FUN_001163d4, FUN_0010eaec, FUN_00116f78, FUN_0010d8f0
void FUN_00115b98(int param_1,int param_2,int param_3,int param_4)

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
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x10))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,param_3,param_4);
  return;
}

// ==== FUN_00115c40 @ 00115c40, 72 bytes
// called by: FUN_0011b2e8
undefined4 FUN_00115c40(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(byte *)(DAT_00182efc + 0x11) < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,8);
  }
  return uVar2;
}

// ==== FUN_00115c8c @ 00115c8c, 60 bytes
// called by: FUN_00116580, display_set_rotation
undefined4 FUN_00115c8c(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,8);
  }
  return uVar2;
}

// ==== FUN_00115cc8 @ 00115cc8, 20 bytes
// called by: FUN_0010eb80
undefined4 FUN_00115cc8(void)

{
  return **(undefined4 **)(DAT_00182efc + 0x38);
}

// ==== FUN_00115ce0 @ 00115ce0, 20 bytes
// called by: FUN_0011b2e8, FUN_0010ebc4
undefined4 FUN_00115ce0(void)

{
  return **(undefined4 **)(DAT_00182efc + 0x3c);
}

// ==== FUN_00115cf8 @ 00115cf8, 60 bytes
// called by: FUN_00125bb0
undefined4 FUN_00115cf8(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,0xc);
  }
  return uVar2;
}

// ==== FUN_00115d34 @ 00115d34, 60 bytes
// called by: FUN_00125bb0
undefined4 FUN_00115d34(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,0xd);
  }
  return uVar2;
}

// ==== FUN_00115d70 @ 00115d70, 72 bytes
// called by: FUN_0012332c, FUN_00123aac, FUN_0011a428
undefined4 FUN_00115d70(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(byte *)(DAT_00182efc + 0x11) < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,9);
  }
  return uVar2;
}

// ==== FUN_00115dbc @ 00115dbc, 60 bytes
// called by: FUN_00116580
undefined4 FUN_00115dbc(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,9);
  }
  return uVar2;
}

// ==== FUN_00115df8 @ 00115df8, 48 bytes
// called by: FUN_00116580
int FUN_00115df8(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = 0;
  do {
    iVar1 = FUN_0010e7f0(iVar2);
    if (iVar1 != 0) {
      iVar3 = iVar3 + 1;
    }
    iVar2 = iVar2 + 1;
  } while (iVar2 < 4);
  return iVar3;
}

// ==== FUN_00115e28 @ 00115e28, 48 bytes
// called by: FUN_00123aac, FUN_0011a428
void FUN_00115e28(void)

{
  int iVar1;
  
  iVar1 = FUN_0010e7f0(*(undefined1 *)(DAT_00182efc + 0x11));
  if (iVar1 == 0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00115e54. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(iVar1 + 0xc) + 0x2c))(iVar1,2);
  return;
}

// ==== FUN_00115e5c @ 00115e5c, 16 bytes
// called by: FUN_00116bbc
void FUN_00115e5c(void)

{
  FUN_00115e6c();
                    /* WARNING: Could not emulate address calculation at 0x00116380 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10) + 4))();
  return;
}

// ==== FUN_00115e6c @ 00115e6c, 144 bytes
// called by: FUN_00115e5c
undefined4 FUN_00115e6c(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  bool bVar4;
  short local_20;
  short local_1e;
  short local_1c;
  short local_1a;
  
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],&local_20);
  iVar2 = (int)local_20;
  bVar4 = SBORROW4(iVar2,param_1);
  iVar1 = iVar2 - param_1;
  if (iVar2 <= param_1) {
    iVar2 = (int)local_1c;
    bVar4 = SBORROW4(param_1,iVar2);
    iVar1 = param_1 - iVar2;
  }
  if (iVar2 == param_1 || iVar1 < 0 != bVar4) {
    iVar2 = (int)local_1e;
    bVar4 = SBORROW4(iVar2,param_2);
    iVar1 = iVar2 - param_2;
    if (iVar2 <= param_2) {
      iVar2 = (int)local_1a;
      bVar4 = SBORROW4(param_2,iVar2);
      iVar1 = param_2 - iVar2;
    }
    if (iVar2 == param_2 || iVar1 < 0 != bVar4) {
      uVar3 = (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x14))
                        ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2);
      return uVar3;
    }
  }
  return 0;
}

// ==== FUN_00115f04 @ 00115f04, 72 bytes
// called by: FUN_00116e40
undefined4 FUN_00115f04(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(byte *)(DAT_00182efc + 0x11) < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,0xe);
  }
  return uVar2;
}

// ==== FUN_00115f50 @ 00115f50, 60 bytes
// called by: FUN_00125bb0
undefined4 FUN_00115f50(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,0xe);
  }
  return uVar2;
}

// ==== FUN_00115f8c @ 00115f8c, 60 bytes
// called by: FUN_00110ed8, display_set_rotation
undefined4 FUN_00115f8c(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,3);
  }
  return uVar2;
}

// ==== FUN_00115fc8 @ 00115fc8, 96 bytes
// called by: FUN_00114b88
int FUN_00115fc8(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = 0;
  iVar3 = 3;
  do {
    iVar4 = 0;
    if ((iVar3 < 4) && (iVar1 = FUN_0010e7f0(iVar3), iVar1 != 0)) {
      iVar4 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,3);
    }
    if (iVar2 < iVar4) {
      iVar2 = iVar4;
    }
    iVar3 = iVar3 + -1;
  } while (-1 < iVar3);
  return iVar2;
}

// ==== FUN_00116028 @ 00116028, 60 bytes
// called by: FUN_00110ed8, display_set_rotation
undefined4 FUN_00116028(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,4);
  }
  return uVar2;
}

// ==== FUN_00116064 @ 00116064, 72 bytes
// called by: FUN_00111208, FUN_0011b2e8
undefined4 FUN_00116064(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(byte *)(DAT_00182efc + 0x11) < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,1);
  }
  return uVar2;
}

// ==== FUN_001160b0 @ 001160b0, 12 bytes
// called by: FUN_0010ece4, FUN_00126110
undefined4 FUN_001160b0(void)

{
  return DAT_00182fc4;
}

// ==== FUN_001160c0 @ 001160c0, 60 bytes
// called by: FUN_00110ed8, FUN_001139a0, display_set_rotation
undefined4 FUN_001160c0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,1);
  }
  return uVar2;
}

// ==== FUN_001160fc @ 001160fc, 96 bytes
// called by: FUN_0010ece4
int FUN_001160fc(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = 0;
  iVar3 = 3;
  do {
    iVar4 = 0;
    if ((iVar3 < 4) && (iVar1 = FUN_0010e7f0(iVar3), iVar1 != 0)) {
      iVar4 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,1);
    }
    if (iVar2 < iVar4) {
      iVar2 = iVar4;
    }
    iVar3 = iVar3 + -1;
  } while (-1 < iVar3);
  return iVar2;
}

// ==== FUN_0011615c @ 0011615c, 72 bytes
// called by: FUN_00111208, FUN_0011b2e8
undefined4 FUN_0011615c(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((*(byte *)(DAT_00182efc + 0x11) < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,2);
  }
  return uVar2;
}

// ==== FUN_001161a8 @ 001161a8, 12 bytes
// called by: FUN_0010ece4, FUN_00126110
undefined4 FUN_001161a8(void)

{
  return DAT_00182fc8;
}

// ==== FUN_001161b8 @ 001161b8, 60 bytes
// called by: FUN_00110ed8, FUN_001139a0, display_set_rotation
undefined4 FUN_001161b8(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if ((param_1 < 4) && (iVar1 = FUN_0010e7f0(), iVar1 != 0)) {
    uVar2 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,2);
  }
  return uVar2;
}

// ==== FUN_001161f4 @ 001161f4, 96 bytes
// called by: FUN_0010ece4
int FUN_001161f4(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = 0;
  iVar3 = 3;
  do {
    iVar4 = 0;
    if ((iVar3 < 4) && (iVar1 = FUN_0010e7f0(iVar3), iVar1 != 0)) {
      iVar4 = (**(code **)(*(int *)(iVar1 + 0xc) + 0x28))(iVar1,2);
    }
    if (iVar2 < iVar4) {
      iVar2 = iVar4;
    }
    iVar3 = iVar3 + -1;
  } while (-1 < iVar3);
  return iVar2;
}

// ==== FUN_00116254 @ 00116254, 136 bytes
int FUN_00116254(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int unaff_r7;
  
  if (param_1 != (int *)0x0) {
    unaff_r7 = param_1[2];
  }
  if (param_1 == (int *)0x0 || unaff_r7 == 0) {
    return 0;
  }
  if (DAT_001830a8 == 0) {
    return 0;
  }
  iVar2 = *param_1;
  if (iVar2 < 0x101) {
    if (DAT_001830a0 < iVar2) {
      iVar2 = DAT_001830a0;
    }
    iVar3 = 0;
    if (0 < iVar2) {
      do {
        uVar1 = FUN_00115630(*(undefined4 *)(unaff_r7 + iVar3 * 4));
        *(undefined4 *)(DAT_001830a8 + iVar3 * 4) = uVar1;
        iVar3 = iVar3 + 1;
      } while (iVar3 < iVar2);
    }
    if (DAT_001830a4 != (code *)0x0) {
      (*DAT_001830a4)(param_1);
    }
    return DAT_001830a8;
  }
  return 0;
}

// ==== FUN_001162e0 @ 001162e0, 140 bytes
// called by: FUN_0010de9c
int FUN_001162e0(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int unaff_r6;
  
  if (param_1 != (int *)0x0) {
    unaff_r6 = param_1[2];
  }
  if (param_1 != (int *)0x0 && unaff_r6 != 0) {
    if (DAT_001830a8 == 0) {
      return 0;
    }
    iVar2 = *param_1;
    if (iVar2 < 0x101) {
      if (DAT_001830a0 < iVar2) {
        iVar2 = DAT_001830a0;
      }
      iVar3 = 0;
      if (0 < iVar2) {
        do {
          uVar1 = FUN_00115630(*(undefined4 *)(unaff_r6 + iVar3 * 4));
          *(undefined4 *)(DAT_001830a8 + iVar3 * 4) = uVar1;
          iVar3 = iVar3 + 1;
        } while (iVar3 < iVar2);
      }
      if (DAT_001830a4 != (code *)0x0) {
        (*DAT_001830a4)(param_1);
      }
      return DAT_001830a8;
    }
  }
  return 0;
}

// ==== FUN_00116370 @ 00116370, 32 bytes
// called by: FUN_00116580, FUN_0010de9c, FUN_00116bbc, FUN_0011b2e8
void FUN_00116370(void)

{
                    /* WARNING: Could not emulate address calculation at 0x00116380 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10) + 4))();
  return;
}

// ==== FUN_001163d4 @ 001163d4, 316 bytes
// called by: FUN_0010ece4
uint FUN_001163d4(void)

{
  uint uVar1;
  code *pcVar2;
  uint uVar3;
  int in_r3;
  int iVar4;
  int local_18;
  
  local_18 = in_r3;
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],DAT_00182efc + 8);
  uVar1 = FUN_00116528();
  if (uVar1 == 0) {
    iVar4 = 0;
    do {
      local_18 = FUN_0010e7f0(iVar4);
      if (local_18 != 0) {
        pcVar2 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,0xc);
        if (pcVar2 == (code *)0x0) {
          return 1;
        }
        uVar3 = (*pcVar2)(local_18);
        uVar1 = uVar1 | uVar3;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < 4);
    if ((uVar1 == 0) && (FUN_00116580(), DAT_00182ef8 != 0x12345678)) {
      iVar4 = 3;
      do {
        local_18 = FUN_0010e7f0(iVar4);
        if (local_18 != 0) {
          FUN_00112bb8(iVar4);
          (**(code **)(DAT_00183084 + 8))(4);
          FUN_00115b98(0,0,0x3fff,0x3fff);
          (**(code **)(DAT_00183084 + 8))(0);
          pcVar2 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,0xe);
          if (pcVar2 != (code *)0x0) {
            (*pcVar2)(local_18);
          }
        }
        iVar4 = iVar4 + -1;
      } while (-1 < iVar4);
    }
  }
  return uVar1;
}

// ==== FUN_00116528 @ 00116528, 76 bytes
// called by: FUN_001163d4
undefined4 FUN_00116528(void)

{
  if (DAT_001830a8 == 0) {
    DAT_001830a8 = FUN_0010e12c(DAT_001830a0 << 2);
    if (DAT_001830a8 == 0) {
      return 1;
    }
    DAT_001830ac = FUN_00124d80;
    FUN_00115274(&DAT_001830ac);
  }
  return 0;
}

// ==== FUN_00116580 @ 00116580, 172 bytes
// called by: FUN_001163d4
void FUN_00116580(void)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  
  iVar2 = FUN_00115df8();
  iVar6 = 0;
  if (0 < iVar2) {
    do {
      iVar3 = FUN_0010e7f0(iVar6);
      if (iVar3 != 0) {
        uVar1 = *(undefined1 *)(DAT_00182efc + 0x11);
        *(char *)(DAT_00182efc + 0x11) = (char)iVar6;
        iVar3 = FUN_00115c8c(iVar6);
        if ((iVar3 < 9) && (*(undefined ***)((&DAT_007bd120)[iVar6] + 0x10) != &PTR_FUN_0018238c)) {
          iVar3 = FUN_00115dbc(iVar6);
          uVar5 = 0;
          if (0 < iVar3) {
            do {
              uVar4 = FUN_00116370(uVar5 & 0xff);
              FUN_00116b5c(iVar6,uVar5 & 0xff,uVar4);
              uVar5 = uVar5 + 1;
            } while ((int)uVar5 < iVar3);
          }
        }
        *(undefined1 *)(DAT_00182efc + 0x11) = uVar1;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < iVar2);
    return;
  }
  return;
}

// ==== FUN_00116638 @ 00116638, 788 bytes
// called by: FUN_00112a74
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00116638(int param_1,int param_2,int param_3,int param_4,byte *param_5,int param_6)

{
  uint uVar1;
  byte bVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  code *pcVar9;
  byte *pbVar10;
  uint uVar11;
  code *pcVar12;
  int iVar13;
  
  pcVar12 = *(code **)(*(int *)(param_6 + 0xc) + 0x14);
  pcVar9 = *(code **)(*(int *)(param_6 + 0xc) + 0x24);
  if ((pcVar9 != (code *)0x0) && (pcVar9 = (code *)(*pcVar9)(&param_6,1), pcVar9 != (code *)0x0)) {
    (*pcVar9)(param_6,param_1,param_2,param_3);
    return;
  }
  iVar4 = FUN_0010eb64(*(undefined1 *)(DAT_00182efc + 0x11));
  if (iVar4 < 9) {
    iVar13 = 1;
  }
  else if (iVar4 < 0x11) {
    iVar13 = 2;
  }
  else {
    iVar13 = 4;
  }
  uVar11 = 0;
  if (param_4 < param_2) {
    return;
  }
  do {
    if (iVar13 == 1) {
      do {
        switch(iVar4) {
        default:
          param_2 = param_2 + 1;
          if (param_4 < param_2) {
            return;
          }
          break;
        case 1:
          do {
            while (pbVar10 = param_5, iVar4 = param_1, param_3 < param_1) {
              param_2 = param_2 + 1;
              if (param_4 < param_2) {
                return;
              }
            }
            do {
              iVar13 = (*pcVar12)(param_6,iVar4,param_2);
              uVar8 = 7 - uVar11;
              uVar1 = uVar11 & 0xff;
              uVar5 = uVar11 + 1;
              iVar4 = iVar4 + 1;
              uVar11 = (int)uVar5 >> (uVar5 & 0xf8);
              param_5 = pbVar10 + ((int)uVar5 >> 3);
              *pbVar10 = (byte)(iVar13 << (uVar8 & 0xff)) | *pbVar10 & (byte)(0xff7f >> uVar1);
              pbVar10 = param_5;
            } while (iVar4 <= param_3);
            param_2 = param_2 + 1;
          } while (param_2 <= param_4);
          return;
        case 2:
          do {
            while (pbVar10 = param_5, iVar4 = param_1, param_3 < param_1) {
              param_2 = param_2 + 1;
              if (param_4 < param_2) {
                return;
              }
            }
            do {
              iVar6 = (*pcVar12)(param_6,iVar4,param_2);
              iVar13 = uVar11 * -2;
              uVar1 = uVar11 & 0x7f;
              uVar5 = uVar11 + 1;
              iVar4 = iVar4 + 1;
              uVar11 = (int)uVar5 >> (uVar5 & 0xfc);
              param_5 = pbVar10 + ((int)uVar5 >> 2);
              *pbVar10 = (byte)(iVar6 << (iVar13 + 6U & 0xff)) |
                         *pbVar10 & (byte)(0xff3f >> (uVar1 << 1));
              pbVar10 = param_5;
            } while (iVar4 <= param_3);
            param_2 = param_2 + 1;
          } while (param_2 <= param_4);
          return;
        case 3:
        case 4:
          pbVar10 = param_5;
          iVar13 = param_1;
          if (param_3 < param_1) {
            param_2 = param_2 + 1;
            if (param_4 < param_2) {
              return;
            }
          }
          else {
            do {
              iVar6 = (*pcVar12)(param_6,iVar13,param_2);
              iVar13 = iVar13 + 1;
              param_5 = pbVar10 + uVar11;
              *pbVar10 = *pbVar10 & (byte)(0xf << (uVar11 << 2)) |
                         (byte)(iVar6 << (uVar11 * -4 + 4 & 0xff));
              uVar11 = uVar11 ^ 1;
              pbVar10 = param_5;
            } while (iVar13 <= param_3);
            param_2 = param_2 + 1;
            if (param_4 < param_2) {
              return;
            }
          }
          break;
        case 5:
        case 6:
        case 7:
        case 8:
          pbVar10 = param_5;
          iVar13 = param_1;
          if (param_3 < param_1) {
            param_2 = param_2 + 1;
            if (param_4 < param_2) {
              return;
            }
          }
          else {
            do {
              bVar2 = (*pcVar12)(param_6,iVar13,param_2);
              iVar13 = iVar13 + 1;
              param_5 = pbVar10 + 1;
              *pbVar10 = bVar2;
              pbVar10 = param_5;
            } while (iVar13 <= param_3);
            param_2 = param_2 + 1;
            if (param_4 < param_2) {
              return;
            }
          }
        }
      } while( true );
    }
    iVar6 = param_1;
    if (iVar13 == 2) {
      for (; iVar6 <= param_3; iVar6 = iVar6 + 1) {
        uVar3 = (*pcVar12)(param_6,iVar6,param_2);
        *(undefined2 *)param_5 = uVar3;
        param_5 = param_5 + 2;
      }
    }
    else if (iVar13 == 4) {
      for (; iVar6 <= param_3; iVar6 = iVar6 + 1) {
        uVar7 = (*pcVar12)(param_6,iVar6,param_2);
        *(undefined4 *)param_5 = uVar7;
        param_5 = param_5 + 4;
      }
    }
    param_2 = param_2 + 1;
  } while (param_2 <= param_4);
  return;
}

// ==== FUN_0011697c @ 0011697c, 108 bytes
// called by: FUN_00112ba4
void FUN_0011697c(void)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)(&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)];
  do {
    if (piVar2 == (int *)0x0) {
LAB_001169cc:
      iVar1 = DAT_00182efc;
      *(int *)(DAT_00182efc + 0x14) = DAT_00182efc + 8;
      *(undefined4 *)(iVar1 + 100) = 0;
                    /* WARNING: Could not emulate address calculation at 0x00116a90 */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
                ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],DAT_00182efc + 8);
      return;
    }
    if (((int *)piVar2[3] != (int *)0x0) && (*(int *)piVar2[3] == 4)) {
      if (piVar2 != (int *)0x0) {
        FUN_0010e754();
      }
      goto LAB_001169cc;
    }
    piVar2 = (int *)*piVar2;
  } while( true );
}

// ==== FUN_001169f0 @ 001169f0, 16 bytes
// called by: FUN_0010d884, FUN_0010ece4
void FUN_001169f0(void)

{
                    /* WARNING: Could not recover jumptable at 0x001169fc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_00183084 + 4))();
  return;
}

// ==== FUN_00116a28 @ 00116a28, 76 bytes
// called by: FUN_001186b4
void FUN_00116a28(undefined4 param_1)

{
  undefined1 auStack_18 [12];
  
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],auStack_18);
  FUN_00114ef0(DAT_00182efc + 8,param_1,auStack_18);
  return;
}

// ==== FUN_00116a7c @ 00116a7c, 36 bytes
// called by: FUN_00110c20, FUN_001114e0, FUN_00118548, FUN_0011147c
void FUN_00116a7c(void)

{
                    /* WARNING: Could not emulate address calculation at 0x00116a90 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],DAT_00182efc + 8);
  return;
}

// ==== FUN_00116aa8 @ 00116aa8, 16 bytes
// called by: FUN_0010d884, FUN_0010ece4, FUN_0011b2e8
void FUN_00116aa8(void)

{
                    /* WARNING: Could not recover jumptable at 0x00116ab4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*DAT_00183084)();
  return;
}

// ==== FUN_00116abc @ 00116abc, 32 bytes
// called by: FUN_00116e10
void FUN_00116abc(undefined4 param_1)

{
  undefined4 *puVar1;
  
  if ((*(byte *)(DAT_00182efc + 0x10) & 4) == 0) {
    puVar1 = *(undefined4 **)(DAT_00182efc + 0x3c);
  }
  else {
    puVar1 = *(undefined4 **)(DAT_00182efc + 0x38);
  }
  *puVar1 = param_1;
  return;
}

// ==== FUN_00116ae0 @ 00116ae0, 88 bytes
// called by: FUN_00116e40
undefined4 FUN_00116ae0(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  undefined4 uVar2;
  int local_18;
  
  uVar2 = 1;
  local_18 = param_4;
  local_18 = FUN_0010e7f0();
  if ((local_18 != 0) &&
     (pcVar1 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,0x16),
     pcVar1 != (code *)0x0)) {
    (*pcVar1)(local_18,param_2,param_3);
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00116b38 @ 00116b38, 12 bytes
// called by: FUN_0010ece4
void FUN_00116b38(undefined4 param_1,undefined4 param_2)

{
  DAT_00182fc4 = param_1;
  DAT_00182fc8 = param_2;
  return;
}

// ==== FUN_00116b48 @ 00116b48, 16 bytes
// called by: FUN_0010e41c, FUN_0010d884, FUN_0010db64, FUN_0010d8f0
void FUN_00116b48(void)

{
                    /* WARNING: Could not recover jumptable at 0x00116b54. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_00183084 + 8))();
  return;
}

// ==== FUN_00116b5c @ 00116b5c, 88 bytes
// called by: FUN_00116580
undefined4 FUN_00116b5c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  undefined4 uVar2;
  int local_18;
  
  uVar2 = 1;
  local_18 = param_4;
  local_18 = FUN_0010e7f0();
  if ((local_18 != 0) &&
     (pcVar1 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,0x10),
     pcVar1 != (code *)0x0)) {
    (*pcVar1)(local_18,param_2,param_3);
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00116bb4 @ 00116bb4, 8 bytes
// called by: FUN_00121194
void FUN_00116bb4(int param_1,int param_2,uint param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  bool bVar5;
  
  if (param_3 == 0) {
    return;
  }
  iVar4 = (int)*(short *)(DAT_00182efc + 10);
  bVar5 = SBORROW4(iVar4,param_2);
  iVar1 = iVar4 - param_2;
  if (iVar4 <= param_2) {
    iVar4 = (int)*(short *)(DAT_00182efc + 0xe);
    bVar5 = SBORROW4(param_2,iVar4);
    iVar1 = param_2 - iVar4;
  }
  if (iVar4 == param_2 || iVar1 < 0 != bVar5) {
    iVar4 = (int)*(short *)(DAT_00182efc + 8);
    bVar5 = SBORROW4(iVar4,param_1);
    iVar1 = iVar4 - param_1;
    if (iVar4 <= param_1) {
      iVar4 = (int)*(short *)(DAT_00182efc + 0xc);
      bVar5 = SBORROW4(param_1,iVar4);
      iVar1 = param_1 - iVar4;
    }
    if (iVar4 == param_1 || iVar1 < 0 != bVar5) {
      if (param_3 < 0xf) {
        uVar2 = FUN_00116370(**(undefined4 **)(DAT_00182efc + 0x3c));
        uVar3 = FUN_00115e5c(param_1,param_2);
        (*DAT_0018308c)(uVar2,uVar3,param_3);
        uVar2 = FUN_00115630();
                    /* WARNING: Could not emulate address calculation at 0x00116c78 */
                    /* WARNING: Treating indirect jump as call */
        (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x18))
                  ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,uVar2);
        return;
      }
                    /* WARNING: Could not emulate address calculation at 0x00116c18 */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x18))
                ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,
                 **(undefined4 **)(DAT_00182efc + 0x3c));
      return;
    }
    return;
  }
  return;
}

// ==== FUN_00116bbc @ 00116bbc, 212 bytes
void FUN_00116bbc(int param_1,int param_2,uint param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  bool bVar5;
  
  iVar4 = (int)*(short *)(DAT_00182efc + 10);
  bVar5 = SBORROW4(iVar4,param_2);
  iVar1 = iVar4 - param_2;
  if (iVar4 <= param_2) {
    iVar4 = (int)*(short *)(DAT_00182efc + 0xe);
    bVar5 = SBORROW4(param_2,iVar4);
    iVar1 = param_2 - iVar4;
  }
  if (iVar4 != param_2 && iVar1 < 0 == bVar5) {
    return;
  }
  iVar4 = (int)*(short *)(DAT_00182efc + 8);
  bVar5 = SBORROW4(iVar4,param_1);
  iVar1 = iVar4 - param_1;
  if (iVar4 <= param_1) {
    iVar4 = (int)*(short *)(DAT_00182efc + 0xc);
    bVar5 = SBORROW4(param_1,iVar4);
    iVar1 = param_1 - iVar4;
  }
  if (iVar4 == param_1 || iVar1 < 0 != bVar5) {
    if (param_3 < 0xf) {
      uVar2 = FUN_00116370(**(undefined4 **)(DAT_00182efc + 0x3c));
      uVar3 = FUN_00115e5c(param_1,param_2);
      (*DAT_0018308c)(uVar2,uVar3,param_3);
      uVar2 = FUN_00115630();
                    /* WARNING: Could not emulate address calculation at 0x00116c78 */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x18))
                ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,uVar2);
      return;
    }
                    /* WARNING: Could not emulate address calculation at 0x00116c18 */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x18))
              ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,
               **(undefined4 **)(DAT_00182efc + 0x3c));
    return;
  }
  return;
}

// ==== FUN_00116c9c @ 00116c9c, 108 bytes
// called by: FUN_0011b2e8
void FUN_00116c9c(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  iVar2 = (int)*(short *)(DAT_00182efc + 8);
  bVar3 = SBORROW4(iVar2,param_1);
  iVar1 = iVar2 - param_1;
  if (iVar2 <= param_1) {
    iVar2 = (int)*(short *)(DAT_00182efc + 0xc);
    bVar3 = SBORROW4(param_1,iVar2);
    iVar1 = param_1 - iVar2;
  }
  if (iVar2 != param_1 && iVar1 < 0 == bVar3) {
    return;
  }
  iVar2 = (int)*(short *)(DAT_00182efc + 10);
  bVar3 = SBORROW4(iVar2,param_2);
  iVar1 = iVar2 - param_2;
  if (iVar2 <= param_2) {
    iVar2 = (int)*(short *)(DAT_00182efc + 0xe);
    bVar3 = SBORROW4(param_2,iVar2);
    iVar1 = param_2 - iVar2;
  }
  if (iVar2 == param_2 || iVar1 < 0 != bVar3) {
                    /* WARNING: Could not emulate address calculation at 0x00116ce8 */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x18))
              ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,param_2,param_3);
    return;
  }
  return;
}

// ==== FUN_00116d10 @ 00116d10, 88 bytes
// called by: FUN_00116e40
undefined4 FUN_00116d10(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  undefined4 uVar2;
  int local_18;
  
  uVar2 = 1;
  local_18 = param_4;
  local_18 = FUN_0010e7f0();
  if ((local_18 != 0) &&
     (pcVar1 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,0xb),
     pcVar1 != (code *)0x0)) {
    (*pcVar1)(local_18,param_2,param_3);
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00116d68 @ 00116d68, 80 bytes
// called by: FUN_00116e40
undefined4 FUN_00116d68(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  undefined4 uVar2;
  int local_10;
  
  uVar2 = 1;
  local_10 = param_4;
  local_10 = FUN_0010e7f0();
  if ((local_10 != 0) &&
     (pcVar1 = (code *)(**(code **)(*(int *)(local_10 + 0xc) + 0x24))(&local_10,9),
     pcVar1 != (code *)0x0)) {
    (*pcVar1)(local_10,param_2);
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00116db8 @ 00116db8, 88 bytes
// called by: FUN_00116e40
undefined4 FUN_00116db8(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  undefined4 uVar2;
  int local_18;
  
  uVar2 = 1;
  local_18 = param_4;
  local_18 = FUN_0010e7f0();
  if ((local_18 != 0) &&
     (pcVar1 = (code *)(**(code **)(*(int *)(local_18 + 0xc) + 0x24))(&local_18,10),
     pcVar1 != (code *)0x0)) {
    (*pcVar1)(local_18,param_2,param_3);
    uVar2 = 0;
  }
  return uVar2;
}

// ==== FUN_00116e10 @ 00116e10, 76 bytes
// called by: FUN_00110c20, FUN_00112ba4, FUN_0011147c
void FUN_00116e10(void)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  FUN_00115630(*(undefined4 *)(DAT_00182efc + 0x30));
  FUN_00116abc();
  uVar1 = FUN_00115630(*(undefined4 *)(DAT_00182efc + 0x34));
  if ((*(byte *)(DAT_00182efc + 0x10) & 4) == 0) {
    puVar2 = *(undefined4 **)(DAT_00182efc + 0x38);
  }
  else {
    puVar2 = *(undefined4 **)(DAT_00182efc + 0x3c);
  }
  *puVar2 = uVar1;
  return;
}

// ==== FUN_00116e40 @ 00116e40, 204 bytes
// called by: FUN_0010ece4
void FUN_00116e40(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uStack_14;
  
  DAT_001823f4 = 0xffffffff;
  uStack_14 = param_4;
  FUN_00106f58();
  FUN_001093c0(2,FUN_001173b0,&uStack_14);
  FUN_001091f4(2);
  FUN_0011273c(2);
  FUN_0010e56c(&DAT_0018166c,&PTR_LAB_00180238,0,0,param_3);
  FUN_00115f04();
  FUN_00116d10(0,0x1e0);
  FUN_00116db8(0,0x1e0);
  DAT_001823f0 = DAT_00182938;
  FUN_00116ae0(0,0x21,&LAB_00115658);
  FUN_00116d68(0,DAT_00182938);
  FUN_001139a0(0,0,0x1e0,0,0x1e0);
  FUN_001139a0(1,0,0x1e0,0,0x1e0);
  return;
}

// ==== FUN_00116f24 @ 00116f24, 80 bytes
// called by: FUN_00125bb0, FUN_0012650c, FUN_00123ae8, FUN_00125944, FUN_00126404, FUN_00125a18, FUN_0012637c, FUN_00126110, FUN_00125a3c, FUN_001259a4, FUN_0012656c
undefined4 FUN_00116f24(undefined4 param_1,int param_2,undefined4 *param_3)

{
  if (param_2 != 4) {
    if (param_2 < 5) {
      if ((param_2 != 1 && param_2 != 2) && param_2 != 3) {
        return 0xffffffff;
      }
    }
    else if (param_2 != 5 && param_2 != 6) {
      if (param_2 != 0xe) {
        return 0xffffffff;
      }
      DAT_001823f4 = *param_3;
    }
  }
  return 0;
}

// ==== FUN_00116f78 @ 00116f78, 776 bytes
// called by: FUN_001119fc
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00116f78(int param_1,int param_2,int param_3,int param_4,uint param_5,uint param_6,
                 uint param_7,int param_8,byte *param_9,int param_10,int param_11)

{
  uint uVar1;
  byte *pbVar2;
  int iVar3;
  uint *puVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int local_50;
  byte *local_48;
  int local_44;
  int local_2c;
  int local_28;
  
  if (DAT_00182f58 != (undefined4 *)0x0) {
    (*(code *)*DAT_00182f58)(param_1,param_2,param_3,param_4);
    return;
  }
  if ((param_6 | param_5) == 1) {
    iVar5 = *(short *)(DAT_00182efc + 10) - param_2;
    iVar3 = param_2;
    local_28 = param_4;
    if ((0 < iVar5) &&
       (local_28 = param_4 - iVar5, iVar3 = (int)*(short *)(DAT_00182efc + 10), local_28 < 1)) {
      return;
    }
    iVar5 = (param_2 + param_4 + -1) - (int)*(short *)(DAT_00182efc + 0xe);
    if ((0 < iVar5) && (local_28 - iVar5 < 1)) {
      return;
    }
    iVar5 = (param_3 + param_1 + -1) - (int)*(short *)(DAT_00182efc + 0xc);
    local_2c = param_3;
    if (0 < iVar5) {
      local_2c = param_3 - iVar5;
    }
    param_1 = param_1 - param_11;
    if (param_1 < *(short *)(DAT_00182efc + 8)) {
      uVar6 = *(short *)(DAT_00182efc + 8) - param_1;
      local_2c = local_2c - uVar6;
      param_1 = param_1 + (uVar6 & 0xfffffff8);
    }
    if (local_2c < 1) {
      return;
    }
    (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 4))
              ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],param_1,iVar3,local_2c);
    return;
  }
  local_44 = 0;
  local_50 = param_2;
  if (param_4 < 1) {
    return;
  }
  do {
    iVar3 = local_50 + param_6 + -1;
    if ((*(short *)(DAT_00182efc + 10) <= iVar3) && (local_50 <= *(short *)(DAT_00182efc + 0xe))) {
      iVar7 = 0;
      local_48 = param_9;
      iVar5 = param_1;
      if (0 < param_3) {
        do {
          iVar8 = 8;
          pbVar2 = local_48 + 1;
          uVar6 = (uint)*local_48;
          do {
            iVar8 = iVar8 - param_7;
            uVar1 = uVar6 >> (8 - param_7 & 0xff);
            uVar6 = uVar6 << (param_7 & 0xff) & 0xff;
            if ((uVar1 != 0) || ((*(byte *)(DAT_00182efc + 0x10) & 2) == 0)) {
              puVar4 = *(uint **)(DAT_00182efc + 0x3c);
              uVar9 = *puVar4;
              if (param_10 == 0) {
                *puVar4 = uVar1;
              }
              else {
                *puVar4 = *(uint *)(param_10 + uVar1 * 4);
              }
              FUN_00115b98(iVar5,local_50,iVar5 + param_5 + -1,iVar3);
              **(uint **)(DAT_00182efc + 0x3c) = uVar9;
            }
            iVar7 = iVar7 + 1;
            iVar5 = iVar5 + param_5;
            if (param_3 <= iVar7) goto LAB_00117244;
            local_48 = pbVar2;
          } while (iVar8 != 0);
        } while( true );
      }
    }
LAB_00117244:
    local_44 = local_44 + 1;
    param_9 = param_9 + param_8;
    local_50 = local_50 + param_6;
    if (param_4 <= local_44) {
      return;
    }
  } while( true );
}

// ==== FUN_0011728c @ 0011728c, 196 bytes
// called by: FUN_00111618, FUN_001110d4
undefined1 FUN_0011728c(uint param_1)

{
  int iVar1;
  
  iVar1 = 0;
  if (param_1 != 0) {
    if ((param_1 & 0xfffffffe) == 0) {
      return 1;
    }
    if ((param_1 & 0xfffffffc) == 0) {
      iVar1 = 2;
    }
    else if ((param_1 & 0xfffffff8) == 0) {
      iVar1 = 3;
    }
    else if ((param_1 & 0xfffffff0) == 0) {
      iVar1 = 4;
    }
    else if ((param_1 & 0xffffffe0) == 0) {
      iVar1 = 5;
    }
    else if ((param_1 & 0xffffff00) == 0) {
      iVar1 = 8;
    }
    else if ((param_1 >> 9 == 0 || param_1 >> 0xc == 0) ||
            (param_1 >> 0xf == 0 || param_1 >> 0x10 == 0)) {
      iVar1 = 0x10;
    }
    else if (param_1 >> 0x12 == 0) {
      iVar1 = 0x12;
    }
    else {
      if ((param_1 & 0xff000000) == 0) {
        return 0x20;
      }
      iVar1 = 0x20;
    }
  }
  if (iVar1 - 2U < 7) {
    return 8;
  }
  if (iVar1 - 9U < 8) {
    return 0x10;
  }
  if (iVar1 - 0x11U < 0x10) {
    return 0x20;
  }
  return 0;
}

// ==== FUN_00117354 @ 00117354, 80 bytes
void FUN_00117354(void)

{
  FUN_0012bd98();
  FUN_0012c204();
  if (DAT_00182b14 != (code *)0x0) {
    (*DAT_00182b14)();
  }
  DAT_0018293c = DAT_0018293c + 1;
  if (999 < DAT_0018293c) {
    DAT_0018293c = 0;
    FUN_0012c4bc();
  }
  DAT_001823f8 = DAT_001823f8 + 1;
  return;
}

// ==== FUN_001173b0 @ 001173b0, 84 bytes
void FUN_001173b0(void)

{
  if (-1 < DAT_001823f4) {
    FUN_00108404(DAT_001823f0 + DAT_001823f4 * 0x70800);
    FUN_001127a4(DAT_001823f4);
    DAT_001823f4 = -1;
  }
  if (DAT_00182b18 == (code *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00117400. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_00182b18)();
  return;
}

// ==== FUN_0011740c @ 0011740c, 20 bytes
// called by: FUN_00110c20
void FUN_0011740c(void)

{
  *(undefined1 *)(DAT_00182efc + 0x50) = 1;
  return;
}

// ==== FUN_00117424 @ 00117424, 204 bytes
// called by: FUN_001174f0
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00117424(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 *local_3c;
  int aiStack_38 [2];
  undefined4 local_30;
  undefined4 local_2c;
  int aiStack_28 [4];
  
  if (param_1 == 0) {
    return;
  }
  local_3c = (undefined4 *)FUN_0010e24c();
  local_30 = *local_3c;
  local_2c = local_3c[1];
  uVar4 = local_3c[8];
  uVar5 = local_3c[10];
  FUN_0010e334(&local_3c);
  iVar1 = FUN_00118ed0(param_1);
  if (iVar1 != 0) {
    iVar2 = FUN_0010e384(param_1);
    uVar3 = *(undefined4 *)(iVar2 + 0x18);
    if ((uVar5 & 2) != 0) {
      FUN_00119458(&local_30,uVar3,uVar4);
    }
    local_3c = (undefined4 *)FUN_0010e24c(param_1);
    aiStack_28[0] = FUN_0010e24c(uVar3);
    aiStack_38[0] = FUN_0010e24c(iVar1);
    *(undefined4 *)(aiStack_38[0] + 0x20) = uVar4;
    local_3c[8] = *(undefined4 *)(aiStack_28[0] + 0x1c);
    *(int *)(aiStack_28[0] + 0x1c) = param_1;
    FUN_0010e334(&local_3c);
    FUN_0010e334(aiStack_28);
    FUN_0010e334(aiStack_38);
    return;
  }
  return;
}

// ==== FUN_001174f0 @ 001174f0, 480 bytes
// called by: FUN_00117fa8
int FUN_001174f0(short param_1,short param_2,int param_3,int param_4,int param_5,uint param_6,
                undefined4 param_7,int param_8)

{
  int iVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  short *local_50 [2];
  short *local_48 [4];
  undefined4 local_38;
  int local_34;
  
  param_6 = param_6 | DAT_001830d0;
  if (param_5 == 0) {
    if (DAT_001830b6 == 0) goto LAB_001175a4;
    param_5 = (&DAT_007c60c0)[*(byte *)(DAT_00182efc + 0x11)];
  }
  bVar4 = param_5 == -1;
  if (bVar4) {
    param_5 = 0;
  }
  if (!bVar4 && param_5 != 0) {
    local_48[0] = (short *)FUN_0010e24c(param_5);
    param_1 = param_1 + *local_48[0];
    param_2 = param_2 + local_48[0][1];
    if (param_3 == 0) {
      param_3 = ((int)local_48[0][2] - (int)*local_48[0]) + 1;
    }
    if (param_4 == 0) {
      param_4 = ((int)local_48[0][3] - (int)local_48[0][1]) + 1;
    }
    FUN_0010e334(local_48);
  }
LAB_001175a4:
  iVar1 = FUN_0010dfec(param_8 + 0x2c);
  if (iVar1 != 0) {
    DAT_001830b6 = DAT_001830b6 + 1;
    local_50[0] = (short *)FUN_0010e24c(iVar1);
    *local_50[0] = param_1;
    local_50[0][1] = param_2;
    local_50[0][2] = param_1 + (short)param_3 + -1;
    local_50[0][3] = param_2 + (short)param_4 + -1;
    *(undefined4 *)(local_50[0] + 8) = param_7;
    *(uint *)(local_50[0] + 0x14) = *(uint *)(local_50[0] + 0x14) | param_6 & 0x3f3f9f;
    FUN_0010e334(local_50);
    iVar2 = iVar1;
    if (DAT_001830c8 != 0) {
      iVar2 = FUN_0010e384();
      iVar3 = FUN_0010e384(iVar1);
      *(undefined4 *)(iVar3 + 0x14) = *(undefined4 *)(iVar2 + 0x14);
      *(int *)(iVar2 + 0x14) = iVar1;
      iVar2 = DAT_001830c8;
    }
    DAT_001830c8 = iVar2;
    FUN_00119100(iVar1,param_5);
    if ((param_6 & 0x20) != 0) {
      FUN_00118548(iVar1);
    }
    if ((param_6 & 1) != 0) {
      DAT_001830dc = DAT_001830dc + 1;
    }
    if ((param_6 & 0x40) != 0) {
      FUN_00117424(iVar1);
    }
    if ((param_6 & 2) != 0) {
      local_50[0] = (short *)FUN_0010e24c(iVar1);
      *(uint *)(local_50[0] + 0x14) = *(uint *)(local_50[0] + 0x14) | 2;
      FUN_0010e334(local_50);
      FUN_001180e8(iVar1,0);
    }
    local_38 = 1;
    local_34 = iVar1;
    FUN_001198a4(iVar1,&local_38);
  }
  return iVar1;
}

// ==== FUN_001176e0 @ 001176e0, 20 bytes
// called by: FUN_00110c20, FUN_0011147c
void FUN_001176e0(void)

{
  *(undefined1 *)(DAT_00182efc + 0x50) = 0;
                    /* WARNING: Could not emulate address calculation at 0x00116a90 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0xc) + 0x30))
            ((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)],DAT_00182efc + 8);
  return;
}

// ==== FUN_00117768 @ 00117768, 644 bytes
// called by: FUN_001179fc, FUN_0011b2c8
/* WARNING: Type propagation algorithm not settling */

void FUN_00117768(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  int iStack_40;
  int aiStack_3c [6];
  
  if (param_1 == 0) {
    return;
  }
  iVar3 = DAT_001830c8;
  if (DAT_001830c8 == 0) {
    return;
  }
  while (iVar4 = param_1, iVar3 != param_1) {
    iVar3 = FUN_0010e384();
    iVar3 = *(int *)(iVar3 + 0x14);
    if (iVar3 == 0) {
      return;
    }
  }
  do {
    iVar3 = iVar4;
    iVar2 = FUN_0010e384();
    iVar4 = *(int *)(iVar2 + 0x18);
  } while (*(int *)(iVar2 + 0x18) != 0);
  bVar6 = DAT_007c60c0 != iVar3;
  iVar4 = DAT_007c60c4;
  if (!bVar6) {
    iVar4 = DAT_007c60c0;
  }
  uVar5 = (uint)bVar6;
  if (bVar6 && iVar4 != iVar3) {
    if (DAT_007c60c8 == iVar3) {
      uVar5 = 2;
    }
    else if (DAT_007c60cc == iVar3) {
      uVar5 = 3;
    }
    else {
      uVar5 = 0xffffffff;
    }
  }
  aiStack_3c[1] = 0xb;
  aiStack_3c[2] = param_1;
  FUN_001198a4(param_1,aiStack_3c + 1);
  FUN_001188c4(param_1);
  iStack_40 = FUN_0010e24c(param_1);
  if (*(int *)(iStack_40 + 0x24) != 0) {
    FUN_00110e78();
  }
  iVar3 = DAT_001830c4;
  DAT_001830ec = 0;
  if ((&DAT_007c60f0)[DAT_001830c4] == param_1) {
    (&DAT_007c60f0)[DAT_001830c4] = 0;
  }
  piVar1 = DAT_001830cc;
  if ((&DAT_007c60e0)[iVar3] == param_1) {
    (&DAT_007c60e0)[iVar3] = 0;
    piVar1 = DAT_001830cc;
  }
  for (; piVar1 != (int *)0x0; piVar1 = (int *)*piVar1) {
    if (piVar1[1] == param_1) {
      piVar1[1] = 0;
    }
  }
  FUN_00118330(param_1,7);
  FUN_0011b2c8(iStack_40);
  aiStack_3c[0] = FUN_0010e24c(param_1);
  if (*(int *)(aiStack_3c[0] + 0x18) != 0) {
    FUN_001192f8(aiStack_3c[0],*(int *)(aiStack_3c[0] + 0x18),*(undefined4 *)(aiStack_3c[0] + 0x20),
                 0x20);
    iVar3 = FUN_0010e384(param_1);
    if (*(int *)(iVar3 + 0x18) != 0) {
      iVar4 = FUN_0010e384();
      iVar2 = *(int *)(iVar4 + 0x1c);
      if (iVar2 == param_1) {
        iVar3 = FUN_0010e384();
        *(undefined4 *)(iVar4 + 0x1c) = *(undefined4 *)(iVar3 + 0x20);
      }
      else {
        do {
          if (iVar2 == 0) goto LAB_0011793c;
          iVar4 = FUN_0010e384();
          iVar2 = *(int *)(iVar4 + 0x20);
        } while (iVar2 != param_1);
        *(undefined4 *)(iVar4 + 0x20) = *(undefined4 *)(iVar3 + 0x20);
      }
    }
LAB_0011793c:
    *(undefined4 *)(aiStack_3c[0] + 0x18) = 0;
  }
  FUN_0010e334(aiStack_3c);
  iVar3 = DAT_001830c8;
  do {
    if (iVar3 == 0) goto LAB_0011798c;
    iVar4 = FUN_0010e384();
    iVar3 = *(int *)(iVar4 + 0x14);
  } while (iVar3 != param_1);
  iVar3 = FUN_0010e384(param_1);
  *(undefined4 *)(iVar4 + 0x14) = *(undefined4 *)(iVar3 + 0x14);
LAB_0011798c:
  if ((*(uint *)(iStack_40 + 0x28) & 1) != 0) {
    DAT_001830dc = DAT_001830dc + -1;
  }
  if ((*(uint *)(iStack_40 + 0x28) & 0x20) != 0) {
    DAT_001830b8 = DAT_001830b8 + -1;
  }
  FUN_0010e334(&iStack_40);
  DAT_001830b6 = DAT_001830b6 + -1;
  FUN_0010e0a8(param_1);
  if (-1 < (int)uVar5) {
    FUN_00118548((&DAT_007c60c0)[uVar5]);
  }
  return;
}

// ==== FUN_001179fc @ 001179fc, 1320 bytes
// called by: FUN_0010ea18
undefined4 FUN_001179fc(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int local_d4;
  int local_d0 [2];
  undefined1 auStack_c8 [8];
  undefined1 auStack_c0 [112];
  int local_50 [3];
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  
  uVar5 = 0;
  if (DAT_001830e8 != (code *)0x0) {
    (*DAT_001830e8)();
  }
  iVar2 = DAT_001830c8;
  if (DAT_001830f0 != 0) {
    while (iVar2 != 0) {
      local_d4 = FUN_0010e24c(iVar2);
      if ((*(ushort *)(local_d4 + 0x28) & 0x8000) == 0) {
        iVar2 = *(int *)(local_d4 + 0x14);
        FUN_0010e334(&local_d4);
      }
      else {
        iVar4 = *(int *)(local_d4 + 0x14);
        FUN_0010e334(&local_d4);
        FUN_00117768(iVar2);
        iVar2 = iVar4;
      }
    }
    DAT_001830f0 = 0;
  }
  if (DAT_001830d4 != (code *)0x0) {
    (*DAT_001830d4)();
  }
  if ((DAT_00182f38 != (code *)0x0) && (iVar2 = (*DAT_00182f38)(), iVar2 != 0)) goto LAB_00117ad4;
  if (*(char *)(DAT_00182efc + 0x50) != '\0') {
    iVar2 = FUN_00112a2c();
    while (iVar2 != 0) {
LAB_00117ad4:
      do {
        uVar5 = 1;
        iVar2 = DAT_001830c8;
        if (DAT_001830f0 != 0) {
          while (iVar2 != 0) {
            local_d0[0] = FUN_0010e24c(iVar2);
            if ((*(ushort *)(local_d0[0] + 0x28) & 0x8000) == 0) {
              iVar2 = *(int *)(local_d0[0] + 0x14);
              FUN_0010e334(local_d0);
            }
            else {
              iVar4 = *(int *)(local_d0[0] + 0x14);
              FUN_0010e334(local_d0);
              FUN_00117768(iVar2);
              iVar2 = iVar4;
            }
          }
          DAT_001830f0 = 0;
        }
        if (DAT_001830d4 != (code *)0x0) {
          (*DAT_001830d4)();
        }
      } while ((DAT_00182f38 != (code *)0x0) && (iVar2 = (*DAT_00182f38)(), iVar2 != 0));
      if (*(char *)(DAT_00182efc + 0x50) == '\0') break;
      iVar2 = FUN_00112a2c();
    }
  }
  if ((*(char *)(DAT_00182efc + 0x50) == '\0') || (DAT_001830b8 == 0)) {
    local_44 = 0;
  }
  else {
    local_44 = 1;
    if (DAT_001830d8 == (undefined4 *)0x0) {
      FUN_00114f5c(1);
    }
    else {
      (*(code *)*DAT_001830d8)();
    }
  }
  if ((*(char *)(DAT_00182efc + 0x50) != '\0') && (DAT_001830b8 != 0)) {
    iVar4 = 1;
    iVar2 = DAT_001830ec;
    if (DAT_001830ec == 0) {
      iVar2 = DAT_001830c8;
    }
    FUN_00112b90(auStack_c8);
    local_38 = *(undefined4 *)(DAT_00182efc + 0x30);
    local_2c = *(undefined4 *)(DAT_00182efc + 0x34);
    iVar7 = 0;
    if (iVar2 != 0) {
      do {
        bVar1 = false;
        local_50[0] = iVar2;
        local_d4 = FUN_0010e24c(iVar2);
        local_40 = *(undefined4 *)(local_d4 + 8);
        local_3c = *(undefined4 *)(local_d4 + 0xc);
        iVar7 = *(int *)(local_d4 + 0x10);
        iVar8 = *(int *)(local_d4 + 0x18);
        uVar6 = *(uint *)(local_d4 + 0x28);
        FUN_0010e334(&local_d4);
        if ((uVar6 & 0x4020) != 0) {
          if ((iVar7 != 0) && (iVar7 = FUN_00118820(&local_40,local_50[0]), iVar7 != 0)) {
            FUN_00118548(local_50[0]);
            if ((uVar6 & 4) == 0) {
              FUN_001196dc(local_50[0]);
              bVar1 = true;
            }
            else {
              uVar3 = 1;
              if (iVar8 != 0) {
                uVar3 = ~uVar6 & 1;
              }
              FUN_00110ed8(&local_40,FUN_00126d70,local_50,0,uVar3);
            }
          }
          local_d4 = FUN_0010e24c(local_50[0]);
          uVar6 = *(uint *)(local_d4 + 0x28);
          uVar3 = uVar6 & 0xffffbfdf;
          *(uint *)(local_d4 + 0x28) = uVar3;
          if ((uVar6 & 0x2000) != 0) {
            *(uint *)(local_d4 + 0x28) = uVar3 | 4;
          }
          FUN_0010e334(&local_d4);
          if (bVar1) {
            iVar4 = iVar4 + -1;
          }
          DAT_001830b8 = DAT_001830b8 + -1;
        }
        iVar2 = FUN_0010e384(iVar2);
        iVar2 = *(int *)(iVar2 + 0x14);
        iVar7 = iVar2;
      } while (iVar2 != 0 && iVar4 != 0);
    }
    DAT_001830ec = iVar7;
    text_set_colour(local_38);
    thunk_FUN_001169f0(local_2c);
    FUN_00112b78(auStack_c8);
    while (((DAT_001830b8 != 0 && (uVar5 = 1, *(char *)(DAT_00182efc + 0x50) != '\0')) &&
           (DAT_001830b8 != 0))) {
      iVar4 = 1;
      iVar2 = DAT_001830ec;
      if (DAT_001830ec == 0) {
        iVar2 = DAT_001830c8;
      }
      FUN_00112b90(auStack_c0);
      local_2c = *(undefined4 *)(DAT_00182efc + 0x30);
      local_34 = *(undefined4 *)(DAT_00182efc + 0x34);
      iVar7 = 0;
      if (iVar2 != 0) {
        do {
          bVar1 = false;
          local_d0[0] = iVar2;
          local_d4 = FUN_0010e24c(iVar2);
          local_40 = *(undefined4 *)(local_d4 + 8);
          local_3c = *(undefined4 *)(local_d4 + 0xc);
          iVar7 = *(int *)(local_d4 + 0x10);
          iVar8 = *(int *)(local_d4 + 0x18);
          uVar6 = *(uint *)(local_d4 + 0x28);
          FUN_0010e334(&local_d4);
          if ((uVar6 & 0x4020) != 0) {
            if ((iVar7 != 0) && (iVar7 = FUN_00118820(&local_40,local_d0[0]), iVar7 != 0)) {
              FUN_00118548(local_d0[0]);
              if ((uVar6 & 4) == 0) {
                FUN_001196dc(local_d0[0]);
                bVar1 = true;
              }
              else {
                uVar3 = 1;
                if (iVar8 != 0) {
                  uVar3 = ~uVar6 & 1;
                }
                FUN_00110ed8(&local_40,FUN_00126d70,local_d0,0,uVar3);
              }
            }
            local_d4 = FUN_0010e24c(local_d0[0]);
            uVar6 = *(uint *)(local_d4 + 0x28);
            uVar3 = uVar6 & 0xffffbfdf;
            *(uint *)(local_d4 + 0x28) = uVar3;
            if ((uVar6 & 0x2000) != 0) {
              *(uint *)(local_d4 + 0x28) = uVar3 | 4;
            }
            FUN_0010e334(&local_d4);
            if (bVar1) {
              iVar4 = iVar4 + -1;
            }
            DAT_001830b8 = DAT_001830b8 + -1;
          }
          iVar2 = FUN_0010e384(iVar2);
          iVar2 = *(int *)(iVar2 + 0x14);
          iVar7 = iVar2;
        } while (iVar2 != 0 && iVar4 != 0);
      }
      DAT_001830ec = iVar7;
      text_set_colour(local_2c);
      thunk_FUN_001169f0(local_34);
      FUN_00112b78(auStack_c0);
    }
  }
  if (local_44 != 0) {
    if (DAT_001830d8 == (undefined4 *)0x0) {
      FUN_00114f5c(0);
    }
    else {
      (*(code *)DAT_001830d8[1])();
      DAT_001830c0 = 0;
    }
  }
  return uVar5;
}

// ==== FUN_00117f38 @ 00117f38, 24 bytes
// called by: FUN_00111208, FUN_0010ece4
undefined4 FUN_00117f38(void)

{
  return (&DAT_007c60c0)[*(byte *)(DAT_00182efc + 0x11)];
}

// ==== FUN_00117f58 @ 00117f58, 24 bytes
// called by: FUN_00119230, FUN_00118250, FUN_001180e8, FUN_001192f8
undefined4 FUN_00117f58(int param_1)

{
  int iVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  iVar1 = FUN_0010e384();
  return *(undefined4 *)(iVar1 + 0x1c);
}

// ==== FUN_00117f70 @ 00117f70, 24 bytes
// called by: FUN_00118ed0
undefined4 FUN_00117f70(int param_1)

{
  int iVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  iVar1 = FUN_0010e384();
  return *(undefined4 *)(iVar1 + 0x18);
}

// ==== FUN_00117f88 @ 00117f88, 32 bytes
// called by: FUN_00111208
int FUN_00117f88(int param_1)

{
  short *psVar1;
  
  if (param_1 == 0) {
    return 0;
  }
  psVar1 = (short *)FUN_0010e384(param_1);
  return (int)*psVar1;
}

// ==== FUN_00117fa8 @ 00117fa8, 284 bytes
// called by: FUN_0010ece4
void FUN_00117fa8(void)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (DAT_001830b5 != '\0') {
    return;
  }
  DAT_001830c8 = 0;
  DAT_001830ec = 0;
  *(undefined4 *)(DAT_00182efc + 0x40) = 0;
  DAT_001830b8 = 0;
  iVar3 = 0;
  DAT_001830b6 = 0;
  do {
    iVar1 = FUN_0010e7f0(iVar3);
    if (iVar1 != 0) {
      uVar2 = FUN_001174f0(0,0,0x3fff,0x3fff,0xffffffff,2,FUN_00126c5c,4);
      (&DAT_007c60c0)[iVar3] = uVar2;
      (&DAT_007c60d0)[iVar3] = 0x12345678;
      FUN_001180e8(uVar2,0);
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 4);
  iVar3 = 0;
  do {
    FUN_00118808(&DAT_007c6158 + iVar3 * 8);
    FUN_00118808(&DAT_007c6178 + iVar3 * 8);
    FUN_00118808(&DAT_007c6198 + iVar3 * 8);
    iVar3 = iVar3 + 1;
  } while (iVar3 < 4);
  FUN_00118548(DAT_007c60c0);
  *(undefined1 *)(DAT_00182efc + 0x50) = 1;
  DAT_001830b5 = 1;
  DAT_001830f4 = FUN_00124da4;
  uRam001830f8 = DAT_00182f18;
  DAT_00182f18 = &DAT_001830f4;
  return;
}

