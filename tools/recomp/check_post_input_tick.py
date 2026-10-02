"""Independent complete C0F5F8 source/CPU/RAM proof."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=8192)
    args=parser.parse_args()
    if args.cases<=0:
        parser.error("--cases must be positive")
    executable=build_oracle("post_input_tick_oracle",
                            "tools/recomp/post_input_tick_oracle.c",default_bash())
    subprocess.run([str(executable),str(args.cases)],cwd=ROOT,check=True)

if __name__=="__main__":
    main()
