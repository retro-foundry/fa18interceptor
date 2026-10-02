"""Independent complete-call CPU/RAM proof for C1CCBC."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=8192)
    parser.add_argument("--positions", action="store_true",
                        help="compare both complete position children, including full SR and all RAM")
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    executable = build_oracle("followup_placements_oracle",
                              "tools/recomp/followup_placements_oracle.c", default_bash())
    command=[str(executable), str(args.cases)]
    if args.positions:
        command.append("positions")
    subprocess.run(command, cwd=ROOT, check=True)

if __name__ == "__main__":
    main()
