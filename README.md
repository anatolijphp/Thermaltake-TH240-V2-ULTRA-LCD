# TT LCD Studio

Replacement software for the 2.1" round LCD on the **Thermaltake TH240 V2 Ultra ARGB (CL-W383)** AIO cooler, built by
reverse engineering TT RGB Plus. It doesn't need TT RGB Plus.

## Features

- **Display modes:** Wallpaper Engine (through a bundled 480×480 virtual monitor), screen mirror, image, GIF,
  video (≤30 s, crop/zoom), colour, clock, Aura Sync, and Off.
- **Clock / readings face:** time, date, water temperature (from the cooler's own sensor), CPU temperature and
  CPU load.
  - Arrange the readings into lines by drag and drop.
  - Style each reading separately: font, size, effect, colour, label beside or above, °C/°F, colour by value.
  - Everything is fitted inside the round screen.
- **Overlay** of time and temperatures on top of any mode.
- **Aura Sync:** the screen appears in ASUS Armoury Crate as an external device, and its effects drive a
  MilkDrop-style visualiser.
- **Dashboard** with live gauges.
- **Game mode:** detects games and lowers the frame rate and sensor polling, or hands the screen back to the
  cooler's idle picture.
- **Idle picture:** set the picture the cooler shows by itself, with crop, upload and verify.
- **Device settings:**
  - brightness
  - one rotation setting for the live picture and the idle screen
  - sleep
  - tray icon and start with Windows
- **CPU temperature:** a one-time Windows service that reads it through TOWEI's sensor driver.

## Install

Download `TTLcdStudio-Setup.exe` from the latest **Release**, or from the latest run on the **Actions** tab.
- **No administrator rights needed:** it installs per user to `%LOCALAPPDATA%\Programs\TT LCD Studio`.
- **Shortcuts and autostart:** it adds a Start menu shortcut and can start the app with Windows.
- **Optional parts, which ask for administrator rights:**
  - the CPU temperature service
  - the Armoury Crate plugin
  - the virtual display driver
- **Uninstall:** from Apps & features. Your settings in `%APPDATA%\ttlcd` are kept.

There's also a portable zip.

Silent install: `TTLcdStudio-Setup.exe /S [/D=<folder>]`.

## Build

You only need Windows. `build.cmd` uses the .NET Framework 4.8 C# compiler that ships with Windows. No SDK and no
packages.

```
build.cmd
```

Sources stay where they are. Everything built goes to `build\<os>\`; Windows is the only target so far
(`build\windows\`, a Linux build will get `build\linux\`).

| Output (`build\windows\`) | What |
|---|---|
| `app\` | The app, laid out exactly as installed. Run `app\TTLcdStudio.exe` straight from here. |
| `app\ttlcd.exe` | CLI. It also installs and uninstalls the driver, the Aura plugin and the sensor service. |
| `app\TTLcdStudio.exe` | App + tray |
| `app\TTLcdAura.dll` | Armoury Crate / Aura Sync plugin |
| `app\driver\` | Virtual display driver (copied from `driver\`) |
| `app\firmware\` | Firmware, `ttrescue.exe` and `PatchPanel.exe` (see `firmware/README.md`) |
| `TTLcdStudio-Setup.exe` | Installer that carries `app\` (without `PatchPanel.exe`) |

GitHub Actions (`.github/workflows/build.yml`) does the following on every push:
1. Runs `build.cmd`.
2. Checks the firmware checksums.
3. Smoke-tests a silent install and uninstall.
4. Uploads the installer and a portable zip.

A `v*` tag also creates a release.

## Repository

| Path | What |
|---|---|
| `build.cmd` | Builds everything into `build\windows\` |
| `src/` | App, CLI and protocol (C# 5, WPF) |
| `aura/` | Armoury Crate HAL plugin |
| `installer/` | Installer source |
| `driver/` | Bundled MttVDD virtual display driver (signed, MIT) |
| `firmware/` | **Firmware for the LCD controller and how to burn it** (`firmware/README.md`) |
| `rescue/` | Sources of the firmware tools |
| `build/` | Build output only, one folder per OS (not in git) |
| `docs/` | Protocol notes, including the controller's config record |
| `re/scripts/` | Ghidra scripts used for the reverse engineering |
| `tools-src/` | Icon generator |

## CLI

Run `ttlcd` with no arguments for the full list. Common commands:

```
ttlcd info                       serial, type, rotation
ttlcd temp                       water temperature from the cooler's sensor
ttlcd image <file>               show a picture
ttlcd stream [-m N]              mirror a monitor
ttlcd service install            CPU temperature service (admin, one time)
ttlcd display install            virtual 480×480 monitor for Wallpaper Engine (admin)
ttlcd aura install               Armoury Crate / Aura Sync plugin (admin)
```

## Warnings

- On the **original Thermaltake firmware**, never use the on-device storage uploads (protocol commands `0x0A`/`0x0B`).
  One upload erased the LCD controller's flash. The app refuses them on that firmware.
- To restore a bricked LCD controller, see `firmware/README.md`. It needs the pump head opened.
- Not affiliated with Thermaltake, ASUS, TCOMAS or Nuvoton.
