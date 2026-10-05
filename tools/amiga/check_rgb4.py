"""Validate the shared ordinary-buffer RGB4 core against the previous host service."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BASELINE = "9645f4de"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=16384)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    old = subprocess.run(["git", "show", f"{BASELINE}:port/amiga/host_graphics.c"],
                         cwd=ROOT, check=True, capture_output=True, text=True).stdout
    body = old[old.index("int amiga_host_load_rgb4("):].strip()
    frozen = body.replace("int amiga_host_load_rgb4(", "static int original_rgb4(", 1)
    if frozen not in (ROOT / "tools/amiga/rgb4_compat_oracle.c").read_text():
        raise RuntimeError("frozen validation body differs from the baseline host service")
    sources = ["tools/amiga/rgb4_compat_oracle.c", "port/amiga/host_graphics.c",
               "port/amiga/host_compat.c", "port/amiga/guest_memory.c",
               "port/amiga/rgb4.c", "port/amiga/viewport_list.c", "port/disk.c", "port/hunk.c"]
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    exe = directory / "rgb4_compat_oracle.exe"
    subprocess.run(["gcc", "-std=c11", "-O2", "-UNDEBUG", "-Wall", "-Wextra", "-Werror",
                    *sources, "-o", str(exe)], cwd=ROOT, check=True)
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, check=True,
                            capture_output=True, text=True, timeout=60)
    (directory / "rgb4_compat_oracle.log").write_text(result.stdout+result.stderr)
    print(result.stdout, end="")
    if args.cases >= 16384:
        paths = sources + ["port/amiga/rgb4.h", "port/amiga/rgb4_contract_test.c",
                           "port/input_palette.c", "port/input_palette.h",
                           "port/input_palette_contract_test.c", "tools/amiga/check_rgb4.py"]
        checkpoint = {
            "status": "validated_native_rgb4_core_and_callback_backend_runtime_pending",
            "cases": args.cases, "baseline_commit": BASELINE,
            "baseline_function_sha256": hashlib.sha256(body.encode()).hexdigest(),
            "native_cpu_dependency": False,
            "comparison": "complete return and all buffer bytes against exact frozen host service; successes and partial/error paths; original memcpy-overlap UB excluded",
            "scope": "behavior-level host LoadRGB4; exact Kickstart layout/timing is separate",
            "report": result.stdout.strip(),
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_rgb4_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
