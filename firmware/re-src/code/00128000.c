// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x00128000-0x0012bfff.
// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.
// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.

// ==== idle_screen_proc @ 0012808c, 1004 bytes
// the idle screen: picture + coolant temperature (message 0x15 draws it)
/* the idle screen: picture + coolant temperature (message 0x15 draws it) */

undefined4 idle_screen_proc(undefined4 param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  bool bVar6;
  bool bVar7;
  undefined4 local_f8 [50];
  int local_30;
  int local_2c;
  
  if (param_2 == 9) {
    FUN_0011147c(0);
    FUN_0010e41c();
  }
  else if (param_2 < 10) {
    if (param_2 == 3) {
      FUN_00112f40(2);
      FUN_0012a4a8(0xffffffff);
      text_set_colour();
      FUN_00113ab4();
      local_f8[0] = 0;
      FUN_0012bd70(param_1,0,10,0xffff);
      FUN_0012c4ac(lcd_boot_restart);
      if (DAT_00182b0c != 0) {
        mem_free();
      }
      DAT_00182b0c = 0;
      DAT_00182b08 = file_size(&DAT_00182b94);
      iVar2 = DAT_00182b0c;
      if (DAT_00182b08 != 0) {
        iVar2 = mem_alloc();
        iVar3 = file_read(&DAT_00182b94,0,iVar2,DAT_00182b08);
        if (iVar3 < 1) {
          mem_free(iVar2);
          DAT_00182b08 = 0;
          iVar2 = DAT_00182b0c;
        }
      }
      DAT_00182b0c = iVar2;
      DAT_00182b00 = 1;
      DAT_00182bc4 = 0;
    }
    else if (param_2 == 6) {
      if (DAT_00182b0c != 0) {
        mem_free();
      }
      DAT_00182b0c = 0;
      DAT_00182b08 = 0;
    }
  }
  else {
    if (param_2 == 0x15) {
      if (DAT_00182bbc != '\0') {
        return 0;
      }
      if (g_config.idle_state == 1) {
        return 0;
      }
      iVar2 = DAT_00182b0c;
      if (DAT_00182bc8 == 0 && DAT_00182bc0 == 0) {
        if ((g_config.idle_state == 2) && (DAT_00182bc4 = DAT_00182bc4 + 1, 999 < DAT_00182bc4)) {
          FUN_0010d3f8(0);
          DAT_00182af8 = 0;
          return 0;
        }
        if (g_config.temp_scale == 0) {
          iVar2 = divide(g_coolant,100);
        }
        else {
          iVar2 = divide(g_coolant * 0x12,1000);
          iVar2 = iVar2 + 0x20;
        }
        bVar6 = DAT_00182b00 == 0;
        uVar1 = DAT_00182b00;
        if (bVar6) {
          uVar1 = DAT_00182afc;
        }
        bVar7 = bVar6 && g_config.idle_hide == uVar1;
        if (bVar6 && g_config.idle_hide == uVar1) {
          bVar7 = DAT_00182b04 == iVar2;
        }
        if (bVar7) {
          return 0;
        }
        DAT_00182afc = g_config.idle_hide;
        DAT_00182b00 = 0;
        DAT_00182b04 = iVar2;
        FUN_0011147c(g_screen);
        if (((g_config.idle_hide & 1) == 0) && (DAT_00182b0c != 0)) {
          local_30 = 0;
          local_2c = 0;
          jpeg_size(DAT_00182b0c,DAT_00182b08,&local_30);
          jpeg_draw(DAT_00182b0c,DAT_00182b08,(0x1e0 - local_30) / 2,(0x1e0 - local_2c) / 2);
          FUN_00105eb8();
        }
        if ((g_config.idle_hide & 2) == 0) {
          thunk_FUN_001169f0(0);
          FUN_00112f40(2);
          text_set_colour(~g_config.text_colour & 0xff000000 |
                          (g_config.text_colour & 0xffffff) >> 0x10 | g_config.text_colour & 0xff00
                          | (g_config.text_colour & 0xff) << 0x10);
          sprintf(local_f8,"%d",iVar2);
          font_select(DAT_00182bd4);
          iVar2 = text_width(local_f8);
          FUN_0010ebec();
          if (g_config.temp_scale == 0) {
            pcVar5 = "C ";
          }
          else {
            pcVar5 = "F ";
          }
          font_select(g_idle_fonts);
          FUN_0010ebec();
          iVar3 = text_width(pcVar5);
          iVar4 = text_width(&DAT_001284b4);
          iVar3 = (((0x1e0 - iVar2) - iVar3) - iVar4) / 2;
          font_select(DAT_00182bd4);
          text_draw(local_f8,g_config.idle_text_x + iVar3,g_config.idle_text_y);
          text_draw(pcVar5,g_config.idle_text_x + iVar3 + iVar4 + iVar2,g_config.idle_text_y);
          font_select(g_idle_fonts);
          text_draw(&DAT_001284b4,g_config.idle_text_x + iVar3 + iVar2,g_config.idle_text_y + 3);
        }
        FUN_00112660();
        FUN_00110c20(g_screen,0);
        FUN_001127f4();
        FUN_0010e82c(0);
        return 0;
      }
    }
    else {
      if (param_2 != 0x20) {
        return 0;
      }
      if (DAT_00182b0c != 0) {
        mem_free();
      }
      DAT_00182b0c = 0;
      DAT_00182b08 = file_size(&DAT_00182b94);
      iVar2 = DAT_00182b0c;
      if (DAT_00182b08 != 0) {
        iVar2 = mem_alloc();
        iVar3 = file_read(&DAT_00182b94,0,iVar2,DAT_00182b08);
        if (iVar3 < 1) {
          mem_free(iVar2);
          DAT_00182b08 = 0;
          iVar2 = DAT_00182b0c;
        }
      }
    }
    DAT_00182b0c = iVar2;
    DAT_00182b00 = 1;
  }
  return 0;
}

// ==== FUN_001284b8 @ 001284b8, 316 bytes
undefined4 FUN_001284b8(undefined4 param_1,int param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  int local_14;
  
  uVar1 = DAT_00383160;
  local_14 = param_4;
  if (param_2 == 3) {
    FUN_00128900(&DAT_00383158);
    DAT_00182434 = -1;
    FUN_001049b4(FUN_00126f74);
    FUN_00104930();
    FUN_001006c4(0x16,5);
    FUN_0012bd70(param_1,0,1,0xffff,0);
    FUN_0012bd70(param_1,1,0x32,0xffff,0);
    FUN_00128900(&DAT_00383158);
  }
  else if ((param_2 != 6) && (param_2 == 0x15)) {
    if (param_3 == 0) {
      if (DAT_00383177 != '\0') {
        DAT_00383177 = '\0';
        iVar2 = FUN_00127ffc(&DAT_00182438,0xa0,8,DAT_00383160,&local_14);
        if (iVar2 == 0) {
          iVar2 = local_14 * 8;
          g_coolant = FUN_00127fd0(*(undefined4 *)(&DAT_00182438 + local_14 * 8),
                                   *(undefined4 *)(&DAT_00182440 + iVar2),uVar1,
                                   *(undefined4 *)(&DAT_0018243c + iVar2),
                                   *(undefined4 *)(&DAT_00182444 + iVar2));
        }
        else {
          g_coolant = *(int *)(&DAT_0018243c + local_14 * 8);
        }
        if (g_coolant != DAT_00182434) {
          DAT_00182434 = g_coolant;
        }
      }
    }
    else {
      FUN_001049c0(4);
    }
  }
  return 0;
}

// ==== screen_create_proc @ 00128608, 168 bytes
// ? message 3 creates the 480x480 surface (stored at g_screen)
/* ? message 3 creates the 480x480 surface (stored at g_screen) */

undefined4 screen_create_proc(undefined4 param_1,int param_2)

{
  undefined4 local_1c [5];
  
  if (param_2 == 3) {
    DAT_00182af8 = 0;
    FUN_0010d3f8();
    local_1c[0] = 1;
    FUN_001083d0(local_1c,&DAT_00383180);
    DAT_00182938 = 0x80383180;
    FUN_0010ece4();
    display_set_rotation(3);
    FUN_00118300(1);
    FUN_00111180(1);
    FUN_00118638(4);
    FUN_0010e41c();
    g_screen = FUN_00110e38(0,0,0x1e0,0x1e0,1,&DAT_00181df8,&PTR_LAB_00180238);
  }
  return 0;
}

// ==== FUN_001286c8 @ 001286c8, 84 bytes
undefined4 FUN_001286c8(undefined4 param_1,int param_2)

{
  int iVar1;
  
  if (param_2 == 3) {
    iVar1 = FUN_001014c8();
    FUN_0010250c(1,iVar1 * 1000);
    FUN_00102558(1,1000);
    FUN_00102a28(1,1,FUN_00117354);
    FUN_001006c4(0xf,6);
  }
  return 0;
}

// ==== FUN_00128720 @ 00128720, 76 bytes
undefined4 FUN_00128720(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_18;
  
  local_18 = param_4;
  for (; 0 < param_2; param_2 = param_2 + -0x10000) {
    FUN_0012ac38(param_1,1);
    param_1 = param_1 + 0x10000;
    local_18 = 0;
    FUN_0012ae24(param_1,4,&local_18);
  }
  return 1;
}

// ==== FUN_0012876c @ 0012876c, 88 bytes
undefined4 FUN_0012876c(int param_1,int param_2,uint param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 local_18;
  
  uVar1 = param_3 & 3;
  iVar2 = param_3 - uVar1;
  local_18 = param_4;
  FUN_0012ae24(param_1,iVar2,param_2);
  if (uVar1 != 0) {
    local_18 = 0;
    FUN_0012ae24(param_1 + iVar2,4,&local_18);
    FUN_0010c9b8(param_2 + iVar2,&local_18,uVar1);
  }
  return 1;
}

// ==== FUN_001287c8 @ 001287c8, 20 bytes
undefined4 FUN_001287c8(undefined4 param_1,undefined4 param_2,uint param_3)

{
  FUN_0012b218(param_1,param_2,param_3 >> 0x10);
  return 1;
}

// ==== FUN_001287dc @ 001287dc, 292 bytes
undefined1 FUN_001287dc(int *param_1,int param_2,int param_3)

{
  char cVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  
  cVar1 = *(char *)((int)param_1 + 0x1d);
  cVar2 = '\x01';
  if (cVar1 == '\0') {
LAB_0012883c:
    param_1[3] = param_2;
    param_1[2] = param_2;
    *(undefined1 *)((int)param_1 + 0x1e) = 0;
  }
  else {
    uVar4 = param_2 - param_1[2];
    if ((int)uVar4 < 0) {
      if (*param_1 < (int)(~uVar4 + 1)) {
        cVar2 = '\x02';
      }
    }
    else if (param_1[1] < (int)uVar4) {
      cVar2 = '\x03';
    }
    if (cVar1 == cVar2) {
      if ((int)(uint)*(byte *)(param_1 + 7) < param_3) {
        *(byte *)(param_1 + 7) = *(byte *)(param_1 + 7) + 1;
        param_1[4] = param_1[4] + param_2;
        if (param_1[5] < param_2) {
          param_1[5] = param_2;
        }
        if (param_2 < param_1[6]) {
          param_1[6] = param_2;
        }
      }
      if (1 < *(byte *)(param_1 + 7)) {
        iVar3 = divide((param_1[4] - param_1[5]) - param_1[6],*(byte *)(param_1 + 7) - 1);
        param_1[3] = iVar3;
      }
      goto LAB_001288c8;
    }
    if (cVar1 == '\0') goto LAB_0012883c;
  }
  *(undefined1 *)(param_1 + 7) = 0;
  *(char *)((int)param_1 + 0x1d) = cVar2;
  param_1[4] = param_2;
  param_1[5] = param_2;
  param_1[6] = param_2;
LAB_001288c8:
  if (param_3 <= (int)(uint)*(byte *)(param_1 + 7)) {
    param_1[2] = param_1[3];
    *(undefined1 *)((int)param_1 + 0x1d) = 1;
    *(byte *)((int)param_1 + 0x1e) = *(char *)((int)param_1 + 0x1e) << 1 | 1;
  }
  *(undefined1 *)((int)param_1 + 0x1f) = 1;
  return *(undefined1 *)((int)param_1 + 0x1e);
}

// ==== FUN_00128900 @ 00128900, 20 bytes
// called by: FUN_001284b8
void FUN_00128900(int param_1)

{
  *(undefined1 *)(param_1 + 0x1e) = 0;
  *(undefined1 *)(param_1 + 0x1d) = 0;
  *(undefined1 *)(param_1 + 0x1f) = 0;
  return;
}

// ==== FUN_00128914 @ 00128914, 60 bytes
undefined4 FUN_00128914(undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  
  if (param_2 == 3) {
    FUN_0012acf8();
    puVar1 = (undefined4 *)FUN_0012e888();
    FUN_0010c9b8(&DAT_00182bb4,puVar1,8);
    dbg_printf("----->uid:%08x%08x\n",*puVar1,puVar1[1]);
  }
  return 0;
}

// ==== FUN_00128968 @ 00128968, 148 bytes
// called by: FUN_00128a24, file_read, file_size
uint * FUN_00128968(int param_1,int param_2,uint *param_3)

