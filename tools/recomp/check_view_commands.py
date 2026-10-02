"""Prove shared view/origin/zoom command actions and their queue exit against source.

Internal action entries are fixtures, not additional registered functions.
No source bytes, register masks or RAM exclusions are changed.
"""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

ENTRIES=("C1B7F0","C1B890","C1B7F8","C1B8F8","C1B914","C1B960","C1B930",
         "C1B98A","C1B9AC","C1B79A","C1B7C2","C1B780","C1B7B6","C1BB66",
         "C1BAE2","C1BB02")
RANGES=((0xc1b77c,0xc1bb7a),(0xc1c23c,0xc1c2b8))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=2048)
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_command_dispatch.py"],cwd=ROOT,check=True)
    source=json.loads((ROOT/"analysis/data/command_dispatch_source_scope.json").read_text())
    executable=build_oracle("view_commands_oracle","tools/recomp/view_commands_oracle.c",default_bash())
    visited=set()
    for entry in ENTRIES:
        result=subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,
                              check=False,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"view_commands_{entry}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(f"{entry}: {result.stderr or result.stdout}")
        print(result.stdout.splitlines()[0],flush=True)
        visited.update(result.stdout.split("visited:")[1].split())
    expected={row["pc"] for row in source["instructions"]
              if any(start<=int(row["pc"],16)<end for start,end in RANGES)}
    if args.cases>=2048 and expected!=visited:
        raise ValueError(f"view/origin/zoom actions: uncovered {sorted(expected-visited)}; extra {sorted(visited-expected)}")
    print(f"view/origin/zoom actions: {len(ENTRIES)*args.cases} complete action/queue calls, "
          f"{len(visited)} source boundaries observed; original owner registration unchanged")

if __name__=="__main__": main()
