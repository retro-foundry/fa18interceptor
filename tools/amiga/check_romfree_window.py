"""Check window frame-limit/close finalization against headless execution."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import os
import csv

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp-cmake/Release/fa18_romfree.exe")
    parser.add_argument("--quit-driver", type=Path, default=ROOT / "build/recomp-cmake/Release/fa18_window_shutdown_test.exe")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="romfree-window-") as directory:
        work = Path(directory)
        shutil.copy2(args.runner, work / "game.exe")
        shutil.copy2(args.quit_driver, work / "quit.exe")
        shutil.copy2(ROOT / "local/media/fa18.adf", work / "original.adf")
        env = os.environ.copy()
        env["SDL_VIDEODRIVER"] = "dummy"

        def run(name, exe, options):
            result = subprocess.run([str(work / exe), "--adf", "original.adf",
                "--save-dir", "saves", "--ram-out", f"{name}.ram", "--ppm", f"{name}.ppm"] + options,
                cwd=work, env=env, capture_output=True, text=True)
            assert result.returncode == 0, (name, result.stdout, result.stderr)
            stats = [json.loads(line) for line in result.stdout.splitlines()]
            assert stats[-1] == dict(rom_reads=0, rom_instruction_fetches=0, unsupported_services=0)
            assert (work / f"{name}.ram").stat().st_size == 0x100000 + 18 * 4
            return stats

        for sync in ("off", "on"):
            stats = run(sync, "game.exe", ["--window", "--vsync", sync, "--frames", "4",
                "--frame-times", f"{sync}.csv"])
            assert stats[0]["frames"] == 4
            with (work / f"{sync}.csv").open(newline="") as stream:
                rows = list(csv.DictReader(stream))
            assert len(rows) == 4
            for frame, row in enumerate(rows, 1):
                assert int(row["frame"]) == frame and row["screen_changed"] in ("0", "1")
                stages = [float(row[key]) for key in
                    ("input_us", "simulation_us", "convert_us", "present_us", "wait_us")]
                assert all(value >= 0 for value in stages) and float(row["total_us"]) > 0
                assert abs(sum(stages) - float(row["total_us"])) < 0.01
            reference = run(f"{sync}-headless", "game.exe", ["--frames", "4"])
            assert stats == reference
            for suffix in ("ram", "ppm"):
                assert (work / f"{sync}.{suffix}").read_bytes() == (work / f"{sync}-headless.{suffix}").read_bytes()
        stats = run("quit", "quit.exe", ["--window", "--frames", "100"])
        frames = stats[0]["frames"]
        assert 0 < frames < 100, frames
        reference = run("headless", "game.exe", ["--frames", str(frames)])
        assert stats == reference
        for suffix in ("ram", "ppm"):
            assert (work / f"quit.{suffix}").read_bytes() == (work / f"headless.{suffix}").read_bytes()
        print(f"SDL close after {frames} live frames: complete RAM/CPU and pixels match headless; zero ROM/fault counters")
        print("Both vsync settings honor frame limits and emit final diagnostics")
        print("Frame timing CSV has one complete row per frame; measured runs preserve full RAM/CPU and pixels")
        stats = run("fast", "game.exe", ["--window", "--frames", "4", "--fast-forward", "2", "--frame-times", "fast.csv"])
        reference = run("fast-headless", "game.exe", ["--frames", "4"])
        assert stats == reference
        for suffix in ("ram", "ppm"):
            assert (work / f"fast.{suffix}").read_bytes() == (work / f"fast-headless.{suffix}").read_bytes()
        with (work / "fast.csv").open(newline="") as stream:
            assert [int(row["frame"]) for row in csv.DictReader(stream)] == [3, 4]
        stats = run("fast-quit", "quit.exe", ["--window", "--frames", "100", "--fast-forward", "2"])
        reference = run("fast-quit-headless", "game.exe", ["--frames", str(stats[0]["frames"])])
        assert 2 < stats[0]["frames"] < 100 and stats == reference
        for suffix in ("ram", "ppm"):
            assert (work / f"fast-quit.{suffix}").read_bytes() == (work / f"fast-quit-headless.{suffix}").read_bytes()
        for value in ("-1", "4", "hello", "2147483648"):
            result = subprocess.run([str(work / "game.exe"), "--adf", "original.adf", "--window",
                "--frames", "4", "--fast-forward", value], cwd=work, env=env, capture_output=True, text=True)
            assert result.returncode == 2 and "--fast-forward requires" in result.stderr
        print("Fast-forward executes all frames, preserves RAM/CPU/pixels, reports only presented frames and honors close")
        rejected = subprocess.run([str(work / "game.exe"), "--adf", "original.adf",
            "--frame-times", "invalid.csv"], cwd=work, env=env, capture_output=True, text=True)
        assert rejected.returncode == 2 and "requires --window" in rejected.stderr
        rejected = subprocess.run([str(work / "game.exe"), "--adf", "original.adf",
            "--window", "--frame-times", "missing/invalid.csv", "--frames", "4"],
            cwd=work, env=env, capture_output=True, text=True)
        assert rejected.returncode == 1 and "cannot write" in rejected.stderr
        assert json.loads(rejected.stdout.splitlines()[-1]) == dict(rom_reads=0, rom_instruction_fetches=0, unsupported_services=0)


if __name__ == "__main__":
    main()
