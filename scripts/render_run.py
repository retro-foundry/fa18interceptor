"""Render a sealed flight with Engine9000's deterministic host scheduler."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess

from profile_window import read_events


ROOT = Path(__file__).resolve().parents[1]
ENGINE = ROOT / "build" / "engine9000-replay" / "e9k-debugger.exe"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def load_run(run: Path) -> tuple[dict, Path, Path]:
    required = ("config.uae", "playback.e9k", "saves", "appdata", "run.json")
    missing = [name for name in required if not (run / name).exists()]
    if missing:
        raise FileNotFoundError(f"{run} missing: {', '.join(missing)}")
    metadata = json.loads((run / "run.json").read_text(encoding="utf-8"))
    restore_frame = metadata.get("restore_frame")
    if (metadata.get("recording_protocol") != "engine9000-boot-restore-v1" or
            not isinstance(restore_frame, int) or restore_frame < 0):
        raise ValueError("capture lacks an explicit deterministic boot restore frame")
    restored_name = metadata.get("restored_state")
    restored_hash = metadata.get("restored_state_sha256")
    restored_state = run / restored_name if isinstance(restored_name, str) else None
    if not restored_state or not restored_state.is_file() or not isinstance(restored_hash, str):
        raise ValueError("capture lacks canonical restore-state evidence")
    if sha256(restored_state) != restored_hash:
        raise ValueError("canonical restore-state evidence hash does not match run.json")
    config = ROOT / "local" / "fa18.uae"
    if not config.is_file() or config.read_bytes() != (run / "config.uae").read_bytes():
        raise ValueError("sealed config differs from local/fa18.uae")
    return metadata, restored_state, config


def capture_frames(output: Path, every: int) -> list[int]:
    frames: list[int] = []
    for image in output.glob("*.png"):
        try:
            frame = int(image.stem)
        except ValueError:
            continue
        frames.append(frame)
    frames.sort()
    if not frames:
        raise RuntimeError("Engine9000 completed without writing a frame")
    if any(frame % every for frame in frames):
        raise RuntimeError("Engine9000 wrote a frame outside the requested cadence")
    return frames


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True, help="sealed capture directory")
    parser.add_argument("--output", type=Path, required=True, help="new directory for PNGs and validation")
    parser.add_argument("--every", type=int, default=5, help="write one PNG per N emulated frames")
    parser.add_argument("--engine", type=Path, default=ENGINE)
    args = parser.parse_args()
    if args.every < 1:
        raise ValueError("--every must be positive")
    run = args.run.resolve()
    output = args.output.resolve()
    if output.exists():
        raise FileExistsError(f"refusing to mix results into existing {output}")
    metadata, expected_state, config = load_run(run)
    engine = args.engine.resolve()
    if not engine.is_file():
        raise FileNotFoundError(engine)

    output.mkdir(parents=True)
    shutil.copyfile(run / "playback.e9k", output / "smoketest.inp")
    shutil.copytree(run / "saves", output / "saves")
    shutil.copytree(run / "appdata", output / "appdata")
    environment = os.environ.copy()
    environment["APPDATA"] = str(output / "appdata")
    environment["E9K_BOOT_RESTORE_FRAME"] = str(metadata["restore_frame"])
    environment["E9K_REPLAY_STATE_DUMP"] = str(output / "restored-state.bin")
    environment["E9K_FRAME_CAPTURE_STEP"] = str(args.every)
    command = [str(engine), "--amiga", "--uae", str(config),
               "--system-dir", str(ROOT / "local" / "system"),
               "--save-dir", str(output / "saves"), "--remake-smoke", str(output),
               "--headless", "--warp"]
    completed = subprocess.run(command, cwd=engine.parent, env=environment,
                               capture_output=True, text=True, check=False)
    (output / "engine.stdout.log").write_text(completed.stdout, encoding="utf-8")
    (output / "engine.stderr.log").write_text(completed.stderr, encoding="utf-8")
    if completed.returncode:
        raise RuntimeError(f"Engine9000 exited with {completed.returncode}; inspect {output}")
    restored = output / "restored-state.bin"
    if not restored.is_file() or sha256(restored) != sha256(expected_state):
        raise RuntimeError("render did not restore the recording's canonical boot state")
    frames = capture_frames(output, args.every)
    last_event = max(read_events(run / "playback.e9k"), default=0)
    if frames[-1] < last_event:
        raise RuntimeError(f"render stops at {frames[-1]}, before final input event {last_event}")
    result = {
        "run": str(run), "engine": str(engine), "every": args.every,
        "restore_frame": metadata["restore_frame"],
        "restored_state_sha256": sha256(restored), "frame_count": len(frames),
        "first_frame": frames[0], "last_frame": frames[-1],
        "last_input_frame": last_event,
    }
    (output / "render.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
