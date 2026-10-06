"""Measure work still executed by the active runners; compare OFF/ON output."""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from compare_recomp_frames import compare_frames, fade_palette
ENGINES = ("interpreted", "generated", "residual")
BUS_ENGINES = ENGINES + ("port", "os", "chipset")


def digest(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def reduction(on: int, off: int) -> float | None:
    return 100 * (1 - on / off) if off else None


def scenario(name: str, runner: Path, args: list[str], output: Path,
             seal: dict | None, frames: int | None) -> dict:
    work = output / name
    work.mkdir(parents=True, exist_ok=True)
    runner_sha256 = digest(runner)
    if seal:
        for option, expected in (("--state", seal["start_state"]["sha256"]),
                                 ("--input", seal["input_sha256"]), ("--rom", seal["rom_sha256"])):
            if digest(Path(args[args.index(option) + 1])) != expected:
                raise ValueError(f"{name}: {option} seal changed")
    env = os.environ.copy()
    for key in ("FA18_TRACE", "FA18_HOST_TRACE", "FA18_WATCH", "FA18_BOUNDARY_TRACE", "FA18_BOUNDARY_RANGE"):
        env.pop(key, None)
    runs = {}
    for mode in ("off", "on"):
        mode_args = list(args)
        if "--save-dir" in mode_args:
            save = work / f"save-{mode}"
            save.mkdir(exist_ok=True)
            mode_args[mode_args.index("--save-dir") + 1] = str(save)
        command = [str(runner), *mode_args, "--ports", mode, "--profile", str(work / f"{mode}.json"),
                   "--rgb444", str(work / f"{mode}.rgb"), "--ram-out", str(work / f"{mode}.ram"),
                   "--index8", str(work / f"{mode}.index8")]
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True)
        (work / f"{mode}.log").write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(f"{name}/{mode}: runner exited {result.returncode}; see {work / f'{mode}.log'}")
        stats = [json.loads(line) for line in result.stdout.splitlines() if line.startswith("{")]
        profile = json.loads((work / f"{mode}.json").read_text())
        meter = profile["_emulation"]
        if meter["schema"] != 1:
            raise ValueError(f"{name}: unsupported meter schema")
        runs[mode] = dict(meter=meter, runner_stats=stats[0],
                          rgb_sha256=digest(work / f"{mode}.rgb"),
                          index8_sha256=digest(work / f"{mode}.index8"),
                          ram_sha256=digest(work / f"{mode}.ram"))
        if digest(runner) != runner_sha256:
            raise ValueError(f"{name}: runner changed during measurement")
    comparison = compare_frames(work / "off.rgb", work / "on.rgb", work / "off.index8",
                                work / "on.index8", fade_palette(work / "off.ram"))
    # These are disposable meter outputs in build/, never recordings.
    for mode in ("off", "on"):
        for suffix in ("rgb", "ram", "index8"):
            (work / f"{mode}.{suffix}").unlink()
    off, on = runs["off"], runs["on"]
    counts = {mode: sum(run["meter"]["instructions"][engine] for engine in ENGINES)
              for mode, run in runs.items()}
    bus = {mode: sum(sum(run["meter"]["bus"][engine].values()) for engine in BUS_ENGINES)
           for mode, run in runs.items()}
    chipset = {mode: sum(run["meter"]["chipset"].values()) for mode, run in runs.items()}
    services = {mode: run["meter"]["os"]["service_steps"] for mode, run in runs.items()}
    sealed_ram = (on["ram_sha256"] == seal["replay"]["final_ram_sha256"]
                  if seal and frames is None else None)
    rgb_equal = off["rgb_sha256"] == on["rgb_sha256"]
    comparable = off["runner_stats"]["iterations"] == on["runner_stats"]["iterations"]
    return dict(name=name, runner_sha256=runner_sha256, runs=runs, emulated_instructions=counts,
                cpu_removed_percent=reduction(counts["on"], counts["off"]),
                guest_accesses=bus, guest_access_reduction_percent=reduction(bus["on"], bus["off"]),
                chipset_operations=chipset, chipset_reduction_percent=reduction(chipset["on"], chipset["off"]),
                service_steps=services, service_reduction_percent=reduction(services["on"], services["off"]),
                rgb_equal=rgb_equal, sealed_ram_equal=sealed_ram, iterations_equal=comparable,
                frame_comparison=comparison,
                parity_passed=comparison["passed"] and comparable and sealed_ram is not False)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp/fa18_recomp.exe")
    parser.add_argument("--gnu-launcher", type=Path, default=ROOT / "build/recomp/fa18_romfree.exe")
    parser.add_argument("--msvc-launcher", type=Path, default=ROOT / "build/recomp-cmake/Release/fa18_romfree.exe")
    parser.add_argument("--scenario", action="append", help="select a scenario; default is all five")
    parser.add_argument("--frames", type=int, help="bounded discovery probe; not the full acceptance suite")
    parser.add_argument("--jobs", type=int, default=3)
    parser.add_argument("--out", type=Path, default=ROOT / "analysis/emulation_removal_meter.json")
    parser.add_argument("--work", type=Path, default=ROOT / "build/emulation-meter")
    parser.add_argument("--launcher-checks", action="store_true", help="also run both full ADF-only launcher checks")
    args = parser.parse_args()
    if args.jobs < 1 or (args.frames is not None and args.frames < 1):
        parser.error("jobs and probe frames must be positive")
    args.work = args.work.resolve()
    if not args.work.is_relative_to((ROOT / "build").resolve()):
        parser.error("disposable output directory must be within build/")
    candidates = []
    for name in ("demo01", "qual_carrier_success", "qual_fail_crashes"):
        capture = ROOT / "captures/native" / name
        seal = json.loads((capture / "run.json").read_text())
        command = ["--state", str(capture / "state.bin"), "--input", str(capture / "input.fa18in"),
                   "--rom", str(ROOT / "local/system/kick13.rom")]
        command += ["--frames", str(args.frames)] if args.frames else ["--to-end"]
        candidates.append((name, args.runner.resolve(), command, seal))
    args.work.mkdir(parents=True, exist_ok=True)
    replay = args.work / "demo.e9k"
    replay.write_text("E9K_INPUT_V1\n" + "".join(
        f"F {1800+i*8} K {key} 0 0 1\nF {1802+i*8} K {key} 0 0 0\n"
        for i, key in enumerate([112, 105, 108, 111, 116, 13, 49])))
    for name, runner in (("adf_gnu", args.gnu_launcher), ("adf_msvc", args.msvc_launcher)):
        command = ["--adf", str(ROOT / "local/media/fa18.adf"), "--replay", str(replay),
                   "--no-restore-lead", "--frames", str(args.frames or 2600),
                   "--save-dir", str(args.work / name / "save")]
        candidates.append((name, runner.resolve(), command, None))
    if args.scenario:
        unknown = set(args.scenario) - {row[0] for row in candidates}
        if unknown:
            parser.error("unknown scenarios: " + ", ".join(sorted(unknown)))
        candidates = [row for row in candidates if row[0] in args.scenario]
    if args.launcher_checks:
        for runner in (args.gnu_launcher, args.msvc_launcher):
            subprocess.run([os.sys.executable, str(ROOT / "tools/amiga/check_romfree_launcher.py"),
                            "--runner", str(runner.resolve())], cwd=ROOT, check=True)
    def run(row):
        name, runner, command, seal = row
        print(f"meter: {name} OFF/ON", flush=True)
        result = scenario(name, runner, command, args.work, seal, args.frames)
        print(f"meter: {name}: CPU {result['cpu_removed_percent']:.4f}% removed; "
              f"parity {'PASS' if result['parity_passed'] else 'FAIL'}", flush=True)
        return result
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        rows = list(pool.map(run, candidates))
    sites = sum(len(re.findall(r"\b(?:rd|wr)_[us](?:8|16|32)\s*\(", path.read_text()))
                for path in (ROOT / "port/game").rglob("*.[ch]"))
    full = not args.frames and len(rows) == 5
    report = dict(schema=1, scope="full fixed suite" if full else "partial discovery probe",
                  frame_comparison_policy="Ignore source-table Copper fade; require identical selected indices and non-fade RGB.",
                  cpu_removed_min_percent=min(row["cpu_removed_percent"] for row in rows),
                  accepted_cpu_removed_min_percent=(min(row["cpu_removed_percent"] for row in rows)
                                                    if full and all(row["parity_passed"] for row in rows) else None),
                  parity_passed=all(row["parity_passed"] for row in rows),
                  axes=dict(memory_sites_converted=0, memory_sites_remaining=sites,
                            memory_cutover_percent=0, native_chipset_percent=0, native_boot_percent=0),
                  deletable_subsystems=0, subsystem_count=4,
                  deletion_evidence="Both active builds still require all four subsystems; no omission build passes.",
                  launcher_checks_passed=args.launcher_checks, scenarios=rows,
                  limits=["CPU share is executed work, not plan completion or whole-game coverage.",
                          "Failed parity makes CPU shares raw observations, not accepted removal progress.",
                          "Residual instructions include original-byte execution between labels and OS RTE opcode helpers.",
                          "Hand-written instruction steps still depend on PC/registers/bus; port step counts expose that debt.",
                          "Bus counts are top-level guest API accesses including instruction fetch/dispatch reads, not bus cycles.",
                          "RAM page counts are overlapping access hits; DMA reads/writes are separate chipset operations.",
                          "Direct DMA and host compatibility memory accesses are outside guest-API page counts; absence does not prove exclusive ownership.",
                          "Traffic reductions do not establish converted memory or native IO/boot.",
                          "Zero OFF dispatches give no service reduction denominator; guest boot is still required.",
                          "Full mission outcomes, progression and active-flight teardown are still unverified."])
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + "\n")
    def percent(value):
        return "n/a" if value is None else f"{value:.4f}%"
    lines = ["# Active-runner emulation meter", "", f"Scope: {report['scope']}.", "",
             f"Raw CPU work removed (minimum): **{report['cpu_removed_min_percent']:.4f}%**. "
             f"Output parity: **{'PASS' if report['parity_passed'] else 'FAIL'}**. Deletable subsystems: **0/4**.", "",
             f"Memory cutover: **0%** (0 converted, {sites} remaining access sites). "
             "Native chipset: **0%**. Native boot: **0%**.", "",
             "| Scenario | OFF instructions | ON instructions | CPU removed | Guest accesses reduced | Chipset ops reduced | OS steps reduced | Parity |",
             "| --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |"]
    for row in rows:
        lines.append(f"| {row['name']} | {row['emulated_instructions']['off']} | {row['emulated_instructions']['on']} | "
                     f"{percent(row['cpu_removed_percent'])} | {percent(row['guest_access_reduction_percent'])} | "
                     f"{percent(row['chipset_reduction_percent'])} | {percent(row['service_reduction_percent'])} | "
                     f"{'PASS' if row['parity_passed'] else 'FAIL'} |")
    lines += ["", *[f"- {limit}" for limit in report["limits"]], ""]
    lines += ["Frame policy: " + report["frame_comparison_policy"], "",
              "| Scenario | First strict RGB difference | First non-fade difference | Fade pixels excluded | Final RAM seal | Iterations equal |",
              "| --- | ---: | --- | ---: | --- | --- |"]
    for row in rows:
        comp = row["frame_comparison"]
        lines.append(f"| {row['name']} | {comp['first_rgb_difference']} | "
                     f"{comp['first_nonfade_difference']} | {comp['ignored_fade_pixels']} | "
                     f"{row['sealed_ram_equal']} | {row['iterations_equal']} |")
    lines.append("")
    args.out.with_suffix(".md").write_text("\n".join(lines))
    print("\n".join(lines[:8]), flush=True)
    return 0 if report["parity_passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
