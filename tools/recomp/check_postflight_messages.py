"""Prove complete postflight messages C with real and controlled original child boundaries."""
import argparse
import json
import subprocess
from audit_postflight_messages_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('cases must be positive')
    subprocess.run(['python','tools/recomp/audit_postflight_messages_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    logging='port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'
    executables={}
    for kind in ['real','contract']:
        name='postflight_messages_'+('contract_' if kind=='contract' else '')+'oracle'
        executable=ROOT/'build/recomp'/(name+'.exe'); executables[kind]=executable
        command=['python','scripts/build_recomp.py','--output',str(executable.relative_to(ROOT)),
            '--main','tools/recomp/'+name+'.c','--replace-source',logging]
        if kind=='contract': command+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/postflight_messages_contract_children.c']
        subprocess.run(command,cwd=ROOT,check=True)
    stops=[]
    service_entries={'C0F4D8':'FC1FC4','C11478':'FC0FF0','C114D2':'FC0FF0'}
    for kind,cases in [('contract',args.contract_cases),('real',args.cases)]:
        for entry in ENTRIES:
            result=subprocess.run([str(executables[kind]),str(cases),entry],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'postflight_messages_{kind}_{entry}.log').write_text(result.stdout+result.stderr)
            if kind=='real' and entry in service_entries:
                diagnostic=f'source did not return at {service_entries[entry]}'
                if result.returncode!=1 or diagnostic not in result.stderr: raise RuntimeError(f'{entry}: original service stop changed: {result.stderr or result.stdout}')
                stops.append({'entry':entry,'returncode':result.returncode,'diagnostic':result.stderr.strip(),'completed_real_proof':False})
                print(f'{entry}: original service stop retained; no complete real-child proof counted',flush=True)
                continue
            if result.returncode: raise RuntimeError(f'{entry} {kind}: exit {result.returncode}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192:
                visited=set(result.stdout.split('visited:')[1].split())
                expected=set(source['owners'][entry]['source_pcs'])
                if expected-visited: raise RuntimeError(f'{entry} {kind}: uncovered source boundaries {sorted(expected-visited)}')
    (ROOT/'build/recomp/postflight_messages_service_stops.json').write_text(json.dumps(stops,indent=2)+'\n')
    if min(args.cases,args.contract_cases)>=8192:
        print('controlled children: all 16 owners / 513 boundaries; real children: 13 owners / 411 boundaries; three original service stops retained')
    else: print('sampled CPU/RAM checks completed; full owner coverage not required for this sample')
if __name__=='__main__': main()
