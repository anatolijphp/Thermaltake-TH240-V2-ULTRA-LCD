// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00120000-0x00123fff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_00120698 @ 00120698, 2808 bytes
void FUN_00120698(int *param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,undefined4 *param_10)

{
  uint uVar1;
  undefined2 uVar2;
  byte bVar3;
  byte bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined1 *puVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  byte *pbVar11;
  int iVar12;
  uint uVar13;
  byte *pbVar14;
  uint uVar15;
  byte *pbVar16;
  undefined4 *puVar17;
  uint uVar18;
  int iVar19;
  uint unaff_r11;
  int local_90;
  int local_8c;
  int local_88;
  int local_84;
  int *local_7c;
  code *local_78;
  int local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c [2];
  int *local_34;
  int iStack_30;
  int local_2c;
  uint local_28;
  
  local_40 = param_9 + param_2;
  local_6c = param_4 - 1;
  puVar10 = (undefined4 *)param_1[2];
  local_34 = param_1;
  iStack_30 = param_2;
  local_2c = param_3;
  local_28 = param_4;
  if (param_6 == 8) {
    iVar9 = 0;
    if (0 < param_5) {
      do {
        uVar15 = local_28;
        puVar17 = (undefined4 *)local_34[2];
        (*(code *)*puVar17)(puVar17,param_2,iVar9 + local_2c,&local_4c,&local_60);
        pbVar14 = (byte *)(local_4c * *(int *)(puVar17[10] + 0x1c) +
                          local_60 * puVar17[3] + puVar17[1]);
        if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
          iVar12 = local_6c >> 1;
          if (param_10 == (undefined4 *)0x0) {
            if ((int)uVar15 < 2) {
              iVar12 = 0;
            }
            else {
              if (0 < local_6c) {
                pbVar11 = pbVar14 + -puVar17[9];
                pbVar16 = param_8 + -1;
                if ((uVar15 & 1) == 0) {
                  pbVar11 = pbVar11 + puVar17[9];
                  *pbVar11 = *param_8;
                  pbVar16 = param_8;
                }
                bVar3 = pbVar16[1];
                for (; iVar12 != 0; iVar12 = iVar12 + -1) {
                  bVar4 = pbVar16[2];
                  pbVar11[puVar17[9]] = bVar3;
                  bVar3 = pbVar16[3];
                  pbVar11 = pbVar11 + puVar17[9] * 2;
                  *pbVar11 = bVar4;
                  pbVar16 = pbVar16 + 2;
                }
              }
              iVar12 = uVar15 - 1;
              if (iVar12 < 0) goto LAB_00120e44;
            }
            pbVar14[puVar17[9] * iVar12] = param_8[iVar12];
          }
          else {
            if ((int)uVar15 < 2) {
              iVar12 = 0;
            }
            else {
              if (0 < local_6c) {
                pbVar11 = pbVar14 + -puVar17[9];
                pbVar16 = param_8 + -1;
                if ((uVar15 & 1) == 0) {
                  pbVar11 = pbVar11 + puVar17[9];
                  *pbVar11 = *(byte *)(param_10 + *param_8);
                  pbVar16 = param_8;
                }
                bVar3 = pbVar16[1];
                for (; iVar12 != 0; iVar12 = iVar12 + -1) {
                  bVar4 = pbVar16[2];
                  pbVar11[puVar17[9]] = *(byte *)(param_10 + bVar3);
                  pbVar11 = pbVar11 + puVar17[9] * 2;
                  bVar3 = pbVar16[3];
                  *pbVar11 = *(byte *)(param_10 + bVar4);
                  pbVar16 = pbVar16 + 2;
                }
              }
              iVar12 = uVar15 - 1;
              if (iVar12 < 0) goto LAB_00120e44;
            }
            pbVar14[puVar17[9] * iVar12] = *(byte *)(param_10 + param_8[iVar12]);
          }
        }
        else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
          if (param_10 == (undefined4 *)0x0) {
            pbVar16 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              if (*pbVar16 != 0) {
                *pbVar14 = *pbVar16;
              }
              uVar15 = uVar15 - 1;
              pbVar14 = pbVar14 + puVar17[9];
              pbVar16 = pbVar16 + 1;
            } while (uVar15 != 0);
          }
          else {
            pbVar16 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              if (*pbVar16 != 0) {
                *pbVar14 = *(byte *)(param_10 + *pbVar16);
              }
              uVar15 = uVar15 - 1;
              pbVar14 = pbVar14 + puVar17[9];
              pbVar16 = pbVar16 + 1;
            } while (uVar15 != 0);
          }
        }
LAB_00120e44:
        iVar9 = iVar9 + 1;
        param_8 = param_8 + param_7;
      } while (iVar9 < param_5);
    }
  }
  else if (param_6 < 9) {
    if (param_6 == 1) {
      local_70 = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          iVar9 = local_40;
          local_84 = local_70 + local_2c;
          local_7c = local_34;
          puVar17 = (undefined4 *)local_34[2];
          uVar5 = *param_10;
          uVar6 = param_10[1];
          (*(code *)*puVar17)(puVar17,local_40,local_84,&local_50,&local_5c);
          pbVar14 = (byte *)(local_50 * *(int *)(puVar17[10] + 0x1c) +
                            local_5c * puVar17[3] + puVar17[1]);
          bVar3 = *(byte *)(DAT_00182efc + 0x10) & 3;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar18 = param_9;
            pbVar16 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              bVar3 = (byte)uVar6;
              if (((uint)*pbVar16 & 0x80U >> (uVar18 & 0xff)) == 0) {
                bVar3 = (byte)uVar5;
              }
              uVar18 = uVar18 + 1;
              *pbVar14 = bVar3;
              if (uVar18 == 8) {
                uVar18 = 0;
                pbVar16 = pbVar16 + 1;
              }
              uVar15 = uVar15 - 1;
              pbVar14 = pbVar14 + puVar17[9];
            } while (uVar15 != 0);
          }
          else if (bVar3 == 1) {
LAB_00120870:
            local_78 = *(code **)(local_7c[3] + 0x14);
            bVar3 = (**(code **)(local_7c[4] + 8))();
            uVar18 = param_9;
            pbVar16 = param_8;
            do {
              if (((uint)*pbVar16 & 0x80U >> (uVar18 & 0xff)) != 0) {
                bVar4 = (*local_78)(local_7c,iVar9,local_84);
                *pbVar14 = bVar4 ^ bVar3;
              }
              uVar18 = uVar18 + 1;
              if (uVar18 == 8) {
                uVar18 = 0;
                pbVar16 = pbVar16 + 1;
              }
              uVar15 = uVar15 - 1;
              pbVar14 = pbVar14 + puVar17[9];
              iVar9 = iVar9 + 1;
            } while (uVar15 != 0);
          }
          else {
            uVar18 = param_9;
            pbVar16 = param_8;
            if (bVar3 == 2) {
              do {
                if (((uint)*pbVar16 & 0x80U >> (uVar18 & 0xff)) != 0) {
                  *pbVar14 = (byte)uVar6;
                }
                uVar18 = uVar18 + 1;
                if (uVar18 == 8) {
                  uVar18 = 0;
                  pbVar16 = pbVar16 + 1;
                }
                uVar15 = uVar15 - 1;
                pbVar14 = pbVar14 + puVar17[9];
              } while (uVar15 != 0);
            }
            else if (bVar3 == 3) goto LAB_00120870;
          }
          local_70 = local_70 + 1;
          param_8 = param_8 + param_7;
        } while (local_70 < param_5);
      }
    }
    else if (param_6 == 2) {
      if (0 < param_5) {
        unaff_r11 = 0xc0;
      }
      local_70 = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          puVar17 = (undefined4 *)local_34[2];
          uVar18 = (uint)*param_8;
          (*(code *)*puVar17)(puVar17,local_40,local_70 + local_2c,local_3c,&local_6c);
          puVar7 = (undefined1 *)
                   (local_3c[0] * *(int *)(puVar17[10] + 0x1c) + puVar17[3] * local_6c + puVar17[1])
          ;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar13 = param_9;
            pbVar14 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                iVar9 = 3 - uVar13;
                uVar13 = uVar13 + 1;
                *puVar7 = (char)((unaff_r11 >> (iVar9 * -2 + 6U & 0xff) & uVar18) >>
                                (iVar9 * 2 & 0xffU));
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 4) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                iVar9 = 3 - uVar13;
                uVar13 = uVar13 + 1;
                *puVar7 = *(undefined1 *)
                           (param_10 +
                           ((unaff_r11 >> (iVar9 * -2 + 6U & 0xff) & uVar18) >> (iVar9 * 2 & 0xffU))
                           );
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 4) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar13 = param_9;
            pbVar14 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar13) * -2 + 6 & 0xff) & uVar18) >>
                        ((3 - uVar13) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = (char)uVar1;
                }
                uVar13 = uVar13 + 1;
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 4) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar13) * -2 + 6 & 0xff) & uVar18) >>
                        ((3 - uVar13) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = *(undefined1 *)(param_10 + uVar1);
                }
                uVar13 = uVar13 + 1;
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 4) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          local_70 = local_70 + 1;
          param_8 = param_8 + param_7;
        } while (local_70 < param_5);
      }
    }
    else if (param_6 == 4) {
      if (0 < param_5) {
        unaff_r11 = 0xf0;
      }
      local_70 = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          puVar17 = (undefined4 *)local_34[2];
          uVar18 = (uint)*param_8;
          (*(code *)*puVar17)(puVar17,local_40,local_70 + local_2c,&local_64,&local_68);
          puVar7 = (undefined1 *)
                   (*(int *)(puVar17[10] + 0x1c) * local_64 + local_68 * puVar17[3] + puVar17[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar13 = param_9;
            pbVar14 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                iVar9 = 1 - uVar13;
                uVar13 = uVar13 + 1;
                *puVar7 = (char)((unaff_r11 >> (iVar9 * -4 + 4U & 0xff) & uVar18) >>
                                (iVar9 * 4 & 0xffU));
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 2) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                iVar9 = 1 - uVar13;
                uVar13 = uVar13 + 1;
                *puVar7 = *(undefined1 *)
                           (param_10 +
                           ((unaff_r11 >> (iVar9 * -4 + 4U & 0xff) & uVar18) >> (iVar9 * 4 & 0xffU))
                           );
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 2) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar13 = param_9;
            pbVar14 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar13) * -4 + 4 & 0xff) & uVar18) >>
                        ((1 - uVar13) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = (char)uVar1;
                }
                uVar13 = uVar13 + 1;
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 2) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar13) * -4 + 4 & 0xff) & uVar18) >>
                        ((1 - uVar13) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = *(undefined1 *)(param_10 + uVar1);
                }
                uVar13 = uVar13 + 1;
                puVar7 = puVar7 + puVar17[9];
                if (uVar13 == 2) {
                  pbVar14 = pbVar14 + 1;
                  uVar18 = (uint)*pbVar14;
                  uVar13 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          local_70 = local_70 + 1;
          param_8 = param_8 + param_7;
        } while (local_70 < param_5);
      }
    }
  }
  else if (param_6 == 0x10) {
    iVar9 = 0;
    if (0 < param_5) {
      do {
        uVar15 = local_28;
        puVar17 = (undefined4 *)local_34[2];
        (*(code *)*puVar17)(puVar17,param_2,iVar9 + local_2c,&local_48,&local_58);
        iVar12 = puVar17[9];
        iVar19 = local_48 * *(int *)(puVar17[10] + 0x1c) + local_58 * puVar17[3] + puVar17[1];
        if ((int)uVar15 < 2) {
          iVar8 = 0;
LAB_00120f84:
          *(byte *)(iVar19 + iVar8 * iVar12) = param_8[iVar8 * 2];
        }
        else {
          if (0 < local_6c) {
            pbVar16 = (byte *)(iVar19 - iVar12);
            pbVar14 = param_8 + -2;
            if ((uVar15 & 1) == 0) {
              pbVar16 = pbVar16 + iVar12;
              *pbVar16 = *param_8;
              pbVar14 = param_8;
            }
            uVar2 = *(undefined2 *)(pbVar14 + 2);
            for (iVar8 = local_6c >> 1; iVar8 != 0; iVar8 = iVar8 + -1) {
              bVar3 = pbVar14[4];
              pbVar16[iVar12] = (byte)uVar2;
              uVar2 = *(undefined2 *)(pbVar14 + 6);
              pbVar16 = pbVar16 + iVar12 * 2;
              *pbVar16 = bVar3;
              pbVar14 = pbVar14 + 4;
            }
          }
          iVar8 = uVar15 - 1;
          if (-1 < iVar8) goto LAB_00120f84;
        }
        iVar9 = iVar9 + 1;
        param_8 = param_8 + param_7;
      } while (iVar9 < param_5);
    }
  }
  else if ((param_6 == 0x20) && (iVar9 = 0, 0 < param_5)) {
    do {
      uVar15 = local_28;
      puVar17 = (undefined4 *)local_34[2];
      (*(code *)*puVar17)(puVar17,param_2,iVar9 + local_2c,&local_44,&local_54);
      iVar12 = puVar17[9];
      iVar19 = local_44 * *(int *)(puVar17[10] + 0x1c) + local_54 * puVar17[3] + puVar17[1];
      if ((int)uVar15 < 2) {
        iVar8 = 0;
LAB_0012106c:
        *(byte *)(iVar19 + iVar12 * iVar8) = param_8[iVar8 * 4];
      }
      else {
        if (0 < local_6c) {
          pbVar16 = (byte *)(iVar19 - iVar12);
          pbVar14 = param_8 + -4;
          if ((uVar15 & 1) == 0) {
            pbVar16 = pbVar16 + iVar12;
            *pbVar16 = *param_8;
            pbVar14 = param_8;
          }
          uVar5 = *(undefined4 *)(pbVar14 + 4);
          for (iVar8 = local_6c >> 1; iVar8 != 0; iVar8 = iVar8 + -1) {
            bVar3 = pbVar14[8];
            pbVar16[iVar12] = (byte)uVar5;
            uVar5 = *(undefined4 *)(pbVar14 + 0xc);
            pbVar16 = pbVar16 + iVar12 * 2;
            *pbVar16 = bVar3;
            pbVar14 = pbVar14 + 8;
          }
        }
        iVar8 = uVar15 - 1;
        if (-1 < iVar8) goto LAB_0012106c;
      }
      iVar9 = iVar9 + 1;
      param_8 = param_8 + param_7;
    } while (iVar9 < param_5);
  }
  (*(code *)*puVar10)(puVar10,local_40,local_2c,&local_74,&local_90);
  (*(code *)*puVar10)(puVar10,local_28 + local_40 + -1,local_2c + param_5 + -1,&local_8c,&local_88);
  iVar12 = local_74;
  iVar9 = local_90;
  if (local_8c < local_74) {
    local_74 = local_8c;
    local_8c = iVar12;
  }
  if (local_88 < local_90) {
    local_90 = local_88;
    local_88 = iVar9;
  }
  (**(code **)(*(int *)(*local_34 + 0xc) + 4))();
  return;
}

