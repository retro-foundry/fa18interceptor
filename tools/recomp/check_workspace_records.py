"""Prove complete workspace selector C against the original shared-tail calls."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=16384)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    executable = build_oracle("workspace_records_oracle",
                              "tools/recomp/workspace_records_oracle.c", default_bash())
    subprocess.run([str(executable), str(args.cases)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
