// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x0010c000-0x0010ffff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== FUN_0010c414 @ 0010c414, 404 bytes
// called by: FUN_001083d0
undefined4 FUN_0010c414(undefined4 *param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  
  local_1c = DAT_00181e44;
  local_18 = DAT_00181e48;
  local_20 = DAT_00181e4c;
  local_24 = DAT_00181e50;
  local_28 = DAT_00181e54;
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 | 0x9000000;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 | 0x400;
  uVar1 = DAT_b0000014;
  DAT_b0000014 = uVar1 & 0xfffffbff;
  uVar2 = FUN_0010b830(3,&DAT_00005dc0);
  iVar3 = FUN_0010d398(uVar2,&DAT_00005dc0);
  uVar1 = DAT_b0000210;
  DAT_b0000210 = uVar1 & 0xfffffff8;
  uVar1 = DAT_b0000210;
  DAT_b0000210 = uVar1 & 0xffff00ff | (iVar3 - 1U & 0xff) << 8;
  uVar1 = DAT_b0000210;
  DAT_b0000210 = uVar1 & 0xffffffe7;
  uVar1 = DAT_b0000210;
  DAT_b0000210 = uVar1 | 0x18;
  FUN_00108484();
  FUN_001088c4(4);
  lcm_init();
  FUN_00108fd4(1);
  FUN_001087bc(0);
  FUN_00108784(1);
  FUN_00108854(2);
  FUN_00108da4(&local_20);
  FUN_00108df8(&local_24);
  FUN_00108e28(&local_1c);
  FUN_00108e8c(&local_28);
  if (param_2 == 0) {
    iVar3 = FUN_001085e8(param_1);
    if (iVar3 == 0) {
      return 0xfff06004;
    }
  }
  else {
    FUN_00108690(param_2);
  }
  FUN_00108874(*param_1);
  FUN_001088a4(0);
  FUN_0010846c();
  dbg_printf("2222222222222222222222222222222222222\n");
  return 0;
}

// ==== FUN_0010c5a8 @ 0010c5a8, 40 bytes
// called by: FUN_001083ec
undefined4 FUN_0010c5a8(void)

{
  uint uVar1;
  
  FUN_00108484();
  FUN_001086bc();
  uVar1 = DAT_b0000204;
  DAT_b0000204 = uVar1 & 0xf7ffffff;
  return 0;
}

// ==== mem_alloc @ 0010c614, 98 bytes
// called by: FUN_001085e8, lcd_boot_proc, idle_screen_proc, FUN_00106ac8, usb_command
// ? returns the buffer a file is read into; freed by mem_free
/* ? returns the buffer a file is read into; freed by mem_free */

uint * mem_alloc(uint param_1)

{
  uint *puVar1;
  uint *puVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  uint *puVar7;
  
  puVar3 = (undefined4 *)FUN_0010cfbc();
  puVar7 = (uint *)*puVar3;
  uVar6 = param_1 + 0xb & 0xfffffff8;
  if (uVar6 <= param_1) {
    return (uint *)0x0;
  }
  do {
    puVar1 = puVar7;
    for (puVar2 = (uint *)puVar7[1]; puVar2 != (uint *)0x0; puVar2 = (uint *)puVar2[1]) {
      if (uVar6 <= *puVar2) {
        if (*puVar2 < uVar6 + 8) {
          puVar1[1] = puVar2[1];
        }
        else {
          piVar5 = (int *)((int)puVar2 + uVar6);
          piVar5[1] = puVar2[1];
          *piVar5 = *puVar2 - uVar6;
          puVar1[1] = (uint)piVar5;
          *puVar2 = uVar6;
        }
        return puVar2 + 1;
      }
      puVar1 = puVar2;
    }
    iVar4 = FUN_0010cfd0(puVar7,uVar6);
  } while (iVar4 != 0);
  return (uint *)0x0;
}

// ==== mem_free @ 0010c678, 8 bytes
// called by: idle_screen_proc, usb_command
void mem_free(int param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  
  piVar3 = (int *)(param_1 + -4);
  piVar1 = (int *)FUN_0010cfbc();
  if (param_1 != 0) {
    piVar2 = (int *)*piVar1;
    for (piVar1 = (int *)((int *)*piVar1)[1]; (piVar1 != (int *)0x0 && (piVar1 < piVar3));
        piVar1 = (int *)piVar1[1]) {
      piVar2 = piVar1;
    }
    if ((int *)(*piVar2 + (int)piVar2) == piVar3) {
      *piVar2 = *piVar2 + *piVar3;
      piVar3 = piVar2;
    }
    else {
      piVar2[1] = (int)piVar3;
    }
    if ((int *)(*piVar3 + (int)piVar3) != piVar1) {
      piVar3[1] = (int)piVar1;
      return;
    }
    piVar3[1] = piVar1[1];
    *piVar3 = *piVar3 + *piVar1;
  }
  return;
}

// ==== mem_free @ 0010c680, 76 bytes
// called by: FUN_0010d208, lcd_boot_proc, FUN_001086bc, idle_screen_proc, FUN_00106c34
// ?
/* ? */

void mem_free(int param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  
  piVar3 = (int *)(param_1 + -4);
  piVar1 = (int *)FUN_0010cfbc();
  if (param_1 != 0) {
    piVar2 = (int *)*piVar1;
    for (piVar1 = (int *)((int *)*piVar1)[1]; (piVar1 != (int *)0x0 && (piVar1 < piVar3));
        piVar1 = (int *)piVar1[1]) {
      piVar2 = piVar1;
    }
    if ((int *)(*piVar2 + (int)piVar2) == piVar3) {
      *piVar2 = *piVar2 + *piVar3;
      piVar3 = piVar2;
    }
    else {
      piVar2[1] = (int)piVar3;
    }
    if ((int *)(*piVar3 + (int)piVar3) != piVar1) {
      piVar3[1] = (int)piVar1;
      return;
    }
    piVar3[1] = piVar1[1];
    *piVar3 = *piVar3 + *piVar1;
  }
  return;
}

// ==== sprintf @ 0010c6cc, 36 bytes
// called by: idle_screen_proc
// sprintf(buffer, "%d", value) in idle_screen_proc
/* sprintf(buffer, "%d", value) in idle_screen_proc */

void sprintf(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  local_20 = param_1;
  uStack_1c = param_4;
  uStack_8 = param_3;
  uStack_4 = param_4;
  FUN_0010d11e(param_2,&local_20,&uStack_8,0x10d145);
  FUN_0010d144(0,&local_20);
  return;
}

// ==== FUN_0010c768 @ 0010c768, 306 bytes
// called by: FUN_0010d11e
undefined8 FUN_0010c768(uint *param_1,uint *param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  code *pcVar4;
  int iVar5;
  uint *local_20;
  uint *local_18;
  
  uVar1 = 0;
  local_18 = param_2;
  do {
    param_1[8] = uVar1;
LAB_0010c772:
    uVar1 = (*(code *)param_1[3])(param_1);
    if (uVar1 == 0) {
LAB_0010c896:
      return CONCAT44(local_20,param_1[8]);
    }
    if (uVar1 != 0x25) {
      pcVar4 = (code *)param_1[1];
      uVar3 = param_1[2];
      goto LAB_0010c88e;
    }
    uVar3 = 0;
    while (((uVar1 = (*(code *)param_1[3])(param_1), 0x1f < (int)uVar1 && (uVar1 < 0x31)) &&
           (*(byte *)((int)&DAT_00182384 + uVar1) != 0))) {
      uVar3 = uVar3 | *(byte *)((int)&DAT_00182384 + uVar1);
    }
    if ((int)(uVar3 << 0x1e) < 0) {
      uVar3 = uVar3 & 0xfffffffb;
    }
    param_1[7] = 0;
    iVar5 = 0;
    param_1[6] = 0;
    do {
      if (uVar1 == 0x2a) {
        uVar1 = *local_18;
        local_18 = local_18 + 1;
        param_1[iVar5 + 6] = uVar1;
        uVar1 = (*(code *)param_1[3])(param_1);
        if (iVar5 == 1) {
          if ((int)param_1[7] < 0) {
            uVar3 = uVar3 & 0xffffffdf;
          }
          break;
        }
      }
      else {
        iVar2 = FUN_00126f5c(uVar1);
        if (iVar2 != 0) {
          local_20 = param_1 + iVar5;
          local_20[6] = uVar1 - 0x30;
          while( true ) {
            uVar1 = (*(code *)param_1[3])(param_1);
            iVar2 = FUN_00126f5c();
            if (iVar2 == 0) break;
            local_20[6] = (local_20[6] * 10 + uVar1) - 0x30;
          }
        }
        if (iVar5 == 1) break;
      }
      if (uVar1 != 0x2e) break;
      uVar1 = (*(code *)param_1[3])(param_1);
      uVar3 = uVar3 | 0x20;
      iVar5 = iVar5 + 1;
    } while (iVar5 < 2);
    if ((int)param_1[6] < 0) {
      param_1[6] = -param_1[6];
      uVar3 = uVar3 | 1;
    }
    if ((uVar3 & 1) != 0) {
      uVar3 = uVar3 & 0xffffffef;
    }
    if (uVar1 == 0) goto LAB_0010c896;
    if (uVar1 - 0x41 < 0x1a) {
      uVar3 = uVar3 | 0x800;
      uVar1 = uVar1 + 0x20;
    }
    *param_1 = uVar3;
    iVar5 = FUN_001001ac(param_1,uVar1);
    if (iVar5 != 0) {
      if (iVar5 == 1) {
        local_18 = local_18 + 1;
      }
      else {
        local_18 = (uint *)(((int)local_18 + 7U & 0xfffffff8) + 8);
      }
      goto LAB_0010c772;
    }
    pcVar4 = (code *)param_1[1];
    uVar3 = param_1[2];
LAB_0010c88e:
    (*pcVar4)(uVar1,uVar3);
    uVar1 = param_1[8] + 1;
  } while( true );
}

// ==== FUN_0010c8a0 @ 0010c8a0, 116 bytes
// called by: usb_command
int FUN_0010c8a0(uint *param_1,uint *param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((((uint)param_1 | (uint)param_2) & 3) == 0) {
    while (3 < param_3) {
      uVar2 = *param_1;
      param_1 = param_1 + 1;
      uVar3 = *param_2;
      param_2 = param_2 + 1;
      param_3 = param_3 - 4;
      if (uVar2 != uVar3) {
        iVar1 = (uVar2 & 0xff) - (uVar3 & 0xff);
        if (((iVar1 == 0) && (iVar1 = (uVar2 & 0xffff) - (uVar3 & 0xffff), iVar1 == 0)) &&
           (iVar1 = (uVar2 & 0xffffff) - (uVar3 & 0xffffff), iVar1 == 0)) {
          iVar1 = (uVar2 >> 0x18) - (uVar3 >> 0x18);
        }
        return iVar1;
      }
    }
  }
  if (param_3 != 0) {
    if ((param_3 & 1) == 0) goto LAB_0010c8f4;
    param_3 = param_3 + 1;
    while( true ) {
      uVar2 = *param_1;
      uVar3 = *param_2;
      param_1 = (uint *)((int)param_1 + 1);
      param_2 = (uint *)((int)param_2 + 1);
      iVar1 = (uint)(byte)uVar2 - (uint)(byte)uVar3;
      if (iVar1 != 0) {
        return iVar1;
      }
      param_3 = param_3 - 2;
      if (param_3 == 0) break;
LAB_0010c8f4:
      uVar2 = *param_1;
      uVar3 = *param_2;
      param_1 = (uint *)((int)param_1 + 1);
      param_2 = (uint *)((int)param_2 + 1);
      iVar1 = (uint)(byte)uVar2 - (uint)(byte)uVar3;
      if (iVar1 != 0) {
        return iVar1;
      }
    }
    return 0;
  }
  return 0;
}

// ==== FUN_0010c91c @ 0010c91c, 72 bytes
// called by: config_load, config_defaults
void FUN_0010c91c(uint *param_1,uint *param_2)

{
  char cVar1;
  uint uVar2;
  
  if ((((uint)param_1 | (uint)param_2) & 3) == 0) {
    while( true ) {
      uVar2 = *param_2;
      param_2 = param_2 + 1;
      if ((uVar2 + 0xfefefeff & ~uVar2 & 0x80808080) != 0) break;
      *param_1 = uVar2;
      param_1 = param_1 + 1;
    }
    while( true ) {
      *(char *)param_1 = (char)uVar2;
      param_1 = (uint *)((int)param_1 + 1);
      if ((uVar2 & 0xff) == 0) break;
      uVar2 = uVar2 >> 8;
    }
  }
  else {
    do {
      uVar2 = *param_2;
      *(char *)param_1 = (char)uVar2;
      if ((char)uVar2 == '\0') {
        return;
      }
      cVar1 = *(char *)((int)param_2 + 1);
      param_2 = (uint *)((int)param_2 + 2);
      *(char *)((int)param_1 + 1) = cVar1;
      param_1 = (uint *)((int)param_1 + 2);
    } while (cVar1 != '\0');
  }
  return;
}

// ==== FUN_0010c968 @ 0010c968, 62 bytes
// called by: FUN_00103ae4
int FUN_0010c968(uint *param_1)

{
  int iVar1;
  uint uVar2;
  char *pcVar3;
  
  pcVar3 = (char *)((int)param_1 + 1);
  while (((uint)param_1 & 3) != 0) {
    uVar2 = *param_1;
    param_1 = (uint *)((int)param_1 + 1);
    if ((char)uVar2 == '\0') {
      return (int)param_1 - (int)pcVar3;
    }
  }
  do {
    uVar2 = *param_1;
    param_1 = param_1 + 1;
    uVar2 = uVar2 + 0xfefefeff & ~uVar2;
  } while ((uVar2 & 0x80808080) == 0);
  iVar1 = (int)param_1 - (int)pcVar3;
  if ((uVar2 & 0x80) == 0) {
    if ((uVar2 & 0x8080) == 0) {
      if ((uVar2 & 0x808080) != 0) {
        return iVar1 + -1;
      }
    }
    else {
      iVar1 = iVar1 + -2;
    }
    return iVar1;
  }
  return iVar1 + -3;
}

// ==== FUN_0010c9ac @ 0010c9ac, 12 bytes
undefined4 FUN_0010c9ac(undefined4 param_1)

{
  FUN_0010c9b8();
  return param_1;
}

// ==== FUN_0010c9b8 @ 0010c9b8, 212 bytes
// called by: FUN_0010d558, FUN_0012876c, usb_receive_task, usb_receive_data, FUN_00109e94, FUN_00115498, sys_set_clock_divider, FUN_00100d90, FUN_00128914, FUN_0012b0a4, FUN_00101414, FUN_0010d63c, FUN_0012a720, FUN_0011551c, FUN_0010c9ac, usb_command
undefined8 FUN_0010c9b8(uint *param_1,uint *param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  byte *pbVar4;
  uint *puVar5;
  byte bVar6;
  undefined2 uVar7;
  byte bVar8;
  uint in_r12;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  
  if (3 < param_3) {
    uVar9 = (uint)param_1 & 3;
    in_r12 = uVar9;
    if (uVar9 != 0) {
      bVar6 = (byte)*param_2;
      puVar3 = (uint *)((int)param_2 + 1);
      if (uVar9 < 3) {
        puVar3 = (uint *)((int)param_2 + 2);
        in_r12 = (uint)*(byte *)((int)param_2 + 1);
      }
      *(byte *)param_1 = bVar6;
      param_2 = puVar3;
      if (uVar9 < 2) {
        param_2 = (uint *)((int)puVar3 + 1);
        bVar6 = (byte)*puVar3;
      }
      puVar3 = (uint *)((int)param_1 + 1);
      if (uVar9 < 3) {
        puVar3 = (uint *)((int)param_1 + 2);
        *(byte *)((int)param_1 + 1) = (byte)in_r12;
      }
      param_3 = (param_3 + uVar9) - 4;
      param_1 = puVar3;
      if (uVar9 < 2) {
        param_1 = (uint *)((int)puVar3 + 1);
        *(byte *)puVar3 = bVar6;
      }
    }
    param_4 = (uint)param_2 & 3;
    if (param_4 == 0) {
      uVar9 = 0;
      while (uVar1 = param_3 - 0x20, 0x1f < param_3) {
        uVar9 = param_2[1];
        uVar10 = param_2[2];
        uVar11 = param_2[3];
        *param_1 = *param_2;
        param_1[1] = uVar9;
        param_1[2] = uVar10;
        param_1[3] = uVar11;
        uVar9 = param_2[4];
        uVar10 = param_2[5];
        uVar11 = param_2[6];
        uVar12 = param_2[7];
        param_2 = param_2 + 8;
        param_1[4] = uVar9;
        param_1[5] = uVar10;
        param_1[6] = uVar11;
        param_1[7] = uVar12;
        param_1 = param_1 + 8;
        param_3 = uVar1;
      }
      if ((bool)((byte)(uVar1 >> 4) & 1)) {
        uVar9 = *param_2;
        uVar10 = param_2[1];
        uVar11 = param_2[2];
        uVar12 = param_2[3];
        param_2 = param_2 + 4;
        *param_1 = uVar9;
        param_1[1] = uVar10;
        param_1[2] = uVar11;
        param_1[3] = uVar12;
        param_1 = param_1 + 4;
      }
      if ((int)(param_3 << 0x1c) < 0) {
        uVar9 = *param_2;
        uVar10 = param_2[1];
        param_2 = param_2 + 2;
        *param_1 = uVar9;
        param_1[1] = uVar10;
        param_1 = param_1 + 2;
      }
      puVar2 = param_1;
      puVar3 = param_2;
      if ((bool)((byte)(uVar1 >> 2) & 1)) {
        puVar3 = param_2 + 1;
        uVar9 = *param_2;
        puVar2 = param_1 + 1;
        *param_1 = uVar9;
      }
      uVar7 = (undefined2)uVar9;
      if ((uVar1 & 3) == 0) {
        return CONCAT44(puVar3,puVar2);
      }
      bVar14 = (bool)((byte)(uVar1 >> 1) & 1);
      param_3 = param_3 << 0x1f;
      bVar13 = (int)param_3 < 0;
      puVar5 = puVar3;
      if (bVar14) {
        puVar5 = (uint *)((int)puVar3 + 2);
        uVar7 = (undefined2)*puVar3;
      }
      puVar3 = puVar5;
      if (bVar13) {
        puVar3 = (uint *)((int)puVar5 + 1);
        param_3 = (uint)(byte)*puVar5;
      }
      puVar5 = puVar2;
      if (bVar14) {
        puVar5 = (uint *)((int)puVar2 + 2);
        *(undefined2 *)puVar2 = uVar7;
      }
      puVar2 = puVar5;
      if (bVar13) {
        puVar2 = (uint *)((int)puVar5 + 1);
        *(byte *)puVar5 = (byte)param_3;
      }
      return CONCAT44(puVar3,puVar2);
    }
    bVar13 = 3 < param_3;
    param_3 = param_3 - 4;
    if (bVar13) {
      param_2 = (uint *)((int)param_2 - param_4);
      in_r12 = *param_2;
      puVar3 = param_1;
      if (param_4 == 2) {
        do {
          puVar2 = param_2;
          param_4 = in_r12 >> 0x10;
          param_2 = puVar2 + 1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          param_4 = param_4 | in_r12 << 0x10;
          param_1 = puVar3 + 1;
          *puVar3 = param_4;
          puVar3 = param_1;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + 6);
      }
      else if (param_4 < 3) {
        do {
          puVar2 = param_2;
          param_4 = in_r12 >> 8;
          param_2 = puVar2 + 1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          param_4 = param_4 | in_r12 << 0x18;
          param_1 = puVar3 + 1;
          *puVar3 = param_4;
          puVar3 = param_1;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + 5);
      }
      else {
        do {
          puVar2 = param_2;
          param_4 = in_r12 >> 0x18;
          param_2 = puVar2 + 1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          param_4 = param_4 | in_r12 << 8;
          param_1 = puVar3 + 1;
          *puVar3 = param_4;
          puVar3 = param_1;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + 7);
      }
    }
  }
  bVar8 = (byte)in_r12;
  bVar6 = (byte)param_4;
  bVar14 = (bool)((byte)(param_3 >> 1) & 1);
  param_3 = param_3 << 0x1f;
  bVar13 = (int)param_3 < 0;
  if (bVar14) {
    pbVar4 = (byte *)((int)param_2 + 1);
    bVar6 = (byte)*param_2;
    param_2 = (uint *)((int)param_2 + 2);
    bVar8 = *pbVar4;
  }
  puVar3 = param_2;
  if (bVar13) {
    puVar3 = (uint *)((int)param_2 + 1);
    param_3 = (uint)(byte)*param_2;
  }
  if (bVar14) {
    pbVar4 = (byte *)((int)param_1 + 1);
    *(byte *)param_1 = bVar6;
    param_1 = (uint *)((int)param_1 + 2);
    *pbVar4 = bVar8;
  }
  puVar2 = param_1;
  if (bVar13) {
    puVar2 = (uint *)((int)param_1 + 1);
    *(byte *)param_1 = (byte)param_3;
  }
  return CONCAT44(puVar3,puVar2);
}

// ==== FUN_0010ca8c @ 0010ca8c, 296 bytes
// called by: FUN_00119cb8, FUN_001257f8
undefined8 FUN_0010ca8c(uint *param_1,uint *param_2,uint param_3)

