# TT LCD project: handover / state (last updated 2026-10-14)

This is replacement software for the LCD on a **Thermaltake TH240 V2 Ultra ARGB (CL-W383-PL12SW-A)** AIO. It was
built by reverse engineering TT RGB Plus.

Owner rules:
- **Install nothing.** Use only built-in OS tools and files inside this folder. The Rust toolchain is files in the
  gitignored `tools/` (fetched by `src/fetch.sh` / `src/fetch.cmd`, pinned versions + SHA256), not an install.
- A driver is allowed only if this tool installs and uninstalls it.
- Do the work yourself, with no "split the work".
- Only Studio and its dependencies were rewritten (Rust, 2.0). `tools/`, `firmware/` and the rescue tools stay as they
  are. Tools not related to Studio live in `extras/`.
- Linux: Ubuntu build with the Studio window, CLI and screen service (see "Linux" below).
- **Every fixed issue gets a regression test in the project** (see "Tests"). Reproduce the issue first with a test
  that fails, fix it, and check the test fails without the fix and passes with it. Never leave a check as a
  one-off script or screenshot outside the repo.

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

Version 2.0 is Rust: `src/` is a cargo workspace. The C# 1.x Studio was removed; it is in the git history (tag/commit
before 2.0, e.g. `ed769c0` = 1.3.3).

- Toolchain: `src/fetch.sh` (Linux host) / `src/fetch.cmd` (Windows host; only curl.exe, tar.exe, certutil.exe) put
  Rust 1.98.0 (+ std for x86_64/i686 Windows), llvm-mingw 20260922 and Node.js 24.21.0 into `tools/`; npm packages
  come from `npm ci` (pinned by `src/studio/ui/package-lock.json`, cache in `tools/npm-cache`); crates go to
  `tools/cargo-home` (pinned by `src/Cargo.lock`). Windows targets are `*-pc-windows-gnullvm` linked by llvm-mingw:
  no Visual Studio. On Linux the GTK/WebKitGTK crates link against the desktop's own runtime libraries through
  pkg-config stubs in `tools/linux-pc` (no `-dev` packages). Without a C compiler fetch.sh also brings Zig as linker.
- `build.sh` (Linux host: Linux packages + Windows cross-build) and `build.cmd` (Windows host) call `src/build.sh` /
  `src/build.cmd`. Intermediate files go to `build/target/`. Outputs:
  - `build\windows\app\` = installed layout: `ttlcd.exe`, `TTLcdStudio.exe` (renamed `ttlcd-studio` bin),
    `TTLcdAura.dll` (i686) + `TTLcdAura64.dll`, `driver\`, `firmware\` (files from `firmware\` + `ttrescue.exe` and
    `PatchPanel.exe`, the C# tools from `extras/rescue` compiled with x86 `csc` on Windows or Mono `mcs` on Linux;
    the build checks PatchPanel rebuilds `lx800_tt021panel.bin` byte for byte).
  - `build\windows\TTLcdStudio-Setup.exe` = installer stub + payload packed by the host tool
    `cargo run -p ttlcd-setup --bin ttlcd-pack -- <stub> build/windows/app <out>` (format in `src/installer/src/pack.rs`).
  - `build/linux/ttlcd_<ver>_amd64.deb`, `ttlcd-studio_<ver>_amd64.deb`, `ttlcd-<ver>-linux-x86_64.tar.gz`.
- `run.sh` (Linux) / `run.cmd` (Windows) run the new build from `build/` without installing: Studio, or
  `run.sh ttlcd <command>` for the CLI. Every start builds the current sources first (`build.sh run` / `build.cmd run`:
  binaries only, no packages or installer; cargo makes it quick when nothing changed).
- The version is `[workspace.package] version` in `src/Cargo.toml`. In-use binaries are renamed to `*.old` before
  copying (Windows). Binary names must differ by more than case.
- Dev build by hand: `cd rs; export PATH=$PWD/../tools/rust/bin:$PWD/../tools/llvm-mingw/bin:$PATH
  CARGO_HOME=$PWD/../tools/cargo-home PKG_CONFIG_LIBDIR=$PWD/../tools/linux-pc/pkgconfig`, then
  `cargo build -p <crate> [--target x86_64-pc-windows-gnullvm]`, `cargo test -p ttlcd-core -p ttlcd-aura-hal -p ttlcd-setup`.

## Tests

Every bug fix and behaviour change comes with a test that would have caught it; it stays in the project and runs in
CI (`.github/workflows/build.yml`). Name the date and the issue in the test's doc comment.
- Logic, rendering, protocol, firmware facts: Rust `#[test]`s next to the code (`cargo test -p ttlcd-core
  -p ttlcd-aura-hal -p ttlcd-setup`; CI's `unit-tests` job). Measurements worth re-running by hand are `#[ignore]`
  tests that print (e.g. `aura_breathing`).
