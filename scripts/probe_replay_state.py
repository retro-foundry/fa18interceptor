"""Verify byte-level state restoration for a sealed Engine9000 replay."""

from __future__ import annotations

import argparse
import ctypes as C
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

from playback_run import ENGINE, ROOT, working_copy
from record_run import key, window


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def probe(run: Path) -> dict[str, object]:
    work = working_copy(run)
    playback = run / "playback.e9k"
    metadata = json.loads((run / "run.json").read_text(encoding="utf-8"))
    restore_frame = metadata.get("restore_frame")
    if not isinstance(restore_frame, int) or restore_frame < 0:
        raise ValueError("capture lacks an explicit boot restore frame")
    dump = work / "restored-state.bin"
    command = [str(ENGINE / "e9k-debugger.exe"), "--amiga", "--uae", str(ROOT / "local" / "fa18.uae"),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(work / "saves"),
               "--playback", str(playback), "--headless"]
    environment = os.environ.copy()
    environment["APPDATA"] = str(work / "appdata")
    environment["E9K_BOOT_RESTORE_FRAME"] = str(restore_frame)
    environment["E9K_REPLAY_STATE_DUMP"] = str(dump)
    process = subprocess.Popen(command, cwd=ENGINE, env=environment)
    try:
        deadline = time.monotonic() + 10
        while not dump.is_file() and time.monotonic() < deadline:
            time.sleep(0.05)
        if not dump.is_file():
            raise RuntimeError("timed out waiting for restored-state.bin")
    finally:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
    return {"work": str(work), "dump": str(dump), "sha256": sha256(dump),
            "size": dump.stat().st_size}


def probe_gui_restore(run: Path) -> dict[str, object]:
    """Run the same visible Ctrl+Alt+R path used while recording."""
    work = working_copy(run)
    dump = work / "restored-state.bin"
    command = [str(ENGINE / "e9k-debugger.exe"), "--amiga", "--uae", str(ROOT / "local" / "fa18.uae"),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(work / "saves")]
    environment = os.environ.copy()
    environment["APPDATA"] = str(work / "appdata")
    environment["E9K_REPLAY_STATE_DUMP"] = str(dump)
    process = subprocess.Popen(command, cwd=ENGINE, env=environment)
    try:
        hwnd = window(process.pid)
        C.windll.user32.ShowWindow(hwnd, 0)
        time.sleep(2)
        key(hwnd, 0x52, modifiers=(0x11, 0x12))
        deadline = time.monotonic() + 10
        while not dump.is_file() and time.monotonic() < deadline:
            time.sleep(0.05)
        if not dump.is_file():
            raise RuntimeError("timed out waiting for GUI-restored state dump")
    finally:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
    return {"work": str(work), "dump": str(dump), "sha256": sha256(dump), "size": dump.stat().st_size}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True)
    parser.add_argument("--repetitions", type=int, default=2)
    parser.add_argument("--compare-gui", action="store_true",
                        help="Also exercise the original Ctrl+Alt+R restoration path.")
    args = parser.parse_args()
    run = args.run.resolve()
    metadata = json.loads((run / "run.json").read_text(encoding="utf-8"))
    expected = run / metadata.get("restored_state", "initial_state.bin")
    if args.repetitions < 1:
        raise ValueError("--repetitions must be positive")
    results = [probe(run) for _ in range(args.repetitions)]
    hashes = {result["sha256"] for result in results}
    output: dict[str, object] = {"recorded_state": str(expected), "recorded_sha256": sha256(expected),
                      "recorded_size": expected.stat().st_size, "results": results,
                      "restores_identical": len(hashes) == 1,
                      "restores_match_recorded": hashes == {sha256(expected)}}
    if args.compare_gui:
        manual = probe_gui_restore(run)
        output["gui_restore"] = manual
        output["queued_restore_matches_gui"] = manual["sha256"] in hashes
    print(json.dumps(output, indent=2))


if __name__ == "__main__":
    main()
