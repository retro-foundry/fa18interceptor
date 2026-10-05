"""Compare viewport-list extraction with the frozen host implementation and state."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
ROOT = Path(__file__).resolve().parents[2]
BASELINE = "5671d334"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=16384)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    old = subprocess.run(["git", "show", f"{BASELINE}:port/amiga/host_graphics.c"],
                         cwd=ROOT, check=True, capture_output=True, text=True).stdout
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "viewport_list_previous.h").write_text(old)
    sources = ["tools/amiga/viewport_list_oracle.c", "port/amiga/host_graphics.c",
               "port/amiga/host_compat.c", "port/amiga/guest_memory.c", "port/amiga/rgb4.c",
               "port/amiga/viewport_list.c", "port/disk.c", "port/hunk.c"]
    exe = directory / "viewport_list_oracle.exe"
    subprocess.run(["gcc", "-std=c11", "-O2", "-UNDEBUG", "-Wall", "-Wextra", "-Werror",
                    "-Iport/amiga", *sources, "-o", str(exe)], cwd=ROOT, check=True)
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, check=True,
                            capture_output=True, text=True, timeout=180)
    (directory / "viewport_list_oracle.log").write_text(result.stdout+result.stderr)
    print(result.stdout, end="")
    if args.cases >= 16384:
        paths = sources + ["port/amiga/viewport_list.h", "port/amiga/viewport_list_contract_test.c",
                           "port/input_palette_contract_test.c", "tools/amiga/check_viewport_list.py"]
        checkpoint = {
            "status": "validated_native_viewport_construction_and_merge_game_setup_integration_pending",
            "cases": args.cases*2, "baseline_commit": BASELINE,
            "baseline_source_sha256": hashlib.sha256(old.encode()).hexdigest(),
            "native_cpu_dependency": False,
            "comparison": "return, all buffer bytes and full allocator/host state; successful and partial/error paths",
            "scope": "existing behavior-level MakeVPort/MrgCop semantics, not exact Kickstart layout/timing",
            "report": result.stdout.strip(),
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_viewport_list_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