{
  uint *puVar1;
  uint *puVar2;
  byte *pbVar3;
  uint *puVar4;
  byte bVar5;
  undefined2 uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  byte bVar10;
  uint in_r12;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  
  uVar7 = (int)param_1 - (int)param_2;
  bVar13 = uVar7 == 0;
  if (param_2 <= param_1) {
    bVar13 = param_3 == uVar7;
  }
  if ((param_2 > param_1 || param_3 < uVar7) || bVar13) {
    if (3 < param_3) {
      uVar7 = (uint)param_1 & 3;
      in_r12 = uVar7;
      if (uVar7 != 0) {
        bVar5 = (byte)*param_2;
        puVar2 = (uint *)((int)param_2 + 1);
        if (uVar7 < 3) {
          puVar2 = (uint *)((int)param_2 + 2);
          in_r12 = (uint)*(byte *)((int)param_2 + 1);
        }
        *(byte *)param_1 = bVar5;
        param_2 = puVar2;
        if (uVar7 < 2) {
          param_2 = (uint *)((int)puVar2 + 1);
          bVar5 = (byte)*puVar2;
        }
        puVar2 = (uint *)((int)param_1 + 1);
        if (uVar7 < 3) {
          puVar2 = (uint *)((int)param_1 + 2);
          *(byte *)((int)param_1 + 1) = (byte)in_r12;
        }
        param_3 = (param_3 + uVar7) - 4;
        param_1 = puVar2;
        if (uVar7 < 2) {
          param_1 = (uint *)((int)puVar2 + 1);
          *(byte *)puVar2 = bVar5;
        }
      }
      uVar7 = (uint)param_2 & 3;
      if (uVar7 == 0) {
        uVar7 = 0;
        while (uVar8 = param_3 - 0x20, 0x1f < param_3) {
          uVar7 = param_2[1];
          uVar9 = param_2[2];
          uVar11 = param_2[3];
          *param_1 = *param_2;
          param_1[1] = uVar7;
          param_1[2] = uVar9;
          param_1[3] = uVar11;
          uVar7 = param_2[4];
          uVar9 = param_2[5];
          uVar11 = param_2[6];
          uVar12 = param_2[7];
          param_2 = param_2 + 8;
          param_1[4] = uVar7;
          param_1[5] = uVar9;
          param_1[6] = uVar11;
          param_1[7] = uVar12;
          param_1 = param_1 + 8;
          param_3 = uVar8;
        }
        if ((bool)((byte)(uVar8 >> 4) & 1)) {
          uVar7 = *param_2;
          uVar9 = param_2[1];
          uVar11 = param_2[2];
          uVar12 = param_2[3];
          param_2 = param_2 + 4;
          *param_1 = uVar7;
          param_1[1] = uVar9;
          param_1[2] = uVar11;
          param_1[3] = uVar12;
          param_1 = param_1 + 4;
        }
        if ((int)(param_3 << 0x1c) < 0) {
          uVar7 = *param_2;
          uVar9 = param_2[1];
          param_2 = param_2 + 2;
          *param_1 = uVar7;
          param_1[1] = uVar9;
          param_1 = param_1 + 2;
        }
        puVar1 = param_1;
        puVar2 = param_2;
        if ((bool)((byte)(uVar8 >> 2) & 1)) {
          puVar2 = param_2 + 1;
          uVar7 = *param_2;
          puVar1 = param_1 + 1;
          *param_1 = uVar7;
        }
        uVar6 = (undefined2)uVar7;
        if ((uVar8 & 3) != 0) {
          bVar14 = (bool)((byte)(uVar8 >> 1) & 1);
          param_3 = param_3 << 0x1f;
          bVar13 = (int)param_3 < 0;
          puVar4 = puVar2;
          if (bVar14) {
            puVar4 = (uint *)((int)puVar2 + 2);
            uVar6 = (undefined2)*puVar2;
          }
          puVar2 = puVar4;
          if (bVar13) {
            puVar2 = (uint *)((int)puVar4 + 1);
            param_3 = (uint)(byte)*puVar4;
          }
          puVar4 = puVar1;
          if (bVar14) {
            puVar4 = (uint *)((int)puVar1 + 2);
            *(undefined2 *)puVar1 = uVar6;
          }
          puVar1 = puVar4;
          if (bVar13) {
            puVar1 = (uint *)((int)puVar4 + 1);
            *(byte *)puVar4 = (byte)param_3;
          }
          return CONCAT44(puVar2,puVar1);
        }
        return CONCAT44(puVar2,puVar1);
      }
      bVar13 = 3 < param_3;
      param_3 = param_3 - 4;
      if (bVar13) {
        param_2 = (uint *)((int)param_2 - uVar7);
        in_r12 = *param_2;
        puVar2 = param_1;
        if (uVar7 == 2) {
          do {
            puVar1 = param_2;
            uVar7 = in_r12 >> 0x10;
            param_2 = puVar1 + 1;
            in_r12 = *param_2;
            bVar13 = 3 < param_3;
            param_3 = param_3 - 4;
            uVar7 = uVar7 | in_r12 << 0x10;
            param_1 = puVar2 + 1;
            *puVar2 = uVar7;
            puVar2 = param_1;
          } while (bVar13);
          param_2 = (uint *)((int)puVar1 + 6);
        }
        else if (uVar7 < 3) {
          do {
            puVar1 = param_2;
            uVar7 = in_r12 >> 8;
            param_2 = puVar1 + 1;
            in_r12 = *param_2;
            bVar13 = 3 < param_3;
            param_3 = param_3 - 4;
            uVar7 = uVar7 | in_r12 << 0x18;
            param_1 = puVar2 + 1;
            *puVar2 = uVar7;
            puVar2 = param_1;
          } while (bVar13);
          param_2 = (uint *)((int)puVar1 + 5);
        }
        else {
          do {
            puVar1 = param_2;
            uVar7 = in_r12 >> 0x18;
            param_2 = puVar1 + 1;
            in_r12 = *param_2;
            bVar13 = 3 < param_3;
            param_3 = param_3 - 4;
            uVar7 = uVar7 | in_r12 << 8;
            param_1 = puVar2 + 1;
            *puVar2 = uVar7;
            puVar2 = param_1;
          } while (bVar13);
          param_2 = (uint *)((int)puVar1 + 7);
        }
      }
    }
    bVar10 = (byte)in_r12;
    bVar5 = (byte)uVar7;
    bVar14 = (bool)((byte)(param_3 >> 1) & 1);
    param_3 = param_3 << 0x1f;
    bVar13 = (int)param_3 < 0;
    if (bVar14) {
      pbVar3 = (byte *)((int)param_2 + 1);
      bVar5 = (byte)*param_2;
      param_2 = (uint *)((int)param_2 + 2);
      bVar10 = *pbVar3;
    }
    puVar2 = param_2;
    if (bVar13) {
      puVar2 = (uint *)((int)param_2 + 1);
      param_3 = (uint)(byte)*param_2;
    }
    if (bVar14) {
      pbVar3 = (byte *)((int)param_1 + 1);
      *(byte *)param_1 = bVar5;
      param_1 = (uint *)((int)param_1 + 2);
      *pbVar3 = bVar10;
    }
    puVar1 = param_1;
    if (bVar13) {
      puVar1 = (uint *)((int)param_1 + 1);
      *(byte *)param_1 = (byte)param_3;
    }
    return CONCAT44(puVar2,puVar1);
  }
  param_1 = (uint *)((int)param_1 + param_3);
  param_2 = (uint *)((int)param_2 + param_3);
  if (3 < param_3) {
    bVar13 = ((uint)param_1 & 3) == 0;
    do {
      if (!bVar13) {
        param_2 = (uint *)((int)param_2 + -1);
        param_3 = param_3 - 1;
        param_1 = (uint *)((int)param_1 + -1);
        *(byte *)param_1 = *(byte *)param_2;
      }
      bVar13 = ((uint)param_1 & 3) == 0;
    } while (!bVar13);
    uVar7 = (uint)param_2 & 3;
    if (uVar7 == 0) {
      while (uVar7 = param_3 - 0x10, 0xf < param_3) {
        puVar2 = param_2 + -1;
        uVar11 = param_2[-2];
        uVar9 = param_2[-3];
        param_2 = param_2 + -4;
        uVar8 = *param_2;
        param_1[-1] = *puVar2;
        param_1[-2] = uVar11;
        param_1[-3] = uVar9;
        param_1 = param_1 + -4;
        *param_1 = uVar8;
        param_3 = uVar7;
      }
      uVar8 = param_3 << 0x1d;
      bVar13 = (int)uVar8 < 0;
      if ((bool)((byte)(uVar7 >> 3) & 1)) {
        puVar2 = param_2 + -1;
        param_2 = param_2 + -2;
        uVar8 = *param_2;
        param_1[-1] = *puVar2;
        param_1 = param_1 + -2;
        *param_1 = uVar8;
      }
      if (bVar13) {
        param_2 = param_2 + -1;
        uVar8 = *param_2;
        param_1 = param_1 + -1;
        *param_1 = uVar8;
      }
      uVar6 = (undefined2)uVar8;
      if ((uVar7 & 3) != 0) {
        bVar14 = (bool)((byte)(uVar7 >> 1) & 1);
        param_3 = param_3 << 0x1f;
        bVar13 = (int)param_3 < 0;
        if (bVar14) {
          param_2 = (uint *)((int)param_2 + -2);
          uVar6 = *(undefined2 *)param_2;
        }
        if (bVar13) {
          param_3 = (uint)*(byte *)((int)param_2 + -1);
        }
        if (bVar14) {
          param_1 = (uint *)((int)param_1 + -2);
          *(undefined2 *)param_1 = uVar6;
        }
        if (bVar13) {
          *(byte *)((int)param_1 + -1) = (byte)param_3;
        }
        return CONCAT44(param_2,param_1);
      }
      return CONCAT44(param_2,param_1);
    }
    bVar13 = 3 < param_3;
    param_3 = param_3 - 4;
    if (bVar13) {
      param_2 = (uint *)((int)param_2 - uVar7);
      in_r12 = *param_2;
      if (uVar7 < 2) {
        do {
          puVar2 = param_2;
          uVar7 = in_r12 << 0x18;
          param_2 = puVar2 + -1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          uVar7 = uVar7 | in_r12 >> 8;
          param_1 = param_1 + -1;
          *param_1 = uVar7;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + -3);
      }
      else if (uVar7 < 3) {
        do {
          puVar2 = param_2;
          uVar7 = in_r12 << 0x10;
          param_2 = puVar2 + -1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          uVar7 = uVar7 | in_r12 >> 0x10;
          param_1 = param_1 + -1;
          *param_1 = uVar7;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + -2);
      }
      else {
        do {
          puVar2 = param_2;
          uVar7 = in_r12 << 8;
          param_2 = puVar2 + -1;
          in_r12 = *param_2;
          bVar13 = 3 < param_3;
          param_3 = param_3 - 4;
          uVar7 = uVar7 | in_r12 >> 0x18;
          param_1 = param_1 + -1;
          *param_1 = uVar7;
        } while (bVar13);
        param_2 = (uint *)((int)puVar2 + -1);
      }
    }
  }
  bVar10 = (byte)in_r12;
  bVar5 = (byte)uVar7;
  bVar14 = (bool)((byte)(param_3 >> 1) & 1);
  param_3 = param_3 << 0x1f;
  bVar13 = (int)param_3 < 0;
  if (bVar14) {
    bVar5 = *(byte *)((int)param_2 + -1);
    param_2 = (uint *)((int)param_2 + -2);
    bVar10 = *(byte *)param_2;
  }
  if (bVar13) {
    param_3 = (uint)*(byte *)((int)param_2 + -1);
  }
  if (bVar14) {
    *(byte *)((int)param_1 + -1) = bVar5;
    param_1 = (uint *)((int)param_1 + -2);
    *(byte *)param_1 = bVar10;
  }
  if (bVar13) {
    *(byte *)((int)param_1 + -1) = (byte)param_3;
  }
  return CONCAT44(param_2,param_1);
}

// ==== FUN_0010cb60 @ 0010cb60, 100 bytes
// called by: config_load, config_save, usb_receive_data, usb_receive_cmd, FUN_00129f54
undefined8 FUN_0010cb60(undefined4 *param_1,byte *param_2,uint param_3,undefined4 param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte *pbVar4;
  byte *pbVar5;
  undefined2 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  bool bVar10;
  bool bVar11;
  
  while (uVar1 = param_3 - 0x20, 0x1f < param_3) {
    uVar7 = *(undefined4 *)(param_2 + 4);
    uVar8 = *(undefined4 *)(param_2 + 8);
    uVar9 = *(undefined4 *)(param_2 + 0xc);
    *param_1 = *(undefined4 *)param_2;
    param_1[1] = uVar7;
    param_1[2] = uVar8;
    param_1[3] = uVar9;
    param_4 = *(undefined4 *)(param_2 + 0x10);
    uVar7 = *(undefined4 *)(param_2 + 0x14);
    uVar8 = *(undefined4 *)(param_2 + 0x18);
    uVar9 = *(undefined4 *)(param_2 + 0x1c);
    param_2 = param_2 + 0x20;
    param_1[4] = param_4;
    param_1[5] = uVar7;
    param_1[6] = uVar8;
    param_1[7] = uVar9;
    param_1 = param_1 + 8;
    param_3 = uVar1;
  }
  if ((bool)((byte)(uVar1 >> 4) & 1)) {
    param_4 = *(undefined4 *)param_2;
    uVar7 = *(undefined4 *)(param_2 + 4);
    uVar8 = *(undefined4 *)(param_2 + 8);
    uVar9 = *(undefined4 *)(param_2 + 0xc);
    param_2 = param_2 + 0x10;
    *param_1 = param_4;
    param_1[1] = uVar7;
    param_1[2] = uVar8;
    param_1[3] = uVar9;
    param_1 = param_1 + 4;
  }
  if ((int)(param_3 << 0x1c) < 0) {
    param_4 = *(undefined4 *)param_2;
    uVar7 = *(undefined4 *)(param_2 + 4);
    param_2 = param_2 + 8;
    *param_1 = param_4;
    param_1[1] = uVar7;
    param_1 = param_1 + 2;
  }
  puVar3 = param_1;
  pbVar4 = param_2;
  if ((bool)((byte)(uVar1 >> 2) & 1)) {
    pbVar4 = param_2 + 4;
    param_4 = *(undefined4 *)param_2;
    puVar3 = param_1 + 1;
    *param_1 = param_4;
  }
  uVar6 = (undefined2)param_4;
  if ((uVar1 & 3) == 0) {
    return CONCAT44(pbVar4,puVar3);
  }
  bVar11 = (bool)((byte)(uVar1 >> 1) & 1);
  param_3 = param_3 << 0x1f;
  bVar10 = (int)param_3 < 0;
  pbVar5 = pbVar4;
  if (bVar11) {
    pbVar5 = pbVar4 + 2;
    uVar6 = *(undefined2 *)pbVar4;
  }
  pbVar4 = pbVar5;
  if (bVar10) {
    pbVar4 = pbVar5 + 1;
    param_3 = (uint)*pbVar5;
  }
  puVar2 = puVar3;
  if (bVar11) {
    puVar2 = (undefined4 *)((int)puVar3 + 2);
    *(undefined2 *)puVar3 = uVar6;
  }
  puVar3 = puVar2;
  if (bVar10) {
    puVar3 = (undefined4 *)((int)puVar2 + 1);
    *(char *)puVar2 = (char)param_3;
  }
  return CONCAT44(pbVar4,puVar3);
}

// ==== FUN_0010cbcc @ 0010cbcc, 20 bytes
undefined4 FUN_0010cbcc(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  param_4 = param_4 | param_4 << 8;
  FUN_0010cbf4(param_1,param_2,param_4 | param_4 << 0x10);
  return param_1;
}

// ==== FUN_0010cbe0 @ 0010cbe0, 16 bytes
// called by: FUN_00113b9c
undefined4 * FUN_0010cbe0(undefined4 *param_1,uint param_2,undefined1 param_3)

{
  uint uVar1;
  undefined2 uVar2;
  undefined4 *puVar3;
  undefined1 *puVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  int iVar7;
  bool bVar8;
  
  uVar2 = CONCAT11(param_3,param_3);
  uVar6 = CONCAT22(uVar2,uVar2);
  if (param_2 < 4) {
    if ((bool)((byte)(param_2 >> 1) & 1)) {
      puVar4 = (undefined1 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = param_3;
      param_1 = (undefined4 *)((int)param_1 + 2);
      *puVar4 = param_3;
    }
    puVar3 = param_1;
    if ((int)(param_2 << 0x1f) < 0) {
      puVar3 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = param_3;
    }
    return puVar3;
  }
  if (((uint)param_1 & 3) != 0) {
    iVar7 = 4 - ((uint)param_1 & 3);
    puVar3 = param_1;
    if (iVar7 != 2) {
      puVar3 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = param_3;
    }
    param_2 = param_2 - iVar7;
    param_1 = puVar3;
    if (1 < iVar7) {
      param_1 = (undefined4 *)((int)puVar3 + 2);
      *(undefined2 *)puVar3 = uVar2;
    }
  }
  bVar8 = 0x1f < param_2;
  param_2 = param_2 - 0x20;
  do {
    if (bVar8) {
      *param_1 = uVar6;
      param_1[1] = uVar6;
      param_1[2] = uVar6;
      param_1[3] = uVar6;
      param_1[4] = uVar6;
      param_1[5] = uVar6;
      param_1[6] = uVar6;
      param_1[7] = uVar6;
      param_1 = param_1 + 8;
      bVar8 = 0x1f < param_2;
      param_2 = param_2 - 0x20;
    }
  } while (bVar8);
  if ((bool)((byte)(param_2 >> 4) & 1)) {
    *param_1 = uVar6;
    param_1[1] = uVar6;
    param_1[2] = uVar6;
    param_1[3] = uVar6;
    param_1 = param_1 + 4;
  }
  if ((int)(param_2 << 0x1c) < 0) {
    *param_1 = uVar6;
    param_1[1] = uVar6;
    param_1 = param_1 + 2;
  }
  uVar1 = param_2 << 0x1e;
  puVar3 = param_1;
  if ((bool)((byte)((param_2 << 0x1c) >> 0x1e) & 1)) {
    puVar3 = param_1 + 1;
    *param_1 = uVar6;
  }
  if (uVar1 == 0) {
    return puVar3;
  }
  puVar5 = puVar3;
  if ((int)uVar1 < 0) {
    puVar5 = (undefined4 *)((int)puVar3 + 2);
    *(undefined2 *)puVar3 = uVar2;
  }
  puVar3 = puVar5;
  if ((uVar1 & 0x40000000) != 0) {
    puVar3 = (undefined4 *)((int)puVar5 + 1);
    *(undefined1 *)puVar5 = param_3;
  }
  return puVar3;
}

// ==== FUN_0010cbf0 @ 0010cbf0, 4 bytes
// called by: FUN_001085ac
undefined4 * FUN_0010cbf0(undefined4 *param_1,uint param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined1 *puVar3;
  undefined4 *puVar4;
  int iVar5;
  bool bVar6;
  
  if (param_2 < 4) {
    if ((bool)((byte)(param_2 >> 1) & 1)) {
      puVar3 = (undefined1 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = 0;
      param_1 = (undefined4 *)((int)param_1 + 2);
      *puVar3 = 0;
    }
    puVar2 = param_1;
    if ((int)(param_2 << 0x1f) < 0) {
      puVar2 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = 0;
    }
    return puVar2;
  }
  if (((uint)param_1 & 3) != 0) {
    iVar5 = 4 - ((uint)param_1 & 3);
    puVar2 = param_1;
    if (iVar5 != 2) {
      puVar2 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = 0;
    }
    param_2 = param_2 - iVar5;
    param_1 = puVar2;
    if (1 < iVar5) {
      param_1 = (undefined4 *)((int)puVar2 + 2);
      *(undefined2 *)puVar2 = 0;
    }
  }
  bVar6 = 0x1f < param_2;
  param_2 = param_2 - 0x20;
  do {
    if (bVar6) {
      *param_1 = 0;
      param_1[1] = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      param_1[4] = 0;
      param_1[5] = 0;
      param_1[6] = 0;
      param_1[7] = 0;
      param_1 = param_1 + 8;
      bVar6 = 0x1f < param_2;
      param_2 = param_2 - 0x20;
    }
  } while (bVar6);
  if ((bool)((byte)(param_2 >> 4) & 1)) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1 = param_1 + 4;
  }
  if ((int)(param_2 << 0x1c) < 0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1 = param_1 + 2;
  }
  uVar1 = param_2 << 0x1e;
  puVar2 = param_1;
  if ((bool)((byte)((param_2 << 0x1c) >> 0x1e) & 1)) {
    puVar2 = param_1 + 1;
    *param_1 = 0;
  }
  if (uVar1 == 0) {
    return puVar2;
  }
  puVar4 = puVar2;
  if ((int)uVar1 < 0) {
    puVar4 = (undefined4 *)((int)puVar2 + 2);
    *(undefined2 *)puVar2 = 0;
  }
  puVar2 = puVar4;
  if ((uVar1 & 0x40000000) != 0) {
    puVar2 = (undefined4 *)((int)puVar4 + 1);
    *(undefined1 *)puVar4 = 0;
  }
  return puVar2;
}

// ==== FUN_0010cbf4 @ 0010cbf4, 60 bytes
// called by: FUN_0010cbcc
undefined4 * FUN_0010cbf4(undefined4 *param_1,uint param_2,undefined4 param_3)

{
  uint uVar1;
  undefined1 uVar2;
  undefined4 *puVar3;
  undefined1 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  bool bVar7;
  
  uVar2 = (undefined1)param_3;
  if (param_2 < 4) {
    if ((bool)((byte)(param_2 >> 1) & 1)) {
      puVar4 = (undefined1 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = uVar2;
      param_1 = (undefined4 *)((int)param_1 + 2);
      *puVar4 = uVar2;
    }
    puVar3 = param_1;
    if ((int)(param_2 << 0x1f) < 0) {
      puVar3 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = uVar2;
    }
    return puVar3;
  }
  if (((uint)param_1 & 3) != 0) {
    iVar6 = 4 - ((uint)param_1 & 3);
    puVar3 = param_1;
    if (iVar6 != 2) {
      puVar3 = (undefined4 *)((int)param_1 + 1);
      *(undefined1 *)param_1 = uVar2;
    }
    param_2 = param_2 - iVar6;
    param_1 = puVar3;
    if (1 < iVar6) {
      param_1 = (undefined4 *)((int)puVar3 + 2);
      *(short *)puVar3 = (short)param_3;
    }
  }
  bVar7 = 0x1f < param_2;
  param_2 = param_2 - 0x20;
  do {
    if (bVar7) {
      *param_1 = param_3;
      param_1[1] = param_3;
      param_1[2] = param_3;
      param_1[3] = param_3;
      param_1[4] = param_3;
      param_1[5] = param_3;
      param_1[6] = param_3;
      param_1[7] = param_3;
      param_1 = param_1 + 8;
      bVar7 = 0x1f < param_2;
      param_2 = param_2 - 0x20;
    }
  } while (bVar7);
  if ((bool)((byte)(param_2 >> 4) & 1)) {
    *param_1 = param_3;
    param_1[1] = param_3;
    param_1[2] = param_3;
    param_1[3] = param_3;
    param_1 = param_1 + 4;
  }
  if ((int)(param_2 << 0x1c) < 0) {
    *param_1 = param_3;
    param_1[1] = param_3;
    param_1 = param_1 + 2;
  }
  uVar1 = param_2 << 0x1e;
  puVar3 = param_1;
  if ((bool)((byte)((param_2 << 0x1c) >> 0x1e) & 1)) {
    puVar3 = param_1 + 1;
    *param_1 = param_3;
  }
  if (uVar1 == 0) {
    return puVar3;
  }
  puVar5 = puVar3;
  if ((int)uVar1 < 0) {
    puVar5 = (undefined4 *)((int)puVar3 + 2);
    *(short *)puVar3 = (short)param_3;
  }
  puVar3 = puVar5;
  if ((uVar1 & 0x40000000) != 0) {
    puVar3 = (undefined4 *)((int)puVar5 + 1);
    *(undefined1 *)puVar5 = uVar2;
  }
  return puVar3;
}

// ==== FUN_0010cc30 @ 0010cc30, 84 bytes
// called by: FUN_001296f8, FUN_00106170, config_defaults, FUN_00123c60, usb_receive_data, FUN_001060fc
undefined4 * FUN_0010cc30(undefined4 *param_1,uint param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  bool bVar4;
  
  bVar4 = 0x1f < param_2;
  param_2 = param_2 - 0x20;
  do {
    if (bVar4) {
      *param_1 = 0;
      param_1[1] = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      param_1[4] = 0;
      param_1[5] = 0;
      param_1[6] = 0;
      param_1[7] = 0;
      param_1 = param_1 + 8;
      bVar4 = 0x1f < param_2;
      param_2 = param_2 - 0x20;
    }
  } while (bVar4);
  if ((bool)((byte)(param_2 >> 4) & 1)) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    param_1 = param_1 + 4;
  }
  if ((int)(param_2 << 0x1c) < 0) {
    *param_1 = 0;
    param_1[1] = 0;
    param_1 = param_1 + 2;
  }
  uVar1 = param_2 << 0x1e;
  puVar3 = param_1;
  if ((bool)((byte)((param_2 << 0x1c) >> 0x1e) & 1)) {
    puVar3 = param_1 + 1;
    *param_1 = 0;
  }
  if (uVar1 != 0) {
    puVar2 = puVar3;
    if ((int)uVar1 < 0) {
      puVar2 = (undefined4 *)((int)puVar3 + 2);
      *(undefined2 *)puVar3 = 0;
    }
    puVar3 = puVar2;
    if ((uVar1 & 0x40000000) != 0) {
      puVar3 = (undefined4 *)((int)puVar2 + 1);
      *(undefined1 *)puVar2 = 0;
    }
    return puVar3;
  }
  return puVar3;
}

// ==== FUN_0010cce4 @ 0010cce4, 124 bytes
uint FUN_0010cce4(uint *param_1,uint *param_2)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  
  do {
    puVar1 = param_1 + 1;
    uVar4 = *param_1;
    puVar3 = param_2 + 1;
    uVar5 = *param_2;
    uVar6 = uVar4 + 0xfefefeff & ~uVar4 & 0x80808080;
    if (uVar6 != 0 || uVar4 != uVar5) break;
    param_1 = param_1 + 2;
    uVar4 = *puVar1;
    param_2 = param_2 + 2;
    uVar5 = *puVar3;
    uVar6 = uVar4 + 0xfefefeff & ~uVar4 & 0x80808080;
  } while (uVar6 == 0 && uVar4 == uVar5);
  uVar2 = uVar5 * 0x1000000 + uVar4 * -0x1000000;
  bVar7 = uVar2 == 0;
  bVar10 = !bVar7 && uVar4 * 0x1000000 <= uVar5 * 0x1000000;
  bVar8 = (uVar6 & 0xff0) == 0;
  bVar9 = bVar7 && bVar8;
  if (bVar7 && bVar8) {
    uVar2 = uVar5 * 0x10000 + uVar4 * -0x10000;
    bVar7 = uVar2 == 0;
    bVar10 = !bVar7 && uVar4 * 0x10000 <= uVar5 * 0x10000;
    bVar8 = (uVar6 & 0xff00) == 0;
    bVar9 = bVar7 && bVar8;
    if (bVar7 && bVar8) {
      uVar2 = uVar5 * 0x100 + uVar4 * -0x100;
      bVar7 = uVar2 == 0;
      bVar10 = !bVar7 && uVar4 * 0x100 <= uVar5 * 0x100;
      bVar8 = (uVar6 & 0xff0000) == 0;
      bVar9 = bVar7 && bVar8;
      if (bVar7 && bVar8) {
        bVar10 = uVar4 <= uVar5;
        uVar2 = uVar5 - uVar4;
        bVar9 = uVar2 == 0;
      }
    }
  }
  if (!bVar9) {
    uVar2 = (uint)bVar10 << 0x1f | uVar2 >> 1;
  }
  return uVar2;
}

