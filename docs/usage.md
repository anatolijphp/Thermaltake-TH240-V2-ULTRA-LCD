# Features & usage

[Introduction](../README.md) · [Installation](installation.md) · **Features & usage** · [Advanced usage & CLI](advanced.md) · [Development](development.md)

---

- [Pick a mode, see it live](#pick-a-mode-see-it-live)
- [Style every reading](#style-every-reading)
- [Game mode](#game-mode)
- [Idle picture](#idle-picture)

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

---

[Introduction](../README.md) · [Installation](installation.md) · **Features & usage** · [Advanced usage & CLI](advanced.md) · [Development](development.md)
