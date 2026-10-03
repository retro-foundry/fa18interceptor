"""Complete real-child and controlled-child context publisher CPU/RAM proofs."""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT,build_oracle,default_bash
from audit_context_publication import ENTRIES,MANIFEST

def run(exe,entry,cases,kind):
    result=subprocess.run([str(exe),str(cases),entry],cwd=ROOT,
                          capture_output=True,text=True,timeout=180)
    (ROOT/"build/recomp"/f"context_publication_{kind}_{entry}.log").write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(result.stderr or result.stdout)
    print(result.stdout.splitlines()[0],flush=True)
    return set(result.stdout.split("visited:")[1].split())

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases",type=int,default=16384)
    parser.add_argument("--contract-cases",type=int,default=8192)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error("cases must be positive")
    subprocess.run(["python","tools/recomp/audit_context_publication.py"],cwd=ROOT,check=True)
    real=build_oracle("context_publication_oracle","tools/recomp/context_publication_oracle.c",default_bash())
    contract=ROOT/"build/recomp/context_publication_contract_oracle.exe"
    subprocess.run(["python","scripts/build_recomp.py","--output",str(contract.relative_to(ROOT)),
        "--main","tools/recomp/context_publication_contract_oracle.c","--replace-source",
        "port/game/glue/glue_child_call.c=tools/recomp/context_publication_contract_children.c"],cwd=ROOT,check=True)
    visited={entry:run(real,entry,args.cases,"real") for entry in ENTRIES}
    # C083A6 fixes the level to one; its shared clear branch is independently
    # entered at C083B6 with the original save frame and both level values.
    visited["C083A6"] |= run(real,"C083B6",args.contract_cases,"body")
    for entry in ("C1BEE8","C09DD0"):
        visited[entry] |= run(contract,entry,args.contract_cases,"contract")
    if args.cases>=16384 and args.contract_cases>=8192:
        source=json.loads(MANIFEST.read_text())
        for entry in ENTRIES:
            missing=set(source["owners"][entry]["source_pcs"])-visited[entry]
            if missing: raise RuntimeError(f"{entry}: uncovered source boundaries {sorted(missing)}")
        print("all 153 owned boundaries covered by separate real/body/contract proofs")

if __name__=="__main__": main()
