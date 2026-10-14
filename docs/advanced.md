# Advanced usage & CLI

[Introduction](../README.md) · [Installation](installation.md) · [Features & usage](usage.md) · **Advanced usage & CLI** · [Development](development.md)

---

- [The `ttlcd` command](#the-ttlcd-command)
- [Settings](#settings)
- [Linux background service](#linux-background-service)
- [Studio options](#studio-options)
- [Silent install (Windows)](#silent-install-windows)
- [Firmware and recovery](#firmware-and-recovery)

## The `ttlcd` command

`ttlcd` (`ttlcd.exe` on Windows, next to Studio; `/usr/bin/ttlcd` on Linux) drives the cooler without the window.
Close Studio first: only one program can drive the LCD at a time.

```
ttlcd probe                         list HID interfaces (read-only)
ttlcd info                          serial, type, rotation
ttlcd temp                          coolant temperature from the cooler's sensor
ttlcd brightness <0-100>
ttlcd rotate <0|90|180|270>         hardware rotation
ttlcd color <RRGGBB>                fill screen with a colour
ttlcd image <file> [-q 85]          show jpg/png/bmp
ttlcd gif <file> [-q 85]            loop an animated GIF (Ctrl+C to stop)
ttlcd idle <file> [-rot 0] [-q 85]  store the idle (standby) screen in the cooler: picture/GIF/video frame
ttlcd files [folder]                list files stored in the cooler (read-only)
ttlcd monitors                      list monitors
ttlcd games                         what game detection sees right now
ttlcd stream [-m N] [-r x,y,w,h] [-fps 30] [-q 80]
                                    mirror a monitor (default: the 480x480 one) or region
ttlcd display install|uninstall|status   virtual 480x480 monitor for Wallpaper Engine (admin)
ttlcd aura install|uninstall|status      Armoury Crate / Aura Sync plugin (admin)
ttlcd service install|uninstall|status   CPU temperature service, starts with Windows (admin)
ttlcd hwmon                              CPU temperature helper in this console (admin)
ttlcd run                           drive the screen from the settings (service; Ctrl+C to stop)
ttlcd config                        show the settings
ttlcd set <name> <value>            change a setting, e.g. set Mode Clock, set GifPath /path/a.gif
ttlcd render <out.jpg> [mode]       render one frame of the current mode to a file (no cooler needed)
ttlcd version
```

Run `ttlcd` without arguments for this list. `display`, `aura`, `service`, `stream`, `monitors` and `games` are
Windows features.

> `ttlcd idle` works only on the restored LX800 firmware, where the upload is verified by reading it back. It is
> refused on the original Thermaltake firmware: there an upload erased the controller's flash (see the
> [warnings](../README.md#warnings)).

## Settings

Studio, `ttlcd run` and the Linux service share one settings file, `config.xml`:

- Windows: `%APPDATA%\ttlcd\config.xml`
- Linux: `~/.config/ttlcd/config.xml`

`ttlcd config` prints every setting and `ttlcd set <name> <value>` changes one, for example:

```
ttlcd set Mode Clock          # VirtualDisplay, Monitor, Image, Gif, Video, Color, Clock, Aura, Pump, Off
ttlcd set PumpFan nct6799/fan7   # Pump mode speed: "" = CPU fan, a motherboard fan, or manual (PumpRpm)
ttlcd set ImagePath ~/Pictures/photo.jpg
ttlcd set GifPath ~/Pictures/loop.gif
```

A running `ttlcd run` (and the Linux service) picks up changes to the file by itself. Caches (video frames, the
web view's data) live in `%LOCALAPPDATA%\ttlcd` on Windows and `~/.local/share/ttlcd` on Linux.

## Linux background service

The packages install a systemd user service that runs `ttlcd run`, so the screen is driven without the window and
after every login:

```
systemctl --user enable --now ttlcd       # drive the screen, also after every login
ttlcd set Mode Clock                      # or Image / Gif / Color / Aura / Off; the service picks it up
ttlcd set GifPath ~/Pictures/loop.gif
ttlcd config                              # all settings (~/.config/ttlcd/config.xml)
```

Studio stops this service while its window is open, so the two never drive the cooler at the same time.

Other distributions: unpack `ttlcd-<version>-linux-x86_64.tar.gz` to `/` (`sudo tar -xzf ... -C /`); the window needs
GTK 3 and WebKitGTK 4.1 from the distribution. The udev rule (`/lib/udev/rules.d/60-ttlcd.rules`) gives the logged-in
user access to the cooler; replug it or reboot once after installing.

## Studio options

```
TTLcdStudio.exe --tray                         start in the tray (what "start with Windows" uses)
TTLcdStudio.exe --screenshot <png> <page>      render a page to a file and exit
                                               (dashboard | modes[:Mode] | device | settings | idle)
```

On Linux the program is `ttlcd-studio`. Starting Studio a second time brings the running one to the front.

## Silent install (Windows)

```
TTLcdStudio-Setup.exe /S [/D=<folder>]
Uninstall.exe /uninstall /S [/all]
```

Uninstalling keeps your settings in `%APPDATA%\ttlcd`. The shared Aura plugin and display driver are removed only with
`/all` or when you confirm it.

## Firmware and recovery

The LCD controller (Nuvoton W55FA93) runs its own firmware. The `firmware/` folder (and `firmware\` next to the
installed app) has the firmware this project ships, the Nuvoton helper and the rescue tools (`ttrescue.exe`,
`PatchPanel.exe`).

If the LCD no longer shows up over USB, [`firmware/README.md`](../firmware/README.md) explains how to restore it in
the controller's ROM recovery mode. That needs the pump head opened. Pump and fans run on separate electronics and keep
working whatever happens to the LCD.

The USB protocol and the firmware's settings record are documented in [protocol notes](protocol.md).

---

[Introduction](../README.md) · [Installation](installation.md) · [Features & usage](usage.md) · **Advanced usage & CLI** · [Development](development.md)
