"""Prove complete game-side file callers, retaining original service failures."""
import argparse,json,subprocess
from audit_postflight_file_callers_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384); parser.add_argument('--contract-cases',type=int,default=8192)
    args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_postflight_file_callers_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text()); stops=[]
    for kind in ['contract','real']:
        name='postflight_file_callers_'+('contract_' if kind=='contract' else '')+'oracle'
        exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
            '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/postflight_file_callers_contract_children.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            cases=args.contract_cases if kind=='contract' else args.cases
            result=subprocess.run([str(exe),str(cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'postflight_file_callers_{kind}_{e}.log').write_text(result.stdout+result.stderr)
            if kind=='real' and e!='C0F56A' and result.returncode:
                if result.returncode!=1 or 'case 0 source did not return at FC0FF0' not in result.stderr: raise RuntimeError(result.stderr or result.stdout)
                stops.append({'entry':e,'returncode':result.returncode,'diagnostic':result.stderr.strip(),'completed_real_proof':False})
                print(f'{e}: {result.stderr.strip()}; no complete real-child proof counted',flush=True)
                continue
            if result.returncode: raise RuntimeError(f'{e} {kind}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192:
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{e} {kind}: unvisited boundaries {sorted(missing)}')
    if {r['entry'] for r in stops}!=set(ENTRIES)-{'C0F56A'}: raise RuntimeError('original file-service stop classification changed')
    (ROOT/'build/recomp/postflight_file_callers_service_stops.json').write_text(json.dumps(stops,indent=2)+'\n')
    print('file caller proof complete; service stops remain separately classified')
if __name__=='__main__': main()
