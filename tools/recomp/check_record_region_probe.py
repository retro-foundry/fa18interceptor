"""Build and run the original-instruction oracle for the region probe.

Uses the headless build's source list and cached generated objects. Reads the
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
    build = (ROOT / "scripts/build_recomp.sh").read_text()
    replacements = {
        'cd "$(dirname "$0")/.."': "",
        "-o build/recomp/fa18_recomp.exe": f"-o build/recomp/{name}.exe",
        "port/recomp/recomp_main.c": source,
    }
    for old, new in replacements.items():
        if build.count(old) != 1:
            raise SystemExit(f"oracle build: expected one occurrence of {old!r} in build_recomp.sh")
        build = build.replace(old, new)
    subprocess.run([bash, "-c", build], cwd=ROOT, check=True)
    return ROOT / f"build/recomp/{name}.exe"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=512)
    parser.add_argument("--bash", default=default_bash())
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    build_oracle("record_region_probe_oracle", "tools/recomp/record_region_probe_oracle.c", args.bash)
    subprocess.run([str(ROOT / "build/recomp/record_region_probe_oracle.exe"),
                    str(args.cases)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
