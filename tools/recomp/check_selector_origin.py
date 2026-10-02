"""Complete CPU/RAM proof for C29042 and its cold candidate contracts."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=8192)
    parser.add_argument("--entries",nargs="+",default=("C29042","C29368","C29490","C2949A"),
                        choices=("C29042","C29368","C29490","C2949A"))
    args=parser.parse_args()
    if args.cases<=0: parser.error("--cases must be positive")
    executable=build_oracle("selector_origin_oracle","tools/recomp/selector_origin_oracle.c",default_bash())
    for entry in args.entries:
        subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,check=True)

if __name__=="__main__": main()
