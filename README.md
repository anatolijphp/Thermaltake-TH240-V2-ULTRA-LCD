<div align="center">

# TT LCD Studio

**Open replacement software for the round 2.1" LCD on the Thermaltake TH240 V2 Ultra ARGB (CL-W383) AIO cooler.**

No TT RGB Plus, no frameworks to install, no telemetry. One small app for Windows and Ubuntu, built by reverse
engineering the cooler's USB protocol.

[![build](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions/workflows/build.yml/badge.svg)](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions/workflows/build.yml)
[![release](https://img.shields.io/github/v/release/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD?include_prereleases&label=release)](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/releases)
![platform](https://img.shields.io/badge/platform-Windows%20%7C%20Ubuntu-0078d4)
![Rust](https://img.shields.io/badge/Rust-1.98%20(bundled%20toolchain)-b7410e)

<img src="docs/media/dashboard.gif" alt="A tour of TT LCD Studio: dashboard, display modes, device and settings, with the live cooler preview" width="860">

</div>

## Highlights

|  |  |
|---|---|
| 🖥️ **Nine display modes** | Wallpaper Engine (via a bundled 480×480 virtual monitor), screen mirror, image, GIF, video, colour, clock, Aura Sync, off |
| 🕒 **Clock and sensor face** | Time, date, coolant temperature from the cooler's own sensor, CPU temperature and load. Drag readings into lines, style each one |
| 🌈 **Aura Sync** | Shows up in ASUS Armoury Crate as a device; its effects drive a MilkDrop-style visualiser on the LCD |
| 📊 **Dashboard** | Live gauges for coolant, CPU temperature, CPU load and RAM |
| 🎮 **Game mode** | Detects games and lowers the frame rate and sensor polling, or hands the screen back to the cooler's idle picture |
| 🖼️ **Idle picture** | Crop, compress, upload and verify the picture the cooler shows on its own |
| 🔄 **One rotation setting** | Turns the live picture and the cooler's own idle and boot screens together |
| 🪶 **Light** | A few MB of native code, no runtimes to install: on Windows it uses only what ships with Windows (WebView2 included) |

## Screens

### Pick a mode, see it live

One click switches what the cooler shows: your **Wallpaper Engine** wallpaper (on a virtual 480×480 monitor),
**Aura Sync** effects from Armoury Crate, a looping **GIF**, and more. The preview on the right is exactly what the
cooler shows, rotated the way the pump is mounted.

<img src="docs/media/modes.gif" alt="Switching between the Wallpaper Engine, Aura Sync and GIF modes" width="860">

### Style every reading

Any mode can carry an overlay of time and temperatures, and the Clock mode is made of them. Arrange readings into lines
by drag and drop, then style each one on its own: font, size, effect (shadow, outline, neon glow), colour, label beside
or above, °C/°F and colour-by-value. Everything is kept inside the round screen automatically.

<img src="docs/media/style.gif" alt="Aura Sync mode: switching on the time overlay, making it bigger and giving it a neon glow" width="860">

### Game mode

Streaming 30 frames a second to the cooler costs a little CPU and USB time. Game mode gives that back while you play:

- **Detects games by itself**, using only what Windows already knows:
  - full-screen Direct3D games
  - games in the Xbox Game Bar list, matched against running processes
  - optionally, any window that covers its whole monitor (borderless games)
  - your own lists of apps to include or ignore
- **While a game runs**, pick one:
  - **Lower frame rate**: keep the screen alive at a frame rate you choose (default 5 fps)
  - **Show idle screen**: stop streaming completely, so the cooler shows its own idle picture
  - **Keep going**: change nothing
- **Slower sensor polling** while gaming (default every 10 s) for the coolant and CPU temperature.
- **Force it on** any time with the gamepad button in the title bar or the switch in Settings, for example for a game
  that isn't detected. The title bar shows when game mode is on.

<img src="docs/media/game.gif" alt="Settings: turning on game detection and forcing game mode on, the frame rate drops to 5 fps" width="860">

## Install

1. Download **`TTLcdStudio-Setup.exe`** from the [latest release](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/releases)
   (or the newest run on the [Actions](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions) tab).
2. Run it. It installs per user to `%LOCALAPPDATA%\Programs\TT LCD Studio`, **no administrator rights needed**, and
   can add a Start menu shortcut and start with Windows.
3. Optional parts ask for administrator rights when you pick them:
   - **CPU temperature service** (reads the CPU temperature through TOWEI's sensor driver, which must be installed)
   - **Armoury Crate plugin** (Aura Sync)
   - **Virtual display driver** (for Wallpaper Engine)

Prefer no installer? Grab **`TTLcdStudio-portable.zip`** instead and run `TTLcdStudio.exe` from anywhere.

<details>
<summary>Silent install and uninstall</summary>

```
TTLcdStudio-Setup.exe /S [/D=<folder>]
Uninstall.exe /uninstall /S [/all]
```

Uninstalling keeps your settings in `%APPDATA%\ttlcd`. The shared Aura plugin and display driver are removed only with
`/all` or when you confirm it.
</details>

> **Close TT RGB Plus first.** Only one program can drive the LCD at a time.

### Ubuntu / Linux

TT LCD Studio runs on Ubuntu too: the same window (the same UI, in the system's WebKitGTK) over the same engine and settings.

<img src="docs/media/ubuntu.gif" alt="TT LCD Studio on Ubuntu: dashboard, display modes (Clock, Colour, Aura Sync), device and settings pages with the live preview" width="860">

```
sudo apt install ./ttlcd-studio_2.0.0_amd64.deb   # from the release, or open the file in the App Center
ttlcd-studio                                       # or "TT LCD Studio" in the app menu
```

`ttlcd-studio` is the only file you need: it includes the `ttlcd` command and background service too (and
replaces the `ttlcd` package if that is installed). Without the window, `ttlcd_<version>_amd64.deb` alone has just
the command and the service, and needs nothing but the C library. Both are native programs; the window uses the
system's GTK 3 and WebKitGTK 4.1 (part of any current Ubuntu desktop, 22.04 to 26.04).

- **Everything Linux can do, in the window:**
  - dashboard
  - Image, GIF, Colour, Clock, Aura Sync and Off modes with the overlay
  - drag-and-drop lines and per-reading styles
  - device and streaming settings
  - the idle picture
  - tray icon (AppIndicator) and start with the desktop session
- **CPU temperature:** read straight from the kernel's sensor driver (k10temp, zenpower, coretemp); nothing to install.
- **Aura Sync on Linux:** needs [armory-crate](https://github.com/anatolijphp/armory-crate-alternative-ubuntu), a separate Linux replacement for Armoury Crate.
  ttlcd installs no plugin or service of its own for this; follow armory-crate's instructions to set it up for
  ttlcd. Until armory-crate is installed, the Aura Sync mode is greyed out. It becomes available on its own once
  armory-crate is there, without restarting Studio.
- **Windows-only for now:** Wallpaper Engine, screen mirroring, video and automatic game detection.
  Game mode can still be switched on by hand.
- **Cooler access without root:** the package adds a udev rule (replug the cooler or reboot once after installing).

<details>
<summary>Without the window: the command line and a background service</summary>

```
systemctl --user enable --now ttlcd       # drive the screen, also after every login
ttlcd set Mode Clock                      # or Image / Gif / Color / Aura / Off; the service picks it up
ttlcd set GifPath ~/Pictures/loop.gif
ttlcd config                              # all settings (~/.config/ttlcd/config.xml)
```

Studio stops this service while its window is open, so the two never drive the cooler at the same time. Other
distributions: unpack `ttlcd-<version>-linux-x86_64.tar.gz` to `/` (`sudo tar -xzf ... -C /`); the window needs
GTK 3 and WebKitGTK 4.1 from the distribution.
</details>

## Build from source

The app is written in Rust (`src/`), its window UI in React + Sass. The build fetches its own toolchain into `tools/`
(Rust 1.98.0, llvm-mingw and Node.js 24 LTS, pinned by version and SHA256, `src/fetch.sh` / `src\fetch.cmd`; npm
packages are pinned by `src/studio/ui/package-lock.json`), so there is nothing to install: no rustup, no Visual Studio,
no system Node.

```
git clone https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD.git
cd Thermaltake-TH240-V2-ULTRA-LCD
build.cmd                      # Windows: build\windows\
./build.sh [all|linux|windows] # Linux: build/linux/ and, cross-linked with llvm-mingw, build/windows/
```

To try the new build without installing it:
```
run.cmd                        # Windows: Studio from build\windows\app
run.cmd ttlcd info             # Windows: the command-line tool
./run.sh                       # Linux: Studio
./run.sh ttlcd info            # Linux: the command-line tool
```
Both build the current sources first (only the binaries, no installer or packages; quick when nothing changed).
Studio takes over the cooler while it runs (on Linux it stops the `ttlcd` user service); close an installed Studio
first.

On Linux the build needs `curl`, `xz`, `dpkg` and the GTK 3 / WebKitGTK 4.1 runtime libraries a desktop already has
(no `-dev` packages: it links through generated pkg-config stubs). Without a C compiler it fetches Zig as the linker.
The version comes from `src/Cargo.toml` (`[workspace.package]`).

| `build\windows\` | What |
|---|---|
| `app\TTLcdStudio.exe` | The app (window + tray). `app\` has the installed layout and runs as is |
| `app\ttlcd.exe` | Command-line tool; also installs the driver, the Aura plugin and the sensor service |
| `app\TTLcdAura.dll`, `app\TTLcdAura64.dll` | Armoury Crate / Aura Sync plugin (32-bit, as LightingService loads it; 64-bit) |
| `app\driver\`, `app\firmware\` | Virtual display driver; LCD firmware and rescue tools |
| `TTLcdStudio-Setup.exe` | The installer |

| `build/linux/` | What |
|---|---|
| `ttlcd_<version>_amd64.deb` | `ttlcd` (CLI + service), udev rule, systemd user service |
| `ttlcd-studio_<version>_amd64.deb` | `ttlcd-studio` (the window), menu entry, plus everything in `ttlcd` (replaces it) |
| `ttlcd-<version>-linux-x86_64.tar.gz` | The same files for other distributions |
| `pkg/`, `pkg-studio/` | The installed layouts |

Every push runs [GitHub Actions](.github/workflows/build.yml) on Windows and Ubuntu with the same bundled toolchain:
build, firmware checksums, a check that `PatchPanel` rebuilds the shipped firmware byte for byte, the Aura plugin
self-test, a silent install and uninstall, an install of both `.deb`s into a clean Ubuntu container that renders
frames, and screenshots of every Studio page on both systems. Tags `v*` publish a release with the files for both.

## Command line

```
ttlcd info                  serial, type, rotation
ttlcd temp                  coolant temperature from the cooler's sensor
ttlcd image <file>          show a picture
ttlcd stream [-m N]         mirror a monitor
ttlcd service install       CPU temperature service (admin, one time)
ttlcd display install       virtual 480×480 monitor for Wallpaper Engine (admin)
ttlcd aura install          Armoury Crate / Aura Sync plugin (admin)
ttlcd run                   drive the screen from the settings (the Linux service runs this)
ttlcd set <name> <value>    change a setting, e.g. ttlcd set Mode Clock
ttlcd render <out.jpg>      render one frame of the current mode, no cooler needed
```

Run `ttlcd` without arguments for the full list.

## How it works

The LCD has its own controller (Nuvoton W55FA93) behind a USB HID interface: one endpoint for control commands
(brightness, rotation, sensor queries) and one for 480×480 JPEG frames. Studio renders each frame (mode, overlay,
rotation), encodes it and streams it at up to 30 fps. The protocol is documented in [`docs/protocol.md`](docs/protocol.md).

## Project layout

```
ttlcd/
├── build.sh, build.cmd     Build everything (Linux host / Windows host); they call src/build.*
├── run.sh, run.cmd         Run the new build from build/ without installing
├── README.md, CLAUDE.md    This file; developer handover notes (hardware state, protocol, layout)
│
├── src/                    The app (Rust, one cargo workspace)
│   ├── Cargo.toml/.lock    Workspace, version, pinned dependencies
│   ├── .cargo/config.toml  Output folder and Windows linker settings (llvm-mingw, static CRT)
│   ├── fetch.sh, fetch.cmd Download the pinned toolchain into tools/ (SHA256-checked)
│   ├── build.sh, build.cmd The build: binaries, Windows app folder and installer, Linux packages
│   ├── app.ico             App icon (embedded into the Windows exes)
│   ├── core/               Shared library, no UI:
│   │   ├── hid/              USB HID access (Linux hidraw, Windows SetupAPI)
│   │   ├── lcd.rs            Cooler protocol: frames, brightness, rotation, queries, idle picture
│   │   ├── engine.rs         Render / send / control / game-detection threads
│   │   ├── render/           Frames: text from system fonts, round-panel fitting, effects
│   │   ├── config.rs         Settings (config.xml)
│   │   ├── model.rs, idle.rs What Studio shows: texts, panels, rotation, line editor, idle picture
│   │   ├── aura.rs           Aura Sync state reader and visualiser
│   │   └── win/              Windows only: screen capture, video, game detection, virtual display,
│   │                         Aura plugin install, CPU temperature service
│   ├── cli/                ttlcd: command line and Linux screen service (ttlcd run)
│   ├── studio/             TT LCD Studio: window and tray
│   │   ├── ui/               The interface, the same on both systems: React components, each a folder with
│   │   │   │                 Name.jsx + Name.scss (Sass), bundled by esbuild (build.mjs) during the cargo build
│   │   │   ├── src/components/ Reusable parts (title bar, nav, slider, switch, segmented toggle, card, gauge, ...)
│   │   │   ├── src/pages/      Dashboard, Display modes (tiles, line editor, video, text style), Device, Settings
│   │   │   ├── src/preview/    The round live / idle preview, drag-and-drop target
│   │   │   └── src/bridge.js   Messages to and from the Rust side
│   │   └── src/os/           System-specific parts (linux.rs, windows.rs, WebView2 loader)
│   ├── aura-hal/           TTLcdAura.dll: Armoury Crate / Aura Sync plugin (32- and 64-bit)
│   └── installer/          TTLcdStudio-Setup.exe and ttlcd-pack (packs the app into the installer)
│
├── linux/                  Debian packaging: control files, udev rule, systemd user unit, desktop entry, icon
├── driver/                 Bundled MttVDD virtual 480×480 display driver (signed, MIT)
├── firmware/               LCD controller firmware, Nuvoton helper, pin photo and the burn guide
├── docs/                   Protocol notes and README media
├── .github/workflows/      CI: Windows and Linux builds, tests, releases
│
├── extras/                 Tools that are not part of Studio
│   ├── rescue/               ttrescue (firmware recovery) and PatchPanel, C#; built into app\firmware\
│   ├── re/                   Reverse engineering: Ghidra launchers and scripts
│   └── tools-src/            Icon generator
│
├── tools/                  (not in git) Toolchain and reverse-engineering tools, fetched or copied locally
└── build/                  (not in git) Output: windows/app/, windows/TTLcdStudio-Setup.exe, linux/*.deb,
                            and target/ (compiler intermediate files)
```

`core` is the base that `cli` and `studio` share. `aura-hal` is standalone: it runs inside ASUS LightingService and
only shares the memory layout with `core`. `installer` packs whatever is in `build/windows/app/`.

<details>
<summary>README media</summary>

The GIFs above were recorded with version 1.x (C#, in the git history before 2.0).
The Rust Studio renders single pages with `--screenshot <png> <dashboard|modes[:Mode]|device|settings|idle>`, which
CI runs on both systems.
</details>

## ⚠️ Warnings

- On the **original Thermaltake firmware**, never use the on-device storage uploads (protocol commands `0x0A`/`0x0B`).
  One upload erased the LCD controller's flash during development. The app refuses them on that firmware.
- If the LCD is bricked (it no longer shows up over USB), [`firmware/README.md`](firmware/README.md) explains how to
  restore it. That needs the pump head opened.
- Pump and fans run on separate electronics and keep working whatever happens to the LCD.

Not affiliated with Thermaltake, ASUS, TCOMAS or Nuvoton. Use at your own risk.
