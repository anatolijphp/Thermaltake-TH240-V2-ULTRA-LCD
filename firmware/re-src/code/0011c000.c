// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x0011c000-0x0011ffff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_0011c47c @ 0011c47c, 3176 bytes
void FUN_0011c47c(int param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,undefined *param_10)

{
  byte bVar1;
  byte bVar2;
  ushort uVar3;
  ushort *puVar4;
  int iVar5;
  uint uVar6;
  ushort *puVar7;
  byte *pbVar8;
  uint uVar9;
  ushort *puVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined *puVar18;
  bool bVar19;
  int local_80 [2];
  int local_78;
  int local_6c;
  int local_5c;
  int local_58;
  ushort *local_54;
  int local_50;
  int local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  uint local_3c;
  int local_34;
  int local_30;
  int local_2c;
  uint local_28;
  
  local_34 = param_1;
  local_30 = param_2;
  local_2c = param_3;
  local_28 = param_4;
  local_4c = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_5c = 0;
  if (*(int *)(local_4c + 0x14) != 0) {
    local_5c = FUN_0010e24c();
  }
  local_50 = *(int *)(local_4c + 0xc);
  FUN_0010e334(&local_4c);
  local_30 = param_9 + param_2;
  if ((local_5c != 0) && ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0)) {
    FUN_00113b58(local_5c,local_30,local_2c,local_28);
  }
  iVar5 = local_2c;
  iVar15 = local_30;
  local_80[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar10 = (ushort *)
            ((iVar5 - *(short *)(local_80[0] + 6)) * *(int *)(local_80[0] + 0xc) +
             local_80[0] + 0x18 + iVar15 * 2 + *(short *)(local_80[0] + 4) * -2);
  FUN_0010e334(local_80);
  local_54 = puVar10;
  if (param_6 == 0x10) {
    iVar15 = 0;
    if (0 < param_5) {
      iVar5 = local_28 << 1;
      do {
        uVar12 = local_28;
        iVar13 = local_30;
        iVar17 = local_5c;
        iVar16 = iVar15 + local_2c;
        if ((param_8 != (byte *)0x0 && local_54 != (ushort *)0x0) &&
           ((*DAT_00183074)(local_54,param_8,iVar5), iVar17 != 0)) {
          (**(code **)(*(int *)(iVar17 + 8) + 4))(iVar17,iVar13,iVar16,uVar12);
        }
        iVar15 = iVar15 + 1;
        param_8 = param_8 + param_7;
        local_54 = (ushort *)((int)local_54 + local_50);
      } while (iVar15 < param_5);
    }
    if (local_5c == 0) {
      return;
    }
  }
  else if (param_6 < 0x18) {
    if (param_6 == 8) {
      iVar15 = 0;
      if (0 < param_5) {
        local_6c = local_28 - 1;
        do {
          iVar5 = local_5c;
          if (param_10 == (undefined *)0x0) {
            iVar17 = iVar15 + local_2c;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              (*DAT_00183074)(local_54,param_8,local_28);
            }
            else {
              pbVar8 = param_8;
              puVar10 = local_54;
              uVar12 = local_28;
              iVar13 = local_30;
              if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
                do {
                  bVar2 = *pbVar8;
                  if (bVar2 != 0) {
                    *puVar10 = (ushort)bVar2;
                  }
                  if (bVar2 != 0 && iVar5 != 0) {
                    (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar13,iVar17);
                  }
                  uVar12 = uVar12 - 1;
                  pbVar8 = pbVar8 + 1;
                  puVar10 = puVar10 + 1;
                  iVar13 = iVar13 + 1;
                } while (uVar12 != 0);
              }
            }
          }
          else {
            iVar17 = iVar15 + local_2c;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              if ((int)local_28 < 2) {
                iVar5 = 0;
              }
              else {
                if (0 < local_6c) {
                  puVar10 = local_54 + -1;
                  pbVar8 = param_8 + -1;
                  if ((local_28 & 1) == 0) {
                    *local_54 = (ushort)*(undefined4 *)(param_10 + (uint)*param_8 * 4);
                    puVar10 = local_54;
                    pbVar8 = param_8;
                  }
                  bVar2 = pbVar8[1];
                  for (iVar5 = local_6c >> 1; iVar5 != 0; iVar5 = iVar5 + -1) {
                    bVar1 = pbVar8[2];
                    puVar10[1] = (ushort)*(undefined4 *)(param_10 + (uint)bVar2 * 4);
                    bVar2 = pbVar8[3];
                    puVar10 = puVar10 + 2;
                    *puVar10 = (ushort)*(undefined4 *)(param_10 + (uint)bVar1 * 4);
                    pbVar8 = pbVar8 + 2;
                  }
                }
                iVar5 = local_28 - 1;
                if (iVar5 < 0) goto LAB_0011c944;
              }
              local_54[iVar5] = (ushort)*(undefined4 *)(param_10 + (uint)param_8[iVar5] * 4);
            }
            else {
              uVar12 = local_28;
              pbVar8 = param_8;
              puVar10 = local_54;
              iVar13 = local_30;
              if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
                do {
                  if ((*pbVar8 != 0) &&
                     (*puVar10 = (ushort)*(undefined4 *)(param_10 + (uint)*pbVar8 * 4), iVar5 != 0))
                  {
                    (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar13,iVar17);
                  }
                  uVar12 = uVar12 - 1;
                  pbVar8 = pbVar8 + 1;
                  puVar10 = puVar10 + 1;
                  iVar13 = iVar13 + 1;
                } while (uVar12 != 0);
              }
            }
          }
LAB_0011c944:
          iVar15 = iVar15 + 1;
          param_8 = param_8 + param_7;
          local_54 = (ushort *)((int)local_54 + local_50);
        } while (iVar15 < param_5);
      }
      if (local_5c == 0) {
        return;
      }
    }
    else {
      if (param_10 == (undefined *)0x0) {
        param_10 = &DAT_00181db8;
      }
      local_58 = 0;
      if (0 < param_5) {
        local_3c = 4 - param_9;
        local_40 = param_9 << 1;
        local_44 = 2 - param_9;
        local_48 = param_9 << 2;
        do {
          uVar12 = local_28;
          iVar15 = local_30;
          puVar10 = local_54;
          iVar5 = local_5c;
          if (param_6 == 1) {
            local_4c = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
            uVar12 = local_28;
            iVar15 = local_30;
            puVar10 = local_54;
            iVar5 = local_5c;
            local_80[0] = local_58 + local_2c;
            puVar18 = (undefined *)(uint)*(byte *)(DAT_00182efc + 0x10);
            uVar11 = (uint)puVar18 & 3;
            bVar19 = (*(byte *)(DAT_00182efc + 0x10) & 3) == 0;
            if (bVar19) {
              puVar18 = &DAT_00182184;
            }
            uVar9 = (uint)((byte)(&DAT_00182184)[*param_8] >> (param_9 & 0xff));
            uVar6 = 8 - param_9;
            pbVar8 = param_8;
            if (bVar19) {
              while( true ) {
                if (uVar12 < uVar6) {
                  uVar6 = uVar12;
                }
                uVar12 = uVar12 - uVar6;
                puVar4 = puVar10;
                if (uVar6 < 0x80000000) {
                  if ((int)uVar6 < 2) {
                    uVar6 = 1;
                  }
                  puVar4 = puVar10 + -1;
                  if ((uVar6 & 1) != 0) {
                    uVar11 = uVar9 & 1;
                    uVar9 = uVar9 >> 1;
                    *puVar10 = (ushort)*(undefined4 *)(param_10 + uVar11 * 4);
                    puVar4 = puVar10;
                  }
                  for (iVar15 = (int)uVar6 >> 1; iVar15 != 0; iVar15 = iVar15 + -1) {
                    uVar14 = uVar9 & 1;
                    uVar11 = uVar9 >> 1;
                    uVar9 = uVar9 >> 2;
                    puVar4[1] = (ushort)*(undefined4 *)(param_10 + uVar14 * 4);
                    puVar4 = puVar4 + 2;
                    *puVar4 = (ushort)*(undefined4 *)(param_10 + (uVar11 & 1) * 4);
                  }
                  puVar10 = puVar10 + uVar6;
                }
                else {
                  do {
                    uVar11 = uVar9 & 1;
                    uVar6 = uVar6 - 1;
                    uVar9 = uVar9 >> 1;
                    puVar10 = puVar4 + 1;
                    *puVar4 = (ushort)*(undefined4 *)(param_10 + uVar11 * 4);
                    puVar4 = puVar10;
                  } while (uVar6 != 0);
                }
                if (uVar12 == 0) break;
                pbVar8 = pbVar8 + 1;
                uVar6 = 8;
                uVar9 = (uint)(byte)puVar18[*pbVar8];
              }
            }
            else {
              if (uVar11 != 1) {
                if (uVar11 == 2) {
                  uVar3 = *(ushort *)(param_10 + 4);
                  puVar10 = local_54;
                  uVar12 = local_28;
                  do {
                    if (uVar12 < uVar6) {
                      uVar6 = uVar12;
                    }
                    uVar12 = uVar12 - uVar6;
                    if (iVar5 == 0) {
                      for (; uVar9 != 0; uVar9 = uVar9 >> 1) {
                        if ((uVar9 & 1) != 0) {
                          *puVar10 = uVar3;
                        }
                        uVar6 = uVar6 - 1;
                        puVar10 = puVar10 + 1;
                        if (uVar6 == 0) goto LAB_0011cb28;
                      }
                      puVar10 = puVar10 + uVar6;
                    }
                    else {
                      for (; uVar9 != 0; uVar9 = uVar9 >> 1) {
                        if ((uVar9 & 1) != 0) {
                          (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15,local_80[0]);
                          *puVar10 = uVar3;
                        }
                        uVar6 = uVar6 - 1;
                        puVar10 = puVar10 + 1;
                        iVar15 = iVar15 + 1;
                        if (uVar6 == 0) goto LAB_0011cb28;
                      }
                      puVar10 = puVar10 + uVar6;
                      iVar15 = iVar15 + uVar6;
                    }
LAB_0011cb28:
                    if (uVar12 == 0) goto LAB_0011cc1c;
                    pbVar8 = pbVar8 + 1;
                    uVar6 = 8;
                    uVar9 = (uint)(byte)(&DAT_00182184)[*pbVar8];
                  } while( true );
                }
                if (uVar11 != 3) goto LAB_0011cc1c;
              }
              uVar3 = (**(code **)(*(int *)(local_34 + 0x10) + 8))();
              while( true ) {
                if (uVar12 < uVar6) {
                  uVar6 = uVar12;
                }
                uVar12 = uVar12 - uVar6;
                if (iVar5 == 0) {
                  if (uVar6 < 0x80000000) {
                    puVar4 = puVar10;
                    uVar11 = uVar6;
                    if ((int)uVar6 < 2) {
                      uVar6 = 1;
                      uVar11 = uVar6;
                    }
                    do {
                      if ((uVar9 & 1) != 0) {
                        *puVar4 = *puVar4 ^ uVar3;
                      }
                      uVar6 = uVar6 - 1;
                      uVar9 = uVar9 >> 1;
                      puVar4 = puVar4 + 1;
                    } while (uVar6 != 0);
                    puVar10 = puVar10 + uVar11;
                  }
                  else {
                    do {
                      if ((uVar9 & 1) != 0) {
                        *puVar10 = *puVar10 ^ uVar3;
                      }
                      uVar6 = uVar6 - 1;
                      uVar9 = uVar9 >> 1;
                      puVar10 = puVar10 + 1;
                    } while (uVar6 != 0);
                  }
                }
                else {
                  do {
                    if ((uVar9 & 1) != 0) {
                      (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15,local_80[0]);
                      *puVar10 = *puVar10 ^ uVar3;
                    }
                    uVar6 = uVar6 - 1;
                    uVar9 = uVar9 >> 1;
                    puVar10 = puVar10 + 1;
                    iVar15 = iVar15 + 1;
                  } while (uVar6 != 0);
                }
                if (uVar12 == 0) break;
                pbVar8 = pbVar8 + 1;
                uVar6 = 8;
                uVar9 = (uint)(byte)(&DAT_00182184)[*pbVar8];
              }
            }
LAB_0011cc1c:
            FUN_0010e334(&local_4c);
          }
          else if (param_6 == 2) {
            local_78 = local_58 + local_2c;
            uVar9 = local_3c & 0xff;
            bVar2 = *(byte *)(DAT_00182efc + 0x10) & 3;
            uVar12 = (uint)*param_8 << (local_40 & 0xff) & 0xff;
            uVar11 = local_28;
            pbVar8 = param_8;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              while( true ) {
                if ((int)uVar11 < (int)uVar9) {
                  uVar9 = uVar11 & 0xff;
                }
                uVar11 = uVar11 - uVar9;
                if (uVar9 < 2) {
                  uVar9 = 1;
                }
                puVar4 = puVar10 + -1;
                if ((uVar9 & 1) != 0) {
                  uVar6 = uVar12 >> 6;
                  uVar12 = (uVar12 & 0x3f) << 2;
                  *puVar10 = (ushort)*(undefined4 *)(param_10 + uVar6 * 4);
                  puVar4 = puVar10;
                }
                for (iVar15 = (int)uVar9 >> 1; iVar15 != 0; iVar15 = iVar15 + -1) {
                  uVar6 = uVar12 & 0x3f;
                  puVar4[1] = (ushort)*(undefined4 *)(param_10 + (uVar12 >> 6) * 4);
                  uVar12 = (uVar12 & 0xf) << 4;
                  puVar4 = puVar4 + 2;
                  *puVar4 = (ushort)*(undefined4 *)(param_10 + (uVar6 >> 4) * 4);
                }
                puVar10 = puVar10 + uVar9;
                if (uVar11 == 0) break;
                pbVar8 = pbVar8 + 1;
                uVar12 = (uint)*pbVar8;
                uVar9 = 4;
              }
            }
            else if (bVar2 == 1) {
              while( true ) {
                if ((int)uVar11 < (int)uVar9) {
                  uVar9 = uVar11 & 0xff;
                }
                uVar11 = uVar11 - uVar9;
                puVar4 = puVar10;
                uVar6 = uVar9;
                if (uVar9 < 2) {
                  uVar9 = 1;
                  uVar6 = uVar9;
                }
                do {
                  if ((uVar12 & 0xc0) != 0) {
                    *puVar4 = *puVar4 ^ 0xff;
                  }
                  uVar9 = uVar9 - 1;
                  uVar12 = (uVar12 & 0x3f) << 2;
                  puVar4 = puVar4 + 1;
                } while (uVar9 != 0);
                puVar10 = puVar10 + uVar6;
                if (uVar11 == 0) break;
                pbVar8 = pbVar8 + 1;
                uVar12 = (uint)*pbVar8;
                uVar9 = 4;
              }
            }
            else if (bVar2 == 2) {
              while( true ) {
                if ((int)uVar11 < (int)uVar9) {
                  uVar9 = uVar11 & 0xff;
                }
                uVar11 = uVar11 - uVar9;
                do {
                  if (((uVar12 & 0xc0) != 0) &&
                     (*puVar10 = (ushort)*(undefined4 *)(param_10 + (uVar12 >> 6) * 4), iVar5 != 0))
                  {
                    (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15,local_78);
                  }
                  uVar9 = uVar9 - 1 & 0xff;
                  uVar12 = (uVar12 & 0x3f) << 2;
                  puVar10 = puVar10 + 1;
                  iVar15 = iVar15 + 1;
                } while (uVar9 != 0);
                if (uVar11 == 0) break;
                pbVar8 = pbVar8 + 1;
                uVar12 = (uint)*pbVar8;
                uVar9 = 4;
              }
            }
          }
          else if (param_6 == 4) {
            iVar17 = local_58 + local_2c;
            uVar6 = local_44 & 0xff;
            uVar9 = (uint)*param_8 << (local_48 & 0xff);
            bVar2 = *(byte *)(DAT_00182efc + 0x10) & 3;
            uVar11 = uVar9 & 0xff;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              pbVar8 = param_8;
              if (param_9 != 0) {
                uVar12 = local_28 - 1;
                puVar10 = local_54 + 1;
                *local_54 = (ushort)*(undefined4 *)(param_10 + (uVar11 >> 4) * 4);
                pbVar8 = param_8 + 1;
                uVar11 = (uint)*pbVar8;
              }
              if (1 < (int)uVar12) {
                iVar15 = (int)uVar12 >> 1;
                if (iVar15 < 2) {
                  iVar15 = 1;
                }
                uVar12 = uVar12 & 1;
                iVar5 = iVar15;
                puVar4 = puVar10;
                do {
                  pbVar8 = pbVar8 + 1;
                  puVar7 = puVar4 + 1;
                  *puVar4 = (ushort)*(undefined4 *)(param_10 + (uVar11 >> 4) * 4);
                  iVar5 = iVar5 + -1;
                  puVar4 = puVar4 + 2;
                  *puVar7 = (ushort)*(undefined4 *)(param_10 + (uVar11 & 0xf) * 4);
                  uVar11 = (uint)*pbVar8;
                } while (iVar5 != 0);
                puVar10 = puVar10 + iVar15 * 2;
              }
              if (uVar12 != 0) {
                *puVar10 = (ushort)*(undefined4 *)(param_10 + (uVar11 >> 4) * 4);
              }
            }
            else {
              pbVar8 = param_8;
              if (bVar2 == 1) {
                while( true ) {
                  if ((int)uVar12 < (int)uVar6) {
                    uVar6 = uVar12 & 0xff;
                  }
                  uVar12 = uVar12 - uVar6;
                  puVar4 = puVar10;
                  uVar9 = uVar6;
                  if (uVar6 < 2) {
                    uVar6 = 1;
                    uVar9 = uVar6;
                  }
                  do {
                    if ((uVar11 & 0xc0) != 0) {
                      *puVar4 = *puVar4 ^ 0xff;
                    }
                    uVar6 = uVar6 - 1;
                    uVar11 = (uVar11 & 0xf) << 4;
                    puVar4 = puVar4 + 1;
                  } while (uVar6 != 0);
                  puVar10 = puVar10 + uVar9;
                  if (uVar12 == 0) break;
                  pbVar8 = pbVar8 + 1;
                  uVar11 = (uint)*pbVar8;
                  uVar6 = 2;
                }
              }
              else if (bVar2 == 2) {
                uVar6 = param_9;
                if (param_9 != 0) {
                  if ((uVar9 & 0xf0) != 0) {
                    uVar6 = *(uint *)(param_10 + (uVar11 >> 4) * 4);
                    *local_54 = (ushort)uVar6;
                    if (local_5c != 0) {
                      uVar6 = (*(code *)**(undefined4 **)(local_5c + 8))(local_5c,local_30,iVar17);
                    }
                  }
                  pbVar8 = param_8 + 1;
                  uVar11 = (uint)*pbVar8;
                  uVar12 = uVar12 - 1;
                  puVar10 = puVar10 + 1;
                  iVar15 = iVar15 + 1;
                }
                for (; 1 < (int)uVar12; uVar12 = uVar12 - 2) {
                  if (((uVar11 & 0xf0) != 0) &&
                     (*puVar10 = (ushort)*(undefined4 *)(param_10 + (uVar11 >> 4) * 4), iVar5 != 0))
                  {
                    (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15,iVar17);
                  }
                  uVar6 = 0;
                  if ((uVar11 & 0xf) != 0) {
                    uVar6 = *(uint *)(param_10 + (uVar11 & 0xf) * 4);
                    puVar10[1] = (ushort)uVar6;
                    if (iVar5 != 0) {
                      uVar6 = (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15 + 1,iVar17);
                    }
                  }
                  uVar11 = (uint)pbVar8[1];
                  puVar10 = puVar10 + 2;
                  pbVar8 = pbVar8 + 1;
                  iVar15 = iVar15 + 2;
                }
                bVar19 = uVar12 == 0;
                if (!bVar19) {
                  uVar6 = uVar11 >> 4;
                  bVar19 = uVar6 == 0;
                }
                if ((!bVar19) &&
                   (*puVar10 = (ushort)*(undefined4 *)(param_10 + uVar6 * 4), iVar5 != 0)) {
                  (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar15,iVar17);
                }
              }
            }
          }
          local_58 = local_58 + 1;
          param_8 = param_8 + param_7;
          local_54 = (ushort *)(local_50 + (int)local_54);
        } while (local_58 < param_5);
      }
      if (local_5c == 0) {
        return;
      }
    }
  }
  else {
    iVar15 = 0;
    if (0 < param_5) {
      do {
        uVar12 = local_28;
        iVar13 = local_30;
        iVar17 = local_34;
        puVar10 = local_54;
        iVar5 = local_5c;
        iVar16 = iVar15 + local_2c;
        if (param_8 != (byte *)0x0 && local_54 != (ushort *)0x0) {
          pbVar8 = param_8;
          if (local_5c != 0) {
            (**(code **)(*(int *)(local_5c + 8) + 4))(local_5c,local_30,iVar16,local_28);
          }
          do {
            uVar3 = (*(code *)**(undefined4 **)(iVar17 + 0x10))(*(undefined4 *)pbVar8);
            *puVar10 = uVar3;
            if (iVar5 != 0) {
              (*(code *)**(undefined4 **)(iVar5 + 8))(iVar5,iVar13,iVar16);
            }
            uVar12 = uVar12 - 1;
            pbVar8 = pbVar8 + 4;
            puVar10 = puVar10 + 1;
            iVar13 = iVar13 + 1;
          } while (uVar12 != 0);
        }
        iVar15 = iVar15 + 1;
        param_8 = param_8 + param_7;
        local_54 = (ushort *)((int)local_54 + local_50);
      } while (iVar15 < param_5);
    }
    if (local_5c == 0) {
      return;
    }
  }
  FUN_0010e334(&local_5c);
  return;
}