// ==== FUN_00121194 @ 00121194, 184 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_00121194(int param_1,int param_2,int param_3,int param_4,int param_5,byte *param_6)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0;
  if (param_4 < 1) {
    return;
  }
  do {
    iVar3 = param_2 + iVar6;
    iVar5 = 0;
    pbVar2 = param_6;
    for (iVar4 = param_3; 1 < iVar4; iVar4 = iVar4 + -2) {
      bVar1 = *pbVar2;
      FUN_00116bb4(iVar5 + param_1,iVar3,bVar1 >> 4);
      FUN_00116bb4(iVar5 + param_1 + 1,iVar3,bVar1 & 0xf);
      iVar5 = iVar5 + 2;
      pbVar2 = pbVar2 + 1;
    }
    if (iVar4 != 0) {
      FUN_00116bb4(iVar5 + param_1,iVar3,*pbVar2 >> 4);
    }
    iVar6 = iVar6 + 1;
    param_6 = param_6 + param_5;
  } while (iVar6 < param_4);
  return;
}

// ==== FUN_0012125c @ 0012125c, 16 bytes
void FUN_0012125c(void)

{
  FUN_00121b80();
  return;
}

// ==== FUN_0012126c @ 0012126c, 16 bytes
void FUN_0012126c(void)

{
  FUN_00121e84();
  return;
}

// ==== FUN_0012127c @ 0012127c, 16 bytes
void FUN_0012127c(void)

{
  FUN_00121ffc();
  return;
}

// ==== FUN_0012128c @ 0012128c, 16 bytes
void FUN_0012128c(void)

{
  FUN_001221a8();
  return;
}

// ==== FUN_0012129c @ 0012129c, 180 bytes
void FUN_0012129c(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  code *pcVar4;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  
  puVar3 = (undefined4 *)param_1[2];
  (*(code *)*puVar3)(puVar3,param_2,param_3,&local_24,&local_20);
  (*(code *)*puVar3)(puVar3,param_4,param_3,&local_1c,&local_18);
  iVar1 = local_20;
  iVar2 = local_24;
  if (local_1c < local_24) {
    local_24 = local_1c;
    local_1c = iVar2;
  }
  if (local_18 < local_20) {
    local_20 = local_18;
    local_18 = iVar1;
  }
  iVar2 = *(int *)(*param_1 + 0xc);
  if ((*(byte *)(puVar3 + 4) & 4) == 0) {
    pcVar4 = *(code **)(iVar2 + 8);
    iVar2 = local_1c;
  }
  else {
    pcVar4 = *(code **)(iVar2 + 0xc);
    iVar2 = local_18;
  }
  (*pcVar4)(*param_1,local_24,local_20,iVar2);
  return;
}