{
  int iVar1;
  
  if (((param_2 != -1) && (iVar1 = (**(code **)(param_1 + 4))(param_2,param_3,0x14), iVar1 != 0)) &&
     (param_3[2] + param_3[1] + param_3[4] + *(int *)(*(int *)(param_1 + 0x18) + 8) == 0)) {
    if ((0x13 < *param_3) &&
       (((param_3[1] + 3 & 0xfffffffc) + *(int *)(param_1 + 0xc) + 0x13 &
        ~(*(int *)(param_1 + 0xc) - 1U)) == *param_3)) {
      return param_3;
    }
  }
  return (uint *)0x0;
}

// ==== file_size @ 001289fc, 40 bytes
// called by: lcd_boot_proc, idle_screen_proc, usb_command
// ? file_size(path): idle_screen_proc sizes the idle picture's buffer with it
/* ? file_size(path): idle_screen_proc sizes the idle picture's buffer with it */

undefined4 file_size(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 auStack_18 [4];
  undefined4 local_14;
  
  iVar1 = FUN_00128968(param_1,**(undefined4 **)(param_1 + 0x18),auStack_18);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = local_14;
  }
  return uVar2;
}

// ==== FUN_00128a24 @ 00128a24, 176 bytes
// called by: config_load
uint * FUN_00128a24(int param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint uVar6;
  uint *puVar7;
  uint local_38 [3];
  int local_2c;
  
  puVar7 = *(uint **)(param_1 + 0x18);
  puVar5 = puVar7 + 1;
  uVar2 = *(uint *)(param_1 + 0x10);
  uVar3 = 0xffffffff;
  uVar6 = 0;
  while( true ) {
    while( true ) {
      uVar4 = uVar2;
      uVar2 = *(uint *)(param_1 + 0x14);
      if (uVar2 <= uVar4) {
        *puVar7 = uVar3;
        if (uVar3 == 0xffffffff) {
          uVar3 = *(uint *)(param_1 + 0x10);
        }
        else {
          uVar3 = uVar3 + uVar6;
        }
        *puVar5 = uVar3;
        return (uint *)0x0;
      }
      iVar1 = FUN_00128968(param_1,uVar4,local_38);
      if ((iVar1 != 0) && (local_38[0] <= uVar2 - uVar4)) break;
      uVar2 = uVar4 + *(int *)(param_1 + 0xc);
    }
    if (local_2c == -1) break;
    uVar2 = local_38[0] + uVar4;
    uVar3 = uVar4;
    uVar6 = local_38[0];
  }
  *puVar7 = uVar4;
  *puVar5 = local_38[0] + uVar4;
  return puVar5;
}

// ==== file_read @ 00128ad4, 156 bytes
// called by: lcd_boot_proc, idle_screen_proc, usb_command
// ? file_read(path, 0, buffer, size) > 0 when read
/* ? file_read(path, 0, buffer, size) > 0 when read */

uint file_read(int param_1,uint param_2,int param_3,uint param_4)

{
  int iVar1;
  undefined1 auStack_30 [4];
  uint local_2c;
  
  iVar1 = *(int *)(param_1 + 0x10);
  **(int **)(param_1 + 0x18) = iVar1;
  if (iVar1 == -1 || param_3 == 0) {
    param_4 = 0;
  }
  else {
    iVar1 = FUN_00128968(param_1,iVar1,auStack_30);
    if (iVar1 == 0) {
      param_4 = 0xffffffff;
    }
    else if (param_2 < local_2c) {
      if (local_2c - param_2 < param_4) {
        param_4 = local_2c - param_2;
      }
      iVar1 = (**(code **)(param_1 + 4))
                        (**(int **)(param_1 + 0x18) + param_2 + 0x14,param_3,param_4);
      if (iVar1 == 0) {
        param_4 = 0xfffffffd;
      }
    }
    else {
      param_4 = 0xfffffffe;
    }
  }
  return param_4;
}

// ==== FUN_00128b70 @ 00128b70, 204 bytes
// called by: usb_command
undefined4 FUN_00128b70(undefined4 *param_1,int *param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint local_30;
  int local_2c;
  undefined4 local_28;
  
  uVar3 = (param_3 + 3U & 0xfffffffc) + param_1[3] + 0x13 & ~(param_1[3] - 1);
  iVar1 = param_1[4];
  uVar2 = 0;
  *(int *)(param_1[6] + 4) = iVar1;
  if ((iVar1 + uVar3 <= (uint)param_1[5]) &&
     (iVar1 = (*(code *)*param_1)(iVar1,param_1[3]), iVar1 != 0)) {
    local_28 = *(undefined4 *)param_1[6];
    local_30 = uVar3;
    local_2c = param_3;
    iVar1 = (*(code *)param_1[2])(*(undefined4 *)(param_1[6] + 4),&local_30,0xc);
    if (iVar1 != 0) {
      uVar2 = 1;
      param_2[1] = *(int *)(param_1[6] + 4);
      *param_2 = (int)param_1;
      param_2[2] = param_3;
      param_2[3] = local_30;
    }
  }
  return uVar2;
}

// ==== FUN_00128c3c @ 00128c3c, 176 bytes
// called by: usb_command
undefined4 FUN_00128c3c(int *param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  iVar1 = 0;
  if (param_1 != (int *)0x0) {
    iVar1 = *param_1;
  }
  if (param_1 == (int *)0x0 || iVar1 == 0) {
    return 0;
  }
  uVar3 = param_2 + param_1[1] + 0x14;
  uVar2 = *(int *)(iVar1 + 0xc) - (*(int *)(iVar1 + 0xc) - 1U & uVar3);
  if (uVar2 == 0) goto LAB_00128cb0;
  if (param_4 < uVar2) {
    uVar2 = param_4;
  }
  while( true ) {
    iVar1 = (**(code **)(iVar1 + 8))(uVar3,param_3,uVar2);
    if (iVar1 == 0) {
      return 0xffffffff;
    }
    uVar3 = uVar3 + uVar2;
    param_4 = param_4 - uVar2;
    param_3 = param_3 + uVar2;
LAB_00128cb0:
    if (param_4 == 0) break;
    uVar2 = ((undefined4 *)*param_1)[3];
    if (param_4 <= uVar2) {
      uVar2 = param_4;
    }
    iVar1 = (**(code **)*param_1)(uVar3,uVar2);
    if (iVar1 == 0) {
      return 0;
    }
    iVar1 = *param_1;
  }
  return 1;
}

// ==== FUN_00128cec @ 00128cec, 200 bytes
// called by: usb_command
undefined4 FUN_00128cec(int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int local_10;
  
  iVar1 = 0;
  if (param_1 != (int *)0x0) {
    iVar1 = *param_1;
  }
  if ((param_1 != (int *)0x0 && iVar1 != 0) &&
     ((**(int **)(iVar1 + 0x18) == -1 ||
      (local_10 = param_4,
      iVar1 = (**(code **)(iVar1 + 8))(**(int **)(iVar1 + 0x18) + 0xc,param_1 + 1,4), iVar1 != 0))))
  {
    local_10 = ~(**(int **)(*param_1 + 0x18) + param_1[2] + (*(int **)(*param_1 + 0x18))[2]) + 1;
    iVar1 = (**(code **)(*param_1 + 8))(param_1[1] + 0x10,&local_10,4);
    if (iVar1 != 0) {
      **(int **)(*param_1 + 0x18) = param_1[1];
      *(int *)(*(int *)(*param_1 + 0x18) + 4) = param_1[1] + param_1[3];
      return 1;
    }
  }
  return 0xffffffff;
}

// ==== FUN_00128db4 @ 00128db4, 168 bytes
// called by: FUN_00128f28
undefined4 FUN_00128db4(int param_1,uint param_2,uint *param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  int local_30;
  uint local_2c;
  uint *puStack_28;
  undefined4 uStack_24;
  
  iVar1 = *(int *)(param_1 + 0xc);
  uVar6 = 0xffffffff;
  uVar3 = param_2 + 8;
  uVar5 = 0xffffffff;
  local_30 = param_1;
  local_2c = param_2;
  puStack_28 = param_3;
  uStack_24 = param_4;
  while( true ) {
    uVar4 = uVar3;
    if (((iVar1 + param_2) - 0x10 <= uVar4) ||
       (iVar2 = (**(code **)(param_1 + 4))(uVar4,&local_30,0x10), iVar2 == 0)) goto LAB_00128e4c;
    if (local_2c == 0xffffffff) break;
    uVar3 = (local_2c & 0x7fffffff) + 3 & 0xfffffffc;
    if (*(uint *)(param_1 + 0xc) < uVar3 + 8) {
      return 0;
    }
    uVar3 = uVar4 + uVar3;
    uVar5 = uVar4;
  }
  if (local_30 != -1) {
    return 0;
  }
  uVar6 = 1;
LAB_00128e4c:
  if (param_3 != (uint *)0x0) {
    *param_3 = uVar4;
    param_3[1] = uVar5;
  }
  return uVar6;
}

// ==== FUN_00128e60 @ 00128e60, 56 bytes
// called by: FUN_00126e0c, store_write
int FUN_00128e60(byte *param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = 0;
  if (param_1 != (byte *)0x0) {
    for (uVar1 = 0; uVar1 < param_2; uVar1 = uVar1 + 1) {
      iVar2 = iVar2 + (uint)*param_1;
      param_1 = param_1 + 1;
    }
  }
  return ~(iVar2 + param_2) + 1;
}

// ==== FUN_00128e98 @ 00128e98, 144 bytes
// called by: FUN_00128f28
undefined4 FUN_00128e98(undefined4 *param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  if ((int)param_2 < 0) {
    iVar3 = param_1[5];
    for (uVar2 = 0; uVar2 < (uint)param_1[4]; uVar2 = uVar2 + 1) {
      iVar1 = (*(code *)*param_1)(iVar3,param_1[3]);
      if (iVar1 == 0) {
        return 0;
      }
      iVar3 = iVar3 + param_1[3];
    }
    *(undefined4 *)param_1[6] = 0xffffffff;
  }
  else if ((param_2 < (uint)param_1[4]) &&
          (iVar3 = (*(code *)*param_1)(param_1[3] * param_2 + param_1[5],param_1[3]), iVar3 == 0)) {
    return 0;
  }
  return 1;
}

// ==== FUN_00128f28 @ 00128f28, 332 bytes
// called by: config_load
void FUN_00128f28(int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  int local_38;
  int local_34;
  int local_30;
  uint local_2c;
  undefined4 local_20;
  undefined4 local_1c;
  
  **(undefined4 **)(param_1 + 0x18) = 0xffffffff;
  iVar4 = *(int *)(param_1 + 0x14);
  uVar3 = 0;
  do {
    if (*(uint *)(param_1 + 0x10) <= uVar3) {
      return;
    }
    iVar1 = (**(code **)(param_1 + 4))(iVar4,&local_38,0x18);
    if (iVar1 == 0) {
      return;
    }
    if (local_30 == -1) {
      iVar1 = local_38;
      if (local_38 == -1) {
        iVar1 = local_34;
      }
      if (local_38 != -1 || iVar1 != -1) goto LAB_00129060;
    }
    else {
      if ((local_30 != 0x555aaa5a) || (*(int *)(param_1 + 0xc) - 8U < (local_2c & 0x7fffffff)))
      goto LAB_00129060;
      if (local_38 == 0x555aaa5a) {
        iVar1 = FUN_00128db4(param_1,iVar4,0);
        if (iVar1 == 0) goto LAB_00129060;
      }
      else {
        if ((local_38 != -1) || (local_34 != -1 && local_34 != 0x555aaa5a)) {
LAB_00129060:
          FUN_00128e98(param_1,0xffffffff);
          return;
        }
        uVar5 = FUN_00128db4(param_1,iVar4,&local_20);
        iVar1 = (int)((ulonglong)uVar5 >> 0x20);
        iVar2 = (int)uVar5;
        if (iVar2 == 0) goto LAB_00129060;
        if (iVar2 != 1) {
          if (iVar2 == -1) {
            iVar1 = **(int **)(param_1 + 0x18);
          }
          if (iVar2 != -1 || iVar1 != -1) goto LAB_00129044;
        }
        *(undefined4 *)(*(int *)(param_1 + 0x18) + 8) = local_1c;
        *(undefined4 *)(*(int *)(param_1 + 0x18) + 4) = local_20;
        **(uint **)(param_1 + 0x18) = uVar3;
      }
    }
LAB_00129044:
    uVar3 = uVar3 + 1;
    iVar4 = iVar4 + *(int *)(param_1 + 0xc);
  } while( true );
}

