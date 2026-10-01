"""Build and run the original-instruction oracle for the grid projection packet.

Uses the headless build's source list and shared cached objects. Reads the
sealed demo start state without modifying it. Run from any working directory:
    python tools/recomp/check_record_region_probe.py [--cases 512]
"""
from pathlib import Path
import argparse
import subprocess
import os

ROOT = Path(__file__).resolve().parents[2]


def default_bash():
    # Windows' system32/bash launches WSL and mangles this MinGW build script.
    return "C:/Program Files/Git/bin/bash.exe" if os.name == "nt" else "bash"


def build_oracle(name, source, bash):
    """Reuse exactly the headless runner's current translation and C sources."""
    del bash  # Retained as a command-line compatibility option.
    subprocess.run([
        "python", "scripts/build_recomp.py", "--output", f"build/recomp/{name}.exe",
        "--main", source,
    ], cwd=ROOT, check=True)
    return ROOT / f"build/recomp/{name}.exe"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=512)
    parser.add_argument("--bash", default=default_bash())
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    build_oracle("grid_projection_oracle", "tools/recomp/grid_projection_oracle.c", args.bash)
    subprocess.run([str(ROOT / "build/recomp/grid_projection_oracle.exe"),
                    str(args.cases)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