// ==== FUN_00121350 @ 00121350, 24 bytes
void FUN_00121350(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_00121368 @ 00121368, 24 bytes
void FUN_00121368(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_00121380 @ 00121380, 24 bytes
void FUN_00121380(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_001213a8 @ 001213a8, 20 bytes
void FUN_001213a8(void)

{
  FUN_00121b80();
  return;
}

// ==== FUN_001213bc @ 001213bc, 432 bytes
void FUN_001213bc(int param_1,int param_2,int param_3,int param_4)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  ushort *puVar4;
  ushort *puVar5;
  int iVar6;
  int local_28;
  int local_24;
  int local_20;
  
  local_24 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_28 = 0;
  if (*(int *)(local_24 + 0x14) != 0) {
    local_28 = FUN_0010e24c();
  }
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar4 = (ushort *)
           ((param_3 - *(short *)(local_20 + 6)) * *(int *)(local_20 + 0xc) + local_20 + 0x18 +
            param_2 * 2 + *(short *)(local_20 + 4) * -2);
  FUN_0010e334(&local_20);
  iVar6 = *(int *)(local_24 + 0xc);
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    uVar1 = (ushort)**(undefined4 **)(DAT_00182efc + 0x3c);
    if (local_28 == 0) {
      uVar2 = (param_4 - param_3) + 1;
      if (uVar2 < 0x7fffffff || param_4 - param_3 == 0x7ffffffe) {
        if ((int)uVar2 < 2) {
          uVar2 = 1;
        }
        puVar5 = puVar4;
        if ((uVar2 & 1) != 0) {
          puVar5 = (ushort *)((int)puVar4 + iVar6);
          *puVar4 = uVar1;
        }
        for (iVar3 = (int)uVar2 >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
          puVar4 = (ushort *)((int)puVar5 + iVar6);
          *puVar5 = uVar1;
          puVar5 = (ushort *)((int)puVar4 + iVar6);
          *puVar4 = uVar1;
        }
      }
      else {
        do {
          uVar2 = uVar2 - 1;
          *puVar4 = uVar1;
          puVar4 = (ushort *)((int)puVar4 + iVar6);
        } while (uVar2 != 0);
      }
    }
    else {
      do {
        (*(code *)**(undefined4 **)(local_28 + 8))(local_28,param_2,param_3);
        param_3 = param_3 + 1;
        *puVar4 = uVar1;
        puVar4 = (ushort *)((int)puVar4 + iVar6);
      } while (param_3 <= param_4);
    }
  }
  else {
    uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
    do {
      if (local_28 != 0) {
        (*(code *)**(undefined4 **)(local_28 + 8))(local_28,param_2,param_3);
      }
      param_3 = param_3 + 1;
      *puVar4 = *puVar4 ^ uVar1;
      puVar4 = (ushort *)((int)puVar4 + *(int *)(local_24 + 0xc));
    } while (param_3 <= param_4);
  }
  FUN_0010e334(&local_24);
  if (local_28 != 0) {
    FUN_0010e334(&local_28);
  }
  return;
}

// ==== FUN_00121570 @ 00121570, 408 bytes
void FUN_00121570(undefined4 param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  int iVar6;
  int local_34 [2];
  int *local_2c [2];
  int local_24 [2];
  
  local_2c[0] = (int *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_24[0] = 0;
  if (local_2c[0][5] != 0) {
    local_24[0] = FUN_0010e24c();
  }
  local_34[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar4 = (uint *)(*(int *)(local_34[0] + 0xc) * (param_3 - *(short *)(local_34[0] + 6)) +
                    local_34[0] + 0x18 + param_2 * 4 + *(short *)(local_34[0] + 4) * -4);
  FUN_0010e334(local_34);
  iVar6 = local_2c[0][3];
  uVar5 = **(uint **)(DAT_00182efc + 0x3c);
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    if (local_24[0] == 0) {
      uVar1 = (param_4 - param_3) + 1;
      if (uVar1 < 0x7fffffff || param_4 - param_3 == 0x7ffffffe) {
        if ((int)uVar1 < 2) {
          uVar1 = 1;
        }
        if ((uVar1 & 1) != 0) {
          *puVar4 = uVar5;
          puVar4 = (uint *)((int)puVar4 + iVar6);
        }
        for (iVar2 = (int)uVar1 >> 1; iVar2 != 0; iVar2 = iVar2 + -1) {
          puVar3 = (uint *)((int)puVar4 + iVar6);
          *puVar4 = uVar5;
          puVar4 = (uint *)((int)puVar3 + iVar6);
          *puVar3 = uVar5;
        }
      }
      else {
        do {
          uVar1 = uVar1 - 1;
          *puVar4 = uVar5;
          puVar4 = (uint *)((int)puVar4 + iVar6);
        } while (uVar1 != 0);
      }
    }
    else {
      do {
        (*(code *)**(undefined4 **)(local_24[0] + 8))(local_24[0],param_2,param_3);
        param_3 = param_3 + 1;
        *puVar4 = uVar5;
        puVar4 = (uint *)((int)puVar4 + iVar6);
      } while (param_3 <= param_4);
    }
  }
  else {
    uVar5 = (**(code **)(*(int *)(*local_2c[0] + 0x10) + 8))();
    do {
      if (local_24[0] != 0) {
        (*(code *)**(undefined4 **)(local_24[0] + 8))(local_24[0],param_2,param_3);
      }
      param_3 = param_3 + 1;
      *puVar4 = *puVar4 ^ uVar5;
      puVar4 = (uint *)((int)puVar4 + local_2c[0][3]);
    } while (param_3 <= param_4);
  }
  FUN_0010e334(local_2c);
  if (local_24[0] != 0) {
    FUN_0010e334(local_24);
  }
  return;
}

// ==== FUN_0012170c @ 0012170c, 416 bytes
void FUN_0012170c(int param_1,int param_2,int param_3,int param_4)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  byte *pbVar4;
  byte *pbVar5;
  int iVar6;
  int local_28;
  int local_24;
  int local_20;
  
  local_24 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_28 = 0;
  if (*(int *)(local_24 + 0x14) != 0) {
    local_28 = FUN_0010e24c();
  }
  local_20 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  pbVar4 = (byte *)(((param_3 - *(short *)(local_20 + 6)) * *(int *)(local_20 + 0xc) +
                     local_20 + 0x18 + param_2) - (int)*(short *)(local_20 + 4));
  FUN_0010e334(&local_20);
  iVar6 = *(int *)(local_24 + 0xc);
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    bVar1 = (byte)**(undefined4 **)(DAT_00182efc + 0x3c);
    if (local_28 == 0) {
      uVar2 = (param_4 - param_3) + 1;
      if (uVar2 < 0x7fffffff || param_4 - param_3 == 0x7ffffffe) {
        if ((int)uVar2 < 2) {
          uVar2 = 1;
        }
        pbVar5 = pbVar4;
        if ((uVar2 & 1) != 0) {
          pbVar5 = pbVar4 + iVar6;
          *pbVar4 = bVar1;
        }
        for (iVar3 = (int)uVar2 >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
          pbVar4 = pbVar5 + iVar6;
          *pbVar5 = bVar1;
          pbVar5 = pbVar4 + iVar6;
          *pbVar4 = bVar1;
        }
      }
      else {
        do {
          uVar2 = uVar2 - 1;
          *pbVar4 = bVar1;
          pbVar4 = pbVar4 + iVar6;
        } while (uVar2 != 0);
      }
    }
    else {
      do {
        (*(code *)**(undefined4 **)(local_28 + 8))(local_28,param_2,param_3);
        param_3 = param_3 + 1;
        *pbVar4 = bVar1;
        pbVar4 = pbVar4 + iVar6;
      } while (param_3 <= param_4);
    }
  }
  else {
    bVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
    do {
      if (local_28 != 0) {
        (*(code *)**(undefined4 **)(local_28 + 8))(local_28,param_2,param_3);
      }
      param_3 = param_3 + 1;
      *pbVar4 = *pbVar4 ^ bVar1;
      pbVar4 = pbVar4 + *(int *)(local_24 + 0xc);
    } while (param_3 <= param_4);
  }
  FUN_0010e334(&local_24);
  if (local_28 != 0) {
    FUN_0010e334(&local_28);
  }
  return;
}

// ==== FUN_001218b0 @ 001218b0, 180 bytes
void FUN_001218b0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  code *pcVar4;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  
  puVar3 = (undefined4 *)param_1[2];
  (*(code *)*puVar3)(puVar3,param_2,param_3,&local_24,&local_20);
  (*(code *)*puVar3)(puVar3,param_2,param_4,&local_1c,&local_18);
  iVar1 = local_20;
  iVar2 = local_24;
  if (local_1c < local_24) {
    local_24 = local_1c;
    local_1c = iVar2;
  }
  if (local_18 < local_20) {
    local_20 = local_18;
    local_18 = iVar1;
  }
  iVar2 = *(int *)(*param_1 + 0xc);
  if ((*(byte *)(puVar3 + 4) & 4) == 0) {
    pcVar4 = *(code **)(iVar2 + 0xc);
    iVar2 = local_18;
  }
  else {
    pcVar4 = *(code **)(iVar2 + 8);
    iVar2 = local_1c;
  }
  (*pcVar4)(*param_1,local_24,local_20,iVar2);
  return;
}

// ==== FUN_00121964 @ 00121964, 28 bytes
void FUN_00121964(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_00121980 @ 00121980, 28 bytes
void FUN_00121980(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_0012199c @ 0012199c, 28 bytes
void FUN_0012199c(int param_1)

{
  (**(code **)(*(int *)(param_1 + 0xc) + 0x10))();
  return;
}

// ==== FUN_00121a30 @ 00121a30, 292 bytes
// called by: FUN_00119cb8, FUN_00123b40
undefined4 FUN_00121a30(void)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  if (DAT_007bd008 == 0) {
    iVar4 = FUN_0010d398(DAT_007bd0a0,DAT_007bd0a4 + 8);
    iVar4 = iVar4 + 2;
  }
  else {
    iVar4 = DAT_007bcfec + (DAT_007bcfec >> 3) + 4;
  }
  iVar2 = FUN_00119cb8(iVar4 << 3);
  uVar1 = DAT_007bd008;
  if (iVar2 == 0) {
    return 1;
  }
  iVar5 = *(int *)(DAT_007bd010 + iVar2 * 8 + 4) + 8;
  (*DAT_00183074)(iVar5,DAT_007bd010,DAT_007bcfec << 3);
  DAT_007bd008 = iVar2;
  DAT_007bd010 = iVar5;
  for (uVar3 = DAT_007bcfec; uVar3 < iVar4 - 1U; uVar3 = uVar3 + 1) {
    *(uint *)(DAT_007bd010 + uVar3 * 8 + 4) = uVar3 + 1;
  }
  *(undefined4 *)(DAT_007bd010 + uVar3 * 8 + 4) = 0;
  DAT_007bd000 = DAT_007bcfec;
  DAT_007bcfe0 = (iVar4 - DAT_007bcfec) + DAT_007bcfe0;
  DAT_007bcfec = iVar4;
  if (uVar1 != 0) {
    iVar4 = uVar1 * 8 + 4;
    FUN_00119b8c(*(undefined4 *)(*(int *)(DAT_007bd010 + iVar4) + 4));
    DAT_007bcfe0 = DAT_007bcfe0 + 1;
    *(uint *)(DAT_007bd010 + iVar4) = DAT_007bd000;
    *(undefined1 *)(DAT_007bd010 + uVar1 * 8) = 0;
    DAT_007bd000 = uVar1;
  }
  return 0;
}

// ==== FUN_00121b60 @ 00121b60, 32 bytes
void FUN_00121b60(int param_1)

{
  (**(code **)(*(int *)(*(int *)(param_1 + 8) + 0x28) + 0xc))();
  return;
}

// ==== FUN_00121b80 @ 00121b80, 768 bytes
// called by: FUN_001213a8, FUN_0012125c
void FUN_00121b80(int param_1,uint param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  int *piVar12;
  
  piVar12 = *(int **)(param_1 + 8);
  uVar10 = **(uint **)(DAT_00182efc + 0x3c);
  if ((code *)piVar12[0xd] != (code *)0x0) {
    (*(code *)piVar12[0xd])(*(undefined4 *)(param_1 + 0x18));
    return;
  }
  iVar1 = piVar12[8] >> 1;
  uVar7 = param_4 - param_2;
  puVar3 = (uint *)0x0;
  puVar5 = (uint *)(*piVar12 + (iVar1 * param_3 + (param_2 >> 1)) * 4);
  uVar9 = (param_5 - param_3) + 1;
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    uVar8 = uVar7 + 1;
    uVar2 = uVar9;
    puVar6 = puVar5;
    if ((param_2 & 1) != 0) {
      for (; puVar3 = puVar6, 0 < (int)uVar2; uVar2 = uVar2 - 1) {
        *puVar3 = (uint)(ushort)*puVar3 | uVar10 << 0x10;
        puVar6 = puVar3 + iVar1;
      }
      puVar5 = puVar5 + 1;
      uVar8 = uVar7;
    }
    uVar7 = uVar10 * 0x10001;
    uVar2 = uVar9;
    if (0xf < (int)uVar8) {
      for (; 0 < (int)uVar2; uVar2 = uVar2 - 1) {
        puVar6 = puVar5 + iVar1 * (uVar2 - 1);
        *puVar6 = uVar7;
        puVar6[1] = uVar7;
        puVar6[2] = uVar7;
        puVar6[3] = uVar7;
        puVar3 = puVar6 + 8;
        puVar6[4] = uVar7;
        puVar6[5] = uVar7;
        puVar6[6] = uVar7;
        puVar6[7] = uVar7;
        if (-1 < (int)(uVar8 - 0x20)) {
          iVar11 = -0x10 - (uVar8 - 0x20);
          for (iVar4 = 1; iVar4 <= -((int)(iVar11 + ((uint)(iVar11 >> 0x1f) >> 0x1c)) >> 4);
              iVar4 = iVar4 + 1) {
            *puVar3 = uVar7;
            puVar3[1] = uVar7;
            puVar3[2] = uVar7;
            puVar3[3] = uVar7;
            puVar3[4] = uVar7;
            puVar3[5] = uVar7;
            puVar3[6] = uVar7;
            puVar3[7] = uVar7;
            puVar3 = puVar3 + 8;
          }
        }
      }
      uVar8 = uVar8 - (uVar8 & 0xfffffff0);
      puVar5 = puVar3;
    }
    puVar6 = puVar5;
    uVar2 = uVar9;
    if (1 < (int)uVar8) {
      for (; puVar6 = puVar3, 0 < (int)uVar2; uVar2 = uVar2 - 1) {
        puVar3 = puVar5 + iVar1 * (uVar2 - 1) + 1;
        puVar5[iVar1 * (uVar2 - 1)] = uVar7;
        if (1 < (int)(uVar8 - 2)) {
          for (iVar4 = 1; iVar4 <= -((int)-(uVar8 - 2) / 2); iVar4 = iVar4 + 1) {
            *puVar3 = uVar7;
            puVar3 = puVar3 + 1;
          }
        }
      }
      uVar8 = uVar8 - (uVar8 & 0xfffffffe);
    }
    if (0 < (int)uVar8) {
      uVar8 = uVar9;
    }
    if (0 < (int)uVar8) {
      do {
        uVar9 = uVar9 - 1;
        *puVar6 = *puVar6 & 0xffff0000 | uVar10;
        puVar6 = puVar6 + iVar1;
      } while (0 < (int)uVar9);
      return;
    }
    return;
  }
  uVar10 = uVar7 + 1;
  puVar6 = puVar5;
  uVar8 = uVar9;
  if ((param_2 & 1) != 0) {
    for (; puVar3 = puVar6, 0 < (int)uVar8; uVar8 = uVar8 - 1) {
      *puVar3 = *puVar3 ^ 0xffff0000;
      puVar6 = puVar3 + iVar1;
    }
    puVar5 = puVar5 + 1;
    uVar10 = uVar7;
  }
  puVar6 = puVar5;
  uVar7 = uVar9;
  if (1 < (int)uVar10) {
    for (; puVar6 = puVar3, 0 < (int)uVar7; uVar7 = uVar7 - 1) {
      puVar6 = puVar5 + iVar1 * (uVar7 - 1);
      puVar3 = puVar6 + 1;
      *puVar6 = ~*puVar6;
      if (1 < (int)(uVar10 - 2)) {
        for (iVar4 = 1; iVar4 <= -((int)-(uVar10 - 2) / 2); iVar4 = iVar4 + 1) {
          *puVar3 = ~*puVar3;
          puVar3 = puVar3 + 1;
        }
      }
    }
    uVar10 = uVar10 - (uVar10 & 0xfffffffe);
  }
  if (0 < (int)uVar10) {
    uVar10 = uVar9;
  }
  if (0 < (int)uVar10) {
    do {
      uVar9 = uVar9 - 1;
      *puVar6 = *puVar6 ^ 0xffff;
      puVar6 = puVar6 + iVar1;
    } while (0 < (int)uVar9);
    return;
  }
  return;
}