// ==== FUN_00129078 @ 00129078, 424 bytes
// called by: store_write
uint FUN_00129078(undefined4 *param_1,int *param_2,int param_3,int param_4)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 local_20;
  
  if (param_2 == (int *)0x0) {
    return 0;
  }
  uVar7 = param_3 + 0x13U & 0xfffffffc;
  param_2[2] = param_3 + 0x10U & 0x7fffffff | param_4 << 0x1f;
  uVar3 = param_1[3];
  if (uVar3 < uVar7 + 8) {
    return 0;
  }
  local_20 = 0x555aaa5a;
  puVar1 = (uint *)param_1[6];
  uVar6 = *puVar1;
  uVar3 = uVar6 * uVar3 + param_1[5] + uVar3;
  uVar4 = puVar1[1];
  if (uVar4 < uVar3) {
    uVar5 = uVar3 - uVar4;
  }
  else {
    uVar5 = 0;
  }
  if ((uVar6 == 0xffffffff) || (uVar5 < uVar7)) {
    *puVar1 = uVar6 + 1;
    if ((uint)param_1[4] <= uVar6 + 1) {
      *(undefined4 *)param_1[6] = 0;
      uVar3 = param_1[5];
    }
    iVar2 = (*(code *)*param_1)(uVar3,param_1[3]);
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = (*(code *)param_1[2])(uVar3 + 0xc,param_2 + 2,4);
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = (*(code *)param_1[2])(uVar3 + 0x10,param_1[6] + 8,4);
    if (iVar2 == 0) {
      return 0;
    }
    if ((uVar6 != 0xffffffff) &&
       (iVar2 = (*(code *)param_1[2])(uVar6 * param_1[3] + param_1[5],&local_20,4), iVar2 == 0)) {
      return 0;
    }
    iVar2 = (*(code *)param_1[2])(uVar3 + 4,&local_20,4);
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = uVar3 + 8;
  }
  else {
    iVar2 = (*(code *)param_1[2])(uVar4 + 4,param_2 + 2,4);
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = (*(code *)param_1[2])(*(int *)(param_1[6] + 4) + 8,param_1[6] + 8,4);
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = *(int *)(param_1[6] + 4);
  }
  param_2[1] = iVar2;
  *param_2 = (int)param_1;
  return uVar7;
}

// ==== FUN_00129224 @ 00129224, 152 bytes
// called by: store_write
undefined4 FUN_00129224(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_18 [2];
  int *piStack_10;
  undefined4 uStack_c;
  
  uVar2 = 0;
  if (param_1 != (int *)0x0) {
    local_18[0] = 0x555aaa5a;
    piStack_10 = param_1;
    uStack_c = param_2;
    iVar1 = (**(code **)(*param_1 + 8))(param_1[1] + 0xc,&uStack_c,4);
    uVar2 = 0;
    if ((iVar1 != 0) &&
       (iVar1 = (**(code **)(*param_1 + 8))(param_1[1],local_18,4), uVar2 = 0, iVar1 != 0)) {
      *(int *)(*(int *)(*param_1 + 0x18) + 8) = param_1[1];
      *(uint *)(*(int *)(*param_1 + 0x18) + 4) =
           param_1[1] + ((param_1[2] & 0x7fffffffU) + 3 & 0xfffffffc);
      uVar2 = 1;
    }
  }
  return uVar2;
}

