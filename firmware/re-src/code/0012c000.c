// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x0012c000-0x0012ffff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_0012c0fc @ 0012c0fc, 164 bytes
// called by: FUN_0012b634
undefined8 FUN_0012c0fc(int param_1,int *param_2,int param_3)

{
  int iVar1;
  int local_20;
  int *piStack_1c;
  int iStack_18;
  int local_14;
  
  local_14 = 0;
  local_20 = param_1;
  piStack_1c = param_2;
  iStack_18 = param_3;
  do {
    do {
      while( true ) {
        do {
          FUN_0012c220(&DAT_00182428,&local_20,&local_14);
          if (local_14 == 0) goto LAB_0012c178;
          iVar1 = *(int *)(local_14 + 8);
        } while (iVar1 == 0);
        if (param_1 != -1) break;
        if (iVar1 == param_3) {
LAB_0012c194:
          FUN_0012c1a4(local_14);
        }
      }
    } while (*(int *)(local_14 + 0xc) != param_1);
    if (param_2 == (int *)0xffffffff) goto LAB_0012c194;
  } while (((int *)(uint)*(byte *)(local_14 + 0x1a) != param_2) ||
          (param_3 != 0 && iVar1 != param_3));
  FUN_0012c1a4(local_14);
LAB_0012c178:
  *(int **)(local_20 + 4) = piStack_1c;
  *piStack_1c = local_20;
  return CONCAT44(piStack_1c,local_20);
}

// ==== FUN_0012c1a4 @ 0012c1a4, 92 bytes
// called by: FUN_0012c0fc
void FUN_0012c1a4(int *param_1)

{
  code *UNRECOVERED_JUMPTABLE;
  
  UNRECOVERED_JUMPTABLE = (code *)0x0;
  if ((*(byte *)((int)param_1 + 0x1b) & 0x20) != 0) {
    UNRECOVERED_JUMPTABLE = (code *)param_1[2];
  }
  *(undefined1 *)((int)param_1 + 0x1b) = 0;
  *(int *)(*param_1 + 4) = param_1[1];
  *(int *)param_1[1] = *param_1;
  param_1[1] = (int)param_1;
  *param_1 = (int)param_1;
  *DAT_00182424 = (int)param_1;
  param_1[1] = (int)DAT_00182424;
  DAT_00182424 = param_1;
  *param_1 = (int)&DAT_00182420;
  if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x0012c1fc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(param_1,5);
  return;
}

// ==== FUN_0012c204 @ 0012c204, 24 bytes
// called by: FUN_00117354
void FUN_0012c204(void)

{
  if (DAT_00182418 == (code *)0x0 || DAT_00182418 == (code *)0xffffffff) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x0012c218. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_00182418)();
  return;
}

// ==== FUN_0012c220 @ 0012c220, 216 bytes
// called by: FUN_0012bf3c, FUN_0012c0fc, FUN_0012bdb0
int FUN_0012c220(int *param_1,int *param_2,int *param_3)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)*param_3;
  if (piVar1 == (int *)0x0) {
    param_2[1] = (int)param_2;
    *param_2 = (int)param_2;
    param_2[2] = 0;
    piVar1 = (int *)*param_1;
    if (piVar1 == (int *)0x0 || piVar1 == param_1) {
      return 0;
    }
    piVar1[1] = (int)param_2;
    *param_2 = *param_1;
    *param_1 = (int)param_2;
    param_2[1] = (int)param_1;
  }
  else if ((int *)*param_2 == piVar1) {
    *(int *)(*piVar1 + 4) = piVar1[1];
    *(undefined4 *)((undefined4 *)*param_3)[1] = *(undefined4 *)*param_3;
    iVar2 = *param_3;
    *(int *)(iVar2 + 4) = iVar2;
    *(int *)*param_3 = iVar2;
    *(int *)param_2[1] = *param_3;
    *(int *)(*param_3 + 4) = param_2[1];
    param_2[1] = *param_3;
    *(int **)*param_3 = param_2;
  }
  piVar1 = (int *)*param_2;
  *param_3 = (int)piVar1;
  if (piVar1 == param_1) {
    *(int *)(*param_2 + 4) = param_2[1];
    *(int *)param_2[1] = *param_2;
    param_2[1] = (int)param_2;
    *param_2 = (int)param_2;
    *param_3 = 0;
  }
  return *param_3;
}

// ==== lcd_boot_restart @ 0012c2f8, 132 bytes
// refs "lcd_boot_proc restart---"
/* refs "lcd_boot_proc restart---" */

void lcd_boot_restart(void)

{
  uint uVar1;
  bool bVar2;
  bool bVar3;
  
  if (DAT_00182bc0 != 0 && DAT_00182bc0 != 0xffffffff) {
    DAT_00182bc0 = DAT_00182bc0 - 1;
    bVar2 = DAT_00182bc0 == 0;
    uVar1 = DAT_00182bc0;
    if (bVar2) {
      uVar1 = (uint)g_config.idle_state;
    }
    bVar3 = bVar2 && uVar1 == 1;
    if (bVar2 && uVar1 == 1) {
      bVar3 = DAT_00182bbd == '\x01';
    }
    if (bVar3) {
      dbg_printf("lcd_boot_proc restart--- \n");
      FUN_0012b7e4(DAT_00182b34,0x19,0,0,0);
    }
  }
  if ((DAT_00182bc8 != 0) && (DAT_00182bc8 = DAT_00182bc8 + -1, DAT_00182bc8 == 0)) {
    DAT_00182bc0 = 0;
  }
  return;
}

// ==== FUN_0012c3ac @ 0012c3ac, 244 bytes
// called by: FUN_0012bd70
int * FUN_0012c3ac(int param_1,uint param_2,int param_3,int param_4,uint param_5,int param_6)

{
  int *piVar1;
  int *piVar2;
  byte bVar3;
  bool bVar4;
  
  if (param_3 == 0) {
    return (int *)0x0;
  }
  piVar1 = (int *)FUN_0012bdb0(param_1,param_2 & 0xffffff3f);
  piVar2 = DAT_00182420;
  if (piVar1 == (int *)0x0) {
    if (DAT_00182420 == (int *)0x0 || (int **)DAT_00182420 == &DAT_00182420) {
      DAT_00182410 = DAT_00182410 + '\x01';
      return (int *)0x0;
    }
    *(int *)(*DAT_00182420 + 4) = DAT_00182420[1];
    *(int *)piVar2[1] = *piVar2;
    piVar2[1] = (int)piVar2;
    *piVar2 = (int)piVar2;
    *DAT_0018242c = (int)piVar2;
    piVar2[1] = (int)DAT_0018242c;
    DAT_0018242c = piVar2;
    bVar3 = 0x10;
    *piVar2 = (int)&DAT_00182428;
  }
  else {
    if ((param_2 & 0x40) != 0) {
      *(byte *)((int)piVar1 + 0x1b) = *(byte *)((int)piVar1 + 0x1b) & 0xdf;
    }
    bVar3 = *(byte *)((int)piVar1 + 0x1b) | 0x10;
    piVar2 = piVar1;
  }
  *(byte *)((int)piVar2 + 0x1b) = bVar3;
  piVar2[2] = param_3;
  piVar2[3] = param_1;
  bVar4 = (param_2 & 0x80) != 0;
  *(byte *)((int)piVar2 + 0x1a) = (byte)param_2 & 0x3f;
  if (bVar4) {
    piVar2[5] = param_4;
  }
  if (bVar4 && param_5 != 0xffff) {
    param_5 = param_5 + 1 & 0xfffeffff;
  }
  piVar2[4] = param_4;
  *(short *)(piVar2 + 6) = (short)param_5;
  piVar2[7] = param_6;
  return piVar2;
}

// ==== FUN_0012c4ac @ 0012c4ac, 12 bytes
// called by: idle_screen_proc
void FUN_0012c4ac(undefined4 param_1)

{
  DAT_00182418 = param_1;
  return;
}

// ==== FUN_0012c4bc @ 0012c4bc, 20 bytes
// called by: FUN_00117354
void FUN_0012c4bc(void)

{
  DAT_0018240c = DAT_0018240c + 1;
  return;
}

// ==== FUN_0012c4d4 @ 0012c4d4, 132 bytes
undefined4 FUN_0012c4d4(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined1 auStack_18 [4];
  int local_14;
  undefined *local_10;
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  
  if (param_2 == 3) {
    FUN_00101524(3,&DAT_0002ee00,&DAT_0002ee00,&DAT_0002ee00,&DAT_00017700,&DAT_0000bb80);
    iVar1 = FUN_001014c8();
    FUN_00103060(1);
    local_10 = &DAT_0001c200;
    local_c = 3;
    local_b = 0;
    local_a = 0;
    local_9 = 0;
    local_14 = iVar1 * 1000;
    FUN_00103514(auStack_18);
    FUN_00100aa4(0);
  }
  return 0;
}

// ==== FUN_0012c560 @ 0012c560, 348 bytes
// called by: usb_receive_task
void FUN_0012c560(void)

