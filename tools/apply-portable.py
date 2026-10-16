"""Write the Quiet Dock setup into a portable Windhawk's mod .ini files.

The portable engine watches AppData/Engine/Mods for file changes, so running
this applies the theme, the taskbar height and the dock settings live, with no
admin rights. Usage:

    python tools/apply-portable.py [--windhawk C:/Users/me/Windhawk] [--off]

--off disables the three mods (the stock taskbar comes back).
"""
import argparse
import re
import time
from pathlib import Path

import yaml

REPO = Path(__file__).resolve().parent.parent


def flatten(prefix, value, out):
    """Windhawk stores nested settings as flat keys: a.b[0].c = value."""
    if isinstance(value, dict):
        for k, v in value.items():
            flatten(f"{prefix}.{k}" if prefix else k, v, out)
    elif isinstance(value, list):
        for i, v in enumerate(value):
            flatten(f"{prefix}[{i}]", v, out)
    else:
        out[prefix] = int(value) if isinstance(value, bool) else value


def mod_defaults(source):
    block = re.search(r"// ==WindhawkModSettings==\n/\*\n(.*?)\*/", source, re.S).group(1)
    out = {}
    for item in yaml.safe_load(block):
        key = next(k for k in item if not k.startswith("$"))
        flatten(key, item[key], out)
    return out


def write_ini(path, mod_fields, settings, force=False):
    """Writes the mod's .ini unless only its change time would differ.

    Every write makes the mod reload its settings, and for the Taskbar Styler
    that means tearing down and re-applying the whole theme.
    """
    def render(change_time):
        fields = dict(mod_fields, SettingsChangeTime=change_time)
        lines = ["[Mod]"] + [f"{k}={v}" for k, v in fields.items()]
        lines += ["", "[Settings]"] + [f"{k}={v}" for k, v in settings.items()]
        return "\r\n".join(lines) + "\r\n"

    if path.exists() and not force:
        old = path.read_text(encoding="utf-16")
        old_time = next((l.split("=", 1)[1] for l in old.splitlines()
                         if l.startswith("SettingsChangeTime=")), "")
        if old.splitlines() == render(old_time).splitlines():
            return False
    # UTF-16 with BOM: what GetPrivateProfileStringW reads as Unicode.
    path.write_text(render(mod_fields["SettingsChangeTime"]), encoding="utf-16", newline="")
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--windhawk", default=str(Path.home() / "Windhawk"))
    ap.add_argument("--off", action="store_true")
    ap.add_argument("--log", action="store_true", help="enable Windhawk logging for quiet-dock")
    ap.add_argument("--force", action="store_true", help="rewrite every mod, even if unchanged")
    args = ap.parse_args()
    mods_dir = Path(args.windhawk) / "AppData" / "Engine" / "Mods"
    # Newest build of each mod wins: a loaded DLL can't be overwritten, so a
    # rebuild goes in under a new name and the engine switches to it.
    dlls = {}
    for p in sorted((mods_dir / "64").glob("*.dll"), key=lambda p: p.stat().st_mtime):
        dlls[p.name.split("_")[0]] = p.name

    styler = {"theme": "", "clickThroughTaskbar": 0, "xamlDiagnosticsHandling": "alert"}
    flatten("", yaml.safe_load((REPO / "theme/quiet-dock-liquid-glass.yaml").read_text(encoding="utf-8")), styler)
    height = yaml.safe_load((REPO / "theme/taskbar-height.yaml").read_text(encoding="utf-8"))
    dock = mod_defaults((REPO / "mod/quiet-dock.wh.cpp").read_text(encoding="utf-8"))
    autohide = {}
    flatten("", yaml.safe_load((REPO / "theme/autohide.yaml").read_text(encoding="utf-8")), autohide)

    mods = {
        "taskbar-icon-size": ("1.3.10", height),
        "windows-11-taskbar-styler": ("1.10", styler),
        "local@quiet-dock": ("1.0.0", dock),
        "taskbar-autohide-instant-show": ("2.2", autohide),
    }
    now = int(time.time())
    for mod_id, (version, settings) in mods.items():
        fields = {
            "LibraryFileName": dlls[mod_id],
            "Disabled": 1 if args.off else 0,
            "Include": "explorer.exe",
            "Exclude": "",
            "Architecture": "x86-64",
            "Version": version,
            "SettingsChangeTime": now,
        }
        if args.log and mod_id == "local@quiet-dock":
            fields["LoggingEnabled"] = 1
        changed = write_ini(mods_dir / f"{mod_id}.ini", fields, settings, args.force)
        state = "disabled" if args.off else "enabled"
        print(f"{mod_id}: {len(settings)} settings, {state}{'' if changed else ' (unchanged)'}")


if __name__ == "__main__":
    main()