- Studio behaviour (page + app together): UI checks in `src/studio/tests/ui/*.js`, each run as
  `ttlcd-studio --check <script.js>` (`TTLcdStudio.exe` on Windows): the script runs in the real page with
  `T.ok(name, cond, detail)`, `T.wait(ms)`, `T.send(msg)` (same messages as the page), `T.store()` (the page's
  state) and `T.$(selector)`; Studio prints `ok`/`FAIL` lines and exits 1 on a failure. `testDrag` simulates the
  OS file-drag events. Run all: `src/studio/tests/run-ui-checks.sh` (uses xvfb-run without a display). CI runs
  them on Windows and in the clean Ubuntu container.
- Tests never touch the cooler (`TTLCD_NO_DEVICE=1`; `--check`/`--screenshot` don't save config.xml).

## Layout

| Path | What |
|---|---|
| `src/core/` | Everything UI-free: `hid/` (hidraw; SetupAPI + overlapped I/O, a timed-out read stays pending), `lcd.rs` (protocol; one lock per interface; `Global\TTLcd.Device` mutex on Windows, flock on Linux), `config.rs` (same `config.xml` as C#), `engine.rs` (render/send/control/game threads, 500 ms keep-alive, LX800 red/blue swap at send time), `render/` (tiny-skia + fontdb/rustybuzz text paths, round-panel fitting, effects), `aura.rs` (TTAS reader + visualiser), `model.rs` (Studio texts/panels/rotation/dashboard/LineDrag), `idle.rs`, `store.rs`, `sensors.rs`, `video.rs`, `capture.rs`, `games.rs`, `platform.rs`, `win/` (Windows: shared memory, registry, elevation, GDI capture, Media Foundation `mf.rs`, games, virtual display `vdd.rs`, `aura_install.rs`, `sensors.rs` helper/service) |
| `src/cli/` | `ttlcd` command line and screen service (`ttlcd run`) |
| `src/studio/` | Studio: wry/tao window, tray (tray-icon), `os/linux.rs` + `os/windows.rs`, `studio.rs` (state for the page, messages from it, drag and drop of files). The page is `ui/`: React 19 components, each a folder with `Name.jsx` + `Name.scss` (Sass; shared `styles/_tokens.scss`), bundled by esbuild (`ui/build.mjs`) from build.rs into OUT_DIR and served over a custom protocol (WebView2 on Windows with a built-in loader, no WebView2Loader.dll; WebKitGTK on Linux). `ui/src/bridge.js` = JSON messages out (`window.ipc`), `window.ttlcd.*` calls in, a small store read with `useStore` |
| `src/studio/tests/` | UI checks (`ui/*.js`, run by `run-ui-checks.sh`; see "Tests") |
| `src/aura-hal/` | `TTLcdAura.dll`: Armoury Crate HAL COM plugin written by hand (vtables), i686 + x86_64; `examples/probe.rs` self-test |
| `src/installer/` | `TTLcdStudio-Setup.exe` (Win32 UI, per-user install/uninstall) and `ttlcd-pack` |
| `linux/` | Debian packaging: control files, udev rule, systemd user unit, desktop entry, icon |
| `driver/` | Bundled MttVDD IddCx virtual 480×480 monitor (for Wallpaper Engine) |
| `firmware/` | Shipped LCD firmware, base image, Nuvoton `FA93_musb.bin` helper, pin photo, `README.md` burn guide |
| `firmware/re-src/` | The shipped firmware decompiled by Ghidra (`code/*.c`, `functions.csv`, `strings.txt`), names in `names.txt`; regenerate with `extras/re/decompile-fw.sh` (Linux; Ghidra + `tools/jdk21-linux`) |
| `extras/rescue/` | C# `Rescue.cs` (ttrescue: recovery-mode read/write/verify via SCSI pass-through), `PatchPanel.cs` |
| `extras/re/` | Ghidra projects (`proj`), scripts (`scripts/*.java`), launchers (`run.cmd`, `runtw.cmd`, `runlx.cmd`), outputs (`out`) |
| `extras/tools-src/` | Icon generator |
| `tools/` | Not in git: portable Ghidra 12.1.4 + JDK 21, Nuvoton TurboWriter, driver downloads, the Rust toolchain |
| `src/app.ico` | The app icon, embedded into the Windows exes by the crates' build.rs |
| `docs/protocol.md` | Protocol notes |

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

- The panel is round. `render/readings.rs` (`draw_readings`, port of `Render.DrawRows`) places each text line inside the circle and shrinks the block until it fits.
- Each reading has its own `ElementStyle` in `Config.ClockStyles` / `OverlayStyles`: font, bold/italic, size,
  effect, colour, label, °F, colour by value. A missing entry falls back to the old global text fields.
- Line arrangement is `ClockOrder` / `OverlayOrder`, for example `"0,2|3|"`: ids, ',' within a line, '|' ends a
  line. It is built by drag and drop in Studio. Old orders without '|' are converted from the legacy
  layout and breaks fields.
- Text is drawn at the true em size (like GDI+ on Windows). Mono's libgdiplus drew it ~16% larger, so Linux 1.x
  renders looked bigger. Fonts missing on Linux go through `fc-match`; missing glyphs fall back per character.
- `TTLcdStudio.exe --screenshot <png> <dashboard|modes[:Mode]|device|settings|idle>` (Linux `ttlcd-studio`) renders a
  page and exits; `TTLCD_SHOT_SELECT=ClkItems:2` pre-selects a reading, `TTLCD_SHOT_HEIGHT` renders taller,
  `TTLCD_SHOT_JS` runs a script first. `TTLCD_NO_DEVICE=1` never opens the cooler (use it for every test run: a normal
  Studio start takes the cooler and, on Linux, stops the `ttlcd` service). `TTLCD_DEVTOOLS=1` = web inspector.
- `--record` (README GIFs) existed only in 1.x (git history).
- Idle screen temperature: the LX800 firmware draws it with its own two 4-bit bitmap fonts (100 px and 60 px cells,
  Arial look) found in the image; `idle_text.rs` copies glyphs (`core/fonts/lx800-idle.bin`, checked against the
  firmware by a test) and layout (number cell at y 100, small "°" at y 103, centred; reading truncated; #CCCCCC).
  The pre-idle frame (`idle::lcd_frame`) and Studio's Idle preview (`/idletemp/<n>.png` overlay) both use it.

## Pump mode

`render/pump.rs`: a look inside the pump head (the LCD is the window: no case, fittings or glass drawn; the real
case and glass are around it). Dark metal impeller turning anticlockwise; water in at the top left, round the
chamber, out at the top right. Cached layers (machined floor with volute, the impeller disc with a brushed highlight
fixed to the light, impeller sprite, blade-edge mask, shadow, water swirl) plus per frame: particles (Flow from inlet
to outlet, Caught in the impeller then thrown out, Wander on their own; lit by the LEDs), the impeller
motion-blurred by doubling (2^k evenly spaced copies in k passes), the LED light. Rule: the turning sprites (blades,
hub) hold only what looks the same at any turn; every directional light or shadow (room light on the blades, the
hub's shadow and shine, the LED shimmer) is applied fixed on screen each frame (`nothing_that_turns_carries_light_or_shadow`). The LEDs ring the pump head on the
left, bottom and right (`LIGHTS` points along that arc): a soft light map (drawn at 120 px, scaled up) glows into
the water and colours the blade-edge mask before the blur. Light = Aura Sync's LED colours with the effect applied
(`aura::Visualiser::lights`, `PumpAura`) or a flickering red. Speed = rpm/1000 turns a second, from `PumpFan`: "" =
CPU fan (`sensors::cpu_fan`: label "CPU", else nct67xx fan2, else first spinning), a fan id ("nct6799/fan7", hwmon;
Linux only, Windows has no fan source yet) or "manual" (`PumpRpm`). ~8-21 ms a frame. `pump_dump` (ignored test)
writes frames at several speeds, lights and coolant colours.

## Game mode

`src/core/src/win/games.rs` detects games using only Windows sources:
- `SHQueryUserNotificationState` = D3D full-screen
- the Xbox Game Bar list `HKCU\System\GameConfigStore\Children\*\MatchedExeFullPath`, matched against running
  processes
- optionally any foreground window covering its monitor
- user include/ignore exe lists

`Engine::game_loop` checks every 3 s (~7 ms per check). While a game runs, it applies `GameAction`:
- LowerFps: caps the render delay
- IdleScreen: renders and sends nothing, so the firmware shows its idle screen

Water temperature is then polled at `GameSensorMs`. The CPU-temp helper reads its interval from offset 24 of the
`TTLcdSensors` map (Studio writes it). A helper started before this change keeps 1.5 s until restarted.

## Linux

- `build.sh` builds native binaries and two packages: `ttlcd` (CLI + systemd user service + udev rule; Depends
  libc6, libgcc-s1, fonts-dejavu-core) and `ttlcd-studio` (self-contained: all of `ttlcd` + `/usr/bin/ttlcd-studio`,
  desktop entry, icon; Depends GTK 3 / WebKitGTK 4.1 / soup 3 runtime libs, computed from the binary's NEEDED
  entries; Recommends libayatana-appindicator3-1 for the tray; Provides/Conflicts/Replaces `ttlcd`). The Ubuntu App
  Center installs one local .deb at a time and failed on a dependency on `ttlcd`; don't reintroduce one. No Mono or
  GTK# any more (GTK# 3 is gone from Debian/Ubuntu, which is why 1.x had to bundle it).
- Studio on Linux: only Image, GIF, Colour, Clock, Aura, Pump and Off (`model::supports`); no game detection (manual game
  mode); CPU temperature from hwmon (k10temp/zenpower/coretemp). It stops the `ttlcd` user service while open. Single
  instance via 127.0.0.1:47913 ("ttlcd show"), same as 1.x.
- Aura on Linux: `aura::Reader` reads `$XDG_RUNTIME_DIR/armory-crate/virtual/ttlcd.ttas` (same TTAS block as the
  Windows plugin) published by armory-crated (`~/Armory-crate`, `armory-crate virtual add ttlcd`; spec in its
  `docs/virtual-devices.md`). Its writer uses a seqlock (odd seq = mid-write); each read opens the file fresh, twice.
  `aura::linux_service_installed()` greys out the Aura tile until armory-crate is installed (rechecked every 3 s).
- hidraw: `/sys/class/hidraw/*/device/uevent` (HID_ID, HID_PHYS ".../inputN" = interface); reads return the report
  without the report-ID byte. The udev rule gives the logged-in user access.
- CLI: `run` (headless service, reloads config.xml on change), `config`, `set <field> <value>`, `render <jpg> [mode]`.
- CI installs the .debs in a clean `ubuntu:24.04` container and renders frames and Studio screenshots under Xvfb.
  On this machine: CLI, engine (30 fps on the cooler) and Studio screenshots verified; the tray and service handover
  not yet run on the desktop.

## GitHub

`.github/workflows/build.yml`: Windows job runs `build.cmd` with the bundled toolchain, smoke-tests `ttlcd.exe`, a
silent install/uninstall of the setup and Studio screenshots; Linux job builds the .debs, runs unit tests, tests the
packages in a clean ubuntu:24.04 container and renders Studio pages under Xvfb. A `v*` tag releases the setup,
portable zip, .debs and tarball. `.gitignore` keeps `build/`, `tools/`, the RE workspace (except
`extras/re/scripts` and its launchers), dumps and binaries out of the repo (except `firmware/*.bin` and `driver/`).

## Features status

| Feature | Status |
|---|---|
| Live modes | Rust engine verified on the restored firmware (Clock at 30 fps). Virtual display, monitor mirror, video: Windows only, compile-checked, not yet run on Windows |
| UI | Same design as 1.x in HTML (WebView2 / WebKitGTK); pages verified by screenshots on Linux; Windows not run yet |
| CPU temperature | Windows: `TTLcdSensors` service / `ttlcd hwmon` (TOWEI `XYDataCenter.dll`), ported, not run yet. Linux: hwmon |
| Installer | `TTLcdStudio-Setup.exe` (Rust, Win32 UI, compressed payload); not run yet |
| Aura plugin (CLSID C0C96C60-30E3-4021-9904-966EC4FA0F0F) | Native DLL (i686 + x64) replaces the .NET one; capability XML byte-identical; `aura install` removes the old mscoree registration. Not run on Windows yet (`examples/probe.rs` self-test) |
| Virtual display driver | Failed to load at the last boot (WUDFRd 0xC0000365 on ROOT\DISPLAY\0000). Not investigated yet |
| Idle picture | LX800 only, verified by read-back; refused on the original firmware. Boot animation upload disabled |
| Cooler queries | On 2026-10-13 the cooler stopped answering queries (serial, coolant) while frames worked, with both 1.x and 2.0; a power cycle is the likely fix |