{
  uint uVar1;
  code *pcVar2;
  
  DAT_0066aa80._1_1_ = 0;
  DAT_0066aa80._2_1_ = 0;
  DAT_0066aa80._3_1_ = 0;
  DAT_0066aa84 = 0;
  DAT_0066aa85 = 0;
  DAT_0066aa86 = 0;
  DAT_0066aa87 = 0;
  DAT_0066aa8d = 0;
  DAT_0066aa8e = 0;
  DAT_0066aa8f = 0;
  DAT_0066aa90 = 0;
  DAT_0066aa88 = 0x200;
  DAT_0066aa8c = 0;
  DAT_0066aac2 = 0;
  DAT_0066aac3 = 0;
  DAT_0066aac4 = 0;
  DAT_0066aac5 = 0;
  DAT_0066aaca = 0;
  DAT_0066aacb = 0;
  DAT_0066aacc = 0;
  DAT_0066aacd = 0;
  DAT_0066aad8 = 0;
  DAT_0066aae0 = 0;
  DAT_0066aa97 = 0xff;
  DAT_0066aa98 = 0xff;
  DAT_0066aa99 = 0xff;
  DAT_0066aa9a = 0xff;
  FUN_00100520(1,0x10,FUN_0012d614);
  FUN_0010043c(0x10);
  FUN_00100830(0x7f);
  pcVar2 = DAT_0066aa20;
  if ((((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 2) ||
     (pcVar2 = DAT_0066aa24, ((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 1)) {
    (*pcVar2)();
  }
  DAT_b1008008 = 0x3f;
  DAT_b1008014 = 0x126;
  DAT_b1008020 = 0;
  DAT_b1008018 = 2;
  DAT_b1008054 = 0;
  DAT_b1008058 = 0x7f;
  DAT_b1008030 = 0x402;
  uVar1 = DAT_b1008704;
  if ((uVar1 & 0x80000000) != 0) {
    DAT_b1008704 = 800;
  }
  return;
}

// ==== FUN_0012c6d0 @ 0012c6d0, 288 bytes
// called by: usb_receive_task
void FUN_0012c6d0(void)

{
  uint uVar1;
  undefined *puVar2;
  undefined4 uVar3;
  int iVar4;
  
  dbg_printf("N9H20 UDC Library (%s)\n","20180315");
  puVar2 = (undefined *)FUN_001014c8();
  uVar1 = DAT_b0000214;
  DAT_b0000214 = uVar1 & 0xff9f0f1f;
  if (puVar2 == &DAT_00006978) {
    uVar3 = FUN_00100e10(3);
    iVar4 = FUN_0010d398(uVar3,&DAT_00005dc0);
    uVar1 = DAT_b0000214;
    DAT_b0000214 = uVar1 | (iVar4 + -1) * 0x1000 | 0x600020;
  }
  uVar1 = DAT_b1008704;
  DAT_0066aac6 = (uVar1 & 0x80000000) != 0;
  DAT_0066aadc = (uint)(byte)DAT_0066aac6;
  DAT_0066aac9 = 0;
  DAT_0066aac8 = 0;
  DAT_0066aac7 = 0;
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 | 0x50000;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 | 0x40;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 & 0xffffffbf;
  DAT_b1008704 = 0x220;
  DAT_b1008058 = 0x7f;
  do {
    iVar4 = DAT_b1008058;
  } while (iVar4 != 0x7f);
  DAT_0066aace = 0;
  DAT_0066aacf = 0;
  DAT_0066aad0 = 0;
  DAT_0066aad1 = 0;
  return;
}

// ==== FUN_0012c828 @ 0012c828, 68 bytes
// called by: FUN_0012c870, FUN_0012d614
void FUN_0012c828(void)

{
  DAT_0066aa6d = 0;
  DAT_0066aa6e = 0;
  DAT_0066aa6f = 0;
  DAT_0066aa70 = 0;
  DAT_0066aa71 = 0;
  DAT_0066aa74 = 0;
  DAT_0066aa75 = 0;
  DAT_0066aabf = 0;
  DAT_0066aac0 = 0;
  DAT_0066aac1 = 0;
  DAT_0066aaab = 0;
  DAT_0066aaac = 0;
  DAT_0066aaad = 0;
  DAT_0066aaae = 0;
  return;
}

// ==== FUN_0012c870 @ 0012c870, 3436 bytes
// called by: FUN_0012d614
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

ulonglong FUN_0012c870(uint param_1)

{
  int iVar1;
  undefined1 uVar2;
  char cVar3;
  undefined1 uVar4;
  ushort uVar5;
  uint uVar6;
  byte bVar7;
  ushort uVar8;
  ushort uVar9;
  uint uVar10;
  undefined4 uVar11;
  bool bVar12;
  bool bVar13;
  ulonglong uVar14;
  uint local_38;
  int local_34;
  
  local_38 = param_1 & 0xffffff00;
  uVar6 = local_38;
  local_34 = DAT_b1008044;
  DAT_00182ab8 = (byte)local_34;
  DAT_00182ab9 = (undefined1)((uint)local_34 >> 8);
  uVar11 = DAT_b1008048;
  DAT_00182aba = (ushort)uVar11;
  uVar11 = DAT_b100804c;
  DAT_00182abc = (ushort)uVar11;
  uVar11 = DAT_b1008050;
  DAT_00182abe = (ushort)uVar11;
  FUN_0012c828();
  DAT_0066aa77 = 0;
  DAT_0066aa78 = 0;
  uVar5 = (ushort)DAT_00182ab8;
  if ((uVar5 & 0xe0) == 0xa0) {
    DAT_b1008034 = 0x400;
    if (DAT_00182abe != 0) {
      DAT_0066aa77 = 1;
      DAT_0066aa7e = 1;
      DAT_b1008034 = 8;
      DAT_b1008030 = 8;
      return CONCAT44(local_34,param_1) & 0xffffffffffffff00;
    }
    if (DAT_0066aa48 == (code *)0x0) {
      return CONCAT44(local_34,param_1) & 0xffffffffffffff00;
    }
                    /* WARNING: Could not recover jumptable at 0x0012c978. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar14 = (*DAT_0066aa48)();
    return uVar14;
  }
  uVar10 = 0x7f;
  if ((uVar5 & 0xe0) == 0x20) {
    iVar1 = _DAT_0066aace;
    if (_DAT_0066aace == 1) {
      iVar1 = DAT_0066aad8;
    }
    if (_DAT_0066aace == 1 && iVar1 == 1) {
      local_38 = 0;
      DAT_b1008058 = 0x7f;
      do {
        while( true ) {
          uVar6 = DAT_b1008070;
          local_34 = 0;
          do {
            local_34 = local_34 + 1;
          } while (local_34 < 5);
          uVar10 = DAT_b1008070;
          if ((uVar6 & 0xffff) == (uVar10 & 0xffff)) break;
          local_38 = 0;
        }
        local_38 = local_38 + 1;
      } while ((int)local_38 < 6);
    }
    DAT_0066aa78 = 1;
    if (DAT_00182abe != 0) {
      DAT_00182ab0 = (uint)DAT_00182abe;
      DAT_b1008030 = 0x40;
      return CONCAT44(local_34,local_38);
    }
    if (DAT_0066aa4c != (code *)0x0) {
      (*DAT_0066aa4c)();
    }
    DAT_b100802c = 0;
    return CONCAT44(local_34,local_38);
  }
  uVar11 = 3;
  local_38._1_3_ = (undefined3)(param_1 >> 8);
  uVar4 = (undefined1)(DAT_00182aba >> 8);
  uVar2 = (undefined1)(DAT_00182abc >> 8);
  local_38 = uVar6;
  switch(DAT_00182ab9) {
  case 0:
    uVar9 = uVar5 & 0xfc;
    uVar8 = uVar9;
    if (uVar9 == 0x80) {
      uVar8 = DAT_00182aba;
    }
    if (uVar9 != 0x80 || uVar8 != 0) {
LAB_0012d498:
      local_38._0_1_ = 1;
    }
    else {
      bVar12 = (DAT_00182abc & 0xff00) == 0;
      uVar8 = DAT_00182abc;
      if (bVar12) {
        uVar8 = DAT_00182abe;
      }
      if (!bVar12 || uVar8 != 2) goto LAB_0012d498;
      local_38._0_1_ = 0;
    }
    if (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 1) {
      local_38._0_1_ = 1;
    }
    if ((((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 2) &&
       ((((uVar5 & 3) == 2 && (DAT_00182abc != 0)) || ((uVar5 & 3) == 1)))) {
      local_38 = CONCAT31(local_38._1_3_,1);
    }
    uVar6 = local_38;
    if ((local_38 & 0xff) == 1) break;
    FUN_0012c828();
    DAT_0066aac1 = 1;
    if ((DAT_00182ab8 & 3) == 0) {
      if (((uint)DAT_0066aa9f | DAT_0066aaa0 << 8) == 1) {
        DAT_0066aabb = (undefined1)uVar11;
        DAT_0066aabc = (undefined1)((uint)uVar11 >> 8);
        DAT_0066aabd = (undefined1)((uint)uVar11 >> 0x10);
        DAT_0066aabe = (undefined1)((uint)uVar11 >> 0x18);
      }
      else {
LAB_0012d588:
        DAT_0066aabb = 1;
        DAT_0066aabc = 0;
        DAT_0066aabd = 0;
        DAT_0066aabe = 0;
      }
    }
    else if ((DAT_00182ab8 & 3) == 1) {
      if (DAT_00182abc == 0) goto LAB_0012d5d4;
LAB_0012d5cc:
      local_38 = CONCAT31(local_38._1_3_,1);
    }
    else if ((DAT_00182ab8 & 3) == 2) {
      if (DAT_00182abc != 0) {
        uVar10 = DAT_00182abc & 0xf;
        uVar6 = DAT_0066aa28;
        if (uVar10 != DAT_0066aa28) {
          uVar6 = DAT_0066aa2c;
        }
        if (uVar10 != DAT_0066aa28 && uVar10 != uVar6) {
          uVar6 = DAT_0066aa30;
          if (uVar10 != DAT_0066aa30) {
            uVar6 = DAT_0066aa34;
          }
          if (uVar10 != DAT_0066aa30 && uVar10 != uVar6) goto LAB_0012d5cc;
        }
        if (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) == uVar10) goto LAB_0012d588;
      }
LAB_0012d5d4:
      DAT_0066aabb = 0;
      DAT_0066aabc = 0;
      DAT_0066aabd = 0;
      DAT_0066aabe = 0;
    }
    goto LAB_0012cc50;
  case 1:
    bVar12 = (DAT_00182ab8 & 0xfc) != 0;
    if (!bVar12) {
      uVar10 = (uint)DAT_00182aba;
    }
    if (bVar12 || uVar10 >> 2 != 0) {
LAB_0012d318:
      uVar8 = 1;
    }
    else {
      bVar12 = (DAT_00182abc & 0xff00) == 0;
      uVar8 = DAT_00182abc;
      if (bVar12) {
        uVar8 = DAT_00182abe;
      }
      if (!bVar12 || uVar8 != 0) goto LAB_0012d318;
    }
    local_38._0_1_ = (char)uVar8;
    if (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 1) {
      local_38._0_1_ = 1;
    }
    if (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 2) {
      if (uVar5 == 2) {
        if (DAT_00182abc != 0) goto LAB_0012d370;
      }
      else if (uVar5 == 1) {
LAB_0012d370:
        local_38 = CONCAT31(local_38._1_3_,1);
      }
    }
    uVar6 = local_38;
    if ((char)local_38 == '\x01') break;
    if ((DAT_00182ab8 & 3) == 0) {
      if (((byte)DAT_00182aba & 3) != 1) goto LAB_0012d464;
      DAT_0066aaaf = 1;
      DAT_0066aab0 = 0;
      DAT_0066aab1 = 0;
      DAT_0066aab2 = 0;
    }
    else if (((uVar5 & 3) != 1) && ((uVar5 & 3) == 2)) {
      if ((DAT_00182aba & 3) == 0) {
        if ((DAT_00182abc & 0xf) == 0) {
          _DAT_0066aa98 = _DAT_0066aa98 & 0xffffff;
          DAT_0066aa9c = 0;
          DAT_0066aa9d = 0;
          DAT_0066aa9e = 0;
        }
        else {
          uVar10 = DAT_00182abc & 0xf;
          uVar6 = DAT_0066aa28;
          if ((((uVar10 != DAT_0066aa28) && (uVar6 = DAT_0066aa2c, uVar10 != DAT_0066aa2c)) &&
              (uVar6 = DAT_0066aa30, uVar10 != DAT_0066aa30)) &&
             (uVar6 = DAT_0066aa34, uVar10 != DAT_0066aa34)) goto LAB_0012d464;
          _DAT_0066aa98 = CONCAT13((char)uVar6,_DAT_0066aa98);
          DAT_0066aa9c = (undefined1)(uVar6 >> 8);
          DAT_0066aa9d = (undefined1)(uVar6 >> 0x10);
          DAT_0066aa9e = (undefined1)(uVar6 >> 0x18);
        }
      }
      else {
LAB_0012d464:
        local_38 = CONCAT31(local_38._1_3_,1);
      }
    }
    goto LAB_0012cd68;
  case 2:
    break;
  case 3:
    if ((DAT_0066aa80 >> 8 | (uint)DAT_0066aa84 << 0x18) == 1) {
      bVar12 = (DAT_00182ab8 & 3) == 0;
      if (bVar12) {
        bVar7 = (byte)DAT_00182aba & 3;
      }
      else {
        local_38 = CONCAT31(local_38._1_3_,1);
        bVar7 = 1;
      }
      uVar6 = local_38;
      if (bVar12 && bVar7 == 2) {
        DAT_0066aaab = 1;
        DAT_0066aaac = 0;
        DAT_0066aaad = 0;
        DAT_0066aaae = 0;
        DAT_0066aab8 = 0;
        DAT_0066aab9 = 0;
        DAT_0066aaba = 0;
        DAT_b1008034 = 0x400;
        DAT_b1008030 = 0x400;
        DAT_0066aab7 = uVar2;
      }
    }
    local_38 = uVar6;
    if ((((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 2) &&
       ((((uVar5 & 3) == 2 && ((char)DAT_00182abc != '\0')) || ((uVar5 & 3) == 1)))) {
      local_38 = CONCAT31(local_38._1_3_,1);
    }
    uVar6 = local_38;
    if ((char)local_38 == '\x01') break;
    if ((DAT_00182ab8 & 3) == 0) {
      if (((byte)DAT_00182aba & 3) == 1) {
        DAT_0066aaa7 = 1;
        DAT_0066aaa8 = 0;
        DAT_0066aaa9 = 0;
        DAT_0066aaaa = 0;
      }
      else {
        if (((byte)DAT_00182aba & 3) != 2) goto LAB_0012d464;
        DAT_0066aaab = 1;
        DAT_0066aaac = 0;
        DAT_0066aaad = 0;
        DAT_0066aaae = 0;
        DAT_0066aab8 = 0;
        DAT_0066aab9 = 0;
        DAT_0066aaba = 0;
        DAT_0066aab7 = uVar2;
      }
      goto LAB_0012cd68;
    }
    if (((uVar5 & 3) == 1) || ((uVar5 & 3) != 2)) goto LAB_0012cd68;
    if ((DAT_00182aba & 3) == 0) {
      if ((DAT_00182abc & 0xf) == 0) {
        DAT_0066aa97 = 0;
        _DAT_0066aa98 = _DAT_0066aa98 & 0xffff0000;
        uVar4 = 0;
      }
      else {
        uVar10 = DAT_00182abc & 0xf;
        uVar6 = DAT_0066aa28;
        if (((uVar10 != DAT_0066aa28) && (uVar6 = DAT_0066aa2c, uVar10 != DAT_0066aa2c)) &&
           ((uVar6 = DAT_0066aa30, uVar10 != DAT_0066aa30 &&
            (uVar6 = DAT_0066aa34, uVar10 != DAT_0066aa34)))) goto LAB_0012d464;
        DAT_0066aa97 = (byte)uVar6;
        _DAT_0066aa98 = (undefined2)(uVar6 >> 8);
        uVar4 = (undefined1)(uVar6 >> 0x18);
      }
      _DAT_0066aa98 = CONCAT12(uVar4,_DAT_0066aa98);
      goto LAB_0012cd68;
    }
    goto LAB_0012d464;
  case 4:
    break;
  case 5:
    uVar8 = uVar5;
    if (uVar5 == 0) {
      uVar8 = DAT_00182aba;
    }
    if (uVar5 != 0 || (uVar8 & 0xff00) != 0) {
LAB_0012cc80:
      uVar5 = 1;
    }
    else {
      uVar5 = DAT_00182abc;
      if (DAT_00182abc == 0) {
        uVar5 = DAT_00182abe;
      }
      if (DAT_00182abc != 0 || uVar5 != 0) goto LAB_0012cc80;
    }
    local_38._0_1_ = (char)uVar5;
    if (0x7f < DAT_00182aba) {
      local_38._0_1_ = '\x01';
    }
    if (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 3) {
      local_38._0_1_ = '\x01';
    }
    uVar6 = local_38;
    if ((char)local_38 == '\x01') break;
    if ((DAT_0066aa80 >> 8 | (uint)DAT_0066aa84 << 0x18) == 2) {
      if (DAT_00182aba == 0) {
        DAT_0066aa80._0_2_ = CONCAT11(1,(undefined1)DAT_0066aa80);
        DAT_0066aa80 = (uint)(ushort)DAT_0066aa80;
        DAT_0066aa84 = 0;
      }
      DAT_0066aa87 = 0;
      DAT_0066aa88 = DAT_0066aa88 & 0xffffff00;
      DAT_0066aa85 = (byte)DAT_00182aba;
      DAT_0066aa86 = uVar4;
    }
    if ((((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 1) && (DAT_00182aba != 0)) {
      DAT_0066aa87 = 0;
      DAT_0066aa88 = DAT_0066aa88 & 0xffffff00;
      DAT_0066aa80._0_2_ = CONCAT11(2,(undefined1)DAT_0066aa80);
      DAT_0066aa80 = (uint)(ushort)DAT_0066aa80;
      DAT_0066aa84 = 0;
      DAT_0066aa85 = (byte)DAT_00182aba;
      DAT_0066aa86 = uVar4;
    }
    goto LAB_0012cd68;
  case 6:
    dbg_printf("USBR_GET_DESCRIPTOR pkt.wLength = %d\n",DAT_00182abe);
    if (0x200 < DAT_00182abe) {
      FUN_0012c828();
      DAT_b1008030 = 8;
      DAT_b100802c = 2;
      break;
    }
    uVar5 = DAT_00182aba >> 8;
    if (uVar5 == 6) {
      FUN_0012c828();
      DAT_0066aa6f = 1;
      if (DAT_0066a9e4 < DAT_00182abe) {
        DAT_00182abe = (ushort)DAT_0066a9e4;
      }
    }
    else if (uVar5 < 7) {
      if (uVar5 == 1) {
        FUN_0012c828();
        DAT_0066aa6d = 1;
        DAT_0066aa80 = CONCAT31(DAT_0066aa80._1_3_,1);
        if (DAT_0066a9e0 < DAT_00182abe) {
          DAT_00182abe = (ushort)DAT_0066a9e0;
        }
      }
      else if (uVar5 == 2) {
        FUN_0012c828();
        DAT_0066aa6e = 1;
        if (((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 2) {
          if (DAT_0066a9e8 < DAT_00182abe) {
            DAT_00182abe = (ushort)DAT_0066a9e8;
          }
        }
        else if (DAT_0066a9ec < DAT_00182abe) {
          DAT_00182abe = (ushort)DAT_0066a9ec;
        }
      }
      else if (uVar5 == 3) {
        FUN_0012c828();
        DAT_0066aa71 = 1;
        iVar1 = ((byte)DAT_00182aba & 7) * 4;
        if (*(uint *)(&DAT_0066aa0c + iVar1) < (uint)DAT_00182abe) {
          DAT_00182abe = (ushort)*(undefined4 *)(&DAT_0066aa0c + iVar1);
        }
      }
      else {
LAB_0012cac8:
        local_38 = CONCAT31(local_38._1_3_,1);
      }
    }
    else if (uVar5 == 7) {
      FUN_0012c828();
      DAT_0066aa70 = 1;
      if (((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 2) {
        if (DAT_0066a9f0 < DAT_00182abe) {
          DAT_00182abe = (ushort)DAT_0066a9f0;
        }
      }
      else if ((((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 1) &&
              (DAT_0066a9f4 < DAT_00182abe)) {
        DAT_00182abe = (ushort)DAT_0066a9f4;
      }
    }
    else if (uVar5 == 0x21) {
      FUN_0012c828();
      DAT_0066aa74 = 1;
      if (DAT_0066a9f8 < DAT_00182abe) {
        DAT_00182abe = (ushort)DAT_0066a9f8;
      }
    }
    else {
      if (uVar5 != 0x22) goto LAB_0012cac8;
      FUN_0012c828();
      DAT_0066aa75 = 1;
      DAT_0066aad8 = 1;
      if (*(uint *)(&DAT_0066a9fc + (uint)DAT_00182abc * 4) < (uint)DAT_00182abe) {
        DAT_00182abe = (ushort)*(undefined4 *)(&DAT_0066a9fc + (uint)DAT_00182abc * 4);
      }
    }
    uVar6 = local_38;
    if ((char)local_38 != '\0') break;
    goto LAB_0012cc50;
  case 7:
    break;
  case 8:
    uVar8 = uVar5;
    if (uVar5 == 0x80) {
      uVar8 = DAT_00182aba;
    }
    if (uVar5 != 0x80 || uVar8 != 0) {
LAB_0012cdd8:
      cVar3 = '\x01';
    }
    else {
      uVar5 = DAT_00182abc;
      if (DAT_00182abc == 0) {
        uVar5 = DAT_00182abe;
      }
      if (DAT_00182abc != 0 || uVar5 != 1) goto LAB_0012cdd8;
      cVar3 = '\0';
    }
    local_38 = CONCAT31(local_38._1_3_,cVar3);
    uVar6 = local_38;
    if (cVar3 == '\x01') break;
    FUN_0012c828();
    DAT_0066aabf = 1;
    goto LAB_0012cc50;
  case 9:
    DAT_00182aa8 = 1;
    uVar8 = uVar5;
    if (uVar5 == 0) {
      uVar8 = DAT_00182aba;
    }
    if (uVar5 != 0 || (uVar8 & 0xff00) != 0) {
LAB_0012ce24:
      local_38._0_1_ = '\x01';
    }
    else {
      bVar12 = (uVar8 & 0x80) == 0;
      if (bVar12) {
        uVar8 = DAT_00182abc;
      }
      bVar13 = bVar12 && uVar8 == 0;
      if (bVar12 && uVar8 == 0) {
        bVar13 = DAT_00182abe == 0;
        uVar8 = DAT_00182abe;
      }
      local_38._0_1_ = (char)uVar8;
      if (!bVar13) goto LAB_0012ce24;
    }
    if (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 1) {
      local_38._0_1_ = '\x01';
    }
    if (1 < DAT_00182aba) {
      local_38._0_1_ = '\x01';
    }
    uVar6 = local_38;
    if ((char)local_38 == '\x01') break;
    if (DAT_00182aba == 0) {
      DAT_0066aa8d = 0;
      DAT_0066aa8e = 0;
      DAT_0066aa80._0_2_ = CONCAT11(2,(undefined1)DAT_0066aa80);
    }
    else {
      DAT_0066aa80._0_2_ = CONCAT11(3,(undefined1)DAT_0066aa80);
      DAT_0066aa8d = (byte)DAT_00182aba;
      DAT_0066aa8e = uVar4;
    }
    DAT_0066aa80 = (uint)(ushort)DAT_0066aa80;
    DAT_0066aa84 = 0;
    DAT_0066aad2 = 1;
    DAT_0066aad3 = 0;
    DAT_0066aad4 = 0;
    DAT_0066aad5 = 0;
LAB_0012cd68:
    DAT_b1008034 = 0x400;
    DAT_b100802c = 0;
    DAT_b1008030 = 0x400;
    uVar6 = local_38;
    break;
  case 10:
    uVar8 = uVar5;
    if (uVar5 == 0x81) {
      uVar8 = DAT_00182aba;
    }
    if ((uVar5 != 0x81 || uVar8 != 0) || (DAT_00182abe != 1)) {
      local_38._0_1_ = '\x01';
    }
    else {
      local_38._0_1_ = '\0';
    }
    if ((((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 1) ||
       (((uint)DAT_0066aa84 << 0x18 | DAT_0066aa80 >> 8) == 2)) {
      local_38._0_1_ = '\x01';
    }
    uVar6 = local_38;
    if ((char)local_38 == '\x01') break;
    FUN_0012c828();
    DAT_0066aac0 = 1;
LAB_0012cc50:
    DAT_b1008034 = 8;
    DAT_b1008030 = 8;
    uVar6 = local_38;
    break;
  case 0xb:
    DAT_0066aa91 = (byte)DAT_00182aba;
    _DAT_0066aa8f = DAT_00182abc;
    DAT_0066aa92 = uVar4;
    if (_DAT_0066aace == 0 && DAT_0066aad1 == '\0') {
      if (DAT_00182aba == 0) {
        uVar6 = DAT_b100805c;
        DAT_b100805c = uVar6 | 0x80;
        uVar6 = DAT_b100805c;
        DAT_b100805c = uVar6 & 0x7f;
        DAT_0066aa7e = 1;
        DAT_0066aac2 = 0;
        DAT_0066aac3 = 0;
        DAT_0066aac4 = 0;
        DAT_0066aac5 = 0;
        DAT_0066aad8 = 0;
      }
      else {
        DAT_b1008054 = 0;
LAB_0012d0b0:
        DAT_0066aad8 = 1;
        DAT_0066aac5 = 0;
        DAT_0066aac4 = 0;
        DAT_0066aac3 = 0;
        DAT_0066aac2 = 1;
        DAT_b1008058 = 0;
      }
    }
    else if (DAT_00182abc == 1) {
      if (DAT_00182aba != 0) goto LAB_0012d0b0;
      uVar6 = DAT_b100805c;
      DAT_b100805c = uVar6 | 0x80;
      uVar6 = DAT_b100805c;
      DAT_b100805c = uVar6 & 0x7f;
      DAT_b1008074 = 1;
      DAT_0066aa7e = 1;
      DAT_0066aac2 = 0;
      DAT_0066aac3 = 0;
      DAT_0066aac4 = 0;
      DAT_0066aac5 = 0;
      DAT_0066aad8 = 0;
      DAT_b1008058 = 0x7f;
    }
    else if (DAT_00182abc == 3) {
      if (DAT_00182aba == 1) {
        DAT_0066aaca = 1;
        DAT_0066aacb = 0;
        DAT_0066aacc = 0;
        DAT_0066aacd = 0;
        DAT_0066aae0 = 1;
      }
      else {
        DAT_0066aaca = 0;
        DAT_0066aacb = 0;
        DAT_0066aacc = 0;
        DAT_0066aacd = 0;
        DAT_0066aae0 = 0;
      }
    }
    goto LAB_0012cd68;
  }
  local_38 = uVar6;
  if ((char)local_38 != '\x01') {
    return CONCAT44(local_34,local_38);
  }
  uVar6 = DAT_b1008030;
  DAT_b1008030 = uVar6 | 3;
  DAT_b100802c = 2;
  return CONCAT44(local_34,local_38);
}

// ==== FUN_0012d614 @ 0012d614, 2252 bytes
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0012d614(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined4 uVar7;
  uint uVar8;
  code *pcVar9;
  uint uVar10;
  bool bVar11;
  bool bVar12;
  int local_28;
  
  uVar3 = DAT_b1008000;
  uVar10 = DAT_b1008008;
  if ((uVar3 & uVar10) == 0) {
    return;
  }
  if ((uVar3 & 1) != 0) {
    uVar10 = DAT_b1008010;
    uVar4 = DAT_b1008014;
    if ((uVar10 & uVar4 & 0x100) != 0) {
      uVar8 = DAT_b1008704;
      if ((uVar8 & 0x80000000) == 0) {
        if (DAT_0066aa60 != (code *)0x0) {
          (*DAT_0066aa60)();
        }
        DAT_0066aac6 = 0;
        DAT_0066aac7 = 0;
        DAT_0066aac8 = 0;
        DAT_0066aac9 = 0;
        DAT_0066aadc = 0;
        DAT_0066aa80._0_1_ = 0;
        DAT_0066aac2 = 0;
        DAT_0066aac3 = 0;
        DAT_0066aac4 = 0;
        DAT_0066aac5 = 0;
        DAT_0066aad8 = 0;
        DAT_0066aaca = 0;
        DAT_0066aacb = 0;
        DAT_0066aacc = 0;
        DAT_0066aacd = 0;
        DAT_0066aae0 = 0;
        FUN_0012c828();
        DAT_00182aa4 = 0;
        uVar7 = 0x220;
      }
      else {
        DAT_0066aa80._1_1_ = 1;
        DAT_0066aa80._2_1_ = 0;
        DAT_0066aa80._3_1_ = 0;
        DAT_0066aa84 = 0;
        DAT_0066aa85 = 0;
        DAT_0066aa86 = 0;
        DAT_0066aa87 = 0;
        DAT_0066aa88._0_1_ = 0;
        if (DAT_0066aa5c != (code *)0x0) {
          (*DAT_0066aa5c)();
        }
        DAT_0066aac6 = 1;
        DAT_0066aac7 = 0;
        DAT_0066aac8 = 0;
        DAT_0066aac9 = 0;
        DAT_0066aadc = 1;
        uVar7 = 800;
      }
      DAT_b1008704 = uVar7;
      DAT_b1008010 = 0x100;
    }
    if ((uVar10 & uVar4 & 1) != 0) {
      if (DAT_0066aa58 != (code *)0x0) {
        (*DAT_0066aa58)();
      }
      DAT_00182aa4 = 1;
      DAT_b1008010 = 1;
    }
    if ((uVar10 & uVar4 & 2) != 0) {
      DAT_0066aa80._1_1_ = 0;
      DAT_0066aa80._2_1_ = 0;
      DAT_0066aa80._3_1_ = 0;
      DAT_0066aa84 = 0;
      DAT_0066aa88._1_1_ = 0;
      DAT_0066aa88._2_1_ = 0;
      DAT_0066aa88._3_1_ = 0;
      DAT_0066aa8c = 0;
      DAT_0066aa85 = 0;
      DAT_0066aa86 = 0;
      DAT_0066aa87 = 0;
      DAT_0066aa88._0_1_ = 0;
      FUN_0012c828();
      DAT_0066aa80._0_1_ = 0;
      DAT_0066aa7d = 0;
      DAT_0066aa7e = 1;
      DAT_00182aa4 = 1;
      if (DAT_0066aa54 != (code *)0x0) {
        (*DAT_0066aa54)();
      }
      DAT_b100805c = 0x80;
      uVar8 = 0;
      do {
        uVar8 = uVar8 + 1;
      } while (uVar8 < 0x100);
      DAT_b100805c = 0;
      DAT_0066aa80._1_1_ = 1;
      DAT_0066aa80._2_1_ = 0;
      DAT_0066aa80._3_1_ = 0;
      DAT_0066aa84 = 0;
      DAT_0066aa85 = 0;
      DAT_0066aa86 = 0;
      DAT_0066aa87 = 0;
      DAT_0066aa88._0_1_ = 0;
      uVar8 = DAT_b1008018;
      if ((uVar8 & 4) == 0) {
        DAT_0066aa88._1_1_ = 1;
        pcVar9 = DAT_0066aa24;
      }
      else {
        DAT_0066aa88._1_1_ = 2;
        pcVar9 = DAT_0066aa20;
      }
      DAT_0066aa8c = 0;
      DAT_0066aa88._3_1_ = 0;
      DAT_0066aa88._2_1_ = 0;
      (*pcVar9)();
      DAT_b1008030 = 2;
      if (DAT_0066aa28 != -1) {
        DAT_b1008074 = 0;
        DAT_b1008074 = 1;
        DAT_b1008074 = 8;
      }
      if (DAT_0066aa2c != -1) {
        DAT_b100809c = 0;
        DAT_b100809c = 1;
        DAT_b100809c = 8;
      }
      if (DAT_0066aa30 != -1) {
        DAT_b10080c4 = 0;
        DAT_b10080c4 = 1;
        DAT_b10080c4 = 8;
      }
      if (DAT_0066aa34 != -1) {
        DAT_b10080ec = 0;
        DAT_b10080ec = 1;
        DAT_b10080ec = 8;
      }
      DAT_b1008020 = 0;
      DAT_b1008010 = 2;
      DAT_b1008014 = 0x10e;
      DAT_b1008034 = 0xfffffffc;
    }
    if ((uVar10 & uVar4 & 4) != 0) {
      DAT_0066aa7f = '\x01';
      DAT_b1008010 = 4;
      DAT_00182aa8 = 1;
      DAT_00182aa4 = 1;
      DAT_b1008014 = 0x10a;
    }
    if ((uVar10 & uVar4 & 8) != 0) {
      DAT_0066aa7f = '\0';
      DAT_00182aa4 = 1;
      DAT_00182aa8 = 0;
      DAT_b1008014 = 0x106;
      uVar8 = DAT_b1008704;
      local_28 = 0;
      do {
        uVar6 = DAT_b1008704;
        if ((uVar6 & 0x80000000) != (uVar8 & 0x80000000)) {
          uVar3 = DAT_b1008704;
          if ((uVar3 & 0x80000000) == 0) {
            DAT_b1008014 = 0x106;
            DAT_0066aac6 = 0;
            DAT_0066aac7 = 0;
            DAT_0066aac8 = 0;
            DAT_0066aac9 = 0;
            DAT_0066aadc = 0;
            DAT_0066aa80._0_1_ = 0;
            DAT_0066aac2 = 0;
            DAT_0066aac3 = 0;
            DAT_0066aac4 = 0;
            DAT_0066aac5 = 0;
            DAT_0066aad8 = 0;
            DAT_0066aaca = 0;
            DAT_0066aacb = 0;
            DAT_0066aacc = 0;
            DAT_0066aacd = 0;
            DAT_0066aae0 = 0;
            DAT_b1008704 = 0x220;
            DAT_00182aa8 = 0;
            dbg_printf("Unplug(S)!!\n");
            DAT_b1008014 = 0x10e;
            DAT_b1008010 = 8;
          }
          else {
            DAT_0066aac6 = 1;
            DAT_0066aac7 = 0;
            DAT_0066aac8 = 0;
            DAT_0066aac9 = 0;
            DAT_0066aadc = 1;
            DAT_b1008704 = 800;
          }
          DAT_b1008010 = 0x100;
          return;
        }
        local_28 = local_28 + 1;
      } while (local_28 < 0x80000);
      DAT_b1008014 = 0x10e;
      DAT_b1008010 = 8;
      pcVar9 = DAT_00182aac;
      if (_DAT_0066aace == 0) {
        pcVar9 = (code *)0x0;
      }
      if (pcVar9 != (code *)0x0) {
        (*pcVar9)();
      }
      DAT_00182aa8 = 0;
      DAT_b1008014 = 0x106;
    }
    if ((uVar10 & uVar4 & 0x10) != 0) {
      DAT_0066aa80._1_1_ = 1;
      DAT_0066aa80._2_1_ = 0;
      DAT_0066aa80._3_1_ = 0;
      DAT_0066aa84 = 0;
      DAT_0066aa88._1_1_ = 2;
      DAT_0066aa88._2_1_ = 0;
      DAT_0066aa88._3_1_ = 0;
      DAT_0066aa8c = 0;
      DAT_0066aa85 = 0;
      DAT_0066aa86 = 0;
      DAT_0066aa87 = 0;
      DAT_0066aa88._0_1_ = 0;
      DAT_00182aa4 = 1;
      DAT_b1008030 = 2;
      DAT_b1008010 = 0x10;
    }
    if ((uVar10 & uVar4 & 0x20) != 0) {
      DAT_0066aa7d = 1;
      DAT_00182aa4 = 1;
      DAT_b1008010 = 0x20;
      if (DAT_0066aa50 != (code *)0x0) {
        (*DAT_0066aa50)();
      }
    }
    if ((uVar10 & uVar4 & 0x40) != 0) {
      DAT_b1008010 = 0x40;
    }
    DAT_b1008000 = 1;
  }
  if ((uVar3 & 2) == 0) {
LAB_0012ddf8:
    if ((uVar3 & 0xfc) == 0) {
      return;
    }
    DAT_00182aa4 = 1;
    if ((uVar3 & 4) != 0) {
      uVar7 = DAT_b1008068;
      uVar5 = DAT_b100806c;
      DAT_b1008068 = uVar7;
      if (DAT_0066aa38 != (code *)0x0) {
        (*DAT_0066aa38)(uVar5,uVar7);
      }
    }
    if ((uVar3 & 8) != 0) {
      uVar7 = DAT_b1008090;
      uVar5 = DAT_b1008094;
      DAT_b1008090 = uVar7;
      if (DAT_0066aa3c != (code *)0x0) {
        (*DAT_0066aa3c)(uVar5,uVar7);
      }
    }
    if ((uVar3 & 0x10) != 0) {
      uVar7 = DAT_b10080b8;
      uVar5 = DAT_b10080bc;
      DAT_b10080b8 = uVar7;
      if (DAT_0066aa40 != (code *)0x0) {
        (*DAT_0066aa40)(uVar5,uVar7);
      }
    }
    if ((uVar3 & 0x20) != 0) {
      uVar7 = DAT_b10080e0;
      uVar5 = DAT_b10080e4;
      DAT_b10080e0 = uVar7;
      if (DAT_0066aa44 != (code *)0x0) {
        (*DAT_0066aa44)(uVar5,uVar7);
      }
    }
    DAT_b1008000 = 0xfc;
    return;
  }
  uVar4 = DAT_b1008034;
  uVar10 = DAT_b1008030;
  DAT_00182aa4 = 1;
  uVar10 = uVar4 & uVar10;
  if ((uVar10 & 1) != 0) {
    DAT_b1008034 = 1;
    DAT_00182aa4 = 1;
    return;
  }
  if ((uVar10 & 2) != 0) {
    DAT_b1008034 = 2;
    FUN_0012c870();
    return;
  }
  if ((uVar10 & 4) == 0) {
    if ((uVar10 & 8) != 0) {
      DAT_b1008034 = 8;
      DAT_b1008034 = 0x20;
      if ((uVar4 & 0x400) == 0) {
        DAT_b1008030 = 0x22;
        FUN_0012df14();
        return;
      }
      DAT_b1008030 = 0x420;
      DAT_00182aa4 = 1;
      return;
    }
    if ((uVar10 & 0x10) == 0) {
      if ((uVar10 & 0x20) == 0) {
        if ((uVar10 & 0x40) == 0) {
          if ((uVar10 & 0x80) == 0) {
            if ((uVar10 & 0x100) == 0) {
              if ((uVar10 & 0x200) == 0) {
                if ((uVar10 & 0x400) != 0) {
                  FUN_0012e2f0();
                  DAT_b1008034 = 0x400;
                  bVar11 = DAT_0066aa77 == '\0';
                  cVar1 = DAT_0066aa77;
                  if (bVar11) {
                    cVar1 = DAT_0066aa7f;
                  }
                  bVar12 = bVar11 && cVar1 == '\0';
                  if (bVar11 && cVar1 == '\0') {
                    bVar12 = DAT_0066aa6d == '\0';
                  }
                  if (!bVar12) {
                    DAT_0066aa80._0_1_ = 1;
                  }
                  DAT_b1008030 = 2;
                  return;
                }
                if ((uVar10 & 0x800) == 0) {
                  if ((uVar10 & 0x1000) == 0) {
                    DAT_b1008000 = 2;
                    goto LAB_0012ddf8;
                  }
                  uVar7 = 0x1000;
                }
                else {
                  uVar7 = 0x800;
                }
              }
              else {
                uVar7 = 0x200;
              }
            }
            else {
              uVar7 = 0x100;
            }
          }
          else {
            uVar7 = 0x80;
          }
          goto LAB_0012dc0c;
        }
        DAT_b1008034 = 0x40;
        DAT_b100802c = 0;
        if (DAT_0066aa4c != (code *)0x0) {
          (*DAT_0066aa4c)();
        }
        iVar2 = DAT_b100803c;
        DAT_00182ab0 = DAT_00182ab0 - iVar2;
        if (DAT_00182ab0 == 0) {
          DAT_b1008030 = 0x412;
        }
        else {
          DAT_b1008030 = 0x40;
        }
        iVar2 = _DAT_0066aace;
        if (_DAT_0066aace == 1) {
          iVar2 = DAT_0066aad8;
        }
        if (_DAT_0066aace != 1 || iVar2 != 1) {
          return;
        }
        DAT_b1008054 = 0;
      }
      else {
        DAT_b1008034 = 0x420;
        if ((char)DAT_0066aa68 == '\0') {
          uVar7 = 0x400;
          DAT_b1008034 = 0x400;
          DAT_b100802c = 0;
        }
        else {
          DAT_b1008034 = 8;
          uVar7 = 0x28;
        }
        DAT_b1008030 = uVar7;
        if (DAT_0066aad8 != 1) {
          DAT_00182aa4 = 1;
          return;
        }
        if (_DAT_0066aace == 0 && DAT_0066aad1 == '\0') {
          DAT_00182aa4 = 1;
          return;
        }
      }
      DAT_b1008058 = 0;
      return;
    }
    uVar7 = 0x10;
  }
  else {
    uVar7 = 4;
  }
LAB_0012dc0c:
  DAT_b1008034 = uVar7;
  DAT_00182aa4 = 1;
  return;
}

// ==== FUN_0012df14 @ 0012df14, 968 bytes
// called by: FUN_0012d614
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_0012df14(undefined4 *param_1,undefined4 *param_2)

{
  undefined1 uVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  uint uVar8;
  undefined4 *puVar9;
  bool bVar10;
  undefined8 uVar11;
  undefined4 *local_20;
  undefined4 *local_1c;
  
  cVar2 = '\x01';
  if (DAT_0066aa77 != '\x01') {
    cVar2 = DAT_0066aa6d;
  }
  local_20 = param_1;
  local_1c = param_2;
  if (DAT_0066aa77 != '\x01' && cVar2 != '\x01') {
    cVar2 = '\x01';
    if (DAT_0066aa6f != '\x01') {
      cVar2 = DAT_0066aa6e;
    }
    if (DAT_0066aa6f != '\x01' && cVar2 != '\x01') {
      cVar2 = '\x01';
      if (DAT_0066aa70 != '\x01') {
        cVar2 = DAT_0066aa71;
      }
      if (DAT_0066aa70 != '\x01' && cVar2 != '\x01') {
        cVar2 = '\x01';
        if (DAT_0066aabf != '\x01') {
          cVar2 = DAT_0066aac0;
        }
        if (DAT_0066aabf != '\x01' && cVar2 != '\x01') {
          bVar10 = DAT_0066aac1 != '\x01';
          cVar2 = '\x01';
          if (bVar10) {
            cVar2 = DAT_0066aa74;
          }
          cVar3 = cVar2;
          if (bVar10 && cVar2 != '\x01') {
            cVar3 = DAT_0066aa75;
          }
          if ((bVar10 && cVar2 != '\x01') && cVar3 != '\x01') goto LAB_0012e1a8;
        }
      }
    }
  }
  uVar8 = 1;
  puVar9 = (undefined4 *)0x0;
  if ((char)DAT_0066aa68 == '\0') {
    puVar6 = _DAT_0066a9a0;
    if ((DAT_0066aa6d == '\x01') || (puVar6 = DAT_0066a9a4, DAT_0066aa6f == '\x01'))
    goto LAB_0012dfb4;
    if (DAT_0066aa6e == '\x01') {
      puVar6 = _DAT_0066a9a8;
      if (((DAT_0066aa88 >> 8 | (uint)DAT_0066aa8c << 0x18) != 2) &&
         (puVar6 = param_1, ((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 1)) {
        puVar6 = _DAT_0066a9ac;
      }
      goto LAB_0012dfb4;
    }
    if (DAT_0066aa70 == '\x01') {
      puVar6 = DAT_0066a9b0;
      if ((((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) != 2) &&
         (puVar6 = param_1, ((uint)DAT_0066aa8c << 0x18 | DAT_0066aa88 >> 8) == 1)) {
        puVar6 = DAT_0066a9b4;
      }
      goto LAB_0012dfb4;
    }
    if (DAT_0066aa71 == '\x01') {
      puVar6 = *(undefined4 **)(&DAT_0066a9cc + ((byte)DAT_00182aba & 7) * 4);
      goto LAB_0012dfb4;
    }
    puVar6 = _DAT_0066a9b8;
    if (DAT_0066aa74 == '\x01') goto LAB_0012dfb4;
    if (DAT_0066aa75 == '\x01') {
      puVar6 = *(undefined4 **)(&DAT_0066a9bc + (uint)DAT_00182abc * 4);
      goto LAB_0012dfb4;
    }
    uVar1 = DAT_0066aa8d;
    if ((DAT_0066aabf != '\x01') && (uVar1 = DAT_0066aa91, DAT_0066aac0 != '\x01')) {
      if (DAT_0066aac1 == '\x01') {
        DAT_b1008028._0_1_ = DAT_0066aabb;
        DAT_b1008028._0_1_ = 0;
        DAT_b1008038 = 2;
        goto LAB_0012e1a8;
      }
      puVar6 = param_1;
      if (DAT_0066aa77 == '\x01') {
        iVar4 = _DAT_0066aace;
        if (_DAT_0066aace == 1) {
          iVar4 = DAT_0066aad8;
        }
        if (_DAT_0066aace == 1 && iVar4 == 1) {
          local_20 = (undefined4 *)0x0;
          do {
            while( true ) {
              uVar8 = DAT_b1008070;
              local_1c = (undefined4 *)0x0;
              do {
                local_1c = (undefined4 *)((int)local_1c + 1);
              } while ((int)local_1c < 5);
              uVar5 = DAT_b1008070;
              if ((uVar8 & 0xffff) == (uVar5 & 0xffff)) break;
              local_20 = (undefined4 *)0x0;
            }
            local_20 = (undefined4 *)((int)local_20 + 1);
          } while ((int)local_20 < 6);
        }
        DAT_b1008058 = 0x7f;
        if (DAT_0066aa48 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0012e23c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          uVar11 = (*DAT_0066aa48)();
          return uVar11;
        }
        goto LAB_0012e1a8;
      }
      goto LAB_0012dfb4;
    }
    DAT_b1008028._0_1_ = uVar1;
  }
  else {
    puVar6 = (undefined4 *)((uint)DAT_0066aa6c << 0x18 | DAT_0066aa68 >> 8);
LAB_0012dfb4:
    uVar8 = (uint)DAT_00182abe;
    if (DAT_00182abe < 0x41) {
      if (DAT_0066aa64 != 0) {
        uVar8 = DAT_0066aa64;
        if (0x40 < DAT_0066aa64) goto LAB_0012dffc;
        DAT_00182abe = (ushort)DAT_0066aa64;
      }
      DAT_0066aa68 = (uint)DAT_0066aa68._1_3_ << 8;
      DAT_0066aa64 = 0;
    }
    else {
LAB_0012dffc:
      DAT_0066aa68 = CONCAT31(DAT_0066aa68._1_3_,1);
      DAT_0066aa64 = uVar8 - 0x40;
      DAT_00182abe = 0x40;
    }
    uVar8 = (uint)DAT_00182abe;
    for (local_20 = (undefined4 *)0x0; local_20 < (undefined4 *)(uint)(DAT_00182abe >> 2);
        local_20 = (undefined4 *)((int)local_20 + 1)) {
      DAT_b1008028 = *puVar6;
      puVar6 = puVar6 + 1;
    }
    local_1c = (undefined4 *)(uVar8 & 3);
    puVar7 = puVar6;
    if ((DAT_00182abe & 3) != 0) {
      for (; local_20 = puVar9, puVar9 < local_1c; puVar9 = (undefined4 *)((int)puVar9 + 1)) {
        DAT_b1008028._0_1_ = *(undefined1 *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
      }
    }
    if ((char)DAT_0066aa68 != '\0') {
      DAT_0066aa68 = CONCAT13((char)((uint)puVar6 >> 0x10),
                              CONCAT12((char)((uint)puVar6 >> 8),
                                       CONCAT11((char)puVar6,(char)DAT_0066aa68)));
      DAT_0066aa6c = (byte)((uint)puVar6 >> 0x18);
    }
  }
  DAT_b1008038 = uVar8;
LAB_0012e1a8:
  return CONCAT44(local_1c,local_20);
}

// ==== FUN_0012e2f0 @ 0012e2f0, 1136 bytes
// called by: FUN_0012d614
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0012e2f0(void)

{
  uint uVar1;
  
  if (DAT_00182ab9 == 5) {
    DAT_b1008020 = _DAT_0066aa84 >> 8 | (uint)(byte)DAT_0066aa88 << 0x18;
    return;
  }
  if (5 < DAT_00182ab9) {
    if (DAT_00182ab9 == 9) {
      if (DAT_0066aa28 != 0xffffffff) {
        DAT_b1008074 = 8;
      }
      if (DAT_0066aa2c != 0xffffffff) {
        DAT_b100809c = 8;
      }
      if (DAT_0066aa30 != 0xffffffff) {
        DAT_b10080c4 = 8;
      }
      if (DAT_0066aa34 != 0xffffffff) {
        DAT_b10080ec = 8;
      }
      return;
    }
    return;
  }
  if (DAT_00182ab9 == 1) {
    if (((DAT_0066aa28 != 0xffffffff) &&
        ((_DAT_0066aa98 >> 0x18 | _DAT_0066aa9c << 8) == DAT_0066aa28)) &&
       (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) == DAT_0066aa28)) {
      DAT_b1008074 = 0;
      DAT_00182ab4 = DAT_00182ab4 & 0xfffffffe;
      DAT_b1008074 = 8;
      DAT_0066aa97 = 0xff;
      _DAT_0066aa98 = CONCAT13(DAT_0066aa9b,0xffffff);
    }
    if (((DAT_0066aa2c != 0xffffffff) &&
        ((_DAT_0066aa98 >> 0x18 | _DAT_0066aa9c << 8) == DAT_0066aa2c)) &&
       (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) == DAT_0066aa2c)) {
      DAT_b100809c = 0;
      DAT_00182ab4 = DAT_00182ab4 & 0xfffffffd;
      DAT_b100809c = 8;
      DAT_0066aa97 = 0xff;
      _DAT_0066aa98 = CONCAT13(DAT_0066aa9b,0xffffff);
    }
    if (((DAT_0066aa30 != 0xffffffff) &&
        ((_DAT_0066aa98 >> 0x18 | _DAT_0066aa9c << 8) == DAT_0066aa30)) &&
       (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) == DAT_0066aa30)) {
      DAT_b10080c4 = 0;
      DAT_00182ab4 = DAT_00182ab4 & 0xfffffffb;
      DAT_b10080c4 = 8;
      DAT_0066aa97 = 0xff;
      _DAT_0066aa98 = CONCAT13(DAT_0066aa9b,0xffffff);
    }
    if (((DAT_0066aa34 != 0xffffffff) &&
        ((_DAT_0066aa98 >> 0x18 | _DAT_0066aa9c << 8) == DAT_0066aa34)) &&
       (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) == DAT_0066aa34)) {
      DAT_b10080ec = 0;
      DAT_00182ab4 = DAT_00182ab4 & 0xfffffff7;
      DAT_b10080ec = 8;
      DAT_0066aa97 = 0xff;
      _DAT_0066aa98 = CONCAT13(DAT_0066aa9b,0xffffff);
      return;
    }
    if ((_DAT_0066aaac >> 0x18 | _DAT_0066aab0 << 8) == 1) {
      _DAT_0066aaac = _DAT_0066aaac & 0xffffff;
      _DAT_0066aab0 = _DAT_0066aab0 & 0xff000000;
      _DAT_0066aa9c = _DAT_0066aa9c & 0xffffff;
      DAT_0066aaa0._0_1_ = 0;
      DAT_0066aaa0._1_1_ = 0;
      DAT_0066aaa0._2_1_ = 0;
      return;
    }
    return;
  }
  if (DAT_00182ab9 != 3) {
    return;
  }
  if (DAT_0066aa97 == 0 && (_DAT_0066aa98 & 0xffffff) == 0) {
    DAT_b100802c = 2;
  }
  else if ((DAT_0066aa28 == 0xffffffff) ||
          (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) != DAT_0066aa28)) {
    if ((DAT_0066aa2c == 0xffffffff) || (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) != DAT_0066aa2c))
    {
      if ((DAT_0066aa30 == 0xffffffff) ||
         (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) != DAT_0066aa30)) {
        if ((DAT_0066aa34 == 0xffffffff) ||
           (((uint)DAT_0066aa97 | _DAT_0066aa98 << 8) != DAT_0066aa34)) {
          if (((uint)DAT_0066aaa7 | _DAT_0066aaa8 << 8) == 1) {
            DAT_0066aaa7 = 0;
            _DAT_0066aaa8 = _DAT_0066aaa8 & 0xff000000;
            _DAT_0066aa9c = CONCAT13(1,_DAT_0066aa9c);
            DAT_0066aaa0._0_1_ = 0;
            DAT_0066aaa0._1_1_ = 0;
            DAT_0066aaa0._2_1_ = 0;
          }
        }
        else {
          DAT_b10080ec = 0x10;
        }
      }
      else {
        DAT_b10080c4 = 0x10;
      }
    }
    else {
      DAT_b100809c = 0x10;
    }
  }
  else {
    DAT_b1008074 = 0x10;
  }
  if ((_DAT_0066aaa8 >> 0x18 | _DAT_0066aaac << 8) == 1) {
    _DAT_0066aaa8 = _DAT_0066aaa8 & 0xffffff;
    _DAT_0066aaac = _DAT_0066aaac & 0xff000000;
    DAT_0066aaa0._3_1_ = 1;
    DAT_0066aaa4 = 0;
    DAT_0066aaa5 = 0;
    DAT_0066aaa6 = 0;
    if (((uint)DAT_0066aab7 | _DAT_0066aab8 << 8) == 1) {
      DAT_b1008024 = 1;
      return;
    }
    if (((uint)DAT_0066aab7 | _DAT_0066aab8 << 8) != 2) {
      if (((uint)DAT_0066aab7 | _DAT_0066aab8 << 8) == 3) {
        uVar1 = 3;
      }
      else if (((uint)DAT_0066aab7 | _DAT_0066aab8 << 8) == 4) {
        uVar1 = 4;
      }
      else {
        uVar1 = (uint)DAT_0066aab7 | _DAT_0066aab8 << 8;
        if (uVar1 != 5) {
          return;
        }
      }
      DAT_b1008024 = uVar1;
      return;
    }
    DAT_b1008024 = 2;
    return;
  }
  return;
}

// ==== FUN_0012e770 @ 0012e770, 120 bytes
// called by: FUN_0012ac38, FUN_0012b0a4, FUN_0012af60, FUN_0012acf8
undefined4 FUN_0012e770(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 | 1;
  DAT_b800c010 = 5;
  FUN_0012b324(0,0,8);
  FUN_0012abf0(0);
  do {
    FUN_00102888();
    DAT_b800c010 = 0xff;
    FUN_0012b324(0,0,8);
    FUN_0012abf0(0);
    uVar1 = DAT_b800c010;
  } while ((uVar1 & 1) != 0);
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 & 0xfe;
  return 0;
}

// ==== FUN_0012e7ec @ 0012e7ec, 148 bytes
// called by: FUN_0012acf8
int FUN_0012e7ec(void)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 in_r3;
  
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 | 1;
  DAT_b800c010 = 0x90;
  FUN_0012b324(0,0,8,in_r3,in_r3);
  FUN_0012abf0(0);
  DAT_b800c010 = 0;
  FUN_0012b324(0,0,0x18);
  FUN_0012abf0(0);
  DAT_b800c010 = 0xffff;
  FUN_0012b324(0,0,0x10);
  FUN_0012abf0(0);
  uVar2 = DAT_b800c010;
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 & 0xfe;
  return (int)(short)uVar2;
}

// ==== FUN_0012e888 @ 0012e888, 180 bytes
// called by: FUN_00128914
undefined4 * FUN_0012e888(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 | 1;
  DAT_b800c010 = 0x4b;
  FUN_0012b324(0,0,8);
  FUN_0012abf0(0);
  DAT_b800c010 = 0;
  FUN_0012b324(0,0,0x20);
  FUN_0012abf0(0);
  DAT_b800c010 = 0xffffffff;
  FUN_0012b324(0,0,0x20);
  FUN_0012abf0(0);
  DAT_00182a9c = DAT_b800c010;
  DAT_b800c010 = 0xffffffff;
  FUN_0012b324(0,0,0x20);
  FUN_0012abf0(0);
  DAT_00182aa0 = DAT_b800c010;
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 & 0xfe;
  return &DAT_00182a9c;
}

// ==== FUN_0012e944 @ 0012e944, 108 bytes
// called by: FUN_0012acf8
undefined4 FUN_0012e944(undefined4 param_1)

{
  uint uVar1;
  
  FUN_0012e9b4();
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 | 1;
  DAT_b800c010 = 1;
  FUN_0012b324(0,0,8);
  FUN_0012abf0(0);
  DAT_b800c010 = param_1;
  FUN_0012b324(0,0,8);
  FUN_0012abf0(0);
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 & 0xfe;
  return 0;
}

// ==== FUN_0012e9b4 @ 0012e9b4, 72 bytes
// called by: FUN_0012ac38, FUN_0012b0a4, FUN_0012e944, FUN_0012af60
undefined4 FUN_0012e9b4(void)

{
  uint uVar1;
  
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 | 1;
  DAT_b800c010 = 6;
  FUN_0012b324(0,0,8);
  FUN_0012abf0(0);
  uVar1 = DAT_b800c008;
  DAT_b800c008 = uVar1 & 0xfe;
  return 0;
}

// ==== FUN_0012ea00 @ 0012ea00, 24 bytes
undefined8 FUN_0012ea00(undefined4 param_1,undefined4 param_2,undefined *param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  int extraout_r1;
  int *piVar4;
  undefined4 in_lr;
  char *pcStack_28;
  int iStack_24;
  undefined *puStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_14;
  char *pcStack_10;
  int iStack_c;
  undefined4 uStack_4;
  
  uStack_4 = in_lr;
  FUN_001028b4();
  FUN_00102888();
  uStack_14 = uStack_4;
  uStack_4 = param_4;
  iStack_c = extraout_r1;
  pcStack_10 = "watch_dog_isr \n";
  pcStack_28 = "watch_dog_isr \n";
  iStack_24 = extraout_r1;
  puStack_20 = param_3;
  uStack_1c = param_4;
  if (DAT_00182da4 == -1) {
    iVar3 = FUN_00103d00();
    if ((iVar3 == 0x47) && (uVar2 = DAT_b0000004, (uVar2 & 4) == 4)) goto LAB_00103d5c;
  }
  else if (DAT_00182da4 == 0) goto LAB_00103d5c;
  DAT_00182d89 = 0;
  if (DAT_00182d88 == '\0') {
    FUN_00103060(1);
    pcStack_28 = (char *)0x1;
    iStack_24 = FUN_001014c8();
    iStack_24 = iStack_24 * 1000;
    puStack_20 = &DAT_0001c200;
    uStack_1c = 3;
    FUN_00103514(&pcStack_28);
  }
  piVar4 = &iStack_c;
  while (*pcStack_10 != '\0') {
    if (*pcStack_10 == '%') {
      pcStack_10 = (char *)FUN_00103ae4(pcStack_10 + 1,*piVar4);
      piVar4 = piVar4 + 1;
    }
    else {
      cVar1 = *pcStack_10;
      pcStack_10 = pcStack_10 + 1;
      FUN_001036e8(cVar1);
    }
  }
LAB_00103d5c:
  return CONCAT44(iStack_24,pcStack_28);
}

// ==== FUN_0012ea28 @ 0012ea28, 192 bytes
// called by: FUN_0011319c
uint FUN_0012ea28(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  bool bVar8;
  bool bVar9;
  
  if ((int)(param_1 ^ param_2) < 0) {
    param_2 = param_2 ^ 0x80000000;
    if (param_1 < param_2) {
      uVar2 = param_1 - param_2 ^ 0x80000000;
      param_1 = param_1 - uVar2;
      param_2 = param_2 + uVar2;
    }
    uVar2 = param_1 >> 0x17;
    uVar6 = uVar2 - (param_2 >> 0x17);
    if ((param_2 & 0x7f800000) == 0 || (uVar2 & 0xff) == 0xff) {
      if ((uVar2 & 0xff) != 0xff) {
        if ((param_1 & 0x7f800000) == 0) {
          param_1 = 0;
        }
        return param_1;
      }
                    /* WARNING: Subroutine does not return */
      FUN_0012ef9c();
    }
    uVar3 = param_1 << 8 | 0x80000000;
    uVar1 = param_2 << 8 | 0x80000000;
    uVar4 = uVar3 - (uVar1 >> (uVar6 & 0xff));
    if ((int)uVar4 < 0) {
      bVar9 = (bool)((byte)(uVar4 >> 7) & 1);
      uVar2 = (uVar4 >> 8) + (uVar2 - 1) * 0x800000 + (uint)bVar9;
      if (!bVar9) {
        return uVar2;
      }
      if ((uVar4 & 0x7f) != 0) {
        return uVar2;
      }
    }
    else {
      bVar8 = (uVar2 & 0xfe) == 0;
      bVar9 = false;
      if (!bVar8) {
        uVar3 = uVar4 * 2;
        bVar9 = (int)uVar3 < 0;
        bVar8 = uVar3 == 0;
      }
      if (!bVar9) {
        if (bVar8) {
          if (uVar4 >> 7 == 0) {
            return 0;
          }
        }
        else {
          uVar3 = uVar3 >> 2;
          if (uVar3 == 0) {
            return 0;
          }
          uVar1 = uVar2 - LZCOUNT(uVar3);
          uVar6 = 0x28U - LZCOUNT(uVar3) & 0x1f;
          if ((uVar2 ^ uVar1) >> 8 == 0) {
            return (uVar3 >> uVar6 | uVar3 << 0x20 - uVar6) + uVar1 * 0x800000;
          }
        }
        return param_1 & 0x80000000;
      }
      bVar9 = (bool)((byte)(uVar3 >> 7) & 1);
      uVar2 = ((int)uVar3 >> 8) + uVar2 * 0x800000 + (uint)bVar9;
      if (!bVar9) {
        return uVar2;
      }
      if ((uVar4 & 0x3f) != 0) {
        return uVar2;
      }
    }
    if (uVar1 << (0x20 - uVar6 & 0xff) == 0) {
      uVar2 = uVar2 & 0xfffffffe;
    }
    else {
      uVar2 = uVar2 - 1;
    }
    return uVar2;
  }
  iVar5 = param_1 - param_2;
  if (param_1 < param_2) {
    param_1 = param_1 - iVar5;
    param_2 = param_2 + iVar5;
  }
  uVar2 = param_1 >> 0x17;
  uVar6 = uVar2 - (param_2 >> 0x17);
  if ((param_2 & 0x7f800000) == 0 || (uVar2 & 0xff) == 0xff) {
    if ((uVar2 & 0xff) != 0xff) {
      if ((param_1 & 0x7f800000) == 0) {
        param_1 = param_1 & 0x80000000;
      }
      return param_1;
    }
                    /* WARNING: Subroutine does not return */
    FUN_0012ef9c();
  }
  uVar1 = param_1 << 8 | 0x80000000;
  uVar4 = param_2 << 8 | 0x80000000;
  uVar3 = uVar4 >> (uVar6 & 0xff);
  uVar7 = uVar1 + uVar3;
  if (CARRY4(uVar1,uVar3)) {
    uVar3 = uVar7 >> 1 | 0x80000000;
    bVar9 = (bool)((byte)(uVar3 >> 7) & 1);
    uVar3 = uVar3 >> 8;
    uVar2 = uVar3 + uVar2 * 0x800000 + (uint)bVar9;
    if (bVar9) {
      uVar3 = uVar7 & 0xff;
    }
    uVar1 = uVar2 * 2;
    if (uVar3 != 0) {
      if (uVar1 < 0xff000000) {
        return uVar2;
      }
      goto LAB_0012eaa0;
    }
  }
  else {
    bVar9 = (bool)((byte)(uVar7 >> 7) & 1);
    uVar2 = (uVar7 >> 8) + (uVar2 - 1) * 0x800000 + (uint)bVar9;
    if (!bVar9) {
      return uVar2;
    }
    uVar1 = uVar2 * 2;
    if ((uVar7 & 0x7f) != 0) {
      bVar9 = 0xfeffffff < uVar1;
    }
    if (!bVar9) {
      return uVar2;
    }
  }
  if (uVar4 << (0x20 - uVar6 & 0xff) == 0) {
    uVar2 = uVar2 & 0xfffffffe;
  }
  if (uVar1 < 0xff000000) {
    return uVar2;
  }
LAB_0012eaa0:
  return (uVar2 + 0xa0000000 >> 0x17 | 0xff) << 0x17;
}

// ==== FUN_0012eb14 @ 0012eb14, 296 bytes
// called by: FUN_0010584c, FUN_0011319c, FUN_00107b98
ulonglong FUN_0012eb14(uint param_1,uint param_2,undefined4 param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  bool bVar8;
  
  uVar3 = param_1 >> 7 & 0xff0000;
  bVar8 = uVar3 == 0;
  if (!bVar8) {
    param_4 = param_2 >> 7 & 0xff0000;
    bVar8 = param_4 == 0;
  }
  if (!bVar8) {
    bVar8 = uVar3 == 0xff0000;
  }
  if (!bVar8) {
    bVar8 = param_4 == 0xff0000;
  }
  if (bVar8) {
    if ((int)(param_1 ^ param_2) < 0) {
      uVar3 = uVar3 | 0x100;
    }
    uVar2 = param_2 >> 7 & 0xff0000;
    if (0xfeffff < uVar3 || 0xfeffff < uVar2) {
                    /* WARNING: Subroutine does not return */
      FUN_0012ef9c();
    }
    if (uVar2 != 0) {
      return CONCAT44(param_2,param_1 ^ param_2) & 0xffffffff80000000;
    }
    if ((uVar3 & 0xff0000) == 0) {
      return CONCAT44(param_2,0x7fc00000);
    }
    param_1 = param_1 ^ param_2;
  }
  else {
    uVar2 = param_1 & 0xffffff | 0x800000;
    uVar1 = param_2 & 0xffffff | 0x800000;
    uVar6 = (uint)*(byte *)(0x12ec5c - (uVar1 >> 0x11));
    if ((int)(param_1 ^ param_2) < 0) {
      uVar3 = uVar3 | 0x100;
    }
    bVar8 = uVar1 <= uVar2;
    uVar1 = -uVar1;
    iVar7 = uVar6 * 2;
    if (!bVar8) {
      uVar2 = uVar2 << 1;
    }
    iVar4 = (uVar3 - param_4) + 0x7d0000;
    uVar5 = iVar4 + (iVar4 >> 0x10) + (uint)bVar8;
    iVar7 = uVar6 * 0x100 + (((int)(uVar1 * iVar7) >> 4) * iVar7 >> 0x15);
    uVar3 = iVar7 * (uVar2 >> 8) >> 0x14;
    uVar6 = uVar1 * uVar3 + uVar2 * 0x800;
    uVar2 = iVar7 * (uVar6 >> 8) >> 0x13;
    uVar6 = uVar1 * uVar2 + uVar6 * 0x1000;
    bVar8 = CARRY4(uVar6,uVar1);
    if (bVar8) {
      uVar6 = uVar6 + uVar1;
    }
    iVar7 = uVar1 + uVar6 * 2;
    param_1 = uVar2 + uVar3 * 0x1000 + (uint)bVar8 + uVar5 * 0x800000 +
              (uint)CARRY4(uVar1,uVar6 * 2);
    if (uVar5 < 0xfc0000) {
      return CONCAT44(iVar7,param_1);
    }
    if ((int)(uVar5 - 0xfc0000) < 0) {
      if ((uVar5 & 0xf0) != 0) {
        param_1 = param_1 + 0x60000000 & 0x80000000;
      }
      return CONCAT44(iVar7,param_1);
    }
    if (0xfdffffff < param_1 * 2 + 0x1000000) {
      return CONCAT44(iVar7,param_1);
    }
    param_1 = param_1 + 0xa0000000;
  }
  return CONCAT44(0xff,(param_1 >> 0x17 | 0xff) << 0x17);
}

// ==== FUN_0012eca4 @ 0012eca4, 116 bytes
// called by: FUN_00107b98
void FUN_0012eca4(uint param_1,uint param_2)

{
  uint uVar1;
  bool bVar2;
  bool bVar3;
  
  uVar1 = param_1 | param_2;
  if ((int)uVar1 < 0) {
    if ((int)(uVar1 + 0x800000) < 0) {
      if (-1 < (int)(uVar1 - 0x800000)) {
        return;
      }
      return;
    }
    if (param_1 << 1 < 0xff000000 && param_2 << 1 < 0xff000000) {
      return;
    }
  }
  else {
    bVar3 = SCARRY4(uVar1,0x800000);
    bVar2 = -1 < (int)(uVar1 + 0x800000);
    if (bVar2) {
      bVar3 = SBORROW4(uVar1,0x800000);
    }
    if (bVar2 && -1 < (int)(uVar1 - 0x800000)) {
      return;
    }
    if (!bVar3) {
      return;
    }
    if (-1 < (int)(param_1 + 0x800000) && -1 < (int)(param_2 + 0x800000)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  FUN_0012ef9c();
}

// ==== FUN_0012ed28 @ 0012ed28, 44 bytes
// called by: FUN_0011319c
uint FUN_0012ed28(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = (param_1 & 0x7fffffff) >> 0x17;
  uVar2 = 0x9e - uVar1;
  if (uVar1 < 0x9f && uVar2 != 0) {
    uVar1 = ((param_1 | 0x800000) << 8) >> (uVar2 & 0xff);
    if ((int)(param_1 | 0x800000) < 0) {
      uVar1 = -uVar1;
    }
    return uVar1;
  }
                    /* WARNING: Subroutine does not return */
  FUN_0012ef9c();
}

// ==== FUN_0012ed6c @ 0012ed6c, 64 bytes
// called by: FUN_0010584c, FUN_00107b98
uint FUN_0012ed6c(uint param_1)

{
  if (param_1 >> 0x17 < 0x9f) {
    return (param_1 << 8 | 0x80000000) >> (0x9e - (param_1 >> 0x17) & 0xff);
  }
  if (((int)param_1 < 0) && (param_1 << 1 < 0x7f000000)) {
    return 0;
  }
                    /* WARNING: Subroutine does not return */
  FUN_0012ef9c();
}

// ==== FUN_0012edc0 @ 0012edc0, 48 bytes
// called by: FUN_0010584c, FUN_0011319c
uint FUN_0012edc0(uint param_1)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  uVar2 = param_1 & 0x80000000;
  if ((int)uVar2 < 0) {
    param_1 = -param_1;
  }
  uVar1 = param_1 << LZCOUNT(param_1);
  if (uVar1 == 0) {
    return param_1;
  }
  bVar3 = (bool)((byte)(uVar1 >> 7) & 1);
  uVar2 = (uVar2 | (0x9d - LZCOUNT(param_1)) * 0x800000) + (uVar1 >> 8) + (uint)bVar3;
  if (!bVar3) {
    return uVar2;
  }
  if ((uVar1 & 0x7f) == 0) {
    uVar2 = uVar2 & 0xfffffffe;
  }
  return uVar2;
}

// ==== FUN_0012edf0 @ 0012edf0, 40 bytes
// called by: FUN_0010584c, FUN_00107b98
uint FUN_0012edf0(uint param_1)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  uVar1 = param_1 << LZCOUNT(param_1);
  if (uVar1 == 0) {
    return param_1;
  }
  bVar3 = (bool)((byte)(uVar1 >> 7) & 1);
  uVar2 = (0x9d - LZCOUNT(param_1)) * 0x800000 + (uVar1 >> 8) + (uint)bVar3;
  if (!bVar3) {
    return uVar2;
  }
  if ((uVar1 & 0x7f) == 0) {
    uVar2 = uVar2 & 0xfffffffe;
  }
  return uVar2;
}

// ==== FUN_0012ee18 @ 0012ee18, 116 bytes
// called by: FUN_00107b98
void FUN_0012ee18(uint param_1,uint param_2)

{
  uint uVar1;
  bool bVar2;
  bool bVar3;
  
  uVar1 = param_1 | param_2;
  if ((int)uVar1 < 0) {
    if ((int)(uVar1 + 0x800000) < 0) {
      if (-1 < (int)(uVar1 - 0x800000)) {
        return;
      }
      return;
    }
    if (param_1 << 1 < 0xff000000 && param_2 << 1 < 0xff000000) {
      return;
    }
  }
  else {
    bVar3 = SCARRY4(uVar1,0x800000);
    bVar2 = -1 < (int)(uVar1 + 0x800000);
    if (bVar2) {
      bVar3 = SBORROW4(uVar1,0x800000);
    }
    if (bVar2 && -1 < (int)(uVar1 - 0x800000)) {
      return;
    }
    if (!bVar3) {
      return;
    }
    if (-1 < (int)(param_1 + 0x800000) && -1 < (int)(param_2 + 0x800000)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  FUN_0012ef9c();
}

// ==== FUN_0012ee9c @ 0012ee9c, 216 bytes
// called by: FUN_0011319c, FUN_00107b98
uint FUN_0012ee9c(uint param_1,uint param_2,undefined4 param_3,uint param_4)

{
  longlong lVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  
  uVar2 = param_1 >> 7 & 0xff0000;
  bVar6 = uVar2 == 0;
  if (!bVar6) {
    param_4 = param_2 >> 7 & 0xff0000;
    bVar6 = param_4 == 0;
  }
  if (!bVar6) {
    bVar6 = uVar2 == 0xff0000;
  }
  if (!bVar6) {
    bVar6 = param_4 == 0xff0000;
  }
  if (bVar6) {
    if ((int)(param_1 ^ param_2) < 0) {
      uVar2 = uVar2 | 0x100;
    }
    if (uVar2 < 0xff0000 && (param_2 >> 7 & 0xff0000) < 0xff0000) {
      return (param_1 ^ param_2) & 0x80000000;
    }
                    /* WARNING: Subroutine does not return */
    FUN_0012ef9c();
  }
  if ((int)(param_1 ^ param_2) < 0) {
    uVar2 = uVar2 | 0x100;
  }
  lVar1 = (ulonglong)(param_2 << 8 | 0x80000000) * (ulonglong)(param_1 << 8 | 0x80000000);
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  iVar3 = uVar2 + param_4 + -0x800000;
  if ((int)lVar1 != 0) {
    uVar5 = uVar5 | 1;
  }
  bVar6 = SUB41(uVar5 >> 0x1f,0);
  uVar2 = uVar5 << 1;
  uVar4 = iVar3 + (iVar3 >> 0x10) + (uint)bVar6;
  if (bVar6) {
    uVar2 = uVar5 & 0x7fffffff | 0x80000000;
  }
  bVar6 = (bool)((byte)(uVar2 >> 7) & 1);
  uVar5 = uVar2 >> 8;
  if (bVar6) {
    uVar5 = uVar2 & 0x7f;
  }
  uVar2 = (uVar2 >> 8) + uVar4 * 0x800000 + (uint)bVar6;
  if (uVar5 != 0) {
    bVar6 = 0xfbffff < uVar4;
  }
  if (!bVar6) {
    return uVar2;
  }
  if (uVar5 == 0 || uVar4 == 0xfc0000) {
    uVar2 = uVar2 & 0xfffffffe;
  }
  if (uVar4 < 0xfc0000) {
    return uVar2;
  }
  if (-1 < (int)(uVar4 - 0xfc0000)) {
    if ((int)((uVar2 + 0x800000) * 2) < 0) {
      return uVar2;
    }
    return (uVar2 + 0xa0000000 >> 0x17 | 0xff) << 0x17;
  }
  if (-0x10000 < (int)(uVar4 ^ 0xff)) {
    return uVar2;
  }
  uVar2 = uVar2 + 0x60000000;
  if ((int)(uVar4 ^ 0xff) < -0xffff) {
    uVar2 = uVar2 & 0x80000000;
  }
  return uVar2;
}

// ==== FUN_0012ef9c @ 0012ef9c, 156 bytes
// called by: FUN_0012ea28, FUN_0012ed6c, FUN_0012eca4, FUN_0012ed28, FUN_0012eb14, FUN_0012ee9c, FUN_0012f0c8, FUN_0012ee18
uint FUN_0012ef9c(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint *in_lr;
  bool bVar2;
  
  if ((((int)*in_lr < 0) || (param_3 = param_2 * 2, param_3 < 0xff000001)) &&
     (param_1 * 2 < 0xff000001)) {
    if (param_1 * 2 == 0xff000000) {
      uVar1 = ((int)param_1 >> 0x1f) * -3 + 2;
      if (param_3 == 0xff000000) {
        uVar1 = (uVar1 - ((int)param_2 >> 0x1f)) + 1;
      }
    }
    else {
      uVar1 = param_2 >> 0x1f;
    }
  }
  else {
    uVar1 = 8;
  }
  uVar1 = *in_lr >> uVar1 * 3 & 7;
  switch(uVar1) {
  case 4:
    break;
  case 5:
    param_2 = param_1;
    break;
  case 6:
  case 7:
    return 0x7fc00000;
  default:
                    /* WARNING: Could not recover jumptable at 0x0012f004. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (*(code *)(in_lr + uVar1 + 1))();
    return uVar1;
  }
  bVar2 = SUB41(param_2 >> 0x1f,0);
  uVar1 = param_2 * 2;
  if (uVar1 != 0) {
    bVar2 = uVar1 < 0x1000001;
  }
  if (bVar2 && (uVar1 != 0 && uVar1 != 0x1000000)) {
    param_2 = param_2 & 0x80000000;
  }
  return param_2;
}

// ==== FUN_0012f04c @ 0012f04c, 124 bytes
// called by: FUN_00107b98
void FUN_0012f04c(uint param_1,uint param_2)

{
  uint uVar1;
  bool bVar2;
  bool bVar3;
  
  uVar1 = param_2 | param_1;
  if ((int)uVar1 < 0) {
    if ((int)(uVar1 + 0x800000) < 0) {
      if (-1 < (int)(uVar1 - 0x800000)) {
        return;
      }
      return;
    }
    if (param_2 << 1 < 0xff000000 && param_1 << 1 < 0xff000000) {
      return;
    }
  }
  else {
    bVar3 = SCARRY4(uVar1,0x800000);
    bVar2 = -1 < (int)(uVar1 + 0x800000);
    if (bVar2) {
      bVar3 = SBORROW4(uVar1,0x800000);
    }
    if (bVar2 && -1 < (int)(uVar1 - 0x800000)) {
      return;
    }
    if (!bVar3) {
      return;
    }
    if (-1 < (int)(param_2 + 0x800000) && -1 < (int)(param_1 + 0x800000)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  FUN_0012ef9c(param_2);
}

// ==== FUN_0012f0c8 @ 0012f0c8, 244 bytes
// called by: FUN_0011319c
uint FUN_0012f0c8(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  bool bVar8;
  bool bVar9;
  
  if (-1 < (int)(param_1 ^ param_2)) {
    if (param_1 < param_2) {
      uVar4 = param_1 - param_2 ^ 0x80000000;
      param_1 = param_1 - uVar4;
      param_2 = param_2 + uVar4;
    }
    uVar4 = param_1 >> 0x17;
    uVar5 = uVar4 - (param_2 >> 0x17);
    if ((param_2 & 0x7f800000) == 0 || (uVar4 & 0xff) == 0xff) {
      if ((uVar4 & 0xff) != 0xff) {
        if ((param_1 & 0x7f800000) == 0) {
          param_1 = 0;
        }
        return param_1;
      }
                    /* WARNING: Subroutine does not return */
      FUN_0012ef9c();
    }
    uVar1 = param_1 << 8 | 0x80000000;
    uVar2 = param_2 << 8 | 0x80000000;
    uVar7 = uVar1 - (uVar2 >> (uVar5 & 0xff));
    if ((int)uVar7 < 0) {
      bVar8 = (bool)((byte)(uVar7 >> 7) & 1);
      uVar4 = (uVar7 >> 8) + (uVar4 - 1) * 0x800000 + (uint)bVar8;
      if (!bVar8) {
        return uVar4;
      }
      if ((uVar7 & 0x7f) != 0) {
        return uVar4;
      }
    }
    else {
      bVar9 = (uVar4 & 0xfe) == 0;
      bVar8 = false;
      if (!bVar9) {
        uVar1 = uVar7 * 2;
        bVar8 = (int)uVar1 < 0;
        bVar9 = uVar1 == 0;
      }
      if (!bVar8) {
        if (bVar9) {
          if (uVar7 >> 7 == 0) {
            return 0;
          }
        }
        else {
          uVar1 = uVar1 >> 2;
          if (uVar1 == 0) {
            return 0;
          }
          uVar2 = uVar4 - LZCOUNT(uVar1);
          uVar5 = 0x28U - LZCOUNT(uVar1) & 0x1f;
          if ((uVar4 ^ uVar2) >> 8 == 0) {
            return (uVar1 >> uVar5 | uVar1 << 0x20 - uVar5) + uVar2 * 0x800000;
          }
        }
        return param_1 & 0x80000000;
      }
      bVar8 = (bool)((byte)(uVar1 >> 7) & 1);
      uVar4 = ((int)uVar1 >> 8) + uVar4 * 0x800000 + (uint)bVar8;
      if (!bVar8) {
        return uVar4;
      }
      if ((uVar7 & 0x3f) != 0) {
        return uVar4;
      }
    }
    if (uVar2 << (0x20 - uVar5 & 0xff) == 0) {
      uVar4 = uVar4 & 0xfffffffe;
    }
    else {
      uVar4 = uVar4 - 1;
    }
    return uVar4;
  }
  param_2 = param_2 ^ 0x80000000;
  iVar3 = param_1 - param_2;
  if (param_1 < param_2) {
    param_1 = param_1 - iVar3;
    param_2 = param_2 + iVar3;
  }
  uVar4 = param_1 >> 0x17;
  uVar5 = uVar4 - (param_2 >> 0x17);
  if ((param_2 & 0x7f800000) == 0 || (uVar4 & 0xff) == 0xff) {
    if ((uVar4 & 0xff) != 0xff) {
      if ((param_1 & 0x7f800000) == 0) {
        param_1 = param_1 & 0x80000000;
      }
      return param_1;
    }
                    /* WARNING: Subroutine does not return */
    FUN_0012ef9c();
  }
  uVar2 = param_1 << 8 | 0x80000000;
  uVar7 = param_2 << 8 | 0x80000000;
  uVar1 = uVar7 >> (uVar5 & 0xff);
  uVar6 = uVar2 + uVar1;
  if (CARRY4(uVar2,uVar1)) {
    uVar1 = uVar6 >> 1 | 0x80000000;
    bVar8 = (bool)((byte)(uVar1 >> 7) & 1);
    uVar1 = uVar1 >> 8;
    uVar4 = uVar1 + uVar4 * 0x800000 + (uint)bVar8;
    if (bVar8) {
      uVar1 = uVar6 & 0xff;
    }
    uVar2 = uVar4 * 2;
    if (uVar1 != 0) {
      if (uVar2 < 0xff000000) {
        return uVar4;
      }
      goto LAB_0012eaa0;
    }
  }
  else {
    bVar8 = (bool)((byte)(uVar6 >> 7) & 1);
    uVar4 = (uVar6 >> 8) + (uVar4 - 1) * 0x800000 + (uint)bVar8;
    if (!bVar8) {
      return uVar4;
    }
    uVar2 = uVar4 * 2;
    if ((uVar6 & 0x7f) != 0) {
      bVar8 = 0xfeffffff < uVar2;
    }
    if (!bVar8) {
      return uVar4;
    }
  }
  if (uVar7 << (0x20 - uVar5 & 0xff) == 0) {
    uVar4 = uVar4 & 0xfffffffe;
  }
  if (uVar2 < 0xff000000) {
    return uVar4;
  }
LAB_0012eaa0:
  return (uVar4 + 0xa0000000 >> 0x17 | 0xff) << 0x17;
}

// ==== FUN_0012f23c @ 0012f23c, 276 bytes
// called by: FUN_0010584c
/* WARNING: Removing unreachable block (ram,0x0012f2a0) */
/* WARNING: Removing unreachable block (ram,0x0012f2b4) */
/* WARNING: Removing unreachable block (ram,0x0012f200) */
/* WARNING: Removing unreachable block (ram,0x0012f1fc) */
/* WARNING: Removing unreachable block (ram,0x0012f204) */
/* WARNING: Removing unreachable block (ram,0x0012f1d0) */
/* WARNING: Removing unreachable block (ram,0x0012f1e8) */
/* WARNING: Removing unreachable block (ram,0x0012f1ec) */
/* WARNING: Removing unreachable block (ram,0x0012f1f0) */
/* WARNING: Removing unreachable block (ram,0x0012f208) */
/* WARNING: Removing unreachable block (ram,0x0012f210) */
/* WARNING: Removing unreachable block (ram,0x0012f224) */
/* WARNING: Removing unreachable block (ram,0x0012f298) */
/* WARNING: Removing unreachable block (ram,0x0012f2cc) */
/* WARNING: Removing unreachable block (ram,0x0012f2a8) */

ulonglong FUN_0012f23c(uint param_1,uint param_2,undefined4 param_3,uint param_4)

{
  uint uVar1;
  char in_OV;
  bool bVar2;
  bool bVar3;
  
  uVar1 = param_1 >> 0x17 & 0xff;
  bVar2 = uVar1 == 0;
  if (!bVar2) {
    param_4 = uVar1 ^ 0xff;
    bVar2 = param_4 == 0;
  }
  bVar3 = bVar2;
  if (!bVar2) {
    in_OV = SBORROW4(param_4,param_2);
    bVar3 = param_4 == param_2;
  }
  bVar2 = !bVar2 && (int)(param_4 - param_2) < 0;
  if (!bVar3 && bVar2 == (bool)in_OV) {
    in_OV = SCARRY4(param_2,uVar1);
    bVar2 = (int)(param_2 + uVar1) < 0;
    bVar3 = param_2 + uVar1 == 0;
  }
  if (!bVar3 && bVar2 == (bool)in_OV) {
    return CONCAT44(param_2,param_1 + param_2 * 0x800000);
  }
  if (uVar1 == 0) {
    return CONCAT44(param_2,param_1) & 0xffffffff80000000;
  }
  if (param_4 == 0) {
    if ((param_1 & 0x7fffff) == 0) {
      return CONCAT44(param_2,param_1);
    }
    return CONCAT44(param_2,0x7fc00000);
  }
  param_1 = param_1 & 0x80000000;
  if (-1 < (int)param_2) {
    param_1 = param_1 | 0x7f800000;
  }
  return CONCAT44(param_2,param_1);
}

