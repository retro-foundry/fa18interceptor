"""Compare resumable plane or audio bridge instructions with original opcodes.

Use --group audio for the voice iterator, its helpers and fading. Run from
any directory. Fixtures exercise all CCR combinations and word
boundaries; the sealed state and ROM are read only.
"""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=64,
                        help="fixtures per source instruction (at least 32 for every CCR)")
    parser.add_argument("--bash", default=default_bash())
    parser.add_argument("--group", choices=("planes", "audio"), default="planes")
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    executable = build_oracle("active_planes_step_oracle",
                             "tools/recomp/active_planes_step_oracle.c", args.bash)
    subprocess.run([str(executable), str(args.cases), args.group], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
