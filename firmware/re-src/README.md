# LX800 firmware, decompiled

Ghidra's decompilation of the firmware this project ships and the cooler runs,
[`firmware/lx800_tt021panel.bin`](../lx800_tt021panel.bin): the LX800 rescue image (Nuvoton W55FA93 / N9H20 LCD
controller, board by Shenzhen Xinyao Technology) with Thermaltake's TT021 ST7701S panel init patched in. It is
pseudo-C for reading and searching, not source that compiles.

| File | What |
|---|---|
| `code/<address>.c` | Every function, decompiled, in 16 KB address windows (`00128000.c` = 0x128000-0x12bfff), each with its size and callers |
| `functions.csv` | All 779 functions: address, name, size, callers, callees, file |
| `strings.txt` | The firmware's texts (debug messages, paths, the serial number) with address and file offset |
| `names.txt` | Names given to functions and data, with where each comes from (`?` = likely, not proven). Edit it and regenerate |

## Regenerate

```
extras/re/decompile-fw.sh
```

Linux host, nothing installed: Ghidra 12.1.4 from `tools/ghidra_12.1.4_PUBLIC` and a Linux JDK 21 in
`tools/jdk21-linux` (Eclipse Temurin 21.0.12.1+1, the archive and its SHA256 are in the script's header). The script
`extras/re/scripts/DecompileFirmware.java`:
- imports the image raw (ARM v5t little-endian at 0xFFFE0, so the code starts at 0x100000);
- adds the address ranges the code uses outside the image (the rest of SDRAM up to 0x2000000, peripheral registers
  at 0xB0000000);
- makes the code area (up to 0x130000) read-only, so loads of addresses from literal pools show as the globals they
  point to;
- turns every ARM `push {..., lr}` into a function, then runs Ghidra's analysis;
- defines the texts as strings, applies `names.txt` and the settings record's type (`lx_config` on `g_config`);
- decompiles every function into this folder.

The Ghidra project is kept in `extras/re/proj/lx800full` (not in git), so it can be opened in the Ghidra UI.

## Memory map

| Address | File offset | What |
|---|---|---|
| 0x0FFFE0-0x0FFFFF | 0x00000 | Image header |
| 0x100000-0x12FFFF | 0x00020 | Code (ARM, some Thumb library code reached with `blx`), literal pools, texts |
| 0x130000-0x17FFFF | 0x30020 | Data: the two bitmap fonts' glyphs, pictures |
| 0x16A440 / 0x17FE9C | 0x6A460 / 0x7FEBC | Glyph tables of the big (100 px) and small (60 px) font |
| 0x180000-0x18FFFF | 0x80020 | Font range lists, then variables (`g_screen`, `g_idle_fonts`, `g_coolant`, ...), mostly zero in the file |
| 0x7BC9A8 | | `g_config`, the 0x114-byte settings record (see `lx_config`) |
| 0xB0000000- | | Peripheral registers |

## What is known

- **Screens are drawn with SEGGER emWin** (V5.48, `strings.txt`); `text_draw`, `text_width`, `font_select` and
  `text_set_colour` are its text calls as the idle screen uses them.
- **USB commands**: `usb_command` is the switch over the command byte (docs/protocol.md lists the commands).
- **Settings record** (`g_config`, type `lx_config`): loaded by `config_load`, defaults in `config_defaults`, saved by
  `config_save`; field meanings in docs/protocol.md.
- **Idle screen** (`idle_screen_proc`, message 0x15): draws the stored picture centred, then the coolant temperature
  (`g_coolant` in 1/100 °C, truncated) with the big font at (`idle_text_x` + centring, `idle_text_y`), "°" with the
  small font 3 px lower and "C "/"F " after it. `src/core/src/idle_text.rs` copies this, with the glyphs taken from
  the font tables above.

Names without a source in `names.txt` are Ghidra's (`FUN_<address>`, `DAT_<address>`).
