"""Prove actual gameport dispatch without counting incomplete source fixtures."""
import argparse,json,subprocess
from audit_input_display_setup_source import ROOT,MANIFEST
from check_input_display_setup import REAL_ENTRIES
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=256); args=parser.parse_args()
    if args.cases<=0: parser.error('cases must be positive')
    source=json.loads(MANIFEST.read_text()); exe=ROOT/'build/recomp/input_display_setup_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/input_display_setup_dispatch_oracle.c','--replace-source',
        'port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c','--replace-source',
        'port/machine/bus.c=tools/recomp/input_display_setup_bus_budget.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        for e in REAL_ENTRIES:
            result=subprocess.run([str(exe),str(args.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'input_display_setup_dispatch_{e}_{name}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(result.stderr or result.stdout)
            print(name+': '+result.stdout.splitlines()[0],flush=True)
            if args.cases>=256 and e=='C17066':
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{name} {e}: missing boundaries {sorted(missing)}')
    print(f'input/display dispatch: {len(REAL_ENTRIES)*3*args.cases} complete CPU/RAM fixtures and entry/mode/selection/write guards passed')
if __name__=='__main__': main()
