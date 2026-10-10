# Cooler LCD firmware (TH240 V2 Ultra / CL-W383)

The LCD in the pump head has its own controller (Nuvoton **W55FA93 / N32905**, ARM926) that runs from a 16 MB SPI NOR
flash on the board. This folder has the firmware that runs on our cooler today, and everything needed to write it.

| File | What |
|---|---|
| `lx800_tt021panel.bin` | **Our firmware, the one to burn.** The LX800 image with Thermaltake's TT021 ST7701S panel init patched in. |
| `make_fix.bin` | Base image: the LX800 rescue firmware (same chip and OEM, Shenzhen Xinyao). Its panel init doesn't match this screen (the backlight comes on but the image stays black). |
| `FA93_musb.bin` | Nuvoton's flash-writer helper, loaded into the chip's RAM by the burner. From Nuvoton's open-source TurboWriter ([OpenNuvoton/N32905_NonOS_Tool](https://github.com/OpenNuvoton/N32905_NonOS_Tool)). |
| `hardware_boot_pins.jpg` | Where to short the flash pins to force the chip into ROM recovery mode. |
| `SHA256SUMS.txt` | Checksums of the files above. |

Tools (built by `build.cmd` from `rescue/` into `build\windows\app\firmware\`; the installer puts them in the installed
`firmware\` folder):
- `ttrescue.exe`: reads, writes and verifies the flash through the chip's recovery mode. Source: `rescue/Rescue.cs`.
- `PatchPanel.exe` (build folder only): rebuilds `lx800_tt021panel.bin` from `make_fix.bin`. Source: `rescue/PatchPanel.cs`.

## When do you need this?

Only when the screen is bricked: it doesn't light up **and** Windows doesn't list the LCD at all (no `USB 1A2C:151D`
or `264A:233C` in Device Manager). Pump and fans are on separate electronics and keep working either way.

As long as the LCD enumerates, TT LCD Studio drives it and nothing here is needed.

> ⚠️ Writing firmware can leave the screen unusable and needs the pump head opened (warranty). Read every step first.

## What's different from the stock firmware

- The original Thermaltake firmware (`2.1inch-update-v42.bin` from TT RGB Plus) is encrypted with a key that lived in
  the on-device bootloader, so it can't be rebuilt. On our unit that flash was erased (see `CLAUDE.md`).
- `lx800_tt021panel.bin` = `make_fix.bin` with one function replaced: the ST7701S panel init (`FUN_0010bd00`) now sends
  Thermaltake's TT021 sequence (from `TT021-00-0_FW_V1.0.5.4`, `FUN_000097e8`). Everything else is byte-identical.
- The LX800 firmware enumerates as `USB 1A2C:151D` and speaks the same protocol. TT LCD Studio handles its quirks
  automatically:
  - red/blue are swapped on the panel, so frames are colour-corrected before sending
  - it falls back to its own idle screen when frames stop
  - the idle picture is `c:\syst\wapper.jpg`

## Burning it (about 5 minutes)

You need a Windows PC, a pair of tweezers (or a wire), and the cooler's LCD USB cable.

1. **Unplug the LCD's USB cable** (or power the PC off).
2. **Force ROM recovery mode.** Open the pump head and find the 8-pin SPI NOR chip on the LCD board. Orient the board
   like `hardware_boot_pins.jpg` and **short the chip's top-left and bottom-right pins**. Keep them shorted while you
   plug USB back in.
   - Device Manager now shows a USB mass-storage device **`0416:9396`** ("W55FA93 Dev Tool Device"). You can release
     the short.
   - Check the pins against your board first. **Never short VCC to GND.**
3. Open a command prompt in the folder that has `ttrescue.exe` (the installed app's `firmware\` folder, or
   `build\windows\app\firmware\` after `build.cmd`) and run:
   ```
   ttrescue.exe write lx800_tt021panel.bin
   ```
   It uploads `FA93_musb.bin` into the chip's RAM, writes the image to flash offset 0, then reads it back and compares.
   Expect `verify OK: 589824 bytes match`. Everything is logged to `rescue.log` next to `ttrescue.exe`.
4. Unplug and replug the USB cable (no short this time). The LCD enumerates as **`1A2C:151D`** and lights up.
5. Start TT LCD Studio.
   - The boot animation area is empty after a rescue. That's normal.
   - Set an idle picture from the preview's **Idle** view if you like.

### Making a backup first (recommended)

In recovery mode, `ttrescue.exe flash_dump.bin` reads the whole 16 MB flash into `flash_dump.bin` without
writing anything.

### Alternative: Nuvoton TurboWriter

Nuvoton's own **TurboWriter V2.30.003_N329x1** (OpenNuvoton) does the same job:
1. Add `A14018    16384` to `SPIFLASH ID.ini`, so it knows this NOR chip.
2. Choose **SPI (Raw Data)** and pick `lx800_tt021panel.bin`.
3. Click **Burn**. Don't let it add a header.

## Rebuilding the image

From the repository root, after `build.cmd`:
```
build\windows\app\firmware\PatchPanel.exe firmware\make_fix.bin firmware\lx800_tt021panel.bin
```
It checks the code layout before patching and prints how many bytes changed. Compare the result with `SHA256SUMS.txt`.

## Credits and licences

- LX800 base firmware: from the TCOMAS LX800 rescue kit (Shenzhen Xinyao / TCOMAS). Its README states that the image,
  including the hard-coded default serial `A247392SS000000`, may be redistributed.
- `FA93_musb.bin` and TurboWriter: © Nuvoton, from OpenNuvoton.
- TT021 panel sequence: Thermaltake firmware.
- This project is not affiliated with Thermaltake, TCOMAS or Nuvoton.