// ==== FUN_0011d0f4 @ 0011d0f4, 3160 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_0011d0f4(int param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,uint *param_8,uint param_9,undefined *param_10)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  byte bVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  int iVar9;
  uint *puVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  undefined *puVar16;
  bool bVar17;
  int aiStack_80 [2];
  uint *puStack_78;
  int iStack_6c;
  int iStack_68;
  int iStack_58;
  int iStack_54;
  int iStack_50;
  int *piStack_4c;
  uint uStack_48;
  uint uStack_40;
  uint uStack_3c;
  uint uStack_38;
  int iStack_34;
  int iStack_30;
  int iStack_2c;
  uint uStack_28;
  
  if (*(int *)(DAT_00182efc + 100) == 0) {
    return;
  }
  iStack_34 = param_1;
  iStack_30 = param_2;
  iStack_2c = param_3;
  uStack_28 = param_4;
  piStack_4c = (int *)FUN_0010e24c();
  iStack_58 = 0;
  if (piStack_4c[5] != 0) {
    iStack_58 = FUN_0010e24c();
  }
  iStack_54 = piStack_4c[3];
  FUN_0010e334(&piStack_4c);
  iStack_30 = param_9 + param_2;
  if ((iStack_58 != 0) && ((*(byte *)(DAT_00182efc + 0x10) & 2) == 0)) {
    aiStack_80[0] = param_5;
    FUN_00113b58(iStack_58,iStack_30,iStack_2c,uStack_28);
  }
  iVar9 = iStack_2c;
  iVar11 = iStack_30;
  aiStack_80[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  puVar8 = (uint *)((iVar9 - *(short *)(aiStack_80[0] + 6)) * *(int *)(aiStack_80[0] + 0xc) +
                    aiStack_80[0] + 0x18 + iVar11 * 4 + *(short *)(aiStack_80[0] + 4) * -4);
  FUN_0010e334(aiStack_80);
  puStack_78 = puVar8;
  if (param_6 < 0x18) {
    if (param_6 == 0x10) {
      iVar11 = 0;
      if (0 < param_5) {
        do {
          uVar7 = uStack_28;
          iVar12 = iStack_30;
          iVar15 = iStack_34;
          iVar9 = iStack_58;
          puVar8 = puStack_78;
          iVar14 = iVar11 + iStack_2c;
          puVar10 = param_8;
          if (iStack_58 != 0) {
            (**(code **)(*(int *)(iStack_58 + 8) + 4))(iStack_58,iStack_30,iVar14,uStack_28);
          }
          do {
            uVar1 = (**(code **)(*(int *)(iVar15 + 0x10) + 4))((short)*puVar10);
            *puVar8 = uVar1;
            if (iVar9 != 0) {
              (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar12,iVar14);
            }
            uVar7 = uVar7 - 1;
            puVar8 = puVar8 + 1;
            puVar10 = (uint *)((int)puVar10 + 2);
            iVar12 = iVar12 + 1;
          } while (uVar7 != 0);
          iVar11 = iVar11 + 1;
          param_8 = (uint *)((int)param_8 + param_7);
          puStack_78 = (uint *)((int)puStack_78 + iStack_54);
        } while (iVar11 < param_5);
      }
      if (iStack_58 == 0) {
        return;
      }
    }
    else if (param_6 == 8) {
      iVar11 = 0;
      if (0 < param_5) {
        do {
          iVar9 = iStack_58;
          iVar15 = iVar11 + iStack_2c;
          if (param_10 == (undefined *)0x0) {
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              (*DAT_00183074)(puStack_78,param_8,uStack_28);
            }
            else {
              uVar7 = uStack_28;
              puVar8 = param_8;
              puVar10 = puStack_78;
              iVar12 = iStack_30;
              if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
                do {
                  uVar1 = (uint)(byte)*puVar8;
                  if (uVar1 != 0) {
                    *puVar10 = uVar1;
                  }
                  if (uVar1 != 0 && iVar9 != 0) {
                    (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar12,iVar15);
                  }
                  uVar7 = uVar7 - 1;
                  puVar8 = (uint *)((int)puVar8 + 1);
                  puVar10 = puVar10 + 1;
                  iVar12 = iVar12 + 1;
                } while (uVar7 != 0);
              }
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar7 = uStack_28;
            if ((int)uStack_28 < 2) {
              uVar7 = 1;
            }
            puVar8 = (uint *)((int)param_8 + -1);
            puVar10 = puStack_78 + -1;
            if ((uVar7 & 1) != 0) {
              *puStack_78 = *(uint *)(param_10 + (uint)(byte)*param_8 * 4);
              puVar8 = param_8;
              puVar10 = puStack_78;
            }
            for (iVar9 = (int)uVar7 >> 1; iVar9 != 0; iVar9 = iVar9 + -1) {
              puVar10[1] = *(uint *)(param_10 + (uint)*(byte *)((int)puVar8 + 1) * 4);
              puVar8 = (uint *)((int)puVar8 + 2);
              puVar10 = puVar10 + 2;
              *puVar10 = *(uint *)(param_10 + (uint)*(byte *)puVar8 * 4);
            }
          }
          else {
            uVar7 = uStack_28;
            puVar8 = puStack_78;
            puVar10 = param_8;
            iVar12 = iStack_30;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
              do {
                if (((byte)*puVar10 != 0) &&
                   (*puVar8 = *(uint *)(param_10 + (uint)(byte)*puVar10 * 4), iVar9 != 0)) {
                  (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar12,iVar15);
                }
                uVar7 = uVar7 - 1;
                puVar8 = puVar8 + 1;
                puVar10 = (uint *)((int)puVar10 + 1);
                iVar12 = iVar12 + 1;
              } while (uVar7 != 0);
            }
          }
          iVar11 = iVar11 + 1;
          param_8 = (uint *)((int)param_8 + param_7);
          puStack_78 = (uint *)((int)puStack_78 + iStack_54);
        } while (iVar11 < param_5);
      }
      if (iStack_58 == 0) {
        return;
      }
    }
    else {
      if (param_10 == (undefined *)0x0) {
        param_10 = &DAT_00182284;
      }
      iStack_6c = 0;
      if (0 < param_5) {
        uStack_38 = 4 - param_9;
        uStack_3c = param_9 << 1;
        uStack_40 = 2 - param_9;
        uStack_48 = param_9 << 2;
        do {
          uVar7 = uStack_28;
          iVar11 = iStack_30;
          iVar9 = iStack_58;
          puVar8 = puStack_78;
          if (param_6 == 1) {
            piStack_4c = (int *)FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
            uVar1 = uStack_28;
            iVar11 = iStack_58;
            puVar8 = puStack_78;
            iStack_50 = iStack_6c + iStack_2c;
            puVar16 = (undefined *)(uint)(byte)(&DAT_00182184)[(byte)*param_8];
            bVar5 = *(byte *)(DAT_00182efc + 0x10) & 3;
            bVar17 = (*(byte *)(DAT_00182efc + 0x10) & 3) == 0;
            uVar7 = (uint)((byte)(&DAT_00182184)[(byte)*param_8] >> (param_9 & 0xff));
            if (bVar17) {
              puVar16 = &DAT_00182184;
            }
            uVar4 = 8 - param_9;
            puVar10 = param_8;
            if (bVar17) {
              while( true ) {
                if (uVar1 < uVar4) {
                  uVar4 = uVar1;
                }
                uVar1 = uVar1 - uVar4;
                puVar3 = puVar8;
                if (uVar4 < 0x80000000) {
                  if ((int)uVar4 < 2) {
                    uVar4 = 1;
                  }
                  puVar3 = puVar8 + -1;
                  if ((uVar4 & 1) != 0) {
                    uVar6 = uVar7 & 1;
                    uVar7 = uVar7 >> 1;
                    *puVar8 = *(uint *)(param_10 + uVar6 * 4);
                    puVar3 = puVar8;
                  }
                  for (iVar11 = (int)uVar4 >> 1; iVar11 != 0; iVar11 = iVar11 + -1) {
                    uVar13 = uVar7 & 1;
                    uVar6 = uVar7 >> 1;
                    uVar7 = uVar7 >> 2;
                    puVar3[1] = *(uint *)(param_10 + uVar13 * 4);
                    puVar3 = puVar3 + 2;
                    *puVar3 = *(uint *)(param_10 + (uVar6 & 1) * 4);
                  }
                  puVar8 = puVar8 + uVar4;
                }
                else {
                  do {
                    uVar6 = uVar7 & 1;
                    uVar4 = uVar4 - 1;
                    uVar7 = uVar7 >> 1;
                    puVar8 = puVar3 + 1;
                    *puVar3 = *(uint *)(param_10 + uVar6 * 4);
                    puVar3 = puVar8;
                  } while (uVar4 != 0);
                }
                if (uVar1 == 0) break;
                puVar10 = (uint *)((int)puVar10 + 1);
                uVar4 = 8;
                uVar7 = (uint)(byte)puVar16[*(byte *)puVar10];
              }
            }
            else {
              if (bVar5 != 1) {
                if (bVar5 == 2) {
                  uVar6 = *(uint *)(param_10 + 4);
                  puVar8 = puStack_78;
                  uVar1 = uStack_28;
                  iVar9 = iStack_30;
                  do {
                    if (uVar1 < uVar4) {
                      uVar4 = uVar1;
                    }
                    uVar1 = uVar1 - uVar4;
                    if (iVar11 == 0) {
                      for (; uVar7 != 0; uVar7 = uVar7 >> 1) {
                        if ((uVar7 & 1) != 0) {
                          *puVar8 = uVar6;
                        }
                        uVar4 = uVar4 - 1;
                        puVar8 = puVar8 + 1;
                        if (uVar4 == 0) goto LAB_0011d7ec;
                      }
                      puVar8 = puVar8 + uVar4;
                    }
                    else {
                      for (; uVar7 != 0; uVar7 = uVar7 >> 1) {
                        if ((uVar7 & 1) != 0) {
                          (*(code *)**(undefined4 **)(iVar11 + 8))(iVar11,iVar9,iStack_50);
                          *puVar8 = uVar6;
                        }
                        uVar4 = uVar4 - 1;
                        puVar8 = puVar8 + 1;
                        iVar9 = iVar9 + 1;
                        if (uVar4 == 0) goto LAB_0011d7ec;
                      }
                      puVar8 = puVar8 + uVar4;
                      iVar9 = iVar9 + uVar4;
                    }
LAB_0011d7ec:
                    if (uVar1 == 0) goto LAB_0011d884;
                    puVar10 = (uint *)((int)puVar10 + 1);
                    uVar4 = 8;
                    uVar7 = (uint)(byte)(&DAT_00182184)[*(byte *)puVar10];
                  } while( true );
                }
                if (bVar5 != 3) goto LAB_0011d884;
              }
              uVar6 = (**(code **)(*(int *)(*piStack_4c + 0x10) + 8))();
              while( true ) {
                if (uVar1 < uVar4) {
                  uVar4 = uVar1;
                }
                uVar1 = uVar1 - uVar4;
                if (uVar4 < 0x80000000) {
                  puVar3 = puVar8;
                  uVar13 = uVar4;
                  if ((int)uVar4 < 2) {
                    uVar4 = 1;
                    uVar13 = uVar4;
                  }
                  do {
                    if ((uVar7 & 1) != 0) {
                      *puVar3 = *puVar3 ^ uVar6;
                    }
                    uVar4 = uVar4 - 1;
                    uVar7 = uVar7 >> 1;
                    puVar3 = puVar3 + 1;
                  } while (uVar4 != 0);
                  puVar8 = puVar8 + uVar13;
                }
                else {
                  do {
                    if ((uVar7 & 1) != 0) {
                      *puVar8 = *puVar8 ^ uVar6;
                    }
                    uVar4 = uVar4 - 1;
                    uVar7 = uVar7 >> 1;
                    puVar8 = puVar8 + 1;
                  } while (uVar4 != 0);
                }
                if (uVar1 == 0) break;
                puVar10 = (uint *)((int)puVar10 + 1);
                uVar4 = 8;
                uVar7 = (uint)(byte)(&DAT_00182184)[*(byte *)puVar10];
              }
            }
LAB_0011d884:
            FUN_0010e334(&piStack_4c);
          }
          else if (param_6 == 2) {
            iStack_68 = iStack_6c + iStack_2c;
            uVar4 = uStack_38 & 0xff;
            bVar5 = *(byte *)(DAT_00182efc + 0x10) & 3;
            uVar7 = (uint)(byte)*param_8 << (uStack_3c & 0xff) & 0xff;
            uVar1 = uStack_28;
            puVar10 = param_8;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              while( true ) {
                if ((int)uVar1 < (int)uVar4) {
                  uVar4 = uVar1 & 0xff;
                }
                uVar1 = uVar1 - uVar4;
                if (uVar4 < 2) {
                  uVar4 = 1;
                }
                puVar3 = puVar8 + -1;
                if ((uVar4 & 1) != 0) {
                  uVar6 = uVar7 >> 6;
                  uVar7 = (uVar7 & 0x3f) << 2;
                  *puVar8 = *(uint *)(param_10 + uVar6 * 4);
                  puVar3 = puVar8;
                }
                for (iVar11 = (int)uVar4 >> 1; iVar11 != 0; iVar11 = iVar11 + -1) {
                  uVar6 = uVar7 & 0x3f;
                  puVar3[1] = *(uint *)(param_10 + (uVar7 >> 6) * 4);
                  uVar7 = (uVar7 & 0xf) << 4;
                  puVar3 = puVar3 + 2;
                  *puVar3 = *(uint *)(param_10 + (uVar6 >> 4) * 4);
                }
                puVar8 = puVar8 + uVar4;
                if (uVar1 == 0) break;
                puVar10 = (uint *)((int)puVar10 + 1);
                uVar7 = (uint)*(byte *)puVar10;
                uVar4 = 4;
              }
            }
            else if (bVar5 == 1) {
              while( true ) {
                if ((int)uVar1 < (int)uVar4) {
                  uVar4 = uVar1 & 0xff;
                }
                uVar1 = uVar1 - uVar4;
                puVar3 = puVar8;
                uVar6 = uVar4;
                if (uVar4 < 2) {
                  uVar4 = 1;
                  uVar6 = uVar4;
                }
                do {
                  if ((uVar7 & 0xc0) != 0) {
                    *puVar3 = *puVar3 ^ 0xff;
                  }
                  uVar4 = uVar4 - 1;
                  uVar7 = (uVar7 & 0x3f) << 2;
                  puVar3 = puVar3 + 1;
                } while (uVar4 != 0);
                puVar8 = puVar8 + uVar6;
                if (uVar1 == 0) break;
                puVar10 = (uint *)((int)puVar10 + 1);
                uVar7 = (uint)*(byte *)puVar10;
                uVar4 = 4;
              }
            }
            else if (bVar5 == 2) {
              while( true ) {
                if ((int)uVar1 < (int)uVar4) {
                  uVar4 = uVar1 & 0xff;
                }
                uVar1 = uVar1 - uVar4;
                do {
                  if (((uVar7 & 0xc0) != 0) &&
                     (*puVar8 = *(uint *)(param_10 + (uVar7 >> 6) * 4), iVar9 != 0)) {
                    (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar11,iStack_68);
                  }
                  uVar4 = uVar4 - 1 & 0xff;
                  uVar7 = (uVar7 & 0x3f) << 2;
                  puVar8 = puVar8 + 1;
                  iVar11 = iVar11 + 1;
                } while (uVar4 != 0);
                if (uVar1 == 0) break;
                puVar10 = (uint *)((int)puVar10 + 1);
                uVar7 = (uint)*(byte *)puVar10;
                uVar4 = 4;
              }
            }
          }
          else if (param_6 == 4) {
            iVar15 = iStack_6c + iStack_2c;
            uVar6 = uStack_40 & 0xff;
            uVar4 = (uint)(byte)*param_8 << (uStack_48 & 0xff);
            bVar5 = *(byte *)(DAT_00182efc + 0x10) & 3;
            uVar1 = uVar4 & 0xff;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
              puVar10 = puStack_78;
              puVar8 = param_8;
              if (param_9 != 0) {
                uVar7 = uStack_28 - 1;
                puVar10 = puStack_78 + 1;
                *puStack_78 = *(uint *)(param_10 + (uVar1 >> 4) * 4);
                puVar8 = (uint *)((int)param_8 + 1);
                uVar1 = (uint)*(byte *)puVar8;
              }
              if (1 < (int)uVar7) {
                iVar11 = (int)uVar7 >> 1;
                if (iVar11 < 2) {
                  iVar11 = 1;
                }
                uVar7 = uVar7 & 1;
                puVar3 = puVar10;
                iVar9 = iVar11;
                do {
                  puVar8 = (uint *)((int)puVar8 + 1);
                  iVar9 = iVar9 + -1;
                  puVar2 = puVar3 + 1;
                  *puVar3 = *(uint *)(param_10 + (uVar1 >> 4) * 4);
                  puVar3 = puVar3 + 2;
                  *puVar2 = *(uint *)(param_10 + (uVar1 & 0xf) * 4);
                  uVar1 = (uint)*(byte *)puVar8;
                } while (iVar9 != 0);
                puVar10 = puVar10 + iVar11 * 2;
              }
              if (uVar7 != 0) {
                *puVar10 = *(uint *)(param_10 + (uVar1 >> 4) * 4);
              }
            }
            else {
              puVar10 = param_8;
              if (bVar5 == 1) {
                while( true ) {
                  if ((int)uVar7 < (int)uVar6) {
                    uVar6 = uVar7 & 0xff;
                  }
                  uVar7 = uVar7 - uVar6;
                  puVar3 = puVar8;
                  uVar4 = uVar6;
                  if (uVar6 < 2) {
                    uVar6 = 1;
                    uVar4 = uVar6;
                  }
                  do {
                    if ((uVar1 & 0xc0) != 0) {
                      *puVar3 = *puVar3 ^ 0xff;
                    }
                    uVar6 = uVar6 - 1;
                    uVar1 = (uVar1 & 0xf) << 4;
                    puVar3 = puVar3 + 1;
                  } while (uVar6 != 0);
                  puVar8 = puVar8 + uVar4;
                  if (uVar7 == 0) break;
                  puVar10 = (uint *)((int)puVar10 + 1);
                  uVar1 = (uint)*(byte *)puVar10;
                  uVar6 = 2;
                }
              }
              else if (bVar5 == 2) {
                uVar6 = param_9;
                if (param_9 != 0) {
                  if ((uVar4 & 0xf0) != 0) {
                    uVar6 = *(uint *)(param_10 + (uVar1 >> 4) * 4);
                    *puStack_78 = uVar6;
                    if (iStack_58 != 0) {
                      uVar6 = (*(code *)**(undefined4 **)(iStack_58 + 8))
                                        (iStack_58,iStack_30,iVar15);
                    }
                  }
                  puVar10 = (uint *)((int)param_8 + 1);
                  uVar1 = (uint)*(byte *)puVar10;
                  puVar8 = puVar8 + 1;
                  uVar7 = uVar7 - 1;
                  iVar11 = iVar11 + 1;
                }
                for (; 1 < (int)uVar7; uVar7 = uVar7 - 2) {
                  if (((uVar1 & 0xf0) != 0) &&
                     (*puVar8 = *(uint *)(param_10 + (uVar1 >> 4) * 4), iVar9 != 0)) {
                    (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar11,iVar15);
                  }
                  uVar6 = 0;
                  if ((uVar1 & 0xf) != 0) {
                    uVar6 = *(uint *)(param_10 + (uVar1 & 0xf) * 4);
                    puVar8[1] = uVar6;
                    if (iVar9 != 0) {
                      uVar6 = (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar11 + 1,iVar15);
                    }
                  }
                  uVar1 = (uint)*(byte *)((int)puVar10 + 1);
                  puVar8 = puVar8 + 2;
                  puVar10 = (uint *)((int)puVar10 + 1);
                  iVar11 = iVar11 + 2;
                }
                bVar17 = uVar7 == 0;
                if (!bVar17) {
                  uVar6 = uVar1 >> 4;
                  bVar17 = uVar6 == 0;
                }
                if ((!bVar17) && (*puVar8 = *(uint *)(param_10 + uVar6 * 4), iVar9 != 0)) {
                  (*(code *)**(undefined4 **)(iVar9 + 8))(iVar9,iVar11,iVar15);
                }
              }
            }
          }
          iStack_6c = iStack_6c + 1;
          param_8 = (uint *)((int)param_8 + param_7);
          puStack_78 = (uint *)((int)puStack_78 + iStack_54);
        } while (iStack_6c < param_5);
      }
      if (iStack_58 == 0) {
        return;
      }
    }
  }
  else {
    iVar11 = 0;
    if (0 < param_5) {
      iStack_50 = uStack_28 << 2;
      do {
        uVar7 = uStack_28;
        puVar8 = puStack_78;
        if (iStack_58 != 0) {
          (**(code **)(*(int *)(iStack_58 + 8) + 4))
                    (iStack_58,iStack_30,iStack_2c + iVar11,uStack_28);
        }
        if (*(int *)(*(int *)(iStack_34 + 0x10) + 0xc) == 0) {
          uVar1 = (**(code **)(*(int *)(iStack_34 + 0x10) + 4))(0xff000000);
          puVar10 = param_8;
          do {
            iVar9 = 0;
            puVar3 = puVar10;
            uVar4 = uVar7;
            do {
              uVar4 = uVar4 - 1;
              if (*puVar3 >> 0x18 != uVar1 >> 0x18) break;
              iVar9 = iVar9 + 1;
              puVar3 = puVar3 + 1;
            } while (uVar4 != 0);
            if (iVar9 == 0) {
              if (*puVar10 >> 0x18 != 0) {
                uVar4 = (*DAT_00183040)(*puVar10,*puVar8);
                *puVar8 = uVar4;
              }
              iVar9 = -1;
              puVar10 = puVar10 + 1;
              puVar8 = puVar8 + 1;
            }
            else {
              (*DAT_00183074)(puVar8,puVar10,iVar9 << 2);
              puVar8 = puVar8 + iVar9;
              puVar10 = puVar10 + iVar9;
              iVar9 = -iVar9;
            }
            uVar7 = uVar7 + iVar9;
          } while (uVar7 != 0);
        }
        else {
          (*DAT_00183074)(puVar8,param_8,iStack_50);
        }
        iVar11 = iVar11 + 1;
        param_8 = (uint *)((int)param_8 + param_7);
        puStack_78 = (uint *)((int)puStack_78 + iStack_54);
      } while (iVar11 < param_5);
    }
    if (iStack_58 == 0) {
      return;
    }
  }
  FUN_0010e334(&iStack_58);
  return;
}

