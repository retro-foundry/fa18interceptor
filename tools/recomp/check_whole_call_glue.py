"""Prove readable whole-call C independently of registered instruction steps.

Builds an isolated registry variant using the shared object cache, then checks
shadow and sandbox calls on every sealed recording. The normal runner and
source registry are unchanged. Live timing is checked separately by
scripts/recomp_live_check.sh with PORTS_ONLY set to the selected entries.
"""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import argparse
import json
import re
import subprocess

from check_record_region_probe import ROOT


def check_recording(recording, executable, entries):
    results = {}
    base = [str(executable), "--state", str(recording / "state.bin"),
            "--input", str(recording / "input.fa18in"),
            "--rom", "local/system/kick13.rom", "--to-end",
            "--ports-only", ",".join(entries)]
    for mode in ("shadow", "sandbox"):
        report = ROOT / "build/recomp" / f"whole_call_{'_'.join(entries)}_{recording.name}_{mode}.json"
        run = subprocess.run(base + ["--ports", mode, "--ports-report", str(report)],
                             cwd=ROOT, stdout=subprocess.DEVNULL,
                             stderr=subprocess.PIPE, text=True)
        if run.returncode:
            raise RuntimeError(f"{recording.name} {mode}: {run.stderr.strip()}")
        rows = {r["entry"]: r for r in json.loads(report.read_text())
                if r["entry"] in entries}
        if set(rows) != set(entries) or any(r["mismatched"] for r in rows.values()):
            raise RuntimeError(f"{recording.name} {mode}: missing or mismatched entry")
        results[mode] = rows
        print(f"{recording.name} {mode}: " + ", ".join(
            f"{entry} {rows[entry]['matched']} matches" for entry in entries), flush=True)
    return results


def main(default_entries=()):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("entries", nargs="*" if default_entries else "+",
                        default=default_entries,
                        help="registered six-digit hexadecimal entries")
    parser.add_argument("--jobs", type=int, default=3)
    args = parser.parse_args()
    entries = tuple(entry.upper().removeprefix("0X") for entry in args.entries)
    if any(not re.fullmatch(r"[0-9A-F]{6}", e) for e in entries):
        parser.error("entries must be six-digit hexadecimal addresses")
    if args.jobs <= 0:
        parser.error("--jobs must be positive")
    registry = (ROOT / "port/game/glue/ports.c").read_text()
    for entry in entries:
        # Retain entry, whole-call glue and name; disable only the step path.
        registry, count = re.subn(
            r"\{0x" + entry + r",[^}]+\}",
            lambda m: ",".join(m[0].split(",")[:3]) + ", 0}", registry)
        if count != 1:
            raise RuntimeError(f"expected exactly one registered {entry}")
    output = ROOT / "build/recomp"
    output.mkdir(parents=True, exist_ok=True)
    variant = output / "selected_whole_call_registry.c"
    if not variant.exists() or variant.read_text() != registry:
        variant.write_text(registry)
    executable = output / "selected_whole_call.exe"
    subprocess.run([
        "python", "scripts/build_recomp.py", "--output", str(executable.relative_to(ROOT)),
        "--replace-source", "port/game/glue/ports.c=" + str(variant.relative_to(ROOT)),
    ], cwd=ROOT, check=True)
    recordings = sorted(p.parent for p in (ROOT / "captures/native").glob("*/input.fa18in"))
    if not recordings:
        raise RuntimeError("no sealed recordings")
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(lambda d: check_recording(d, executable, entries), recordings))
    for entry in entries:
        if not any(result[mode][entry]["matched"] for result in results
                   for mode in ("shadow", "sandbox")):
            raise RuntimeError(f"{entry}: no completed comparisons")
    print("whole-call C proof: " + ", ".join(
        f"{sum(row[mode][e]['matched'] for row in results for e in entries)} {mode} matches"
        for mode in ("shadow", "sandbox")))


if __name__ == "__main__":
    main()