// ==== FUN_00121e84 @ 00121e84, 372 bytes
// called by: FUN_0012126c
void FUN_00121e84(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  ushort uVar1;
  ushort *puVar2;
  ushort *puVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  ushort *puVar7;
  int iVar8;
  uint uVar9;
  int local_30;
  int local_2c;
  undefined4 local_28;
  
  uVar9 = **(uint **)(DAT_00182efc + 0x3c);
  local_2c = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_30 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar7 = (ushort *)
           ((param_3 - *(short *)(local_30 + 6)) * *(int *)(local_30 + 0xc) + local_30 + 0x18 +
            param_2 * 2 + *(short *)(local_30 + 4) * -2);
  FUN_0010e334(&local_30);
  iVar8 = *(int *)(local_2c + 0xc);
  uVar6 = (param_4 - param_2) + 1;
  if (*(int *)(local_2c + 0x14) != 0) {
    local_28 = FUN_0010e24c(*(int *)(local_2c + 0x14));
    local_30 = (param_5 - param_3) + 1;
    FUN_00113b58(local_28,param_2,param_3,uVar6);
    FUN_0010e334(&local_28);
  }
  if (param_3 <= param_5) {
    do {
      if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
        FUN_001153b4(puVar7,uVar9 & 0xffff,uVar6);
      }
      else {
        uVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
        uVar4 = uVar6;
        if ((int)uVar6 < 2) {
          uVar4 = 1;
        }
        puVar2 = puVar7 + -1;
        if ((uVar4 & 1) != 0) {
          *puVar7 = *puVar7 ^ uVar1;
          puVar2 = puVar7;
        }
        for (iVar5 = (int)uVar4 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
          puVar3 = puVar2 + 2;
          puVar2[1] = puVar2[1] ^ uVar1;
          *puVar3 = *puVar3 ^ uVar1;
          puVar2 = puVar3;
        }
      }
      param_3 = param_3 + 1;
      puVar7 = (ushort *)((int)puVar7 + iVar8);
    } while (param_3 <= param_5);
  }
  FUN_0010e334(&local_2c);
  return;
}

// ==== FUN_00121ffc @ 00121ffc, 424 bytes
// called by: FUN_0012127c
void FUN_00121ffc(undefined4 param_1,int param_2,int param_3,int param_4,int param_5)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  int iVar9;
  int local_30;
  int *local_2c;
  undefined4 local_28;
  
  local_2c = (int *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_30 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar8 = (uint *)((param_3 - *(short *)(local_30 + 6)) * *(int *)(local_30 + 0xc) +
                    local_30 + 0x18 + param_2 * 4 + *(short *)(local_30 + 4) * -4);
  FUN_0010e334(&local_30);
  iVar9 = local_2c[3];
  uVar6 = **(uint **)(DAT_00182efc + 0x3c);
  uVar7 = (param_4 - param_2) + 1;
  if (local_2c[5] != 0) {
    local_28 = FUN_0010e24c();
    local_30 = (param_5 - param_3) + 1;
    FUN_00113b58(local_28,param_2,param_3,uVar7);
    FUN_0010e334(&local_28);
  }
  for (; param_3 <= param_5; param_3 = param_3 + 1) {
    if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
      if (0 < (int)uVar7) {
        puVar2 = puVar8;
        uVar5 = uVar7;
        if (uVar7 < 0x7fffffff || param_4 - param_2 == 0x7ffffffe) {
          if ((int)uVar7 < 2) {
            uVar5 = 1;
          }
          puVar2 = puVar8 + -1;
          if ((uVar5 & 1) != 0) {
            *puVar8 = uVar6;
            puVar2 = puVar8;
          }
          for (iVar4 = (int)uVar5 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
            puVar2[1] = uVar6;
            puVar2 = puVar2 + 2;
            *puVar2 = uVar6;
          }
        }
        else {
          do {
            uVar5 = uVar5 - 1;
            *puVar2 = uVar6;
            puVar2 = puVar2 + 1;
          } while (uVar5 != 0);
        }
      }
    }
    else {
      uVar1 = (**(code **)(*(int *)(*local_2c + 0x10) + 8))();
      uVar5 = uVar7;
      if ((int)uVar7 < 2) {
        uVar5 = 1;
      }
      puVar2 = puVar8 + -1;
      if ((uVar5 & 1) != 0) {
        *puVar8 = *puVar8 ^ uVar1;
        puVar2 = puVar8;
      }
      for (iVar4 = (int)uVar5 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
        puVar3 = puVar2 + 2;
        *puVar3 = *puVar3 ^ uVar1;
        puVar2[1] = puVar2[1] ^ uVar1;
        puVar2 = puVar3;
      }
    }
    puVar8 = (uint *)((int)puVar8 + iVar9);
  }
  FUN_0010e334(&local_2c);
  return;
}

// ==== FUN_001221a8 @ 001221a8, 368 bytes
// called by: FUN_0012128c
void FUN_001221a8(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  byte bVar1;
  byte *pbVar2;
  byte *pbVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  byte *pbVar7;
  int iVar8;
  uint uVar9;
  int local_30;
  int local_2c;
  undefined4 local_28;
  
  uVar9 = **(uint **)(DAT_00182efc + 0x3c);
  local_2c = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_30 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  pbVar7 = (byte *)(((param_3 - *(short *)(local_30 + 6)) * *(int *)(local_30 + 0xc) +
                     local_30 + 0x18 + param_2) - (int)*(short *)(local_30 + 4));
  FUN_0010e334(&local_30);
  iVar8 = *(int *)(local_2c + 0xc);
  uVar6 = (param_4 - param_2) + 1;
  if (*(int *)(local_2c + 0x14) != 0) {
    local_28 = FUN_0010e24c(*(int *)(local_2c + 0x14));
    local_30 = (param_5 - param_3) + 1;
    FUN_00113b58(local_28,param_2,param_3,uVar6);
    FUN_0010e334(&local_28);
  }
  for (; param_3 <= param_5; param_3 = param_3 + 1) {
    if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
      (*DAT_00183070)(pbVar7,uVar9 & 0xff,uVar6);
    }
    else {
      bVar1 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
      uVar4 = uVar6;
      if ((int)uVar6 < 2) {
        uVar4 = 1;
      }
      pbVar2 = pbVar7 + -1;
      if ((uVar4 & 1) != 0) {
        *pbVar7 = *pbVar7 ^ bVar1;
        pbVar2 = pbVar7;
      }
      for (iVar5 = (int)uVar4 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
        pbVar3 = pbVar2 + 2;
        pbVar2[1] = pbVar2[1] ^ bVar1;
        *pbVar3 = *pbVar3 ^ bVar1;
        pbVar2 = pbVar3;
      }
    }
    pbVar7 = pbVar7 + iVar8;
  }
  FUN_0010e334(&local_2c);
  return;
}

