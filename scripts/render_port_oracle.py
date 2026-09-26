"""Render a bounded, deterministic Engine9000 run075 frame oracle.

The sealed input and state remain untouched.  A zero-motion event after the
requested final frame lets Engine9000's smoke renderer terminate cleanly.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

from render_run import ROOT, load_run


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, default=ROOT / "captures/run075")
    parser.add_argument("--last-frame", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.last_frame < 200:
        parser.error("the port oracle starts at run075 frame 200")
    run = args.run.resolve()
    output = args.output.resolve()
    if output.exists():
        raise FileExistsError(output)
    metadata, _state, _config = load_run(run)
    if metadata.get("status") != "sealed":
        raise ValueError(f"{run} is not sealed")
    playback_hash = hashlib.sha256((run / "playback.e9k").read_bytes()).hexdigest()
    if playback_hash != metadata.get("hashes", {}).get("playback.e9k"):
        raise ValueError("sealed playback hash does not match run.json")
    source = (run / "playback.e9k").read_text(encoding="ascii").splitlines()
    if not source or source[0] != "E9K_INPUT_V1":
        raise ValueError("invalid source replay")
    kept = [source[0]]
    for row in source[1:]:
        fields = row.split()
        if len(fields) < 3 or fields[0] != "F":
            raise ValueError(f"invalid replay row: {row}")
        if int(fields[1]) <= args.last_frame:
            kept.append(row)
    # No buttons or motion are delivered by this event.  It only establishes
    # the bounded end of the smoke-test playback.
    kept.append(f"F {args.last_frame + 1} m 4 0 0")
    work = output.parent / (output.name + "_input")
    if work.exists():
        raise FileExistsError(work)
    work.mkdir(parents=True)
    try:
        for name in ("config.uae", "restored-state.bin", "run.json"):
            shutil.copy2(run / name, work / name)
        shutil.copytree(run / "saves", work / "saves")
        shutil.copytree(run / "appdata", work / "appdata")
        (work / "playback.e9k").write_text("\n".join(kept) + "\n", encoding="ascii")
        subprocess.run([sys.executable, str(ROOT / "scripts/render_run.py"),
                        "--run", str(work), "--output", str(output),
                        "--every", "1"], check=True)
        (output / "port_oracle.json").write_text(json.dumps({
            "source_run": str(run), "source_playback": str(run / "playback.e9k"),
            "source_playback_sha256": playback_hash,
            "restored_state_sha256": metadata["restored_state_sha256"],
            "first_port_frame": 200, "last_port_frame": args.last_frame,
            "synthetic_stop_frame": args.last_frame + 1,
            "rendered_from_canonical_restore": True,
        }, indent=2) + "\n", encoding="utf-8")
    finally:
        shutil.rmtree(work)


if __name__ == "__main__":
    main()