// ==== divide @ 0010cd94, 552 bytes
// called by: FUN_001287dc, FUN_0012a848, FUN_00107b98, FUN_0010fff0, FUN_00101524, idle_screen_proc, FUN_00119fd4, FUN_0012b2c8, FUN_00104c60, FUN_00127fd0
// called as divide(value, 1000) / (value, 100); result in r0 ?
/* called as divide(value, 1000) / (value, 100); result in r0 ? */

longlong divide(uint param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  
  if ((int)(param_1 | param_2) < 0) {
    uVar5 = param_2 & 0x80000000;
    if ((int)uVar5 < 0) {
      param_2 = -param_2;
    }
    uVar4 = uVar5 ^ (int)param_1 >> 0x20;
    if (SUB41(param_1 >> 0x1f,0)) {
      param_1 = -param_1;
    }
    iVar3 = (int)uVar4 >> 0x1f;
    uVar2 = param_2;
    if (param_1 >> 4 < param_2) goto LAB_0010cf60;
    if (param_1 >> 8 < param_2) goto LAB_0010cf30;
    uVar2 = param_2 << 6;
    uVar5 = 0xfc000000;
    if (param_1 >> 8 < uVar2) goto LAB_0010cf30;
    uVar2 = param_2 << 0xc;
    uVar5 = 0xfff00000;
    if (param_1 >> 8 < uVar2) goto LAB_0010cf30;
    uVar2 = param_2 << 0x12;
    uVar5 = 0xffffc000;
    if (uVar2 <= param_1 >> 8) {
      uVar2 = param_2 << 0x18;
      uVar5 = 0xffffff00;
    }
    bVar7 = uVar2 == 0;
    if (!bVar7) {
      do {
        if (bVar7) {
          uVar2 = uVar2 >> 6;
        }
LAB_0010cf30:
        bVar7 = uVar2 <= param_1 >> 7;
        if (bVar7) {
          param_1 = param_1 + uVar2 * -0x80;
        }
        bVar8 = uVar2 <= param_1 >> 6;
        if (bVar8) {
          param_1 = param_1 + uVar2 * -0x40;
        }
        bVar9 = uVar2 <= param_1 >> 5;
        if (bVar9) {
          param_1 = param_1 + uVar2 * -0x20;
        }
        bVar10 = uVar2 <= param_1 >> 4;
        if (bVar10) {
          param_1 = param_1 + uVar2 * -0x10;
        }
        uVar5 = (((uVar5 * 2 + (uint)bVar7) * 2 + (uint)bVar8) * 2 + (uint)bVar9) * 2 + (uint)bVar10
        ;
LAB_0010cf60:
        bVar7 = uVar2 <= param_1 >> 3;
        if (bVar7) {
          param_1 = param_1 + uVar2 * -8;
        }
        uVar6 = uVar5 * 2 + (uint)bVar7;
        bVar8 = uVar2 <= param_1 >> 2;
        if (bVar8) {
          param_1 = param_1 + uVar2 * -4;
        }
        uVar5 = uVar6 * 2 + (uint)bVar8;
        bVar7 = true;
      } while (CARRY4(uVar6,uVar6) || CARRY4(uVar6 * 2,(uint)bVar8));
      bVar7 = uVar2 <= param_1 >> 1;
      if (bVar7) {
        param_1 = param_1 + uVar2 * -2;
      }
      uVar6 = param_1 - uVar2;
      if (uVar2 > param_1) {
        uVar6 = param_1;
      }
      iVar1 = (uVar5 * 2 + (uint)bVar7) * 2 + (uint)(uVar2 <= param_1);
      if (iVar3 < 0) {
        iVar1 = -iVar1;
      }
      if ((bool)((byte)(uVar4 >> 0x1e) & 1)) {
        uVar6 = -uVar6;
      }
      return CONCAT44(uVar6,iVar1);
    }
    if (iVar3 < 0) {
      param_1 = -param_1;
    }
LAB_0010cfac:
    return (ulonglong)param_1 << 0x20;
  }
  if (param_1 >> 1 < param_2) {
    if (param_1 < param_2) {
      return (ulonglong)param_1 << 0x20;
    }
    return CONCAT44(param_1 - param_2,1);
  }
  iVar3 = 0;
  uVar5 = param_2;
  if (param_2 <= param_1 >> 4) {
    if (param_2 <= param_1 >> 8) {
      if (param_1 >> 0xc < param_2) goto LAB_0010ce20;
      if (param_1 >> 0x10 < param_2) goto LAB_0010cdf0;
      uVar5 = param_2 << 8;
      iVar3 = -0x1000000;
      if (uVar5 <= param_1 >> 0x10) {
        iVar3 = -0x10000;
        uVar5 = param_2 << 0x10;
      }
      if (param_1 >> 0xc < uVar5) goto LAB_0010ce20;
      bVar7 = false;
      if (uVar5 == 0) goto LAB_0010cfac;
      do {
        if (bVar7) {
          uVar5 = uVar5 >> 8;
        }
LAB_0010cdf0:
        bVar7 = uVar5 <= param_1 >> 0xf;
        if (bVar7) {
          param_1 = param_1 + uVar5 * -0x8000;
        }
        bVar8 = uVar5 <= param_1 >> 0xe;
        if (bVar8) {
          param_1 = param_1 + uVar5 * -0x4000;
        }
        bVar9 = uVar5 <= param_1 >> 0xd;
        if (bVar9) {
          param_1 = param_1 + uVar5 * -0x2000;
        }
        bVar10 = uVar5 <= param_1 >> 0xc;
        if (bVar10) {
          param_1 = param_1 + uVar5 * -0x1000;
        }
        iVar3 = (((iVar3 * 2 + (uint)bVar7) * 2 + (uint)bVar8) * 2 + (uint)bVar9) * 2 + (uint)bVar10
        ;
LAB_0010ce20:
        bVar7 = uVar5 <= param_1 >> 0xb;
        if (bVar7) {
          param_1 = param_1 + uVar5 * -0x800;
        }
        bVar8 = uVar5 <= param_1 >> 10;
        if (bVar8) {
          param_1 = param_1 + uVar5 * -0x400;
        }
        bVar9 = uVar5 <= param_1 >> 9;
        if (bVar9) {
          param_1 = param_1 + uVar5 * -0x200;
        }
        uVar4 = ((iVar3 * 2 + (uint)bVar7) * 2 + (uint)bVar8) * 2 + (uint)bVar9;
        bVar8 = uVar5 <= param_1 >> 8;
        if (bVar8) {
          param_1 = param_1 + uVar5 * -0x100;
        }
        iVar3 = uVar4 * 2 + (uint)bVar8;
        bVar7 = true;
      } while (CARRY4(uVar4,uVar4) || CARRY4(uVar4 * 2,(uint)bVar8));
    }
    bVar7 = uVar5 <= param_1 >> 7;
    if (bVar7) {
      param_1 = param_1 + uVar5 * -0x80;
    }
    bVar8 = uVar5 <= param_1 >> 6;
    if (bVar8) {
      param_1 = param_1 + uVar5 * -0x40;
    }
    bVar9 = uVar5 <= param_1 >> 5;
    if (bVar9) {
      param_1 = param_1 + uVar5 * -0x20;
    }
    bVar10 = uVar5 <= param_1 >> 4;
    if (bVar10) {
      param_1 = param_1 + uVar5 * -0x10;
    }
    iVar3 = (((iVar3 * 2 + (uint)bVar7) * 2 + (uint)bVar8) * 2 + (uint)bVar9) * 2 + (uint)bVar10;
  }
  bVar7 = uVar5 <= param_1 >> 3;
  if (bVar7) {
    param_1 = param_1 + uVar5 * -8;
  }
  bVar8 = uVar5 <= param_1 >> 2;
  if (bVar8) {
    param_1 = param_1 + uVar5 * -4;
  }
  bVar9 = uVar5 <= param_1 >> 1;
  if (bVar9) {
    param_1 = param_1 + uVar5 * -2;
  }
  uVar4 = param_1 - uVar5;
  if (uVar5 > param_1) {
    uVar4 = param_1;
  }
  return CONCAT44(uVar4,(((iVar3 * 2 + (uint)bVar7) * 2 + (uint)bVar8) * 2 + (uint)bVar9) * 2 +
                        (uint)(uVar5 <= param_1));
}

// ==== FUN_0010cfbc @ 0010cfbc, 8 bytes
// called by: mem_free, mem_alloc, FUN_0010cffa
undefined * FUN_0010cfbc(void)

{
  return &DAT_007c61c0;
}

// ==== FUN_0010cfd0 @ 0010cfd0, 32 bytes
// called by: mem_alloc
bool FUN_0010cfd0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 local_10;
  
  local_10 = param_4;
  iVar1 = FUN_0010d260(0,&local_10,param_2);
  if (iVar1 != 0) {
    FUN_0010d208(param_1,local_10,iVar1);
  }
  return iVar1 != 0;
}

// ==== FUN_0010cff0 @ 0010cff0, 10 bytes
void FUN_0010cff0(void)

{
  FUN_0010d23c(1);
  return;
}

// ==== FUN_0010cffa @ 0010cffa, 90 bytes
// called by: FUN_001001bc
void FUN_0010cffa(void)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  uint in_r3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined8 uVar7;
  uint local_18;
  
  local_18 = in_r3;
  uVar7 = FUN_0010d248();
  uVar3 = (uint)((ulonglong)uVar7 >> 0x20);
  if (uVar3 < (uint)uVar7 + 0x10) {
    iVar1 = FUN_0010d260(0,&local_18,0x10);
    if (iVar1 == 0) {
      FUN_0010d23c();
    }
    uVar4 = (uint)uVar7;
    if (local_18 != uVar3) {
      uVar4 = local_18;
    }
    uVar7 = CONCAT44(local_18 + iVar1,uVar4);
  }
  iVar1 = (int)((ulonglong)uVar7 >> 0x20);
  iVar5 = (int)uVar7;
  piVar2 = (int *)FUN_0010cfbc();
  *piVar2 = iVar5;
  iVar6 = iVar5 + 0x10;
  FUN_0010d054(iVar5);
  if (iVar1 != iVar6) {
    FUN_0010d208(*piVar2,iVar6,iVar1 - iVar6);
  }
  return;
}

// ==== FUN_0010d054 @ 0010d054, 10 bytes
// called by: FUN_0010cffa
void FUN_0010d054(undefined4 *param_1)

{
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = param_1;
  return;
}

// ==== FUN_0010d062 @ 0010d062, 176 bytes
undefined4 FUN_0010d062(uint *param_1,int param_2,int param_3,int param_4)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  if ((int)(*param_1 << 0x1a) < 0) {
    uVar1 = param_1[7];
    *param_1 = *param_1 & 0xffffffef;
  }
  else {
    uVar1 = 1;
  }
  if (param_2 < (int)uVar1) {
    iVar4 = uVar1 - param_2;
  }
  else {
    iVar4 = 0;
  }
  param_1[6] = param_1[6] - (iVar4 + param_2 + param_4);
  for (iVar3 = 0; iVar3 < param_4; iVar3 = iVar3 + 1) {
    (*(code *)param_1[1])(*(undefined1 *)(param_3 + iVar3),param_1[2]);
    param_1[8] = param_1[8] + 1;
  }
  while (0 < iVar4) {
    (*(code *)param_1[1])(0x30,param_1[2]);
    param_1[8] = param_1[8] + 1;
    iVar4 = iVar4 + -1;
  }
  while (0 < param_2) {
    (*(code *)param_1[1])(*(byte *)((int)param_1 + param_2 + 0x23),param_1[2]);
    param_1[8] = param_1[8] + 1;
    param_2 = param_2 + -1;
  }
  if ((int)((uint)(byte)*param_1 << 0x18) < 0) {
    uVar2 = 2;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

// ==== FUN_0010d11e @ 0010d11e, 32 bytes
// called by: sprintf
void FUN_0010d11e(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 auStack_40 [4];
  undefined4 local_3c;
  undefined4 local_38;
  undefined1 *local_34;
  undefined4 local_30;
  undefined4 local_2c;
  
  local_2c = 0;
  local_34 = &LAB_0010d114_1;
  local_3c = param_4;
  local_38 = param_2;
  local_30 = param_1;
  FUN_0010c768(auStack_40,param_3);
  return;
}

// ==== FUN_0010d144 @ 0010d144, 10 bytes
// called by: sprintf
void FUN_0010d144(undefined1 param_1,undefined4 *param_2)

{
  undefined1 *puVar1;
  
  puVar1 = (undefined1 *)*param_2;
  *puVar1 = param_1;
  *param_2 = puVar1 + 1;
  return;
}

// ==== FUN_0010d1d0 @ 0010d1d0, 44 bytes
undefined8 FUN_0010d1d0(uint param_1)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = param_1 - (param_1 >> 2);
  uVar1 = uVar1 + (uVar1 >> 4);
  uVar1 = uVar1 + (uVar1 >> 8);
  uVar1 = uVar1 + (uVar1 >> 0x10) >> 3;
  iVar2 = (param_1 - 10) + uVar1 * -10;
  if (iVar2 < 0) {
    iVar2 = iVar2 + 10;
  }
  else {
    uVar1 = uVar1 + 1;
  }
  return CONCAT44(iVar2,uVar1);
}

// ==== FUN_0010d1fc @ 0010d1fc, 8 bytes
// called by: FUN_0010d298
undefined * FUN_0010d1fc(void)

{
  return &DAT_007c61b8;
}

// ==== FUN_0010d208 @ 0010d208, 52 bytes
// called by: FUN_0010cfd0, FUN_0010cffa
void FUN_0010d208(int *param_1,int *param_2,int param_3)

{
  int *piVar1;
  int *piVar2;
  
  piVar2 = (int *)param_1[1];
  while ((piVar1 = piVar2, piVar1 != (int *)0x0 && (piVar1 < param_2))) {
    param_1 = piVar1;
    piVar2 = (int *)piVar1[1];
  }
  piVar2 = param_2;
  if ((int *)(*param_1 + (int)param_1) != param_2) {
    piVar2 = (int *)(((int)param_2 + 3U & 0xfffffff8) + 4);
    param_3 = param_3 - ((int)piVar2 - (int)param_2);
  }
  *piVar2 = param_3;
  mem_free(piVar2 + 1);
  return;
}

// ==== FUN_0010d23c @ 0010d23c, 12 bytes
// called by: FUN_0010cff0, FUN_0010cffa
void FUN_0010d23c(void)

{
  FUN_0010d314();
  FUN_0010d308();
  return;
}

// ==== FUN_0010d248 @ 0010d248, 24 bytes
// called by: FUN_0010cffa
void FUN_0010d248(int param_1)

{
  DAT_0018312c = (undefined1 *)(param_1 + 0x1000U);
  if ((undefined1 *)(param_1 + 0x1000U) <= &stack0x00000000) {
    return;
  }
  DAT_0018312c = &stack0x00000000;
  return;
}

// ==== FUN_0010d260 @ 0010d260, 52 bytes
// called by: FUN_0010cfd0, FUN_0010cffa
int FUN_0010d260(int param_1,uint *param_2,int param_3)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 auStack_1010 [4096];
  
  puVar2 = (undefined1 *)(DAT_0018312c + param_3);
  if (puVar2 <= auStack_1010) {
    puVar1 = puVar2 + 0x1000;
    if (puVar2 + ((int)auStack_1010 - (int)puVar2) / 2 < puVar2 + 0x1000) {
      puVar1 = puVar2 + ((int)auStack_1010 - (int)puVar2) / 2;
    }
    *param_2 = DAT_0018312c;
    param_1 = ((uint)(puVar1 + 7) & 0xfffffff8) - DAT_0018312c;
    DAT_0018312c = (uint)(puVar1 + 7) & 0xfffffff8;
  }
  return param_1;
}

// ==== FUN_0010d298 @ 0010d298, 96 bytes
// called by: FUN_001001cc
void FUN_0010d298(void)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 in_lr;
  uint *puVar3;
  
  uVar1 = FUN_0010d1fc();
  *(undefined4 *)((uVar1 & 0xfffffff8) + 0x5c) = in_lr;
  puVar3 = (uint *)((uVar1 & 0xfffffff8) + 0x58);
  *puVar3 = uVar1;
  FUN_00126c50();
  puVar2 = (undefined4 *)*puVar3;
  *puVar2 = 0;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  puVar2[4] = 0;
  puVar2[5] = 0;
  puVar2[6] = 0;
  puVar2[7] = 0;
  puVar2[8] = 0;
  puVar2[9] = 0;
  puVar2[10] = 0;
  puVar2[0xb] = 0;
  puVar2[0xc] = 0;
  puVar2[0xd] = 0;
  puVar2[0xe] = 0;
  puVar2[0xf] = 0;
  return;
}

// ==== FUN_0010d2f8 @ 0010d2f8, 16 bytes
void FUN_0010d2f8(undefined4 param_1)

{
  FUN_00100200(param_1);
  return;
}

// ==== FUN_0010d308 @ 0010d308, 12 bytes
// called by: FUN_0010d23c
void FUN_0010d308(int param_1)

{
  if (param_1 != 0) {
    FUN_00126f6c();
  }
  return;
}

// ==== FUN_0010d314 @ 0010d314, 22 bytes
// called by: FUN_0010d23c
undefined4 FUN_0010d314(int param_1)

