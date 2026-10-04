"""Compare twelve complete segment projection owners with controlled and original children."""
import argparse,json,subprocess
from audit_segment_projection_source import ROOT,ENTRIES,MANIFEST
from hud_stream_machine_variant import prepare
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=16384)
    p.add_argument('--kind',choices=('contract','real','both'),default='both')
    p.add_argument('--entries',nargs='+',choices=ENTRIES,default=ENTRIES); a=p.parse_args()
    if a.cases<=0: p.error('case count must be positive')
    prepare()
    subprocess.run(['python','tools/recomp/audit_segment_projection_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    union_owned=set(); union_observed=set()
    for kind in (('contract','real') if a.kind=='both' else (a.kind,)):
        name='segment_projection_'+('contract_' if kind=='contract' else '')+'oracle'
        exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
             '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
             '--replace-source','port/machine/machine.c=tools/recomp/segment_projection_hardware_log.c',
             '--replace-source','port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/segment_projection_contract_children.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/segment_projection_bus_budget.c','--replace-source','port/game/glue/glue_child_call.c=tools/recomp/segment_projection_child_call.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in a.entries:
            r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'segment_projection_{kind}_{e}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind}: {r.stderr or r.stdout}')
            print(r.stdout.splitlines()[0],flush=True)
            owned=set(source['owners'][e]['source_pcs']); observed=set(r.stdout.split('visited:')[1].splitlines()[0].split())&owned
            print(f'{e} {kind} owner coverage {len(observed)}/{len(owned)}; missing {sorted(owned-observed)}',flush=True)
            if kind=='contract': union_owned|=owned; union_observed|=observed
    if a.cases>=16384 and a.kind in ('contract','both'):
        missing=union_owned-union_observed
        if missing:raise RuntimeError('Controlled whole coverage incomplete: '+str(sorted(missing)))
    print('completed segment projections match full CPU/PC/SR/all RAM, ordered Custom writes and terminal hardware/latches')
    if a.kind in ('contract','both'):
        print('controlled children compare complete entry CPU/SR/RAM contracts')
if __name__=='__main__': main()
