"""Independent complete CPU/RAM proof for context refresh and bootstrap."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=8192,help="cases per complete entry")
    parser.add_argument("--entries",nargs="+",choices=("C1C860","C08F26","C0F920","C0F992"),
                        default=("C1C860","C08F26","C0F920","C0F992"))
    args=parser.parse_args()
    if args.cases<=0: parser.error("--cases must be positive")
    executable=build_oracle("scene_bootstrap_oracle","tools/recomp/scene_bootstrap_oracle.c",default_bash())
    for entry in args.entries:
        subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,check=True)

if __name__=="__main__": main()
