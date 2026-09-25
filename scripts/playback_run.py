"""Open a sealed capture in visible Engine9000 playback from its boot state."""

from __future__ import annotations

import argparse
import ctypes as C
from ctypes import wintypes as W
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


def effective_playback(run: Path, work: Path) -> tuple[Path, int]:
    """Recreate host input which survived the recorded GUI restore.

    The raw prelude ends with the setup timeline, then its frame number drops
    when Ctrl+Alt+R restores the snapshot.  Core-mouse events queued for that
    final pre-restore frame live in the host, not in the core serialization;
    the original run therefore consumes them at restored frame 1.  Keep the
    sealed recording unchanged and synthesize that handoff only in this
    disposable playback directory.
    """
    metadata = json.loads((run / "run.json").read_text(encoding="utf-8"))
    prelude_bytes = metadata.get("prelude_bytes")
    canonical = run / "playback.e9k"
    if not isinstance(prelude_bytes, int) or prelude_bytes <= 0:
        return canonical, 0
    raw = (run / "inputs.e9k").read_bytes()
    if prelude_bytes > len(raw):
        raise ValueError("recording prelude exceeds inputs.e9k")
    prelude_lines = raw[:prelude_bytes].decode("ascii").splitlines()
    events: list[tuple[int, str]] = []
    for line in prelude_lines:
        parts = line.split()
        if len(parts) >= 3 and parts[0] == "F":
            events.append((int(parts[1]), line))
    reset_index: int | None = None
    previous = -1
    for index, (frame, _line) in enumerate(events):
        if frame < previous:
            reset_index = index
            break
        previous = frame
    if reset_index is None or reset_index == 0:
        return canonical, 0
    handoff_frame = events[reset_index - 1][0]
    carry: list[str] = []
    for frame, line in events[:reset_index]:
        parts = line.split()
        # These input kinds remain in Engine9000's host-side input queues.
        if frame == handoff_frame and len(parts) >= 3 and parts[2] in {"m", "b", "J", "K"}:
            parts[1] = "1"
            carry.append(" ".join(parts))
    post_restore = [line for _frame, line in events[reset_index:]]
    canonical_lines = canonical.read_text(encoding="ascii").splitlines()
    if not canonical_lines or canonical_lines[0] != "E9K_INPUT_V1":
        raise ValueError(f"invalid sealed playback: {canonical}")
    output = work / "replay.e9k"
    output.write_text("E9K_INPUT_V1\n" + "\n".join(carry + post_restore + canonical_lines[1:]) + "\n",
                      encoding="ascii")
    (work / "replay-input.json").write_text(json.dumps({
        "source": str(run / "inputs.e9k"),
        "prelude_bytes": prelude_bytes,
        "restore_handoff_frame": handoff_frame,
        "restored_frame": 1,
        "carried_events": carry,
        "post_restore_events": post_restore,
    }, indent=2) + "\n", encoding="utf-8")
    return output, len(carry)


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
    playback, carried_events = effective_playback(run, work)
    command = [str(engine), "--amiga", "--uae", str(original_config),
               "--system-dir", str(ROOT / "local" / "system"), "--save-dir", str(work / "saves"),
               "--playback", str(playback), "--window-size", args.window_size]
    environment = os.environ.copy()
    environment["APPDATA"] = str(work / "appdata")
    process = subprocess.Popen(command, cwd=engine.parent, env=environment)
    hwnd = find_window(process.pid)
    print(json.dumps({"pid": process.pid, "window": hwnd, "run": str(run),
                      "frame_counter": "visible in Engine9000 status bar as FRAME:<n>",
                      "engine": str(engine), "working_copy": str(work),
                      "playback": str(playback), "restored_at_boot": True,
                      "carried_restore_inputs": carried_events}))


if __name__ == "__main__":
    main()
