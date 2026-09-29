"""First frames where native game RAM differs from the Engine9000 oracle.

Compares Chip and Slow RAM after N frames of a recorded run, ignoring the
two stack areas (dead stack words depend on interrupt timing).

  python scripts/recomp_state_diff.py --run captures/uae/run060 100 200 300
"""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build/recomp/fa18_recomp.exe"
ROM = ROOT / "local/system/kick13.rom"
IGNORE = [(0xC53000, 0xC55100), (0xC7E000, 0xC80000)]


def address(i: int) -> int:
    return i if i < 0x80000 else 0xC00000 + i - 0x80000


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--run", type=Path, required=True)
    parser.add_argument("frames", type=int, nargs="+")
    args = parser.parse_args()
    cache = ROOT / "build/recomp/oracle" / args.run.name
    ram = ROOT / "build/recomp/state_diff.bin"
    for frame in args.frames:
        oracle = cache / f"f{frame}"
        if not (oracle / "slow.bin").is_file():
            subprocess.run([sys.executable, str(ROOT / "scripts/engine9000_bridge.py"),
                            "--restore", str(args.run / "restored-state.bin"), "--config", str(args.run / "config.uae"),
                            "--playback", str(args.run / "playback.e9k"), "--frames", str(frame),
                            "--output", str(oracle)], check=True, stdout=subprocess.DEVNULL)
        subprocess.run([str(EXE), "--state", str(args.run / "restored-state.bin"), "--rom", str(ROM),
                        "--replay", str(args.run / "playback.e9k"), "--frames", str(frame),
                        "--ram-out", str(ram)], check=True, stdout=subprocess.DEVNULL)
        n = ram.read_bytes()[:0x100000]
        o = (oracle / "chip.bin").read_bytes() + (oracle / "slow.bin").read_bytes()
        diffs = [i for i in range(len(o)) if n[i] != o[i]
                 and not any(lo <= address(i) < hi for lo, hi in IGNORE)]
        shown = " ".join(f"{address(i):06X}:{o[i]:02X}/{n[i]:02X}" for i in diffs[:10])
        print(f"frame {frame:6d}: {len(diffs):6d} bytes differ (oracle/native) {shown}", flush=True)


if __name__ == "__main__":
    main()
