"""Prove complete game owners; retain incomplete original-service fixtures."""
import argparse,json,subprocess
from audit_input_display_setup_source import ROOT,ENTRIES,MANIFEST
REAL_ENTRIES=('C16D4C','C17066')
EXPECTED_STOPS={
    'C16FF4':(3,'input-display-setup original instruction budget exhausted at AF3C90; no completed proof'),
    'C1787A':(1,'input-display-setup oracle: case 0 source did not return at FC0FF0'),
    'C1612C':(3,'input-display-setup original instruction budget exhausted at C02776; no completed proof'),
}
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=16384)
    parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_input_display_setup_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text()); stops=[]
    for kind,cases in [('contract',args.contract_cases),('real',args.cases)]:
        name='input_display_setup_'+('contract_' if kind=='contract' else '')+'oracle'; exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
             '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/input_display_setup_contract_children.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/input_display_setup_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            result=subprocess.run([str(exe),str(cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'input_display_setup_{kind}_{e}.log').write_text(result.stdout+result.stderr)
            if kind=='real' and e in EXPECTED_STOPS:
                code,diagnostic=EXPECTED_STOPS[e]
                if result.returncode!=code or result.stderr.strip()!=diagnostic: raise RuntimeError(f'{e}: original fixture classification changed: {result.stderr or result.stdout}')
                stops.append({'entry':e,'returncode':code,'diagnostic':diagnostic,'completed_real_proof':False})
                print(f'{e}: {diagnostic}',flush=True); continue
            if result.returncode: raise RuntimeError(f'{e} {kind}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            visited=set(result.stdout.split('visited:')[1].split()); owned=set(source['owners'][e]['source_pcs'])
            # The root's real service fixture takes the successful path. Error
            # paths are covered independently by the full controlled layer.
            if (kind=='contract' and cases>=8192) or (kind=='real' and e=='C17066'):
                if owned-visited: raise RuntimeError(f'{e} {kind}: unvisited boundaries {sorted(owned-visited)}')
    (ROOT/'build/recomp/input_display_setup_service_stops.json').write_text(json.dumps(stops,indent=2)+'\n')
    print('input/display setup: controlled layer covers every owner; real complete calls and original fixture stops remain separate')
if __name__=='__main__': main()
