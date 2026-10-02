"""Prove shared aircraft command actions and their queue exit against source.

Internal action entries are fixtures, not additional registered functions.
No source bytes, register masks or RAM exclusions are changed.
"""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash

ENTRIES=("C1B126","C1B1A4","C1B1C8","C1B21C","C1B22C","C1B236","C1B264",
         "C1B4F4","C1B4FC","C1B504","C1B540","C1B548","C1B550",
         "C1B58E","C1B594","C1B59A","C1B5B8","C1B5BC","C1B5C0",
         "C1B5DC","C1B616","C1BB7A","C1BC12","C1C052","C1C0E0",
         "C1C172","C1C1F2","C1C224")
RANGES=((0xc1b126,0xc1b27e),(0xc1b4f4,0xc1b50c),(0xc1b540,0xc1b558),
        (0xc1b58e,0xc1b602),(0xc1b616,0xc1b664),(0xc1bb7a,0xc1bc50),
        (0xc1c052,0xc1c06e),(0xc1c0e0,0xc1c1b0),(0xc1c1f2,0xc1c2b8))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=512)
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_command_dispatch.py"],cwd=ROOT,check=True)
    source=json.loads((ROOT/"analysis/data/command_dispatch_source_scope.json").read_text())
    executable=build_oracle("flight_commands_oracle","tools/recomp/flight_commands_oracle.c",default_bash())
    visited=set()
    for entry in ENTRIES:
        result=subprocess.run([str(executable),str(args.cases),entry],cwd=ROOT,
                              check=False,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"flight_commands_{entry}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(f"{entry}: {result.stderr or result.stdout}")
        print(result.stdout.splitlines()[0],flush=True)
        visited.update(result.stdout.split("visited:")[1].split())
    expected={row["pc"] for row in source["instructions"]
              if any(start<=int(row["pc"],16)<end for start,end in RANGES)}
    if args.cases>=512 and expected!=visited:
        raise ValueError(f"aircraft actions: uncovered {sorted(expected-visited)}; extra {sorted(visited-expected)}")
    print(f"aircraft actions: {len(ENTRIES)*args.cases} complete action/queue calls, "
          f"{len(visited)} source boundaries observed; original owner registration unchanged")

if __name__=="__main__": main()
