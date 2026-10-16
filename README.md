# Quiet Dock

A macOS-style dock for the Windows 11 taskbar, built on
[Windhawk](https://windhawk.net). Two parts:

- **`mod/quiet-dock.wh.cpp`**: icon magnification on hover. A fork of
  [Taskbar Dock Animation Plus](https://windhawk.net/mods/taskbar-dock-animation-plus)
  that stops using CPU once the dock settles.
- **`theme/`**: a frosted-glass floating pill for the
  [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler)
  mod, with headroom so magnified icons aren't clipped.

**Status:** compiles with Windhawk 1.7.3's toolchain against Windows 11 build
26300; first live test on hardware in progress.

## What the fork changes

Upstream subscribes to XAML's per-frame callback for as long as the cursor is
over the taskbar. That callback runs the animation math and also makes XAML
redraw the taskbar every frame, even when nothing is moving.

| | Upstream (Plus 2.0.1) | Quiet Dock |
|---|---|---|
| Cursor resting on the dock | callback runs every frame | callback unsubscribes once icons settle; the next mouse move wakes it |
| Per icon, per frame | ~10 WinRT lookups for the transforms, every property rewritten | transforms cached at setup, properties written only when they change |
| Multiple monitors | one shared "active taskbar"; switching monitors can freeze icons magnified ([#3593](https://github.com/ramensoftware/windhawk-mods/issues/3593)) | each taskbar keeps its own state and its own callback, on its own thread |
| Frame timing | whole milliseconds (8.33 ms → 8 at 120 Hz) | fractional milliseconds, clamped after stalls |
| Idle "breathing" bounce | on | off by default: macOS doesn't do it, and it moves the click target ([#2790](https://github.com/ramensoftware/windhawk-mods/issues/2790)) |
| Default magnification | 130% | 140%, with the theme's headroom |

Animation curves, settings and the visual design are upstream's.

## Install

You need [Windhawk](https://windhawk.net) (or `winget install RamenSoftware.Windhawk`).

1. In Windhawk, install **Windows 11 Taskbar Styler** and **Taskbar height
   and icon size** from Explore.
2. Run `.\copy.ps1 height`, open *Taskbar height and icon size* → Settings →
   Textual mode, paste, Save.
3. Run `.\copy.ps1 theme`, open *Windows 11 Taskbar Styler* → Settings →
   Textual mode, paste, Save.
4. Run `.\copy.ps1 mod`. In Windhawk click **Create a new mod**, select all,
   paste, click **Compile mod**, then **Exit editing mode**.

The theme gives up 16px of vertical screen space: the taskbar window has to be
taller than the pill, because icons can't draw outside it.

## Performance

Measured before/after with Windows Performance Recorder: pending.

## Credits and license

MIT. Taskbar Dock Animation by [@Ph0en1x-dev](https://github.com/Ph0en1x-dev),
Plus fork by [@incconutwo](https://github.com/incconutwo). Theme selectors
adapted from FrostyGlass by [Guido Lamanna](https://github.com/guidolamanna)
in the [taskbar styling guide](https://github.com/ramensoftware/windows-11-taskbar-styling-guide).
