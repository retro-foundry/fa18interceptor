"""Independent complete-owner CPU/RAM proof for C1AC28 and C1AD74.

This runs the complete original parent and its real children. Component
proofs separately exercise all action branches that recordings leave cold.
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
    pcs=[row["pc"] for row in source["instructions"]]
    (ROOT/"build/recomp/command_dispatch_owner_pcs.h").write_text(
        "/* Audited original owner PCs; test oracle input only. */\n"
        "static const uint32_t owner_pcs[]={"+",".join("0x"+pc for pc in pcs)+"};\n")
    executable=build_oracle("command_dispatch_oracle","tools/recomp/command_dispatch_oracle.c",default_bash())
    contract=ROOT/"build/recomp/command_dispatch_contract_oracle.exe"
    subprocess.run(["python","scripts/build_recomp.py","--output",str(contract.relative_to(ROOT)),
        "--main","tools/recomp/command_dispatch_contract_oracle.c","--replace-source",
        "port/game/glue/glue_child_call.c=tools/recomp/command_dispatch_contract_children.c"],cwd=ROOT,check=True)
    for kind,exe in (("real",executable),("contract",contract)):
        visited=set()
        for entry in ("C1AC28","C1AD74"):
            result=subprocess.run([str(exe),str(args.cases),entry],cwd=ROOT,
                                  check=False,capture_output=True,text=True,timeout=180)
            (ROOT/"build/recomp"/f"command_dispatch_{kind}_{entry}.log").write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(f"{kind} {entry}: {result.stderr or result.stdout}")
            print(result.stdout.splitlines()[0],flush=True)
            visited.update(result.stdout.split("visited:")[1].split())
        if args.cases>=8192:
            exits={"C06BF0","C06BF6","C06BFA","C06C00","C1AC18","C1AC20","C1AC26",
                   "C1AD70","C1AD72","C1C2B8"}
            if not exits<=visited: raise RuntimeError(f"{kind}: uncovered owner exits {sorted(exits-visited)}")
        print(f"complete command owners: {args.cases*2} {kind} CPU/RAM calls; "
              f"{len(visited)} parent boundaries observed")

if __name__=="__main__": main()
