"""Exercise actual ON/shadow/sandbox dispatch and guards for five marker owners."""
import argparse,subprocess
from audit_flight_markers_source import ROOT,ENTRIES
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=256); a=p.parse_args()
    if a.cases<=0: p.error('case count must be positive')
    exe=ROOT/'build/recomp/flight_markers_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/flight_markers_dispatch_oracle.c',
        '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
        '--replace-source','port/machine/bus.c=tools/recomp/flight_markers_bus_budget.c'],cwd=ROOT,check=True)
    for e in ENTRIES:
        for mode,name in ((1,'on'),(2,'shadow'),(3,'sandbox')):
            r=subprocess.run([str(exe),str(a.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'flight_markers_dispatch_{e}_{name}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {name}: {r.stderr or r.stdout}')
            print(name,r.stdout.splitlines()[0],flush=True)
    print('actual flight-markers dispatch, strict one-call classification and guards pass')
if __name__=='__main__': main()