{
  char *pcVar1;
  
  if (param_1 == 1) {
    pcVar1 = ": Heap memory corrupted";
  }
  else {
    pcVar1 = (char *)0x0;
  }
  FUN_0010d364("SIGRTMEM: Out of heap memory",pcVar1);
  return 1;
}

// ==== FUN_0010d364 @ 0010d364, 52 bytes
// called by: FUN_0010d314
void FUN_0010d364(char *param_1,char *param_2)

{
  char cVar1;
  
  cVar1 = '\n';
  for (; (FUN_00126f70(cVar1), param_1 != (char *)0x0 && (cVar1 = *param_1, cVar1 != '\0'));
      param_1 = param_1 + 1) {
  }
  for (; (param_2 != (char *)0x0 && (*param_2 != '\0')); param_2 = param_2 + 1) {
    FUN_00126f70();
  }
  FUN_00126f70(10);
  return;
}

// ==== FUN_0010d398 @ 0010d398, 48 bytes
// called by: FUN_0012a848, FUN_00107b98, FUN_00102558, FUN_00126f74, FUN_00103448, FUN_00101ea8, FUN_00102dc8, FUN_00111618, FUN_00101524, FUN_00100e10, FUN_00101fec, FUN_0010b830, FUN_00108564, FUN_0010c414, FUN_0010584c, FUN_00100eb0, FUN_001052cc, FUN_00104770, usb_receive_data, FUN_00103924, FUN_00102150, sys_set_clock_divider, FUN_0010b430, FUN_0012c6d0, FUN_00121a30, FUN_00102f50, FUN_00102104, FUN_00105eb8, FUN_0012acf8
longlong FUN_0010d398(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  
  iVar3 = 0;
  uVar1 = param_2;
  if (param_2 <= param_1 >> 2) {
    if (param_2 <= param_1 >> 5) {
      if (param_2 <= param_1 >> 8) {
        if (param_1 >> 0xc < param_2) goto LAB_0010ce20;
        if (param_1 >> 0x10 < param_2) goto LAB_0010cdf0;
        uVar1 = param_2 << 8;
        iVar3 = -0x1000000;
        if (uVar1 <= param_1 >> 0x10) {
          iVar3 = -0x10000;
          uVar1 = param_2 << 0x10;
        }
        if (param_1 >> 0xc < uVar1) goto LAB_0010ce20;
        bVar4 = false;
        if (uVar1 == 0) {
          return (ulonglong)param_1 << 0x20;
        }
        do {
          if (bVar4) {
            uVar1 = uVar1 >> 8;
          }
LAB_0010cdf0:
          bVar4 = uVar1 <= param_1 >> 0xf;
          if (bVar4) {
            param_1 = param_1 + uVar1 * -0x8000;
          }
          bVar5 = uVar1 <= param_1 >> 0xe;
          if (bVar5) {
            param_1 = param_1 + uVar1 * -0x4000;
          }
          bVar6 = uVar1 <= param_1 >> 0xd;
          if (bVar6) {
            param_1 = param_1 + uVar1 * -0x2000;
          }
          bVar7 = uVar1 <= param_1 >> 0xc;
          if (bVar7) {
            param_1 = param_1 + uVar1 * -0x1000;
          }
          iVar3 = (((iVar3 * 2 + (uint)bVar4) * 2 + (uint)bVar5) * 2 + (uint)bVar6) * 2 +
                  (uint)bVar7;
LAB_0010ce20:
          bVar4 = uVar1 <= param_1 >> 0xb;
          if (bVar4) {
            param_1 = param_1 + uVar1 * -0x800;
          }
          bVar5 = uVar1 <= param_1 >> 10;
          if (bVar5) {
            param_1 = param_1 + uVar1 * -0x400;
          }
          bVar6 = uVar1 <= param_1 >> 9;
          if (bVar6) {
            param_1 = param_1 + uVar1 * -0x200;
          }
          uVar2 = ((iVar3 * 2 + (uint)bVar4) * 2 + (uint)bVar5) * 2 + (uint)bVar6;
          bVar5 = uVar1 <= param_1 >> 8;
          if (bVar5) {
            param_1 = param_1 + uVar1 * -0x100;
          }
          iVar3 = uVar2 * 2 + (uint)bVar5;
          bVar4 = true;
        } while (CARRY4(uVar2,uVar2) || CARRY4(uVar2 * 2,(uint)bVar5));
      }
      bVar4 = uVar1 <= param_1 >> 7;
      if (bVar4) {
        param_1 = param_1 + uVar1 * -0x80;
      }
      bVar5 = uVar1 <= param_1 >> 6;
      if (bVar5) {
        param_1 = param_1 + uVar1 * -0x40;
      }
      bVar6 = uVar1 <= param_1 >> 5;
      if (bVar6) {
        param_1 = param_1 + uVar1 * -0x20;
      }
      iVar3 = ((iVar3 * 2 + (uint)bVar4) * 2 + (uint)bVar5) * 2 + (uint)bVar6;
    }
    bVar4 = uVar1 <= param_1 >> 4;
    if (bVar4) {
      param_1 = param_1 + uVar1 * -0x10;
    }
    bVar5 = uVar1 <= param_1 >> 3;
    if (bVar5) {
      param_1 = param_1 + uVar1 * -8;
    }
    bVar6 = uVar1 <= param_1 >> 2;
    if (bVar6) {
      param_1 = param_1 + uVar1 * -4;
    }
    iVar3 = ((iVar3 * 2 + (uint)bVar4) * 2 + (uint)bVar5) * 2 + (uint)bVar6;
  }
  bVar4 = uVar1 <= param_1 >> 1;
  if (bVar4) {
    param_1 = param_1 + uVar1 * -2;
  }
  uVar2 = param_1 - uVar1;
  if (uVar1 > param_1) {
    uVar2 = param_1;
  }
  return CONCAT44(uVar2,(iVar3 * 2 + (uint)bVar4) * 2 + (uint)(uVar1 <= param_1));
}

// ==== FUN_0010d3c8 @ 0010d3c8, 12 bytes
// called by: FUN_00100b28, FUN_00100ad0
void FUN_0010d3c8(void)

{
  bool in_ZR;
  undefined4 in_cr7;
  undefined4 in_cr14;
  
  do {
    coprocessor_movefromRt(0xf,0,3,in_cr7,in_cr14);
  } while (!in_ZR);
  return;
}

// ==== FUN_0010d3d4 @ 0010d3d4, 36 bytes
// called by: FUN_001096e0
void FUN_0010d3d4(undefined4 param_1)

{
  uint uVar1;
  
  coproc_moveto_Translation_table_base_0(param_1);
  coproc_moveto_Domain_Access_Control(0x40000000);
  uVar1 = coproc_movefrom_Control();
  coproc_moveto_Control(uVar1 | 0x1005);
  return;
}

// ==== FUN_0010d3f8 @ 0010d3f8, 196 bytes
// called by: FUN_0012a298, lcd_boot_proc, idle_screen_proc, usb_command, screen_create_proc
undefined8 FUN_0010d3f8(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 uStack_c;
  
  local_18 = param_1;
  local_14 = param_2;
  local_10 = param_3;
  uStack_c = param_4;
  if (((param_1 & 0x80) == 0) && ((param_1 & 0x7f) != 0)) {
    FUN_0010799c();
    local_18 = CONCAT31(local_18._1_3_,1);
    local_14 = 0x42c80000;
    local_10._0_2_ = (ushort)(byte)param_1;
    FUN_00107b98(1,&local_18);
    FUN_001082e8(1);
    FUN_00107a98(1);
    return CONCAT44(local_14,local_18);
  }
  FUN_00107a98(1,0);
  FUN_001075ac(1,0);
  FUN_001079cc();
  uVar1 = DAT_b000008c;
  DAT_b000008c = uVar1 & 0xfffffff3;
  uVar1 = DAT_b8001034;
  DAT_b8001034 = uVar1 | 2;
  uVar1 = DAT_b8001030;
  DAT_b8001030 = uVar1 & 0xffff | 2;
  uVar1 = DAT_b8001038;
  DAT_b8001038 = uVar1 & 0xfffd;
  return CONCAT44(local_14,local_18);
}

// ==== FUN_0010d4c8 @ 0010d4c8, 116 bytes
char * FUN_0010d4c8(undefined4 param_1,undefined4 param_2,undefined *param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  undefined *puVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined8 uVar7;
  char *pcStack_28;
  int iStack_24;
  undefined *puStack_20;
  undefined4 uStack_1c;
  char *pcVar8;
  
  DAT_001823e8._0_1_ = '\0';
  puVar4 = param_3;
  uVar5 = param_4;
  uVar7 = FUN_00106e94();
  iStack_24 = (int)((ulonglong)uVar7 >> 0x20);
  DAT_001823ec = (int)uVar7;
  if (-1 < DAT_001823ec) {
    FUN_001069e8(DAT_001823ec,param_2,param_3,param_4);
    FUN_00106c98(DAT_001823ec,2,FUN_0010d75c,0);
    pcVar8 = (char *)FUN_00106ef0(DAT_001823ec);
    do {
    } while ((char)DAT_001823e8 == '\0');
    return pcVar8;
  }
  pcVar8 = "Request VDMA fail.\n";
  pcStack_28 = "Request VDMA fail.\n";
  puStack_20 = puVar4;
  uStack_1c = uVar5;
  if (DAT_00182da4 == -1) {
    iVar3 = FUN_00103d00();
    uVar7 = CONCAT44(iStack_24,DAT_001823ec);
    if ((iVar3 == 0x47) &&
       (uVar2 = DAT_b0000004, uVar7 = CONCAT44(iStack_24,DAT_001823ec), (uVar2 & 4) == 4))
    goto LAB_00103d5c;
  }
  else if (DAT_00182da4 == 0) goto LAB_00103d5c;
  iStack_24 = (int)((ulonglong)uVar7 >> 0x20);
  DAT_001823ec = (int)uVar7;
  DAT_00182d89 = 0;
  if (DAT_00182d88 == '\0') {
    FUN_00103060(1);
    pcStack_28 = (char *)0x1;
    iStack_24 = FUN_001014c8();
    iStack_24 = iStack_24 * 1000;
    puStack_20 = &DAT_0001c200;
    uStack_1c = 3;
    FUN_00103514(&pcStack_28);
    uVar7 = CONCAT44(iStack_24,DAT_001823ec);
  }
  puVar6 = (undefined4 *)&stack0xfffffff4;
  while( true ) {
    iStack_24 = (int)((ulonglong)uVar7 >> 0x20);
    DAT_001823ec = (int)uVar7;
    if (*pcVar8 == '\0') break;
    if (*pcVar8 == '%') {
      pcVar8 = (char *)FUN_00103ae4(pcVar8 + 1,*puVar6);
      uVar7 = CONCAT44(iStack_24,DAT_001823ec);
      puVar6 = puVar6 + 1;
    }
    else {
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
      FUN_001036e8(cVar1);
      uVar7 = CONCAT44(iStack_24,DAT_001823ec);
    }
  }
LAB_00103d5c:
  DAT_001823ec = (int)uVar7;
  return pcStack_28;
}

// ==== FUN_0010d558 @ 0010d558, 180 bytes
void FUN_0010d558(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint *puVar2;
  
  puVar2 = (uint *)FUN_0012ab3c(&DAT_00489a98,0x1b8,param_3,param_4,param_3,param_4);
  if (puVar2 != (uint *)0x0) {
    DAT_b100805c = 1;
    DAT_b1008700 = 0x805381c0;
    DAT_b1008060 = 0x1b8;
    uVar1 = DAT_b100805c;
    DAT_b100805c = uVar1 | 0x20;
    do {
      uVar1 = 0;
      if ((char)DAT_0066aa80 != '\0') {
        uVar1 = DAT_b100805c;
      }
    } while ((char)DAT_0066aa80 != '\0' && (uVar1 & 0x20) != 0);
    FUN_0010c9b8(puVar2,0x805381c0,0x1b8);
    if ((~*puVar2 & 0xff) != 0) {
      return;
    }
    FUN_0012bb08(&DAT_00182b24,&DAT_00008035,*puVar2 & 0xff,puVar2 + 1,0x1b4,0xffffffff);
    FUN_0012a964();
  }
  DAT_00182950 = DAT_00182950 | 1;
  return;
}

// ==== FUN_0010d63c @ 0010d63c, 236 bytes
/* WARNING: Control flow encountered bad instruction data */

void FUN_0010d63c(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  
  DAT_b100805c = 3;
  DAT_b1008700 = 0x80538380;
  DAT_b1008060 = 0x400;
  uVar1 = DAT_b100805c;
  DAT_b100805c = uVar1 | 0x20;
  do {
    uVar1 = 0;
    if ((char)DAT_0066aa80 != '\0') {
      uVar1 = DAT_b100805c;
    }
  } while ((char)DAT_0066aa80 != '\0' && (uVar1 & 0x20) != 0);
  iVar2 = FUN_00129f54(&DAT_00489ac0,0x80538380,0x400,0x80538380,param_3,param_4);
  if (iVar2 != 0) {
    if (DAT_00489acc == 0xff) {
      FUN_0012bb08(&DAT_00182b24,&DAT_00008035,0xff,&DAT_00489ad0,DAT_00489ac4,0xffffffff);
      FUN_0012a964();
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    }
    if (DAT_00489acc == 8) {
      iVar2 = FUN_0012ab3c(&DAT_00489a70,DAT_00489ac4 * 0x3fc);
      if (iVar2 != 0) {
        FUN_0010c9b8(iVar2,&DAT_00489ad0);
      }
      DAT_00182bc0 = g_config.unknown_108;
    }
  }
  return;
}

// ==== FUN_0010d75c @ 0010d75c, 28 bytes
void FUN_0010d75c(void)

{
  FUN_00106de4(DAT_001823ec);
  DAT_001823e8._0_1_ = 1;
  return;
}

// ==== FUN_0010d884 @ 0010d884, 92 bytes
// called by: FUN_001195dc
void FUN_0010d884(void)

{
  int iVar1;
  
  FUN_001169f0(DAT_00182ef8);
  FUN_00116aa8(DAT_00182ef4);
  iVar1 = DAT_00182efc;
  *(undefined1 *)(DAT_00182efc + 0x18) = 1;
  *(undefined2 *)(iVar1 + 0x2c) = 0;
  *(undefined2 *)(iVar1 + 0x2a) = 0;
  FUN_00116b48();
  iVar1 = DAT_00182efc;
  if (DAT_00182ef0 != 0) {
    *(int *)(DAT_00182efc + 0x1c) = DAT_00182ef0;
  }
  *(undefined1 *)(iVar1 + 0x1a) = 0;
  return;
}

// ==== FUN_0010d8f0 @ 0010d8f0, 476 bytes
void FUN_0010d8f0(uint param_1)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  undefined1 uVar4;
  short *psVar5;
  int iVar6;
  uint uVar7;
  ushort *puVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  bool bVar14;
  bool bVar15;
  
  iVar6 = (int)*(short *)(DAT_00182efc + 0x22);
  piVar9 = *(int **)(*(int *)(DAT_00182efc + 0x1c) + 0x1c);
  iVar11 = (int)*(short *)(DAT_00182efc + 0x24);
  uVar7 = (uint)*(ushort *)(piVar9 + 3);
  bVar15 = param_1 <= uVar7;
  bVar14 = uVar7 == param_1;
  if (!bVar15 || bVar14) {
    bVar15 = *(ushort *)((int)piVar9 + 0xe) <= param_1;
    bVar14 = param_1 == *(ushort *)((int)piVar9 + 0xe);
  }
  if (bVar15 && !bVar14) {
    puVar8 = (ushort *)piVar9[2];
    iVar10 = piVar9[1];
    if (puVar8 == (ushort *)0x0) goto LAB_0010daac;
    uVar7 = (uint)*puVar8;
    bVar15 = param_1 <= uVar7;
    bVar14 = uVar7 == param_1;
    if (!bVar15 || bVar14) {
      bVar15 = puVar8[1] <= param_1;
      bVar14 = param_1 == puVar8[1];
    }
    if (bVar15 && !bVar14) goto LAB_0010daac;
    psVar5 = (short *)(*(int *)(puVar8 + 2) + (param_1 - uVar7 & 0xffff) * 4);
    iVar13 = (int)*psVar5;
    iVar12 = (int)psVar5[1];
  }
  else {
    iVar10 = *piVar9;
    iVar13 = param_1 - uVar7;
    iVar12 = -1;
  }
  if (iVar13 != -1) {
    bVar1 = *(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x18);
    uVar7 = (uint)*(short *)(DAT_00182efc + 0x2a);
    bVar2 = *(byte *)(piVar9 + 4);
    iVar3 = (int)(short)(ushort)bVar1 * (int)(short)(ushort)*(byte *)((int)piVar9 + 0x12);
    uVar4 = FUN_00116b48(uVar7);
    FUN_00115678(iVar6,iVar11,bVar2,bVar1,*(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1a),
                 *(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1b),1,
                 *(undefined1 *)((int)piVar9 + 0x12),iVar13 * iVar3 + iVar10,
                 *(undefined4 *)(DAT_00182efc + 0x38));
    if (iVar12 != -1) {
      FUN_00116b48(uVar7 | 2);
      FUN_00115678(iVar6,iVar11,bVar2,bVar1,*(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1a),
                   *(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1b),1,
                   *(undefined1 *)((int)piVar9 + 0x12),iVar12 * iVar3 + iVar10,
                   *(undefined4 *)(DAT_00182efc + 0x38));
    }
    if ((*(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x18) <
         *(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x19)) && (uVar7 != 2)) {
      FUN_00116b48(uVar7 ^ 4);
      iVar10 = *(int *)(DAT_00182efc + 0x1c);
      FUN_00115b98(iVar6,(int)(short)(ushort)*(byte *)(iVar10 + 0x18) *
                         (int)(short)(ushort)*(byte *)(iVar10 + 0x1b) + iVar11,
                   (int)(short)(ushort)bVar2 * (int)(short)(ushort)*(byte *)(iVar10 + 0x1a) + iVar6,
                   (uint)*(byte *)(iVar10 + 0x19) + iVar11);
    }
    FUN_00116b48(uVar4);
  }
LAB_0010daac:
  *(ushort *)(DAT_00182efc + 0x22) =
       (ushort)*(byte *)((int)piVar9 + 0x11) + *(short *)(DAT_00182efc + 0x22);
  return;
}

// ==== FUN_0010db64 @ 0010db64, 248 bytes
void FUN_0010db64(int param_1)

{
  short sVar1;
  ushort *puVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  
  sVar1 = *(short *)(DAT_00182efc + 0x2a);
  puVar2 = (ushort *)FUN_0010de6c(*(undefined4 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1c),param_1);
  if (puVar2 != (ushort *)0x0) {
    puVar4 = (undefined1 *)(*(int *)(puVar2 + 2) + (param_1 - (uint)*puVar2) * 8);
    uVar3 = FUN_00116b48((int)sVar1);
    if (DAT_00182f58 == (undefined4 *)0x0) {
      FUN_0010de9c((int)*(short *)(DAT_00182efc + 0x22),(int)*(short *)(DAT_00182efc + 0x24),*puVar4
                   ,*(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x18));
    }
    else {
      (*(code *)*DAT_00182f58)
                ((int)*(short *)(DAT_00182efc + 0x22),(int)*(short *)(DAT_00182efc + 0x24),*puVar4,
                 *(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x18));
    }
    FUN_00116b48(uVar3);
    *(ushort *)(DAT_00182efc + 0x22) = (ushort)(byte)puVar4[1] + *(short *)(DAT_00182efc + 0x22);
    return;
  }
  return;
}

// ==== FUN_0010dc64 @ 0010dc64, 88 bytes
undefined1 FUN_0010dc64(int param_1,uint *param_2)

{
  undefined1 uVar1;
  ushort *puVar2;
  
  puVar2 = (ushort *)FUN_0010de6c(*(undefined4 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1c),param_1);
  if (puVar2 == (ushort *)0x0) {
    return 0;
  }
  uVar1 = *(undefined1 *)(*(int *)(puVar2 + 2) + (param_1 - (uint)*puVar2) * 8 + 1);
  if (param_2 == (uint *)0x0) {
    return uVar1;
  }
  *param_2 = (uint)*(byte *)(*(int *)(puVar2 + 2) + (param_1 - (uint)*puVar2) * 8);
  return uVar1;
}

// ==== FUN_0010dccc @ 0010dccc, 24 bytes
bool FUN_0010dccc(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_0010de6c(*(undefined4 *)(param_1 + 0x1c));
  return iVar1 != 0;
}

// ==== FUN_0010dce4 @ 0010dce4, 332 bytes
void FUN_0010dce4(undefined4 param_1,int param_2)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  ushort *puVar4;
  int iVar5;
  undefined4 uVar6;
  code *pcVar7;
  int iVar8;
  int iVar9;
  ushort local_28;
  short local_26;
  short local_24;
  short local_22;
  undefined4 uStack_20;
  int iStack_1c;
  
  iVar9 = param_2 + -1;
  iVar8 = 0;
  bVar1 = *(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x19);
  bVar2 = *(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x1b);
  uStack_20 = param_1;
  iStack_1c = param_2;
  if (-1 < iVar9) {
    do {
      iVar3 = FUN_00113acc(&uStack_20);
      puVar4 = (ushort *)FUN_0010de3c(*(undefined4 *)(*(int *)(DAT_00182efc + 0x1c) + 0x1c),iVar3);
      if ((puVar4 != (ushort *)0x0) &&
         (iVar5 = *(int *)(puVar4 + 2) + (iVar3 - (uint)*puVar4) * 0xc,
         iVar5 = (int)(short)((ushort)*(byte *)(iVar5 + 4) + (short)*(char *)(iVar5 + 2)) *
                 (int)(short)(ushort)*(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x1a), 0 < iVar5)) {
        iVar8 = iVar8 + iVar5;
      }
      iVar9 = iVar9 + -1;
    } while (-1 < iVar9);
    if (iVar3 != 0) {
      iVar9 = FUN_00114e74(iVar3);
      iVar8 = iVar8 + iVar9;
    }
  }
  local_28 = *(ushort *)(DAT_00182efc + 0x22);
  local_26 = *(short *)(DAT_00182efc + 0x24);
  pcVar7 = (code *)((uint)local_28 + iVar8 + -1);
  local_24 = (short)pcVar7;
  local_22 = (ushort)bVar1 * (ushort)bVar2 + -1 + local_26;
  if (DAT_00182f58 != 0) {
    pcVar7 = *(code **)(DAT_00182f58 + 8);
  }
  if (DAT_00182f58 != 0 && pcVar7 != (code *)0x0) {
    (*pcVar7)(&local_28);
  }
  uVar6 = FUN_0010ebc4();
  if ((*(byte *)(DAT_00182efc + 0x2a) & 4) == 0) {
    FUN_0010eb80();
  }
  else {
    FUN_0010ebc4();
  }
  text_set_colour();
  FUN_00115b98((int)(short)local_28,(int)local_26,(int)local_24,(int)local_22);
  text_set_colour(uVar6);
  DAT_00183000 = 1;
  return;
}

