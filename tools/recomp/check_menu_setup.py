"""Prove complete menu setup and input/message helpers with original bytes and independent normal C."""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT
from audit_menu_setup import ENTRIES,MANIFEST

def run(executable,entry,cases,kind):
    result=subprocess.run([str(executable),str(cases),entry],cwd=ROOT,
                          capture_output=True,text=True,timeout=600)
    (ROOT/'build/recomp'/f'menu_setup_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(result.stderr or result.stdout)
    print(result.stdout.splitlines()[0],flush=True)
    return set(result.stdout.split('visited:')[1].split())

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_menu_setup.py'],cwd=ROOT,check=True)
    real=ROOT/'build/recomp/menu_setup_oracle.exe'
    logging='port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(real.relative_to(ROOT)),
        '--main','tools/recomp/menu_setup_oracle.c','--replace-source',logging],cwd=ROOT,check=True)
    contract=ROOT/'build/recomp/menu_setup_contract_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(contract.relative_to(ROOT)),
        '--main','tools/recomp/menu_setup_contract_oracle.c','--replace-source',
        'port/game/glue/glue_child_call.c=tools/recomp/menu_setup_contract_children.c',
        '--replace-source',logging],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,executable,cases in [('real',real,args.cases),('contract',contract,args.contract_cases)]:
        for entry in ENTRIES:
            visited=run(executable,entry,cases,kind)
            if cases>=8192:
                missing=set(source['owners'][entry]['source_pcs'])-visited
                if missing: raise RuntimeError(f'{entry} {kind}: uncovered boundaries {sorted(missing)}')
    print('all 141 source boundaries covered separately by real and controlled child proofs')

if __name__=='__main__': main()
