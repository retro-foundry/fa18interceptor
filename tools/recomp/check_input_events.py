"""Independent real-child and cold child-contract input owner proofs."""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from port_info import instructions

ENTRIES=("C16EAE", "C16BF2", "C16C56", "C13D34")

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=16384)
    args=parser.parse_args()
    if args.cases<=0: parser.error("cases must be positive")
    real=build_oracle("input_events_oracle","tools/recomp/input_events_oracle.c",default_bash())
    contract=ROOT/"build/recomp/input_events_contract_oracle.exe"
    subprocess.run(["python","scripts/build_recomp.py","--output",str(contract.relative_to(ROOT)),
        "--main","tools/recomp/input_events_contract_oracle.c","--replace-source",
        "port/game/glue/glue_child_call.c=tools/recomp/input_events_contract_children.c"],cwd=ROOT,check=True)
    for kind,exe in (("real",real),("contract",contract)):
        for entry in ENTRIES:
            result=subprocess.run([str(exe),str(args.cases),entry],cwd=ROOT,check=True,capture_output=True,text=True)
            (ROOT/"build/recomp"/f"input_events_{kind}_{entry}.log").write_text(result.stdout)
            print(result.stdout.splitlines()[0],flush=True)
            if kind=="contract" and args.cases>=16384:
                visited=set(result.stdout.split("visited:")[1].split())
                missing={line.split(":")[0] for line in instructions(entry)}-visited
                if missing: raise RuntimeError(f"{entry}: uncovered source boundaries {sorted(missing)}")

if __name__=="__main__": main()
