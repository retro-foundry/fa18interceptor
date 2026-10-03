"""Actual timer/bounds/readout dispatch with strict reference classification."""
import argparse,json,subprocess
from audit_main_loop_timers import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=256); args=parser.parse_args()
    if args.cases<=0: parser.error('cases must be positive')
    source=json.loads(MANIFEST.read_text()); exe=ROOT/'build/recomp/main_loop_timers_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/main_loop_timers_dispatch_oracle.c','--replace-source',
        'port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c','--replace-source',
        'port/machine/bus.c=tools/recomp/main_loop_timers_bus_budget.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        for e in ENTRIES:
            result=subprocess.run([str(exe),str(args.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'main_loop_timers_dispatch_{e}_{name}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(result.stderr or result.stdout)
            print(name+': '+result.stdout.splitlines()[0],flush=True)
            if args.cases>=256 and e!='C25312':
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{name} {e}: missing boundaries {sorted(missing)}')
    print(f'main-loop timer dispatch: {len(ENTRIES)*3*args.cases} complete fixtures and entry/mode/selection/write guards passed')
if __name__=='__main__': main()