// ==== FUN_001292c0 @ 001292c0, 108 bytes
// called by: store_write
undefined4 FUN_001292c0(int *param_1,int param_2,undefined4 param_3,uint param_4)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  
  if (((param_1 != (int *)0x0) && (0xf < (param_1[2] & 0x7fffffffU))) &&
     (iVar2 = (param_1[2] & 0x7fffffffU) - 0x10, param_2 < iVar2)) {
    uVar3 = iVar2 - param_2;
    if (uVar3 < param_4) {
      param_4 = uVar3;
    }
                    /* WARNING: Could not recover jumptable at 0x0012931c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (**(code **)(*param_1 + 8))(param_2 + param_1[1] + 0x10,param_3,param_4);
    return uVar1;
  }
  return 0;
}

// ==== FUN_0012932c @ 0012932c, 208 bytes
// called by: usb_receive_task
void FUN_0012932c(int param_1)

{
  bool bVar1;
  int local_38;
  int local_34;
  int local_2c;
  undefined4 local_18;
  undefined4 uStack_14;
  
  FUN_0012ae24(&DAT_00021000,0x20,&local_38);
  if (param_1 == 0) {
    if (local_38 != 0x1234abcd) {
      return;
    }
    bVar1 = local_34 != -1;
    if (!bVar1) {
      local_18 = 0x1234abcd;
      uStack_14 = 0x1234abcd;
      FUN_0012af60(&DAT_00021004,8,&local_18);
      FUN_0012ae24(&DAT_00021000,0x20,&local_38);
    }
  }
  else {
    bVar1 = local_2c != -1;
    local_34 = local_2c;
    if (!bVar1) {
      local_18 = 0x1234abcd;
      FUN_0012af60(&DAT_0002100c,4,&local_18);
      FUN_0012ae24(&DAT_00021000,0x20,&local_38);
      local_34 = local_2c;
    }
  }
  if ((local_34 == 0x1234abcd) && (!bVar1)) {
    FUN_0012a964();
  }
  return;
}

// ==== FUN_00129450 @ 00129450, 312 bytes
void FUN_00129450(void)

{
  int iVar1;
  
  DAT_0066aa79 = 0x40;
  DAT_0066aa7a = 0;
  DAT_0066aa7b = 0;
  DAT_0066aa7c = 0;
  DAT_b1008078 = 0x40;
  do {
    iVar1 = DAT_b1008078;
  } while (iVar1 != 0x40);
  DAT_b100806c = 0x10;
  DAT_b1008074 = 0;
  DAT_b1008078 = 0x40;
  DAT_b1008080 = 0x15;
  DAT_b1008084 = 0x40;
  DAT_b1008088 = 0x7f;
  DAT_b1008094 = 8;
  DAT_b100809c = 0;
  DAT_b10080a0 = 0x40;
  DAT_b10080a8 = 0x2d;
  DAT_b10080ac = 0x80;
  DAT_b10080b0 = 0xbf;
  DAT_b10080bc = 0x10;
  DAT_b10080c4 = 0;
  DAT_b10080c8 = 0x40;
  DAT_b10080d0 = 0x35;
  DAT_b10080d4 = 0xc0;
  DAT_b10080d8 = 0xff;
  DAT_b10080e4 = 8;
  DAT_b10080ec = 0;
  DAT_b10080f0 = 0x40;
  DAT_b10080f8 = 0x4d;
  DAT_b10080fc = 0x100;
  DAT_b1008100 = 0x13f;
  DAT_0066a9bc = 0x9c;
  DAT_0066a9bd = 0x29;
  DAT_0066a9be = 0x18;
  DAT_0066a9bf = 0;
  DAT_0066a9fc = 0x1d;
  DAT_0066a9fd = 0;
  DAT_0066a9fe = 0;
  DAT_0066a9ff = 0;
  DAT_0066a9c0 = 0x9c;
  DAT_0066a9c1 = 0x29;
  DAT_0066a9c2 = 0x18;
  DAT_0066a9c3 = 0;
  DAT_0066aa00 = 0x1d;
  DAT_0066aa01 = 0;
  DAT_0066aa02 = 0;
  DAT_0066aa03 = 0;
  return;
}

// ==== FUN_00129594 @ 00129594, 332 bytes
void FUN_00129594(void)

{
  int iVar1;
  
  DAT_0066aa79 = 0x40;
  DAT_0066aa7a = 0;
  DAT_0066aa7b = 0;
  DAT_0066aa7c = 0;
  DAT_b1008078 = 0x40;
  do {
    iVar1 = DAT_b1008078;
  } while (iVar1 != 0x40);
  DAT_b100806c = 0x10;
  DAT_b1008074 = 0;
  DAT_b1008078 = 0x1b8;
  DAT_b1008080 = 0x15;
  DAT_b1008084 = 0x40;
  DAT_b1008088 = 0x1f7;
  DAT_b1008094 = 8;
  DAT_b100809c = 0;
  DAT_b10080a0 = 0x1b8;
  DAT_b10080a8 = 0x2d;
  DAT_b10080ac = 0x1f8;
  DAT_b10080b0 = 0x3af;
  DAT_b10080bc = 0x10;
  DAT_b10080c4 = 0;
  DAT_b10080c8 = 0x400;
  DAT_b10080d0 = 0x35;
  DAT_b10080d4 = 0x3b0;
  DAT_b10080d8 = 0x7af;
  DAT_b10080e4 = 8;
  DAT_b10080ec = 0;
  DAT_b10080f0 = 0x10;
  DAT_b10080f8 = 0x4d;
  DAT_b10080fc = 0x7b0;
  DAT_b1008100 = 0x7bf;
  DAT_0066a9bc = 0x5c;
  DAT_0066a9bd = 0x29;
  DAT_0066a9be = 0x18;
  DAT_0066a9bf = 0;
  DAT_0066a9fc = 0x1d;
  DAT_0066a9fd = 0;
  DAT_0066a9fe = 0;
  DAT_0066a9ff = 0;
  DAT_0066a9c0 = 0x7c;
  DAT_0066a9c1 = 0x29;
  DAT_0066a9c2 = 0x18;
  DAT_0066a9c3 = 0;
  DAT_0066aa00 = 0x1d;
  DAT_0066aa01 = 0;
  DAT_0066aa02 = 0;
  DAT_0066aa03 = 0;
  return;
}

// ==== FUN_001296f8 @ 001296f8, 1024 bytes
// called by: usb_receive_task
void FUN_001296f8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  
  DAT_0066aa28._0_1_ = 1;
  DAT_0066aa28._1_1_ = 0;
  DAT_0066aa28._2_1_ = 0;
  DAT_0066aa28._3_1_ = 0;
  DAT_0066aa2c._0_1_ = 2;
  DAT_0066aa2c._1_1_ = 0;
  DAT_0066aa2c._2_1_ = 0;
  DAT_0066aa2c._3_1_ = 0;
  DAT_0066aa30._0_1_ = 3;
  DAT_0066aa30._1_1_ = 0;
  DAT_0066aa30._2_1_ = 0;
  DAT_0066aa30._3_1_ = 0;
  DAT_0066aa34._0_1_ = 4;
  DAT_0066aa34._1_1_ = 0;
  DAT_0066aa34._2_1_ = 0;
  DAT_0066aa34._3_1_ = 0;
  DAT_0066aa24._0_1_ = 0x50;
  DAT_0066aa24._1_1_ = 0x94;
  DAT_0066aa24._2_1_ = 0x12;
  DAT_0066aa24._3_1_ = 0;
  DAT_0066aa20._0_1_ = 0x94;
  DAT_0066aa20._1_1_ = 0x95;
  DAT_0066aa20._2_1_ = 0x12;
  DAT_0066aa20._3_1_ = 0;
  DAT_0066a9a0 = 0xbc;
  DAT_0066a9a1 = 0x29;
  DAT_0066a9a2 = 0x18;
  DAT_0066a9a3 = 0;
  DAT_0066a9a8 = 0xd0;
  DAT_0066a9a9 = 0x29;
  DAT_0066a9aa = 0x18;
  DAT_0066a9ab = 0;
  DAT_0066a9ac = 0x1c;
  DAT_0066a9ad = 0x2a;
  DAT_0066a9ae = 0x18;
  DAT_0066a9af = 0;
  DAT_0066a9b8 = 0xe2;
  DAT_0066a9b9 = 0x29;
  DAT_0066a9ba = 0x18;
  DAT_0066a9bb = 0;
  DAT_0066aa4c._0_1_ = 0xc;
  DAT_0066aa4c._1_1_ = 0x94;
  DAT_0066aa4c._2_1_ = 0x12;
  DAT_0066aa4c._3_1_ = 0;
  DAT_0066a9bc = 0x5c;
  DAT_0066a9bd = 0x29;
  DAT_0066a9be = 0x18;
  DAT_0066a9bf = 0;
  DAT_0066a9fc = 0x1d;
  DAT_0066a9fd = 0;
  DAT_0066a9fe = 0;
  DAT_0066a9ff = 0;
  DAT_0066a9c0 = 0x7c;
  DAT_0066a9c1 = 0x29;
  DAT_0066a9c2 = 0x18;
  DAT_0066a9c3 = 0;
  DAT_0066aa00 = 0x1d;
  DAT_0066aa01 = 0;
  DAT_0066aa02 = 0;
  DAT_0066aa03 = 0;
  DAT_0066aa38._0_1_ = 0x58;
  DAT_0066aa38._1_1_ = 0xd5;
  DAT_0066aa38._2_1_ = 0x10;
  DAT_0066aa38._3_1_ = 0;
  DAT_0066aa3c._0_1_ = 0x28;
  DAT_0066aa3c._1_1_ = 0xd6;
  DAT_0066aa3c._2_1_ = 0x10;
  DAT_0066aa3c._3_1_ = 0;
  DAT_0066aa40._0_1_ = 0x3c;
  DAT_0066aa40._1_1_ = 0xd6;
  DAT_0066aa40._2_1_ = 0x10;
  DAT_0066aa40._3_1_ = 0;
  DAT_0066aa44._0_1_ = 0x48;
  DAT_0066aa44._1_1_ = 0xd7;
  DAT_0066aa44._2_1_ = 0x10;
  DAT_0066aa44._3_1_ = 0;
  DAT_0066aa54._0_1_ = 0x50;
  DAT_0066aa54._1_1_ = 0x73;
  DAT_0066aa54._2_1_ = 0x11;
  DAT_0066aa54._3_1_ = 0;
  DAT_0066a9cc = 0x44;
  DAT_0066a9cd = 0x29;
  DAT_0066a9ce = 0x18;
  DAT_0066a9cf = 0;
  DAT_0066a9d0 = 0x68;
  DAT_0066a9d1 = 0x2a;
  DAT_0066a9d2 = 0x18;
  DAT_0066a9d3 = 0;
  DAT_0066a9d4 = 0x7c;
  DAT_0066a9d5 = 0x2a;
  DAT_0066a9d6 = 0x18;
  DAT_0066a9d7 = 0;
  DAT_0066a9e0._0_1_ = 0x12;
  DAT_0066a9e0._1_1_ = 0;
  DAT_0066a9e0._2_1_ = 0;
  DAT_0066a9e0._3_1_ = 0;
  DAT_0066a9e8._0_1_ = 0x49;
  DAT_0066a9e8._1_1_ = 0;
  DAT_0066a9e8._2_1_ = 0;
  DAT_0066a9e8._3_1_ = 0;
  DAT_0066a9ec._0_1_ = 0x49;
  DAT_0066a9ec._1_1_ = 0;
  DAT_0066a9ec._2_1_ = 0;
  DAT_0066a9ec._3_1_ = 0;
  DAT_00182944 = 4;
  DAT_0066aa0c = 4;
  DAT_0066aa0d = 0;
  DAT_0066aa0e = 0;
  DAT_0066aa0f = 0;
  DAT_00182a68 = 0x12;
  DAT_0066aa10 = 0x12;
  DAT_0066aa11 = 0;
  DAT_0066aa12 = 0;
  DAT_0066aa13 = 0;
  DAT_00182a7c = 0x1a;
  DAT_0066aa14 = 0x1a;
  DAT_0066aa15 = 0;
  DAT_0066aa16 = 0;
  DAT_0066aa17 = 0;
  pcVar2 = &DAT_00538780;
  cVar1 = '\x02';
  FUN_0010cc30(&DAT_00538780,0x44,0,&DAT_00182a7c,param_3,param_4);
  pcVar3 = &DAT_007bcabc;
  while( true ) {
    pcVar2 = pcVar2 + 2;
    pcVar3 = pcVar3 + 1;
    if (((char *)0x7bcadc < pcVar3) || (*pcVar3 == '\0')) break;
    *pcVar2 = *pcVar3;
    cVar1 = cVar1 + '\x02';
  }
  *pcVar2 = '\0';
  DAT_00538781 = 3;
  DAT_0066a9d8 = 0x80;
  DAT_0066a9d9 = 0x87;
  DAT_0066a9da = 0x53;
  DAT_0066a9db = 0;
  DAT_0066aa19 = 0;
  DAT_0066aa1a = 0;
  DAT_0066aa1b = 0;
  DAT_00538780 = cVar1;
  DAT_0066aa18 = cVar1;
  dbg_printf("======>sn: %s, %02x %02x %02x %02x\n",&DAT_007bcabd,cVar1,0,3,0);
  return;
}

// ==== usb_receive_task @ 00129b64, 672 bytes
// ? calls usb_receive_cmd
/* ? calls usb_receive_cmd */

longlong usb_receive_task(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  undefined4 local_20;
  
  if (param_2 != 6) {
    local_20 = param_4;
    if ((int)param_2 < 7) {
      if (param_2 == 3) {
        FUN_0012a9f0(&DAT_00484520,0x80464180,0x1b8,300);
        FUN_0012ab08(&DAT_00489a70,0x805389a0,0x100000);
        FUN_0012ab08(&DAT_00489a98,0x806389a0,&DAT_00032000);
        param_2 = 0;
        FUN_0012bd70(param_1,0,1,0xffff);
        FUN_0012c6d0();
        FUN_001296f8();
        FUN_0012c560();
        piVar1 = DAT_00182acc;
        DAT_00182acc[3] = 0;
        *(undefined4 *)(*piVar1 + 0xc) = 0;
      }
    }
    else {
      if (param_2 == 0x38) {
        iVar2 = FUN_0012aadc(&DAT_00489a98,&local_20,0,param_4,0x38,param_3);
        if (iVar2 != 0) {
          iVar2 = usb_receive_cmd(&DAT_00484538,local_20);
          if (iVar2 != 0) {
            if (DAT_00484544 == 0x88) {
              FUN_0012932c(DAT_00484548);
            }
            else {
              param_2 = DAT_0048453c * 0x1b4;
              FUN_0012bb08(&DAT_00182b24,&DAT_00008035,DAT_00484544,&DAT_00484548,param_2,0xffffffff
                          );
            }
          }
          FUN_0012aa5c(&DAT_00489a98);
        }
        if ((DAT_00182950 == 0) ||
           (puVar3 = (uint *)FUN_0012ab3c(&DAT_00489a98,0x1b8), puVar3 == (uint *)0x0))
        goto LAB_00129dfc;
        DAT_b100805c = 1;
        DAT_b1008700 = 0x805387e0;
        DAT_b1008060 = 0x1b8;
        uVar4 = DAT_b100805c;
        DAT_b100805c = uVar4 | 0x20;
        do {
          uVar4 = 0;
          if ((char)DAT_0066aa80 != '\0') {
            uVar4 = DAT_b100805c;
            uVar4 = uVar4 & 0x20;
          }
        } while ((char)DAT_0066aa80 != '\0' && uVar4 != 0);
        DAT_00182950 = 0;
        FUN_0010c9b8(puVar3,0x805387e0,0x1b8);
        if ((~*puVar3 & 0xff) != 0) goto LAB_00129dfc;
        FUN_0012bb08(&DAT_00182b24,&DAT_00008035,*puVar3 & 0xff,puVar3 + 1,0x1b4,0xffffffff);
        FUN_0012a964();
      }
      else if (param_2 != 0x3b) goto LAB_00129dfc;
      param_2 = 0;
      usb_receive_data(0xff,param_4);
    }
  }
LAB_00129dfc:
  return (ulonglong)param_2 << 0x20;
}

// ==== usb_receive_cmd @ 00129e44, 236 bytes
// called by: usb_receive_task
// refs "---->receiving cmd error %02x %02x"
/* refs "---->receiving cmd error %02x %02x" */

bool usb_receive_cmd(uint *param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  bool bVar4;
  
  uVar1 = *param_2;
  if ((uVar1 & 0x80000000) == 0) {
    uVar3 = param_1[3];
    if (uVar3 == (uVar1 & 0xff)) {
      uVar2 = param_1[2];
      bVar4 = uVar2 != (uVar1 & 0x7fffffff) >> 8;
      if (bVar4) {
        uVar3 = ((uVar1 & 0x7fffffff) >> 8) - 1;
      }
      if (!bVar4 || uVar3 == uVar2) {
        if ((uVar1 & 0x7fffffff) >> 8 < 0x32) {
          FUN_0010cb60(param_1 + ((uVar1 & 0x7fffffff) >> 8) * 0x6d + 4,param_2 + 1,0x1b4);
        }
        param_1[2] = (*param_2 & 0x7fffffff) >> 8;
      }
    }
    else {
      FUN_00103ecc("---->receiving cmd error %02x %02x\n",uVar1 & 0xff);
      param_1[3] = 0;
    }
  }
  else {
    param_1[3] = uVar1 & 0xff;
    param_1[2] = 0;
    param_1[1] = (*param_2 & 0x7fffffff) >> 8;
    FUN_0010cb60(param_1 + 4,param_2 + 1,0x1b4);
  }
  bVar4 = param_1[1] <= param_1[2] + 1;
  if (bVar4) {
    *param_1 = *param_1 | 1;
  }
  return bVar4;
}

// ==== FUN_00129f54 @ 00129f54, 232 bytes
// called by: FUN_0010d63c
bool FUN_00129f54(uint *param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  bool bVar4;
  
  uVar1 = *param_2;
  if ((uVar1 & 0x80000000) == 0) {
    uVar3 = param_1[3];
    if (uVar3 == (uVar1 & 0xff)) {
      uVar2 = param_1[2];
      bVar4 = uVar2 != (uVar1 & 0x7fffffff) >> 8;
      if (bVar4) {
        uVar3 = ((uVar1 & 0x7fffffff) >> 8) - 1;
      }
      if (!bVar4 || uVar3 == uVar2) {
        if ((uVar1 & 0x7fffffff) >> 8 < 700) {
          FUN_0010cb60(param_1 + ((uVar1 & 0x7fffffff) >> 8) * 0xff + 4,param_2 + 1,0x3fc);
        }
        param_1[2] = (*param_2 & 0x7fffffff) >> 8;
      }
    }
    else {
      FUN_00103ecc("---->receiving data error %02x %02x\n",uVar1 & 0xff);
      param_1[3] = 0;
    }
  }
  else {
    param_1[3] = uVar1 & 0xff;
    param_1[2] = 0;
    param_1[1] = (*param_2 & 0x7fffffff) >> 8;
    FUN_0010cb60(param_1 + 4,param_2 + 1,0x3fc);
  }
  bVar4 = param_1[1] <= param_1[2] + 1;
  if (bVar4) {
    *param_1 = *param_1 | 1;
  }
  return bVar4;
}

// ==== usb_receive_data @ 0012a064, 476 bytes
// called by: usb_receive_task, usb_command
// refs "-----> no enough space0:..." (file upload data)
/* refs "-----> no enough space0:..." (file upload data) */

undefined4 usb_receive_data(byte param_1,int param_2,uint param_3,undefined4 param_4,uint param_5)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  uint local_1e4;
  undefined1 auStack_1e0 [436];
  undefined4 local_2c;
  
  local_2c = 0;
  FUN_0010cc30(&local_1e4,0x1b8);
  if (param_5 < 0x1b5) {
    uVar1 = FUN_0010d398(param_3 + param_5 + 0x1b3,0x1b4);
    local_1e4 = CONCAT31(local_1e4._1_3_,param_1);
    if (uVar1 == 0) {
      uVar1 = 1;
    }
    local_1e4 = (uVar1 & 0x7fffff) << 8 | local_1e4 & 0x800000ff | 0x80000000;
    FUN_0010cc30(auStack_1e0,0x1b4);
    if (param_5 != 0) {
      FUN_0010c9b8(auStack_1e0,param_4,param_5);
    }
    uVar1 = 0x1b4 - param_5;
    if (param_3 <= 0x1b4 - param_5) {
      uVar1 = param_3;
    }
    FUN_0010c9b8(auStack_1e0 + param_5,param_2,uVar1);
    iVar2 = FUN_0012aa18(&DAT_00484520);
    if (iVar2 == 0) {
      pcVar4 = "-----> no enough space1:%08x %d %d %d\n";
LAB_0012a210:
      dbg_printf(pcVar4,&DAT_00484520,DAT_00484534,DAT_00484532,DAT_00484530);
    }
    else {
      FUN_0010cb60(iVar2,&local_1e4,0x1b8);
      param_2 = param_2 + uVar1;
      uVar3 = 1;
      for (param_3 = param_3 - uVar1; param_3 != 0; param_3 = param_3 - uVar1) {
        local_1e4 = (uVar3 & 0x7fffff) << 8 | (uint)param_1;
        FUN_0010cc30(auStack_1e0,0x1b4);
        uVar1 = param_3;
        if (0x1b4 < param_3) {
          uVar1 = 0x1b4;
        }
        FUN_0010c9b8(auStack_1e0,param_2,uVar1);
        iVar2 = FUN_0012aa18(&DAT_00484520);
        if (iVar2 == 0) {
          pcVar4 = "-----> no enough space0:%08x %d %d %d\n";
          goto LAB_0012a210;
        }
        FUN_0010cb60(iVar2,&local_1e4,0x1b8);
        param_2 = param_2 + uVar1;
        uVar3 = ((local_1e4 & 0x7fffffff) >> 8) + 1;
      }
    }
  }
  return local_2c;
}

// ==== FUN_0012a298 @ 0012a298, 408 bytes
void FUN_0012a298(void)

{
  int iVar1;
  uint in_r3;
  uint local_20;
  
  DAT_00182954 = DAT_00182954 + -1;
  if (DAT_00182954 == -1) {
    DAT_00182954 = 5;
    local_20 = in_r3;
    iVar1 = FUN_0012a9dc(&DAT_00484520);
    if ((iVar1 != 0) && (DAT_00182948 == 0)) {
      DAT_00182948 = 1;
      FUN_00115498(iVar1,0x1b8);
      FUN_0012a99c(&DAT_00484520);
    }
    if (DAT_00182958 == 0) {
      if ((*(int *)(*DAT_00182acc + 0xc) == 0) &&
         (iVar1 = FUN_0012aadc(&DAT_00489a70,&local_20,0), iVar1 != 0)) {
        if ((g_config.idle_state == 2) && (DAT_00182bc4 = 0, DAT_00182af8 == 0)) {
          DAT_00182af8 = 1;
          FUN_0010d3f8(g_config.brightness);
        }
        FUN_0011551c(&DAT_0012f2e4,2);
        FUN_001056b4();
        FUN_001065d4(8,&DAT_00006021,0);
        FUN_001065d4(0,*(uint *)(*DAT_00182acc + 4) | 0x80000000);
        FUN_001065d4(0x11,&DAT_001155a0,0);
        FUN_001065d4(4,local_20 | 0x80000000,0);
        FUN_001065d4(0xc,0);
        DAT_00182958 = 1;
      }
    }
    else {
      iVar1 = FUN_00105e70();
      if (iVar1 == 0) {
        if (DAT_00182bc0 == 0) {
          DAT_00182958 = 0;
          FUN_0012aa5c(&DAT_00489a70);
        }
      }
      else {
        DAT_00182958 = 0;
        FUN_0012aa5c(&DAT_00489a70);
        *(undefined4 *)(*DAT_00182acc + 0xc) = 0xffffffff;
      }
    }
  }
  return;
}

