"""Prove complete postflight completion C with real and controlled original child boundaries."""
import argparse
import json
import subprocess
from audit_postflight_completion_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_postflight_completion_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    logging='port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'
    executables={}
    for kind in ['real','contract']:
        name='postflight_completion_'+('contract_' if kind=='contract' else '')+'oracle'
        executable=ROOT/'build/recomp'/(name+'.exe'); executables[kind]=executable
        command=['python','scripts/build_recomp.py','--output',str(executable.relative_to(ROOT)),
            '--main','tools/recomp/'+name+'.c','--replace-source',logging]
        if kind=='contract': command+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/postflight_completion_contract_children.c']
        subprocess.run(command,cwd=ROOT,check=True)
    for kind,cases in [('real',args.cases),('contract',args.contract_cases)]:
        for entry in ENTRIES:
            result=subprocess.run([str(executables[kind]),str(cases),entry],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'postflight_completion_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(f'{entry} {kind}: exit {result.returncode}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192:
                visited=set(result.stdout.split('visited:')[1].split())
                expected=set(source['owners'][entry]['source_pcs'])
                if expected-visited: raise RuntimeError(f'{entry} {kind}: uncovered source boundaries {sorted(expected-visited)}')
    if min(args.cases,args.contract_cases)>=8192:
        print('real and controlled-child proofs cover all 205 original owner boundaries')
    else: print('sampled CPU/RAM checks completed; full owner coverage not required for this sample')
if __name__=='__main__': main()
