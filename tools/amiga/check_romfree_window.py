"""Check window frame-limit/close finalization against headless execution."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import os

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
            stats = run(sync, "game.exe", ["--window", "--vsync", sync, "--frames", "4"])
            assert stats[0]["frames"] == 4
        stats = run("quit", "quit.exe", ["--window", "--frames", "100"])
        frames = stats[0]["frames"]
        assert 0 < frames < 100, frames
        reference = run("headless", "game.exe", ["--frames", str(frames)])
        assert stats == reference
        for suffix in ("ram", "ppm"):
            assert (work / f"quit.{suffix}").read_bytes() == (work / f"headless.{suffix}").read_bytes()
        print(f"SDL close after {frames} live frames: complete RAM/CPU and pixels match headless; zero ROM/fault counters")
        print("Both vsync settings honor frame limits and emit final diagnostics")


if __name__ == "__main__":
    main()
