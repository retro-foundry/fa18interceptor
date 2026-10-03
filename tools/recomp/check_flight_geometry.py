"""Compare four complete geometry owners with controlled and original children."""
import argparse,json,subprocess
from audit_flight_geometry_source import ROOT,ENTRIES,MANIFEST
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=16384)
    p.add_argument('--kind',choices=('contract','real','both'),default='both'); a=p.parse_args()
    if a.cases<=0: p.error('case count must be positive')
    subprocess.run(['python','tools/recomp/audit_flight_geometry_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind in (('contract','real') if a.kind=='both' else (a.kind,)):
        name='flight_geometry_'+('contract_' if kind=='contract' else '')+'oracle'
        exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
             '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/flight_geometry_contract_children.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/flight_geometry_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'flight_geometry_{kind}_{e}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind}: {r.stderr or r.stdout}')
            print(r.stdout.splitlines()[0],flush=True)
            owned=set(source['owners'][e]['source_pcs']); observed=set(r.stdout.split('visited:')[1].splitlines()[0].split())&owned
            print(f'{e} {kind} owner coverage {len(observed)}/{len(owned)}; missing {sorted(owned-observed)}',flush=True)
            if a.cases>=16384 and kind=='contract' and observed!=owned:
                raise RuntimeError(f'{e} controlled owner coverage incomplete')
    print('completed flight geometry match full CPU/PC/SR and all RAM; each child boundary retains complete input contracts')
if __name__=='__main__': main()
