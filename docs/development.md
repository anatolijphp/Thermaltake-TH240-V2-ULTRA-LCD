# Development

[Introduction](../README.md) · [Installation](installation.md) · [Features & usage](usage.md) · [Advanced usage & CLI](advanced.md) · **Development**

---

- [Build from source](#build-from-source)
- [Run without installing](#run-without-installing)
- [How it works](#how-it-works)
- [Project layout](#project-layout)
- [Tests](#tests)
- [Continuous integration](#continuous-integration)
- [Extending](#extending)

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

## Run without installing

```
run.cmd                        # Windows: Studio from build\windows\app
run.cmd ttlcd info             # Windows: the command-line tool
./run.sh                       # Linux: Studio
./run.sh ttlcd info            # Linux: the command-line tool
```

Both build the current sources first (only the binaries, no installer or packages; quick when nothing changed; UI
sources are re-bundled when they change). Studio takes over the cooler while it runs (on Linux it stops the `ttlcd`
user service); close an installed Studio first.

Environment variables for working on Studio:

| Variable | Effect |
|---|---|
| `TTLCD_NO_DEVICE=1` | Never open the cooler (frames are still rendered for the preview). Use it for every test run |
| `TTLCD_DEVTOOLS=1` | Open the web inspector with the window; right-click works everywhere |
| `TTLCD_SHOT_JS`, `TTLCD_SHOT_SELECT`, `TTLCD_SHOT_HEIGHT` | With `--screenshot`: run a script first, pre-select a reading (`ClkItems:2`), render taller |

## How it works

The LCD has its own controller (Nuvoton W55FA93) behind a USB HID interface: one endpoint for control commands
(brightness, rotation, sensor queries) and one for 480×480 JPEG frames. Studio renders each frame (mode, overlay,
rotation), encodes it and streams it at up to 30 fps. When streaming stops, the cooler's firmware shows its own idle
screen: the stored picture with the coolant temperature, drawn with the firmware's bitmap fonts (copied into
`src/core/src/idle_text.rs` so the app's preview matches). The protocol is documented in [protocol notes](protocol.md).

## Project layout

```
ttlcd/
├── build.sh, build.cmd     Build everything (Linux host / Windows host); they call src/build.*
├── run.sh, run.cmd         Run the new build from build/ without installing
├── README.md, CLAUDE.md    Introduction; developer handover notes (hardware state, protocol, layout)
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
│   │   ├── idle_text.rs      The idle screen's temperature as the firmware draws it (fonts/lx800-idle.bin)
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
│   │   ├── src/os/           System-specific parts (linux.rs, windows.rs, WebView2 loader)
│   │   └── tests/            UI checks (ui/*.js) and their runner
│   ├── aura-hal/           TTLcdAura.dll: Armoury Crate / Aura Sync plugin (32- and 64-bit)
│   └── installer/          TTLcdStudio-Setup.exe and ttlcd-pack (packs the app into the installer)
│
├── linux/                  Debian packaging: control files, udev rule, systemd user unit, desktop entry, icon
├── driver/                 Bundled MttVDD virtual 480×480 display driver (signed, MIT)
├── firmware/               LCD controller firmware, Nuvoton helper, pin photo and the burn guide
│   └── re-src/               That firmware decompiled (Ghidra): code/*.c, functions, strings, names
├── docs/                   These pages, protocol notes and media
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

## Tests

Every bug fix and behaviour change comes with a test that reproduces the issue (it fails without the fix) and stays in
the project.

**Rust tests** sit next to the code they check: logic, rendering, protocol and firmware facts.

```
cd src
cargo test -p ttlcd-core -p ttlcd-aura-hal -p ttlcd-setup     # with the toolchain from tools/ on PATH
```

Measurements worth re-running by hand are `#[ignore]` tests that print, e.g.
`cargo test -p ttlcd-core aura_breathing -- --ignored --nocapture`.

**UI checks** test Studio's page and app together. Each is a script in `src/studio/tests/ui/` that runs inside the
real page:

```
ttlcd-studio --check src/studio/tests/ui/select-toggle.js     # one check (TTLcdStudio.exe on Windows)
src/studio/tests/run-ui-checks.sh                              # all of them (xvfb-run without a display)
```

A script gets `T.ok(name, condition, detail)`, `T.wait(ms)`, `T.send(message)` (the same messages the page sends),
`T.store()` (the page's state) and `T.$(selector)`. Studio prints an `ok` or `FAIL` line per check and exits with 1
when one fails. `T.send({t: 'testDrag', a: 'enter' | 'leave' | 'drop', paths})` simulates a file dragged over the
window. Checks never open the cooler or save settings.

## Continuous integration

Every push runs [GitHub Actions](../.github/workflows/build.yml) on Windows and Ubuntu with the same bundled
toolchain: build, firmware checksums, a check that `PatchPanel` rebuilds the shipped firmware byte for byte, the unit
tests, the Aura plugin self-test, a silent install and uninstall, an install of both `.deb`s into a clean Ubuntu
container that renders frames, screenshots of every Studio page and the UI checks on both systems. Compiled
dependencies are cached from `main`. Tags `v*` publish a release with the files for both.

Studio renders single pages with `--screenshot <png> <dashboard|modes[:Mode]|device|settings|idle>`; CI uploads them
as artifacts. The GIFs in these pages were recorded with version 1.x (C#, in the git history before 2.0), except
`pump.gif`: `ttlcd-studio --record <out.gif> <seconds> <script.js>` runs a script (the UI checks' `T` helpers) and
records the window every 100 ms into a GIF at 75 % like the others. The scripts are in `src/studio/tests/record/`,
each with its command line (fresh settings via `XDG_CONFIG_HOME`, `TTLCD_NO_DEVICE=1`).

## Extending

**A display mode.** Modes are listed in `Mode` (`src/core/src/config.rs`). Then:
- `Engine::produce` (`src/core/src/engine.rs`) renders its frames;
- `model.rs` gives it a description, decides where it runs (`supports`) and which option panels Studio shows
  (`panels`);
- `ModeTiles.jsx` gives it a tile, and `Modes.jsx` its options for those panels.

**A setting.** Add the field to `Config` (`config.rs`): it is read from and written to `config.xml`, `ttlcd set`
changes it, and the page changes it with `setCfg('<Name>', value)`.

**A part of the UI.** Each component is a folder with `Name.jsx` + `Name.scss` under `src/studio/ui/src/`, using the
shared tokens from `styles/_tokens.scss`. The page reads the app's state with `useStore` and sends messages with
`send` (`bridge.js`); `src/studio/src/studio.rs` handles them and pushes state back.

**Firmware.** The firmware the cooler runs is decompiled in [`firmware/re-src/`](../firmware/re-src/README.md)
(regenerate with `extras/re/decompile-fw.sh`; add what you learn to its `names.txt`).

**Protocol work.** The command list and the firmware's config record are in [protocol notes](protocol.md); read the
warnings there and in the [introduction](../README.md#warnings) before sending anything new to the cooler.

Add a test for whatever you change (see [Tests](#tests)).

---

[Introduction](../README.md) · [Installation](installation.md) · [Features & usage](usage.md) · [Advanced usage & CLI](advanced.md) · **Development**