// ==== FUN_0010de3c @ 0010de3c, 48 bytes
// called by: FUN_0010dce4
void FUN_0010de3c(ushort *param_1,uint param_2)

{
  if (param_1 == (ushort *)0x0) {
    return;
  }
  while ((param_2 < *param_1 || (param_1[1] < param_2))) {
    param_1 = *(ushort **)(param_1 + 4);
    if (param_1 == (ushort *)0x0) {
      return;
    }
  }
  return;
}

// ==== FUN_0010de6c @ 0010de6c, 48 bytes
// called by: FUN_0010db64, FUN_0010dc64, FUN_0010dccc
void FUN_0010de6c(ushort *param_1,uint param_2)

{
  if (param_1 == (ushort *)0x0) {
    return;
  }
  while ((param_2 < *param_1 || (param_1[1] < param_2))) {
    param_1 = *(ushort **)(param_1 + 4);
    if (param_1 == (ushort *)0x0) {
      return;
    }
  }
  return;
}

// ==== FUN_0010de9c @ 0010de9c, 552 bytes
// called by: FUN_0010db64
void FUN_0010de9c(undefined4 param_1,undefined4 param_2,undefined2 param_3,undefined2 param_4,
                 undefined2 param_5,undefined4 param_6)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
  if ((*(byte *)(DAT_00182efc + 0x10) & 2) != 0) {
    (*DAT_00182eac)();
    return;
  }
  iVar6 = **(int **)(DAT_00182efc + 0x3c);
  iVar7 = **(int **)(DAT_00182efc + 0x38);
  if ((DAT_00182eb0 != iVar6) || (DAT_00182eb4 != iVar7)) {
    uVar2 = FUN_00116370(iVar7);
    uVar3 = FUN_00116370(iVar6);
    uVar5 = 1;
    DAT_007bcf9c = uVar2;
    do {
      uVar4 = (*DAT_0018308c)(uVar3,uVar2,uVar5 & 0xff);
      (&DAT_007bcf9c)[uVar5] = uVar4;
      uVar5 = uVar5 + 1;
    } while ((int)uVar5 < 0xf);
    DAT_007bcfd8 = uVar3;
    FUN_001162e0(&DAT_00182ebc);
    DAT_00182eb0 = iVar6;
    DAT_00182eb4 = iVar7;
  }
  iVar6 = DAT_00182ed4;
  DAT_00182ecc = param_5;
  DAT_00182ec0 = DAT_00183000;
  DAT_00182ec8 = param_3;
  DAT_00182eca = param_4;
  DAT_00182ed0 = param_6;
  uVar1 = thunk_FUN_00116b48(0);
  uVar5 = uVar1;
  if (iVar6 != 0) {
    uVar5 = (uint)*(byte *)(iVar6 + 4);
  }
  if (iVar6 != 0 && uVar5 != 0) {
    uVar5 = uVar1 | 2;
  }
  else {
    uVar5 = uVar1 & 0xfffffffd;
  }
  thunk_FUN_00116b48(uVar5);
  if (DAT_00182ed8 == (undefined4 *)0x0) {
    iVar6 = (*DAT_00182eec)(DAT_00182ed4,&DAT_00182ec8,*(undefined1 *)(DAT_00182efc + 0x11));
    if (iVar6 == 0) {
      if (DAT_00182ece == 1) {
        iVar6 = *(int *)(DAT_00182efc + 0x38);
      }
      else {
        iVar6 = 0;
      }
    }
    FUN_00115678(param_1,param_2,DAT_00182ec8,DAT_00182eca,1,1,DAT_00182ece,DAT_00182ecc,
                 DAT_00182ed0,iVar6);
  }
  else {
    (*(code *)*DAT_00182ed8)
              (param_1,param_2,DAT_00182ec8,DAT_00182eca,DAT_00182ed0,DAT_00182ed4,1,1);
  }
                    /* WARNING: Could not recover jumptable at 0x00116b54. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_00183084 + 8))(uVar1);
  return;
}

// ==== FUN_0010dfe0 @ 0010dfe0, 12 bytes
// called by: FUN_00111618, FUN_001119fc
int FUN_0010dfe0(int *param_1)

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
  
  if (param_1 == (int *)0x0) {
    return 0;
  }
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

// ==== FUN_0010dfec @ 0010dfec, 160 bytes
// called by: FUN_00113c90, FUN_001174f0, FUN_00111618, FUN_0010fff0, jpeg_draw, jpeg_size
int FUN_0010dfec(int param_1)

{
  char *pcVar1;
  int *piVar2;
  byte *pbVar3;
  int iVar4;
  
  if (param_1 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = FUN_00119cb8();
    if (iVar4 != 0) {
      DAT_007bcff8 = DAT_007bcff8 + 1;
      pcVar1 = (char *)(DAT_007bd010 + iVar4 * 8);
      *pcVar1 = *pcVar1 + '\x01';
      piVar2 = *(int **)(DAT_007bd010 + iVar4 * 8 + 4);
      (*DAT_00183070)(piVar2 + 2,0,param_1);
      pbVar3 = (byte *)(DAT_007bd010 + *piVar2 * 8);
      if ((1 < *pbVar3) && (DAT_007bcff8 != 0)) {
        DAT_007bcff8 = DAT_007bcff8 + -1;
      }
      *pbVar3 = *pbVar3 - 1;
    }
  }
  return iVar4;
}

// ==== FUN_0010e094 @ 0010e094, 16 bytes
// called by: FUN_00113e68
void FUN_0010e094(undefined4 param_1,uint param_2)

{
  DAT_007bd09c = param_1;
  DAT_007bd0a0 = param_2 & 0xfffffffc;
  return;
}

// ==== FUN_0010e0a8 @ 0010e0a8, 88 bytes
// called by: FUN_00111618, FUN_0010fff0, FUN_00113e2c, jpeg_draw, FUN_0011b2e8, FUN_001188c4, FUN_00117768, FUN_001119fc, FUN_00110e78, jpeg_size
void FUN_0010e0a8(int param_1)

{
  int iVar1;
  
  if (param_1 == 0) {
    return;
  }
  iVar1 = param_1 * 8 + 4;
  FUN_00119b8c(*(undefined4 *)(*(int *)(DAT_007bd010 + iVar1) + 4));
  DAT_007bcfe0 = DAT_007bcfe0 + 1;
  *(int *)(DAT_007bd010 + iVar1) = DAT_007bd000;
  *(undefined1 *)(DAT_007bd010 + param_1 * 8) = 0;
  DAT_007bd000 = param_1;
  return;
}

// ==== FUN_0010e104 @ 0010e104, 36 bytes
// called by: FUN_00124d40, FUN_00124d80, display_set_rotation
void FUN_0010e104(int *param_1)

{
  if (param_1 == (int *)0x0) {
    return;
  }
  *param_1 = (int)DAT_007bd018;
  if (DAT_007bd018 != (int *)0x0) {
    *(int **)((int)DAT_007bd018 + 4) = param_1;
  }
  DAT_007bd018 = param_1;
  return;
}

// ==== FUN_0010e12c @ 0010e12c, 284 bytes
// called by: FUN_0012367c, FUN_0011a52c, FUN_00123ae8, FUN_00116528, FUN_0010e56c, FUN_00114b88, FUN_00125ab0, FUN_0012650c, FUN_00126468, FUN_00125944, FUN_00126404, FUN_0012637c, FUN_00126110, FUN_001264b8, display_set_rotation, FUN_0012656c, FUN_0010e4d8
uint * FUN_0010e12c(int param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  int *piVar4;
  int iVar5;
  
  FUN_00123b40();
  uVar3 = (param_1 + 3U & 0xfffffffc) + 4;
  puVar2 = DAT_007bd018;
  if (uVar3 < DAT_007bd01c) {
    uVar3 = DAT_007bd01c;
  }
  do {
    if (puVar2 == (uint *)0x0) {
LAB_0010e1b8:
      puVar2 = (uint *)0x0;
      iVar5 = DAT_007bd014 + DAT_007bcfe8;
      piVar4 = (int *)(iVar5 - *(int *)(iVar5 + -4));
      if (*piVar4 != 0) {
        FUN_001257f8(0);
        piVar4 = (int *)(iVar5 - *(int *)(iVar5 + -4));
        if (*piVar4 != 0) {
          return (uint *)0x0;
        }
      }
      if ((int)uVar3 <= piVar4[1]) {
        puVar1 = (uint *)((int)piVar4 + (piVar4[1] - uVar3));
        puVar2 = puVar1 + 1;
        FUN_0012576c(piVar4);
        DAT_007bcfe8 = DAT_007bcfe8 - uVar3;
        DAT_007bcff0 = DAT_007bcff0 + uVar3;
        if (piVar4[1] - uVar3 != 0) {
          FUN_00119b8c(piVar4[1] - uVar3,piVar4);
        }
        *puVar1 = uVar3;
      }
      return puVar2;
    }
    if (puVar2[-1] == uVar3) {
      if (puVar2 != (uint *)0x0) {
        if (*puVar2 != 0) {
          *(uint *)(*puVar2 + 4) = puVar2[1];
        }
        if ((uint *)puVar2[1] != (uint *)0x0) {
          *(uint *)puVar2[1] = *puVar2;
        }
        if (DAT_007bd018 == puVar2) {
          DAT_007bd018 = (uint *)*puVar2;
          return puVar2;
        }
        return puVar2;
      }
      goto LAB_0010e1b8;
    }
    puVar2 = (uint *)*puVar2;
  } while( true );
}

// ==== FUN_0010e24c @ 0010e24c, 56 bytes
// called by: FUN_001189d8, FUN_0011aabc, FUN_00121570, FUN_001174f0, FUN_00118250, FUN_0012297c, FUN_00122a60, FUN_00124394, FUN_001119fc, FUN_0011997c, FUN_00111114, FUN_001186b4, FUN_001195dc, FUN_00111618, FUN_00110c20, FUN_00126974, FUN_00118f24, FUN_00113e2c, FUN_00125d74, FUN_0011c47c, FUN_0011a5fc, FUN_0011091c, FUN_0011113c, FUN_00121e84, FUN_0010f030, FUN_001130bc, FUN_001130f8, FUN_001114e0, FUN_00111208, FUN_001221a8, FUN_00118650, FUN_00110b9c, FUN_001179fc, FUN_00118978, FUN_001119a4, FUN_0011147c, FUN_001110d4, FUN_00112f8c, FUN_001234f8, FUN_00125ecc, FUN_00111430, FUN_00119460, FUN_00123758, FUN_00119100, FUN_001265c8, FUN_00126d70, FUN_001237b8, FUN_00124510, FUN_0011158c, jpeg_size, FUN_0011acb8, FUN_00113c90, FUN_00125e20, FUN_00124b64, FUN_00119230, FUN_0012170c, FUN_00123450, FUN_001188c4, FUN_001183e0, FUN_00124e84, FUN_00123818, FUN_001117ac, FUN_0010fff0, FUN_001213bc, FUN_00126a38, FUN_00117424, FUN_00117768, FUN_00121ffc, FUN_00122b34, FUN_001196dc, FUN_0011ac14, FUN_00110e78, FUN_00118820, FUN_0011dd60, FUN_00113140, FUN_001234a4, FUN_001180e8, FUN_0011b2e8, FUN_0011d0f4, FUN_001268b4, FUN_001192f8, FUN_0011a7e8
int FUN_0010e24c(int param_1)

{
  char *pcVar1;
  
  DAT_007bcff8 = DAT_007bcff8 + 1;
  pcVar1 = (char *)(DAT_007bd010 + param_1 * 8);
  *pcVar1 = *pcVar1 + '\x01';
  return *(int *)(DAT_007bd010 + param_1 * 8 + 4) + 8;
}

// ==== FUN_0010e288 @ 0010e288, 164 bytes
// called by: FUN_00111618
undefined8 FUN_0010e288(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  FUN_00123b40();
  iVar1 = 0xf;
  do {
    if (*(int *)(iVar1 * 8 + 0x7bd020) != 0) {
      iVar1 = *(int *)(*(int *)(iVar1 * 8 + 0x7bd020) + 8);
      if (iVar1 != 0) goto LAB_0010e2c4;
      break;
    }
    iVar1 = iVar1 + -1;
  } while (-1 < iVar1);
  uVar3 = 0xfffffff4;
LAB_0010e2f0:
  return CONCAT44(DAT_007bcffc,DAT_007bcffc * ((int)uVar3 / 100));
LAB_0010e2c4:
  do {
    iVar2 = *(int *)(iVar1 + 8);
    if (uVar3 < *(uint *)(iVar1 + 4)) {
      uVar3 = *(uint *)(iVar1 + 4);
    }
    iVar1 = 0;
    if (iVar2 != 0) {
      iVar1 = *(int *)(iVar2 + 8);
    }
  } while (iVar2 != 0 && iVar1 != 0);
  uVar3 = uVar3 - 0xc;
  if (uVar3 < 0x400001) {
    return CONCAT44(DAT_007bcffc * uVar3 * 0x51eb851f,(DAT_007bcffc * uVar3) / 100);
  }
  goto LAB_0010e2f0;
}

// ==== FUN_0010e334 @ 0010e334, 76 bytes
// called by: FUN_001189d8, FUN_0011aabc, FUN_00121570, FUN_001174f0, FUN_00118250, FUN_0012297c, FUN_00122a60, FUN_00124394, FUN_001119fc, FUN_0011997c, FUN_00111114, FUN_001186b4, FUN_001195dc, FUN_00111618, FUN_00110c20, FUN_00126974, FUN_00118f24, FUN_00113e2c, FUN_00125d74, FUN_0011c47c, FUN_0011a5fc, FUN_0011091c, FUN_0011113c, FUN_00121e84, FUN_0010f030, FUN_001130bc, FUN_001130f8, FUN_001114e0, FUN_00111208, FUN_001221a8, FUN_00118650, FUN_00110b9c, FUN_001179fc, FUN_00118978, FUN_001119a4, FUN_0011147c, FUN_001110d4, FUN_00112f8c, FUN_001234f8, FUN_00125ecc, FUN_00111430, FUN_00119460, FUN_00123758, FUN_00119100, FUN_001265c8, FUN_00126d70, FUN_001237b8, FUN_00124510, FUN_0011158c, jpeg_size, FUN_0011acb8, FUN_00113c90, FUN_00125e20, FUN_00124b64, FUN_00119230, FUN_0012170c, FUN_00123450, FUN_001188c4, FUN_001183e0, FUN_00124e84, FUN_00123818, FUN_001117ac, FUN_0010fff0, FUN_001213bc, FUN_00126a38, FUN_00117424, FUN_00117768, FUN_00121ffc, FUN_00122b34, FUN_001196dc, FUN_0011ac14, FUN_00110e78, FUN_00118820, FUN_0011dd60, FUN_00113140, FUN_001234a4, FUN_001180e8, FUN_0011b2e8, FUN_0011d0f4, FUN_001268b4, FUN_001192f8, FUN_0011a7e8
undefined4 FUN_0010e334(int *param_1)

{
  byte *pbVar1;
  
  pbVar1 = (byte *)(DAT_007bd010 + *(int *)(*param_1 + -8) * 8);
  if ((1 < *pbVar1) && (DAT_007bcff8 != 0)) {
    DAT_007bcff8 = DAT_007bcff8 + -1;
  }
  *pbVar1 = *pbVar1 - 1;
  *param_1 = 0;
  return 0;
}

// ==== FUN_0010e384 @ 0010e384, 28 bytes
// called by: FUN_001189d8, FUN_001174f0, FUN_00119230, FUN_00118250, FUN_0012297c, FUN_001188c4, FUN_00122a60, FUN_00117f70, FUN_00124e84, FUN_00124394, FUN_00118ed0, FUN_001192bc, FUN_00117424, FUN_00117768, FUN_00122b34, FUN_00111208, FUN_00118330, FUN_001196dc, FUN_001179fc, FUN_00117f58, FUN_00117f88, FUN_001198a4, FUN_0011990c, FUN_001180e8, FUN_00118548, FUN_00119100, FUN_001192f8
int FUN_0010e384(int param_1)

{
  return *(int *)(DAT_007bd010 + param_1 * 8 + 4) + 8;
}

// ==== FUN_0010e3a4 @ 0010e3a4, 120 bytes
// called by: FUN_0011a428
int FUN_0010e3a4(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = (param_1 & 0xff) - (param_2 & 0xff);
  if ((int)uVar2 < 0) {
    uVar2 = -uVar2;
  }
  uVar3 = ((param_1 & 0xffff) >> 8) - ((param_2 & 0xffff) >> 8);
  if ((int)uVar3 < 0) {
    uVar3 = -uVar3;
  }
  uVar1 = (int)(((param_1 >> 0x10) - (param_2 >> 0x10)) * 0x10000) >> 0x10;
  if ((int)uVar1 < 0) {
    uVar1 = (int)(uVar1 * -0x10000) >> 0x10;
  }
  return (uVar1 & 0xffff) * (uVar1 & 0xffff) +
         (uVar3 & 0xffff) * (uVar3 & 0xffff) + (uVar2 & 0xffff) * (uVar2 & 0xffff);
}

// ==== FUN_0010e41c @ 0010e41c, 160 bytes
// called by: FUN_0012a964, lcd_boot_proc, idle_screen_proc, FUN_00126c5c, screen_create_proc
void FUN_0010e41c(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  
  FUN_0010ecc8(0);
  uVar1 = FUN_00116b48(4);
  iVar6 = *(int *)(DAT_00182efc + 0x48) + -0x3fff;
  iVar4 = *(int *)(DAT_00182efc + 0x48) + 0x3fff;
  iVar5 = *(int *)(DAT_00182efc + 0x4c) + -0x3fff;
  iVar3 = *(int *)(DAT_00182efc + 0x4c) + 0x3fff;
  local_20 = (undefined2)iVar6;
  local_1c = (undefined2)iVar4;
  local_1e = (undefined2)iVar5;
  local_1a = (undefined2)iVar3;
  iVar2 = FUN_00118f24(&local_20);
  while (iVar2 != 0) {
    FUN_00115b98(iVar6,iVar5,iVar4,iVar3);
    iVar2 = FUN_001189d8();
  }
  FUN_00116b48(uVar1);
  return;
}

// ==== FUN_0010e4c0 @ 0010e4c0, 20 bytes
// called by: FUN_001127f4, FUN_001128ac
void FUN_0010e4c0(void)

{
  if (DAT_00182f0c == (code *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x0010e4d0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_00182f0c)();
  return;
}

// ==== FUN_0010e4d8 @ 0010e4d8, 144 bytes
// called by: FUN_00111618, display_set_rotation
void FUN_0010e4d8(undefined4 param_1,int param_2,undefined2 param_3,int param_4)

{
  int *piVar1;
  undefined4 *puVar2;
  int *piVar3;
  
  if (3 < param_4) {
    return;
  }
  puVar2 = (undefined4 *)FUN_0010e12c(0x1c);
  if (puVar2 == (undefined4 *)0x0) {
    return;
  }
  if (param_2 == 0) {
    piVar3 = (int *)(&DAT_007bd120)[param_4];
    if (piVar3 != (int *)0x0) {
      for (piVar1 = (int *)*piVar3; piVar1 != (int *)0x0; piVar1 = (int *)*piVar1) {
        piVar3 = piVar1;
      }
      puVar2[4] = piVar3[4];
    }
  }
  else {
    puVar2[4] = param_2;
  }
  puVar2[6] = param_4;
  *(undefined2 *)(puVar2 + 5) = param_3;
  *puVar2 = 0;
  puVar2[3] = param_1;
  puVar2[1] = 0;
  puVar2[2] = 0;
  return;
}

// ==== FUN_0010e56c @ 0010e56c, 188 bytes
// called by: FUN_00116e40
undefined4 * FUN_0010e56c(undefined4 param_1,int param_2,undefined2 param_3,int param_4)

{
  int *piVar1;
  undefined4 *puVar2;
  int *piVar3;
  undefined4 *puVar4;
  
  puVar4 = (undefined4 *)0x0;
  if ((param_4 < 4) && (puVar2 = (undefined4 *)FUN_0010e12c(0x1c), puVar2 != (undefined4 *)0x0)) {
    if (param_2 == 0) {
      piVar3 = (int *)(&DAT_007bd120)[param_4];
      if (piVar3 != (int *)0x0) {
        for (piVar1 = (int *)*piVar3; piVar1 != (int *)0x0; piVar1 = (int *)*piVar1) {
          piVar3 = piVar1;
        }
        puVar2[4] = piVar3[4];
      }
    }
    else {
      puVar2[4] = param_2;
    }
    puVar2[6] = param_4;
    *(undefined2 *)(puVar2 + 5) = param_3;
    *puVar2 = 0;
    puVar2[3] = param_1;
    puVar2[1] = 0;
    puVar2[2] = 0;
    puVar4 = puVar2;
    if ((puVar2 != (undefined4 *)0x0) && (FUN_0010e634(), *(int *)puVar2[3] == 0)) {
      *(undefined4 **)(DAT_00182efc + param_4 * 4 + 0x54) = puVar2;
    }
  }
  return puVar4;
}

// ==== thunk_FUN_0010e104 @ 0010e630, 4 bytes
// called by: FUN_00111618, display_set_rotation, FUN_00110e78
void thunk_FUN_0010e104(int *param_1)

{
  if (param_1 == (int *)0x0) {
    return;
  }
  *param_1 = (int)DAT_007bd018;
  if (DAT_007bd018 != (int *)0x0) {
    *(int **)((int)DAT_007bd018 + 4) = param_1;
  }
  DAT_007bd018 = param_1;
  return;
}

// ==== FUN_0010e634 @ 0010e634, 280 bytes
// called by: FUN_0010e56c, FUN_00110c20, display_set_rotation, FUN_0011147c
undefined4 FUN_0010e634(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  uint uVar4;
  int *piVar5;
  bool bVar6;
  
  iVar1 = 0;
  if (param_1 != (int *)0x0) {
    param_3 = param_1[6];
    iVar1 = param_3 + -4;
  }
  if (iVar1 < 0 == (param_1 != (int *)0x0 && SBORROW4(param_3,4))) {
    return 1;
  }
  if ((int)(uint)DAT_00182f50 < param_3 + 1) {
    DAT_00182f50 = (byte)(param_3 + 1);
  }
  piVar2 = (int *)(&DAT_007bd120)[param_3];
  if (piVar2 == (int *)0x0) {
    (&DAT_007bd120)[param_3] = param_1;
    return 0;
  }
  piVar5 = (int *)piVar2[1];
  if ((int *)piVar2[1] != (int *)0x0) {
    do {
      piVar2 = piVar5;
      piVar5 = (int *)piVar2[1];
    } while ((int *)piVar2[1] != (int *)0x0);
    if (piVar2 == (int *)0x0) {
      return 1;
    }
  }
  uVar3 = *(uint *)param_1[3];
  while ((int)uVar3 < *(int *)piVar2[3]) {
    piVar2 = (int *)*piVar2;
    if (piVar2 == (int *)0x0) {
      return 1;
    }
  }
  if (piVar2 == (int *)0x0) {
    return 1;
  }
  uVar4 = *(uint *)piVar2[3];
  bVar6 = uVar4 == uVar3;
  if (bVar6) {
    uVar4 = (uint)*(ushort *)(param_1 + 5);
  }
  piVar5 = piVar2;
  if (bVar6 && uVar4 == 1) {
    do {
      piVar5 = (int *)*piVar2;
      if (piVar5 == (int *)0x0) {
        piVar5 = piVar2;
        if (piVar2 == (int *)0x0) {
          return 1;
        }
        break;
      }
      piVar2 = piVar5;
    } while ((int)uVar3 <= *(int *)piVar5[3]);
  }
  if ((int *)piVar5[1] == (int *)0x0) {
    (&DAT_007bd120)[param_3] = param_1;
  }
  else {
    *(int *)piVar5[1] = (int)param_1;
    param_1[1] = piVar5[1];
  }
  piVar5[1] = (int)param_1;
  if (param_1 == piVar5) {
    piVar5 = (int *)0x0;
  }
  *param_1 = (int)piVar5;
  return 0;
}

// ==== FUN_0010e754 @ 0010e754, 88 bytes
// called by: FUN_0011697c, display_set_rotation
void FUN_0010e754(int *param_1)

{
  if ((int *)(&DAT_007bd120)[param_1[6]] == param_1) {
    (&DAT_007bd120)[param_1[6]] = *param_1;
    if (*param_1 != 0) {
      *(undefined4 *)(*param_1 + 4) = 0;
    }
  }
  if ((int *)param_1[1] != (int *)0x0) {
    *(int *)param_1[1] = *param_1;
  }
  if (*param_1 != 0) {
    *(int *)(*param_1 + 4) = param_1[1];
  }
  *param_1 = 0;
  param_1[1] = 0;
  return;
}

// ==== FUN_0010e7b0 @ 0010e7b0, 60 bytes
// called by: display_set_rotation
int FUN_0010e7b0(int param_1,int param_2)

{
  int *piVar1;
  
  if (param_1 < 4) {
    for (piVar1 = (int *)(&DAT_007bd120)[param_1]; piVar1 != (int *)0x0; piVar1 = (int *)*piVar1) {
      if (*(int *)piVar1[3] == param_2) {
        return (int)piVar1;
      }
    }
  }
  return 0;
}

// ==== FUN_0010e7f0 @ 0010e7f0, 56 bytes
// called by: FUN_001127f4, FUN_00116d10, FUN_00116d68, FUN_00112674, FUN_00116b5c, FUN_00115c40, FUN_0011615c, FUN_00116db8, FUN_001160c0, FUN_00115f8c, FUN_001160fc, FUN_00115f04, FUN_00114f5c, FUN_00115df8, FUN_00115cf8, FUN_00116064, FUN_00115e28, FUN_00115f50, FUN_00115fc8, FUN_00116028, FUN_00115d34, FUN_001128ac, FUN_00116580, FUN_00115dbc, FUN_00117fa8, FUN_00116ae0, FUN_00110d58, FUN_00115c8c, FUN_001163d4, FUN_00112bb8, FUN_001161f4, display_set_rotation, FUN_00115d70, FUN_001161b8
void FUN_0010e7f0(int param_1)

{
  int *piVar1;
  int *piVar2;
  
  if (3 < param_1) {
    return;
  }
  piVar2 = (int *)(&DAT_007bd120)[param_1];
  if (piVar2 != (int *)0x0) {
    do {
      piVar1 = piVar2 + 3;
      if (*(int *)*piVar1 != 0) {
        piVar2 = (int *)*piVar2;
      }
    } while (*(int *)*piVar1 != 0 && piVar2 != (int *)0x0);
    return;
  }
  return;
}

// ==== FUN_0010e82c @ 0010e82c, 60 bytes
// called by: idle_screen_proc
void FUN_0010e82c(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_00113eb4();
  while( true ) {
    iVar2 = FUN_00113eb4();
    iVar2 = (iVar1 + param_1) - iVar2;
    if (iVar2 < 1) break;
    FUN_0010ea18();
    iVar3 = iRam00182f88;
    if (iVar2 <= iRam00182f88) {
      iVar3 = iVar2;
    }
    FUN_00113e8c(iVar3);
  }
  return;
}

// ==== text_draw @ 0010ea00, 416 bytes
// called by: idle_screen_proc
// text_draw(str, x, y) with the selected font and colour
/* text_draw(str, x, y) with the selected font and colour */

