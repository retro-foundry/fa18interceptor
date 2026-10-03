"""Prove complete cold menu callbacks/helpers with original bytes and independent normal C."""
import argparse
import json
import subprocess
from check_record_region_probe import ROOT
from audit_menu_cold import ENTRIES,MANIFEST
REAL_OS_BLOCKED_PCS={'C0FE5A','C0FE60','C0FE68','C0FE6E','C0FE72','C0FE76','C0FE78','C0FE82'}

def run(executable,entry,cases,kind):
    result=subprocess.run([str(executable),str(cases),entry],cwd=ROOT,
                          capture_output=True,text=True,timeout=600)
    (ROOT/'build/recomp'/f'menu_cold_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(result.stderr or result.stdout)
    print(result.stdout.splitlines()[0],flush=True)
    return set(result.stdout.split('visited:')[1].split())

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_menu_cold.py'],cwd=ROOT,check=True)
    real=ROOT/'build/recomp/menu_cold_oracle.exe'
    logging='port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(real.relative_to(ROOT)),
        '--main','tools/recomp/menu_cold_oracle.c','--replace-source',logging],cwd=ROOT,check=True)
    contract=ROOT/'build/recomp/menu_cold_contract_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(contract.relative_to(ROOT)),
        '--main','tools/recomp/menu_cold_contract_oracle.c','--replace-source',
        'port/game/glue/glue_child_call.c=tools/recomp/menu_cold_contract_children.c',
        '--replace-source',logging],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,executable,cases in [('real',real,args.cases),('contract',contract,args.contract_cases)]:
        for entry in ENTRIES:
            visited=run(executable,entry,cases,kind)
            if cases>=8192:
                expected=set(source['owners'][entry]['source_pcs'])
                if kind=='real' and entry=='C0FE36': expected-=REAL_OS_BLOCKED_PCS
                missing=expected-visited
                if missing: raise RuntimeError(f'{entry} {kind}: uncovered boundaries {sorted(missing)}')
    if min(args.cases,args.contract_cases)>=8192:
        print('controlled-child proof covers all 132 boundaries; real-child proof covers the remaining original OS-returning scope, excluding eight C0FE36 load-path boundaries')
    else: print('sampled real/controlled-child CPU/RAM checks completed; full boundary coverage not required for this sample')

if __name__=='__main__': main()
