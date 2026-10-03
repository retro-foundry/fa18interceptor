"""Compare ten complete projection/readout owners with controlled and original children."""
import argparse,json,subprocess
from audit_projection_readouts_source import ROOT,ENTRIES,MANIFEST
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=16384)
    p.add_argument('--kind',choices=('contract','real','both'),default='both'); a=p.parse_args()
    if a.cases<=0: p.error('case count must be positive')
    subprocess.run(['python','tools/recomp/audit_projection_readouts_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    union_owned=set(); union_observed=set()
    for kind in (('contract','real') if a.kind=='both' else (a.kind,)):
        name='projection_readouts_'+('contract_' if kind=='contract' else '')+'oracle'
        exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
             '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/projection_readouts_contract_children.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/projection_readouts_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'projection_readouts_{kind}_{e}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind}: {r.stderr or r.stdout}')
            print(r.stdout.splitlines()[0],flush=True)
            owned=set(source['owners'][e]['source_pcs']); observed=set(r.stdout.split('visited:')[1].splitlines()[0].split())&owned
            print(f'{e} {kind} owner coverage {len(observed)}/{len(owned)}; missing {sorted(owned-observed)}',flush=True)
            if kind=='contract': union_owned|=owned; union_observed|=observed
        # The entry bounds exclude upper-clamp inputs. Exercise the same domain
        # helpers from the original internal boundaries; these are segments,
        # not additional callable owners or whole-function coverage claims.
        for e in ('C2ECD2','C2ECE4'):
            r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'projection_readouts_{kind}_segment_{e}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind} segment: {r.stderr or r.stdout}')
            print('internal source segment '+r.stdout.splitlines()[0],flush=True)
            observed=set(r.stdout.split('visited:')[1].splitlines()[0].split())
            if kind=='contract': union_observed|=observed&union_owned
    if a.cases>=16384 and a.kind in ('contract','both') and union_owned!=union_observed:
        raise RuntimeError('controlled shared-family coverage incomplete: '+str(sorted(union_owned-union_observed)))
    print('completed projection readouts match full CPU/PC/SR and all RAM')
    if a.kind in ('contract','both'):
        print('controlled children compare complete entry CPU/SR/RAM contracts')
if __name__=='__main__': main()