// ==== FUN_00122320 @ 00122320, 160 bytes
void FUN_00122320(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  
  puVar3 = (undefined4 *)param_1[2];
  (*(code *)*puVar3)(puVar3,param_2,param_3,&local_24,&local_20);
  (*(code *)*puVar3)(puVar3,param_4,param_5,&local_1c,&local_18);
  iVar2 = local_20;
  iVar1 = local_24;
  if (local_1c < local_24) {
    local_24 = local_1c;
    local_1c = iVar1;
  }
  if (local_18 < local_20) {
    local_20 = local_18;
    local_18 = iVar2;
  }
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x10))(*param_1,local_24,local_20,local_1c,local_18);
  return;
}

// ==== FUN_001223c0 @ 001223c0, 500 bytes
void FUN_001223c0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  ushort *puVar4;
  ushort *puVar5;
  ushort *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  
  puVar7 = (undefined4 *)param_1[2];
  (*(code *)*puVar7)(puVar7,param_2,param_3,&local_28,&local_20);
  (*(code *)*puVar7)(puVar7,param_4,param_5,&local_24,&local_1c);
  iVar3 = local_20;
  iVar8 = local_28;
  if (local_24 < local_28) {
    local_28 = local_24;
    local_24 = iVar8;
  }
  if (local_1c < local_20) {
    local_20 = local_1c;
    local_1c = iVar3;
  }
  puVar6 = (ushort *)(*(int *)(puVar7[10] + 0x1c) * local_28 + local_20 * puVar7[3] + puVar7[1]);
  iVar8 = (local_1c - local_20) + 1;
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    uVar1 = **(ushort **)(DAT_00182efc + 0x3c);
    do {
      uVar2 = (local_24 - local_28) + 1;
      if ((int)uVar2 < 2) {
        uVar2 = 1;
      }
      puVar5 = puVar6 + -1;
      if ((uVar2 & 1) != 0) {
        *puVar6 = uVar1;
        puVar5 = puVar6;
      }
      for (iVar3 = (int)uVar2 >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
        puVar5[1] = uVar1;
        puVar5 = puVar5 + 2;
        *puVar5 = uVar1;
      }
      iVar8 = iVar8 + -1;
      puVar6 = puVar6 + puVar7[7];
    } while (iVar8 != 0);
  }
  else {
    uVar1 = (**(code **)(param_1[4] + 8))();
    do {
      uVar2 = (local_24 - local_28) + 1;
      if ((int)uVar2 < 2) {
        uVar2 = 1;
      }
      puVar5 = puVar6 + -1;
      if ((uVar2 & 1) != 0) {
        *puVar6 = *puVar6 ^ uVar1;
        puVar5 = puVar6;
      }
      for (iVar3 = (int)uVar2 >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
        puVar4 = puVar5 + 2;
        puVar5[1] = puVar5[1] ^ uVar1;
        *puVar4 = *puVar4 ^ uVar1;
        puVar5 = puVar4;
      }
      iVar8 = iVar8 + -1;
      puVar6 = puVar6 + puVar7[7];
    } while (iVar8 != 0);
  }
  (**(code **)(*(int *)(*param_1 + 0xc) + 4))();
  return;
}

// ==== FUN_001225b8 @ 001225b8, 484 bytes
void FUN_001225b8(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  undefined4 *puVar7;
  int iVar8;
  bool bVar9;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  
  puVar7 = (undefined4 *)param_1[2];
  (*(code *)*puVar7)(puVar7,param_2,param_3,&local_28,&local_20);
  (*(code *)*puVar7)(puVar7,param_4,param_5,&local_24,&local_1c);
  iVar5 = local_20;
  iVar8 = local_28;
  if (local_24 < local_28) {
    local_28 = local_24;
    local_24 = iVar8;
  }
  if (local_1c < local_20) {
    local_20 = local_1c;
    local_1c = iVar5;
  }
  iVar8 = (local_1c - local_20) + 1;
  bVar9 = (DAT_00182efc[4] & 1) == 0;
  puVar1 = DAT_00182efc;
  if (bVar9) {
    puVar1 = (uint *)DAT_00182efc[0xf];
  }
  puVar6 = (uint *)(*(int *)(puVar7[10] + 0x1c) * local_28 + local_20 * puVar7[3] + puVar7[1]);
  if (bVar9) {
    uVar3 = *puVar1;
    do {
      uVar4 = (local_24 - local_28) + 1;
      if ((int)uVar4 < 2) {
        uVar4 = 1;
      }
      puVar1 = puVar6 + -1;
      if ((uVar4 & 1) != 0) {
        *puVar6 = uVar3;
        puVar1 = puVar6;
      }
      for (iVar5 = (int)uVar4 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
        puVar1[1] = uVar3;
        puVar1 = puVar1 + 2;
        *puVar1 = uVar3;
      }
      iVar8 = iVar8 + -1;
      puVar6 = puVar6 + puVar7[7];
    } while (iVar8 != 0);
  }
  else {
    uVar3 = (**(code **)(param_1[4] + 8))();
    do {
      uVar4 = (local_24 - local_28) + 1;
      if ((int)uVar4 < 2) {
        uVar4 = 1;
      }
      puVar1 = puVar6 + -1;
      if ((uVar4 & 1) != 0) {
        *puVar6 = *puVar6 ^ uVar3;
        puVar1 = puVar6;
      }
      for (iVar5 = (int)uVar4 >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
        puVar2 = puVar1 + 2;
        *puVar2 = *puVar2 ^ uVar3;
        puVar1[1] = puVar1[1] ^ uVar3;
        puVar1 = puVar2;
      }
      iVar8 = iVar8 + -1;
      puVar6 = puVar6 + puVar7[7];
    } while (iVar8 != 0);
  }
  (**(code **)(*(int *)(*param_1 + 0xc) + 4))();
  return;
}

// ==== FUN_001227a0 @ 001227a0, 468 bytes
void FUN_001227a0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  undefined1 uVar1;
  byte bVar2;
  byte *pbVar3;
  byte *pbVar4;
  uint uVar5;
  int iVar6;
  byte *pbVar7;
  undefined4 *puVar8;
  int iVar9;
  int local_40;
  int local_3c;
  int local_38;
  int local_34 [4];
  
  puVar8 = (undefined4 *)param_1[2];
  (*(code *)*puVar8)(puVar8,param_2,param_3,&local_3c,&local_38);
  (*(code *)*puVar8)(puVar8,param_4,param_5,&local_40,local_34);
  iVar6 = local_38;
  iVar9 = local_40;
  if (local_40 < local_3c) {
    local_40 = local_3c;
    local_3c = iVar9;
  }
  if (local_34[0] < local_38) {
    local_38 = local_34[0];
    local_34[0] = iVar6;
  }
  iVar9 = (local_34[0] - local_38) + 1;
  pbVar7 = (byte *)(local_3c * *(int *)(puVar8[10] + 0x1c) + local_38 * puVar8[3] + puVar8[1]);
  if ((*(byte *)(DAT_00182efc + 0x10) & 1) == 0) {
    uVar1 = **(undefined1 **)(DAT_00182efc + 0x3c);
    iVar6 = local_40 - local_3c;
    do {
      (*DAT_00183070)(pbVar7,uVar1,iVar6 + 1);
      iVar9 = iVar9 + -1;
      pbVar7 = pbVar7 + puVar8[7];
    } while (iVar9 != 0);
  }
  else {
    bVar2 = (**(code **)(param_1[4] + 8))();
    do {
      uVar5 = (local_40 - local_3c) + 1;
      if ((int)uVar5 < 2) {
        uVar5 = 1;
      }
      pbVar3 = pbVar7 + -1;
      if ((uVar5 & 1) != 0) {
        *pbVar7 = *pbVar7 ^ bVar2;
        pbVar3 = pbVar7;
      }
      for (iVar6 = (int)uVar5 >> 1; iVar6 != 0; iVar6 = iVar6 + -1) {
        pbVar4 = pbVar3 + 2;
        pbVar3[1] = pbVar3[1] ^ bVar2;
        *pbVar4 = *pbVar4 ^ bVar2;
        pbVar3 = pbVar4;
      }
      iVar9 = iVar9 + -1;
      pbVar7 = pbVar7 + puVar8[7];
    } while (iVar9 != 0);
  }
  (**(code **)(*(int *)(*param_1 + 0xc) + 4))();
  return;
}

// ==== FUN_0012297c @ 0012297c, 228 bytes
// called by: FUN_001189d8, FUN_0012297c
undefined4 FUN_0012297c(int param_1,short *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 *local_28 [2];
  undefined4 local_20;
  undefined4 local_1c;
  
  uVar4 = 0;
  while (param_1 != 0) {
    local_28[0] = (undefined4 *)FUN_0010e24c(param_1);
    uVar3 = local_28[0][10];
    if ((uVar3 & 2) != 0) {
      if (param_3 == 0) {
        local_20 = *local_28[0];
        local_1c = local_28[0][1];
      }
      else {
        FUN_00114ef0(&local_20,local_28[0],param_3);
      }
      iVar1 = FUN_00112b2c(param_2,&local_20);
      if (iVar1 != 0) {
        if ((uVar3 & 1) == 0) {
          uVar4 = 1;
          *param_2 = (short)local_1c + 1;
        }
        else {
          for (iVar1 = local_28[0][7]; iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x20)) {
            iVar2 = FUN_0012297c(iVar1,param_2,&local_20);
            if (iVar2 != 0) {
              uVar4 = 1;
            }
            iVar1 = FUN_0010e384(iVar1);
          }
        }
      }
    }
    param_1 = local_28[0][8];
    FUN_0010e334(local_28);
  }
  return uVar4;
}

// ==== FUN_00122a60 @ 00122a60, 212 bytes
// called by: FUN_001189d8, FUN_00122a60
void FUN_00122a60(int param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 *local_20 [2];
  undefined4 local_18;
  undefined4 uStack_14;
  
  if (param_1 == 0) {
    return;
  }
  do {
    local_20[0] = (undefined4 *)FUN_0010e24c(param_1);
    uVar2 = local_20[0][10];
    if ((uVar2 & 2) != 0) {
      if (param_3 == 0) {
        local_18 = *local_20[0];
        uStack_14 = local_20[0][1];
      }
      else {
        FUN_00114ef0(&local_18,local_20[0],param_3);
      }
      iVar1 = FUN_00112b2c(param_2,&local_18);
      if (iVar1 != 0) {
        if ((uVar2 & 1) == 0) {
          *(short *)(param_2 + 4) = (short)local_18 + -1;
        }
        else {
          for (iVar1 = local_20[0][7]; iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x20)) {
            FUN_00122a60(iVar1,param_2,&local_18);
            iVar1 = FUN_0010e384(iVar1);
          }
        }
      }
    }
    param_1 = local_20[0][8];
    FUN_0010e334(local_20);
  } while (param_1 != 0);
  return;
}

