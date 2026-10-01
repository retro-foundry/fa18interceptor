"""Prove the readable whole-call map and region C independently of their steps.

Builds an isolated registry variant using the shared object cache, then checks
shadow and sandbox calls on every sealed recording. The normal runner and
source registry are unchanged. Live timing is checked separately by
scripts/recomp_live_check.sh with PORTS_ONLY set to these four entries.
"""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import argparse
import json
import re
import subprocess

from check_record_region_probe import ROOT

ENTRIES = ("C2AA9C", "C2AB34", "C2AB5A", "C2B05A")


def check_recording(recording, executable):
    results = {}
    base = [str(executable), "--state", str(recording / "state.bin"),
            "--input", str(recording / "input.fa18in"),
            "--rom", "local/system/kick13.rom", "--to-end",
            "--ports-only", ",".join(ENTRIES)]
    for mode in ("shadow", "sandbox"):
        report = ROOT / "build/recomp" / f"whole_call_{recording.name}_{mode}.json"
        run = subprocess.run(base + ["--ports", mode, "--ports-report", str(report)],
                             cwd=ROOT, stdout=subprocess.DEVNULL,
                             stderr=subprocess.PIPE, text=True)
        if run.returncode:
            raise RuntimeError(f"{recording.name} {mode}: {run.stderr.strip()}")
        rows = {r["entry"]: r for r in json.loads(report.read_text())
                if r["entry"] in ENTRIES}
        if set(rows) != set(ENTRIES) or any(r["mismatched"] for r in rows.values()):
            raise RuntimeError(f"{recording.name} {mode}: missing or mismatched entry")
        results[mode] = rows
        print(f"{recording.name} {mode}: " + ", ".join(
            f"{entry} {rows[entry]['matched']} matches" for entry in ENTRIES), flush=True)
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--jobs", type=int, default=3)
    args = parser.parse_args()
    if args.jobs <= 0:
        parser.error("--jobs must be positive")
    registry = (ROOT / "port/game/glue/ports.c").read_text()
    for entry in ENTRIES:
        # Retain entry, whole-call glue and name; disable only the step path.
        registry, count = re.subn(
            r"\{0x" + entry + r",[^}]+\}",
            lambda m: ",".join(m[0].split(",")[:3]) + ", 0}", registry)
        if count != 1:
            raise RuntimeError(f"expected exactly one registered {entry}")
    output = ROOT / "build/recomp"
    output.mkdir(parents=True, exist_ok=True)
    variant = output / "map_region_whole_call_registry.c"
    if not variant.exists() or variant.read_text() != registry:
        variant.write_text(registry)
    executable = output / "map_region_whole_call.exe"
    subprocess.run([
        "python", "scripts/build_recomp.py", "--output", str(executable.relative_to(ROOT)),
        "--replace-source", "port/game/glue/ports.c=" + str(variant.relative_to(ROOT)),
    ], cwd=ROOT, check=True)
    recordings = sorted(p.parent for p in (ROOT / "captures/native").glob("*/input.fa18in"))
    if not recordings:
        raise RuntimeError("no sealed recordings")
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(lambda d: check_recording(d, executable), recordings))
    for entry in ENTRIES:
        if not any(result[mode][entry]["matched"] for result in results
                   for mode in ("shadow", "sandbox")):
            raise RuntimeError(f"{entry}: no completed comparisons")
    print("whole-call map/region proof: " + ", ".join(
        f"{sum(row[mode][e]['matched'] for row in results for e in ENTRIES)} {mode} matches"
        for mode in ("shadow", "sandbox")))


if __name__ == "__main__":
    main()