void text_draw(char *param_1,undefined2 param_2,undefined2 param_3)

{
  undefined2 uVar1;
  int iVar2;
  byte bVar3;
  short sVar4;
  short sVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  short local_30;
  short local_2e;
  short local_2c;
  short local_2a;
  
  iVar2 = DAT_00182efc;
  *(undefined2 *)(DAT_00182efc + 0x22) = param_2;
  *(undefined2 *)(iVar2 + 0x24) = param_3;
  if (param_1 != (char *)0x0) {
    sVar4 = FUN_0010ebd4();
    uVar1 = *(undefined2 *)(DAT_00182efc + 0x22);
    sVar5 = FUN_0010ec4c();
    *(short *)(DAT_00182efc + 0x24) = *(short *)(DAT_00182efc + 0x24) - sVar5;
    if (*param_1 != '\0') {
      do {
        uVar6 = FUN_00114d70(param_1,0x7fff);
        iVar7 = FUN_00114ca0(param_1,uVar6);
        iVar2 = DAT_00182efc;
        bVar3 = *(byte *)(DAT_00182efc + 0x2c) & 3;
        iVar8 = iVar7;
        if (bVar3 != 1) {
          if (bVar3 == 2) {
            iVar8 = iVar7 / 2;
          }
          else {
            iVar8 = 0;
          }
        }
        local_30 = *(short *)(DAT_00182efc + 0x22) - (short)iVar8;
        *(short *)(DAT_00182efc + 0x22) = local_30;
        local_2c = (short)iVar7 + -1 + local_30;
        local_2e = *(short *)(iVar2 + 0x24);
        local_2a = local_2e + sVar4 + -1;
        FUN_001143a8(param_1,uVar6,&local_30);
        *(short *)(DAT_00182efc + 0x24) = local_2e;
        iVar8 = FUN_00113b14(param_1,uVar6);
        iVar2 = DAT_00182efc;
        param_1 = param_1 + iVar8;
        if (*param_1 == '\n' || *param_1 == '\r') {
          bVar3 = *(byte *)(DAT_00182efc + 0x2c) & 3;
          if (bVar3 == 1 || bVar3 == 2) {
            *(undefined2 *)(DAT_00182efc + 0x22) = uVar1;
          }
          else {
            *(undefined2 *)(DAT_00182efc + 0x22) = *(undefined2 *)(DAT_00182efc + 0x20);
          }
          if (*param_1 == '\n') {
            *(short *)(iVar2 + 0x24) = *(short *)(iVar2 + 0x24) + sVar4;
          }
        }
        else {
          *(short *)(DAT_00182efc + 0x22) = local_30 + (short)iVar7;
        }
      } while ((*param_1 != '\0') && (param_1 = param_1 + 1, *param_1 != '\0'));
    }
    iVar2 = DAT_00182efc;
    *(short *)(DAT_00182efc + 0x24) = *(short *)(DAT_00182efc + 0x24) + sVar5;
    *(ushort *)(iVar2 + 0x2c) = *(ushort *)(iVar2 + 0x2c) & 0xfffc;
    return;
  }
  return;
}

// ==== FUN_0010ea18 @ 0010ea18, 200 bytes
// called by: FUN_0010e82c
undefined4 FUN_0010ea18(void)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar3 = 0;
  bVar1 = false;
  if ((DAT_00182f08 != (code *)0x0) && (iVar2 = (*DAT_00182f08)(), iVar2 != 0)) {
    bVar1 = true;
  }
  iVar2 = FUN_001179fc();
  if (iVar2 != 0) {
    bVar1 = true;
  }
  if ((DAT_00182f34 != (code *)0x0) && (iVar2 = (*DAT_00182f34)(), iVar2 != 0)) {
    bVar1 = true;
  }
  if (DAT_00182f00 != (code *)0x0) {
    (*DAT_00182f00)();
  }
  if (bVar1) {
    do {
      uVar3 = 1;
      bVar1 = false;
      if ((DAT_00182f08 != (code *)0x0) && (iVar2 = (*DAT_00182f08)(), iVar2 != 0)) {
        bVar1 = true;
      }
      iVar2 = FUN_001179fc();
      if (iVar2 != 0) {
        bVar1 = true;
      }
      if ((DAT_00182f34 != (code *)0x0) && (iVar2 = (*DAT_00182f34)(), iVar2 != 0)) {
        bVar1 = true;
      }
      if (DAT_00182f00 != (code *)0x0) {
        (*DAT_00182f00)();
      }
    } while (bVar1);
  }
  return uVar3;
}

// ==== FUN_0010eaec @ 0010eaec, 116 bytes
// called by: usb_command
void FUN_0010eaec(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  
  param_1 = *(int *)(DAT_00182efc + 0x48) + param_1;
  param_3 = *(int *)(DAT_00182efc + 0x48) + param_3;
  param_2 = *(int *)(DAT_00182efc + 0x4c) + param_2;
  param_4 = *(int *)(DAT_00182efc + 0x4c) + param_4;
  local_20 = (undefined2)param_1;
  local_1c = (undefined2)param_3;
  local_1e = (undefined2)param_2;
  local_1a = (undefined2)param_4;
  iVar1 = FUN_00118f24(&local_20);
  if (iVar1 != 0) {
    do {
      FUN_00115b98(param_1,param_2,param_3,param_4);
      iVar1 = FUN_001189d8();
    } while (iVar1 != 0);
    return;
  }
  return;
}

// ==== FUN_0010eb64 @ 0010eb64, 24 bytes
// called by: FUN_0011b2e8, FUN_001119fc, FUN_001146cc, FUN_00116638
void FUN_0010eb64(int param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0010eb78. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[param_1] + 0xc) + 0x28))((&DAT_007bd120)[param_1],8);
  return;
}

// ==== FUN_0010eb80 @ 0010eb80, 16 bytes
// called by: FUN_0010dce4
void FUN_0010eb80(void)

{
  FUN_00115cc8();
                    /* WARNING: Could not emulate address calculation at 0x00116380 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10) + 4))();
  return;
}

// ==== FUN_0010eb90 @ 0010eb90, 24 bytes
// called by: FUN_00114ca0
void FUN_0010eb90(undefined4 param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0010eba4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(DAT_00182efc + 0x1c) + 4))(param_1,0);
  return;
}

// ==== FUN_0010ebac @ 0010ebac, 20 bytes
// called by: FUN_00114e74, FUN_00114ca0
void FUN_0010ebac(void)

{
                    /* WARNING: Could not recover jumptable at 0x0010ebbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(DAT_00182efc + 0x1c) + 4))();
  return;
}

// ==== FUN_0010ebc4 @ 0010ebc4, 16 bytes
// called by: FUN_0010dce4
void FUN_0010ebc4(void)

{
  FUN_00115ce0();
                    /* WARNING: Could not emulate address calculation at 0x00116380 */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(DAT_00182efc + 0x11)] + 0x10) + 4))();
  return;
}

// ==== FUN_0010ebd4 @ 0010ebd4, 20 bytes
// called by: text_draw
undefined1 FUN_0010ebd4(void)

{
  return *(undefined1 *)(*(int *)(DAT_00182efc + 0x1c) + 0x19);
}

// ==== FUN_0010ebec @ 0010ebec, 28 bytes
// called by: idle_screen_proc
int FUN_0010ebec(void)

{
  return (int)(short)(ushort)*(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x18) *
         (int)(short)(ushort)*(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x1b);
}

// ==== FUN_0010ec0c @ 0010ec0c, 28 bytes
// called by: FUN_00110ed8
void FUN_0010ec0c(int *param_1,int *param_2)

{
  *param_1 = (int)DAT_00182f5c;
  *param_2 = (int)DAT_00182f5e;
  return;
}

// ==== text_width @ 0010ec30, 28 bytes
// called by: idle_screen_proc
// text_width(str) in pixels with the selected font
/* text_width(str) in pixels with the selected font */

int text_width(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  code *pcVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int aiStack_20 [2];
  int iStack_18;
  int aiStack_14 [2];
  
  aiStack_14[0] = FUN_00114dec();
  pcVar3 = (code *)0x0;
  iStack_18 = param_1;
  iStack_18 = (*DAT_00182ff4)(param_1,aiStack_14);
  iVar4 = 0;
  if (iStack_18 != 0) {
    uVar5 = 0;
    puVar2 = *(undefined4 **)(*(int *)(DAT_00182efc + 0x1c) + 0x14);
    if (puVar2 != (undefined4 *)0x0) {
      pcVar3 = (code *)*puVar2;
    }
    if (puVar2 != (undefined4 *)0x0 && pcVar3 != (code *)0x0) {
      iVar4 = (*pcVar3)(iStack_18,aiStack_14[0]);
      return iVar4;
    }
    while (aiStack_14[0] = aiStack_14[0] + -1, -1 < aiStack_14[0]) {
      uVar5 = FUN_00113acc(&iStack_18);
      iVar1 = FUN_0010eb90();
      iVar4 = iVar4 + iVar1;
    }
    iVar6 = 0;
    aiStack_20[0] = 0;
    iVar1 = FUN_0010ebac(uVar5,aiStack_20);
    if (iVar1 < aiStack_20[0]) {
      iVar6 = aiStack_20[0] - iVar1;
    }
    iVar4 = iVar4 + iVar6;
  }
  return iVar4;
}

// ==== FUN_0010ec4c @ 0010ec4c, 72 bytes
// called by: text_draw
uint FUN_0010ec4c(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(DAT_00182efc + 0x2c) & 0xc;
  if (bVar1 == 4) {
    return *(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x18) - 1;
  }
  if (bVar1 != 8 && bVar1 != 0xc) {
    return 0;
  }
  return (uint)(*(byte *)(*(int *)(DAT_00182efc + 0x1c) + 0x18) >> 1);
}

// ==== FUN_0010ec98 @ 0010ec98, 20 bytes
// called by: FUN_001119fc
undefined4 FUN_0010ec98(int param_1)

{
  return **(undefined4 **)((&DAT_007bd120)[param_1] + 0x10);
}

// ==== FUN_0010ecb0 @ 0010ecb0, 20 bytes
// called by: FUN_001119fc, FUN_001146cc
undefined4 FUN_0010ecb0(int param_1)

{
  return *(undefined4 *)(*(int *)((&DAT_007bd120)[param_1] + 0x10) + 4);
}

// ==== FUN_0010ecc8 @ 0010ecc8, 24 bytes
// called by: FUN_0010e41c
undefined4 FUN_0010ecc8(undefined2 param_1,undefined2 param_2)

{
  int iVar1;
  
  iVar1 = DAT_00182efc;
  *(undefined2 *)(DAT_00182efc + 0x22) = param_1;
  *(undefined2 *)(iVar1 + 0x24) = param_2;
  return 0;
}

// ==== FUN_0010ece4 @ 0010ece4, 412 bytes
// called by: screen_create_proc
undefined4 FUN_0010ece4(void)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  
  uVar1 = DAT_b0000000;
  if (((~uVar1 & 0xfa5c30) == 0) && (uVar1 = DAT_b800100c, (uVar1 & 0x100) == 0)) {
    if (DAT_00182f20 == 0) {
      DAT_00182f20 = 1;
      FUN_00113e68();
      FUN_00116e40();
      DAT_00182f24 = FUN_00124c38;
      DAT_00182f28 = DAT_00182f18;
      DAT_00182f18 = &DAT_00182f24;
    }
    DAT_00182ee8 = 0x2e;
    FUN_00113ec4();
    iVar3 = DAT_00182efc;
    DAT_007bd11c = "SEGGER emWin V5481110";
    iVar6 = DAT_00182efc + 4;
    *(int *)(DAT_00182efc + 0x38) = DAT_00182efc;
    DAT_007bd0bc = iVar3 + 8;
    *(int *)(iVar3 + 0x3c) = iVar6;
    (**(code **)(*(int *)((&DAT_007bd120)[*(byte *)(iVar3 + 0x11)] + 0xc) + 0x30))
              ((&DAT_007bd120)[*(byte *)(iVar3 + 0x11)],&DAT_007bd0b0);
    DAT_007bd110 = &DAT_00180250;
    DAT_007bd0c4 = DAT_00182ef0;
    DAT_007bd0bc = DAT_00182efc + 8;
    DAT_007bd0c0 = 1;
    DAT_007bd0c3 = 1;
    DAT_007bd0ec = FUN_00117f38();
    DAT_007bd114 = 3;
    DAT_007bd0d8 = 0x12345678;
    DAT_007bd0dc = 0x12345678;
    FUN_001169f0(DAT_00182ef8);
    FUN_00116aa8(DAT_00182ef4);
    DAT_00182f04 = &PTR_LAB_00181f08;
    uVar2 = FUN_001163d4();
    iVar3 = FUN_001160b0();
    iVar6 = FUN_001161a8();
    if (iVar6 == 0 || iVar3 == 0) {
      uVar4 = FUN_001161f4();
      uVar5 = FUN_001160fc();
      FUN_00116b38(uVar5,uVar4);
    }
    FUN_00112c44(0);
    FUN_00117fa8();
    for (; DAT_00182f1c != (undefined4 *)0x0; DAT_00182f1c = (undefined4 *)DAT_00182f1c[1]) {
      if ((code *)*DAT_00182f1c != (code *)0x0) {
        (*(code *)*DAT_00182f1c)();
      }
    }
    DAT_00182eea = 1;
    return uVar2;
  }
  return 0xffffffff;
}

// ==== jpeg_draw @ 0010eeb8, 208 bytes
// called by: idle_screen_proc
// ? jpeg_draw(jpeg, length, x, y)
/* ? jpeg_draw(jpeg, length, x, y) */

undefined4 jpeg_draw(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_28;
  undefined4 uStack_24;
  
  local_28 = param_1;
  uStack_24 = param_2;
  DAT_00182f74 = param_1;
  DAT_00182f78 = param_2;
  if (DAT_00182f60 == (code *)0x0) {
    iVar2 = FUN_0010dfec(&DAT_00008498);
    if (iVar2 == 0) goto LAB_0010ef44;
    iVar1 = FUN_0011b2e8(iVar2,&LAB_0010ffb0,&local_28,param_3);
    FUN_0010e0a8(iVar2);
  }
  else {
    iVar1 = (*DAT_00182f60)(&LAB_0010ffb0,&local_28);
  }
  if (iVar1 == 0) {
    return 0;
  }
LAB_0010ef44:
  uVar3 = 1;
  iVar2 = FUN_0010dfec(&DAT_00008498);
  if (iVar2 != 0) {
    uVar3 = FUN_0011b2e8(iVar2,&LAB_0010ffb0,&local_28,param_3);
    FUN_0010e0a8(iVar2);
  }
  return uVar3;
}

// ==== jpeg_size @ 0010ef94, 148 bytes
// called by: idle_screen_proc
// ? jpeg_size(jpeg, length, &width_height): centres the idle picture with it
/* ? jpeg_size(jpeg, length, &width_height): centres the idle picture with it */

undefined4 jpeg_size(undefined4 param_1,undefined4 param_2,uint *param_3)

{
  int iVar1;
  undefined4 uVar2;
  int local_38 [2];
  int local_30 [4];
  undefined4 local_20;
  undefined4 uStack_1c;
  
  uVar2 = 1;
  local_20 = param_1;
  uStack_1c = param_2;
  iVar1 = FUN_0010dfec(&DAT_00008498);
  if (iVar1 != 0) {
    local_30[0] = FUN_0010e24c();
    *(undefined1 **)(local_30[0] + 4) = &LAB_0010ffb0;
    *(undefined4 **)(local_30[0] + 8) = &local_20;
    FUN_0010e334(local_30);
    uVar2 = FUN_0011091c(iVar1);
    if (param_3 != (uint *)0x0) {
      local_38[0] = FUN_0010e24c(iVar1);
      *param_3 = (uint)*(ushort *)(local_38[0] + 0x10);
      param_3[1] = (uint)*(ushort *)(local_38[0] + 0x12);
      FUN_0010e334(local_38);
    }
    FUN_0010e0a8(iVar1);
  }
  return uVar2;
}

