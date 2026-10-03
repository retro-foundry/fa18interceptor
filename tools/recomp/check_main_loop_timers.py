"""Complete game timer CPU/SR/RAM proof with separate controlled/real layers."""
import argparse,json,subprocess,os
from audit_main_loop_timers import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=16384); parser.add_argument('--contract-cases',type=int,default=8192); args=parser.parse_args()
    if min(args.cases,args.contract_cases)<=0: parser.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_main_loop_timers.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text())
    for kind,cases in [('contract',args.contract_cases),('real',args.cases)]:
        name='main_loop_timers_'+('contract_' if kind=='contract' else '')+'oracle'; exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c',
            '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract': cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/main_loop_timers_contract_children.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/main_loop_timers_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        for e in ENTRIES:
            result=subprocess.run([str(exe),str(cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'main_loop_timers_{kind}_{e}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(f'{e} {kind}: {result.stderr or result.stdout}')
            print(result.stdout.splitlines()[0],flush=True)
            if cases>=8192 and (kind=='contract' or e!='C25312'):
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{e} {kind}: missing boundaries {sorted(missing)}')
    result=subprocess.run([str(exe),'1','C2548A'],cwd=ROOT,capture_output=True,text=True,timeout=600,
                          env={**os.environ,'FA18_TIMER_ZERO_DIVISOR':'1'})
    (ROOT/'build/recomp/main_loop_timers_zero_divisor.log').write_text(result.stdout+result.stderr)
    diagnostic='main-loop-timers original instruction budget exhausted at FC30C2; no completed proof'
    if result.returncode!=3 or result.stderr.strip()!=diagnostic:
        raise RuntimeError(f'zero-divide exception fixture classification changed: {result.stderr or result.stdout}')
    print(diagnostic,flush=True)
    print('main-loop timers: controlled proof covers every owner; real timer poll coverage remains partial; zero-divide exception service is separate')
if __name__=='__main__': main()