// ==== FUN_0011dd60 @ 0011dd60, 2640 bytes
void FUN_0011dd60(int param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,undefined *param_10)

{
  byte bVar1;
  byte *pbVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  byte *pbVar12;
  int iVar13;
  int iVar14;
  undefined *puVar15;
  bool bVar16;
  int local_7c [2];
  int local_74;
  int local_60;
  int local_5c;
  byte *local_58;
  int local_54;
  int local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  int local_34;
  int local_30;
  int local_2c;
  uint local_28;
  
  local_34 = param_1;
  local_30 = param_2;
  local_2c = param_3;
  local_28 = param_4;
  local_50 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  local_60 = 0;
  if (*(int *)(local_50 + 0x14) != 0) {
    local_60 = FUN_0010e24c();
  }
  local_54 = *(int *)(local_50 + 0xc);
  FUN_0010e334(&local_50);
  local_30 = param_9 + param_2;
  if ((local_60 != 0) && ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0)) {
    FUN_00113b58(local_60,local_30,local_2c,local_28);
  }
  iVar4 = local_2c;
  iVar13 = local_30;
  local_7c[0] = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
  pbVar6 = (byte *)(((iVar4 - *(short *)(local_7c[0] + 6)) * *(int *)(local_7c[0] + 0xc) +
                     local_7c[0] + 0x18 + iVar13) - (int)*(short *)(local_7c[0] + 4));
  FUN_0010e334(local_7c);
  local_58 = pbVar6;
  if (param_6 == 8) {
    iVar13 = 0;
    if (0 < param_5) {
      do {
        iVar4 = local_60;
        iVar14 = iVar13 + local_2c;
        if (param_10 == (undefined *)0x0) {
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            (*DAT_00183074)(local_58,param_8,local_28);
          }
          else {
            pbVar6 = param_8;
            pbVar12 = local_58;
            uVar8 = local_28;
            iVar11 = local_30;
            if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
              do {
                bVar1 = *pbVar6;
                if (bVar1 != 0) {
                  *pbVar12 = bVar1;
                }
                if (bVar1 != 0 && iVar4 != 0) {
                  (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar11,iVar14);
                }
                uVar8 = uVar8 - 1;
                pbVar6 = pbVar6 + 1;
                pbVar12 = pbVar12 + 1;
                iVar11 = iVar11 + 1;
              } while (uVar8 != 0);
            }
          }
        }
        else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
          uVar8 = local_28;
          if ((int)local_28 < 2) {
            uVar8 = 1;
          }
          pbVar6 = param_8 + -1;
          pbVar12 = local_58 + -1;
          if ((uVar8 & 1) != 0) {
            *local_58 = param_10[(uint)*param_8 * 4];
            pbVar6 = param_8;
            pbVar12 = local_58;
          }
          for (iVar4 = (int)uVar8 >> 1; iVar4 != 0; iVar4 = iVar4 + -1) {
            pbVar12[1] = param_10[(uint)pbVar6[1] * 4];
            pbVar6 = pbVar6 + 2;
            pbVar12 = pbVar12 + 2;
            *pbVar12 = param_10[(uint)*pbVar6 * 4];
          }
        }
        else {
          pbVar6 = local_58;
          uVar8 = local_28;
          pbVar12 = param_8;
          iVar11 = local_30;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            do {
              if ((*pbVar12 != 0) && (*pbVar6 = param_10[(uint)*pbVar12 * 4], iVar4 != 0)) {
                (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar11,iVar14);
              }
              uVar8 = uVar8 - 1;
              pbVar6 = pbVar6 + 1;
              pbVar12 = pbVar12 + 1;
              iVar11 = iVar11 + 1;
            } while (uVar8 != 0);
          }
        }
        iVar13 = iVar13 + 1;
        param_8 = param_8 + param_7;
        local_58 = local_58 + local_54;
      } while (iVar13 < param_5);
    }
    if (local_60 == 0) {
      return;
    }
  }
  else {
    if (param_10 == (undefined *)0x0) {
      param_10 = &DAT_001822f8;
    }
    local_5c = 0;
    if (0 < param_5) {
      local_40 = 4 - param_9;
      local_44 = param_9 << 1;
      local_48 = 2 - param_9;
      local_4c = param_9 << 2;
      do {
        uVar8 = local_28;
        iVar13 = local_30;
        pbVar6 = local_58;
        iVar4 = local_60;
        if (param_6 == 1) {
          local_50 = FUN_0010e24c(*(undefined4 *)(DAT_00182efc + 100));
          uVar8 = local_28;
          iVar13 = local_30;
          pbVar6 = local_58;
          iVar4 = local_60;
          iVar14 = local_5c + local_2c;
          puVar15 = (undefined *)(uint)*(byte *)(DAT_00182efc + 0x10);
          uVar9 = (uint)puVar15 & 3;
          bVar16 = (*(byte *)(DAT_00182efc + 0x10) & 3) == 0;
          if (bVar16) {
            puVar15 = &DAT_00182184;
          }
          uVar7 = (uint)((byte)(&DAT_00182184)[*param_8] >> (param_9 & 0xff));
          uVar5 = 8 - param_9;
          pbVar12 = param_8;
          if (bVar16) {
            while( true ) {
              if (uVar8 < uVar5) {
                uVar5 = uVar8;
              }
              uVar8 = uVar8 - uVar5;
              pbVar3 = pbVar6;
              if (uVar5 < 0x80000000) {
                if ((int)uVar5 < 2) {
                  uVar5 = 1;
                }
                pbVar3 = pbVar6 + -1;
                if ((uVar5 & 1) != 0) {
                  uVar9 = uVar7 & 1;
                  uVar7 = uVar7 >> 1;
                  *pbVar6 = param_10[uVar9 * 4];
                  pbVar3 = pbVar6;
                }
                for (iVar13 = (int)uVar5 >> 1; iVar13 != 0; iVar13 = iVar13 + -1) {
                  uVar10 = uVar7 & 1;
                  uVar9 = uVar7 >> 1;
                  uVar7 = uVar7 >> 2;
                  pbVar3[1] = param_10[uVar10 * 4];
                  pbVar3 = pbVar3 + 2;
                  *pbVar3 = param_10[(uVar9 & 1) * 4];
                }
                pbVar6 = pbVar6 + uVar5;
              }
              else {
                do {
                  uVar9 = uVar7 & 1;
                  uVar5 = uVar5 - 1;
                  uVar7 = uVar7 >> 1;
                  pbVar6 = pbVar3 + 1;
                  *pbVar3 = param_10[uVar9 * 4];
                  pbVar3 = pbVar6;
                } while (uVar5 != 0);
              }
              if (uVar8 == 0) break;
              pbVar12 = pbVar12 + 1;
              uVar5 = 8;
              uVar7 = (uint)(byte)puVar15[*pbVar12];
            }
          }
          else {
            if (uVar9 != 1) {
              if (uVar9 == 2) {
                bVar1 = param_10[4];
                pbVar6 = local_58;
                uVar8 = local_28;
                do {
                  if (uVar8 < uVar5) {
                    uVar5 = uVar8;
                  }
                  uVar8 = uVar8 - uVar5;
                  if (iVar4 == 0) {
                    for (; uVar7 != 0; uVar7 = uVar7 >> 1) {
                      if ((uVar7 & 1) != 0) {
                        *pbVar6 = bVar1;
                      }
                      uVar5 = uVar5 - 1;
                      pbVar6 = pbVar6 + 1;
                      if (uVar5 == 0) goto LAB_0011e200;
                    }
                    pbVar6 = pbVar6 + uVar5;
                  }
                  else {
                    for (; uVar7 != 0; uVar7 = uVar7 >> 1) {
                      if ((uVar7 & 1) != 0) {
                        (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13,iVar14);
                        *pbVar6 = bVar1;
                      }
                      uVar5 = uVar5 - 1;
                      pbVar6 = pbVar6 + 1;
                      iVar13 = iVar13 + 1;
                      if (uVar5 == 0) goto LAB_0011e200;
                    }
                    pbVar6 = pbVar6 + uVar5;
                    iVar13 = iVar13 + uVar5;
                  }
LAB_0011e200:
                  if (uVar8 == 0) goto LAB_0011e2f0;
                  pbVar12 = pbVar12 + 1;
                  uVar5 = 8;
                  uVar7 = (uint)(byte)(&DAT_00182184)[*pbVar12];
                } while( true );
              }
              if (uVar9 != 3) goto LAB_0011e2f0;
            }
            bVar1 = (**(code **)(*(int *)(local_34 + 0x10) + 8))();
            while( true ) {
              if (uVar8 < uVar5) {
                uVar5 = uVar8;
              }
              uVar8 = uVar8 - uVar5;
              if (iVar4 == 0) {
                if (uVar5 < 0x80000000) {
                  pbVar3 = pbVar6;
                  uVar9 = uVar5;
                  if ((int)uVar5 < 2) {
                    uVar5 = 1;
                    uVar9 = uVar5;
                  }
                  do {
                    if ((uVar7 & 1) != 0) {
                      *pbVar3 = *pbVar3 ^ bVar1;
                    }
                    uVar5 = uVar5 - 1;
                    uVar7 = uVar7 >> 1;
                    pbVar3 = pbVar3 + 1;
                  } while (uVar5 != 0);
                  pbVar6 = pbVar6 + uVar9;
                }
                else {
                  do {
                    if ((uVar7 & 1) != 0) {
                      *pbVar6 = *pbVar6 ^ bVar1;
                    }
                    uVar5 = uVar5 - 1;
                    uVar7 = uVar7 >> 1;
                    pbVar6 = pbVar6 + 1;
                  } while (uVar5 != 0);
                }
              }
              else {
                do {
                  if ((uVar7 & 1) != 0) {
                    (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13,iVar14);
                    *pbVar6 = *pbVar6 ^ bVar1;
                  }
                  uVar5 = uVar5 - 1;
                  uVar7 = uVar7 >> 1;
                  pbVar6 = pbVar6 + 1;
                  iVar13 = iVar13 + 1;
                } while (uVar5 != 0);
              }
              if (uVar8 == 0) break;
              pbVar12 = pbVar12 + 1;
              uVar5 = 8;
              uVar7 = (uint)(byte)(&DAT_00182184)[*pbVar12];
            }
          }
LAB_0011e2f0:
          FUN_0010e334(&local_50);
        }
        else if (param_6 == 2) {
          local_74 = local_5c + local_2c;
          uVar7 = local_40 & 0xff;
          bVar1 = *(byte *)(DAT_00182efc + 0x10) & 3;
          uVar8 = (uint)*param_8 << (local_44 & 0xff) & 0xff;
          uVar9 = local_28;
          pbVar12 = param_8;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            while( true ) {
              if ((int)uVar9 < (int)uVar7) {
                uVar7 = uVar9 & 0xff;
              }
              uVar9 = uVar9 - uVar7;
              if (uVar7 < 2) {
                uVar7 = 1;
              }
              pbVar3 = pbVar6 + -1;
              if ((uVar7 & 1) != 0) {
                uVar5 = uVar8 >> 6;
                uVar8 = (uVar8 & 0x3f) << 2;
                *pbVar6 = param_10[uVar5 * 4];
                pbVar3 = pbVar6;
              }
              for (iVar13 = (int)uVar7 >> 1; iVar13 != 0; iVar13 = iVar13 + -1) {
                uVar5 = uVar8 & 0x3f;
                pbVar3[1] = param_10[(uVar8 >> 6) * 4];
                uVar8 = (uVar8 & 0xf) << 4;
                pbVar3 = pbVar3 + 2;
                *pbVar3 = param_10[(uVar5 >> 4) * 4];
              }
              pbVar6 = pbVar6 + uVar7;
              if (uVar9 == 0) break;
              pbVar12 = pbVar12 + 1;
              uVar8 = (uint)*pbVar12;
              uVar7 = 4;
            }
          }
          else if (bVar1 == 1) {
            while( true ) {
              if ((int)uVar9 < (int)uVar7) {
                uVar7 = uVar9 & 0xff;
              }
              uVar9 = uVar9 - uVar7;
              pbVar3 = pbVar6;
              uVar5 = uVar7;
              if (uVar7 < 2) {
                uVar7 = 1;
                uVar5 = uVar7;
              }
              do {
                if ((uVar8 & 0xc0) != 0) {
                  *pbVar3 = *pbVar3 ^ 0xff;
                }
                uVar7 = uVar7 - 1;
                uVar8 = (uVar8 & 0x3f) << 2;
                pbVar3 = pbVar3 + 1;
              } while (uVar7 != 0);
              pbVar6 = pbVar6 + uVar5;
              if (uVar9 == 0) break;
              pbVar12 = pbVar12 + 1;
              uVar8 = (uint)*pbVar12;
              uVar7 = 4;
            }
          }
          else if (bVar1 == 2) {
            while( true ) {
              if ((int)uVar9 < (int)uVar7) {
                uVar7 = uVar9 & 0xff;
              }
              uVar9 = uVar9 - uVar7;
              do {
                if (((uVar8 & 0xc0) != 0) && (*pbVar6 = param_10[(uVar8 >> 6) * 4], iVar4 != 0)) {
                  (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13,local_74);
                }
                uVar7 = uVar7 - 1 & 0xff;
                uVar8 = (uVar8 & 0x3f) << 2;
                pbVar6 = pbVar6 + 1;
                iVar13 = iVar13 + 1;
              } while (uVar7 != 0);
              if (uVar9 == 0) break;
              pbVar12 = pbVar12 + 1;
              uVar8 = (uint)*pbVar12;
              uVar7 = 4;
            }
          }
        }
        else if (param_6 == 4) {
          iVar14 = local_5c + local_2c;
          uVar5 = local_48 & 0xff;
          uVar7 = (uint)*param_8 << (local_4c & 0xff);
          bVar1 = *(byte *)(DAT_00182efc + 0x10) & 3;
          uVar9 = uVar7 & 0xff;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            pbVar12 = local_58;
            pbVar6 = param_8;
            if (param_9 != 0) {
              uVar8 = local_28 - 1;
              pbVar12 = local_58 + 1;
              *local_58 = param_10[(uVar9 >> 4) * 4];
              pbVar6 = param_8 + 1;
              uVar9 = (uint)*pbVar6;
            }
            if (1 < (int)uVar8) {
              iVar13 = (int)uVar8 >> 1;
              if (iVar13 < 2) {
                iVar13 = 1;
              }
              uVar8 = uVar8 & 1;
              pbVar3 = pbVar12;
              iVar4 = iVar13;
              do {
                pbVar6 = pbVar6 + 1;
                iVar4 = iVar4 + -1;
                pbVar2 = pbVar3 + 1;
                *pbVar3 = param_10[(uVar9 >> 4) * 4];
                pbVar3 = pbVar3 + 2;
                *pbVar2 = param_10[(uVar9 & 0xf) * 4];
                uVar9 = (uint)*pbVar6;
              } while (iVar4 != 0);
              pbVar12 = pbVar12 + iVar13 * 2;
            }
            if (uVar8 != 0) {
              *pbVar12 = param_10[(uVar9 >> 4) * 4];
            }
          }
          else {
            pbVar12 = param_8;
            if (bVar1 == 1) {
              while( true ) {
                if ((int)uVar8 < (int)uVar5) {
                  uVar5 = uVar8 & 0xff;
                }
                uVar8 = uVar8 - uVar5;
                pbVar3 = pbVar6;
                uVar7 = uVar5;
                if (uVar5 < 2) {
                  uVar5 = 1;
                  uVar7 = uVar5;
                }
                do {
                  if ((uVar9 & 0xc0) != 0) {
                    *pbVar3 = *pbVar3 ^ 0xff;
                  }
                  uVar5 = uVar5 - 1;
                  uVar9 = (uVar9 & 0xf) << 4;
                  pbVar3 = pbVar3 + 1;
                } while (uVar5 != 0);
                pbVar6 = pbVar6 + uVar7;
                if (uVar8 == 0) break;
                pbVar12 = pbVar12 + 1;
                uVar9 = (uint)*pbVar12;
                uVar5 = 2;
              }
            }
            else if (bVar1 == 2) {
              uVar5 = param_9;
              if (param_9 != 0) {
                if ((uVar7 & 0xf0) != 0) {
                  uVar5 = (uint)(byte)param_10[(uVar9 >> 4) * 4];
                  *local_58 = param_10[(uVar9 >> 4) * 4];
                  if (local_60 != 0) {
                    uVar5 = (*(code *)**(undefined4 **)(local_60 + 8))(local_60,local_30,iVar14);
                  }
                }
                pbVar12 = param_8 + 1;
                uVar9 = (uint)*pbVar12;
                uVar8 = uVar8 - 1;
                pbVar6 = pbVar6 + 1;
                iVar13 = iVar13 + 1;
              }
              for (; 1 < (int)uVar8; uVar8 = uVar8 - 2) {
                if (((uVar9 & 0xf0) != 0) && (*pbVar6 = param_10[(uVar9 >> 4) * 4], iVar4 != 0)) {
                  (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13,iVar14);
                }
                uVar5 = 0;
                if ((uVar9 & 0xf) != 0) {
                  uVar5 = (uint)(byte)param_10[(uVar9 & 0xf) * 4];
                  pbVar6[1] = param_10[(uVar9 & 0xf) * 4];
                  if (iVar4 != 0) {
                    uVar5 = (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13 + 1,iVar14);
                  }
                }
                uVar9 = (uint)pbVar12[1];
                pbVar6 = pbVar6 + 2;
                pbVar12 = pbVar12 + 1;
                iVar13 = iVar13 + 2;
              }
              bVar16 = uVar8 == 0;
              if (!bVar16) {
                uVar5 = uVar9 >> 4;
                bVar16 = uVar5 == 0;
              }
              if ((!bVar16) && (*pbVar6 = param_10[uVar5 * 4], iVar4 != 0)) {
                (*(code *)**(undefined4 **)(iVar4 + 8))(iVar4,iVar13,iVar14);
              }
            }
          }
        }
        local_5c = local_5c + 1;
        param_8 = param_8 + param_7;
        local_58 = local_58 + local_54;
      } while (local_5c < param_5);
    }
    if (local_60 == 0) {
      return;
    }
  }
  FUN_0010e334(&local_60);
  return;
}