// ==== FUN_0010f030 @ 0010f030, 3892 bytes
// called by: FUN_0011b2e8
undefined4 FUN_0010f030(int param_1)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  ushort uVar4;
  ushort uVar5;
  ushort uVar6;
  ushort uVar7;
  ushort uVar8;
  ushort uVar9;
  ushort uVar10;
  undefined1 uVar11;
  undefined4 uVar12;
  ushort *puVar13;
  undefined1 *puVar14;
  undefined **ppuVar15;
  undefined1 *puVar16;
  byte *pbVar17;
  byte *pbVar18;
  uint uVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  undefined *puVar23;
  int iVar24;
  undefined4 *unaff_r7;
  int iVar25;
  int iVar26;
  undefined *puVar27;
  int iVar28;
  int iVar29;
  int iVar30;
  int iVar31;
  uint uVar32;
  int iVar33;
  undefined *puVar34;
  bool bVar35;
  byte local_f8 [4];
  undefined *local_f4;
  undefined **local_ec;
  undefined *local_e8;
  undefined *local_e4;
  int local_e0;
  int local_dc;
  undefined **local_d8;
  int local_d4;
  int local_d0;
  ushort *local_cc;
  ushort *local_c8;
  undefined *local_c4;
  undefined **local_c0;
  int local_bc;
  undefined *local_b8;
  undefined **local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  int local_a0;
  int local_9c;
  int local_98;
  int local_94;
  int local_90;
  undefined **local_8c;
  int local_88;
  int local_84;
  ushort local_80 [8];
  ushort local_70 [8];
  ushort local_60 [8];
  ushort local_50 [8];
  ushort local_40 [8];
  int local_30;
  undefined1 *local_28;
  
  local_bc = param_1 + 0x7000;
  if (*(int *)(&DAT_00007488 + param_1) == 0) {
    if ((&DAT_00007409)[param_1] == '\0') {
      iVar28 = FUN_0011acb8(param_1);
      if (iVar28 != 0) {
        return 1;
      }
      if (*(int *)(local_bc + 0x484) <= *(int *)(local_bc + 0x420)) {
        uVar32 = FUN_00125054(param_1,local_f8);
        bVar35 = uVar32 == 0;
        if (bVar35) {
          uVar32 = (uint)local_f8[0];
        }
        if (!bVar35 || uVar32 != 0xd9) {
          return 1;
        }
      }
    }
    else {
      iVar28 = FUN_00124510();
      if (iVar28 != 0) {
        return 1;
      }
    }
    *(undefined1 *)(local_bc + 0x408) = 1;
    *(undefined4 *)(local_bc + 0x488) = *(undefined4 *)(local_bc + 0x420);
LAB_0010f060:
    if (*(int *)(local_bc + 0x480) == 0) {
      uVar12 = FUN_0010e24c(*(undefined4 *)(local_bc + 0x47c));
      *(undefined4 *)(local_bc + 0x480) = uVar12;
    }
    local_28 = *(undefined1 **)(local_bc + 0x480);
    local_b4 = (undefined **)FUN_0010e24c(*(undefined4 *)(local_bc + 0x474));
    if (local_b4 != (undefined **)0x0) {
      for (local_30 = 0; local_30 < *(int *)(local_bc + 0x418); local_30 = local_30 + 1) {
        (*DAT_00183074)(&local_b0,local_b4 + local_30 * 0x20,0x80);
        puVar16 = local_28;
        local_c8 = (ushort *)&local_b0;
        iVar28 = 7;
        puVar13 = local_c8;
        do {
          uVar4 = puVar13[1];
          uVar5 = puVar13[2];
          uVar6 = puVar13[3];
          uVar7 = puVar13[4];
          uVar8 = puVar13[5];
          uVar9 = puVar13[6];
          uVar10 = puVar13[7];
          if (((((uVar4 == 0 && uVar5 == 0) && (uVar6 == 0 && uVar7 == 0)) && uVar8 == 0) &&
              uVar9 == 0) && uVar10 == 0) {
            uVar4 = *puVar13 << 2;
            *puVar13 = uVar4;
            puVar13[1] = uVar4;
            puVar13[2] = uVar4;
            puVar13[3] = uVar4;
            puVar13[4] = uVar4;
            puVar13[5] = uVar4;
            puVar13[6] = uVar4;
            puVar13[7] = uVar4;
          }
          else {
            iVar29 = ((int)(short)uVar5 + (int)(short)uVar9) * 0x1151;
            iVar25 = (short)uVar9 * -0x3b21 + iVar29;
            iVar29 = iVar29 + (short)uVar5 * 0x187e;
            iVar22 = ((uint)*puVar13 + (int)(short)uVar7) * 0x2000;
            local_d4 = ((uint)*puVar13 - (int)(short)uVar7) * 0x2000;
            local_d8 = (undefined **)(iVar22 + iVar29);
            local_b8 = (undefined *)(local_d4 + iVar25);
            local_d4 = local_d4 - iVar25;
            iVar22 = iVar22 - iVar29;
            local_e8 = (undefined *)((int)(short)uVar10 + (int)(short)uVar6);
            iVar25 = (int)(short)uVar8 + (int)(short)uVar4;
            iVar29 = ((int)local_e8 + iVar25) * 0x25a1;
            iVar31 = iVar25 * -0xc7c + iVar29;
            iVar26 = ((int)(short)uVar8 + (int)(short)uVar6) * -0x5203;
            iVar30 = ((int)(short)uVar10 + (int)(short)uVar4) * -0x1ccd;
            iVar25 = (short)uVar8 * 0x41b3 + iVar31 + iVar26;
            puVar13[2] = (ushort)((uint)((local_d4 + iVar25) * 0x20 + 0x8000) >> 0x10);
            puVar13[5] = (ushort)((uint)((local_d4 - iVar25) * 0x20 + 0x8000) >> 0x10);
            iVar31 = iVar30 + iVar31 + (short)uVar4 * 0x300b;
            iVar29 = iVar29 + (int)local_e8 * -0x3ec5;
            iVar25 = (short)uVar10 * 0x98e + iVar30 + iVar29;
            iVar29 = (short)uVar6 * 0x6254 + iVar29 + iVar26;
            *puVar13 = (ushort)((uint)(((int)local_d8 + iVar31) * 0x20 + 0x8000) >> 0x10);
            puVar13[7] = (ushort)((uint)(((int)local_d8 - iVar31) * 0x20 + 0x8000) >> 0x10);
            puVar13[1] = (ushort)((uint)(((int)local_b8 + iVar29) * 0x20 + 0x8000) >> 0x10);
            puVar13[6] = (ushort)((uint)(((int)local_b8 - iVar29) * 0x20 + 0x8000) >> 0x10);
            puVar13[3] = (ushort)((uint)((iVar22 + iVar25) * 0x20 + 0x8000) >> 0x10);
            puVar13[4] = (ushort)((uint)((iVar22 - iVar25) * 0x20 + 0x8000) >> 0x10);
          }
          puVar13 = puVar13 + 8;
          iVar28 = iVar28 + -1;
        } while (-1 < iVar28);
        iVar28 = 7;
        puVar13 = local_c8;
        do {
          uVar4 = puVar13[8];
          uVar5 = puVar13[0x10];
          uVar6 = puVar13[0x18];
          unaff_r7 = (undefined4 *)(int)(short)puVar13[0x20];
          uVar7 = puVar13[0x28];
          uVar8 = puVar13[0x30];
          uVar9 = puVar13[0x38];
          if (((((uVar4 == 0 && uVar5 == 0) && (uVar6 == 0 && puVar13[0x20] == 0)) && uVar7 == 0) &&
              uVar8 == 0) && uVar9 == 0) {
            iVar22 = ((short)*puVar13 + 0x10 >> 5) + 0x80;
            if (iVar22 < 0) {
              iVar22 = 0;
            }
            else if (0xff < iVar22) {
              iVar22 = 0xff;
            }
            uVar11 = (undefined1)iVar22;
            *puVar16 = uVar11;
            puVar16[8] = uVar11;
            puVar16[0x10] = uVar11;
            puVar16[0x18] = uVar11;
            puVar16[0x20] = uVar11;
            puVar16[0x28] = uVar11;
            puVar16[0x30] = uVar11;
            puVar16[0x38] = uVar11;
          }
          else {
            iVar25 = ((int)(short)uVar5 + (int)(short)uVar8) * 0x1151;
            iVar22 = ((int)(short)*puVar13 + (int)unaff_r7) * 0x2000;
            iVar29 = iVar25 + (short)uVar5 * 0x187e;
            local_d0 = iVar22 - iVar29;
            local_d4 = ((int)(short)*puVar13 - (int)unaff_r7) * 0x2000;
            iVar25 = (short)uVar8 * -0x3b21 + iVar25;
            local_b8 = (undefined *)(local_d4 + iVar25);
            local_d8 = (undefined **)(iVar29 + iVar22);
            local_d4 = local_d4 - iVar25;
            iVar22 = (int)(short)uVar7 + (int)(short)uVar4;
            iVar29 = (int)(short)uVar9 + (int)(short)uVar6;
            local_cc = (ushort *)(((int)(short)uVar9 + (int)(short)uVar4) * -0x1ccd);
            iVar25 = (iVar29 + iVar22) * 0x25a1;
            local_e4 = (undefined *)(iVar22 * -0xc7c + iVar25);
            unaff_r7 = (undefined4 *)((int)local_cc + (int)local_e4 + (short)uVar4 * 0x300b);
            iVar22 = ((int)(short)uVar7 + (int)(short)uVar6) * -0x5203;
            uVar32 = ((int)unaff_r7 + (int)local_d8 + 0x20000 >> 0x12) + 0x80;
            iVar25 = iVar25 + iVar29 * -0x3ec5;
            uVar19 = (short)uVar9 * 0x98e;
            iVar29 = iVar22 + (int)local_e4 + (short)uVar7 * 0x41b3;
            bVar35 = (uVar32 & 0xff00) != 0;
            iVar31 = (int)local_cc + iVar25 + uVar19;
            if (bVar35) {
              uVar19 = ~((int)uVar32 >> 8);
            }
            if (bVar35) {
              uVar32 = uVar19 >> 0x18;
            }
            iVar22 = iVar22 + iVar25 + (short)uVar6 * 0x6254;
            *puVar16 = (char)uVar32;
            uVar32 = (((int)local_d8 - (int)unaff_r7) + 0x20000 >> 0x12) + 0x80;
            bVar35 = (uVar32 & 0xff00) != 0;
            if (bVar35) {
              uVar32 = ~((int)uVar32 >> 8);
            }
            if (bVar35) {
              uVar32 = uVar32 >> 0x18;
            }
            uVar19 = ((int)local_b8 + iVar22 + 0x20000 >> 0x12) + 0x80;
            puVar16[0x38] = (char)uVar32;
            if ((uVar19 & 0xff00) != 0) {
              uVar19 = (uint)~((int)uVar19 >> 8) >> 0x18;
            }
            puVar16[8] = (char)uVar19;
            uVar32 = (((int)local_b8 - iVar22) + 0x20000 >> 0x12) + 0x80;
            bVar35 = (uVar32 & 0xff00) != 0;
            if (bVar35) {
              uVar32 = ~((int)uVar32 >> 8);
            }
            if (bVar35) {
              uVar32 = uVar32 >> 0x18;
            }
            uVar19 = (local_d4 + iVar29 + 0x20000 >> 0x12) + 0x80;
            puVar16[0x30] = (char)uVar32;
            if ((uVar19 & 0xff00) != 0) {
              uVar19 = (uint)~((int)uVar19 >> 8) >> 0x18;
            }
            puVar16[0x10] = (char)uVar19;
            uVar32 = ((local_d4 - iVar29) + 0x20000 >> 0x12) + 0x80;
            bVar35 = (uVar32 & 0xff00) != 0;
            if (bVar35) {
              uVar32 = ~((int)uVar32 >> 8);
            }
            if (bVar35) {
              uVar32 = uVar32 >> 0x18;
            }
            uVar19 = (local_d0 + iVar31 + 0x20000 >> 0x12) + 0x80;
            puVar16[0x28] = (char)uVar32;
            if ((uVar19 & 0xff00) != 0) {
              uVar19 = (uint)~((int)uVar19 >> 8) >> 0x18;
            }
            puVar16[0x18] = (char)uVar19;
            uVar32 = ((local_d0 - iVar31) + 0x20000 >> 0x12) + 0x80;
            if ((uVar32 & 0xff00) != 0) {
              uVar32 = (uint)~((int)uVar32 >> 8) >> 0x18;
            }
            puVar16[0x20] = (char)uVar32;
          }
          puVar16 = puVar16 + 1;
          puVar13 = puVar13 + 1;
          iVar28 = iVar28 + -1;
        } while (-1 < iVar28);
        local_28 = local_28 + 0x40;
      }
      FUN_0010e334(&local_b4);
    }
    *(undefined1 *)(local_bc + 0x408) = 0;
  }
  else if ((&DAT_00007408)[param_1] != '\0') goto LAB_0010f060;
  switch(*(undefined1 *)(local_bc + 0x40a)) {
  case 0:
    iVar25 = *(int *)(local_bc + 0x420);
    iVar22 = *(int *)(local_bc + 0x488);
    ppuVar15 = (undefined **)FUN_0010e24c(*(undefined4 *)(local_bc + 0x48c));
    iVar28 = *(int *)(local_bc + 0x40c);
    if (0 < iVar28) {
      unaff_r7 = &DAT_00183074;
    }
    iVar22 = *(int *)(local_bc + 0x480) + (iVar25 - iVar22) * 8;
    local_ec = ppuVar15;
    for (; 0 < iVar28; iVar28 = iVar28 + -1) {
      (*(code *)*unaff_r7)(ppuVar15,iVar22,8);
      iVar22 = iVar22 + 0x40;
      ppuVar15 = ppuVar15 + 2;
    }
    FUN_0010e334(&local_ec);
    *(undefined1 *)(local_bc + 0x494) = 0;
    break;
  case 1:
    iVar28 = *(int *)(local_bc + 0x420);
    iVar22 = *(int *)(local_bc + 0x488);
    local_e4 = &DAT_00180558;
    local_cc = (ushort *)FUN_0010e24c(*(undefined4 *)(local_bc + 0x48c));
    iVar25 = 0;
    local_e0 = 0;
    local_d0 = *(int *)(local_bc + 0x480) + (iVar28 - iVar22) * 8;
    iVar28 = 0;
    if (-1 < *(int *)(local_bc + 0x40c) + -1) {
      local_d4 = *(int *)(local_bc + 0x40c) + -1;
      do {
        iVar22 = 8;
        pbVar17 = (byte *)(local_e0 + local_d0);
        puVar16 = (undefined1 *)((int)local_cc + iVar25 + 2);
        do {
          bVar1 = pbVar17[0x80];
          pbVar18 = pbVar17 + 0x40;
          bVar2 = *pbVar17;
          pbVar17 = pbVar17 + 1;
          bVar3 = *pbVar18;
          iVar29 = bVar2 + 0x100;
          iVar22 = iVar22 + -1;
          *puVar16 = *(undefined1 *)(*(int *)(local_e4 + (uint)bVar1 * 4) + 0x180258 + iVar29);
          puVar14 = puVar16 + -2;
          puVar16[-1] = (&DAT_00180358)
                        [(uint)bVar2 +
                         ((int)((&PTR_DAT_00180d58)[bVar1] + (int)(&PTR_DAT_00181158)[bVar3]) >>
                         0x10)];
          puVar16 = puVar16 + 4;
          *puVar14 = *(undefined1 *)(*(int *)(&DAT_00180958 + (uint)bVar3 * 4) + 0x180258 + iVar29);
        } while (iVar22 != 0);
        iVar28 = iVar28 + 1;
        local_e0 = local_e0 + 0xc0;
        iVar25 = iVar25 + 0x20;
      } while (iVar28 <= local_d4);
    }
    local_c8 = local_cc;
    FUN_0010e334(&local_c8);
    *(undefined1 *)(local_bc + 0x494) = 0;
    break;
  case 2:
    iVar22 = *(int *)(local_bc + 0x420) - *(int *)(local_bc + 0x488);
    local_c4 = &DAT_00180958;
    local_d8 = &PTR_DAT_00180d58;
    local_ec = (undefined **)&DAT_00180558;
    local_b4 = &PTR_DAT_00181158;
    local_8c = (undefined **)FUN_0010e24c(*(undefined4 *)(local_bc + 0x48c));
    local_e8 = (undefined *)0x0;
    local_a4 = 0;
    iVar28 = *(int *)(local_bc + 0x40c);
    bVar35 = -1 < iVar28 + -1;
    if (bVar35) {
      iVar28 = iVar28 + -1;
    }
    iVar25 = *(int *)(local_bc + 0x480) + iVar22 * 8;
    iVar22 = *(int *)(local_bc + 0x480) + iVar22 * 8 + 0x80;
    local_a8 = (undefined **)0x0;
    if (bVar35) {
      do {
        local_d4 = 0;
        local_d0 = 0;
        local_ac = (undefined *)0x0;
        local_90 = 0;
        do {
          iVar26 = 0;
          local_e4 = (undefined *)((int)local_ac + local_a4);
          iVar31 = 0;
          local_f4 = (undefined *)(local_d0 + (int)local_e8);
          iVar29 = 0;
          local_9c = local_d4 + (int)local_e8;
          do {
            iVar30 = local_9c + iVar29;
            bVar1 = *(byte *)(iVar30 + iVar22 + 0x40);
            iVar29 = iVar29 + 1;
            puVar34 = local_ec[bVar1];
            puVar23 = local_d8[bVar1];
            uVar32 = (uint)*(byte *)(iVar22 + iVar30);
            puVar27 = local_b4[uVar32];
            iVar30 = *(int *)(local_c4 + uVar32 * 4);
            uVar32 = (uint)*(byte *)(iVar25 + (int)local_f4 + iVar26);
            local_94 = (int)local_e4 + iVar31;
            *(undefined *)((int)local_8c + local_94 + 2) = (&DAT_00180358)[(int)(puVar34 + uVar32)];
            *(undefined *)((int)local_8c + local_94 + 1) =
                 (&DAT_00180358)[uVar32 + ((int)(puVar27 + (int)puVar23) >> 0x10)];
            iVar31 = iVar31 + 8;
            *(undefined *)((int)local_8c + local_94) = (&DAT_00180358)[uVar32 + iVar30];
            uVar32 = (uint)*(byte *)((int)local_f4 + iVar26 + iVar25 + 1);
            iVar26 = iVar26 + 2;
            *(undefined *)((int)local_8c + local_94 + 6) = (&DAT_00180358)[(int)(puVar34 + uVar32)];
            *(undefined *)((int)local_8c + local_94 + 5) =
                 (&DAT_00180358)[((int)(puVar27 + (int)puVar23) >> 0x10) + uVar32];
            *(undefined *)((int)local_8c + local_94 + 4) = (&DAT_00180358)[iVar30 + uVar32];
          } while (iVar29 < 4);
          local_90 = local_90 + 1;
          local_d4 = local_d4 + 4;
          local_d0 = local_d0 + 0x40;
          local_ac = (undefined *)((int)local_ac + 0x20);
        } while (local_90 < 2);
        local_a8 = (undefined **)((int)local_a8 + 1);
        local_e8 = (undefined *)((int)local_e8 + 0x100);
        local_a4 = local_a4 + 0x40;
        local_98 = iVar28;
      } while ((int)local_a8 <= iVar28);
    }
    FUN_0010e334(&local_8c);
    *(undefined1 *)(local_bc + 0x494) = 0;
    break;
  case 3:
    if ((*(uint *)(local_bc + 0x488) & 1) == 0) {
      local_ec = &PTR_DAT_00180d58;
      local_a8 = &PTR_DAT_00181158;
      uVar32 = *(int *)(local_bc + 0x420) - *(uint *)(local_bc + 0x488);
      local_f4 = &DAT_00180958;
      local_e8 = &DAT_00180558;
      iVar28 = FUN_0010e24c(*(undefined4 *)(local_bc + 0x48c));
      local_ac = (undefined *)iVar28;
      local_a4 = FUN_0010e24c(*(undefined4 *)(local_bc + 0x490));
      if ((int)uVar32 < 8) {
        iVar22 = *(int *)(local_bc + 0x480) + uVar32 * 8;
      }
      else {
        iVar22 = *(int *)(local_bc + 0x480) + (uVar32 & 7) * 8 + 0x40;
      }
      local_e0 = 0;
      local_e4 = (undefined *)0x0;
      local_cc = (ushort *)0x0;
      local_b8 = (undefined *)(*(int *)(local_bc + 0x480) + ((int)uVar32 >> 1) * 8 + 0x80);
      if (-1 < *(int *)(local_bc + 0x40c) + -1) {
        local_c4 = (undefined *)(*(int *)(local_bc + 0x40c) + -1);
        do {
          iVar29 = 0;
          iVar25 = 0;
          do {
            iVar20 = local_e0 + iVar25;
            uVar19 = (uint)*(byte *)(iVar22 + iVar20);
            uVar32 = (uint)*(byte *)((int)local_b8 + iVar20 + 0x40);
            iVar25 = iVar25 + 1;
            iVar30 = *(int *)(local_e8 + uVar32 * 4);
            puVar23 = local_ec[uVar32];
            puVar27 = local_a8[*(byte *)((int)local_b8 + iVar20)];
            iVar26 = *(int *)(local_f4 + (uint)*(byte *)((int)local_b8 + iVar20) * 4);
            iVar31 = (int)local_e4 + iVar29;
            *(undefined *)(iVar31 + iVar28 + 2) = (&DAT_00180358)[uVar19 + iVar30];
            iVar29 = iVar29 + 4;
            *(undefined *)(iVar31 + iVar28 + 1) =
                 (&DAT_00180358)[uVar19 + ((int)(puVar27 + (int)puVar23) >> 0x10)];
            *(undefined *)(iVar28 + iVar31) = (&DAT_00180358)[uVar19 + iVar26];
            uVar32 = (uint)*(byte *)(iVar20 + iVar22 + 8);
            *(undefined *)(iVar31 + local_a4 + 2) = (&DAT_00180358)[iVar30 + uVar32];
            *(undefined *)(iVar31 + local_a4 + 1) =
                 (&DAT_00180358)[((int)(puVar27 + (int)puVar23) >> 0x10) + uVar32];
            *(undefined *)(local_a4 + iVar31) = (&DAT_00180358)[iVar26 + uVar32];
          } while (iVar25 < 8);
          local_cc = (ushort *)((int)local_cc + 1);
          local_e0 = local_e0 + 0x100;
          local_e4 = (undefined *)((int)local_e4 + 0x20);
        } while ((int)local_cc <= (int)local_c4);
      }
      FUN_0010e334(&local_ac);
      FUN_0010e334(&local_a4);
      *(undefined1 *)(local_bc + 0x494) = 0;
      break;
    }
    goto LAB_0010fbb4;
  case 4:
    if ((*(uint *)(local_bc + 0x488) & 1) == 0) {
      local_c0 = &PTR_DAT_00180d58;
      uVar32 = *(int *)(local_bc + 0x420) - *(uint *)(local_bc + 0x488);
      local_b8 = &DAT_00180958;
      local_8c = &PTR_DAT_00181158;
      local_ac = &DAT_00180558;
      local_cc = (ushort *)FUN_0010e24c(*(undefined4 *)(local_bc + 0x48c));
      local_a0 = (int)local_cc;
      local_d0 = FUN_0010e24c(*(undefined4 *)(local_bc + 0x490));
      if ((int)uVar32 < 8) {
        iVar28 = *(int *)(local_bc + 0x480) + uVar32 * 8;
      }
      else {
        iVar28 = *(int *)(local_bc + 0x480) + (uVar32 & 7) * 8 + 0x80;
      }
      local_c4 = (undefined *)0x0;
      local_a4 = 0;
      local_d8 = (undefined **)0x0;
      local_d4 = *(int *)(local_bc + 0x480) + ((int)uVar32 >> 1) * 8 + 0x100;
      if (-1 < *(int *)(local_bc + 0x40c) + -1) {
        local_90 = *(int *)(local_bc + 0x40c) + -1;
        do {
          local_b0 = 0;
          local_dc = 0;
          local_e0 = 0;
          local_88 = 0;
          do {
            iVar29 = 0;
            local_98 = local_e0 + (int)local_d8;
            iVar25 = 0;
            local_94 = (int)local_c4 + local_dc;
            iVar22 = 0;
            local_9c = local_b0 + (int)local_c4;
            do {
              iVar31 = local_9c + iVar22;
              iVar22 = iVar22 + 1;
              uVar32 = (uint)*(byte *)(local_d4 + iVar31 + 0x40);
              iVar21 = *(int *)(local_ac + uVar32 * 4);
              uVar19 = (uint)*(byte *)(local_d4 + iVar31);
              iVar20 = *(int *)(local_b8 + uVar19 * 4);
              iVar31 = (int)(local_8c[uVar19] + (int)local_c0[uVar32]) >> 0x10;
              uVar32 = (uint)*(byte *)(iVar28 + local_94 + iVar29);
              iVar33 = local_94 + iVar29 + iVar28;
              iVar24 = local_98 + iVar25;
              iVar30 = (int)local_cc + iVar24;
              *(undefined *)(iVar30 + 2) = (&DAT_00180358)[uVar32 + iVar21];
              *(undefined *)(iVar30 + 1) = (&DAT_00180358)[uVar32 + iVar31];
              *(undefined *)((int)local_cc + iVar24) = (&DAT_00180358)[uVar32 + iVar20];
              uVar32 = (uint)*(byte *)(iVar33 + 1);
              iVar26 = local_d0 + iVar24;
              *(undefined *)(iVar30 + 6) = (&DAT_00180358)[uVar32 + iVar21];
              *(undefined *)(iVar30 + 5) = (&DAT_00180358)[uVar32 + iVar31];
              *(undefined *)(iVar30 + 4) = (&DAT_00180358)[uVar32 + iVar20];
              uVar32 = (uint)*(byte *)(iVar33 + 8);
              *(undefined *)(iVar26 + 2) = (&DAT_00180358)[uVar32 + iVar21];
              *(undefined *)(iVar26 + 1) = (&DAT_00180358)[uVar32 + iVar31];
              *(undefined *)(local_d0 + iVar24) = (&DAT_00180358)[uVar32 + iVar20];
              uVar32 = (uint)*(byte *)(iVar33 + 9);
              iVar25 = iVar25 + 8;
              *(undefined *)(iVar26 + 6) = (&DAT_00180358)[iVar21 + uVar32];
              iVar29 = iVar29 + 2;
              *(undefined *)(iVar26 + 5) = (&DAT_00180358)[iVar31 + uVar32];
              *(undefined *)(iVar26 + 4) = (&DAT_00180358)[uVar32 + iVar20];
            } while (iVar22 < 4);
            local_88 = local_88 + 1;
            local_b0 = local_b0 + 4;
            local_dc = local_dc + 0x40;
            local_e0 = local_e0 + 0x20;
          } while (local_88 < 2);
          local_a4 = local_a4 + 1;
          local_c4 = (undefined *)((int)local_c4 + 0x180);
          local_d8 = (undefined **)((int)local_d8 + 0x40);
        } while (local_a4 <= local_90);
      }
      local_84 = local_d0;
      FUN_0010e334(&local_a0);
      FUN_0010e334(&local_84);
      *(undefined1 *)(local_bc + 0x494) = 0;
      break;
    }
LAB_0010fbb4:
    *(undefined1 *)(local_bc + 0x494) = 1;
  }
  *(int *)(local_bc + 0x488) = *(int *)(local_bc + 0x488) + -1;
  *(int *)(local_bc + 0x484) = *(int *)(local_bc + 0x484) + -1;
  return 0;
}

