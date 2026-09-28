"""Frame parity for the translated port: the project's progress metric.

Runs fa18_recomp from a bridge snapshot of a sealed run and compares every
native frame with the Engine9000 oracle frame of the same number. Oracle
frames are rendered once with scripts/engine9000_bridge.py and cached under
build/recomp/oracle/<run>/. Reports per-frame match ratios and the first
diverging frame.

  python scripts/recomp_parity.py --start 392 --frames 10
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build/recomp/fa18_recomp.exe"
ROM = ROOT / "local/system/kick13.rom"


def bridge(run: Path, frames: int, output: Path, trace_frames: int = 0) -> None:
    command = [sys.executable, str(ROOT / "scripts/engine9000_bridge.py"),
               "--restore", str(run / "restored-state.bin"), "--config", str(run / "config.uae"),
               "--playback", str(run / "playback.e9k"), "--frames", str(frames), "--output", str(output)]
    if trace_frames:
        command += ["--trace-frames", str(trace_frames)]
    subprocess.run(command, check=True, stdout=subprocess.DEVNULL)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--run", type=Path, default=ROOT / "captures/run075")
    parser.add_argument("--start", type=int, default=392, help="snapshot frame the native run starts from")
    parser.add_argument("--frames", type=int, default=10)
    parser.add_argument("--no-recomp", action="store_true", help="interpreter-only baseline")
    args = parser.parse_args()

    cache = ROOT / "build/recomp/oracle" / args.run.name
    cache.mkdir(parents=True, exist_ok=True)
    start = cache / f"f{args.start}"
    if not (start / "state.bin").is_file():
        bridge(args.run, args.start, start)
    for frame in range(args.start + 1, args.start + args.frames + 1):
        if not (cache / f"f{frame}" / "screen.png").is_file():
            bridge(args.run, frame, cache / f"f{frame}")

    native = ROOT / "build/recomp/parity" / args.run.name
    native.mkdir(parents=True, exist_ok=True)
    command = [str(EXE), "--state", str(start / "state.bin"), "--rom", str(ROM),
               "--frames", str(args.frames), "--ppm-every", str(native)]
    if args.no_recomp:
        command.append("--no-recomp")
    stats = json.loads(subprocess.run(command, check=True, capture_output=True, text=True).stdout)

    rows, first = [], None
    for i in range(1, args.frames + 1):
        frame = args.start + i
        result = subprocess.run([sys.executable, str(ROOT / "scripts/compare_recomp_oracle.py"),
                                 str(native / f"frame_{i:03d}.ppm"),
                                 str(cache / f"f{frame}" / "screen.png")],
                                check=True, capture_output=True, text=True)
        match = json.loads(result.stdout)
        rows.append({"frame": frame, **match})
        if first is None and match["matching"] != match["total"]:
            first = frame
    report = {"run": args.run.name, "start": args.start, "frames": args.frames,
              "first_diverging_frame": first, "exact_frames": sum(r["ratio"] == 1.0 for r in rows),
              "per_frame": rows, "native": stats}
    (native / "parity.json").write_text(json.dumps(report, indent=2) + "\n")
    for r in rows:
        print(f"{r['frame']}: {r['ratio']:.4f}")
    print(json.dumps({k: report[k] for k in ("run", "first_diverging_frame", "exact_frames", "frames")}))


if __name__ == "__main__":
    main()
