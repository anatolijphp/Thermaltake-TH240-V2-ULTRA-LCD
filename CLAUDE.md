# TT LCD project: handover / state (last updated 2026-10-09)

This is replacement software for the LCD on a **Thermaltake TH240 V2 Ultra ARGB (CL-W383-PL12SW-A)** AIO. It was
built by reverse engineering TT RGB Plus.

Owner rules:
- **Install nothing.** Use only built-in Windows tools (C# 5 / .NET 4.8 `csc`) and files inside this folder.
- A driver is allowed only if this tool installs and uninstalls it.
- Do the work yourself, with no "split the work".
- Linux support is wanted later (not started).

Full conversation transcript (very long):
`C:\Users\anato\.claude\projects\C--Users-anato\87ff2591-a9e9-489b-9bea-a8a27fdb3ba3.jsonl`

## Current hardware state (read first)

- On 2026-10-09 an on-device file-upload test (`ttlcd idle` on the **original** Thermaltake firmware) erased the LCD
  controller's SPI flash. The Thermaltake firmware and bootloader are gone, and there is no backup.
  - The Thermaltake image (`2.1inch-update-v42.bin` from TT RGB Plus) is encrypted with a key that lived in the
    erased bootloader, so it can't be rebuilt.
- **Restored the same day.** The board now runs **`firmware/lx800_tt021panel.bin`**: the LX800 rescue image (same
  Nuvoton W55FA93 chip and OEM) with Thermaltake's TT021 ST7701S panel init patched in (`rescue/PatchPanel.cs`).
  - It was written in ROM recovery mode (`USB 0416:9396`, flash pins shorted) with `ttrescue.exe write` and
    verified by read-back (`rescue/rescue.log`).
  - It enumerates as `USB 1A2C:151D`, serial `A247392SS000000`. The screen works.
  - Studio swaps red/blue for this panel and sends keep-alive frames.
  - Burn instructions and checksums: `firmware/README.md`.
- **Do not** send `idle`/`boot` uploads (cmds 0x0A/0x0B) to the original firmware. That is what caused the loss:
  the cooler erases flash for minutes before it acks the first chunk. `DeviceStore.WriteStandby` refuses it.
  On the LX800 firmware the idle upload (`UploadFileLx`, ≤950 KB, type 0 = `c:\syst\wapper.jpg`) is verified by
  read-back (0x0C/0x0D) and worked on 2026-10-09. Studio does it from the preview's **Idle** view (crop, compress,
  upload, read back). The stored image is cached in `%APPDATA%\ttlcd\idle-current.jpg`. Idle images are stored
  **upright**, without the live-frame `Rotation`. The firmware draws its idle screen in the right orientation by
  itself; baking in rotation showed it sideways.
- **Do not** use the on-device file commands to erase firmware. That approach was ruled out.

## Build

