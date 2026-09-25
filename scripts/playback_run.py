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
    for key in (0x11, 0x12, 0x52):  # Ctrl, Alt, R
        scan = user.MapVirtualKeyW(key, 0)
        user.PostMessageW(hwnd, 0x100, key, 1 | (scan << 16))
    time.sleep(0.05)
    for key in (0x52, 0x12, 0x11):
        scan = user.MapVirtualKeyW(key, 0)
        user.PostMessageW(hwnd, 0x101, key, 1 | (scan << 16) | (3 << 30))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True,
                        help="Sealed capture directory containing config, saves, appdata, and playback.")
    parser.add_argument("--window-size", default="1400x900")
    args = parser.parse_args()
    run = args.run.resolve()
    required = ("config.uae", "initial_state.bin", "playback.e9k", "saves", "appdata")
    missing = [name for name in required if not (run / name).exists()]
    if missing:
        raise FileNotFoundError(f"{run} missing: {', '.join(missing)}")
    command = [str(ENGINE / "e9k-debugger.exe"), "--amiga", "--uae", str(run / "config.uae"),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(run / "saves"),
               "--playback", str(run / "playback.e9k"), "--window-size", args.window_size]
    environment = os.environ.copy()
    environment["APPDATA"] = str(run / "appdata")
    process = subprocess.Popen(command, cwd=ENGINE, env=environment)
    hwnd = find_window(process.pid)
    time.sleep(2)
    restore_saved_state(hwnd)
    print(json.dumps({"pid": process.pid, "window": hwnd, "run": str(run),
                      "frame_counter": "visible in Engine9000 status bar as FRAME:<n>"}))


if __name__ == "__main__":
    main()