// ==== FUN_0011e7c0 @ 0011e7c0, 2192 bytes
/* WARNING: Restarted to delay deadcode elimination for space: stack */

void FUN_0011e7c0(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,int param_10)

{
  int iVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  byte *pbVar7;
  int iVar8;
  uint uVar9;
  code *unaff_r11;
  code *pcVar10;
  int local_3c;
  
  if (param_6 == 8) {
    iVar3 = 0;
    if (param_5 < 1) {
      return;
    }
    do {
      iVar6 = param_3 + iVar3;
      pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
      if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
        pbVar7 = param_8;
        iVar8 = param_4;
        iVar1 = param_2;
        if (param_10 == 0) {
          for (; 0 < iVar8; iVar8 = iVar8 + -1) {
            (*pcVar10)(param_1,iVar1,iVar6,*pbVar7);
            pbVar7 = pbVar7 + 1;
            iVar1 = iVar1 + 1;
          }
        }
        else {
          for (; 0 < iVar8; iVar8 = iVar8 + -1) {
            (*pcVar10)(param_1,iVar1,iVar6,*(undefined4 *)(param_10 + (uint)*pbVar7 * 4));
            pbVar7 = pbVar7 + 1;
            iVar1 = iVar1 + 1;
          }
        }
      }
      else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
        iVar8 = param_4;
        pbVar7 = param_8;
        iVar1 = param_2;
        if (param_10 == 0) {
          for (; 0 < iVar8; iVar8 = iVar8 + -1) {
            if (*pbVar7 != 0) {
              (*pcVar10)(param_1,iVar1,iVar6);
            }
            pbVar7 = pbVar7 + 1;
            iVar1 = iVar1 + 1;
          }
        }
        else {
          for (; 0 < iVar8; iVar8 = iVar8 + -1) {
            if (*pbVar7 != 0) {
              (*pcVar10)(param_1,iVar1,iVar6,*(undefined4 *)(param_10 + (uint)*pbVar7 * 4));
            }
            pbVar7 = pbVar7 + 1;
            iVar1 = iVar1 + 1;
          }
        }
      }
      iVar3 = iVar3 + 1;
      param_8 = param_8 + param_7;
    } while (iVar3 < param_5);
    return;
  }
  if (8 < param_6) {
    if (param_6 == 0x10) {
      iVar3 = 0;
      if (param_5 < 1) {
        return;
      }
      do {
        pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
        pbVar7 = param_8;
        iVar8 = param_2;
        for (iVar6 = param_4; 0 < iVar6; iVar6 = iVar6 + -1) {
          (*pcVar10)(param_1,iVar8,param_3 + iVar3,*(undefined2 *)pbVar7);
          pbVar7 = pbVar7 + 2;
          iVar8 = iVar8 + 1;
        }
        iVar3 = iVar3 + 1;
        param_8 = param_8 + param_7;
      } while (iVar3 < param_5);
      return;
    }
    if (param_6 != 0x20) {
      return;
    }
    iVar3 = 0;
    if (param_5 < 1) {
      return;
    }
    do {
      pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
      pbVar7 = param_8;
      iVar8 = param_2;
      for (iVar6 = param_4; 0 < iVar6; iVar6 = iVar6 + -1) {
        (*pcVar10)(param_1,iVar8,param_3 + iVar3,*(undefined4 *)pbVar7);
        pbVar7 = pbVar7 + 4;
        iVar8 = iVar8 + 1;
      }
      iVar3 = iVar3 + 1;
      param_8 = param_8 + param_7;
    } while (iVar3 < param_5);
    return;
  }
  param_2 = param_9 + param_2;
  if (param_6 != 1) {
    if (param_6 == 2) {
      local_3c = 0;
      if (param_5 < 1) {
        return;
      }
      do {
        iVar3 = local_3c + param_3;
        uVar9 = (uint)*param_8;
        pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
        if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
          uVar5 = param_9;
          iVar6 = param_2;
          pbVar7 = param_8;
          iVar8 = param_4;
          if (param_10 == 0) {
            do {
              (*pcVar10)(param_1,iVar6,iVar3,
                         (0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar9) >>
                         ((3 - uVar5) * 2 & 0xff));
              uVar5 = uVar5 + 1;
              if (uVar5 == 4) {
                pbVar7 = pbVar7 + 1;
                uVar9 = (uint)*pbVar7;
                uVar5 = 0;
              }
              iVar8 = iVar8 + -1;
              iVar6 = iVar6 + 1;
            } while (iVar8 != 0);
          }
          else {
            do {
              (*pcVar10)(param_1,iVar6,iVar3,
                         *(undefined4 *)
                          (param_10 +
                          ((0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar9) >>
                          ((3 - uVar5) * 2 & 0xff)) * 4));
              uVar5 = uVar5 + 1;
              if (uVar5 == 4) {
                pbVar7 = pbVar7 + 1;
                uVar9 = (uint)*pbVar7;
                uVar5 = 0;
              }
              iVar8 = iVar8 + -1;
              iVar6 = iVar6 + 1;
            } while (iVar8 != 0);
          }
        }
        else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
          uVar5 = param_9;
          iVar6 = param_2;
          pbVar7 = param_8;
          iVar8 = param_4;
          if (param_10 == 0) {
            do {
              if ((0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar9) >> ((3 - uVar5) * 2 & 0xff) != 0)
              {
                (*pcVar10)(param_1,iVar6,iVar3);
              }
              uVar5 = uVar5 + 1;
              if (uVar5 == 4) {
                pbVar7 = pbVar7 + 1;
                uVar9 = (uint)*pbVar7;
                uVar5 = 0;
              }
              iVar8 = iVar8 + -1;
              iVar6 = iVar6 + 1;
            } while (iVar8 != 0);
          }
          else {
            do {
              uVar4 = (0xc0U >> ((3 - uVar5) * -2 + 6 & 0xff) & uVar9) >> ((3 - uVar5) * 2 & 0xff);
              if (uVar4 != 0) {
                (*pcVar10)(param_1,iVar6,iVar3,*(undefined4 *)(param_10 + uVar4 * 4));
              }
              uVar5 = uVar5 + 1;
              iVar6 = iVar6 + 1;
              if (uVar5 == 4) {
                pbVar7 = pbVar7 + 1;
                uVar9 = (uint)*pbVar7;
                uVar5 = 0;
              }
              iVar8 = iVar8 + -1;
            } while (iVar8 != 0);
          }
        }
        local_3c = local_3c + 1;
        param_8 = param_8 + param_7;
      } while (local_3c < param_5);
      return;
    }
    if (param_6 != 4) {
      return;
    }
    local_3c = 0;
    if (param_5 < 1) {
      return;
    }
    do {
      iVar3 = local_3c + param_3;
      uVar9 = (uint)*param_8;
      pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
      if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
        uVar5 = param_9;
        iVar6 = param_2;
        pbVar7 = param_8;
        iVar8 = param_4;
        if (param_10 == 0) {
          do {
            (*pcVar10)(param_1,iVar6,iVar3,
                       (0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar9) >> ((1 - uVar5) * 4 & 0xff))
            ;
            uVar5 = uVar5 + 1;
            if (uVar5 == 2) {
              pbVar7 = pbVar7 + 1;
              uVar9 = (uint)*pbVar7;
              uVar5 = 0;
            }
            iVar8 = iVar8 + -1;
            iVar6 = iVar6 + 1;
          } while (iVar8 != 0);
        }
        else {
          do {
            (*pcVar10)(param_1,iVar6,iVar3,
                       *(undefined4 *)
                        (param_10 +
                        ((0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar9) >>
                        ((1 - uVar5) * 4 & 0xff)) * 4));
            uVar5 = uVar5 + 1;
            if (uVar5 == 2) {
              pbVar7 = pbVar7 + 1;
              uVar9 = (uint)*pbVar7;
              uVar5 = 0;
            }
            iVar8 = iVar8 + -1;
            iVar6 = iVar6 + 1;
          } while (iVar8 != 0);
        }
      }
      else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
        uVar5 = param_9;
        iVar6 = param_2;
        pbVar7 = param_8;
        iVar8 = param_4;
        if (param_10 == 0) {
          do {
            if ((0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar9) >> ((1 - uVar5) * 4 & 0xff) != 0) {
              (*pcVar10)(param_1,iVar6,iVar3);
            }
            uVar5 = uVar5 + 1;
            if (uVar5 == 2) {
              pbVar7 = pbVar7 + 1;
              uVar9 = (uint)*pbVar7;
              uVar5 = 0;
            }
            iVar8 = iVar8 + -1;
            iVar6 = iVar6 + 1;
          } while (iVar8 != 0);
        }
        else {
          do {
            uVar4 = (0xf0U >> ((1 - uVar5) * -4 + 4 & 0xff) & uVar9) >> ((1 - uVar5) * 4 & 0xff);
            if (uVar4 != 0) {
              (*pcVar10)(param_1,iVar6,iVar3,*(undefined4 *)(param_10 + uVar4 * 4));
            }
            uVar5 = uVar5 + 1;
            iVar6 = iVar6 + 1;
            if (uVar5 == 2) {
              pbVar7 = pbVar7 + 1;
              uVar9 = (uint)*pbVar7;
              uVar5 = 0;
            }
            iVar8 = iVar8 + -1;
          } while (iVar8 != 0);
        }
      }
      local_3c = local_3c + 1;
      param_8 = param_8 + param_7;
    } while (local_3c < param_5);
    return;
  }
  local_3c = 0;
  if (param_5 < 1) {
    return;
  }
  do {
    iVar3 = local_3c + param_3;
    pcVar10 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
    if (param_10 != 0) {
      unaff_r11 = *(code **)(param_10 + 4);
    }
    if (param_10 == 0) {
      unaff_r11 = (code *)&DAT_00000001;
    }
    bVar2 = *(byte *)(DAT_00182efc + 0x10) & 3;
    uVar9 = param_9;
    iVar6 = param_2;
    iVar8 = param_4;
    if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
      do {
        (*pcVar10)(param_1,iVar6,iVar3);
        uVar9 = uVar9 + 1;
        if (uVar9 == 8) {
          uVar9 = 0;
        }
        iVar8 = iVar8 + -1;
        iVar6 = iVar6 + 1;
      } while (iVar8 != 0);
    }
    else if (bVar2 == 1) {
LAB_0011ea60:
      unaff_r11 = *(code **)(*(int *)(param_1 + 0xc) + 0x14);
      uVar5 = (**(code **)(*(int *)(param_1 + 0x10) + 8))();
      pbVar7 = param_8;
      do {
        if (((uint)*pbVar7 & 0x80U >> (uVar9 & 0xff)) != 0) {
          uVar4 = (*unaff_r11)(param_1,iVar6,iVar3);
          (*pcVar10)(param_1,iVar6,iVar3,uVar4 ^ uVar5);
        }
        uVar9 = uVar9 + 1;
        if (uVar9 == 8) {
          uVar9 = 0;
          pbVar7 = pbVar7 + 1;
        }
        iVar8 = iVar8 + -1;
        iVar6 = iVar6 + 1;
      } while (iVar8 != 0);
    }
    else {
      pbVar7 = param_8;
      if (bVar2 == 2) {
        do {
          if (((uint)*pbVar7 & 0x80U >> (uVar9 & 0xff)) != 0) {
            (*pcVar10)(param_1,iVar6,iVar3,unaff_r11);
          }
          uVar9 = uVar9 + 1;
          if (uVar9 == 8) {
            uVar9 = 0;
            pbVar7 = pbVar7 + 1;
          }
          iVar8 = iVar8 + -1;
          iVar6 = iVar6 + 1;
        } while (iVar8 != 0);
      }
      else if (bVar2 == 3) goto LAB_0011ea60;
    }
    local_3c = local_3c + 1;
    param_8 = param_8 + param_7;
    if (param_5 <= local_3c) {
      return;
    }
  } while( true );
}