Sources and build output are separate. Everything built goes to `build\<os>\` (only `build\windows\` so far; Linux
will get `build\linux\`). Nothing built lives in the source folders.

`build.cmd` builds `ttlcd.exe` (CLI), `TTLcdStudio.exe` (WPF app + tray) and `TTLcdAura.dll` (Aura plugin) into
`build\windows\app\` with `%WINDIR%\Microsoft.NET\Framework64\v4.0.30319\csc.exe`. That folder has the installed
layout (`driver\` and `firmware\` are copied in), so it runs as is; the owner's Studio and its autostart entry run from
there. Binary names must differ by more than case.
Binaries in use (Studio, or the elevated `ttlcd hwmon` helper) are renamed to `*.old` before compiling, since a
running exe can be renamed but not overwritten. Leftovers are deleted on a later build. Running processes keep the old
code until they restart. It also builds:
- the `rescue/` tools (x86 `csc`) into `build\windows\app\firmware\`
- `build\windows\TTLcdStudio-Setup.exe` from `installer/Setup.cs`, with `build\windows\app` embedded as
  `payload/<path>` resources

The version is in `src/Version.cs`. `.github/workflows/build.yml` builds, checks the firmware checksums, smoke-tests a
silent install and uninstall, and uploads the installer and a portable zip.

## Layout

| Path | What |
|---|---|
| `src/Hid.cs` | SetupAPI/hid.dll P/Invoke. `Enumerate(vid,pid,queryCaps)`: `false` avoids a ~5 s MI_00 stall |
| `src/Lcd.cs` | Protocol. VID/PID 264A:233C, falling back to 1A2C:151D. `Global\TTLcd.Device` single-writer mutex |
| `src/Render.cs` `Capture.cs` `Video.cs` | Compose/rotate/JPEG; GDI screen capture; Media Foundation video → 480×480 frames (cached) |
| `src/Engine.cs` | Renderer + sender threads; control/upload jobs on the control thread; `timeBeginPeriod(1)` |
| `src/Config.cs` | `%APPDATA%\ttlcd\config.xml`. Modes: VirtualDisplay, Monitor, Image, Gif, Video, Color, Clock, Aura, Off |
| `src/App.cs` `MainWindow.xaml` | Armoury-Crate-style WPF UI with preview, tray, `--tray`, `--screenshot` dev aid |
| `src/VirtualDisplay.cs` + `driver/` | Bundled MttVDD IddCx virtual 480×480 monitor (for Wallpaper Engine); install/uninstall |
| `src/Aura.cs` + `aura/AuraHal.cs` | ASUS Aura (Armoury Crate) HAL COM plugin + installer + MilkDrop-style feedback visualiser |
| `src/Storage.cs` | Idle/boot upload + boot.pak builder. **Dangerous, see above.** Not wired into the UI |
| `docs/protocol.md` | Protocol notes |
| `re/` | Ghidra projects (`re/proj`), scripts (`re/scripts/*.java`), launchers (`run.cmd`, `runtw.cmd`, `runlx.cmd`), outputs (`re/out`) |
| `tools/` | Portable Ghidra 12.1.4 + JDK 21, Nuvoton TurboWriter (analysis only), driver downloads |
| `build/windows/` | Build output only (not in git): `app\` (runnable, installed layout), `TTLcdStudio-Setup.exe` |
| `rescue/` | `Rescue.cs` (ttrescue: recovery-mode read/write/verify via SCSI pass-through), `PatchPanel.cs` |
| `firmware/` | Shipped LCD firmware, base image, Nuvoton `FA93_musb.bin` helper, pin photo, `README.md` burn guide |
| `installer/` | `Setup.cs`: per-user installer and uninstaller (shortcuts, autostart, optional admin parts) |
| `src/Sensors.cs` | CPU temperature: TOWEI `XYDataCenter.dll` helper (`ttlcd hwmon`) or the `TTLcdSensors` service |

## Protocol (HKC / Xinyao LCD, both firmwares)

- MI_00 = control. 440-byte packets (+ report ID), 436-byte payload.
- MI_01 = image. 1024-byte packets, 1020-byte payload.
- Header u32 LE: `(first ? 0x80000000 : 0) | ((first ? total : seq) << 8) | cmd`.

Commands:

| Cmd | Meaning |
|---|---|
| `08` | JPEG frame (MI_01, 480×480) |
| `09` | Fill colour {R,G,B,1} |
| `10` | Bit 0x80 = backlight **off** (send 0 for on) |
| `11` | Rotation code (0 = 0°, 1 = 270°, 2 = 180°, 3 = 90°) |
| `12` | Brightness 0–100 |
| `81` / `84` / `85` / `82` / `87` | Queries: rotation / type / serial / … |
| `0A` / `0B` | File upload. **Do not use** |
| `13` / `14` | Boot file name / boot frame interval |

Replies come back on MI_00 with the same cmd byte. Replies produced while no handle is open are lost.
A timed-out read must stay pending (`Lcd.ReadCtrl`). An abandoned one swallows the next reply.

Rotation:
- The cooler's stored rotation is byte 1 of its flash config record (see `docs/protocol.md`). It is set with `0x11`,
  read with `0x81`, and is 90° on this unit.
- It turns only the firmware's own idle and boot screens, not streamed frames (tested on the LCD 2026-10-10).
- Rotation is one global setting.
  - Live frames are rotated in software (`Config.Rotation`).
  - The engine sets the cooler's stored rotation to match: `Engine.HwFor`, hw = HwUpright - (Rotation - PreviewMount).
    The cooler's +90° turns counter-clockwise and ours clockwise (tested 2026-10-10). It is written only when it differs.
  - Calibration on this unit: PreviewMount 270 (upright) with HwUpright 90.
  - The preview shows frames turned by -PreviewMount, so it matches the LCD. The arrow at the preview's top-left adds 90°.

On 2026-10-09 the cooler once stopped answering every query while frames still worked. It recovered without
stopping the `ttlcd hwmon` helper, so the helper wasn't the cause. `ttlcd temp` checks it.

## LCD text / UI

- The panel is round. `Render.DrawRows` places each text line inside the circle and shrinks the block until it fits.
- Each reading has its own `ElementStyle` in `Config.ClockStyles` / `OverlayStyles`: font, bold/italic, size,
  effect, colour, label, °F, colour by value. A missing entry falls back to the old global text fields.
- Line arrangement is `ClockOrder` / `OverlayOrder`, for example `"0,2|3|"`: ids, ',' within a line, '|' ends a
  line. It is built by drag and drop in Studio. Old orders without '|' are converted from the legacy
  layout and breaks fields.
- `TTLCD_SHOT_SELECT=ClkItems:2` pre-selects a reading for `--screenshot`.
- `TTLcdStudio.exe --screenshot <png> modes:Clock` renders that mode's options without saving it.
- `TTLcdStudio.exe --record <gif> <modes|dashboard|clock>` records the README GIFs (`src/Record.cs`, `src/Gif.cs`) in an
  off-screen window. Close Studio first and restore `config.xml` afterwards (the scenes change the mode).
  `TTLCD_SHOT_HEIGHT` renders a taller window.

## Game mode

`src/Games.cs` detects games using only Windows sources:
- `SHQueryUserNotificationState` = D3D full-screen
- the Xbox Game Bar list `HKCU\System\GameConfigStore\Children\*\MatchedExeFullPath`, matched against running
  processes
- optionally any foreground window covering its monitor
- user include/ignore exe lists

`Engine.GameLoop` checks every 3 s (~7 ms per check). While a game runs, it applies `GameAction`:
- LowerFps: caps the render delay
- IdleScreen: renders and sends nothing, so the firmware shows its idle screen

Water temperature is then polled at `GameSensorMs`. The CPU-temp helper reads its interval from offset 24 of the
`TTLcdSensors` map (Studio writes it). A helper started before this change keeps 1.5 s until restarted.

## GitHub

`.github/workflows/build.yml` runs `build.cmd` on `windows-latest` and uploads the binaries. A `v*` tag also creates
a release. `.gitignore` keeps `build/`, `tools/`, the RE workspace (except `re/scripts`), dumps and binaries out of the
repo (except `firmware/*.bin` and `driver/`).

## Features status

| Feature | Status |
|---|---|
| Live modes | Working on the restored firmware: virtual display / Wallpaper Engine, monitor mirror, image, GIF, video (≤30 s, crop/zoom), colour, clock, Aura, off |
| UI | Armoury-Crate-style UI: dashboard gauges, round preview with Idle view and rotate, per-reading text styles, drag-and-drop lines, game mode, full screen |
| CPU temperature | `TTLcdSensors` Windows service (`ttlcd service install`, one time, admin), or the `ttlcd hwmon` console helper. Needs TOWEI installed (its `XYDataCenter.dll`) |
| Installer | `build\windows\TTLcdStudio-Setup.exe`: per-user install. The uninstaller removes the shared Aura plugin and display driver only on request (`/all`) |
| Aura plugin (CLSID C0C96C60-30E3-4021-9904-966EC4FA0F0F) | Shows in Armoury Crate as an External device. Shared memory reattach is fixed in the built `TTLcdAura.dll`. **Needs reinstall:** `ttlcd.exe aura uninstall` then `aura install` (admin; restarts LightingService) |
| Virtual display driver | Failed to load at the last boot (WUDFRd 0xC0000365 on ROOT\DISPLAY\0000). Not investigated yet |
| Idle/loading screens | Protocol known (types 0/1, 10 KB chunks, boot.pak format in `Storage.cs`), but unsafe on this hardware |
