"""Prove complete control-record action bodies with distinct controlled/real coverage."""
import argparse,json,subprocess
from audit_record_control_actions_source import ROOT,ENTRIES,MANIFEST
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=16384); p.add_argument('--contract-cases',type=int,default=8192); a=p.parse_args()
    if min(a.cases,a.contract_cases)<=0: p.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_record_control_actions_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,cases in [('contract',a.contract_cases),('real',a.cases)]:
        name='record_control_actions_'+('contract_' if kind=='contract' else '')+'oracle'; exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c','--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/record_control_actions_contract_children.c'] if kind=='contract' else ['--replace-source','port/machine/bus.c=tools/recomp/record_control_actions_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            r=subprocess.run([str(exe),str(cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'record_control_actions_{kind}_{e}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind}: {r.stderr or r.stdout}')
            print(r.stdout.splitlines()[0],flush=True)
            if kind=='contract' and cases>=8192:
                missing=set(source['owners'][e]['source_pcs'])-set(r.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{e}: unvisited controlled boundaries {sorted(missing)}')
    print('record/control complete CPU/SR/RAM cases pass; controlled layer covers every owner, real-layer path coverage is partial')
if __name__=='__main__': main()
