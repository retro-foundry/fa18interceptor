"""Prove complete menu follow-up C with real and controlled original child boundaries."""
import argparse
import json
import subprocess
from audit_menu_followup_source import ROOT,ENTRIES,MANIFEST
REAL_FILE_GATE_PCS={'C1643A','C1643E','C16444','C16446','C1650E','C16510'}
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_menu_followup_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    logging='port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'
    executables={}
    for kind in ['real','contract']:
        name='menu_followup_'+('contract_' if kind=='contract' else '')+'oracle'
        executable=ROOT/'build/recomp'/(name+'.exe'); executables[kind]=executable
        command=['python','scripts/build_recomp.py','--output',str(executable.relative_to(ROOT)),
            '--main','tools/recomp/'+name+'.c','--replace-source',logging]
        if kind=='contract': command+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/menu_followup_contract_children.c']
        subprocess.run(command,cwd=ROOT,check=True)
    for kind,cases in [('real',args.cases),('contract',args.contract_cases)]:
        for entry in ENTRIES:
            result=subprocess.run([str(executables[kind]),str(cases),entry],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'menu_followup_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(result.stderr or result.stdout)
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192:
                visited=set(result.stdout.split('visited:')[1].split())
                expected=REAL_FILE_GATE_PCS if kind=='real' and entry=='C1643A' else set(source['owners'][entry]['source_pcs'])
                if expected-visited: raise RuntimeError(f'{entry} {kind}: uncovered source boundaries {sorted(expected-visited)}')
    if min(args.cases,args.contract_cases)>=8192:
        print('controlled-child proof covers all 151 boundaries; real-child proof covers four complete owners and the six-boundary file-status gate')
    else: print('sampled CPU/RAM checks completed; full owner coverage not required for this sample')
if __name__=='__main__': main()