// ==== FUN_00122b34 @ 00122b34, 284 bytes
// called by: FUN_001189d8, FUN_00122b34
void FUN_00122b34(int param_1,int param_2,int param_3)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  undefined4 *local_20 [2];
  undefined4 local_18;
  undefined4 uStack_14;
  
  if (param_1 == 0) {
    return;
  }
  do {
    local_20[0] = (undefined4 *)FUN_0010e24c(param_1);
    uVar3 = local_20[0][10];
    if ((uVar3 & 2) != 0) {
      if (param_3 == 0) {
        local_18 = *local_20[0];
        uStack_14 = local_20[0][1];
      }
      else {
        FUN_00114ef0(&local_18,local_20[0],param_3);
      }
      iVar2 = FUN_00112b2c(param_2,&local_18);
      if (iVar2 != 0) {
        if ((uVar3 & 1) == 0) {
          sVar1 = *(short *)((int)local_20[0] + 2);
          if ((param_3 != 0) && (sVar1 <= *(short *)(param_3 + 2))) {
            sVar1 = *(short *)(param_3 + 2);
          }
          if (*(short *)(param_2 + 2) < sVar1) {
            iVar2 = (int)*(short *)(param_2 + 6);
            if (local_18._2_2_ + -1 <= (int)*(short *)(param_2 + 6)) {
              iVar2 = local_18._2_2_ + -1;
            }
            sVar1 = (short)iVar2;
          }
          else {
            sVar1 = uStack_14._2_2_;
            if (*(short *)(param_2 + 6) < uStack_14._2_2_) {
              sVar1 = *(short *)(param_2 + 6);
            }
          }
          *(short *)(param_2 + 6) = sVar1;
        }
        else {
          for (iVar2 = local_20[0][7]; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x20)) {
            FUN_00122b34(iVar2,param_2,&local_18);
            iVar2 = FUN_0010e384(iVar2);
          }
        }
      }
    }
    param_1 = local_20[0][8];
    FUN_0010e334(local_20);
  } while (param_1 != 0);
  return;
}

// ==== FUN_00122c54 @ 00122c54, 448 bytes
// called by: FUN_0011acb8, FUN_0011aabc, FUN_0010fff0, FUN_0011a5fc, FUN_0011ac14, FUN_00123c60, FUN_0011a7e8
uint FUN_00122c54(undefined4 *param_1,uint param_2,undefined4 param_3,uint param_4)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint local_28;
  
  iVar4 = param_1[10];
  uVar2 = param_1[9];
  uVar5 = uVar2 >> (0x10 - param_2 & 0xff) & (1 << (param_2 & 0xff)) - 1U;
  param_1[10] = iVar4 - param_2;
  if (0 < (int)(iVar4 - param_2)) {
    param_1[9] = uVar2 >> (-param_2 & 0x1f) | uVar2 << 0x20 - (-param_2 & 0x1f);
    return uVar5;
  }
  local_28 = param_4;
  iVar4 = (*(code *)*param_1)(param_1,&local_28);
  if (iVar4 == 0) {
    uVar6 = local_28 & 0xff;
    uVar2 = 0xff;
    if (uVar6 == 0xff) {
      iVar4 = (*(code *)*param_1)(param_1,&local_28);
      if (iVar4 != 0) {
        return 0xff;
      }
      if ((char)local_28 != '\0') {
        bVar1 = *(byte *)((int)param_1 + 0x21);
        if (bVar1 != 4) {
          *(byte *)((int)param_1 + 0x21) = bVar1 + 1;
          *(char *)((int)param_1 + bVar1 + 0x1d) = (char)local_28;
          *param_1 = FUN_00123878;
        }
        bVar1 = *(byte *)((int)param_1 + 0x21);
        if (bVar1 != 4) {
          *(byte *)((int)param_1 + 0x21) = bVar1 + 1;
          *(undefined1 *)((int)param_1 + bVar1 + 0x1d) = 0xff;
          *param_1 = FUN_00123878;
        }
      }
      uVar6 = 0xff;
    }
    iVar4 = (*(code *)*param_1)(param_1,&local_28);
    if (iVar4 == 0) {
      uVar3 = local_28 & 0xff;
      if (uVar3 == 0xff) {
        iVar4 = (*(code *)*param_1)(param_1,&local_28);
        if (iVar4 != 0) {
          return 0xff;
        }
        uVar3 = uVar2;
        if ((char)local_28 != '\0') {
          bVar1 = *(byte *)((int)param_1 + 0x21);
          if (bVar1 != 4) {
            *(byte *)((int)param_1 + 0x21) = bVar1 + 1;
            *(char *)((int)param_1 + bVar1 + 0x1d) = (char)local_28;
            *param_1 = FUN_00123878;
          }
          bVar1 = *(byte *)((int)param_1 + 0x21);
          if (bVar1 != 4) {
            *(byte *)((int)param_1 + 0x21) = bVar1 + 1;
            *(undefined1 *)((int)param_1 + bVar1 + 0x1d) = 0xff;
            *param_1 = FUN_00123878;
          }
        }
      }
      iVar4 = param_1[10];
      uVar2 = -(iVar4 + param_2) & 0x1f;
      uVar2 = ((uint)param_1[9] >> uVar2 | param_1[9] << 0x20 - uVar2) & 0xffff | uVar6 << 0x18 |
              uVar3 << 0x10;
      param_1[9] = uVar2 << (-iVar4 & 0xffU) | uVar2 >> (iVar4 + 0x20U & 0xff);
      param_1[10] = iVar4 + 0x10;
      return uVar5;
    }
  }
  return 0xff;
}

// ==== FUN_0012332c @ 0012332c, 120 bytes
int FUN_0012332c(void)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_00115d70();
  if (uVar1 == 0x100) {
    iVar2 = 8;
  }
  else if (uVar1 < 0x80) {
    if (uVar1 < 0x40) {
      if (uVar1 < 0x20) {
        if (uVar1 < 0x10) {
          if (uVar1 < 8) {
            if (uVar1 < 4) {
              if (uVar1 < 2) {
                iVar2 = 0;
              }
              else {
                iVar2 = 1;
              }
            }
            else {
              iVar2 = 2;
            }
          }
          else {
            iVar2 = 3;
          }
        }
        else {
          iVar2 = 4;
        }
      }
      else {
        iVar2 = 5;
      }
    }
    else {
      iVar2 = 6;
    }
  }
  else {
    iVar2 = 7;
  }
  return (1 << iVar2) + -1;
}

// ==== FUN_001233c0 @ 001233c0, 100 bytes
// called by: FUN_00125054
undefined4 FUN_001233c0(undefined4 *param_1,uint *param_2)

{
  int iVar1;
  undefined1 local_18 [4];
  undefined1 local_14 [8];
  
  iVar1 = (*(code *)*param_1)(param_1,local_18);
  if ((iVar1 == 0) && (iVar1 = (*(code *)*param_1)(param_1,local_14), iVar1 == 0)) {
    *param_2 = (uint)CONCAT11(local_18[0],local_14[0]);
    return 0;
  }
  return 1;
}

// ==== FUN_00123450 @ 00123450, 80 bytes
undefined2 FUN_00123450(undefined4 param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_10 + 6);
  iVar4 = *(int *)(local_10 + 0xc);
  iVar3 = local_10 + 0x18;
  sVar2 = *(short *)(local_10 + 4);
  FUN_0010e334(&local_10);
  return *(undefined2 *)((param_3 - sVar1) * iVar4 + iVar3 + param_2 * 2 + sVar2 * -2);
}

// ==== FUN_001234a4 @ 001234a4, 80 bytes
undefined4 FUN_001234a4(undefined4 param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_10 + 6);
  iVar4 = *(int *)(local_10 + 0xc);
  iVar3 = local_10 + 0x18;
  sVar2 = *(short *)(local_10 + 4);
  FUN_0010e334(&local_10);
  return *(undefined4 *)((param_3 - sVar1) * iVar4 + iVar3 + param_2 * 4 + sVar2 * -4);
}

// ==== FUN_001234f8 @ 001234f8, 80 bytes
undefined1 FUN_001234f8(undefined4 param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  sVar1 = *(short *)(local_10 + 6);
  iVar4 = *(int *)(local_10 + 0xc);
  iVar3 = local_10 + 0x18;
  sVar2 = *(short *)(local_10 + 4);
  FUN_0010e334(&local_10);
  return *(undefined1 *)(((param_3 - sVar1) * iVar4 + iVar3 + param_2) - (int)sVar2);
}

// ==== FUN_0012354c @ 0012354c, 64 bytes
void FUN_0012354c(int *param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  undefined4 uStack_14;
  undefined4 auStack_10 [2];
  
  puVar2 = auStack_10;
  (**(code **)param_1[2])();
  pcVar1 = *(code **)(*(int *)(*param_1 + 0xc) + 0x14);
  (*pcVar1)(*param_1,uStack_14,auStack_10[0],pcVar1,puVar2);
  return;
}

// ==== FUN_0012358c @ 0012358c, 80 bytes
undefined2 FUN_0012358c(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int local_14;
  int local_10 [2];
  
  puVar1 = *(undefined4 **)(param_1 + 8);
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_14,local_10);
  return *(undefined2 *)
          (local_14 * *(int *)(puVar1[10] + 0x1c) + local_10[0] * puVar1[3] + puVar1[1]);
}

// ==== FUN_001235dc @ 001235dc, 80 bytes
undefined4 FUN_001235dc(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int local_14;
  int local_10 [2];
  
  puVar1 = *(undefined4 **)(param_1 + 8);
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_14,local_10);
  return *(undefined4 *)
          (local_14 * *(int *)(puVar1[10] + 0x1c) + local_10[0] * puVar1[3] + puVar1[1]);
}

// ==== FUN_0012362c @ 0012362c, 80 bytes
undefined1 FUN_0012362c(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int local_14;
  int local_10 [2];
  
  puVar1 = *(undefined4 **)(param_1 + 8);
  (*(code *)*puVar1)(puVar1,param_2,param_3,&local_14,local_10);
  return *(undefined1 *)
          (local_14 * *(int *)(puVar1[10] + 0x1c) + local_10[0] * puVar1[3] + puVar1[1]);
}

// ==== FUN_0012367c @ 0012367c, 88 bytes
void FUN_0012367c(int param_1,undefined4 *param_2,undefined4 *param_3)

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
  *param_2 = *(undefined4 *)(iVar2 + 0x24);
  *param_3 = *(undefined4 *)(iVar2 + 0x28);
  return;
}