// ==== FUN_0012a460 @ 0012a460, 68 bytes
void FUN_0012a460(void)

{
  int iVar1;
  int in_r3;
  
  iVar1 = **(int **)(in_r3 + 0xc);
  (*(int **)(in_r3 + 0xc))[3] = 0;
  if (*(int *)(iVar1 + 0xc) != 0) {
    *(int *)(in_r3 + 0xc) = iVar1;
    FUN_00108404(*(uint *)(iVar1 + 4) | 0x80000000);
  }
  return;
}

// ==== FUN_0012a4a8 @ 0012a4a8, 72 bytes
// called by: idle_screen_proc
undefined4 FUN_0012a4a8(undefined4 param_1)

{
  undefined4 local_c;
  undefined1 local_8;
  undefined1 local_7;
  undefined1 local_6;
  undefined1 local_5;
  
  local_5 = (byte)((uint)param_1 >> 0x18);
  local_6 = (byte)((uint)param_1 >> 0x10);
  local_7 = (byte)((uint)param_1 >> 8);
  local_8 = (byte)param_1;
  local_c = CONCAT13(local_5,CONCAT12((char)((uint)((int)(short)(ushort)local_5 *
                                                   (int)(short)(ushort)local_8) >> 8),
                                      CONCAT11((char)((uint)((int)(short)(ushort)local_5 *
                                                            (int)(short)(ushort)local_7) >> 8),
                                               (char)((uint)((int)(short)(ushort)local_5 *
                                                            (int)(short)(ushort)local_6) >> 8))));
  return local_c;
}

// ==== FUN_0012a4f0 @ 0012a4f0, 476 bytes
// called by: FUN_001001cc
void FUN_0012a4f0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  FUN_0012b760(&DAT_007bc310,0x10,0x1e,param_4,param_3,param_4);
  FUN_0012b7d0(&DAT_007bc4f0,0x200);
  FUN_0012be74(&DAT_007bc6f0,0x20,0x14);
  FUN_0012b388(&DAT_00182b1c,FUN_0012c4d4,0,0,0,0);
  FUN_0012b388(&DAT_00182b24,usb_command,0,0,0,0);
  FUN_0012b388(&DAT_00182b1c,FUN_001286c8,0,0,0,0);
  FUN_0012b388(&DAT_00182b1c,FUN_00128914,0,0,0,0);
  FUN_0012b388(&DAT_00182b1c,screen_create_proc,0,0,0,0);
  DAT_00182b30 = FUN_0012b388(&DAT_00182b1c,config_load,0,0,0,0);
  FUN_0012b388(&DAT_00182b1c,usb_receive_task,0,0,0,0);
  FUN_0012b388(&DAT_00182b1c,FUN_001284b8,0,0,0,0);
  DAT_00182b34 = FUN_0012b388(&DAT_00182b1c,lcd_boot_proc,0,0,0,0);
  DAT_00182b2c = FUN_0012b388(&DAT_00182b1c,idle_screen_proc,0,0,0,0);
  FUN_0012bb08(&DAT_00182b1c,&DAT_00004003,0,0,0,0xffffffff);
  FUN_00102990(1,FUN_0012ea00);
  FUN_001029ec(2);
  FUN_00102938();
  FUN_00102964();
  do {
    if (DAT_00182bcc == 0) {
      FUN_00102888();
    }
    else {
      FUN_001028e0();
    }
    FUN_0012b8b0();
    FUN_0012bf3c();
    FUN_0012bb08(&DAT_00182b1c,0x38,0,0,0,0xffffffff);
  } while( true );
}

// ==== FUN_0012a720 @ 0012a720, 108 bytes
// called by: usb_command
undefined8 FUN_0012a720(undefined4 param_1,int param_2,int param_3,uint param_4)

{
  uint uVar1;
  undefined4 uStack_28;
  int iStack_24;
  int iStack_20;
  uint uStack_1c;
  
  uStack_28 = param_1;
  iStack_24 = param_2;
  iStack_20 = param_3;
  uStack_1c = param_4;
  for (; param_4 != 0; param_4 = param_4 - uVar1) {
    uVar1 = param_4;
    if (0x10 < param_4) {
      uVar1 = 0x10;
    }
    FUN_0010c9b8(&uStack_28,param_2,uVar1);
    FUN_0012a78c(param_1,&uStack_28);
    FUN_0010c9b8(param_3,&uStack_28,uVar1);
    param_3 = param_3 + uVar1;
    param_2 = param_2 + uVar1;
  }
  return CONCAT44(iStack_24,uStack_28);
}

// ==== FUN_0012a78c @ 0012a78c, 188 bytes
// called by: FUN_0012a720
uint * FUN_0012a78c(int *param_1,int *param_2,uint *param_3)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  iVar3 = 0x28;
  uVar4 = param_2[3];
  uVar5 = param_2[1];
  uVar8 = param_2[2] - param_1[0x2b];
  uVar9 = *param_2 - param_1[0x2a];
  do {
    uVar7 = uVar9;
    uVar6 = uVar8;
    uVar10 = uVar7 * (uVar7 * 2 + 1);
    uVar9 = uVar10 >> 0x1b;
    uVar8 = uVar6 * (uVar6 * 2 + 1);
    piVar2 = param_1 + iVar3;
    uVar1 = uVar8 >> 0x1b;
    uVar8 = (uVar5 - param_1[iVar3 + 1] >> uVar9 |
            uVar5 - param_1[iVar3 + 1] << (0x20 - uVar9 & 0xff)) ^ (uVar1 | uVar8 * 0x20);
    iVar3 = iVar3 + -2;
    uVar9 = (uVar4 - *piVar2 >> uVar1 | uVar4 - *piVar2 << (0x20 - uVar1 & 0xff)) ^
            (uVar9 | uVar10 * 0x20);
    uVar4 = uVar6;
    uVar5 = uVar7;
  } while (1 < iVar3);
  iVar3 = *param_1;
  param_3[3] = uVar6 - param_1[1];
  param_3[1] = uVar7 - iVar3;
  param_3[2] = uVar8;
  *param_3 = uVar9;
  return param_3;
}

// ==== FUN_0012a848 @ 0012a848, 280 bytes
// called by: usb_command
/* WARNING: Type propagation algorithm not settling */

void FUN_0012a848(undefined4 *param_1,int param_2,int param_3)

{
  int iVar1;
  byte *pbVar2;
  uint *puVar3;
  int iVar4;
  int extraout_r1;
  int extraout_r1_00;
  uint uVar5;
  int iVar6;
  uint uVar7;
  undefined4 *puVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  bool bVar12;
  uint uStack_4c;
  uint local_48 [9];
  
  if (0x20 < param_3) {
    param_3 = 0x20;
  }
  for (puVar8 = param_1; puVar8 < param_1 + 0x2c; puVar8 = puVar8 + 1) {
    *puVar8 = 0;
  }
  iVar1 = (int)(param_3 + 3 + ((uint)(param_3 + 3 >> 0x1f) >> 0x1e)) >> 2;
  param_3 = param_3 + -1;
  bVar12 = -1 < param_3;
  iVar4 = iVar1;
  if (bVar12) {
    iVar4 = (int)local_48;
  }
  (&uStack_4c)[iVar1] = 0;
  while (bVar12) {
    puVar3 = (uint *)(iVar4 + (param_3 / 4) * 4);
    pbVar2 = (byte *)(param_2 + param_3);
    param_3 = param_3 + -1;
    bVar12 = -1 < param_3;
    *puVar3 = (uint)(*puVar3 < 8) + (uint)*pbVar2;
  }
  iVar6 = 0;
  *param_1 = &DAT_b7e15163;
  iVar11 = 0;
  uVar7 = 0;
  iVar4 = (int)&DAT_0000002b + 1;
  if (0x2c < iVar1) {
    iVar4 = iVar1;
  }
  uVar10 = 0;
  for (iVar9 = 1; iVar9 <= iVar4 * 3; iVar9 = iVar9 + 1) {
    uVar10 = param_1[iVar11] + uVar10 + uVar7;
    uVar10 = uVar10 >> 0x1d | uVar10 * 8;
    param_1[iVar11] = uVar10;
    uVar5 = local_48[iVar6] + uVar10 + uVar7;
    uVar7 = -(uVar10 + uVar7) & 0x1f;
    uVar7 = uVar5 >> uVar7 | uVar5 << 0x20 - uVar7;
    local_48[iVar6] = uVar7;
    FUN_0010d398(iVar11 + 1,0x2c);
    divide(iVar6 + 1,iVar1);
    iVar6 = extraout_r1_00;
    iVar11 = extraout_r1;
  }
  return;
}

// ==== FUN_0012a964 @ 0012a964, 48 bytes
// called by: FUN_0010d558, FUN_0012932c, FUN_0010d63c, usb_receive_task
void FUN_0012a964(void)

{
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

// ==== FUN_0012a99c @ 0012a99c, 64 bytes
// called by: FUN_0012a298
int FUN_0012a99c(undefined4 *param_1)

{
  int iVar1;
  
  if (*(short *)(param_1 + 5) == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = param_1[3];
    *(short *)(param_1 + 5) = *(short *)(param_1 + 5) + -1;
    param_1[3] = (uint)*(ushort *)(param_1 + 4) + iVar1;
    if ((uint)param_1[1] <= (uint)*(ushort *)(param_1 + 4) + iVar1) {
      param_1[3] = *param_1;
    }
  }
  return iVar1;
}

// ==== FUN_0012a9dc @ 0012a9dc, 20 bytes
// called by: FUN_0012a298
undefined4 FUN_0012a9dc(int param_1)

{
  undefined4 uVar1;
  
  if (*(short *)(param_1 + 0x14) == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined4 *)(param_1 + 0xc);
  }
  return uVar1;
}

// ==== FUN_0012a9f0 @ 0012a9f0, 40 bytes
// called by: usb_receive_task
void FUN_0012a9f0(int *param_1,int param_2,int param_3,int param_4)

{
  *param_1 = param_2;
  param_1[1] = param_3 * param_4 + param_2;
  param_1[2] = param_2;
  param_1[3] = param_2;
  *(short *)(param_1 + 4) = (short)param_3;
  *(short *)((int)param_1 + 0x12) = (short)param_4;
  *(undefined2 *)(param_1 + 5) = 0;
  return;
}

// ==== FUN_0012aa18 @ 0012aa18, 68 bytes
// called by: usb_receive_data
int FUN_0012aa18(undefined4 *param_1)

