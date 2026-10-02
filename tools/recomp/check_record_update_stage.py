"""Independent complete CPU/RAM proof for record update and its update-stage owner."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=4096,help="cases per complete entry")
    parser.add_argument("--entries",nargs="+",choices=("C22C80","C1C63E"),
                        default=("C22C80","C1C63E"))
    args=parser.parse_args()
    if args.cases<=0: parser.error("--cases must be positive")
    executable=build_oracle("record_update_stage_oracle","tools/recomp/record_update_stage_oracle.c",default_bash())
    for entry in args.entries:
        subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,check=True)

if __name__=="__main__": main()
