# Installation

[Introduction](../README.md) · **Installation** · [Features & usage](usage.md) · [Advanced usage & CLI](advanced.md) · [Development](development.md)

---

- [Windows](#windows)
- [Ubuntu / Linux](#ubuntu--linux)

> **Close TT RGB Plus first.** Only one program can drive the LCD at a time.

## Windows

1. Download **`TTLcdStudio-Setup.exe`** from the [latest release](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/releases)
   (or the newest run on the [Actions](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions) tab).
2. Run it. The first step picks the folder (default per user, `%LOCALAPPDATA%\Programs\TT LCD Studio`, **no
   administrator rights needed**) and adds a Start menu shortcut and start with Windows.
3. Then each optional component gets its own step, with a short animation of what it adds to the LCD. Each asks
   for administrator rights when you pick it, and can also be added later from Studio:
   - **CPU temperature service** (reads the CPU temperature through TOWEI's sensor driver, which must be installed)
   - **Armoury Crate plugin** (Aura Sync)
   - **Virtual display driver** (for Wallpaper Engine)

Prefer no installer? Grab **`TTLcdStudio-portable.zip`** instead and run `TTLcdStudio.exe` from anywhere.
Silent install and uninstall are described in [Advanced usage](advanced.md#silent-install-windows).


## Ubuntu / Linux

TT LCD Studio runs on Ubuntu too: the same window (the same UI, in the system's WebKitGTK) over the same engine and settings.

<img src="media/ubuntu.gif" alt="TT LCD Studio on Ubuntu: dashboard, display modes (Clock, Colour, Aura Sync), device and settings pages with the live preview" width="860">

```
sudo apt install ./ttlcd-studio_2.2.0_amd64.deb   # from the release, or open the file in the App Center
ttlcd-studio                                       # or "TT LCD Studio" in the app menu
```

`ttlcd-studio` is the only file you need: it includes the `ttlcd` command and background service too (and
replaces the `ttlcd` package if that is installed). Without the window, `ttlcd_<version>_amd64.deb` alone has just
the command and the service, and needs nothing but the C library. Both are native programs; the window uses the
system's GTK 3 and WebKitGTK 4.1 (part of any current Ubuntu desktop, 22.04 to 26.04).

- **Everything Linux can do, in the window:**
  - dashboard
  - Image, GIF, Colour, Clock, Aura Sync, Pump and Off modes with the overlay
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

Running without the window (a background service and the command line) is described in
[Advanced usage](advanced.md#linux-background-service).

Once installed, see [Features & usage](usage.md) for what each mode does.

---

[Introduction](../README.md) · **Installation** · [Features & usage](usage.md) · [Advanced usage & CLI](advanced.md) · [Development](development.md)
