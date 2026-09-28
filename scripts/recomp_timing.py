"""Per-instruction timing against a cycle-exact Engine9000 trace.

Runs fa18_recomp interpreter-only with FA18_TRACE from the trace's start
state, pairs native and reference instructions while their PCs agree, and
reports where native time differs (in colour clocks, the trace's unit),
grouped by instruction and by the memory an instruction touches.

  python scripts/recomp_timing.py build/recomp/trace392 [--top 30]
"""
from __future__ import annotations

import argparse
import collections
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build/recomp/fa18_recomp.exe"
ROM = ROOT / "local/system/kick13.rom"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("trace_dir", type=Path)
    parser.add_argument("--top", type=int, default=30)
    parser.add_argument("--frames", type=int, default=3)
    parser.add_argument("--show", type=int, default=0, help="print the first N differing instructions")
    parser.add_argument("--window", default="", help="only count reference CCK offsets A-B from the trace start")
    parser.add_argument("--by-pc", action="store_true", help="group differences by instruction address")
    parser.add_argument("--opcodes", action="store_true",
                        help="list opcode words whose smallest difference is nonzero (cycle-table errors)")
    args = parser.parse_args()

    ref = [json.loads(line) for line in (args.trace_dir / "trace.jsonl").open()]
    window = tuple(int(x) for x in args.window.split("-")) if args.window else None
    env = dict(os.environ, FA18_TRACE=str(len(ref) + 1))
    command = [str(EXE), "--state", str(args.trace_dir / "state.bin"), "--rom", str(ROM),
               "--frames", str(args.frames), "--no-recomp"]
    run = subprocess.run(command, env=env, capture_output=True, text=True)
    native = []
    for line in run.stderr.splitlines():
        if line.startswith("T "):
            parts = line.split(" | ")
            native.append((int(line.split()[1], 16), int(parts[-1]), line.split()[19:21]))

    by_asm = collections.defaultdict(lambda: [0, 0, 0])  # count, ref, native
    by_opcode = {}  # first word -> [count, min diff, max diff, asm]
    total_ref = total_nat = paired = 0
    shown = 0
    i = j = 0  # reference and native positions
    resyncs = 0
    drifts = []
    while i + 1 < len(ref) and j + 1 < len(native):
        if native[j][0] != ref[i]["pc"] or native[j + 1][0] != ref[i + 1]["pc"]:
            # An interrupt taken at a different instruction: skip ahead to where
            # both streams run the same three instructions again.
            best = None
            for di in range(0, 4000):
                if i + di + 2 >= len(ref): break
                for dj in range(0, 4000 - di):
                    if j + dj + 2 >= len(native): break
                    if (native[j + dj][0] == ref[i + di]["pc"] and native[j + dj + 1][0] == ref[i + di + 1]["pc"]
                            and native[j + dj + 2][0] == ref[i + di + 2]["pc"] and (di or dj)):
                        best = (di, dj)
                        break
                if best: break
            if not best:
                print(f"no resync after instruction {i}")
                break
            i += best[0]; j += best[1]; resyncs += 1
            continue
        r = ref[i + 1]["cycles"] - ref[i]["cycles"]
        n = (native[j + 1][1] - native[j][1]) / 2
        # Drift: native time minus reference time at the same instruction,
        # both measured from the start of the trace (CCKs; + = native late).
        drift = (native[j][1] - native[0][1]) / 2 - (ref[i]["cycles"] - ref[0]["cycles"])
        if paired % 4000 == 0:
            drifts.append((ref[i]["frame"], drift))
        offset = ref[i]["cycles"] - ref[0]["cycles"]
        if window and not (window[0] <= offset < window[1]):
            i += 1; j += 1
            continue
        key = ref[i]["asm"].split()[0]
        operands = ref[i]["asm"][len(key):].strip()
        if "$bf" in operands: key += " CIA"
        elif "$dff" in operands: key += " CUSTOM"
        if args.by_pc: key = f"{ref[i]['pc']:06X} {ref[i]['asm']}"
        slot = by_asm[key]
        slot[0] += 1; slot[1] += r; slot[2] += n
        total_ref += r; total_nat += n; paired += 1
        word = ref[i]["bytes"][:4]
        entry = by_opcode.setdefault(word, [0, r - n, r - n, ref[i]["asm"]])
        entry[0] += 1; entry[1] = min(entry[1], r - n); entry[2] = max(entry[2], r - n)
        if r != n and shown < args.show:
            print(f"{i:6d} {ref[i]['pc']:06X} ref {r:4d} nat {n:6.1f}  v={native[j][2][0]} h={native[j][2][1]}  {ref[i]['asm']}")
            shown += 1
        i += 1; j += 1
    print(f"{resyncs} resyncs; drift (CCK, + = native late) by frame: " +
          " ".join(f"{f}:{d:+.0f}" for f, d in drifts))
    print(f"paired {paired} instructions: reference {total_ref} CCK, native {total_nat:.0f} CCK "
          f"({100 * (total_nat - total_ref) / max(1, total_ref):+.2f}%)")
    if args.opcodes:
        for word, (count, low, high, asm) in sorted(by_opcode.items()):
            if low != 0:
                print(f"opcode {word} x{count:<6d} ref-native min {low:+5.1f} max {high:+5.1f}  {asm}")
        return
    rows = sorted(by_asm.items(), key=lambda kv: -abs(kv[1][1] - kv[1][2]))
    print(f"{'instruction':24s} {'count':>7s} {'ref':>9s} {'native':>9s} {'diff':>8s}")
    for key, (count, r, n) in rows[:args.top]:
        print(f"{key:40s} {count:7d} {r:9d} {n:9.0f} {n - r:+8.0f}")


if __name__ == "__main__":
    main()
