"""Complete real-child CPU/RAM scheduler proof, including cold mode paths."""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT,build_oracle,default_bash
from audit_postflight_scheduler import ENTRIES,MANIFEST

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=16384)
    parser.add_argument("--entry",choices=ENTRIES,action="append")
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_postflight_scheduler.py"],cwd=ROOT,check=True)
    exe=build_oracle("postflight_scheduler_oracle","tools/recomp/postflight_scheduler_oracle.c",default_bash())
    source=json.loads(MANIFEST.read_text())
    for entry in args.entry or ENTRIES:
        result=subprocess.run([str(exe),str(args.cases),entry],cwd=ROOT,
                              capture_output=True,text=True,timeout=180)
        (ROOT/"build/recomp"/f"postflight_scheduler_{entry}.log").write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(result.stderr or result.stdout)
        print(result.stdout.splitlines()[0],flush=True)
        if args.cases>=16384:
            visited=set(result.stdout.split("visited:")[1].split())
            missing=set(source["owners"][entry]["source_pcs"])-visited
            if missing: raise RuntimeError(f"{entry}: uncovered source boundaries {sorted(missing)}")

if __name__=="__main__": main()
