# Thermaltake CL-W383 AIO LCD protocol (USB 264A:233C)

Recovered from TT RGB PLUS 3.0.3 (`TTRGBPlus.exe`, class `Device_Toughliquid_CPU_HKC`) with Ghidra headless decompilation.
Functions: `ProtocolWrite` = `FUN_140165fc0`, `ProtocolRead` = `FUN_140165c00`, `findDevice` = `FUN_140168940`,
`set_screen_brightness` = `FUN_14016c3e0`, `run` = `FUN_14016af10`, live frame push in `slot_Lighting` = `FUN_14016d490`.

## Device
Composite USB device, two vendor HID interfaces (usage page FF06, usage 01). TT RGB Plus talks to them through a statically
linked libusb (interrupt transfers); on Windows plain HID `WriteFile`/`ReadFile` with report ID 0 does the same.

| Interface | Role | Packet (no report ID) | Payload per packet | libusb EP out / in |
|---|---|---|---|---|
| MI_00 | control / queries | 440 bytes | 436 (0x1B4) | 0x01 / 0x82 |
| MI_01 | LCD image data (cmd 8 only) | 1024 bytes | 1020 (0x3FC) | 0x03 / 0x84 |

Windows HID report lengths: MI_00 in/out 441, MI_01 out 1025 / in 17 (1 report-ID byte + packet).

## Packet framing (ProtocolWrite)
Every packet starts with a 32-bit little-endian header, followed by payload, zero-padded to the full packet size.

```
header = (first ? 0x80000000 : 0) | ((n & 0x7FFFFF) << 8) | cmd
first packet: n = total packet count = max(1, ceil(len / payload))
next packets: n = sequence number 1, 2, 3, ...
```
Byte view: `[cmd] [n lo] [n mid] [n hi | 0x80 if first]`.

A command with no data is a single packet with header `0x80000100 | cmd` and zero payload.

## Replies (ProtocolRead)
Read one packet from the same interface (MI_00: 440 bytes, MI_01: 16 bytes). Byte 0 must equal the command; for a first
packet (bit 31 set) the data starts at offset 4. Up to 10 non-matching packets are skipped.

## Commands
Interface 0 unless noted. Only the ones marked **used** are implemented by ttlcd.

| Cmd | Dir | Data | Meaning |
|---|---|---|---|
| 0x08 | write, **MI_01** | JPEG bytes | **used** – show live frame. 480x480 JPEG (Qt default quality ~75). No reply. |
| 0x12 | write | 1 byte 0-100 | **used** – screen brightness |
| 0x11 | write | 1 byte 0-3 | **used** – stored rotation: 0=0°, 1=270°, 2=180°, 3=90°. Turns the firmware's own idle/boot screens only, **not** cmd 0x08 frames (tested 2026-10-10) |
| 0x81 | query | → 1 byte | **used** – read the stored rotation (same encoding) |
| 0x84 | query | → 1 byte | read device "type" (3, 8-11, 12 valid) |
| 0x85 | query | → 16 bytes | read serial number (ASCII) |
| 0x87 | query | → 4 bytes | version (not used) |
| 0x82 | query | → uint32 | sensor value /10 (shown as temperature) |
| 0x16 | write | uint32 | text colour 0xRRGGBB for built-in screens |
| 0x18 | write | 1 byte (0 / 2) | built-in screen option |
| 0x1A | write | 1 byte | temperature scale (°C/°F) |
| 0x1B | write | uint32 | set device type — **do not use** |
| 0x02/0x03/0x0A/0x0B/0x13/0x14 | file ops | | boot-animation / standby file upload & delete — **not used** |
| 0xFF | write | | **firmware update — never send** |

## Config record in the cooler's flash (LX800 firmware)
The firmware keeps its settings in one 0x114-byte record in its flash store (decompiled: `re/out/lx_cfg.c` `FUN_00127d00`
loads it at boot; `lx_85.c` holds the command handlers; `lx_idle.c` `FUN_00127efc` = defaults, `FUN_00127f94` = save).
A command that changes it sets a "dirty" flag. A timer then saves the whole record about 300 ticks later, so values
survive power cycles. Write such commands only when the value really changes.

| Offset | Set by | Read by | Meaning (default) |
|---|---|---|---|
| 0 | 0x12 (low 7 bits), 0x10 (bit 0x80) | | brightness 0-100 (100); bit 0x80 = backlight off |
| 1 | 0x11 | 0x81 | rotation code 0-3 (0); applied at boot via `FUN_00112c90(table[code & 3])` |
| 2 | 0x14 | | boot animation frame interval (0x14) |
| 3 | | | boot file path ("c:\syst\boot") |
| 0x7B | | | idle picture path ("c:\syst\wapper.jpg") |
| 0xF8 / 0xFC-0x10C | 0x16 / 0x17-0x1D | | colours / options of the built-in screens |
| 0x110 | 0x1A | 0x83 | temperature scale |
| 0x111 | 0x1E | | idle state: 0x02 = dim after idle (sleep), 0x00 = stay on |

## Live streaming
TT RGB Plus renders a 480x480 QImage, rotates it in software by the selected angle, saves as JPEG and calls
`ProtocolWrite(8, jpeg, len)`. A ~30 KB JPEG = ~30 packets of 1024 bytes. No handshake or mode switch is required
beyond opening interface 1. TT RGB Plus must not be running at the same time (it holds the device).