// ==== FUN_0011f054 @ 0011f054, 2880 bytes
void FUN_0011f054(int *param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,byte *param_8,uint param_9,undefined4 *param_10)

{
  uint uVar1;
  byte bVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  byte bVar5;
  ushort uVar6;
  ushort uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined2 *puVar10;
  int iVar11;
  int iVar12;
  ushort *puVar13;
  int iVar14;
  undefined4 *puVar15;
  int iVar16;
  uint uVar17;
  ushort *puVar18;
  byte *pbVar19;
  int iVar20;
  uint uVar21;
  undefined4 *puVar22;
  uint uVar23;
  int iVar24;
  uint unaff_r11;
  int local_7c;
  int local_78;
  int local_74;
  int local_70;
  int local_6c;
  int *local_68;
  code *local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int *local_34;
  int iStack_30;
  int local_2c;
  uint local_28;
  
  local_3c = param_9 + param_2;
  iVar14 = param_4 - 1;
  puVar15 = (undefined4 *)param_1[2];
  iVar12 = iVar14 >> 1;
  local_34 = param_1;
  iStack_30 = param_2;
  local_2c = param_3;
  local_28 = param_4;
  if (param_6 == 8) {
    iVar20 = 0;
    if (0 < param_5) {
      do {
        uVar21 = local_28;
        puVar22 = (undefined4 *)local_34[2];
        (*(code *)*puVar22)(puVar22,param_2,iVar20 + local_2c,&local_48,&local_58);
        puVar18 = (ushort *)
                  (local_48 * *(int *)(puVar22[10] + 0x1c) + local_58 * puVar22[3] + puVar22[1]);
        if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
          if (param_10 == (undefined4 *)0x0) {
            if ((int)uVar21 < 2) {
              iVar16 = 0;
            }
            else {
              if (0 < iVar14) {
                puVar13 = puVar18 + -puVar22[9];
                pbVar19 = param_8 + -1;
                if ((uVar21 & 1) == 0) {
                  puVar13 = puVar13 + puVar22[9];
                  *puVar13 = (ushort)*param_8;
                  pbVar19 = param_8;
                }
                bVar5 = pbVar19[1];
                for (iVar16 = iVar12; iVar16 != 0; iVar16 = iVar16 + -1) {
                  bVar2 = pbVar19[2];
                  puVar13[puVar22[9]] = (ushort)bVar5;
                  bVar5 = pbVar19[3];
                  puVar13 = puVar13 + puVar22[9] * 2;
                  *puVar13 = (ushort)bVar2;
                  pbVar19 = pbVar19 + 2;
                }
              }
              iVar16 = uVar21 - 1;
              if (iVar16 < 0) goto LAB_0011f824;
            }
            puVar18[puVar22[9] * iVar16] = (ushort)param_8[iVar16];
          }
          else {
            if ((int)uVar21 < 2) {
              iVar16 = 0;
            }
            else {
              if (0 < iVar14) {
                puVar13 = puVar18 + -puVar22[9];
                pbVar19 = param_8 + -1;
                if ((uVar21 & 1) == 0) {
                  puVar13 = puVar13 + puVar22[9];
                  *puVar13 = (ushort)param_10[*param_8];
                  pbVar19 = param_8;
                }
                bVar5 = pbVar19[1];
                for (iVar16 = iVar12; iVar16 != 0; iVar16 = iVar16 + -1) {
                  bVar2 = pbVar19[2];
                  puVar13[puVar22[9]] = (ushort)param_10[bVar5];
                  bVar5 = pbVar19[3];
                  puVar13 = puVar13 + puVar22[9] * 2;
                  *puVar13 = (ushort)param_10[bVar2];
                  pbVar19 = pbVar19 + 2;
                }
              }
              iVar16 = uVar21 - 1;
              if (iVar16 < 0) goto LAB_0011f824;
            }
            puVar18[iVar16 * puVar22[9]] = (ushort)param_10[param_8[iVar16]];
          }
        }
        else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
          if (param_10 == (undefined4 *)0x0) {
            pbVar19 = param_8;
            if ((int)uVar21 < 2) {
              uVar21 = 1;
            }
            do {
              if (*pbVar19 != 0) {
                *puVar18 = (ushort)*pbVar19;
              }
              uVar21 = uVar21 - 1;
              puVar18 = puVar18 + puVar22[9];
              pbVar19 = pbVar19 + 1;
            } while (uVar21 != 0);
          }
          else {
            pbVar19 = param_8;
            if ((int)uVar21 < 2) {
              uVar21 = 1;
            }
            do {
              if (*pbVar19 != 0) {
                *puVar18 = (ushort)param_10[*pbVar19];
              }
              uVar21 = uVar21 - 1;
              puVar18 = puVar18 + puVar22[9];
              pbVar19 = pbVar19 + 1;
            } while (uVar21 != 0);
          }
        }
LAB_0011f824:
        iVar20 = iVar20 + 1;
        param_8 = param_8 + param_7;
      } while (iVar20 < param_5);
    }
  }
  else if (param_6 < 9) {
    if (param_6 == 1) {
      local_60 = 0;
      if (0 < param_5) {
        do {
          uVar21 = local_28;
          iVar12 = local_3c;
          iVar14 = local_60 + local_2c;
          local_68 = local_34;
          puVar22 = (undefined4 *)local_34[2];
          uVar8 = *param_10;
          uVar9 = param_10[1];
          (*(code *)*puVar22)(puVar22,local_3c,iVar14,&local_4c,&local_5c);
          bVar5 = *(byte *)(DAT_00182efc + 0x10) & 3;
          puVar18 = (ushort *)
                    (local_4c * *(int *)(puVar22[10] + 0x1c) + local_5c * puVar22[3] + puVar22[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar23 = param_9;
            pbVar19 = param_8;
            if ((int)uVar21 < 2) {
              uVar21 = 1;
            }
            do {
              uVar6 = (ushort)uVar9;
              if (((uint)*pbVar19 & 0x80U >> (uVar23 & 0xff)) == 0) {
                uVar6 = (ushort)uVar8;
              }
              uVar23 = uVar23 + 1;
              *puVar18 = uVar6;
              if (uVar23 == 8) {
                uVar23 = 0;
                pbVar19 = pbVar19 + 1;
              }
              uVar21 = uVar21 - 1;
              puVar18 = puVar18 + puVar22[9];
            } while (uVar21 != 0);
          }
          else if (bVar5 == 1) {
LAB_0011f234:
            local_64 = *(code **)(local_68[3] + 0x14);
            uVar6 = (**(code **)(local_68[4] + 8))();
            uVar23 = param_9;
            pbVar19 = param_8;
            do {
              if (((uint)*pbVar19 & 0x80U >> (uVar23 & 0xff)) != 0) {
                uVar7 = (*local_64)(local_68,iVar12,iVar14);
                *puVar18 = uVar7 ^ uVar6;
              }
              uVar23 = uVar23 + 1;
              if (uVar23 == 8) {
                uVar23 = 0;
                pbVar19 = pbVar19 + 1;
              }
              uVar21 = uVar21 - 1;
              puVar18 = puVar18 + puVar22[9];
              iVar12 = iVar12 + 1;
            } while (uVar21 != 0);
          }
          else {
            uVar23 = param_9;
            pbVar19 = param_8;
            if (bVar5 == 2) {
              do {
                if (((uint)*pbVar19 & 0x80U >> (uVar23 & 0xff)) != 0) {
                  *puVar18 = (ushort)uVar9;
                }
                uVar23 = uVar23 + 1;
                if (uVar23 == 8) {
                  uVar23 = 0;
                  pbVar19 = pbVar19 + 1;
                }
                uVar21 = uVar21 - 1;
                puVar18 = puVar18 + puVar22[9];
              } while (uVar21 != 0);
            }
            else if (bVar5 == 3) goto LAB_0011f234;
          }
          local_60 = local_60 + 1;
          param_8 = param_8 + param_7;
        } while (local_60 < param_5);
      }
    }
    else if (param_6 == 2) {
      if (0 < param_5) {
        unaff_r11 = 0xc0;
      }
      local_60 = 0;
      if (0 < param_5) {
        do {
          uVar21 = local_28;
          puVar22 = (undefined4 *)local_34[2];
          uVar23 = (uint)*param_8;
          (*(code *)*puVar22)(puVar22,local_3c,local_60 + local_2c,&local_38,&local_70);
          puVar10 = (undefined2 *)
                    (*(int *)(puVar22[10] + 0x1c) * local_38 + local_70 * puVar22[3] + puVar22[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar17 = param_9;
            pbVar19 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                iVar12 = 3 - uVar17;
                uVar17 = uVar17 + 1;
                *puVar10 = (short)((unaff_r11 >> (iVar12 * -2 + 6U & 0xff) & uVar23) >>
                                  (iVar12 * 2 & 0xffU));
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 4) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
            else {
              do {
                iVar12 = 3 - uVar17;
                uVar17 = uVar17 + 1;
                *puVar10 = (short)param_10[(unaff_r11 >> (iVar12 * -2 + 6U & 0xff) & uVar23) >>
                                           (iVar12 * 2 & 0xffU)];
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 4) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar17 = param_9;
            pbVar19 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar17) * -2 + 6 & 0xff) & uVar23) >>
                        ((3 - uVar17) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar10 = (short)uVar1;
                }
                uVar17 = uVar17 + 1;
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 4) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar17) * -2 + 6 & 0xff) & uVar23) >>
                        ((3 - uVar17) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar10 = (short)param_10[uVar1];
                }
                uVar17 = uVar17 + 1;
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 4) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
          }
          local_60 = local_60 + 1;
          param_8 = param_8 + param_7;
        } while (local_60 < param_5);
      }
    }
    else if (param_6 == 4) {
      if (0 < param_5) {
        unaff_r11 = 0xf0;
      }
      local_60 = 0;
      if (0 < param_5) {
        do {
          uVar21 = local_28;
          puVar22 = (undefined4 *)local_34[2];
          uVar23 = (uint)*param_8;
          (*(code *)*puVar22)(puVar22,local_3c,local_60 + local_2c,&local_38,&local_64);
          puVar10 = (undefined2 *)
                    (*(int *)(puVar22[10] + 0x1c) * local_38 +
                    (int)local_64 * puVar22[3] + puVar22[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar17 = param_9;
            pbVar19 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                iVar12 = 1 - uVar17;
                uVar17 = uVar17 + 1;
                *puVar10 = (short)((unaff_r11 >> (iVar12 * -4 + 4U & 0xff) & uVar23) >>
                                  (iVar12 * 4 & 0xffU));
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 2) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
            else {
              do {
                iVar12 = 1 - uVar17;
                uVar17 = uVar17 + 1;
                *puVar10 = (short)param_10[(unaff_r11 >> (iVar12 * -4 + 4U & 0xff) & uVar23) >>
                                           (iVar12 * 4 & 0xffU)];
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 2) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar17 = param_9;
            pbVar19 = param_8;
            if (param_10 == (undefined4 *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar17) * -4 + 4 & 0xff) & uVar23) >>
                        ((1 - uVar17) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar10 = (short)uVar1;
                }
                uVar17 = uVar17 + 1;
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 2) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar17) * -4 + 4 & 0xff) & uVar23) >>
                        ((1 - uVar17) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar10 = (short)param_10[uVar1];
                }
                uVar17 = uVar17 + 1;
                puVar10 = puVar10 + puVar22[9];
                if (uVar17 == 2) {
                  pbVar19 = pbVar19 + 1;
                  uVar23 = (uint)*pbVar19;
                  uVar17 = 0;
                }
                uVar21 = uVar21 - 1;
              } while (uVar21 != 0);
            }
          }
          local_60 = local_60 + 1;
          param_8 = param_8 + param_7;
        } while (local_60 < param_5);
      }
    }
  }
  else if (param_6 == 0x10) {
    iVar20 = 0;
    if (0 < param_5) {
      do {
        uVar21 = local_28;
        puVar22 = (undefined4 *)local_34[2];
        (*(code *)*puVar22)(puVar22,param_2,iVar20 + local_2c,&local_44,&local_54);
        iVar16 = puVar22[9];
        iVar24 = local_44 * *(int *)(puVar22[10] + 0x1c) + local_54 * puVar22[3] + puVar22[1];
        if ((int)uVar21 < 2) {
          iVar11 = 0;
LAB_0011f970:
          *(undefined2 *)(iVar24 + iVar16 * iVar11 * 2) = *(undefined2 *)(param_8 + iVar11 * 2);
        }
        else {
          if (0 < iVar14) {
            puVar10 = (undefined2 *)(iVar24 + iVar16 * -2);
            pbVar19 = param_8 + -2;
            if ((uVar21 & 1) == 0) {
              puVar10 = puVar10 + iVar16;
              *puVar10 = *(undefined2 *)param_8;
              pbVar19 = param_8;
            }
            uVar4 = *(undefined2 *)(pbVar19 + 2);
            for (iVar11 = iVar12; iVar11 != 0; iVar11 = iVar11 + -1) {
              uVar3 = *(undefined2 *)(pbVar19 + 4);
              puVar10[iVar16] = uVar4;
              uVar4 = *(undefined2 *)(pbVar19 + 6);
              puVar10 = puVar10 + iVar16 * 2;
              *puVar10 = uVar3;
              pbVar19 = pbVar19 + 4;
            }
          }
          iVar11 = uVar21 - 1;
          if (-1 < iVar11) goto LAB_0011f970;
        }
        iVar20 = iVar20 + 1;
        param_8 = param_8 + param_7;
      } while (iVar20 < param_5);
    }
  }
  else if ((param_6 == 0x20) && (iVar20 = 0, 0 < param_5)) {
    do {
      uVar21 = local_28;
      puVar22 = (undefined4 *)local_34[2];
      (*(code *)*puVar22)(puVar22,param_2,iVar20 + local_2c,&local_40,&local_50);
      iVar16 = puVar22[9];
      iVar24 = local_40 * *(int *)(puVar22[10] + 0x1c) + local_50 * puVar22[3] + puVar22[1];
      if ((int)uVar21 < 2) {
        iVar11 = 0;
LAB_0011fa6c:
        *(short *)(iVar24 + iVar16 * iVar11 * 2) = (short)*(undefined4 *)(param_8 + iVar11 * 4);
      }
      else {
        if (0 < iVar14) {
          puVar10 = (undefined2 *)(iVar24 + iVar16 * -2);
          pbVar19 = param_8 + -4;
          if ((uVar21 & 1) == 0) {
            puVar10 = puVar10 + iVar16;
            *puVar10 = *(undefined2 *)param_8;
            pbVar19 = param_8;
          }
          uVar8 = *(undefined4 *)(pbVar19 + 4);
          for (iVar11 = iVar12; iVar11 != 0; iVar11 = iVar11 + -1) {
            uVar4 = *(undefined2 *)(pbVar19 + 8);
            puVar10[iVar16] = (short)uVar8;
            uVar8 = *(undefined4 *)(pbVar19 + 0xc);
            puVar10 = puVar10 + iVar16 * 2;
            *puVar10 = uVar4;
            pbVar19 = pbVar19 + 8;
          }
        }
        iVar11 = uVar21 - 1;
        if (-1 < iVar11) goto LAB_0011fa6c;
      }
      iVar20 = iVar20 + 1;
      param_8 = param_8 + param_7;
    } while (iVar20 < param_5);
  }
  (*(code *)*puVar15)(puVar15,local_3c,local_2c,&local_6c,&local_7c);
  (*(code *)*puVar15)(puVar15,local_3c + local_28 + -1,local_2c + param_5 + -1,&local_78,&local_74);
  iVar12 = local_6c;
  if (local_78 < local_6c) {
    local_6c = local_78;
    local_78 = iVar12;
  }
  if (local_74 < local_7c) {
    local_7c = local_74;
  }
  (**(code **)(*(int *)(*local_34 + 0xc) + 4))();
  return;
}

