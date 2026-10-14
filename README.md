<div align="center">

# TT LCD Studio

**Open replacement software for the round 2.1" LCD on the Thermaltake TH240 V2 Ultra ARGB (CL-W383) AIO cooler.**

No TT RGB Plus, no frameworks to install, no telemetry. One small app for Windows and Ubuntu, built by reverse
engineering the cooler's USB protocol.

[![build](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions/workflows/build.yml/badge.svg)](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions/workflows/build.yml)
[![release](https://img.shields.io/github/v/release/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD?include_prereleases&label=release)](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/releases)
![platform](https://img.shields.io/badge/platform-Windows%20%7C%20Ubuntu-0078d4)
![Rust](https://img.shields.io/badge/Rust-1.98%20(bundled%20toolchain)-b7410e)

**Introduction** · [Installation](docs/installation.md) · [Features & usage](docs/usage.md) · [Advanced usage & CLI](docs/advanced.md) · [Development](docs/development.md)

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

## Installation

Windows installer, portable zip and Ubuntu packages: see [Installation](docs/installation.md).

> **Close TT RGB Plus first.** Only one program can drive the LCD at a time.

## Documentation

| Page | What's in it |
|---|---|
| [Installation](docs/installation.md) | Installing on Windows and Ubuntu, the optional parts, what runs on Linux |
| [Features & usage](docs/usage.md) | The screens and modes, styling readings, game mode, the idle picture |
| [Advanced usage & CLI](docs/advanced.md) | The `ttlcd` command, settings from the command line, the Linux background service, silent install, firmware recovery |
| [Development](docs/development.md) | Building from source, how it works, the project layout, tests, CI, adding a mode or a UI part |
| [Protocol notes](docs/protocol.md) | The cooler's USB protocol and the firmware's config record |

## Warnings

- On the **original Thermaltake firmware**, never use the on-device storage uploads (protocol commands `0x0A`/`0x0B`).
  One upload erased the LCD controller's flash during development. The app refuses them on that firmware.
- If the LCD is bricked (it no longer shows up over USB), [`firmware/README.md`](firmware/README.md) explains how to
  restore it. That needs the pump head opened.
- Pump and fans run on separate electronics and keep working whatever happens to the LCD.

Not affiliated with Thermaltake, ASUS, TCOMAS or Nuvoton. Use at your own risk.

---

**Introduction** · [Installation](docs/installation.md) · [Features & usage](docs/usage.md) · [Advanced usage & CLI](docs/advanced.md) · [Development](docs/development.md)
