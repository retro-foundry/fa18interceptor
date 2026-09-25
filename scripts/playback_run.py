"""Open a sealed capture in visible Engine9000 playback with its saved state."""

from __future__ import annotations

import argparse
import ctypes as C
from ctypes import wintypes as W
import json
import os
from pathlib import Path
import subprocess
import time


ROOT = Path(__file__).resolve().parents[1]
ENGINE = ROOT / "tools" / "engine9000" / "e9k-debugger"


def find_window(pid: int, timeout: float = 15) -> int:
    user = C.windll.user32
    found: list[int] = []
    callback_type = C.WINFUNCTYPE(W.BOOL, W.HWND, W.LPARAM)

    @callback_type
    def visit(hwnd: int, _unused: int) -> bool:
        owner = W.DWORD()
        user.GetWindowThreadProcessId(hwnd, C.byref(owner))
        if owner.value == pid and user.IsWindowVisible(hwnd):
            title = C.create_unicode_buffer(512)
            user.GetWindowTextW(hwnd, title, 512)
            if "ENGINE9000" in title.value.upper():
                found.append(hwnd)
        return True

    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        user.EnumWindows(visit, 0)
        if found:
            return found[0]
        time.sleep(0.1)
    raise RuntimeError("Engine9000 window did not appear")


def restore_saved_state(hwnd: int) -> None:
    user = C.windll.user32
    user.PostMessageW.argtypes = [W.HWND, W.UINT, W.WPARAM, W.LPARAM]
    user.ShowWindow(hwnd, 9)  # SW_RESTORE
    user.SetForegroundWindow(hwnd)
    for key in (0x11, 0x12):  # Ctrl, Alt
        scan = user.MapVirtualKeyW(key, 0)
        user.PostMessageW(hwnd, 0x100, key, 1 | (scan << 16))
    scan = user.MapVirtualKeyW(0x52, 0)
    user.PostMessageW(hwnd, 0x100, 0x52, 1 | (scan << 16))
    time.sleep(0.05)
    user.PostMessageW(hwnd, 0x101, 0x52, 1 | (scan << 16) | (3 << 30))
    for key in (0x12, 0x11):
        scan = user.MapVirtualKeyW(key, 0)
        user.PostMessageW(hwnd, 0x101, key, 1 | (scan << 16) | (3 << 30))


def delayed_playback(run: Path, delay: int) -> Path:
    """Create a derived playback whose input starts after GUI timeline restore."""
    source = run / "playback.e9k"
    lines = source.read_text(encoding="ascii").splitlines()
    if not lines or lines[0] != "E9K_INPUT_V1":
        raise ValueError(f"{source} is not an E9K playback")
    shifted = [lines[0]]
    for line in lines[1:]:
        fields = line.split()
        if len(fields) < 3 or fields[0] != "F":
            raise ValueError(f"invalid playback row: {line!r}")
        fields[1] = str(int(fields[1]) + delay)
        shifted.append(" ".join(fields))
    output = ROOT / "build" / "playback" / f"{run.name}_delay_{delay}.e9k"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(shifted) + "\n", encoding="ascii")
    return output


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True,
                        help="Sealed capture directory containing config, saves, appdata, and playback.")
    parser.add_argument("--window-size", default="1400x900")
    parser.add_argument("--restore-delay", type=float, default=2.0,
                        help="seconds to wait after window creation before restoring the saved timeline")
    parser.add_argument("--input-delay-frames", type=int, default=180,
                        help="delay all replay events so timeline restore happens before input begins")
    args = parser.parse_args()
    run = args.run.resolve()
    required = ("config.uae", "initial_state.bin", "playback.e9k", "saves", "appdata")
    missing = [name for name in required if not (run / name).exists()]
    if missing:
        raise FileNotFoundError(f"{run} missing: {', '.join(missing)}")
    original_config = ROOT / "local" / "fa18.uae"
    if not original_config.is_file() or original_config.read_bytes() != (run / "config.uae").read_bytes():
        raise ValueError("sealed config differs from local/fa18.uae; cannot resolve its historical save-slot name")
    # Engine9000 keys the save slot from the UAE filename. Captures retain the
    # original fa18.uae.e9k-save slot, even though their config copy is named
    # config.uae for archival clarity.
    if args.input_delay_frames < 0:
        raise ValueError("input-delay-frames must be nonnegative")
    playback = delayed_playback(run, args.input_delay_frames)
    command = [str(ENGINE / "e9k-debugger.exe"), "--amiga", "--uae", str(original_config),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(run / "saves"),
               "--playback", str(playback), "--window-size", args.window_size]
    environment = os.environ.copy()
    environment["APPDATA"] = str(run / "appdata")
    process = subprocess.Popen(command, cwd=ENGINE, env=environment)
    hwnd = find_window(process.pid)
    if args.restore_delay < 0:
        raise ValueError("restore-delay must be nonnegative")
    if args.restore_delay:
        time.sleep(args.restore_delay)
    restore_saved_state(hwnd)
    print(json.dumps({"pid": process.pid, "window": hwnd, "run": str(run),
                      "frame_counter": "visible in Engine9000 status bar as FRAME:<n>",
                      "restore_delay": args.restore_delay,
                      "input_delay_frames": args.input_delay_frames,
                      "derived_playback": str(playback)}))


if __name__ == "__main__":
    main()
