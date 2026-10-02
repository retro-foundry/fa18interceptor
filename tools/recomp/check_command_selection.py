"""Prove the command selection component against unmodified source prefixes.

This stops at action entry, not owner return. It does not count either parent
as complete or registered. Child execution and timing remain separate work.
"""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=8192)
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_command_dispatch.py"],cwd=ROOT,check=True)
    source=json.loads((ROOT/"analysis/data/command_dispatch_source_scope.json").read_text())
    executable=build_oracle("command_selection_oracle","tools/recomp/command_selection_oracle.c",default_bash())
    for entry,start,end in (("C1AC28",0xc1ac28,0xc1ad70),("C1AD74",0xc1ad74,0xc1b126)):
        result=subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,
                              check=True,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"command_selection_{entry}.log").write_text(result.stdout)
        print(result.stdout.splitlines()[0],flush=True)
        if args.cases>=8192:
            expected={pc for pc in source["owners"][entry]["source_pcs"] if start<=int(pc,16)<end}
            visited=set(result.stdout.split("visited:")[1].split())
            if expected!=visited: raise ValueError(f"{entry}: uncovered selection boundaries {sorted(expected-visited)}")
    publication=build_oracle("command_publication_oracle","tools/recomp/command_publication_oracle.c",default_bash())
    result=subprocess.run([str(publication),str(args.cases)],cwd=ROOT,
                          check=True,capture_output=True,text=True)
    (ROOT/"build/recomp/command_publication.log").write_text(result.stdout)
    print(result.stdout.splitlines()[0],flush=True)
    if args.cases>=8192:
        expected={row["pc"] for row in source["instructions"] if 0xc1c23c<=int(row["pc"],16)<0xc1c2b8}
        visited=set(result.stdout.split("visited:")[1].split())
        if expected!=visited: raise ValueError(f"publication: uncovered boundaries {sorted(expected-visited)}")

if __name__=="__main__": main()