// ==== FUN_0010fff0 @ 0010fff0, 2324 bytes
// called by: FUN_0011b2e8
/* WARNING: Type propagation algorithm not settling */

int FUN_0010fff0(undefined4 param_1)

{
  char cVar1;
  byte bVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  undefined4 *puVar6;
  int iVar7;
  undefined4 uVar8;
  undefined1 *puVar9;
  byte *pbVar10;
  int iVar11;
  int *piVar12;
  undefined4 *puVar13;
  int iVar14;
  uint uVar15;
  code *pcVar16;
  int iVar17;
  uint uVar18;
  bool bVar19;
  bool bVar20;
  int local_88 [5];
  int local_74;
  undefined1 *local_70;
  undefined *local_6c;
  undefined4 *local_64;
  int local_60;
  int local_5c;
  undefined4 auStack_58 [5];
  byte local_44 [4];
  int local_40;
  int local_3c;
  byte *local_38;
  int local_2c;
  
  iVar7 = FUN_0011091c();
  if (iVar7 != 0) {
    return iVar7;
  }
  iVar11 = 0;
  iVar7 = FUN_0010e24c(param_1);
  cVar1 = (&DAT_0000731c)[iVar7];
  local_88[2] = iVar7;
  if (cVar1 == '\x01') {
    (&DAT_0000740a)[iVar7] = 0;
    *(undefined4 *)(&DAT_0000741c + iVar7) = 8;
    *(undefined4 *)(&DAT_00007414 + iVar7) = 1;
    *(undefined4 *)(&DAT_00007420 + iVar7) = 8;
LAB_00110138:
    uVar3 = *(ushort *)(iVar7 + 0x10);
    local_60 = divide((uint)uVar3 + *(int *)(&DAT_0000741c + iVar7) + -1);
    *(int *)(&DAT_0000740c + iVar7) = local_60;
    uVar4 = *(ushort *)(iVar7 + 0x12);
    uVar8 = divide((uint)uVar4 + *(int *)(&DAT_00007420 + iVar7) + -1);
    *(undefined4 *)(&DAT_00007488 + iVar7) = 0;
    *(undefined4 *)(&DAT_00007410 + iVar7) = uVar8;
    *(uint *)(&DAT_00007484 + iVar7) = (uint)uVar4;
    cVar1 = (&DAT_0000740a)[iVar7];
    if (cVar1 == '\0') {
      *(undefined4 *)(&DAT_00007424 + iVar7) = 1;
    }
    else {
      *(undefined4 *)(&DAT_00007424 + iVar7) = 4;
    }
    local_88[1] = (uVar3 + 0xf & 0xfffefff0) * *(int *)(&DAT_00007424 + iVar7);
    *(int *)(&DAT_00007428 + iVar7) = local_88[1];
    iVar14 = local_60 * *(int *)(&DAT_00007414 + iVar7);
    *(int *)(&DAT_00007418 + iVar7) = iVar14;
    FUN_0010e334(local_88 + 2);
    local_60 = FUN_0010dfec(iVar14 * 0x80);
    local_74 = FUN_0010dfec(iVar14 * 0x40);
    iVar7 = FUN_0010dfec(iVar14);
    iVar14 = FUN_0010dfec(local_88[1]);
    if (cVar1 == '\x03' || cVar1 == '\x04') {
      iVar17 = FUN_0010dfec(local_88[1]);
    }
    else {
      iVar17 = 0;
    }
    iVar5 = 0;
    if (local_60 != 0) {
      iVar5 = local_74;
    }
    if (((local_60 != 0 && iVar5 != 0) && (iVar7 != 0 && iVar14 != 0)) &&
       ((iVar17 != 0 || (cVar1 != '\x03' && cVar1 != '\x04')))) {
      local_88[2] = FUN_0010e24c(param_1);
      *(int *)(&DAT_00007474 + local_88[2]) = local_60;
      *(int *)(&DAT_00007478 + local_88[2]) = iVar7;
      *(int *)(&DAT_0000747c + local_88[2]) = local_74;
      *(int *)(&DAT_0000748c + local_88[2]) = iVar14;
      *(int *)(&DAT_00007490 + local_88[2]) = iVar17;
      local_70 = (undefined1 *)FUN_0010e24c(iVar7);
      if (0 < (int)*(uint *)(&DAT_00007418 + local_88[2])) {
        puVar9 = local_70 + -1;
        if ((*(uint *)(&DAT_00007418 + local_88[2]) & 1) != 0) {
          *local_70 = 0x40;
          puVar9 = local_70;
        }
        for (iVar7 = *(int *)(&DAT_00007418 + local_88[2]) >> 1; iVar7 != 0; iVar7 = iVar7 + -1) {
          puVar9[1] = 0x40;
          puVar9 = puVar9 + 2;
          *puVar9 = 0x40;
        }
      }
      FUN_0010e334(&local_70);
    }
    else {
      FUN_0010e0a8(local_60);
      FUN_0010e0a8(local_74);
      FUN_0010e0a8(iVar7);
      FUN_0010e0a8(iVar14);
      FUN_0010e0a8(iVar17);
      iVar11 = 1;
    }
  }
  else {
    bVar19 = cVar1 == '\x03';
    if (bVar19) {
      cVar1 = (&DAT_0000731e)[iVar7];
    }
    if (bVar19 && cVar1 == '\x01') {
      cVar1 = (&DAT_00007322)[iVar7];
      bVar19 = cVar1 == '\x01';
      if (bVar19) {
        cVar1 = (&DAT_0000731f)[iVar7];
      }
      bVar20 = bVar19 && cVar1 == '\x01';
      if (bVar19 && cVar1 == '\x01') {
        bVar20 = (&DAT_00007323)[iVar7] == '\x01';
      }
      if (bVar20) {
        cVar1 = (&DAT_0000731d)[iVar7];
        if (cVar1 == '\x01') {
          if ((&DAT_00007321)[iVar7] == '\x01') {
            (&DAT_0000740a)[iVar7] = 1;
            *(undefined4 *)(&DAT_0000741c + iVar7) = 8;
            *(undefined4 *)(&DAT_00007414 + iVar7) = 3;
LAB_0011010c:
            *(undefined4 *)(&DAT_00007420 + iVar7) = 8;
            goto LAB_00110138;
          }
LAB_001100bc:
          if ((&DAT_00007321)[iVar7] == '\x02') {
            (&DAT_0000740a)[iVar7] = 3;
            *(undefined4 *)(&DAT_0000741c + iVar7) = 8;
            *(undefined4 *)(&DAT_00007414 + iVar7) = 4;
            goto LAB_00110134;
          }
        }
        else {
          if (cVar1 == '\x02') {
            if ((&DAT_00007321)[iVar7] == '\x01') {
              (&DAT_0000740a)[iVar7] = 2;
              *(undefined4 *)(&DAT_0000741c + iVar7) = 0x10;
              *(undefined4 *)(&DAT_00007414 + iVar7) = 4;
              goto LAB_0011010c;
            }
          }
          else {
            if (cVar1 == '\x01') goto LAB_001100bc;
            if (cVar1 != '\x02') goto LAB_001100f4;
          }
          if ((&DAT_00007321)[iVar7] == '\x02') {
            (&DAT_0000740a)[iVar7] = 4;
            *(undefined4 *)(&DAT_00007414 + iVar7) = 6;
            *(undefined4 *)(&DAT_0000741c + iVar7) = 0x10;
LAB_00110134:
            *(undefined4 *)(&DAT_00007420 + iVar7) = 0x10;
            goto LAB_00110138;
          }
        }
      }
    }
LAB_001100f4:
    iVar11 = 1;
  }
  if (local_88[2] != 0) {
    FUN_0010e334(local_88 + 2);
  }
  if (iVar11 == 0) {
    local_5c = FUN_0010e24c(param_1);
    if ((&DAT_00007409)[local_5c] != '\0') {
      FUN_0010e334(&local_5c);
      local_40 = 0;
      local_64 = (undefined4 *)FUN_0010e24c(param_1);
      uVar15 = (uint)(byte)(&DAT_0000731c)[(int)local_64];
      FUN_0010e334(&local_64);
      iVar7 = 1;
      if (uVar15 != 4) {
        iVar7 = 0;
        if (uVar15 != 0) {
          do {
            local_88[1] = FUN_0010e24c(param_1);
            iVar11 = local_88[1] + iVar7 * 0x10;
            piVar12 = (int *)(&DAT_00007378 + iVar11);
            if (piVar12 == (int *)0x0) {
              return 1;
            }
            *piVar12 = (uint)(byte)(&DAT_0000731d)[local_88[1] + iVar7] *
                       *(int *)(&DAT_0000740c + local_88[1]);
            *(uint *)(&DAT_0000737c + iVar11) =
                 (uint)(byte)(&DAT_00007321)[local_88[1] + iVar7] *
                 *(int *)(&DAT_00007410 + local_88[1]);
            *(undefined4 *)(&DAT_00007380 + iVar11) = 2;
            FUN_0010e334(local_88 + 1);
            iVar11 = FUN_0010dfec(*(int *)(&DAT_0000737c + iVar11) *
                                  *(int *)(&DAT_00007380 + iVar11) * *piVar12);
            if (iVar11 == 0) {
              return 1;
            }
            local_88[1] = FUN_0010e24c(param_1);
            *(int *)(&DAT_00007384 + local_88[1] + iVar7 * 0x10) = iVar11;
            FUN_0010e334(local_88 + 1);
            local_88[0] = FUN_0010e24c(param_1);
            iVar11 = local_88[0] + iVar7 * 0x10;
            piVar12 = (int *)(&DAT_000073b8 + iVar11);
            if (piVar12 == (int *)0x0) {
              return 1;
            }
            *piVar12 = (uint)(byte)(&DAT_0000731d)[local_88[0] + iVar7] *
                       *(int *)(&DAT_0000740c + local_88[0]);
            *(uint *)(&DAT_000073bc + iVar11) =
                 (uint)(byte)(&DAT_00007321)[local_88[0] + iVar7] *
                 *(int *)(&DAT_00007410 + local_88[0]);
            *(undefined4 *)(&DAT_000073c0 + iVar11) = 0x80;
            FUN_0010e334(local_88);
            iVar11 = FUN_0010dfec(*(int *)(&DAT_000073bc + iVar11) *
                                  *piVar12 * *(int *)(&DAT_000073c0 + iVar11));
            if (iVar11 == 0) {
              return 1;
            }
            local_88[0] = FUN_0010e24c(param_1);
            *(int *)(&DAT_000073c4 + local_88[0] + iVar7 * 0x10) = iVar11;
            FUN_0010e334(local_88);
            iVar7 = iVar7 + 1;
          } while (iVar7 < (int)uVar15);
        }
        local_64 = (undefined4 *)FUN_0010e24c(param_1);
        while (iVar7 = FUN_00123c60(local_64), puVar6 = local_64, iVar7 == 0) {
          bVar2 = *(byte *)(local_64 + 0x1cdd);
          bVar19 = *(byte *)((int)local_64 + 0x7377) != 0;
          if (*(byte *)((int)local_64 + 0x7375) < bVar2) {
LAB_001107a4:
            local_40 = 1;
            goto LAB_0011052c;
          }
          if (bVar2 != 0) {
            if (*(char *)((int)local_64 + 0x732d) == '\x01') goto LAB_00110598;
            goto LAB_001107a4;
          }
          if (*(byte *)((int)local_64 + 0x7375) != 0) goto LAB_001107a4;
LAB_00110598:
          if ((bVar19) &&
             ((uint)*(byte *)((int)local_64 + 0x7376) != *(byte *)((int)local_64 + 0x7377) - 1))
          goto LAB_001107a4;
          if (bVar2 != 0) {
            if (bVar19) {
              pcVar16 = FUN_0011a7e8;
            }
            else {
              pcVar16 = FUN_0011a5fc;
            }
          }
          else if (bVar19) {
            pcVar16 = FUN_0011ac14;
          }
          else {
            pcVar16 = FUN_0011aabc;
          }
          (*DAT_00183070)(local_88,0,0x10);
          local_3c = 0;
          if (0 < (int)puVar6[0x1d11]) {
            local_38 = (byte *)((int)puVar6 + 0x732d);
            local_6c = &DAT_00007321 + (int)puVar6;
            do {
              puVar13 = &DAT_00183070;
              (*DAT_00183070)(auStack_58,0);
              if (0 < (int)puVar6[0x1d10]) {
                puVar13 = auStack_58;
              }
              local_2c = 0;
              if (0 < (int)puVar6[0x1d10]) {
LAB_0011063c:
                uVar18 = 0;
                uVar15 = 0;
                if ((puVar6[0x1d0d] != 0) && (puVar6[0x1d0e] == 0)) {
                  iVar7 = 2000;
                  do {
                    iVar11 = (*(code *)*puVar6)(puVar6,local_44);
                    if (iVar11 != 0) break;
                    if (local_44[0] == 0xff) {
                      if (iVar7 != 0) {
                        if (0 < iVar7) goto LAB_001106b0;
                        goto LAB_001106e8;
                      }
                      break;
                    }
                    iVar7 = iVar7 + -1;
                  } while (iVar7 != 0);
                }
                goto LAB_00110764;
              }
LAB_00110838:
              bVar2 = *(byte *)((int)puVar6 + 0x732d);
              if (bVar2 == 1) {
                local_88[*(byte *)((int)puVar6 + 0x732e)] =
                     local_88[*(byte *)((int)puVar6 + 0x732e)] + 1;
              }
              else if (bVar2 != 0) {
                pbVar10 = local_38;
                if ((bVar2 & 1) != 0) {
                  pbVar10 = local_38 + 1;
                  bVar2 = *pbVar10;
                  local_88[bVar2] = (uint)(byte)local_6c[bVar2] + local_88[bVar2];
                }
                for (uVar15 = (uint)(*(byte *)((int)puVar6 + 0x732d) >> 1); uVar15 != 0;
                    uVar15 = uVar15 - 1) {
                  bVar2 = pbVar10[1];
                  local_88[bVar2] = (uint)(byte)local_6c[bVar2] + local_88[bVar2];
                  pbVar10 = pbVar10 + 2;
                  bVar2 = *pbVar10;
                  local_88[bVar2] = (uint)(byte)local_6c[bVar2] + local_88[bVar2];
                }
              }
              local_3c = local_3c + 1;
            } while (local_3c < (int)puVar6[0x1d11]);
          }
        }
        uVar15 = (uint)*(byte *)(local_64 + 0x1cc7);
        *(byte *)((int)local_64 + 0x732d) = *(byte *)(local_64 + 0x1cc7);
        if (uVar15 != 0) {
          iVar7 = 0;
          do {
            uVar15 = uVar15 - 1;
            (&DAT_0000732e)[(int)local_64 + iVar7] = (char)iVar7;
            iVar7 = iVar7 + 1;
          } while (uVar15 != 0);
        }
        FUN_00119fd4();
LAB_0011052c:
        FUN_0010e334(&local_64);
        iVar7 = local_40;
      }
      return iVar7;
    }
    iVar11 = FUN_00123c60();
    FUN_0010e334(&local_5c);
  }
  return iVar11;
  while ((local_44[0] == 0xff && (iVar7 = iVar7 + -1, 0 < iVar7))) {
LAB_001106b0:
    iVar11 = (*(code *)*puVar6)(puVar6,local_44);
    if (iVar11 != 0) goto LAB_00110764;
  }
  if (iVar7 != 0) {
LAB_001106e8:
    if (puVar6[0x1d0f] + 0xd0 == (uint)local_44[0]) {
      (*DAT_00183070)(puVar6 + 0x1cd9,0,(uint)*(byte *)(puVar6 + 0x1cc7) << 2);
      puVar6[0x1d0c] = 0;
      puVar6[0x1d0e] = puVar6[0x1d0d];
      puVar6[0x1d0f] = *(byte *)(puVar6 + 0x1d0f) + 1 & 7;
      puVar6[10] = 0x10;
      FUN_00122c54(puVar6,0x10);
      FUN_00122c54(puVar6,0x10);
    }
  }
LAB_00110764:
  iVar7 = 0;
  if (0 < (int)puVar6[0x1d12]) {
    do {
      iVar14 = puVar6[iVar7 + 0x1d13];
      iVar11 = (*pcVar16)(puVar6,iVar14,puVar13[iVar14] + uVar15);
      if (iVar11 != 0) goto LAB_001107a4;
      if (*(char *)((int)puVar6 + 0x732d) == '\x01') {
        iVar11 = puVar13[iVar14] + 1;
LAB_001107fc:
        puVar13[iVar14] = iVar11;
      }
      else {
        uVar15 = uVar15 + 1;
        if (uVar15 == (byte)(&DAT_0000731d)[(int)puVar6 + iVar14]) {
          uVar18 = uVar18 + 1;
          uVar15 = 0;
          if (uVar18 == (byte)(&DAT_00007321)[(int)puVar6 + iVar14]) {
            uVar18 = 0;
            iVar11 = (uint)(byte)(&DAT_0000731d)[(int)puVar6 + iVar14] + puVar13[iVar14];
            goto LAB_001107fc;
          }
        }
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 < (int)puVar6[0x1d12]);
  }
  local_2c = local_2c + 1;
  puVar6[0x1d0e] = puVar6[0x1d0e] + -1;
  if ((int)puVar6[0x1d10] <= local_2c) goto LAB_00110838;
  goto LAB_0011063c;
}

