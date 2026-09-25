"""Open a sealed capture in visible Engine9000 playback from its boot state."""

from __future__ import annotations

import argparse
import ctypes as C
from ctypes import wintypes as W
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
ENGINE = ROOT / "build" / "engine9000-replay"


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


def working_copy(run: Path) -> Path:
    """Copy mutable Engine9000 state so playback never edits sealed evidence."""
    work_root = ROOT / "build" / "playback"
    work_root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix=f"{run.name}_", dir=work_root))
    shutil.copytree(run / "saves", work / "saves")
    shutil.copytree(run / "appdata", work / "appdata")
    return work


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True,
                        help="Sealed capture directory containing config, saves, appdata, and playback.")
    parser.add_argument("--window-size", default="1400x900")
    parser.add_argument("--engine", type=Path, default=ENGINE / "e9k-debugger.exe",
                        help="Engine9000 binary with boot-time snapshot restoration.")
    args = parser.parse_args()
    run = args.run.resolve()
    required = ("config.uae", "initial_state.bin", "inputs.e9k", "playback.e9k", "saves", "appdata")
    missing = [name for name in required if not (run / name).exists()]
    if missing:
        raise FileNotFoundError(f"{run} missing: {', '.join(missing)}")
    metadata = json.loads((run / "run.json").read_text(encoding="utf-8"))
    restore_frame = metadata.get("restore_frame")
    if (metadata.get("recording_protocol") != "engine9000-boot-restore-v1" or
            not isinstance(restore_frame, int) or restore_frame < 0):
        raise ValueError("capture lacks an explicit boot restore frame and cannot be replayed deterministically; record it with scripts/record_run.py")
    restored_name = metadata.get("restored_state")
    restored_hash = metadata.get("restored_state_sha256")
    restored_state = run / restored_name if isinstance(restored_name, str) else None
    if not restored_state or not restored_state.is_file() or not isinstance(restored_hash, str):
        raise ValueError("deterministic capture lacks canonical restore-state evidence")
    if hashlib.sha256(restored_state.read_bytes()).hexdigest() != restored_hash:
        raise ValueError("canonical restore-state evidence hash does not match run.json")
    original_config = ROOT / "local" / "fa18.uae"
    if not original_config.is_file() or original_config.read_bytes() != (run / "config.uae").read_bytes():
        raise ValueError("sealed config differs from local/fa18.uae; cannot resolve its historical save-slot name")
    # Engine9000 keys the save slot from the UAE filename. Captures retain the
    # original fa18.uae.e9k-save slot, even though their config copy is named
    # config.uae for archival clarity.
    engine = args.engine.resolve()
    if not engine.is_file():
        raise FileNotFoundError(engine)
    work = working_copy(run)
    playback = run / "playback.e9k"
    command = [str(engine), "--amiga", "--uae", str(original_config),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(work / "saves"),
               "--playback", str(playback), "--window-size", args.window_size]
    environment = os.environ.copy()
    environment["APPDATA"] = str(work / "appdata")
    environment["E9K_BOOT_RESTORE_FRAME"] = str(restore_frame)
    environment["E9K_REPLAY_STATE_DUMP"] = str(work / "restored-state.bin")
    process = subprocess.Popen(command, cwd=engine.parent, env=environment)
    hwnd = find_window(process.pid)
    print(json.dumps({"pid": process.pid, "window": hwnd, "run": str(run),
                      "frame_counter": "visible in Engine9000 status bar as FRAME:<n>",
                      "engine": str(engine), "working_copy": str(work),
                      "playback": str(playback), "restored_at_boot": True,
                      "restored_state_dump": str(work / "restored-state.bin"),
                      "recorded_restored_state": str(restored_state),
                      "restore_frame": restore_frame}))


if __name__ == "__main__":
    main()
