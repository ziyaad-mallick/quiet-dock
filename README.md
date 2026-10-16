# Quiet Dock

A macOS-style dock for the Windows 11 taskbar, built on
[Windhawk](https://windhawk.net). Two parts:

- **`mod/quiet-dock.wh.cpp`**: icon magnification on hover. A fork of
  [Taskbar Dock Animation Plus](https://windhawk.net/mods/taskbar-dock-animation-plus)
  that stops using CPU once the dock settles.
- **`theme/`**: a frosted-glass floating pill for the
  [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler)
  mod, with headroom so magnified icons aren't clipped.

**Status:** running daily on Windows 11 build 26300 (1920x1200 at 125%, plus a
second monitor) with Windhawk 1.7.3 portable. Tuned by screenshot, not yet
measured: CPU numbers below are still pending.

## What the fork changes

**Sharp magnification.** Windows renders taskbar icons at one size and the
original mod stretches that bitmap, so magnified icons blur. Quiet Dock has
Windows render icons at the *magnified* size (via the "Taskbar height and icon
size" mod) and draws them scaled down at rest, so at full magnification you
see native pixels. It also packs the icons at its own pitch (rest size + a
small gap) instead of Windows' wider tiles.

**The dock grows with the icons.** The background pill is sized to the icons
at rest, and stretched with a render transform while they magnify, so the end
icons never slide off it (or get cut off at the edge of Windows' layout).

**It stops using CPU once the dock settles.** Upstream keeps a per-frame
callback subscribed for as long as the cursor is over the taskbar, which also
makes XAML redraw the taskbar every frame. Here the callback unsubscribes as
soon as every icon has reached its target.

| | Upstream (Plus 2.0.1) | Quiet Dock |
|---|---|---|
| Magnified icons | bitmap stretched up to 157% | rendered at full size, drawn smaller at rest |
| Dock background | fixed; magnified end icons slide off | resized to wrap the icons every frame |
| Cursor resting on the dock | callback runs every frame | unsubscribes once icons settle |
| Per icon, per frame | ~10 WinRT lookups, every property rewritten | transforms cached, written only on change |
| Multiple monitors | one shared "active taskbar"; icons can freeze magnified ([#3593](https://github.com/ramensoftware/windhawk-mods/issues/3593)) | each taskbar keeps its own state and loop |
| Layout changes (apps opening/closing) | picked up on the next hover | applied immediately (hooks the frame's ArrangeOverride) |
| Idle "breathing" bounce | on | off by default ([#2790](https://github.com/ramensoftware/windhawk-mods/issues/2790)) |

Things that bit during development, kept here because they explain the code:
the taskbar's item host also holds recycled buttons parked ~10000px
off-screen; `TransformToVisual` on an element includes its own render
transform (measure layout from the parent instead); each hook gets a different
interface pointer for the same TaskbarFrame (key state by IUnknown); Windows
replaces the TaskbarBackground's own RenderTransform every frame; and the
Taskbar Styler re-applies any property it styles, so the mod and the theme
must never style the same property.

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