{
  int iVar1;
  
  if (*(ushort *)(param_1 + 5) < *(ushort *)((int)param_1 + 0x12)) {
    iVar1 = param_1[2];
    *(ushort *)(param_1 + 5) = *(ushort *)(param_1 + 5) + 1;
    param_1[2] = (uint)*(ushort *)(param_1 + 4) + iVar1;
    if ((uint)param_1[1] <= (uint)*(ushort *)(param_1 + 4) + iVar1) {
      param_1[2] = *param_1;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}

// ==== FUN_0012aa5c @ 0012aa5c, 128 bytes
// called by: FUN_0012a298, usb_receive_task, FUN_0012b8b0
undefined4 FUN_0012aa5c(undefined4 *param_1)

{
  int iVar1;
  int iVar2;
  
  if (param_1[6] != 0) {
    iVar1 = (*(int *)param_1[3] + 3U & 0xfffffffc) + 4;
    param_1[6] = param_1[6] - iVar1;
    param_1[7] = param_1[7] + iVar1;
    *(int *)param_1[3] = 0;
    iVar2 = param_1[3];
    param_1[3] = iVar2 + iVar1;
    if ((uint)param_1[4] <= (uint)(iVar2 + iVar1)) {
      param_1[7] = (param_1[1] - param_1[4]) + param_1[7];
      param_1[4] = param_1[1];
      param_1[3] = *param_1;
    }
    return 1;
  }
  return 0;
}

// ==== FUN_0012aadc @ 0012aadc, 44 bytes
// called by: FUN_0012a298, usb_receive_task, FUN_0012b8b0
undefined4 FUN_0012aadc(int param_1,int *param_2,int param_3)

{
  if (*(int *)(param_1 + 0x18) != 0) {
    *param_2 = param_3 + *(int *)(param_1 + 0xc) + 4;
    return **(undefined4 **)(param_1 + 0xc);
  }
  return 0;
}

// ==== FUN_0012ab08 @ 0012ab08, 52 bytes
// called by: usb_receive_task
void FUN_0012ab08(int *param_1,int *param_2,int param_3)

{
  param_1[5] = param_3;
  *param_1 = (int)param_2;
  param_1[1] = param_3 + (int)param_2;
  param_1[2] = (int)param_2;
  param_1[3] = (int)param_2;
  *param_2 = param_3;
  param_1[6] = 0;
  param_1[7] = param_1[5];
  param_1[4] = param_1[1];
  return;
}

// ==== FUN_0012ab3c @ 0012ab3c, 180 bytes
// called by: FUN_0012b804, FUN_0010d558, FUN_0010d63c, usb_receive_task
int FUN_0012ab3c(uint *param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  
  iVar5 = 0;
  if (param_2 != 0) {
    uVar6 = param_1[2];
    uVar3 = param_1[4];
    iVar1 = (param_2 + 3U & 0xfffffffc) + 4;
    uVar2 = uVar6 + iVar1;
    uVar4 = uVar3;
    if (uVar3 <= uVar2) {
      uVar4 = uVar3 - uVar6;
    }
    if (uVar2 < uVar3) {
      uVar4 = 0;
    }
    if (uVar4 + iVar1 <= param_1[7]) {
      if (uVar4 != 0) {
        param_1[4] = uVar6;
        if (param_1[3] == uVar6) {
          param_1[3] = *param_1;
        }
        param_1[2] = *param_1;
        uVar2 = *param_1 + iVar1;
      }
      param_1[7] = param_1[7] - uVar4;
      *(int *)param_1[2] = param_2;
      iVar5 = param_1[2] + 4;
      param_1[6] = param_1[6] + iVar1;
      param_1[7] = param_1[7] - iVar1;
      if (param_1[4] <= uVar2) {
        uVar2 = *param_1;
      }
      param_1[2] = uVar2;
    }
  }
  return iVar5;
}

// ==== FUN_0012abf0 @ 0012abf0, 64 bytes
// called by: FUN_0012e9b4, FUN_0012e770, FUN_0012ae24, FUN_0012ac38, FUN_0012e888, FUN_0012e7ec, FUN_0012b0a4, FUN_0012e944, FUN_0012af60
undefined4 FUN_0012abf0(int param_1)

{
  uint *puVar1;
  
  if (param_1 == 0) {
    puVar1 = &DAT_b800c000;
  }
  else {
    puVar1 = &DAT_b800c400;
  }
  *puVar1 = *puVar1 | 1;
  if (param_1 == 0) {
    do {
    } while ((*puVar1 & 1) != 0);
  }
  else {
    do {
    } while ((*puVar1 & 1) != 0);
  }
  return 0;
}

// ==== FUN_0012ac38 @ 0012ac38, 184 bytes
// called by: FUN_00128720
void FUN_0012ac38(uint param_1,uint param_2)

{
  uint uVar1;
  uint local_18;
  
  if ((param_1 & 0xffff) == 0) {
    for (local_18 = 0; local_18 < param_2; local_18 = local_18 + 1) {
      FUN_0012e9b4(local_18);
      uVar1 = DAT_b800c008;
      DAT_b800c008 = uVar1 | 1;
      DAT_b800c010 = 0xd8;
      FUN_0012b324(0,0,8);
      FUN_0012abf0(0);
      DAT_b800c010 = param_1 + local_18 * 0x10000;
      FUN_0012b324(0);
      FUN_0012abf0(0);
      uVar1 = DAT_b800c008;
      DAT_b800c008 = uVar1 & 0xfe;
      FUN_0012e770();
    }
  }
  return;
}

// ==== FUN_0012acf8 @ 0012acf8, 232 bytes
// called by: FUN_00128914, config_load
undefined4 FUN_0012acf8(void)

{
  undefined4 uVar1;
  uint uVar2;
  int local_18;
  
  if (DAT_00182a98 == '\0') {
    uVar2 = DAT_b0000208;
    DAT_b0000208 = uVar2 | 0x40;
    uVar2 = DAT_b0000018;
    DAT_b0000018 = uVar2 | 0x1000;
    uVar2 = DAT_b0000018;
    DAT_b0000018 = uVar2 & 0xffffefff;
    local_18 = 0;
    do {
      local_18 = local_18 + 1;
    } while (local_18 < 500);
    uVar1 = FUN_00102104();
    uVar1 = FUN_0010d398(uVar1,1000);
    FUN_0012b2c8(0,0,uVar1,&DAT_00002710);
    DAT_b800c004 = 0;
    uVar2 = DAT_b000008c;
    DAT_b000008c = uVar2 | 0xff000000;
    DAT_b800c008 = 0;
    DAT_b800c000 = 4;
    uVar2 = FUN_0012e7ec();
    if (uVar2 == 0xffffffff) {
      dbg_printf("read id error !! [0x%x]\n",0xffff);
      return 0xffffffff;
    }
    dbg_printf("SPI flash id [0x%x]\n",uVar2 & 0xffff);
    FUN_0012e944(0);
    FUN_0012e770();
    DAT_00182a98 = '\x01';
  }
  return 0;
}

// ==== FUN_0012ae24 @ 0012ae24, 308 bytes
// called by: FUN_0012932c, FUN_0012876c, FUN_00128720
undefined4 FUN_0012ae24(undefined4 param_1,uint param_2,uint *param_3,undefined4 param_4)

{
  undefined1 uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = DAT_b800c008;
  DAT_b800c008 = uVar3 | 1;
  DAT_b800c010 = 3;
  FUN_0012b324(0,0,8,param_4,param_4);
  FUN_0012abf0(0);
  DAT_b800c010 = param_1;
  FUN_0012b324(0);
  FUN_0012abf0(0);
  for (uVar3 = 0; uVar3 < param_2 >> 2; uVar3 = uVar3 + 1) {
    DAT_b800c010 = 0xffffffff;
    FUN_0012b324(0,0,0x20);
    FUN_0012abf0(0);
    uVar2 = DAT_b800c010;
    *param_3 = (uVar2 >> 0x10 | uVar2 << 0x10) >> 8 & 0xffff00ff |
               (uVar2 >> 0x10 & 0xffff00ff | uVar2 << 0x10) << 8;
    param_3 = param_3 + 1;
  }
  if ((param_2 & 3) != 0) {
    for (uVar3 = 0; uVar3 < (param_2 & 3); uVar3 = uVar3 + 1) {
      DAT_b800c010._0_1_ = 0xff;
      FUN_0012b324(0,0,8);
      FUN_0012abf0(0);
      uVar1 = DAT_b800c010._0_1_;
      *(undefined1 *)param_3 = uVar1;
      param_3 = (uint *)((int)param_3 + 1);
    }
  }
  uVar3 = DAT_b800c008;
  DAT_b800c008 = uVar3 & 0xfe;
  return 0;
}

// ==== FUN_0012af60 @ 0012af60, 316 bytes
// called by: FUN_0012932c
void FUN_0012af60(int param_1,uint param_2,uint *param_3)

{
  uint uVar1;
  uint local_28;
  int local_24;
  uint local_20;
  
  local_28 = param_2 >> 8;
  if ((param_2 & 0xff) != 0) {
    local_28 = local_28 + 1;
  }
  for (local_24 = 0; local_24 < (int)local_28; local_24 = local_24 + 1) {
    local_20 = param_2;
    if (0xff < param_2) {
      local_20 = 0x100;
      param_2 = param_2 - 0x100;
    }
    FUN_0012e9b4();
    uVar1 = DAT_b800c008;
    DAT_b800c008 = uVar1 | 1;
    DAT_b800c010 = 2;
    FUN_0012b324(0,0,8);
    FUN_0012abf0(0);
    DAT_b800c010 = param_1 + local_24 * 0x100;
    FUN_0012b324(0);
    FUN_0012abf0(0);
    for (; 0 < (int)local_20; local_20 = local_20 - 4) {
      uVar1 = *param_3;
      DAT_b800c010 = uVar1 << 0x18 | (uVar1 & 0xff00) << 8 | uVar1 >> 8 & 0xff00 | uVar1 >> 0x18;
      FUN_0012b324(0,0,0x20);
      FUN_0012abf0(0);
      param_3 = param_3 + 1;
    }
    uVar1 = DAT_b800c008;
    DAT_b800c008 = uVar1 & 0xfe;
    FUN_0012e770();
  }
  return;
}

// ==== FUN_0012b0a4 @ 0012b0a4, 364 bytes
// called by: FUN_0012b218
undefined4 FUN_0012b0a4(uint param_1,int param_2,uint param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  uint local_20;
  
  if ((0 < (int)param_3) && ((param_1 & 0xff) + param_3 < 0x101)) {
    local_20 = param_4;
    FUN_0012e9b4();
    uVar2 = DAT_b800c008;
    DAT_b800c008 = uVar2 | 1;
    DAT_b800c010 = 2;
    FUN_0012b324(0,0,8);
    FUN_0012abf0(0);
    DAT_b800c010 = param_1;
    FUN_0012b324(0);
    FUN_0012abf0(0);
    uVar2 = param_3 & 3;
    for (iVar1 = param_3 - uVar2; iVar1 = iVar1 * 0x10000 >> 0x10, 0 < iVar1; iVar1 = iVar1 + -4) {
      FUN_0010c9b8(&local_20,param_2,4);
      local_20 = (local_20 >> 0x10 | local_20 << 0x10) >> 8 & 0xffff00ff |
                 (local_20 >> 0x10 & 0xffff00ff | local_20 << 0x10) << 8;
      DAT_b800c010 = local_20;
      FUN_0012b324(0,0,0x20);
      FUN_0012abf0(0);
      param_2 = param_2 + 4;
    }
    if (uVar2 != 0) {
      local_20 = 0;
      FUN_0010c9b8(&local_20,param_2,uVar2);
      local_20 = (local_20 >> 0x10 | local_20 << 0x10) >> 8 & 0xffff00ff |
                 (local_20 >> 0x10 & 0xffff00ff | local_20 << 0x10) << 8;
      DAT_b800c010 = local_20;
      FUN_0012b324(0,0,0x20);
      FUN_0012abf0(0);
    }
    uVar2 = DAT_b800c008;
    DAT_b800c008 = uVar2 & 0xfe;
    FUN_0012e770();
    return 1;
  }
  return 0;
}

// ==== FUN_0012b218 @ 0012b218, 176 bytes
// called by: FUN_001287c8
int FUN_0012b218(uint param_1,int param_2,uint param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint auStack_20 [2];
  
  iVar1 = 0;
  if ((param_1 & 0xff) != 0) {
    uVar2 = 0x100 - (param_1 & 0xff);
    if (param_3 < uVar2) {
      uVar2 = param_3;
    }
    iVar1 = FUN_0012b0a4(param_1,param_2,(int)(short)uVar2);
    if (iVar1 == 0) {
      return 0;
    }
    param_2 = param_2 + uVar2;
    param_3 = param_3 - uVar2 & 0xffff;
    param_1 = param_1 + uVar2;
  }
  uVar2 = param_3 >> 8;
  uVar3 = param_3 & 0xff;
  do {
    if (uVar2 == 0) {
LAB_0012b2ac:
      if (uVar3 == 0) {
        return iVar1;
      }
      if ((uVar3 != 0) && ((param_1 & 0xff) + uVar3 < 0x101)) {
        auStack_20[0] = param_4;
        FUN_0012e9b4();
        uVar2 = DAT_b800c008;
        DAT_b800c008 = uVar2 | 1;
        DAT_b800c010 = 2;
        FUN_0012b324(0,0,8);
        FUN_0012abf0(0);
        DAT_b800c010 = param_1;
        FUN_0012b324(0);
        FUN_0012abf0(0);
        param_3 = param_3 & 3;
        for (iVar1 = uVar3 - param_3; iVar1 = iVar1 * 0x10000 >> 0x10, 0 < iVar1; iVar1 = iVar1 + -4
            ) {
          FUN_0010c9b8(auStack_20,param_2,4);
          auStack_20[0] =
               (auStack_20[0] >> 0x10 | auStack_20[0] << 0x10) >> 8 & 0xffff00ff |
               (auStack_20[0] >> 0x10 & 0xffff00ff | auStack_20[0] << 0x10) << 8;
          DAT_b800c010 = auStack_20[0];
          FUN_0012b324(0,0,0x20);
          FUN_0012abf0(0);
          param_2 = param_2 + 4;
        }
        if (param_3 != 0) {
          auStack_20[0] = 0;
          FUN_0010c9b8(auStack_20,param_2,param_3);
          auStack_20[0] =
               (auStack_20[0] >> 0x10 | auStack_20[0] << 0x10) >> 8 & 0xffff00ff |
               (auStack_20[0] >> 0x10 & 0xffff00ff | auStack_20[0] << 0x10) << 8;
          DAT_b800c010 = auStack_20[0];
          FUN_0012b324(0,0,0x20);
          FUN_0012abf0(0);
        }
        uVar2 = DAT_b800c008;
        DAT_b800c008 = uVar2 & 0xfe;
        FUN_0012e770();
        return 1;
      }
      return 0;
    }
    iVar1 = FUN_0012b0a4(param_1,param_2,0x100);
    if (iVar1 == 0) {
      if (uVar2 != 0) {
        return 0;
      }
      goto LAB_0012b2ac;
    }
    uVar2 = uVar2 - 1;
    param_1 = param_1 + 0x100;
    param_2 = param_2 + 0x100;
  } while( true );
}

// ==== FUN_0012b2c8 @ 0012b2c8, 84 bytes
// called by: FUN_0012acf8
void FUN_0012b2c8(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  
  if (param_2 != 0) {
    return;
  }
  iVar1 = divide(param_3 * 1000,param_4 << 1,param_4,param_3,param_4);
  if (param_1 == 0) {
    DAT_b800c004 = iVar1 + -1;
  }
  else {
    DAT_b800c404 = iVar1 + -1;
  }
  return;
}

// ==== FUN_0012b324 @ 0012b324, 96 bytes
// called by: FUN_0012e9b4, FUN_0012e770, FUN_0012ae24, FUN_0012ac38, FUN_0012e888, FUN_0012e7ec, FUN_0012b0a4, FUN_0012e944, FUN_0012af60
undefined4 FUN_0012b324(int param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  
  if (param_1 == 0) {
    uVar1 = DAT_b800c000;
  }
  else {
    uVar1 = DAT_b800c400;
  }
  if ((param_2 < 4) && (0 < (int)param_3)) {
    uVar2 = param_3;
    if (param_3 == 0x20) {
      uVar2 = uVar1 & 0xffffff07;
    }
    if ((int)param_3 < 0x20) {
      uVar2 = uVar1 & 0xffffff07 | uVar2 << 3;
    }
    if ((int)param_3 < 0x21) {
      uVar1 = uVar2 & 0xfffffcff | param_2 << 8;
      if (param_1 == 0) {
        DAT_b800c000 = uVar1;
      }
      else {
        DAT_b800c400 = uVar1;
      }
      return 0;
    }
  }
  return 0xffffffff;
}

// ==== FUN_0012b388 @ 0012b388, 80 bytes
// called by: FUN_0012a4f0, FUN_0012b580
void FUN_0012b388(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)

{
  int iVar1;
  
  iVar1 = FUN_0012b724(param_1,0,param_2,param_4,param_3,param_4);
  if (iVar1 == 0) {
    FUN_0012b3d8(param_1,param_2,param_3,param_4,param_5,param_6);
  }
  return;
}

// ==== FUN_0012b3d8 @ 0012b3d8, 208 bytes
// called by: FUN_0012b388
int * FUN_0012b3d8(int *param_1,code *param_2,undefined4 param_3,undefined4 param_4,
                  undefined4 param_5,int param_6)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = DAT_00182404;
  piVar2 = (int *)0x0;
  if ((DAT_00182404 == (int *)0x0 || (int **)DAT_00182404 == &DAT_00182404) ||
     (param_2 == (code *)0x0 || param_2 == (code *)0xffffffff)) {
    DAT_001823fc._0_1_ = (char)DAT_001823fc + '\x01';
  }
  else {
    *(int *)(*DAT_00182404 + 4) = DAT_00182404[1];
    *(int *)piVar1[1] = *piVar1;
    piVar1[1] = (int)piVar1;
    *piVar1 = (int)piVar1;
    if ((int *)*param_1 == (int *)0x0 || (int *)*param_1 == param_1) {
      param_1[1] = (int)param_1;
      *param_1 = (int)param_1;
    }
    piVar1[2] = (int)param_2;
    if (param_6 == 0) {
      *(int **)(*param_1 + 4) = piVar1;
      *piVar1 = *param_1;
      *param_1 = (int)piVar1;
      piVar1[1] = (int)param_1;
    }
    else {
      *(int **)param_1[1] = piVar1;
      piVar1[1] = param_1[1];
      param_1[1] = (int)piVar1;
      *piVar1 = (int)param_1;
    }
    piVar1[3] = 2;
    (*param_2)(piVar1,1,param_3,param_4,param_5);
    piVar2 = piVar1;
  }
  return piVar2;
}

// ==== FUN_0012b4b0 @ 0012b4b0, 208 bytes
// called by: FUN_0012bb08, FUN_0012bac8
undefined4 FUN_0012b4b0(int param_1,uint param_2,uint param_3)

{
  if (param_1 != 0) {
    param_3 = *(uint *)(param_1 + 0xc);
  }
  if (param_1 == 0 || (param_3 & 2) == 0) {
    return 0;
  }
  if (((param_2 & 0x8000) == 0 && (param_3 & 4) == 0) && ((param_2 & 0xffff1fff) != 3)) {
    return 0;
  }
  switch(param_2 & 0xffff1fff) {
  case 3:
    param_3 = param_3 | 4;
    break;
  case 4:
  default:
switchD_0012b4e4_default:
    if ((param_3 & 8) != 0) {
      return 0;
    }
    goto LAB_0012b574;
  case 5:
    goto switchD_0012b4e4_default;
  case 6:
    param_3 = param_3 & 0xfffffffb;
    break;
  case 7:
    if ((param_3 & 8) != 0) {
      return 0;
    }
    param_3 = param_3 | 8;
    break;
  case 8:
    goto switchD_0012b4e4_default;
  case 9:
    if ((param_3 & 8) == 0) {
      return 0;
    }
    param_3 = param_3 & 0xfffffff7;
    break;
  case 10:
    goto switchD_0012b4e4_default;
  case 0xb:
    if ((param_3 & 0x10) != 0) {
      return 0;
    }
    param_3 = param_3 | 0x10;
    break;
  case 0xc:
    goto switchD_0012b4e4_default;
  case 0xd:
    if ((param_3 & 0x10) == 0) {
      return 0;
    }
    param_3 = param_3 & 0xffffffef;
  }
  *(uint *)(param_1 + 0xc) = param_3;
LAB_0012b574:
  return *(undefined4 *)(param_1 + 8);
}

// ==== FUN_0012b580 @ 0012b580, 140 bytes
// called by: usb_command
int FUN_0012b580(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                undefined4 param_5,undefined4 param_6)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 unaff_r4;
  undefined4 unaff_r5;
  undefined4 in_lr;
  bool bVar4;
  
  iVar1 = FUN_0012b388(param_1,param_2,param_3,param_4,param_5,param_6);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = *(uint *)(iVar1 + 0xc);
  }
  if (iVar1 != 0 && (uVar2 & 2) != 0) {
    bVar4 = (uVar2 & 4) == 0;
    if (bVar4) {
      *(uint *)(iVar1 + 0xc) = uVar2 | 4;
    }
    if (bVar4) {
      uVar3 = 3;
    }
    else {
      uVar3 = 4;
    }
    (**(code **)(iVar1 + 8))(iVar1,uVar3,param_3,param_4,param_5,unaff_r4,unaff_r5,in_lr);
  }
  return iVar1;
}

