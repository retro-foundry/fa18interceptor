"""Capture one exact native-host replay state from a sealed recording."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess

from render_run import ENGINE, ROOT, load_run, sha256
from profile_window import read_events


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, required=True, help="sealed capture directory")
    parser.add_argument("--frame", type=int, required=True, help="Engine9000 GUI frame counter")
    parser.add_argument("--output", type=Path, required=True, help="new checkpoint directory")
    parser.add_argument("--engine", type=Path, default=ENGINE)
    args = parser.parse_args()
    run = args.run.resolve()
    output = args.output.resolve()
    if args.frame < 0:
        raise ValueError("--frame must be non-negative")
    if output.exists():
        raise FileExistsError(f"refusing to mix checkpoint results into existing {output}")
    metadata, expected_state, config = load_run(run)
    engine = args.engine.resolve()
    if not engine.is_file():
        raise FileNotFoundError(engine)
    last_input = max(read_events(run / "playback.e9k"), default=0)
    if args.frame > last_input:
        raise ValueError(f"frame {args.frame} is after final input frame {last_input}")

    output.mkdir(parents=True)
    shutil.copyfile(run / "playback.e9k", output / "smoketest.inp")
    shutil.copytree(run / "saves", output / "saves")
    shutil.copytree(run / "appdata", output / "appdata")
    checkpoint = output / f"frame_{args.frame:05d}_state.bin"
    restored = output / "restored-state.bin"
    environment = os.environ.copy()
    environment["APPDATA"] = str(output / "appdata")
    environment["E9K_BOOT_RESTORE_FRAME"] = str(metadata["restore_frame"])
    environment["E9K_REPLAY_STATE_DUMP"] = str(restored)
    environment["E9K_REPLAY_FRAME_DUMP"] = str(checkpoint)
    environment["E9K_REPLAY_FRAME_DUMP_FRAME"] = str(args.frame)
    environment["E9K_FRAME_CAPTURE_STEP"] = str(last_input + 1)
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
    if not restored.is_file() or sha256(restored) != sha256(expected_state):
        raise RuntimeError("native checkpoint run did not restore the recording's canonical state")
    if not checkpoint.is_file():
        raise RuntimeError(f"Engine9000 did not write checkpoint for frame {args.frame}")
    result = {"run": str(run), "frame": args.frame, "checkpoint": str(checkpoint),
              "checkpoint_sha256": sha256(checkpoint), "checkpoint_size": checkpoint.stat().st_size,
              "restored_state_sha256": sha256(restored), "last_input_frame": last_input}
    (output / "checkpoint.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
