"""Prove shared context command actions and their queue exit against source.

Internal action entries are fixtures, not additional registered functions.
No source bytes, register masks or RAM exclusions are changed.
"""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

ENTRIES=("C1B664","C1B66C","C1B6C2","C1BF8C","C1C06E")
RANGES=((0xc1b664,0xc1b77c),(0xc1bf8c,0xc1c052),(0xc1c06e,0xc1c0e0),
        # This family reaches the toggle with ORIGIN_GATE_A nonzero. The
        # zero-to-one arm is reached/proven by the flight and indexed families.
        (0xc1c214,0xc1c21e),(0xc1c23c,0xc1c2b8))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=4096)
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_command_dispatch.py"],cwd=ROOT,check=True)
    source=json.loads((ROOT/"analysis/data/command_dispatch_source_scope.json").read_text())
    executable=build_oracle("context_commands_oracle","tools/recomp/context_commands_oracle.c",default_bash())
    visited=set()
    for entry in ENTRIES:
        result=subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,
                              check=False,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"context_commands_{entry}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(f"{entry}: {result.stderr or result.stdout}")
        print(result.stdout.splitlines()[0],flush=True)
        visited.update(result.stdout.split("visited:")[1].split())
    expected={row["pc"] for row in source["instructions"]
              if any(start<=int(row["pc"],16)<end for start,end in RANGES)}
    if args.cases>=4096 and expected!=visited:
        raise ValueError(f"context actions: uncovered {sorted(expected-visited)}; extra {sorted(visited-expected)}")
    print(f"context actions: {len(ENTRIES)*args.cases} complete action/queue calls, "
          f"{len(visited)} source boundaries observed; original owner registration unchanged")

if __name__=="__main__": main()
