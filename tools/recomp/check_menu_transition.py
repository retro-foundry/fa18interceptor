"""Prove complete menu callbacks with original bytes and independent normal C."""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT,build_oracle,default_bash
from audit_menu_transition import ENTRIES,MANIFEST

def run(executable,entry,cases,kind):
    result=subprocess.run([str(executable),str(cases),entry],cwd=ROOT,
                          capture_output=True,text=True,timeout=240)
    (ROOT/'build/recomp'/f'menu_transition_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(result.stderr or result.stdout)
    print(result.stdout.splitlines()[0],flush=True)
    return set(result.stdout.split('visited:')[1].split())

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_menu_transition.py'],cwd=ROOT,check=True)
    real=build_oracle('menu_transition_oracle','tools/recomp/menu_transition_oracle.c',default_bash())
    contract=ROOT/'build/recomp/menu_transition_contract_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(contract.relative_to(ROOT)),
        '--main','tools/recomp/menu_transition_contract_oracle.c','--replace-source',
        'port/game/glue/glue_child_call.c=tools/recomp/menu_transition_contract_children.c'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,executable,cases in [('real',real,args.cases),('contract',contract,args.contract_cases)]:
        for entry in ENTRIES:
            visited=run(executable,entry,cases,kind)
            if cases>=8192:
                missing=set(source['owners'][entry]['source_pcs'])-visited
                if missing: raise RuntimeError(f'{entry} {kind}: uncovered boundaries {sorted(missing)}')
    print('all 324 source boundaries covered separately by real and controlled child proofs')

if __name__=='__main__': main()
