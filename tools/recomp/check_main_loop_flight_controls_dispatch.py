"""Actual control/flight ON/shadow/sandbox dispatch with strict guards."""
import argparse,subprocess
from audit_main_loop_flight_controls_source import ROOT,ENTRIES
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=256); a=p.parse_args()
    if a.cases<=0: p.error('cases must be positive')
    exe=ROOT/'build/recomp/main_loop_flight_controls_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/main_loop_flight_controls_dispatch_oracle.c',
        '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c','--replace-source','port/machine/bus.c=tools/recomp/main_loop_flight_controls_bus_budget.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        for e in ENTRIES:
            r=subprocess.run([str(exe),str(a.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'main_loop_flight_controls_dispatch_{e}_{name}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(r.stderr or r.stdout)
            print(name+': '+r.stdout.splitlines()[0],flush=True)
    print(f'control/flight actual dispatch: {len(ENTRIES)*3*a.cases} complete fixtures and all guards passed; real-path coverage remains partial')
if __name__=='__main__': main()
