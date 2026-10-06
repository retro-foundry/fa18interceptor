#!/usr/bin/env python3
"""Rank registered C-port timing drift against one bounded source replay.

Each positional probe is an entry address, a comma-separated entry group, or
ALL for the complete registered set.
The source stream is generated once, the candidate stream is overwritten for
each probe, and both streams are removed unless --keep is requested.
"""

from __future__ import annotations

import argparse
import json
import mmap
import os
from pathlib import Path
import re
import subprocess
from compare_recomp_frames import compare_frames, fade_palette


ROOT = Path(__file__).resolve().parents[1]
WIDTH = 320
HEIGHT = 256
BYTES_PER_FRAME = WIDTH * HEIGHT * 2


def ranked_fixed_entries(report: Path, limit: int) -> list[str]:
    registry = (ROOT / "port/game/glue/ports.c").read_text(encoding="utf-8")
    pattern = re.compile(
        r'\{0x([0-9A-Fa-f]{6}),[^\n]*?"[^"]+",\s*(-?\d+)(?:\s*[,}])')
    charges = {match.group(1).upper(): int(match.group(2))
               for match in pattern.finditer(registry)}
    rows = json.loads(report.read_text(encoding="utf-8"))
    ranked = []
    for row in rows:
        entry = row["entry"]
        charge = charges.get(entry, 0)
        if charge > 0 and row["compared"]:
            score = abs(charge - row["mean_cycles"]) * row["calls"]
            ranked.append((score, entry))
    return [entry for _score, entry in sorted(ranked, reverse=True)[:limit]]


def normalize_probe(value: str) -> str:
    if value.strip().upper() == "ALL":
        return "ALL"
    entries = []
    for entry in value.split(","):
        entry = entry.strip().upper().removeprefix("$").removeprefix("0X")
        if len(entry) != 6 or any(ch not in "0123456789ABCDEF" for ch in entry):
            raise argparse.ArgumentTypeError(f"invalid game entry address: {entry!r}")
        entries.append(entry)
    if not entries:
        raise argparse.ArgumentTypeError("empty entry group")
    return ",".join(entries)


def replay(executable: Path, recording: Path, rom: Path, frames: int,
           mode: str, output: Path, only: str | None = None,
           indices: Path | None = None, ram: Path | None = None) -> None:
    command = [str(executable), "--state", str(recording / "state.bin"),
               "--input", str(recording / "input.fa18in"), "--frames", str(frames),
               "--rom", str(rom), "--ports", mode, "--rgb444", str(output)]
    if only:
        command += ["--ports-only", only]
    if indices:
        command += ["--index8", str(indices)]
    if ram:
        command += ["--ram-out", str(ram)]
    environment = os.environ.copy()
    required_mib = (frames * BYTES_PER_FRAME + (1 << 20) - 1) >> 20
    environment["FA18_RGB444_MAX_MIB"] = str(required_mib + 1)
    result = subprocess.run(command, cwd=ROOT, env=environment,
                            stdout=subprocess.DEVNULL, stderr=subprocess.PIPE,
                            text=True)
    if result.returncode:
        detail = result.stderr.strip()
        raise RuntimeError(f"replay exited {result.returncode}: {detail}")


def first_difference(reference: Path, actual: Path, frames: int) -> tuple[int, int] | None:
    expected_size = frames * BYTES_PER_FRAME
    if reference.stat().st_size != expected_size or actual.stat().st_size != expected_size:
        raise RuntimeError(
            f"expected {expected_size} stream bytes; got "
            f"{reference.stat().st_size} source and {actual.stat().st_size} candidate")
    with reference.open("rb") as reference_file, actual.open("rb") as actual_file:
        with mmap.mmap(reference_file.fileno(), 0, access=mmap.ACCESS_READ) as source:
            with mmap.mmap(actual_file.fileno(), 0, access=mmap.ACCESS_READ) as candidate:
                for index in range(frames):
                    start = index * BYTES_PER_FRAME
                    end = start + BYTES_PER_FRAME
                    if source[start:end] == candidate[start:end]:
                        continue
                    pixels = sum(
                        source[offset:offset + 2] != candidate[offset:offset + 2]
                        for offset in range(start, end, 2))
                    return index + 1, pixels
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("probes", nargs="*", type=normalize_probe,
                        metavar="ENTRY[,ENTRY...]", help="registered entry, entry group, or ALL")
    parser.add_argument("--frames", type=int, default=500)
    parser.add_argument("--recording", type=Path,
                        default=ROOT / "captures/native/demo01")
    parser.add_argument("--executable", type=Path,
                        default=ROOT / "build/recomp/fa18_recomp.exe")
    parser.add_argument("--rom", type=Path, default=ROOT / "local/system/kick13.rom")
    parser.add_argument("--keep", action="store_true",
                        help="retain the final source and candidate streams")
    parser.add_argument("--rank-fixed", type=int, default=0, metavar="N",
                        help="also probe the N largest fixed-charge drifts in a gate report")
    parser.add_argument("--report", type=Path,
                        default=ROOT / "build/recomp/ports_report_demo01.json")
    parser.add_argument("--differences-only", action="store_true")
    args = parser.parse_args()
    if args.frames <= 0:
        parser.error("--frames must be positive")
    if args.rank_fixed < 0:
        parser.error("--rank-fixed must be nonnegative")
    if args.rank_fixed:
        report = args.report.resolve()
        if not report.is_file():
            parser.error(f"gate report does not exist: {report}")
        ranked = ranked_fixed_entries(report, args.rank_fixed)
        args.probes.extend(probe for probe in ranked if probe not in args.probes)
        print("ranked fixed entries: " + ",".join(ranked))
    if not args.probes:
        parser.error("supply at least one probe or --rank-fixed N")

    recording = args.recording.resolve()
    executable = args.executable.resolve()
    rom = args.rom.resolve()
    for required in (recording / "state.bin", recording / "input.fa18in", executable, rom):
        if not required.is_file():
            parser.error(f"required file does not exist: {required}")

    output_dir = ROOT / "build/recomp"
    output_dir.mkdir(parents=True, exist_ok=True)
    reference = output_dir / "timing_probe_off.bin"
    actual = output_dir / "timing_probe_on.bin"
    reference_indices = reference.with_suffix(".index8")
    actual_indices = actual.with_suffix(".index8")
    reference_ram = reference.with_suffix(".ram")
    try:
        replay(executable, recording, rom, args.frames, "off", reference,
               indices=reference_indices, ram=reference_ram)
        print(f"source: {recording.name}, {args.frames} frames")
        for probe in args.probes:
            replay(executable, recording, rom, args.frames, "on", actual,
                   None if probe == "ALL" else probe, indices=actual_indices)
            comparison = compare_frames(reference, actual, reference_indices,
                                        actual_indices, fade_palette(reference_ram))
            found = comparison["first_nonfade_difference"]
            difference = (found["frame"], found["pixels"]) if found else None
            if difference is None:
                if not args.differences_only:
                    print(f"{probe}: matches through frame {args.frames} with Copper fade excluded "
                          f"({comparison['ignored_fade_pixels']} pixels)")
            else:
                frame, pixels = difference
                print(f"{probe}: first non-fade difference frame {frame}, {pixels} pixels; "
                      f"{comparison['ignored_fade_pixels']} fade pixels excluded")
    finally:
        if not args.keep:
            reference.unlink(missing_ok=True)
            actual.unlink(missing_ok=True)
            reference_indices.unlink(missing_ok=True)
            actual_indices.unlink(missing_ok=True)
            reference_ram.unlink(missing_ok=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