// ==== FUN_0012b5c0 @ 0012b5c0, 116 bytes
// called by: lcd_boot_proc
int FUN_0012b5c0(int *param_1,undefined4 param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  int local_1c;
  
  local_1c = *param_1;
  uVar1 = param_3;
  if (local_1c != 0) {
    uVar1 = *(uint *)(local_1c + 0xc);
  }
  if (local_1c != 0 && (uVar1 & 2) != 0) {
    uVar2 = param_3;
    if ((uVar1 & 4) != 0) {
      uVar2 = param_4;
      (**(code **)(local_1c + 8))(local_1c,6,param_2,param_3,param_4);
    }
    FUN_0012b634(&local_1c,param_2,param_3,param_4,uVar2);
    *param_1 = 0;
  }
  else {
    local_1c = 0;
  }
  return local_1c;
}

// ==== FUN_0012b634 @ 0012b634, 160 bytes
// called by: FUN_0012b8b0, FUN_0012b5c0
int * FUN_0012b634(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int *piVar2;
  
  piVar2 = (int *)*param_1;
  puVar1 = param_1;
  if (piVar2 != (int *)0x0) {
    puVar1 = (undefined4 *)(uint)*(byte *)(piVar2 + 3);
  }
  if (piVar2 != (int *)0x0 && ((uint)puVar1 & 2) != 0) {
    *(int *)(*piVar2 + 4) = piVar2[1];
    *(int *)piVar2[1] = *piVar2;
    piVar2[1] = (int)piVar2;
    *piVar2 = (int)piVar2;
    piVar2[3] = 0;
    FUN_0012c0fc(piVar2,0xffffffff,0,param_4,param_4);
    (*(code *)piVar2[2])(piVar2,2,param_2,param_3,param_4);
    *param_1 = 0;
    DAT_00182404[1] = (int)piVar2;
    *piVar2 = (int)DAT_00182404;
    DAT_00182404 = piVar2;
    piVar2[1] = (int)&DAT_00182404;
  }
  else {
    piVar2 = (int *)0x0;
  }
  return piVar2;
}

// ==== FUN_0012b724 @ 0012b724, 60 bytes
// called by: FUN_0012b388
int * FUN_0012b724(int *param_1,int *param_2,int param_3)

{
  int *piVar1;
  int in_r12;
  
  piVar1 = (int *)*param_1;
  if (piVar1 != (int *)0x0 && piVar1 != param_1) {
    for (; piVar1 != param_1; piVar1 = (int *)*piVar1) {
      if (param_2 != piVar1) {
        in_r12 = piVar1[2];
      }
      if (param_2 == piVar1 || in_r12 == param_3) {
        return piVar1;
      }
    }
  }
  return (int *)0x0;
}

// ==== FUN_0012b760 @ 0012b760, 108 bytes
// called by: FUN_0012a4f0
void FUN_0012b760(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  int *piVar2;
  
  param_1[1] = (int)param_1;
  *param_1 = (int)param_1;
  piVar1 = param_1;
  for (; piVar2 = (int *)((int)piVar1 + param_2), 1 < param_3; param_3 = param_3 + -1) {
    *(int **)(*piVar1 + 4) = piVar2;
    *piVar2 = *piVar1;
    *piVar1 = (int)piVar2;
    piVar2[1] = (int)piVar1;
    piVar1 = piVar2;
  }
  *(int ***)param_1[1] = &DAT_00182404;
  DAT_00182408 = param_1[1];
  param_1[1] = (int)&DAT_00182404;
  DAT_00182404 = param_1;
  return;
}

// ==== FUN_0012b7d0 @ 0012b7d0, 16 bytes
// called by: FUN_0012a4f0
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0012b7d0(int *param_1,int param_2)

{
  _DAT_00383134 = param_2 + (int)param_1;
  _DAT_00383130 = param_1;
  _DAT_00383138 = param_1;
  _DAT_0038313c = param_1;
  _DAT_00383144 = param_2;
  *param_1 = param_2;
  _DAT_00383148 = 0;
  _DAT_0038314c = _DAT_00383144;
  _DAT_00383140 = _DAT_00383134;
  return;
}

// ==== FUN_0012b7e4 @ 0012b7e4, 32 bytes
// called by: lcd_boot_proc, lcd_boot_restart, usb_command
void FUN_0012b7e4(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  FUN_0012b804(param_1,param_2 & 0xffff9fff,param_3,param_4,param_5,0,0);
  return;
}

// ==== FUN_0012b804 @ 0012b804, 168 bytes
// called by: FUN_0012b7e4
int * FUN_0012b804(int param_1,ushort param_2,undefined2 param_3,undefined1 *param_4,int param_5,
                  undefined1 *param_6,int param_7)

{
  int *piVar1;
  uint uVar2;
  int *piVar3;
  bool bVar4;
  
  if (param_1 == 0) {
    return (int *)0x0;
  }
  if ((param_2 & 0x8000) == 0) {
    uVar2 = (uint)*(byte *)(param_1 + 0xc);
    bVar4 = (*(byte *)(param_1 + 0xc) & 2) != 0;
    if (bVar4) {
      uVar2 = *(uint *)(param_1 + 8);
    }
    if (!bVar4 || uVar2 == 0) {
      return (int *)0x0;
    }
  }
  piVar1 = (int *)FUN_0012ab3c(&DAT_00383130,param_5 + param_7 + 8);
  if (piVar1 == (int *)0x0) {
    return (int *)0x0;
  }
  *piVar1 = param_1;
  *(ushort *)(piVar1 + 1) = param_2 & 0x7fff;
  *(undefined2 *)((int)piVar1 + 6) = param_3;
  piVar3 = piVar1 + 2;
  for (; 0 < param_5; param_5 = param_5 + -1) {
    *(undefined1 *)piVar3 = *param_4;
    piVar3 = (int *)((int)piVar3 + 1);
    param_4 = param_4 + 1;
  }
  for (; 0 < param_7; param_7 = param_7 + -1) {
    *(undefined1 *)piVar3 = *param_6;
    piVar3 = (int *)((int)piVar3 + 1);
    param_6 = param_6 + 1;
  }
  return piVar1;
}