// ==== FUN_001236d4 @ 001236d4, 88 bytes
void FUN_001236d4(int *param_1,undefined2 *param_2)

{
  int iVar1;
  undefined1 auStack_18 [4];
  undefined2 local_14;
  undefined2 local_12;
  
  iVar1 = param_1[2];
  (**(code **)(*(int *)(*param_1 + 0xc) + 0x30))(*param_1,auStack_18);
  param_2[1] = 0;
  *param_2 = 0;
  if ((*(byte *)(iVar1 + 0x10) & 4) == 0) {
    param_2[2] = local_14;
  }
  else {
    param_2[2] = local_12;
    local_12 = local_14;
  }
  param_2[3] = local_12;
  return;
}

// ==== FUN_00123758 @ 00123758, 96 bytes
void FUN_00123758(int param_1,undefined2 *param_2,undefined4 param_3,int param_4)

{
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(param_1 + 8));
  *param_2 = *(undefined2 *)(local_10 + 4);
  param_2[1] = *(undefined2 *)(local_10 + 6);
  param_2[2] = *(short *)(local_10 + 4) + *(short *)(local_10 + 8) + -1;
  param_2[3] = *(short *)(local_10 + 10) + *(short *)(local_10 + 6) + -1;
  FUN_0010e334(&local_10);
  return;
}

// ==== FUN_001237b8 @ 001237b8, 96 bytes
void FUN_001237b8(int param_1,undefined2 *param_2,undefined4 param_3,int param_4)

{
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(param_1 + 8));
  *param_2 = *(undefined2 *)(local_10 + 4);
  param_2[1] = *(undefined2 *)(local_10 + 6);
  param_2[2] = *(short *)(local_10 + 4) + *(short *)(local_10 + 8) + -1;
  param_2[3] = *(short *)(local_10 + 10) + *(short *)(local_10 + 6) + -1;
  FUN_0010e334(&local_10);
  return;
}

// ==== FUN_00123818 @ 00123818, 96 bytes
void FUN_00123818(int param_1,undefined2 *param_2,undefined4 param_3,int param_4)

{
  int local_10;
  
  local_10 = param_4;
  local_10 = FUN_0010e24c(*(undefined4 *)(param_1 + 8));
  *param_2 = *(undefined2 *)(local_10 + 4);
  param_2[1] = *(undefined2 *)(local_10 + 6);
  param_2[2] = *(short *)(local_10 + 4) + *(short *)(local_10 + 8) + -1;
  param_2[3] = *(short *)(local_10 + 10) + *(short *)(local_10 + 6) + -1;
  FUN_0010e334(&local_10);
  return;
}

// ==== FUN_00123878 @ 00123878, 52 bytes
// called by: FUN_00122c54
undefined4 FUN_00123878(undefined4 *param_1,undefined1 *param_2)

{
  byte bVar1;
  
  bVar1 = *(char *)((int)param_1 + 0x21) - 1;
  *(byte *)((int)param_1 + 0x21) = bVar1;
  *param_2 = *(undefined1 *)((int)param_1 + bVar1 + 0x1d);
  if (*(char *)((int)param_1 + 0x21) == '\0') {
    *param_1 = FUN_001238b0;
  }
  return 0;
}

// ==== FUN_001238b0 @ 001238b0, 164 bytes
// called by: FUN_0011091c
undefined4 FUN_001238b0(int param_1,undefined1 *param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 local_10;
  
  if (*(int *)(param_1 + 0x18) == 0) {
    if (*(char *)(param_1 + 0x1c) != '\0') {
      *(undefined4 *)(param_1 + 0xc) = 0;
      *(undefined1 *)(param_1 + 0x1c) = 0;
    }
    local_10 = param_4;
    iVar1 = (**(code **)(param_1 + 4))
                      (*(undefined4 *)(param_1 + 8),&local_10,0x1000,*(undefined4 *)(param_1 + 0xc))
    ;
    if (iVar1 == 0) {
      if (*(int *)(param_1 + 0x18) == 0) {
        return 1;
      }
    }
    else {
      *(int *)(param_1 + 0x18) = iVar1;
      *(int *)(param_1 + 0xc) = iVar1 + *(int *)(param_1 + 0xc);
      *(undefined4 *)(param_1 + 0x14) = local_10;
    }
  }
  puVar2 = *(undefined1 **)(param_1 + 0x14);
  *(undefined1 **)(param_1 + 0x14) = puVar2 + 1;
  *param_2 = *puVar2;
  *(int *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + -1;
  return 0;
}

// ==== FUN_00123aac @ 00123aac, 44 bytes
undefined4 FUN_00123aac(uint param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar1 = FUN_00115d70();
  iVar2 = FUN_00115e28();
  if (param_1 < uVar1) {
    uVar3 = 0;
    if (iVar2 != 0) {
      uVar3 = *(undefined4 *)(iVar2 + param_1 * 4);
    }
    return uVar3;
  }
  return 0;
}

// ==== FUN_00123ae8 @ 00123ae8, 88 bytes
uint FUN_00123ae8(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  uint uVar3;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = FUN_0010e12c(0x50);
    *(undefined4 *)(param_1 + 8) = uVar1;
    FUN_0011528c(uVar1,0,0x50);
    if (*(int *)(param_1 + 8) == 0) {
      uVar3 = 1;
      goto LAB_00123b28;
    }
  }
  uVar3 = 0;
LAB_00123b28:
  uVar2 = FUN_00116f24(*(undefined4 *)(param_1 + 0x18),1,0);
  return uVar2 | uVar3;
}

// ==== FUN_00123b40 @ 00123b40, 276 bytes
// called by: FUN_0010e12c, FUN_00119cb8, FUN_0010e288
void FUN_00123b40(void)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  
  if (DAT_007bd004 != '\0') {
    return;
  }
  DAT_007bd004 = 1;
  DAT_00182ee4 = 0x10;
  if (DAT_007bd0a4 == 0) {
    DAT_007bd0a4 = DAT_007bd0a0 >> 2;
  }
  DAT_007bcffc = 0x5a;
  uVar1 = 0;
  iVar3 = 8;
  piVar2 = &DAT_007bd014;
  do {
    uVar4 = uVar1 + 1;
    piVar2[2] = 0x10 << (uVar1 & 0xff);
    iVar3 = iVar3 + -1;
    uVar1 = uVar1 + 2;
    piVar2 = piVar2 + 4;
    *piVar2 = 0x10 << (uVar4 & 0xff);
  } while (iVar3 != 0);
  DAT_007bcfec = 2;
  DAT_007bd010 = DAT_007bd09c + DAT_007bd0a0 + -0x14;
  DAT_007bd014 = DAT_007bd09c;
  DAT_007bcfe8 = DAT_007bd0a0;
  *(undefined4 *)(DAT_007bd09c + DAT_007bd0a0 + -0x10) = 1;
  uVar1 = 1;
  if (1 < DAT_007bcfec - 1U) {
    do {
      *(uint *)(DAT_007bd010 + uVar1 * 8 + 4) = uVar1 + 1;
      *(uint *)(DAT_007bd010 + uVar1 * 8 + 0xc) = uVar1 + 2;
      uVar1 = uVar1 + 2;
    } while (uVar1 < DAT_007bcfec - 1U);
  }
  DAT_007bd000 = 1;
  DAT_007bcfe0 = DAT_007bcfec + -1;
  FUN_00119b8c(DAT_007bcfe8,DAT_007bd014);
  FUN_00121a30();
  DAT_007bd00c = 1;
  return;
}

// ==== FUN_00123c60 @ 00123c60, 1836 bytes
// called by: FUN_0010fff0
undefined4 FUN_00123c60(undefined4 *param_1)