// ==== FUN_0011fb98 @ 0011fb98, 2812 bytes
void FUN_0011fb98(int *param_1,int param_2,int param_3,uint param_4,int param_5,int param_6,
                 int param_7,ushort *param_8,uint param_9,uint *param_10)

{
  uint uVar1;
  ushort uVar2;
  ushort uVar3;
  byte bVar4;
  int iVar5;
  code *pcVar6;
  uint *puVar7;
  int iVar8;
  int iVar9;
  uint *puVar10;
  undefined4 *puVar11;
  uint uVar12;
  undefined4 uVar13;
  ushort *puVar14;
  uint uVar15;
  undefined4 *puVar16;
  uint uVar17;
  int iVar18;
  uint unaff_r11;
  undefined4 uVar19;
  int local_9c;
  int local_84;
  int local_80 [2];
  code *local_78;
  int *local_74;
  code *local_70;
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
  int local_3c;
  int local_38;
  int *local_34;
  int iStack_30;
  int local_2c;
  uint local_28;
  
  local_40 = param_9 + param_2;
  local_60 = param_4 - 1;
  puVar11 = (undefined4 *)param_1[2];
  local_34 = param_1;
  iStack_30 = param_2;
  local_2c = param_3;
  local_28 = param_4;
  if (param_6 == 8) {
    iVar9 = 0;
    if (0 < param_5) {
      do {
        uVar15 = local_28;
        puVar16 = (undefined4 *)local_34[2];
        (*(code *)*puVar16)(puVar16,param_2,iVar9 + local_2c,&local_4c,&local_5c);
        puVar7 = (uint *)(local_4c * *(int *)(puVar16[10] + 0x1c) +
                         local_5c * puVar16[3] + puVar16[1]);
        if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
          iVar5 = local_60 >> 1;
          if (param_10 == (uint *)0x0) {
            if ((int)uVar15 < 2) {
              iVar5 = 0;
            }
            else {
              if (0 < local_60) {
                puVar10 = puVar7 + -puVar16[9];
                puVar14 = (ushort *)((int)param_8 + -1);
                if ((uVar15 & 1) == 0) {
                  puVar10 = puVar10 + puVar16[9];
                  *puVar10 = (uint)(byte)*param_8;
                  puVar14 = param_8;
                }
                bVar4 = *(byte *)((int)puVar14 + 1);
                for (; iVar5 != 0; iVar5 = iVar5 + -1) {
                  uVar2 = puVar14[1];
                  puVar10[puVar16[9]] = (uint)bVar4;
                  bVar4 = *(byte *)((int)puVar14 + 3);
                  puVar10 = puVar10 + puVar16[9] * 2;
                  *puVar10 = (uint)(byte)uVar2;
                  puVar14 = puVar14 + 1;
                }
              }
              iVar5 = uVar15 - 1;
              if (iVar5 < 0) goto LAB_00120344;
            }
            uVar15 = (uint)*(byte *)((int)param_8 + iVar5);
            iVar5 = puVar16[9] * iVar5;
          }
          else {
            if ((int)uVar15 < 2) {
              iVar18 = 0;
            }
            else {
              if (0 < local_60) {
                puVar10 = puVar7 + -puVar16[9];
                puVar14 = (ushort *)((int)param_8 + -1);
                if ((uVar15 & 1) == 0) {
                  puVar10 = puVar10 + puVar16[9];
                  *puVar10 = param_10[(byte)*param_8];
                  puVar14 = param_8;
                }
                bVar4 = *(byte *)((int)puVar14 + 1);
                for (; iVar5 != 0; iVar5 = iVar5 + -1) {
                  uVar2 = puVar14[1];
                  puVar10[puVar16[9]] = param_10[bVar4];
                  bVar4 = *(byte *)((int)puVar14 + 3);
                  puVar10 = puVar10 + puVar16[9] * 2;
                  *puVar10 = param_10[(byte)uVar2];
                  puVar14 = puVar14 + 1;
                }
              }
              iVar18 = uVar15 - 1;
              if (iVar18 < 0) goto LAB_00120344;
            }
            iVar5 = puVar16[9] * iVar18;
            uVar15 = param_10[*(byte *)((int)param_8 + iVar18)];
          }
          puVar7[iVar5] = uVar15;
        }
        else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
          if (param_10 == (uint *)0x0) {
            puVar14 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              if ((byte)*puVar14 != 0) {
                *puVar7 = (uint)(byte)*puVar14;
              }
              uVar15 = uVar15 - 1;
              puVar7 = puVar7 + puVar16[9];
              puVar14 = (ushort *)((int)puVar14 + 1);
            } while (uVar15 != 0);
          }
          else {
            puVar14 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              if ((byte)*puVar14 != 0) {
                *puVar7 = param_10[(byte)*puVar14];
              }
              uVar15 = uVar15 - 1;
              puVar7 = puVar7 + puVar16[9];
              puVar14 = (ushort *)((int)puVar14 + 1);
            } while (uVar15 != 0);
          }
        }
