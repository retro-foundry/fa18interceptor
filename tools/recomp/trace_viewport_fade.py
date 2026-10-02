"""Compare bounded original/C fade start, palette loads and terminal writes.

Reads the sealed demo state/input. Keeps a small JSON summary and deletes the
capped instruction CSVs after extraction; never produces full RGB streams.
"""
from concurrent.futures import ThreadPoolExecutor
import argparse
import csv
import json
import os
from pathlib import Path
import subprocess

from check_record_region_probe import ROOT


def trace(mode, label, bounds, frames, only):
    output = ROOT / "build/recomp" / f"viewport_{label}_{mode}.csv"
    environment = dict(os.environ, FA18_BOUNDARY_TRACE=str(output),
                       FA18_BOUNDARY_RANGE=bounds, FA18_BOUNDARY_TRACE_MAX_MIB="64")
    command = [str(ROOT / "build/recomp/fa18_recomp.exe"),
               "--state", "captures/native/demo01/state.bin",
               "--input", "captures/native/demo01/input.fa18in",
               "--rom", "local/system/kick13.rom", "--frames", str(frames),
               "--ports", mode]
    if mode == "on" and only:
        command += ["--ports-only", only]
    try:
        subprocess.run(command, cwd=ROOT, env=environment, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        result = []
        with output.open() as stream:
            for row in csv.DictReader(stream):
                if row["kind"] != "instruction":
                    continue
                pc = row["source_pc"]
                if pc in ("C0FA0E", "C0FA12", "C0FA2A", "C0FA32", "C17322", "C1735A", "C1741A"):
                    result.append({"pc": pc, "machine_frame": int(row["frame"]),
                                   "vpos": int(row["vpos"]), "cycle": int(row["cycle"]),
                                   "d0_low_byte": int(row["d0"], 16) & 255,
                                   "a0": row["a0"]})
        return result
    finally:
        output.unlink(missing_ok=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--frames", type=int, default=500)
    parser.add_argument("--only", help="optional registered ON selector")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/figures/native_viewport_fade_checkpoint.json")
    args = parser.parse_args()
    if not 1 <= args.frames <= 1000:
        parser.error("frames must be 1..1000 for a bounded checkpoint")
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    result = {"recording": "captures/native/demo01", "frames": args.frames,
              "source": "fresh OFF", "candidate": "fresh ON " + (args.only or "ALL"),
              "frame_convention": "machine frame counter; RGB output uses one-based frame numbers"}
    for label, bounds in (("start", "C0FA04-C0FA4C"), ("fade", "C172DE-C17456")):
        with ThreadPoolExecutor(max_workers=2) as pool:
            jobs = {mode: pool.submit(trace, mode, label, bounds, args.frames, args.only)
                    for mode in ("off", "on")}
            result[label] = {mode: job.result() for mode, job in jobs.items()}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"fade checkpoint: {args.output}")
    for mode in ("off", "on"):
        reset = [r["machine_frame"] for r in result["start"][mode] if r["pc"] == "C0FA32"]
        terminal = [r["machine_frame"] for r in result["fade"][mode] if r["pc"] == "C1741A"]
        print(f"{mode}: current-mode reset machine frames {reset}; terminal writes {terminal}")


if __name__ == "__main__":
    main()