// ==== FUN_0012b8b0 @ 0012b8b0, 444 bytes
// called by: FUN_0012a4f0
void FUN_0012b8b0(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  uint uVar4;
  ushort uVar5;
  int *piVar6;
  int *local_18;
  
  local_18 = param_4;
  iVar1 = FUN_0012aadc(&DAT_00383130,&local_18,0,param_4,param_2,param_3);
  if (iVar1 < 1) {
    return;
  }
  iVar1 = iVar1 + -8;
  if (iVar1 == 0) {
    piVar6 = (int *)0x0;
  }
  else {
    piVar6 = local_18 + 2;
  }
  if ((DAT_00182400 == (code *)0x0) ||
     (iVar2 = (*DAT_00182400)(*local_18,(short)local_18[1],*(undefined2 *)((int)local_18 + 6),piVar6
                              ,iVar1), iVar2 != 1)) {
    uVar5 = *(ushort *)(local_18 + 1) & 0x1fff;
    if (uVar5 == 3) {
      uVar4 = *(uint *)(*local_18 + 0xc);
      if ((uVar4 & 6) == 2) {
        *(uint *)(*local_18 + 0xc) = uVar4 | 4;
        (**(code **)(*local_18 + 8))(*local_18,3,*(undefined2 *)((int)local_18 + 6),piVar6,iVar1);
      }
    }
    else if (uVar5 == 6) {
      iVar2 = *local_18;
      if ((*(byte *)(iVar2 + 0xc) & 2) != 0) {
        (**(code **)(iVar2 + 8))(iVar2,6,*(undefined2 *)((int)local_18 + 6),piVar6,iVar1);
        FUN_0012b634(local_18,*(undefined2 *)((int)local_18 + 6),piVar6,iVar1);
      }
    }
    else {
      if (uVar5 == 0xb) {
        FUN_0012bb08(*local_18,0xb,0xb,piVar6,iVar1,0xffffffff);
        uVar5 = 0x400d;
        uVar3 = *(undefined2 *)((int)local_18 + 6);
        iVar2 = *local_18;
      }
      else {
        if (uVar5 == 0x13) {
          FUN_0012bef4(piVar6,1);
        }
        if ((*(ushort *)(local_18 + 1) & 0x4000) == 0) {
          if ((*(ushort *)(local_18 + 1) & 0x2000) == 0) {
            FUN_0012bac8(*local_18,uVar5,*(undefined2 *)((int)local_18 + 6),piVar6,iVar1);
            goto LAB_0012ba60;
          }
          uVar3 = *(undefined2 *)((int)local_18 + 6);
          iVar2 = *local_18;
        }
        else {
          uVar3 = *(undefined2 *)((int)local_18 + 6);
          iVar2 = *local_18;
          uVar5 = uVar5 | 0x4000;
        }
      }
      FUN_0012bb08(iVar2,uVar5,uVar3,piVar6,iVar1,0xffffffff);
    }
  }
LAB_0012ba60:
  FUN_0012aa5c(&DAT_00383130);
  return;
}

// ==== FUN_0012ba78 @ 0012ba78, 76 bytes
uint FUN_0012ba78(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = *(int *)(param_1 + 0xc);
  if (param_2 == 1) {
    return (uint)((*(byte *)(iVar1 + 0xc) & 0x1e) != 6);
  }
  uVar2 = (**(code **)(iVar1 + 8))
                    (iVar1,(&DAT_0012f2d4)[param_2],*(undefined1 *)(param_1 + 0x1a),param_1,0x20);
  return uVar2;
}

// ==== FUN_0012bac8 @ 0012bac8, 64 bytes
// called by: FUN_0012b8b0, usb_command
void FUN_0012bac8(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  code *pcVar1;
  
  pcVar1 = (code *)FUN_0012b4b0();
  if (pcVar1 != (code *)0x0) {
    (*pcVar1)(param_1,param_2 & 0xffff1fff,param_3,param_4,param_5);
  }
  return;
}

// ==== FUN_0012bb08 @ 0012bb08, 172 bytes
// called by: FUN_0010d558, FUN_0010d63c, usb_receive_task, FUN_0012a4f0, FUN_0012b8b0
int FUN_0012bb08(undefined4 param_1,uint param_2,undefined4 param_3,undefined4 param_4,
                undefined4 param_5,int param_6)

{
  code *pcVar1;
  int iVar2;
  code *pcVar3;
  int local_3c;
  int *local_38;
  int local_2c [2];
  
  if ((param_2 & 0x4000) == 0) {
    pcVar3 = FUN_0012bc98;
  }
  else {
    pcVar3 = FUN_0012bbbc;
  }
  iVar2 = 0;
  local_2c[0] = 0;
  do {
    (*pcVar3)(param_1,&local_3c,local_2c);
    if (local_2c[0] == 0) {
      return iVar2;
    }
    pcVar1 = (code *)FUN_0012b4b0(local_2c[0],param_2);
  } while ((pcVar1 == (code *)0x0) ||
          (iVar2 = (*pcVar1)(local_2c[0],param_2 & 0xffff1fff,param_3,param_4,param_5),
          param_6 != iVar2));
  *(int **)(local_3c + 4) = local_38;
  *local_38 = local_3c;
  return iVar2;
}

// ==== FUN_0012bbbc @ 0012bbbc, 220 bytes
// called by: FUN_0012bb08
int FUN_0012bbbc(int *param_1,int *param_2,int *param_3)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)*param_3;
  if (piVar1 == (int *)0x0) {
    param_2[2] = 0;
    param_2[1] = (int)param_2;
    *param_2 = (int)param_2;
    if ((int *)*param_1 == (int *)0x0 || (int *)*param_1 == param_1) {
      return 0;
    }
    *(int **)param_1[1] = param_2;
    param_2[1] = param_1[1];
    param_1[1] = (int)param_2;
    *param_2 = (int)param_1;
  }
  else if ((int *)param_2[1] == piVar1) {
    *(int *)(*piVar1 + 4) = piVar1[1];
    *(undefined4 *)((undefined4 *)*param_3)[1] = *(undefined4 *)*param_3;
    iVar2 = *param_3;
    *(int *)(iVar2 + 4) = iVar2;
    *(int *)*param_3 = iVar2;
    *(int *)(*param_2 + 4) = *param_3;
    *(int *)*param_3 = *param_2;
    *param_2 = *param_3;
    *(int **)(*param_3 + 4) = param_2;
  }
  piVar1 = (int *)param_2[1];
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

// ==== FUN_0012bc98 @ 0012bc98, 216 bytes
// called by: FUN_0012bb08
int FUN_0012bc98(int *param_1,int *param_2,int *param_3)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)*param_3;
  if (piVar1 == (int *)0x0) {
    param_2[2] = 0;
    param_2[1] = (int)param_2;
    *param_2 = (int)param_2;
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

// ==== FUN_0012bd70 @ 0012bd70, 36 bytes
// called by: config_load, lcd_boot_proc, idle_screen_proc, usb_receive_task, FUN_001284b8
void FUN_0012bd70(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined2 param_4,
                 undefined4 param_5)

{
  FUN_0012c3ac(param_1,param_2,FUN_0012ba78,param_3,param_4,param_5);
  return;
}

// ==== FUN_0012bd98 @ 0012bd98, 20 bytes
// called by: FUN_00117354
void FUN_0012bd98(void)

{
  DAT_00182414 = DAT_00182414 + 1;
  return;
}

// ==== FUN_0012bdb0 @ 0012bdb0, 144 bytes
// called by: FUN_0012c3ac
int FUN_0012bdb0(uint param_1,uint *param_2,int param_3)

{
  uint *puVar1;
  uint local_20;
  uint *puStack_1c;
  int iStack_18;
  int local_14;
  
  local_14 = 0;
  local_20 = param_1;
  puStack_1c = param_2;
  iStack_18 = param_3;
LAB_0012bdc8:
  do {
    do {
      FUN_0012c220(&DAT_00182428,&local_20,&local_14);
      if (local_14 == 0) goto LAB_0012be24;
    } while (*(int *)(local_14 + 8) == 0);
    if (param_1 != 0xffffffff) {
      puVar1 = *(uint **)(local_14 + 0xc);
      if (puVar1 != (uint *)param_1) goto LAB_0012bdc8;
      if (param_2 != (uint *)0xffffffff) {
        puVar1 = (uint *)(uint)*(byte *)(local_14 + 0x1a);
      }
      if (param_2 != (uint *)0xffffffff && puVar1 != param_2) goto LAB_0012bdc8;
      if (param_3 == 0) goto LAB_0012be24;
    }
    if (*(int *)(local_14 + 8) == param_3) {
LAB_0012be24:
      *(uint **)(local_20 + 4) = puStack_1c;
      *puStack_1c = local_20;
      return local_14;
    }
  } while( true );
}

// ==== FUN_0012be44 @ 0012be44, 12 bytes
// called by: config_load
undefined4 FUN_0012be44(void)

{
  return DAT_00182414;
}

// ==== FUN_0012be54 @ 0012be54, 28 bytes
// called by: FUN_0012bf3c, config_load
int FUN_0012be54(uint param_1)

{
  if (DAT_00182414 < param_1) {
    param_1 = ~param_1;
  }
  else {
    param_1 = -param_1;
  }
  return param_1 + DAT_00182414;
}

// ==== FUN_0012be74 @ 0012be74, 120 bytes
// called by: FUN_0012a4f0
void FUN_0012be74(int *param_1,int param_2,int param_3)

{
  int *piVar1;
  int *piVar2;
  
  DAT_0018242c = &DAT_00182428;
  DAT_00182428 = &DAT_00182428;
  param_1[1] = (int)param_1;
  *param_1 = (int)param_1;
  piVar1 = param_1;
  for (; piVar2 = (int *)((int)piVar1 + param_2), 1 < param_3; param_3 = param_3 + -1) {
    *(int **)(*piVar1 + 4) = piVar2;
    *piVar2 = *piVar1;
    *piVar1 = (int)piVar2;
    piVar2[1] = (int)piVar1;
    piVar1 = piVar2;
  }
  *(int ***)param_1[1] = &DAT_00182420;
  DAT_00182424 = param_1[1];
  param_1[1] = (int)&DAT_00182420;
  DAT_00182420 = param_1;
  return;
}

// ==== FUN_0012bef4 @ 0012bef4, 68 bytes
// called by: FUN_0012b8b0
void FUN_0012bef4(int *param_1,int param_2)

{
  if (param_2 != 0) {
    *DAT_0018242c = (int)param_1;
    param_1[1] = (int)DAT_0018242c;
    DAT_0018242c = param_1;
    *param_1 = (int)&DAT_00182428;
    return;
  }
  *(int **)((int)DAT_00182428 + 4) = param_1;
  *param_1 = (int)DAT_00182428;
  DAT_00182428 = param_1;
  param_1[1] = (int)&DAT_00182428;
  return;
}

// ==== FUN_0012bf3c @ 0012bf3c, 436 bytes
// called by: FUN_0012a4f0
undefined8 FUN_0012bf3c(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined1 *puVar6;
  int *local_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  
  puVar6 = &DAT_00182410;
  local_20 = param_1;
  uStack_1c = param_2;
  uStack_18 = param_3;
  uStack_14 = param_4;
  iVar3 = FUN_0012be54(DAT_0018241c);
  if (iVar3 != 0) {
    *(undefined4 *)(puVar6 + 0xc) = *(undefined4 *)(puVar6 + 4);
    local_20 = (int *)0x0;
    while (FUN_0012c220(&DAT_00182428,&uStack_1c,&local_20), local_20 != (int *)0x0) {
      if (((code *)local_20[2] != (code *)0x0) &&
         (iVar4 = (*(code *)local_20[2])(local_20,1), iVar4 == 0)) {
        iVar4 = 0;
        if (((*(byte *)((int)local_20 + 0x1b) & 0x20) != 0) &&
           (iVar5 = local_20[5], local_20[5] = iVar5 + iVar3,
           (uint)local_20[4] <= (uint)(iVar5 + iVar3))) {
          local_20[5] = 0;
          sVar2 = (short)local_20[6];
          if (sVar2 != 0) {
            if (sVar2 != -1) {
              *(short *)(local_20 + 6) = sVar2 + -1;
            }
            iVar4 = (*(code *)local_20[2])(local_20,4);
            if ((short)local_20[6] != 0) goto LAB_0012c00c;
          }
          iVar4 = -1;
        }
LAB_0012c00c:
        if ((*(byte *)((int)local_20 + 0x1b) & 0x40) != 0) {
          iVar4 = -1;
        }
        if ((*(byte *)((int)local_20 + 0x1b) & 0x10) != 0) {
          if (iVar4 == -1) {
            (*(code *)local_20[2])(local_20,5);
            *(byte *)((int)local_20 + 0x1b) = *(byte *)((int)local_20 + 0x1b) & 0xbf;
          }
          bVar1 = *(byte *)((int)local_20 + 0x1b);
          *(byte *)((int)local_20 + 0x1b) = bVar1 & 0xef;
          if ((bVar1 & 0x20) == 0) {
            iVar4 = (*(code *)local_20[2])(local_20,2);
            *(byte *)((int)local_20 + 0x1b) = *(byte *)((int)local_20 + 0x1b) | 0x20;
          }
          else {
            iVar4 = (*(code *)local_20[2])(local_20,3);
          }
        }
        if (iVar4 == -1) {
          (*(code *)local_20[2])(local_20,5);
          *(undefined1 *)((int)local_20 + 0x1b) = 0;
          *(int *)(*local_20 + 4) = local_20[1];
          *(int *)local_20[1] = *local_20;
          local_20[1] = (int)local_20;
          *local_20 = (int)local_20;
          *DAT_00182424 = (int)local_20;
          local_20[1] = (int)DAT_00182424;
          DAT_00182424 = local_20;
          *local_20 = (int)&DAT_00182420;
        }
      }
    }
  }
  return CONCAT44(uStack_1c,local_20);
}