LAB_00120344:
        iVar9 = iVar9 + 1;
        param_8 = (ushort *)((int)param_8 + param_7);
      } while (iVar9 < param_5);
    }
  }
  else if (param_6 < 9) {
    if (param_6 == 1) {
      local_9c = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          iVar9 = local_40;
          iVar5 = local_9c + local_2c;
          local_74 = local_34;
          puVar16 = (undefined4 *)local_34[2];
          local_70 = (code *)*param_10;
          local_78 = (code *)param_10[1];
          (*(code *)*puVar16)(puVar16,local_40,iVar5,&local_50,&local_64);
          puVar7 = (uint *)(local_50 * *(int *)(puVar16[10] + 0x1c) +
                           local_64 * puVar16[3] + puVar16[1]);
          bVar4 = *(byte *)(DAT_00182efc + 0x10) & 3;
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar17 = param_9;
            puVar14 = param_8;
            if ((int)uVar15 < 2) {
              uVar15 = 1;
            }
            do {
              pcVar6 = local_78;
              if (((uint)(byte)*puVar14 & 0x80U >> (uVar17 & 0xff)) == 0) {
                pcVar6 = local_70;
              }
              uVar17 = uVar17 + 1;
              *puVar7 = (uint)pcVar6;
              if (uVar17 == 8) {
                uVar17 = 0;
                puVar14 = (ushort *)((int)puVar14 + 1);
              }
              uVar15 = uVar15 - 1;
              puVar7 = puVar7 + puVar16[9];
            } while (uVar15 != 0);
          }
          else if (bVar4 == 1) {
LAB_0011fd70:
            local_70 = *(code **)(local_74[3] + 0x14);
            local_78 = (code *)(**(code **)(local_74[4] + 8))();
            uVar17 = param_9;
            puVar14 = param_8;
            do {
              if (((uint)(byte)*puVar14 & 0x80U >> (uVar17 & 0xff)) != 0) {
                uVar12 = (*local_70)(local_74,iVar9,iVar5);
                *puVar7 = uVar12 ^ (uint)local_78;
              }
              uVar17 = uVar17 + 1;
              if (uVar17 == 8) {
                uVar17 = 0;
                puVar14 = (ushort *)((int)puVar14 + 1);
              }
              uVar15 = uVar15 - 1;
              puVar7 = puVar7 + puVar16[9];
              iVar9 = iVar9 + 1;
            } while (uVar15 != 0);
          }
          else {
            uVar17 = param_9;
            puVar14 = param_8;
            if (bVar4 == 2) {
              do {
                if (((uint)(byte)*puVar14 & 0x80U >> (uVar17 & 0xff)) != 0) {
                  *puVar7 = (uint)local_78;
                }
                uVar17 = uVar17 + 1;
                if (uVar17 == 8) {
                  uVar17 = 0;
                  puVar14 = (ushort *)((int)puVar14 + 1);
                }
                uVar15 = uVar15 - 1;
                puVar7 = puVar7 + puVar16[9];
              } while (uVar15 != 0);
            }
            else if (bVar4 == 3) goto LAB_0011fd70;
          }
          local_9c = local_9c + 1;
          param_8 = (ushort *)((int)param_8 + param_7);
        } while (local_9c < param_5);
      }
    }
    else if (param_6 == 2) {
      if (0 < param_5) {
        unaff_r11 = 0xc0;
      }
      local_9c = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          puVar16 = (undefined4 *)local_34[2];
          uVar17 = (uint)(byte)*param_8;
          (*(code *)*puVar16)(puVar16,local_40,local_9c + local_2c,&local_38,&local_3c);
          puVar7 = (uint *)(local_38 * *(int *)(puVar16[10] + 0x1c) +
                           puVar16[3] * local_3c + puVar16[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar12 = param_9;
            puVar14 = param_8;
            if (param_10 == (uint *)0x0) {
              do {
                iVar9 = 3 - uVar12;
                uVar12 = uVar12 + 1;
                *puVar7 = (unaff_r11 >> (iVar9 * -2 + 6U & 0xff) & uVar17) >> (iVar9 * 2 & 0xffU);
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 4) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                iVar9 = 3 - uVar12;
                uVar12 = uVar12 + 1;
                *puVar7 = param_10[(unaff_r11 >> (iVar9 * -2 + 6U & 0xff) & uVar17) >>
                                   (iVar9 * 2 & 0xffU)];
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 4) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar12 = param_9;
            puVar14 = param_8;
            if (param_10 == (uint *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar12) * -2 + 6 & 0xff) & uVar17) >>
                        ((3 - uVar12) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = uVar1;
                }
                uVar12 = uVar12 + 1;
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 4) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((3 - uVar12) * -2 + 6 & 0xff) & uVar17) >>
                        ((3 - uVar12) * 2 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = param_10[uVar1];
                }
                uVar12 = uVar12 + 1;
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 4) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          local_9c = local_9c + 1;
          param_8 = (ushort *)((int)param_8 + param_7);
        } while (local_9c < param_5);
      }
    }
    else if (param_6 == 4) {
      if (0 < param_5) {
        unaff_r11 = 0xf0;
      }
      local_9c = 0;
      if (0 < param_5) {
        do {
          uVar15 = local_28;
          puVar16 = (undefined4 *)local_34[2];
          uVar17 = (uint)(byte)*param_8;
          (*(code *)*puVar16)(puVar16,local_40,local_9c + local_2c,&local_38,&local_3c);
          puVar7 = (uint *)(*(int *)(puVar16[10] + 0x1c) * local_38 +
                           local_3c * puVar16[3] + puVar16[1]);
          if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 0) {
            uVar12 = param_9;
            puVar14 = param_8;
            if (param_10 == (uint *)0x0) {
              do {
                iVar9 = 1 - uVar12;
                uVar12 = uVar12 + 1;
                *puVar7 = (unaff_r11 >> (iVar9 * -4 + 4U & 0xff) & uVar17) >> (iVar9 * 4 & 0xffU);
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 2) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                iVar9 = 1 - uVar12;
                uVar12 = uVar12 + 1;
                *puVar7 = param_10[(unaff_r11 >> (iVar9 * -4 + 4U & 0xff) & uVar17) >>
                                   (iVar9 * 4 & 0xffU)];
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 2) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          else if ((*(byte *)(DAT_00182efc + 0x10) & 3) == 2) {
            uVar12 = param_9;
            puVar14 = param_8;
            if (param_10 == (uint *)0x0) {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar12) * -4 + 4 & 0xff) & uVar17) >>
                        ((1 - uVar12) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = uVar1;
                }
                uVar12 = uVar12 + 1;
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 2) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
            else {
              do {
                uVar1 = (unaff_r11 >> ((1 - uVar12) * -4 + 4 & 0xff) & uVar17) >>
                        ((1 - uVar12) * 4 & 0xff);
                if (uVar1 != 0) {
                  *puVar7 = param_10[uVar1];
                }
                uVar12 = uVar12 + 1;
                puVar7 = puVar7 + puVar16[9];
                if (uVar12 == 2) {
                  puVar14 = (ushort *)((int)puVar14 + 1);
                  uVar17 = (uint)*(byte *)puVar14;
                  uVar12 = 0;
                }
                uVar15 = uVar15 - 1;
              } while (uVar15 != 0);
            }
          }
          local_9c = local_9c + 1;
          param_8 = (ushort *)((int)param_8 + param_7);
        } while (local_9c < param_5);
      }
    }
  }
  else if (param_6 == 0x10) {
    iVar9 = 0;
    if (0 < param_5) {
      do {
        uVar15 = local_28;
        puVar16 = (undefined4 *)local_34[2];
        (*(code *)*puVar16)(puVar16,param_2,iVar9 + local_2c,&local_48,&local_58);
        iVar5 = puVar16[9];
        iVar18 = local_48 * *(int *)(puVar16[10] + 0x1c) + local_58 * puVar16[3] + puVar16[1];
        if ((int)uVar15 < 2) {
          iVar8 = 0;
LAB_00120484:
          *(uint *)(iVar18 + iVar5 * iVar8 * 4) = (uint)param_8[iVar8];
        }
        else {
          if (0 < local_60) {
            puVar7 = (uint *)(iVar18 + iVar5 * -4);
            puVar14 = param_8 + -1;
            if ((uVar15 & 1) == 0) {
              puVar7 = puVar7 + iVar5;
              *puVar7 = (uint)*param_8;
              puVar14 = param_8;
            }
            uVar2 = puVar14[1];
            for (iVar8 = local_60 >> 1; iVar8 != 0; iVar8 = iVar8 + -1) {
              uVar3 = puVar14[2];
              puVar7[iVar5] = (uint)uVar2;
              uVar2 = puVar14[3];
              puVar7 = puVar7 + iVar5 * 2;
              *puVar7 = (uint)uVar3;
              puVar14 = puVar14 + 2;
            }
          }
          iVar8 = uVar15 - 1;
          if (-1 < iVar8) goto LAB_00120484;
        }
        iVar9 = iVar9 + 1;
        param_8 = (ushort *)((int)param_8 + param_7);
      } while (iVar9 < param_5);
    }
  }
  else if ((param_6 == 0x20) && (iVar9 = 0, 0 < param_5)) {
    do {
      uVar15 = local_28;
      puVar16 = (undefined4 *)local_34[2];
      (*(code *)*puVar16)(puVar16,param_2,iVar9 + local_2c,&local_44,&local_54);
      iVar5 = puVar16[9];
      iVar18 = local_44 * *(int *)(puVar16[10] + 0x1c) + local_54 * puVar16[3] + puVar16[1];
      if ((int)uVar15 < 2) {
        iVar8 = 0;
LAB_00120570:
        *(undefined4 *)(iVar18 + iVar5 * iVar8 * 4) = *(undefined4 *)(param_8 + iVar8 * 2);
      }
      else {
        if (0 < local_60) {
          puVar16 = (undefined4 *)(iVar18 + iVar5 * -4);
          puVar14 = param_8 + -2;
          if ((uVar15 & 1) == 0) {
            puVar16 = puVar16 + iVar5;
            *puVar16 = *(undefined4 *)param_8;
            puVar14 = param_8;
          }
          uVar19 = *(undefined4 *)(puVar14 + 2);
          for (iVar8 = local_60 >> 1; iVar8 != 0; iVar8 = iVar8 + -1) {
            uVar13 = *(undefined4 *)(puVar14 + 4);
            puVar16[iVar5] = uVar19;
            uVar19 = *(undefined4 *)(puVar14 + 6);
            puVar16 = puVar16 + iVar5 * 2;
            *puVar16 = uVar13;
            puVar14 = puVar14 + 4;
          }
        }
        iVar8 = uVar15 - 1;
        if (-1 < iVar8) goto LAB_00120570;
      }
      iVar9 = iVar9 + 1;
      param_8 = (ushort *)((int)param_8 + param_7);
    } while (iVar9 < param_5);
  }
  (*(code *)*puVar11)(puVar11,local_40,local_2c,&local_68,&local_84);
  (*(code *)*puVar11)(puVar11,local_28 + local_40 + -1,local_2c + param_5 + -1,local_80,&local_6c);
  iVar9 = local_80[0];
  if (local_80[0] < local_68) {
    local_80[0] = local_68;
    local_68 = iVar9;
  }
  if (local_6c < local_84) {
    local_84 = local_6c;
  }
  (**(code **)(*(int *)(*local_34 + 0xc) + 4))();
  return;
}

