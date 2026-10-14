# Features & usage

[Introduction](../README.md) · **Features & usage** · [Advanced usage & CLI](advanced.md) · [Development](development.md)

---

- [Pick a mode, see it live](#pick-a-mode-see-it-live)
- [Style every reading](#style-every-reading)
- [Game mode](#game-mode)
- [Idle picture](#idle-picture)
- [Install on Windows](#install-on-windows)
- [Install on Ubuntu / Linux](#install-on-ubuntu--linux)

## Pick a mode, see it live

One click switches what the cooler shows: your **Wallpaper Engine** wallpaper (on a virtual 480×480 monitor),
**Aura Sync** effects from Armoury Crate, a looping **GIF**, and more. The preview on the right is exactly what the
cooler shows, rotated the way the pump is mounted.

<img src="media/modes.gif" alt="Switching between the Wallpaper Engine, Aura Sync and GIF modes" width="860">

Drag a picture, GIF or video onto the window and Studio switches to the matching mode and to Display modes while the
file is over the window; drop it to show it.

## Style every reading

Any mode can carry an overlay of time and temperatures, and the Clock mode is made of them. Arrange readings into lines
by drag and drop, then style each one on its own: font, size, effect (shadow, outline, neon glow), colour, label beside
or above, °C/°F and colour-by-value. Everything is kept inside the round screen automatically. Click a reading to style
it; click it again to close its style.

<img src="media/style.gif" alt="Aura Sync mode: switching on the time overlay, making it bigger and giving it a neon glow" width="860">

## Game mode

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

<img src="media/game.gif" alt="Settings: turning on game detection and forcing game mode on, the frame rate drops to 5 fps" width="860">

## Idle picture

When nothing streams to it (PC asleep, off, booting, or game mode's "Show idle screen"), the cooler shows a picture
stored in its own flash with the coolant temperature over it. Switch the preview to **Idle** to see it: choose a
picture, drag and zoom to place it, upload it (it is read back to check), or read the stored one from the cooler.
The preview draws the temperature exactly as the cooler does (its own fonts, position and colour).

## Install on Windows

1. Download **`TTLcdStudio-Setup.exe`** from the [latest release](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/releases)
   (or the newest run on the [Actions](https://github.com/anatolijphp/Thermaltake-TH240-V2-ULTRA-LCD/actions) tab).
2. Run it. It installs per user to `%LOCALAPPDATA%\Programs\TT LCD Studio`, **no administrator rights needed**, and
   can add a Start menu shortcut and start with Windows.
3. Optional parts ask for administrator rights when you pick them:
   - **CPU temperature service** (reads the CPU temperature through TOWEI's sensor driver, which must be installed)
   - **Armoury Crate plugin** (Aura Sync)
   - **Virtual display driver** (for Wallpaper Engine)

Prefer no installer? Grab **`TTLcdStudio-portable.zip`** instead and run `TTLcdStudio.exe` from anywhere.
Silent install and uninstall are described in [Advanced usage](advanced.md#silent-install-windows).

> **Close TT RGB Plus first.** Only one program can drive the LCD at a time.

## Install on Ubuntu / Linux

TT LCD Studio runs on Ubuntu too: the same window (the same UI, in the system's WebKitGTK) over the same engine and settings.

<img src="media/ubuntu.gif" alt="TT LCD Studio on Ubuntu: dashboard, display modes (Clock, Colour, Aura Sync), device and settings pages with the live preview" width="860">

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

Running without the window (a background service and the command line) is described in
[Advanced usage](advanced.md#linux-background-service).

---

[Introduction](../README.md) · **Features & usage** · [Advanced usage & CLI](advanced.md) · [Development](development.md)
