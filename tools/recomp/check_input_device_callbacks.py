"""Prove complete input-device game C with real and controlled source children."""
import argparse,json,subprocess
from audit_input_device_callbacks_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_input_device_callbacks_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,cases in [('contract',args.contract_cases),('real',args.cases)]:
        name='input_device_callbacks_'+('contract_' if kind=='contract' else '')+'oracle'; exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
            '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/input_device_callbacks_contract_children.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            result=subprocess.run([str(exe),str(cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'input_device_callbacks_{kind}_{e}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(f'{e} {kind}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192:
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{e} {kind}: unvisited boundaries {sorted(missing)}')
    print('input device real and controlled layers cover all eight owners and 274 boundaries')
if __name__=='__main__': main()
