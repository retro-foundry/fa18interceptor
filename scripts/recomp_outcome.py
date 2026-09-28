"""Frame agreement over a whole recorded run: the timing metric.

Renders Engine9000 oracle frames at the given frame numbers (cached under
build/recomp/oracle/<run>/f<N>) and native frames from the run's restore
state with its playback, then prints the pixel match ratio per frame.

  python scripts/recomp_outcome.py --run captures/run060 100 500 1000 9545
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


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--run", type=Path, required=True)
    parser.add_argument("--state", default="restored-state.bin")
    parser.add_argument("frames", type=int, nargs="+")
    args = parser.parse_args()
    cache = ROOT / "build/recomp/oracle" / args.run.name
    out = ROOT / "build/recomp/outcome" / args.run.name
    out.mkdir(parents=True, exist_ok=True)
    for frame in args.frames:
        oracle = cache / f"f{frame}"
        if not (oracle / "screen.png").is_file():
            subprocess.run([sys.executable, str(ROOT / "scripts/engine9000_bridge.py"),
                            "--restore", str(args.run / args.state), "--config", str(args.run / "config.uae"),
                            "--playback", str(args.run / "playback.e9k"), "--frames", str(frame),
                            "--output", str(oracle)], check=True, stdout=subprocess.DEVNULL)
        native = out / f"f{frame}.ppm"
        subprocess.run([str(EXE), "--state", str(args.run / args.state), "--rom", str(ROM),
                        "--replay", str(args.run / "playback.e9k"), "--frames", str(frame),
                        "--ppm", str(native)], check=True, stdout=subprocess.DEVNULL)
        result = json.loads(subprocess.run(
            [sys.executable, str(ROOT / "scripts/compare_recomp_oracle.py"), str(native),
             str(oracle / "screen.png"), "--diff", str(out / f"f{frame}_diff.png")],
            check=True, capture_output=True, text=True).stdout)
        print(f"frame {frame:6d}: {result['ratio']:.4f}", flush=True)


if __name__ == "__main__":
    main()
