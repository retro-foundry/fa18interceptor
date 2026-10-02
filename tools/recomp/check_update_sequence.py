"""Independent complete update/input/display CPU/RAM and child-contract proofs.

Real original children and controlled child-boundary fixtures are separate.
Neither executable uses the source timing bridges to implement its parent.
"""
import argparse
import subprocess
from check_record_region_probe import ROOT,build_oracle,default_bash
from port_info import instructions

ENTRIES=("C0EFD4","C0F3C4","C0D730")

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=8192)
    parser.add_argument("--contract-cases",type=int,default=1024)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error("case counts must be positive")
    real=build_oracle("update_sequence_oracle","tools/recomp/update_sequence_oracle.c",default_bash())
    for entry in ENTRIES:
        result=subprocess.run([str(real),str(args.cases),entry],cwd=ROOT,check=True,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"update_real_{entry}.log").write_text(result.stdout)
        print(result.stdout.splitlines()[0],flush=True)
    result=subprocess.run([str(real),"256","C0D730","timed"],cwd=ROOT,check=True,capture_output=True,text=True)
    (ROOT/"build/recomp/update_display_continuation.log").write_text(result.stdout)
    print("C0D730: 256 original/native frame-exit timing calls leave zero continuations",flush=True)
    contract=ROOT/"build/recomp/update_sequence_contract_oracle.exe"
    subprocess.run(["python","scripts/build_recomp.py","--output",str(contract.relative_to(ROOT)),
        "--main","tools/recomp/update_sequence_contract_oracle.c","--replace-source",
        "port/game/glue/glue_child_call.c=tools/recomp/update_sequence_contract_children.c"],cwd=ROOT,check=True)
    for entry in ENTRIES:
        result=subprocess.run([str(contract),str(args.contract_cases),entry],cwd=ROOT,check=True,capture_output=True,text=True)
        (ROOT/"build/recomp"/f"update_contract_{entry}.log").write_text(result.stdout)
        print(result.stdout.splitlines()[0],flush=True)
        visited=set(result.stdout.split("visited:")[1].split())
        missing={line.split(":")[0] for line in instructions(entry)}-visited
        # C0DA38 unlinks the enclosing frame before C0D748 could run.
        allowed={"C0D748"} if entry=="C0D730" else set()
        if missing-allowed: raise RuntimeError(f"{entry}: uncovered parent boundaries {sorted(missing-allowed)}")
        print(f"{entry}: uncovered boundaries {sorted(missing)}",flush=True)

if __name__=="__main__": main()