{
  byte bVar1;
  byte bVar2;
  ushort uVar3;
  undefined4 *puVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  byte *pbVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  undefined4 *puVar14;
  int iVar15;
  uint uVar16;
  char *unaff_r11;
  int iVar17;
  bool bVar18;
  uint local_548;
  undefined1 local_544 [4];
  byte local_540 [4];
  byte local_53c [4];
  char local_538 [4];
  undefined1 local_534 [4];
  undefined1 local_530 [1003];
  byte abStack_145 [261];
  byte local_40 [4];
  undefined4 *local_3c;
  undefined4 *local_38;
  undefined4 *local_28;
  
  local_28 = param_1;
  uVar5 = FUN_00125054(param_1,local_40);
  puVar4 = local_28;
  bVar18 = uVar5 == 0;
  if (bVar18) {
    uVar5 = (uint)local_40[0];
  }
  if (((bVar18 && uVar5 == 0xda) && (iVar6 = (*(code *)*local_28)(local_28,&local_548), iVar6 == 0))
     && (iVar6 = (*(code *)*puVar4)(puVar4,local_544), iVar6 == 0)) {
    uVar3 = CONCAT11((undefined1)local_548,local_544[0]);
    iVar6 = (*(code *)*puVar4)(puVar4,local_53c);
    if (((iVar6 == 0) && (uVar5 = (uint)local_53c[0], uVar5 * 2 + 6 == (uint)uVar3)) && (uVar5 < 5))
    {
      local_3c = puVar4 + 0x1c00;
      iVar6 = uVar3 - 3;
      iVar15 = 0;
      if (uVar5 != 0) {
        unaff_r11 = local_538;
      }
      *(byte *)((int)puVar4 + 0x732d) = local_53c[0];
      if (uVar5 != 0) {
        do {
          iVar7 = (*(code *)*puVar4)(puVar4,unaff_r11);
          if (iVar7 != 0) {
            return 1;
          }
          iVar7 = (*(code *)*puVar4)(puVar4,local_540);
          if (iVar7 != 0) {
            return 1;
          }
          iVar7 = 0;
          iVar6 = iVar6 + -2;
          if (*(byte *)(puVar4 + 0x1cc7) == 0) {
            return 1;
          }
          while ((&DAT_00007329)[(int)puVar4 + iVar7] != local_538[0]) {
            iVar7 = iVar7 + 1;
            if ((int)(uint)*(byte *)(puVar4 + 0x1cc7) <= iVar7) {
              return 1;
            }
          }
          (&DAT_0000732e)[(int)puVar4 + iVar15] = (char)iVar7;
          (&DAT_00007332)[(int)puVar4 + iVar7] = local_540[0] >> 4;
          iVar15 = iVar15 + 1;
          (&DAT_00007336)[(int)puVar4 + iVar7] = (local_540[0] & 0xf) + 4;
        } while (iVar15 < (int)(uint)local_53c[0]);
      }
      iVar15 = (*(code *)*puVar4)(puVar4,local_534);
      if (((iVar15 == 0) && (iVar15 = (*(code *)*puVar4)(puVar4,local_530), iVar15 == 0)) &&
         (iVar15 = (*(code *)*puVar4)(puVar4,local_540), iVar15 == 0)) {
        *(byte *)((int)puVar4 + 0x7377) = local_540[0] >> 4;
        *(byte *)((int)puVar4 + 0x7376) = local_540[0] & 0xf;
        if (*(char *)((int)puVar4 + 0x7409) == '\0') {
          local_530[0] = 0x3f;
          *(undefined1 *)(puVar4 + 0x1cdd) = 0;
        }
        else {
          *(undefined1 *)(puVar4 + 0x1cdd) = local_534[0];
        }
        *(undefined1 *)((int)puVar4 + 0x7375) = local_530[0];
        iVar6 = iVar6 + -3;
        if (0 < iVar6) {
          iVar15 = puVar4[6];
          if (iVar15 < iVar6) {
            do {
              iVar6 = iVar6 - iVar15;
              if (*(char *)(puVar4 + 7) != '\0') {
                puVar4[3] = 0;
                *(undefined1 *)(puVar4 + 7) = 0;
              }
              iVar15 = (*(code *)puVar4[1])(puVar4[2],&local_548,0x1000,puVar4[3]);
              if (iVar15 == 0) {
                if (puVar4[6] == 0) {
                  return 1;
                }
              }
              else {
                puVar4[6] = iVar15;
                puVar4[3] = iVar15 + puVar4[3];
                puVar4[5] = local_548;
              }
              iVar15 = puVar4[6];
            } while (iVar15 < iVar6);
          }
          puVar4[5] = puVar4[5] + iVar6;
          puVar4[6] = iVar15 - iVar6;
        }
        FUN_00119fd4(local_28);
        puVar4 = local_28;
        local_38 = local_3c;
        iVar6 = 0;
        if (*(byte *)((int)local_3c + 0x32d) != 0) {
          do {
            if (7 < (byte)(&DAT_00007332)
                          [(uint)(byte)(&DAT_0000732e)[(int)local_28 + iVar6] + (int)local_28]) {
              return 1;
            }
            if ((*(char *)(local_3c + 0xdd) == '\0') &&
               (*(char *)((int)local_28 +
                         (byte)(&DAT_00007332)
                               [(uint)(byte)(&DAT_0000732e)[(int)local_28 + iVar6] + (int)local_28]
                         + 0x2c) == '\0')) {
              return 1;
            }
            if (7 < (byte)(&DAT_00007336)
                          [(uint)(byte)(&DAT_0000732e)[(int)local_28 + iVar6] + (int)local_28]) {
              return 1;
            }
            if ((*(char *)((int)local_3c + 0x375) != '\0') &&
               (*(char *)((int)local_28 +
                         (byte)(&DAT_00007336)
                               [(uint)(byte)(&DAT_0000732e)[(int)local_28 + iVar6] + (int)local_28]
                         + 0x2c) == '\0')) {
              return 1;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 < (int)(uint)*(byte *)((int)local_3c + 0x32d));
        }
        iVar6 = 0;
        do {
          if (*(char *)((int)puVar4 + iVar6 + 0x2c) != '\0') {
            puVar14 = puVar4 + iVar6 * 0x340 + 0x22f;
            FUN_0010cc30(&local_548,0x404);
            iVar7 = 0;
            iVar15 = 1;
            do {
              iVar9 = iVar15 + iVar6 * 0x11;
              bVar1 = *(byte *)((int)puVar4 + iVar9 + 0x34);
              if (bVar1 != 0) {
                bVar2 = (byte)iVar15;
                pbVar8 = abStack_145 + iVar7;
                if ((bVar1 & 1) != 0) {
                  abStack_145[iVar7 + 1] = bVar2;
                  pbVar8 = abStack_145 + iVar7 + 1;
                }
                for (uVar5 = (uint)(*(byte *)((int)puVar4 + iVar9 + 0x34) >> 1); uVar5 != 0;
                    uVar5 = uVar5 - 1) {
                  pbVar8[1] = bVar2;
                  pbVar8 = pbVar8 + 2;
                  *pbVar8 = bVar2;
                }
                iVar7 = iVar7 + (uint)*(byte *)((int)puVar4 + iVar9 + 0x34);
              }
              iVar15 = iVar15 + 1;
            } while (iVar15 < 0x11);
            iVar9 = 0;
            abStack_145[iVar7 + 1] = 0;
            uVar5 = (uint)abStack_145[1];
            iVar15 = 0;
            uVar10 = uVar5;
            while (uVar10 != 0) {
              bVar1 = abStack_145[iVar9 + 1];
              while (uVar10 = (uint)bVar1, uVar10 == uVar5) {
                *(int *)(local_544 + iVar9 * 4 + -4) = iVar15;
                iVar15 = iVar15 + 1;
                bVar1 = abStack_145[iVar9 + 2];
                iVar9 = iVar9 + 1;
              }
              iVar15 = iVar15 << 1;
              uVar5 = uVar5 + 1;
            }
            (*DAT_00183070)(puVar14,0,0x400);
            (*DAT_00183070)(puVar4 + iVar6 * 0x340 + 0x36f,0,0x800);
            (*DAT_00183070)(puVar4 + iVar6 * 0x340 + 0x32f,0,0x100);
            iVar15 = -1;
            iVar9 = 0;
            if (0 < iVar7) {
              do {
                uVar10 = (uint)abStack_145[iVar9 + 1];
                uVar13 = (uint)*(byte *)((int)puVar4 + iVar9 + iVar6 * 0x100 + 0xbc);
                uVar5 = *(uint *)(local_544 + iVar9 * 4 + -4);
                *(byte *)((int)puVar14 + uVar13 + 0x400) = abStack_145[iVar9 + 1];
                if (uVar10 < 9) {
                  uVar16 = 1 << (8 - uVar10 & 0xff);
                  iVar12 = uVar5 << (8 - uVar10 & 0xff);
                  if (0 < (int)uVar16) {
                    uVar5 = uVar16 & 1;
                    uVar10 = (uint)(uVar5 != 0);
                    if (uVar5 != 0) {
                      puVar14[iVar12] = uVar13;
                    }
                    for (; (int)uVar5 < (int)uVar16; uVar5 = uVar5 + 2) {
                      iVar17 = uVar10 + 1;
                      puVar14[iVar12 + uVar10] = uVar13;
                      uVar10 = uVar10 + 2;
                      puVar14[iVar12 + iVar17] = uVar13;
                    }
                  }
                }
                else {
                  uVar16 = uVar5 >> (uVar10 - 8 & 0xff) & 0xff;
                  iVar12 = puVar14[uVar16];
                  if (puVar14[uVar16] == 0) {
                    puVar14[uVar16] = iVar15;
                    iVar12 = iVar15;
                    iVar15 = iVar15 + -2;
                  }
                  uVar5 = uVar5 << (0x18 - uVar10 & 0xff);
                  iVar17 = iVar15;
                  if (9 < uVar10) {
                    do {
                      iVar11 = iVar12;
                      if ((uVar5 & 0x8000) == 0) {
                        iVar11 = iVar12 + -1;
                      }
                      iVar12 = puVar14[0x13f - iVar11];
                      iVar15 = iVar17;
                      if (puVar14[0x13f - iVar11] == 0) {
                        puVar14[0x13f - iVar11] = iVar17;
                        iVar15 = iVar17 + -2;
                        iVar12 = iVar17;
                      }
                      uVar10 = uVar10 - 1;
                      uVar5 = uVar5 << 1;
                      iVar17 = iVar15;
                    } while (9 < (int)uVar10);
                  }
                  if ((uVar5 & 0x8000) == 0) {
                    iVar12 = iVar12 + -1;
                  }
                  puVar14[0x13f - iVar12] = uVar13;
                }
                iVar9 = iVar9 + 1;
              } while (iVar9 < iVar7);
            }
          }
          iVar6 = iVar6 + 1;
        } while (iVar6 < 8);
        iVar6 = local_38[0x112];
        if (0 < iVar6) {
          iVar15 = 0;
          do {
            iVar6 = iVar6 + -1;
            bVar1 = (&DAT_00007332)[(int)puVar4 + puVar4[iVar15 + 0x1d13]];
            puVar4[iVar15 + 0x1c2f] = puVar4 + puVar4[iVar15 + 0x1d13] * 0x340 + 0x22f;
            puVar4[iVar15 + 0x1c39] = puVar4 + puVar4[iVar15 + 0x1d13] * 0x340 + 0x22f;
            puVar4[iVar15 + 0x1ccf] = puVar4 + bVar1 + 0x1cd9;
            iVar15 = iVar15 + 1;
          } while (iVar6 != 0);
        }
        uVar5 = (uint)*(byte *)((int)local_3c + 0x32d);
        iVar6 = 0;
        if (uVar5 == 0) {
          uVar10 = 0;
        }
        else {
          uVar10 = uVar5 & 1;
        }
        if (-1 < (int)(uVar10 - 1)) {
          do {
            if (local_28[(byte)(&DAT_00007325 + (int)local_28)
                               [(byte)(&DAT_0000732e)[(int)local_28 + iVar6]] + 0x1cc3] == 0) {
              return 1;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 <= (int)(uVar10 - 1));
        }
        if (uVar10 < uVar5) {
          do {
            if (local_28[(byte)(&DAT_00007325 + (int)local_28)
                               [(byte)(&DAT_0000732e)[(int)local_28 + uVar10]] + 0x1cc3] == 0) {
              return 1;
            }
            if (local_28[(byte)(&DAT_00007325 + (int)local_28)
                               [(byte)(&DAT_0000732f)[(int)local_28 + uVar10]] + 0x1cc3] == 0) {
              return 1;
            }
            uVar10 = uVar10 + 2;
          } while ((int)uVar10 < (int)uVar5);
        }
        (*DAT_00183070)(local_28 + 0x1cd9,0,0x10);
        puVar4 = local_28;
        local_3c[0x10c] = 0;
        if (local_3c[0x10d] != 0) {
          local_3c[0x10e] = local_3c[0x10d];
          local_3c[0x10f] = 0;
        }
        local_28[10] = 0x10;
        FUN_00122c54(local_28,0x10);
        FUN_00122c54(puVar4,0x10);
        return 0;
      }
    }
  }
  return 1;
}

